#include "MicrosoftAuthProvider.h"

#include "Application.h"
#include "BuildConfig.h"
#include "minecraft/auth/flows/MSA.h"

shared_qobject_ptr<AccountTask> MicrosoftAuthProvider::createRefreshTask(AccountData* data)
{
    return shared_qobject_ptr<AccountTask>(new MSASilent(data));
}

bool MicrosoftAuthProvider::isAvailable()
{
    return APPLICATION->hasMsaClientId();
}

QString MicrosoftAuthProvider::unavailableReason()
{
    return QObject::tr(
        "Microsoft login needs a client ID, and this build doesn't include one.\n\n"
        "Register your own application in Microsoft Entra and put its client ID in the \"MSAClientID\" "
        "setting of %1 (or in the ULTIMMC_MSA_CLIENT_ID environment variable), then restart the launcher.\n\n"
        "Local and Ely.by accounts work without it."
    ).arg(BuildConfig.LAUNCHER_CONFIGFILE);
}
