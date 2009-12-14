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

#include "bishop.h"
#include <iostream>

namespace Miniature
{

bool MBishop::hasFinishedLoading = false;
QSvgRenderer MBishop::blackRenderer;
QSvgRenderer MBishop::whiteRenderer;

MBishop::MBishop(MColour colour, int width, int height)
: MPiece(colour, BISHOP, width, height)
{
    loadFromSvgFile();
}

MBishop::~MBishop()
{}

QList<QPoint> MBishop::getPossibleSquares(QPoint point) const
{
    QList<QPoint> possibleSquares;

    // north-east
    int i = 1;
    while ((point.x() + i < xDim) && (point.y() - i >= 0))
    {
        possibleSquares.append(QPoint(point.x() + i, point.y() - i));
        ++i;
    }

    // south-east
    i = 1;
    while ((point.x() + i < xDim) && (point.y() + i < yDim))
    {
        possibleSquares.append(QPoint(point.x() + i, point.y() + i));
        ++i;
    }

    // south-west
    i = 1;
    while ((point.x() - i >= 0) && (point.y() + i < yDim))
    {
        possibleSquares.append(QPoint(point.x() - i, point.y() + i));
        ++i;
    }

    // north-west
    i = 1;
    while ((point.x() - i >= 0) && (point.y() - i >= 0))
    {
        possibleSquares.append(QPoint(point.x() - i, point.y() - i));
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

void MBishop::loadFromSvgFile(int pieceSize)
{
    if (!MBishop::hasFinishedLoading)
    {
        MBishop::blackRenderer.load(QString(":pieces/black/bishop.svg"));
        MBishop::whiteRenderer.load(QString(":pieces/white/bishop.svg"));
        MBishop::hasFinishedLoading = true;
    }

    applyRenderer((MBishop::BLACK == getColour() ? MBishop::blackRenderer
                                                 : MBishop::whiteRenderer), pieceSize);
}

QChar MBishop::getLetter() const
{
    return QChar('B');
}

}
