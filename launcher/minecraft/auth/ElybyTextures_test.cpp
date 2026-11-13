#include <QTest>

#include "minecraft/auth/ElybyTextures.h"

namespace {

// Responses of GET /textures/<name>. They are named here, not written inside the QVERIFY macros,
// because moc can't parse a raw string literal in a macro call.
const char* classicSkin = R"({"SKIN":{"url":"http://ely.by/storage/skins/69c6740d.png"}})"; // as the live service sends it
const char* slimSkin = R"({"SKIN":{"url":"http://example.com/s.png","metadata":{"model":"slim"}}})";
const char* otherModel = R"({"SKIN":{"url":"http://example.com/s.png","metadata":{"model":"default"}}})";
const char* capeOnly = R"({"CAPE":{"url":"http://example.com/c.png"}})";
const char* skinWithoutAddress = R"({"SKIN":{}})";
const char* notJson = "<html>Bad Gateway</html>";

}

class ElybyTexturesTest : public QObject
{
    Q_OBJECT

private
slots:

    void a_classic_skin_has_no_metadata()
    {
        ElybyTextures textures;
        QVERIFY(parseElybyTextures(classicSkin, textures));
        QVERIFY(textures.hasSkin);
        QCOMPARE(textures.skinUrl, QString("http://ely.by/storage/skins/69c6740d.png"));
        QVERIFY(!textures.slim);
    }

    void a_slim_skin_says_so_in_its_metadata()
    {
        ElybyTextures textures;
        QVERIFY(parseElybyTextures(slimSkin, textures));
        QVERIFY(textures.hasSkin);
        QVERIFY(textures.slim);
    }

    void metadata_that_isnt_slim_is_classic()
    {
        ElybyTextures textures;
        QVERIFY(parseElybyTextures(otherModel, textures));
        QVERIFY(!textures.slim);
    }

    void a_cape_without_a_skin_means_no_skin()
    {
        ElybyTextures textures;
        QVERIFY(parseElybyTextures(capeOnly, textures));
        QVERIFY(!textures.hasSkin);
    }

    void an_empty_response_means_no_textures()
    {
        // HTTP 204 has no body
        ElybyTextures textures;
        textures.hasSkin = true; // left over from earlier, must be reset
        QVERIFY(parseElybyTextures(QByteArray(), textures));
        QVERIFY(!textures.hasSkin);
        QVERIFY(textures.skinUrl.isEmpty());
    }

    void a_response_that_is_not_json_is_rejected()
    {
        ElybyTextures textures;
        QVERIFY(!parseElybyTextures(notJson, textures));
        QVERIFY(!textures.hasSkin);
    }

    void a_skin_without_an_address_is_rejected()
    {
        ElybyTextures textures;
        QVERIFY(!parseElybyTextures(skinWithoutAddress, textures));
        QVERIFY(!textures.hasSkin);
    }
};

QTEST_GUILESS_MAIN(ElybyTexturesTest)

#include "ElybyTextures_test.moc"
