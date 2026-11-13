#include "ElybyTextures.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

bool parseElybyTextures(const QByteArray& response, ElybyTextures& out)
{
    out = ElybyTextures();

    if (response.trimmed().isEmpty())
    {
        // HTTP 204: no textures
        return true;
    }

    QJsonParseError error;
    const QJsonDocument document = QJsonDocument::fromJson(response, &error);
    if (error.error != QJsonParseError::NoError || !document.isObject())
    {
        return false;
    }

    const QJsonValue skinValue = document.object().value("SKIN");
    if (!skinValue.isObject())
    {
        // no skin (there may still be a cape)
        return true;
    }

    const QJsonObject skin = skinValue.toObject();
    const QString url = skin.value("url").toString();
    if (url.isEmpty())
    {
        return false;
    }

    out.hasSkin = true;
    out.skinUrl = url;
    out.slim = skin.value("metadata").toObject().value("model").toString().compare("slim", Qt::CaseInsensitive) == 0;
    return true;
}
