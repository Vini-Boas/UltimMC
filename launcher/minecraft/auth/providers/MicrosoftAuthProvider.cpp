#include "MicrosoftAuthProvider.h"

#include "minecraft/auth/flows/MSA.h"

shared_qobject_ptr<AccountTask> MicrosoftAuthProvider::createRefreshTask(AccountData* data)
{
    return shared_qobject_ptr<AccountTask>(new MSASilent(data));
}
