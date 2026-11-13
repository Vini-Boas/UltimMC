#include "SkinModelGuess.h"

#include "TextureMappings.h"

namespace Skins {

namespace {

/*
 * The back of a classic arm is 4 pixels wide, the back of a slim one 3. The pixels to the right of the
 * slim back, up to the end of the classic one, are only used by classic arms. The numbers come from the
 * texture mappings, so this follows them if they ever change.
 */
bool unusedBySlimArmIsEmpty(const QImage& skin, const TextureMapping& classic, const TextureMapping& slim)
{
    const int left = int(slim.back.x + slim.back.w);
    const int right = int(classic.back.x + classic.back.w);
    const int top = int(classic.back.y);
    const int bottom = int(classic.back.y + classic.back.h);
    for (int y = top; y < bottom; y++)
    {
        for (int x = left; x < right; x++)
        {
            if (qAlpha(skin.pixel(x, y)) != 0)
            {
                return false;
            }
        }
    }
    return true;
}

}

Model guessSkinModel(const QImage& skin)
{
    if (skin.width() != 64 || skin.height() != 64)
    {
        return Model::Classic;
    }
    const bool slim = unusedBySlimArmIsEmpty(skin, right_arm_classic, right_arm_slim)
        && unusedBySlimArmIsEmpty(skin, left_arm_classic, left_arm_slim);
    return slim ? Model::Slim : Model::Classic;
}

}
