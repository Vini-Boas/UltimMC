#include <QTest>

#include <QJsonDocument>
#include <QJsonObject>

#include "minecraft/auth/AccountList.h"
#include "minecraft/auth/MinecraftAccount.h"

namespace {

// What an account saved by the last versions that still had Mojang accounts looks like in accounts.json
QJsonObject oldMojangEntry()
{
    const char* json = R"json({
        "type": "Mojang",
        "profile": { "id": "abc", "name": "OldSteve" },
        "ygg": { "token": "t", "extra": { "userName": "OldSteve", "clientToken": "c" } }
    })json";
    return QJsonDocument::fromJson(json).object();
}

MinecraftAccountPtr microsoftAccount(const QString& xid)
{
    auto account = MinecraftAccount::createBlankMSA();
    account->accountData()->xboxApiToken.extra["xid"] = xid;
    return account;
}

}

class AccountListTest : public QObject
{
    Q_OBJECT

private
slots:

    // ---- which accounts count as "the same account" when adding one ----

    void local_accounts_with_different_names_are_both_kept()
    {
        // Regression: local accounts have no Xbox ID, and an empty ID used to match the first
        // such account, so adding a second one silently replaced the first.
        AccountList list;
        list.addAccount(MinecraftAccount::createLocal("Alex"));
        list.addAccount(MinecraftAccount::createLocal("Steve"));
        QCOMPARE(list.count(), 2);
    }

    void adding_the_same_local_name_again_replaces_it()
    {
        AccountList list;
        list.addAccount(MinecraftAccount::createLocal("Alex"));
        list.addAccount(MinecraftAccount::createLocal("Alex"));
        QCOMPARE(list.count(), 1);
    }

    void elyby_accounts_are_matched_by_login_name_too()
    {
        AccountList list;
        list.addAccount(MinecraftAccount::createElyby("Alex"));
        list.addAccount(MinecraftAccount::createElyby("Steve"));
        QCOMPARE(list.count(), 2);
        list.addAccount(MinecraftAccount::createElyby("Steve"));
        QCOMPARE(list.count(), 2);
    }

    void a_local_and_an_elyby_account_with_the_same_name_are_different_accounts()
    {
        AccountList list;
        list.addAccount(MinecraftAccount::createLocal("Alex"));
        list.addAccount(MinecraftAccount::createElyby("Alex"));
        QCOMPARE(list.count(), 2);
    }

    void microsoft_accounts_are_matched_by_xbox_id()
    {
        AccountList list;
        list.addAccount(microsoftAccount("1"));
        list.addAccount(microsoftAccount("2"));
        QCOMPARE(list.count(), 2);
        list.addAccount(microsoftAccount("1"));
        QCOMPARE(list.count(), 2);
    }

    void other_kinds_of_accounts_never_replace_a_microsoft_one()
    {
        AccountList list;
        list.addAccount(microsoftAccount("1"));
        list.addAccount(MinecraftAccount::createLocal("Alex"));
        list.addAccount(MinecraftAccount::createElyby("Alex"));
        QCOMPARE(list.count(), 3);
    }

    void an_empty_xbox_id_matches_nothing()
    {
        AccountList list;
        list.addAccount(MinecraftAccount::createLocal("Alex"));
        MinecraftAccountPtr found;
        int index = -1;
        QVERIFY(!list.getAccountByXID(QString(), found, index));
        QCOMPARE(index, -1);
    }

    // ---- accounts this version can't use are kept, not dropped ----

    void an_old_mojang_entry_loads_as_an_unsupported_account()
    {
        auto account = MinecraftAccount::loadFromJsonV3(oldMojangEntry());
        QVERIFY(account);
        QVERIFY(!account->isSupported());
        QCOMPARE(account->unsupportedName(), QString("OldSteve"));
        QCOMPARE(account->unsupportedKind(), QString("Mojang"));
    }

    void an_empty_entry_is_not_worth_keeping()
    {
        QVERIFY(!MinecraftAccount::loadFromJsonV3(QJsonObject()));
    }

    void an_unsupported_entry_is_saved_back_untouched()
    {
        const auto entry = oldMojangEntry();
        auto account = MinecraftAccount::loadFromJsonV3(entry);
        QVERIFY(account->saveToJson() == entry);
    }

    void an_unsupported_account_survives_other_accounts_being_added()
    {
        // Regression: the placeholder has no Xbox ID either, so adding an account used to be able to overwrite it.
        const auto entry = oldMojangEntry();
        auto placeholder = MinecraftAccount::loadFromJsonV3(entry);

        AccountList list;
        list.addAccount(placeholder);
        list.addAccount(MinecraftAccount::createLocal("Alex"));
        list.addAccount(MinecraftAccount::createElyby("Alex"));
        list.addAccount(microsoftAccount("1"));

        QCOMPARE(list.count(), 4);
        QVERIFY(list.at(0).account == placeholder);
        QVERIFY(placeholder->saveToJson() == entry);
    }

    void an_unsupported_account_is_disabled_and_cannot_be_the_default()
    {
        auto placeholder = MinecraftAccount::loadFromJsonV3(oldMojangEntry());
        AccountList list;
        list.addAccount(placeholder);

        const auto index = list.index(0);
        QVERIFY(!(list.flags(index) & Qt::ItemIsEnabled));
        QVERIFY(!(list.flags(index) & Qt::ItemIsUserCheckable));
        QVERIFY(list.flags(index) & Qt::ItemIsSelectable); // so it can still be selected and removed

        list.setDefaultAccount(placeholder);
        QVERIFY(!list.defaultAccount());
        QVERIFY(!list.setData(index, Qt::Checked, Qt::CheckStateRole));
        QVERIFY(!list.defaultAccount());
    }

    void an_unsupported_account_is_never_refreshed()
    {
        auto placeholder = MinecraftAccount::loadFromJsonV3(oldMojangEntry());
        QVERIFY(!placeholder->shouldRefresh());
        QVERIFY(!placeholder->refresh());
    }

    // ---- supported accounts survive a save and a load ----

    void local_and_elyby_accounts_round_trip_through_json()
    {
        auto local = MinecraftAccount::loadFromJsonV3(MinecraftAccount::createLocal("Alex")->saveToJson());
        QVERIFY(local);
        QVERIFY(local->isSupported());
        QCOMPARE(local->typeString(), QString("local"));
        QCOMPARE(local->userName(), QString("Alex"));

        auto elyby = MinecraftAccount::loadFromJsonV3(MinecraftAccount::createElyby("Steve")->saveToJson());
        QVERIFY(elyby);
        QVERIFY(elyby->isSupported());
        QCOMPARE(elyby->typeString(), QString("elyby"));
        QCOMPARE(elyby->userName(), QString("Steve"));
    }
};

QTEST_GUILESS_MAIN(AccountListTest)

#include "AccountList_test.moc"
