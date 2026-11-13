#pragma once

#include <QByteArray>
#include <QString>

//! What Ely.by's skin system says about a player's skin
struct ElybyTextures {
    //! False if the player has no skin
    bool hasSkin = false;
    QString skinUrl;
    //! The skin is for the slim (3 pixel wide arms) model. Ely.by only says so when it is slim.
    bool slim = false;
};

/*!
 * Reads the response of Ely.by's `GET /textures/<name>`:
 *
 *   {"SKIN": {"url": "...", "metadata": {"model": "slim"}}, "CAPE": {"url": "..."}}
 *
 * "metadata" is only there for slim skins, and SKIN and CAPE are left out when the player doesn't have one.
 * An empty response (HTTP 204) means the player has no textures at all.
 *
 * Returns false if the response can't be understood. "No skin" is not a failure: it returns true with
 * hasSkin set to false.
 */
bool parseElybyTextures(const QByteArray& response, ElybyTextures& out);
