#include "ElybyAuthStep.h"

#include <QDateTime>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QUrl>

#include "minecraft/auth/AccountData.h"
#include "minecraft/auth/AuthRequest.h"

ElybyAuthStep::ElybyAuthStep(AccountData* data, QString password) : AuthStep(data), m_password(password) {}

ElybyAuthStep::~ElybyAuthStep() noexcept = default;

QString ElybyAuthStep::describe() {
    if (m_password.isEmpty()) {
        return tr("Refreshing the Ely.by session.");
    }
    return tr("Logging in with an Ely.by account.");
}

void ElybyAuthStep::perform() {
    if (!m_data->provider) {
        emit finished(AccountTaskState::STATE_FAILED_HARD, tr("This Ely.by account has no auth provider."));
        return;
    }

    QJsonObject req;
    QString call;
    if (m_password.isEmpty()) {
        call = "refresh";
        req.insert("accessToken", m_data->accessToken());
    } else {
        call = "authenticate";
        QJsonObject agent;
        agent.insert("name", QString("Minecraft"));
        agent.insert("version", 1);
        req.insert("agent", agent);
        req.insert("username", m_data->userName());
        req.insert("password", m_password);
        m_data->generateClientTokenIfMissing();
    }
    req.insert("clientToken", m_data->clientToken());
    req.insert("requestUser", false);

    QNetworkRequest request(QUrl(m_data->provider->authEndpoint() + call));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    auto requestor = new AuthRequest(this);
    connect(requestor, &AuthRequest::finished, this, &ElybyAuthStep::onRequestDone);
    requestor->post(request, QJsonDocument(req).toJson());
}

void ElybyAuthStep::onRequestDone(
    QNetworkReply::NetworkError error,
    QByteArray data,
    QList<QNetworkReply::RawHeaderPair> headers
) {
    auto requestor = qobject_cast<AuthRequest*>(QObject::sender());
    requestor->deleteLater();

    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &jsonError);
    const bool haveJson = (jsonError.error == QJsonParseError::NoError) && doc.isObject();

    if (error != QNetworkReply::NoError) {
        qWarning() << "Ely.by authentication request failed:" << requestor->errorString_
                   << "HTTP status" << requestor->httpStatus_;
        const int status = requestor->httpStatus_;
        if (status >= 400 && status < 500) {
            // The server answered and said no (wrong password, expired token, ...).
            // It explains why in a JSON body: {"error": "...", "errorMessage": "..."}
            QString message = tr("Ely.by rejected the login.");
            if (haveJson) {
                auto serverMessage = doc.object().value("errorMessage").toString();
                if (!serverMessage.isEmpty()) {
                    message = serverMessage;
                }
            }
            emit finished(AccountTaskState::STATE_FAILED_HARD, message);
        } else {
            // No usable answer: no network, timeout, server trouble. Not the account's fault.
            // NOTE: a soft failure in the first step means 'offline'.
            emit finished(
                AccountTaskState::STATE_OFFLINE,
                tr("Could not reach the Ely.by authentication server: %1").arg(requestor->errorString_)
            );
        }
        return;
    }

    if (!haveJson) {
        emit finished(AccountTaskState::STATE_FAILED_SOFT, tr("The Ely.by authentication server sent an unreadable response."));
        return;
    }
    const QJsonObject response = doc.object();

    const QString clientToken = response.value("clientToken").toString();
    if (clientToken.isEmpty()) {
        emit finished(AccountTaskState::STATE_FAILED_HARD, tr("Authentication server didn't send a client token."));
        return;
    }
    if (m_data->clientToken().isEmpty()) {
        m_data->setClientToken(clientToken);
    } else if (clientToken != m_data->clientToken()) {
        emit finished(
            AccountTaskState::STATE_FAILED_HARD,
            tr("Authentication server attempted to change the client token. This isn't supported.")
        );
        return;
    }

    const QString accessToken = response.value("accessToken").toString();
    if (accessToken.isEmpty()) {
        emit finished(AccountTaskState::STATE_FAILED_HARD, tr("Authentication server didn't send an access token."));
        return;
    }

    m_data->yggdrasilToken.token = accessToken;
    m_data->yggdrasilToken.validity = Katabasis::Validity::Certain;
    m_data->yggdrasilToken.issueInstant = QDateTime::currentDateTimeUtc();

    emit finished(AccountTaskState::STATE_WORKING, tr("Logged in with Ely.by."));
}
