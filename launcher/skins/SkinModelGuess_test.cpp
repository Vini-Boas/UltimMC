#include <QTest>

#include "skins/SkinModelGuess.h"

namespace {

// An opaque skin, like a classic one: every pixel is used.
QImage opaqueSkin(int height = 64)
{
    QImage skin(64, height, QImage::Format_ARGB32);
    skin.fill(qRgba(200, 100, 50, 255));
    return skin;
}

void makeTransparent(QImage& skin, int left, int right, int top, int bottom)
{
    for (int y = top; y < bottom; y++)
    {
        for (int x = left; x < right; x++)
        {
            skin.setPixel(x, y, qRgba(0, 0, 0, 0));
        }
    }
}

// The columns of the right and left arm that a slim skin leaves empty
void emptyRightArmStrip(QImage& skin) { makeTransparent(skin, 54, 56, 20, 32); }
void emptyLeftArmStrip(QImage& skin) { makeTransparent(skin, 46, 48, 52, 64); }

}

class SkinModelGuessTest : public QObject
{
    Q_OBJECT

private
slots:

    void a_fully_used_skin_is_classic()
    {
        QCOMPARE(Skins::guessSkinModel(opaqueSkin()), Skins::Model::Classic);
    }

    void a_skin_with_both_slim_strips_empty_is_slim()
    {
        auto skin = opaqueSkin();
        emptyRightArmStrip(skin);
        emptyLeftArmStrip(skin);
        QCOMPARE(Skins::guessSkinModel(skin), Skins::Model::Slim);
    }

    void one_empty_strip_is_not_enough()
    {
        // a classic skin can leave one arm's edge transparent, so both have to agree
        auto onlyRight = opaqueSkin();
        emptyRightArmStrip(onlyRight);
        QCOMPARE(Skins::guessSkinModel(onlyRight), Skins::Model::Classic);

        auto onlyLeft = opaqueSkin();
        emptyLeftArmStrip(onlyLeft);
        QCOMPARE(Skins::guessSkinModel(onlyLeft), Skins::Model::Classic);
    }

    void a_single_used_pixel_in_a_strip_makes_it_classic()
    {
        auto skin = opaqueSkin();
        emptyRightArmStrip(skin);
        emptyLeftArmStrip(skin);
        skin.setPixel(55, 31, qRgba(1, 2, 3, 255));
        QCOMPARE(Skins::guessSkinModel(skin), Skins::Model::Classic);
    }

    void old_64x32_skins_are_always_classic()
    {
        auto skin = opaqueSkin(32);
        makeTransparent(skin, 54, 56, 20, 32);
        QCOMPARE(Skins::guessSkinModel(skin), Skins::Model::Classic);
    }

    void an_image_of_the_wrong_size_is_classic()
    {
        QCOMPARE(Skins::guessSkinModel(QImage()), Skins::Model::Classic);
    }
};

QTEST_GUILESS_MAIN(SkinModelGuessTest)

#include "SkinModelGuess_test.moc"
