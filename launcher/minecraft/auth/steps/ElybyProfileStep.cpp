#include "ElybyProfileStep.h"

#include <QNetworkRequest>

#include "minecraft/auth/AuthRequest.h"
#include "minecraft/auth/ElybyTextures.h"
#include "minecraft/auth/Parsers.h"
#include "skins/SkinUtils.h"

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

    // Parsing the profile starts from a clean one, but the skin we already have is still good
    const Skin previousSkin = m_data->minecraftProfile.skin;
    if(!Parsers::parseMinecraftProfile(data, m_data->minecraftProfile)) {
        m_data->minecraftProfile = MinecraftProfile();
        emit finished(
            AccountTaskState::STATE_FAILED_SOFT,
            tr("Minecraft Java profile response could not be parsed")
        );
        return;
    }
    m_data->minecraftProfile.skin = previousSkin;

    fetchSkin();
}

void ElybyProfileStep::fetchSkin() {
    const QUrl url = m_data->provider ? m_data->provider->texturesUrl(m_data->minecraftProfile.name) : QUrl();
    if (url.isEmpty() || !url.isValid()) {
        finishSkin();
        return;
    }

    AuthRequest *requestor = new AuthRequest(this);
    connect(requestor, &AuthRequest::finished, this, &ElybyProfileStep::onTexturesDone);
    requestor->get(QNetworkRequest(url));
}

void ElybyProfileStep::onTexturesDone(
    QNetworkReply::NetworkError error,
    QByteArray data,
    QList<QNetworkReply::RawHeaderPair> headers
) {
    auto requestor = qobject_cast<AuthRequest *>(QObject::sender());
    requestor->deleteLater();

    ElybyTextures textures;
    if (error == QNetworkReply::ContentNotFoundError) {
        // the player has no textures. 'textures' stays empty.
    } else if (error != QNetworkReply::NoError) {
        // No network, server trouble... keep the skin we already have.
        qWarning() << "Ely.by textures: request failed:" << requestor->errorString_;
        finishSkin();
        return;
    } else if (!parseElybyTextures(data, textures)) {
        qWarning() << "Ely.by textures: couldn't understand the response (HTTP status" << requestor->httpStatus_ << ")";
        finishSkin();
        return;
    }

    if (!textures.hasSkin) {
        // Ely.by says the player has no skin, so forget any old one
        m_data->minecraftProfile.skin = Skin();
        finishSkin();
        return;
    }

    // Ely.by only says so when the skin is slim. No metadata means classic.
    m_data->minecraftProfile.skin.url = textures.skinUrl;
    m_data->minecraftProfile.skin.variant = textures.slim ? "SLIM" : "CLASSIC";

    AuthRequest *skinRequestor = new AuthRequest(this);
    connect(skinRequestor, &AuthRequest::finished, this, &ElybyProfileStep::onSkinDone);
    skinRequestor->get(QNetworkRequest(QUrl(textures.skinUrl)));
}

void ElybyProfileStep::onSkinDone(
    QNetworkReply::NetworkError error,
    QByteArray data,
    QList<QNetworkReply::RawHeaderPair> headers
) {
    auto requestor = qobject_cast<AuthRequest *>(QObject::sender());
    requestor->deleteLater();

    if (error == QNetworkReply::NoError) {
        // Only keep what really is a skin. Anything else (an error page, a redirect) would later look
        // like a skin we can't draw.
        QImage image;
        QString textureId;
        if (Skins::readSkinFromData(data, image, textureId)) {
            m_data->minecraftProfile.skin.data = data;
        } else {
            qWarning() << "Ely.by skin: the response isn't a skin picture (HTTP status" << requestor->httpStatus_ << ")";
        }
    } else if (error == QNetworkReply::ContentNotFoundError) {
        m_data->minecraftProfile.skin.data.clear();
    } else {
        qWarning() << "Ely.by skin: download failed:" << requestor->errorString_;
    }

    finishSkin();
}

void ElybyProfileStep::finishSkin() {
    emit finished(
        AccountTaskState::STATE_WORKING,
        tr("Minecraft Java profile acquisition succeeded.")
    );
}
