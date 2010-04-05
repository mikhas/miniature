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

#include "king.h"
#include <position.h>

namespace Miniature
{

bool MKing::hasFinishedLoading = false;
QSvgRenderer MKing::blackRenderer;
QSvgRenderer MKing::whiteRenderer;

MKing::MKing(MColour colour)
: MPiece(colour, KING),
  castle(true)
{
    loadFromSvgFile();
}

MKing::~MKing()
{}

QList<QPoint> MKing::getPossibleSquares(const MPosition &pos, const QPoint &origin) const
{
    QList<QPoint> possibleSquares;

    // north-west, north and north-east
    if (origin.y() + 1 < pos.getHeight())
    {
        possibleSquares.append(QPoint(origin.x(), origin.y() + 1));
        if (origin.x() - 1 >= 0)
        {
            possibleSquares.append(QPoint(origin.x() - 1, origin.y() + 1));
        }
        if (origin.x() + 1 < pos.getWidth())
        {
            possibleSquares.append(QPoint(origin.x() + 1, origin.y() + 1));
        }
    }

    // east
    if (origin.x() + 1 < pos.getWidth())
    {
        possibleSquares.append(QPoint(origin.x() + 1, origin.y()));
    }

    // south-east, south, south-west
    if (origin.y() - 1 >= 0)
    {
        possibleSquares.append(QPoint(origin.x(), origin.y() - 1));
        if (origin.x() - 1 >= 0)
        {
            possibleSquares.append(QPoint(origin.x() - 1, origin.y() - 1));
        }
        if (origin.x() + 1 < pos.getWidth())
        {
            possibleSquares.append(QPoint(origin.x() + 1, origin.y() - 1));
        }
    }

    // west
    if (origin.x() - 1 >= 0)
    {
        possibleSquares.append(QPoint(origin.x() - 1, origin.y()));
    }

    // castling
    if (castle)
    {
        if (origin.x() + 2 < pos.getWidth())
        {
            possibleSquares.append(QPoint(origin.x() + 2, origin.y()));
        }
        if (origin.x() - 2 >= 0)
        {
            possibleSquares.append(QPoint(origin.x() - 2, origin.y()));
        }
    }

    return possibleSquares;
}

void MKing::hasMoved()
{
    castle = false;
}

bool MKing::canCastle()
{
    return castle;
}

void MKing::loadFromSvgFile(int pieceSize)
{
    if (!MKing::hasFinishedLoading)
    {
        MKing::blackRenderer.load(QString(":pieces/black/king.svg"));
        MKing::whiteRenderer.load(QString(":pieces/white/king.svg"));
        MKing::hasFinishedLoading = true;
    }

    applyRenderer((MKing::BLACK == getColour() ? MKing::blackRenderer
                                               : MKing::whiteRenderer), pieceSize);
}

QChar MKing::getLetter() const
{
    return QChar('K');
}

}
