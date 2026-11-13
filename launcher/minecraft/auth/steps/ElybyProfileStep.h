#pragma once
#include <QObject>

#include "QObjectPtr.h"
#include "minecraft/auth/AuthStep.h"


class ElybyProfileStep : public AuthStep {
    Q_OBJECT

public:
    explicit ElybyProfileStep(AccountData *data);
    virtual ~ElybyProfileStep() noexcept;

    void perform() override;

    QString describe() override;

private slots:
    void onRequestDone(QNetworkReply::NetworkError, QByteArray, QList<QNetworkReply::RawHeaderPair>);
    void onTexturesDone(QNetworkReply::NetworkError, QByteArray, QList<QNetworkReply::RawHeaderPair>);
    void onSkinDone(QNetworkReply::NetworkError, QByteArray, QList<QNetworkReply::RawHeaderPair>);

private:
    /*!
     * The profile lookup doesn't include skins. Ely.by's skin system does, in two steps:
     * ask it where the skin is (and which model it is for), then download the picture.
     * Neither can fail the account: it works fine without a skin.
     */
    void fetchSkin();
    void finishSkin();
};
