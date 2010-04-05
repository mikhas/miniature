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
#include <position.h>

namespace Miniature
{

bool MQueen::hasFinishedLoading = false;
QSvgRenderer MQueen::blackRenderer;
QSvgRenderer MQueen::whiteRenderer;

MQueen::MQueen(MColour colour)
: MPiece(colour, QUEEN)
{
    loadFromSvgFile();
}

MQueen::~MQueen()
{}

QList<QPoint> MQueen::getPossibleSquares(const MPosition &pos, const QPoint &origin) const
{
    QList<QPoint> possibleSquares;

    // north
    int i = 1;
    while (origin.y() + i < pos.getHeight())
    {
        possibleSquares.append(QPoint(origin.x(), origin.y() + i));
        ++i;
    }

    // north-east
    i = 1;
    while ((origin.x() + i < pos.getWidth()) && (origin.y() + i < pos.getHeight()))
    {
        possibleSquares.append(QPoint(origin.x() + i, origin.y() + i));
        ++i;
    }

    // east
    i = 1;
    while (origin.x() + i < pos.getWidth())
    {
        possibleSquares.append(QPoint(origin.x() + i, origin.y()));
        ++i;
    }

    // south-east
    i = 1;
    while ((origin.x() + i < pos.getWidth()) && (origin.y() - i >= 0))
    {
        possibleSquares.append(QPoint(origin.x() + i, origin.y() - i));
        ++i;
    }

    // south
    i = 1;
    while (origin.y() - i >= 0)
    {
        possibleSquares.append(QPoint(origin.x(), origin.y() - i));
        ++i;
    }

    // south-west
    i = 1;
    while ((origin.x() - i >= 0) && (origin.y() - i >= 0))
    {
        possibleSquares.append(QPoint(origin.x() - i, origin.y() - i));
        ++i;
    }

    // west
    i = 1;
    while (origin.x() - i >= 0)
    {
        possibleSquares.append(QPoint(origin.x() - i, origin.y()));
        ++i;
    }

    // north-west
    i = 1;
    while ((origin.x() - i >= 0) && (origin.y() + i < pos.getHeight()))
    {
        possibleSquares.append(QPoint(origin.x() - i, origin.y() + i));
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
