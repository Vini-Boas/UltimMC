#pragma once
#include <QString>
#include <QByteArray>
#include <QVector>
#include <katabasis/Bits.h>
#include <QJsonObject>

#include "providers/BaseAuthProvider.h"

struct Skin {
    QString id;
    QString url;
    QString variant;

    QByteArray data;
};

struct Cape {
    QString id;
    QString url;
    QString alias;

    QByteArray data;
};

struct MinecraftEntitlement {
    bool ownsMinecraft = false;
    bool canPlayMinecraft = false;
    Katabasis::Validity validity = Katabasis::Validity::None;
};

struct MinecraftProfile {
    QString id;
    QString name;
    Skin skin;
    QString currentCape;
    QMap<QString, Cape> capes;
    Katabasis::Validity validity = Katabasis::Validity::None;
};

/**
 * The kinds of account we can log in with. Each one has an AuthProvider that knows how to
 * refresh it (see providers/BaseAuthProvider.h). Old account types that no longer exist
 * (Mojang) are deliberately not here: they are kept as 'unsupported' placeholders instead.
 */
enum class AccountType {
    MSA,
    Local,
    Elyby
};

enum class AccountState {
    Unchecked,
    Offline,
    Working,
    Online,
    Errored,
    Expired,
    Gone,
    MustMigrate,
    //! An account in accounts.json that this version can't use. Shown disabled, never refreshed or launched.
    Unsupported
};

struct AccountData {
    QJsonObject saveState() const;
    bool resumeStateFromV3(QJsonObject data);

    //! Sets the type and the matching auth provider together, so they can't disagree.
    void setType(AccountType newType);

    //! The name to show for the account: gamertag for MSA, user name for Ely.by, '<Local>' for local ones.
    QString gamerTag() const;

    //! The login name. Only meaningful for Ely.by and local accounts.
    QString userName() const;

    //! Identifies this launcher installation to a Yggdrasil-style auth server (Ely.by).
    QString clientToken() const;
    void setClientToken(QString clientToken);
    void generateClientTokenIfMissing();

    //! Unsupported accounts only: something recognisable to show, read from the saved entry (profile or user name).
    QString unsupportedName() const;
    //! Unsupported accounts only: the 'type' of the saved entry, e.g. "Mojang".
    QString unsupportedKind() const;

    //! Yggdrasil access token, as passed to the game.
    QString accessToken() const;

    QString profileId() const;
    QString profileName() const;

    QString xid() const;

    QString lastError() const;

    AccountType type = AccountType::MSA;
    AuthProviderPtr provider;

    //! For unsupported accounts only: the original entry from accounts.json, written back untouched on save.
    QJsonObject unsupportedEntry;

    Katabasis::Token msaToken;
    Katabasis::Token userToken;
    Katabasis::Token xboxApiToken;
    Katabasis::Token mojangservicesToken;

    Katabasis::Token yggdrasilToken;
    MinecraftProfile minecraftProfile;
    MinecraftEntitlement minecraftEntitlement;
    Katabasis::Validity validity_ = Katabasis::Validity::None;

    // runtime only information (not saved with the account)
    QString internalId;
    QString errorString;
    AccountState accountState = AccountState::Unchecked;
};
