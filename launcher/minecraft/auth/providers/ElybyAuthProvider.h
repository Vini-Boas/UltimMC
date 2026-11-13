#pragma once

#include "BaseAuthProvider.h"

/*!
 * Ely.by accounts. Ely.by runs its own authentication server (the same protocol the old Mojang
 * accounts used), and the game is pointed at it with the authlib injector.
 */
class ElybyAuthProvider : public BaseAuthProvider
{
    Q_OBJECT

public:
    QString id() override
    {
        return "elyby";
    }

    QString displayName() override
    {
        return "Ely.by";
    };

    QString injectorEndpoint() override
    {
        return "ely.by";
    };

    QString authEndpoint() override
    {
        return "https://authserver.ely.by/auth/";
    };

    QUrl resolveSkinUrl(QString id, QString name) override
    {
        return QUrl(((QString) "https://skinsystem.ely.by/skins/%1.png").arg(QString::fromLatin1(QUrl::toPercentEncoding(name))));
    }

    QString skinHint() override
    {
        // Ely.by has no public way for a launcher to change skins
        return QObject::tr("Skins for Ely.by accounts are changed on <a href=\"https://ely.by\">ely.by</a>.");
    }

    shared_qobject_ptr<AccountTask> createRefreshTask(AccountData* data) override;
};
