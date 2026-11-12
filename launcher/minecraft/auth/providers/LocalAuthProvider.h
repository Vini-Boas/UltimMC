#pragma once

#include "BaseAuthProvider.h"

/*!
 * Local (offline) accounts: any user name, no authentication. The game still talks to an
 * auth server through the authlib injector, so the launcher runs a tiny one on 127.0.0.1 (AuthServer).
 */
class LocalAuthProvider : public BaseAuthProvider
{
    Q_OBJECT

public:
    QString id() override
    {
        return "local";
    }

    QString displayName() override
    {
        return "Local";
    }

    QString injectorEndpoint() override
    {
        return ((QString) "http://127.0.0.1:%1").arg(m_authServer->port());
    };

    QString authEndpoint() override
    {
        return ((QString) "http://127.0.0.1:%1/auth/").arg(m_authServer->port());
    };

    shared_qobject_ptr<AccountTask> createRefreshTask(AccountData* data) override;
};
