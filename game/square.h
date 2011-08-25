/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2011 Michael Hasselmann <michael@taschenorakel.de>
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

#ifndef SQUARE_H
#define SQUARE_H

#include "namespace.h"
#include <QtCore>

namespace Game {

//! Represents square on the board.
struct Square
{
public:
    File file;
    Rank rank;

    explicit Square(File new_file,
                    Rank new_rank);

    bool valid() const;
};

bool operator==(const Square &a,
                const Square &b);

Square toSquare(uint file,
                uint rank);

Square toSquare(int index);

Square toSquare(const QByteArray &data);

} // namespace Miniature

#endif // SQUARE_H
