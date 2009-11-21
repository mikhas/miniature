/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2009 Dennis Stötzel <kore@meeQ.de>
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

#ifndef ROOK_H__
#define ROOK_H__

#include "piece.h"

namespace Miniature
{

class MRook
: public MPiece
{
public:
    MRook(MColour colour, int width = 8, int height = 8);
    ~MRook();

    virtual QList<QPoint> getPossibleSquares(QPoint) const;

    void hasMoved();
    bool canCastle() const;

private:
    bool castle;
};

}

#endif /* ROOK_H__ */
