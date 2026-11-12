#pragma once

#include <QObject>
#include <QNetworkReply>
#include "QObjectPtr.h"

#include "minecraft/auth/AuthStep.h"

/*!
 * Logs in to, or refreshes the session of, an Ely.by account.
 *
 * Ely.by runs its own authentication server (https://authserver.ely.by/auth/), which speaks
 * the same small JSON protocol Mojang's old servers did. We only need two of its calls:
 *  - 'authenticate': user name + password -> access token (used when a password is given)
 *  - 'refresh':      old access token     -> new access token (used when it isn't)
 *
 * This deliberately knows nothing about Mojang accounts: those are gone.
 */
class ElybyAuthStep : public AuthStep {
    Q_OBJECT

public:
    explicit ElybyAuthStep(AccountData* data, QString password);
    virtual ~ElybyAuthStep() noexcept;

    void perform() override;
    QString describe() override;

private slots:
    void onRequestDone(QNetworkReply::NetworkError, QByteArray, QList<QNetworkReply::RawHeaderPair>);

private:
    //! empty -> refresh the existing session, otherwise log in with it
    QString m_password;
};
