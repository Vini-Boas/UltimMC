#pragma once

#include <QImage>

#include "SkinTypes.h"

namespace Skins {

/*!
 * Works out whether a skin image is for the classic (4 pixel wide arms) or the slim (3 pixel wide arms) model.
 *
 * Skins that don't come with that information, such as the ones from Ely.by, only give us the picture.
 * A slim arm is one pixel narrower than a classic one, so the columns the classic arm would use but the
 * slim one doesn't stay empty (transparent) in a slim skin. Both arms are checked.
 *
 * Old 64x32 skins predate slim arms, so they are always classic.
 */
Model guessSkinModel(const QImage& skin);

}
