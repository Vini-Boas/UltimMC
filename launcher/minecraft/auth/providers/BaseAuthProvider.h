#pragma once

#include <QObject>
#include <QString>
#include <QUrl>
#include <memory>
#include "QObjectPtr.h"

#include "../../../AuthServer.h"

class AccountTask;
struct AccountData;
class BaseAuthProvider;

typedef std::shared_ptr<BaseAuthProvider> AuthProviderPtr;

/*!
 * \brief Everything that differs between kinds of accounts lives behind this interface.
 *
 * A provider answers "how does an account of my kind authenticate?" so the rest of the
 * launcher never has to branch on the account type. There is one provider per account
 * type (see AccountType in AccountData.h): Microsoft, Local and Ely.by.
 *
 * To add a new kind of account:
 *  - add a value to AccountType and handle it in AccountData::setType, ::saveState and ::resumeStateFromV3,
 *  - subclass this class and implement the pure virtual functions,
 *  - register the subclass in AuthProviders.cpp.
 */
class BaseAuthProvider : public QObject
{
    Q_OBJECT

public:
    virtual ~BaseAuthProvider(){};

    //! Unique id of the provider. Used to look it up in AuthProviders.
    virtual QString id() = 0;

    //! Name shown in the launcher's account list.
    virtual QString displayName() = 0;

    /*!
     * Creates the task that refreshes (re-validates) an account of this kind.
     * The task works on the given data, which stays owned by the account.
     */
    virtual shared_qobject_ptr<AccountTask> createRefreshTask(AccountData* data) = 0;

    //! Whether accounts of this kind can authenticate right now. Microsoft needs a client ID, for example.
    virtual bool isAvailable()
    {
        return true;
    }

    //! Why isAvailable() is false, worded so the user knows what to do about it.
    virtual QString unavailableReason()
    {
        return QString();
    }

    //! Server for the authlib injector to talk to. Empty if the game needs no injector (vanilla MSA).
    virtual QString injectorEndpoint()
    {
        return "";
    };

    //! Endpoint of the authentication server, for providers that talk to one.
    virtual QString authEndpoint()
    {
        return "";
    };

    //! Url of the skin to display in the launcher
    virtual QUrl resolveSkinUrl(QString id, QString name)
    {
        return QUrl(((QString) "https://crafatar.com/skins/%1.png").arg(id));
    };

    //! Whether the launcher can change the skin of these accounts (only Microsoft ones)
    virtual bool canChangeSkin()
    {
        return false;
    }

    bool setAuthServer(std::shared_ptr<AuthServer> authServer)
    {
        m_authServer = authServer;
        return true;
    }

protected:
    std::shared_ptr<AuthServer> m_authServer;
};
