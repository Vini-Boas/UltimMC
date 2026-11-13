#include "LocalAuthProvider.h"

#include "minecraft/auth/flows/Local.h"

shared_qobject_ptr<AccountTask> LocalAuthProvider::createRefreshTask(AccountData* data)
{
    return shared_qobject_ptr<AccountTask>(new LocalRefresh(data));
}
