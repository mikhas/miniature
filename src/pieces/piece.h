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

#ifndef PIECE_H__
#define PIECE_H__

namespace miniature
{

class MPiece
{
public:

    enum MPieceTypes {ROOK, KNIGHT, BISHOP, QUEEN, KING, PAWN};
    enum MColours {BLACK, WHITE};

    virtual MPiece(MColours) = 0;
    virtual MPiece(MColours, int, int) = 0;
    virtual ~MPiece();

    virtual QList<QPoint> getPossibleSquares (QPoint) const = 0;
    //virtual QList<QPoint> getInbetweenSquares (QPoint, QPoint) const = 0;
    virtual bool isMoveAllowed (QPoint, QPoint) const;

    MPieceTypes getType() const;
    MColours getColour() const;

private:
    MPieceTypes type;
    MColours colour;
    int xDim;
    int yDim;
};

}

#endif /* PIECE_H__ */
