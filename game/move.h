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

#ifndef MOVE_H
#define MOVE_H

#include "position.h"
#include "square.h"

#include <QtCore>

namespace Game {

//! Represents a move, POD struct.
struct Move
{
public:
    const Position result; //!< The resulting position.
    const Square origin; //!< The original square of the moved piece.
    const Square target; //!< The target square of the moved piece.
    const QString notation; //!< A human-readable notation of the move.

    explicit Move(const Position &new_result,
                  const Square &new_origin,
                  const Square &new_target,
                  const QString &new_notation);

    //! Construct move with start position.
    explicit Move();
};

} // namespace Game

#endif // MOVE_H
