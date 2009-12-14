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
#include <iostream>

namespace Miniature
{

bool MKing::hasFinishedLoading = false;
QSvgRenderer MKing::blackRenderer;
QSvgRenderer MKing::whiteRenderer;

MKing::MKing(MColour colour, int width, int height)
: MPiece(colour, KING, width, height),
  castle(true)
{
    loadFromSvgFile();
}

MKing::~MKing()
{}

QList<QPoint> MKing::getPossibleSquares(QPoint point) const
{
    QList<QPoint> possibleSquares;

    // north-west, north and north-east
    if (point.y() + 1 < yDim)
    {
        possibleSquares.append(QPoint(point.x(), point.y() + 1));
        if (point.x() - 1 >= 0)
        {
            possibleSquares.append(QPoint(point.x() - 1, point.y() + 1));
        }
        if (point.x() + 1 < xDim)
        {
            possibleSquares.append(QPoint(point.x() + 1, point.y() + 1));
        }
    }

    // east
    if (point.x() + 1 < xDim)
    {
        possibleSquares.append(QPoint(point.x() + 1, point.y()));
    }

    // south-east, south, south-west
    if (point.y() - 1 >= 0)
    {
        possibleSquares.append(QPoint(point.x(), point.y() - 1));
        if (point.x() - 1 >= 0)
        {
            possibleSquares.append(QPoint(point.x() - 1, point.y() - 1));
        }
        if (point.x() + 1 < xDim)
        {
            possibleSquares.append(QPoint(point.x() + 1, point.y() - 1));
        }
    }

    // west
    if (point.x() - 1 >= 0)
    {
        possibleSquares.append(QPoint(point.x() - 1, point.y()));
    }

    // castling
    if (castle)
    {
        if (point.x() + 2 < xDim)
        {
            possibleSquares.append(QPoint(point.x() + 2, point.y()));
        }
        if (point.x() - 2 >= 0)
        {
            possibleSquares.append(QPoint(point.x() - 2, point.y()));
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
