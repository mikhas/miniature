/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2009 Michael Hasselmann <michael@taschenorakel.de>
 *
 *
 * Miniature is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Miniature is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Miniature. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PLAYER_INFO_H__
#define PLAYER_INFO_H__

#include <QString>

namespace Miniature
{

// TODO: rename to mGameInfo
struct MPlayerInfo
{
    QString white_name;
    QString white_rating;
    int white_material;

    QString black_name;
    QString black_rating;
    int black_material;

    int half_move;
    bool has_white_moved;
    QString notation;
    QString full_notation;
};

};

#endif
