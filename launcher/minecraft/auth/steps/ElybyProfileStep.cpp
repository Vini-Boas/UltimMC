#include "ElybyProfileStep.h"

#include <QNetworkRequest>

#include "minecraft/auth/AuthRequest.h"
#include "minecraft/auth/Parsers.h"

ElybyProfileStep::ElybyProfileStep(AccountData* data) : AuthStep(data) {

}

ElybyProfileStep::~ElybyProfileStep() noexcept = default;

QString ElybyProfileStep::describe() {
    return tr("Fetching the Ely.by profile.");
}


void ElybyProfileStep::perform() {
    auto url = QUrl(QString("https://authserver.ely.by/api/users/profiles/minecraft/%1").arg(m_data->userName()).toUtf8());
    QNetworkRequest request = QNetworkRequest(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    AuthRequest *requestor = new AuthRequest(this);
    connect(requestor, &AuthRequest::finished, this, &ElybyProfileStep::onRequestDone);
    requestor->get(request);
}

void ElybyProfileStep::onRequestDone(
    QNetworkReply::NetworkError error,
    QByteArray data,
    QList<QNetworkReply::RawHeaderPair> headers
) {
    auto requestor = qobject_cast<AuthRequest *>(QObject::sender());
    requestor->deleteLater();

#ifndef NDEBUG
    qDebug() << data;
#endif
    if (error != QNetworkReply::NoError) {
        qWarning() << "Error getting profile:";
        qWarning() << " HTTP Status:        " << requestor->httpStatus_;
        qWarning() << " Internal error no.: " << error;
        qWarning() << " Error string:       " << requestor->errorString_;

        qWarning() << " Response:";
        qWarning() << QString::fromUtf8(data);

        emit finished(
            AccountTaskState::STATE_FAILED_SOFT,
            tr("Minecraft Java profile acquisition failed.")
        );
        return;
    }
    if(!Parsers::parseMinecraftProfile(data, m_data->minecraftProfile)) {
        m_data->minecraftProfile = MinecraftProfile();
        emit finished(
            AccountTaskState::STATE_FAILED_SOFT,
            tr("Minecraft Java profile response could not be parsed")
        );
        return;
    }

    fetchSkin();
}

void ElybyProfileStep::fetchSkin() {
    if (!m_data->provider) {
        emit finished(AccountTaskState::STATE_WORKING, tr("Minecraft Java profile acquisition succeeded."));
        return;
    }

    auto url = m_data->provider->resolveSkinUrl(m_data->minecraftProfile.id, m_data->minecraftProfile.name);
    m_data->minecraftProfile.skin.url = url.toString();
    if (m_data->minecraftProfile.skin.variant.isEmpty()) {
        // Ely.by's skin file doesn't say which arm model it is for. Only the preview depends on this.
        m_data->minecraftProfile.skin.variant = "CLASSIC";
    }

    AuthRequest *requestor = new AuthRequest(this);
    connect(requestor, &AuthRequest::finished, this, &ElybyProfileStep::onSkinDone);
    requestor->get(QNetworkRequest(url));
}

void ElybyProfileStep::onSkinDone(
    QNetworkReply::NetworkError error,
    QByteArray data,
    QList<QNetworkReply::RawHeaderPair> headers
) {
    auto requestor = qobject_cast<AuthRequest *>(QObject::sender());
    requestor->deleteLater();

    // Not having a skin, or not being able to fetch it right now, never fails the account:
    // it works fine without one. 404 means the player has no skin, so forget any old one.
    if (error == QNetworkReply::NoError && !data.isEmpty()) {
        m_data->minecraftProfile.skin.data = data;
    } else if (error == QNetworkReply::ContentNotFoundError) {
        m_data->minecraftProfile.skin.data.clear();
    }

    emit finished(
        AccountTaskState::STATE_WORKING,
        tr("Minecraft Java profile acquisition succeeded.")
    );
}
