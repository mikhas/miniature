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

#include "queen.h"
#include <iostream>

namespace Miniature
{

bool MQueen::hasFinishedLoading = false;
QSvgRenderer MQueen::blackRenderer;
QSvgRenderer MQueen::whiteRenderer;

MQueen::MQueen(MColour colour, int width, int height)
: MPiece(colour, QUEEN, width, height)
{
    loadFromSvgFile();
}

MQueen::~MQueen()
{}

QList<QPoint> MQueen::getPossibleSquares(QPoint point) const
{
    QList<QPoint> possibleSquares;

    // north
    int i = 1;
    while (point.y() + i < yDim)
    {
        possibleSquares.append(QPoint(point.x(), point.y() + i));
        ++i;
    }

    // north-east
    i = 1;
    while ((point.x() + i < xDim) && (point.y() + i < yDim))
    {
        possibleSquares.append(QPoint(point.x() + i, point.y() + i));
        ++i;
    }

    // east
    i = 1;
    while (point.x() + i < xDim)
    {
        possibleSquares.append(QPoint(point.x() + i, point.y()));
        ++i;
    }

    // south-east
    i = 1;
    while ((point.x() + i < xDim) && (point.y() - i >= 0))
    {
        possibleSquares.append(QPoint(point.x() + i, point.y() - i));
        ++i;
    }

    // south
    i = 1;
    while (point.y() - i >= 0)
    {
        possibleSquares.append(QPoint(point.x(), point.y() - i));
        ++i;
    }

    // south-west
    i = 1;
    while ((point.x() - i >= 0) && (point.y() - i >= 0))
    {
        possibleSquares.append(QPoint(point.x() - i, point.y() - i));
        ++i;
    }

    // west
    i = 1;
    while (point.x() - i >= 0)
    {
        possibleSquares.append(QPoint(point.x() - i, point.y()));
        ++i;
    }

    // north-west
    i = 1;
    while ((point.x() - i >= 0) && (point.y() + i < yDim))
    {
        possibleSquares.append(QPoint(point.x() - i, point.y() + i));
        ++i;
    }

    for (QList<QPoint>::iterator iter = possibleSquares.begin();
         iter != possibleSquares.end();
         ++iter)
    {
        //std::cout << "    (" << (*iter).x() << ", " << (*iter).y() << "), " << std::endl;
    }

    return possibleSquares;
}

void MQueen::loadFromSvgFile(int pieceSize)
{
    if (!MQueen::hasFinishedLoading)
    {
        MQueen::blackRenderer.load(QString(":pieces/black/queen.svg"));
        MQueen::whiteRenderer.load(QString(":pieces/white/queen.svg"));
        MQueen::hasFinishedLoading = true;
    }

    applyRenderer((MQueen::BLACK == getColour() ? MQueen::blackRenderer
                                                : MQueen::whiteRenderer), pieceSize);
}

QChar MQueen::getLetter() const
{
    return QChar('Q');
}

}
