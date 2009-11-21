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

#include "piece.h"
#include "rook.h"

namespace Miniature
{

MPiece::MPiece(MColour colour, MType pieceType, int width, int height)
: colour(colour),
  type(pieceType),
  xDim(width),
  yDim(height)
{}

MPiece::~MPiece()
{}


MPiece::MColour MPiece::getColour() const
{
    return colour;
}

MPiece::MType MPiece::getType() const
{
    return type;
}

MPiece* MPiece::createPiece(MPiece::MType type, MPiece::MColour colour, int width, int height)
{
    MPiece* piece = 0;

    switch(type)
    {
        case ROOK:   piece = new MRook(colour, width, height); break;
/*
        case KNIGHT: piece = new MKnight(colour, width, height); break;
        case BISHOP: piece = new MBishop(colour, width, height); break;
        case QUEEN:  piece = new MQueen(colour, width, height); break;
        case KING:   piece = new MKing(colour, width, height); break;
        case PAWN:   piece = new MPawn(colour, width, height); break;
*/
        default:     qCritical("MPiece::createPiece(.): Failed to instantiate requested piece."); break;
    }

    return piece;
}

MPiece* MPiece::createFromFenPiece(QChar fenPiece, int width, int height)
{
    if ('r' == fenPiece) {return createPiece(ROOK, BLACK, width, height);}
/*
    if ('n' == fenPiece) {return createPiece(KNIGHT, BLACK, width, height);}
    if ('b' == fenPiece) {return createPiece(BISHOP, BLACK, width, height);}
    if ('q' == fenPiece) {return createPiece(QUEEN, BLACK, width, height);}
    if ('k' == fenPiece) {return createPiece(KING, BLACK, width, height);}
    if ('p' == fenPiece) {return createPiece(PAWN, BLACK, width, height);}
*/
    if ('R' == fenPiece) {return createPiece(ROOK, WHITE, width, height);}
/*
    if ('N' == fenPiece) {return createPiece(KNIGHT, WHITE, width, height);}
    if ('B' == fenPiece) {return createPiece(BISHOP, WHITE, width, height);}
    if ('Q' == fenPiece) {return createPiece(QUEEN, WHITE, width, height);}
    if ('K' == fenPiece) {return createPiece(KING, WHITE, width, height);}
    if ('P' == fenPiece) {return createPiece(PAWN, WHITE, width, height);}
*/
    return createPiece(NONE, BLACK, width, height);
}

} // namespace Miniature
