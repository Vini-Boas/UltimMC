#include "ElybyAuthProvider.h"

#include "minecraft/auth/flows/Elyby.h"

shared_qobject_ptr<AccountTask> ElybyAuthProvider::createRefreshTask(AccountData* data)
{
    return shared_qobject_ptr<AccountTask>(new ElybyRefresh(data));
}
