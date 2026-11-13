#pragma once

#include "BaseAuthProvider.h"

/*!
 * Microsoft accounts. This is upstream MultiMC's own login, so the provider is a thin wrapper:
 * the flows (MSASilent, MSAInteractive, ...) are the unmodified upstream ones.
 */
class MicrosoftAuthProvider : public BaseAuthProvider
{
    Q_OBJECT

public:
    QString id() override
    {
        return "MSA";
    }

    QString displayName() override
    {
        return "Microsoft";
    }

    bool canChangeSkin() override
    {
        return true;
    }

    //! Microsoft login needs a client ID, and this fork doesn't ship one (see Application::msaClientId)
    bool isAvailable() override;
    QString unavailableReason() override;

    shared_qobject_ptr<AccountTask> createRefreshTask(AccountData* data) override;
};
