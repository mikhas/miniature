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

#include "knight.h"
#include <position.h>

namespace Miniature
{

bool MKnight::hasFinishedLoading = false;
QSvgRenderer MKnight::blackRenderer;
QSvgRenderer MKnight::whiteRenderer;

MKnight::MKnight(MColour colour)
: MPiece(colour, KNIGHT)
{
    loadFromSvgFile();
}

MKnight::~MKnight()
{}

QList<QPoint> MKnight::getPossibleSquares(const MPosition &pos, const QPoint &origin) const
{
    QList<QPoint> possibleSquares;

    // north
    if (origin.y() + 2 < pos.getHeight())
    {
        if (origin.x() + 1 < pos.getWidth())
        {
            possibleSquares.append(QPoint(origin.x() + 1, origin.y() + 2));
        }

        if (origin.x() - 1 >= 0)
        {
            possibleSquares.append(QPoint(origin.x() - 1, origin.y() + 2));
        }
    }

    // east
    if (origin.x() + 2 < pos.getWidth())
    {
        if (origin.y() + 1 < pos.getHeight())
        {
            possibleSquares.append(QPoint(origin.x() + 2, origin.y() + 1));
        }

        if (origin.y() - 1 >= 0)
        {
            possibleSquares.append(QPoint(origin.x() + 2, origin.y() - 1));
        }
    }

    // south
    if (origin.y() - 2 >= 0)
    {
        if (origin.x() + 1 < pos.getWidth())
        {
            possibleSquares.append(QPoint(origin.x() + 1, origin.y() - 2));
        }

        if (origin.x() - 1 >= 0)
        {
            possibleSquares.append(QPoint(origin.x() - 1, origin.y() - 2));
        }
    }

    // west
    if (origin.x() - 2 >= 0)
    {
        if (origin.y() + 1 < pos.getHeight())
        {
            possibleSquares.append(QPoint(origin.x() - 2, origin.y() + 1));
        }

        if (origin.y() - 1 >= 0)
        {
            possibleSquares.append(QPoint(origin.x() - 2, origin.y() - 1));
        }
    }

    for (QList<QPoint>::iterator iter = possibleSquares.begin();
         iter != possibleSquares.end();
         ++iter)
        {
            //std::cout << "    (" << (*iter).x() << ", " << (*iter).y() << "), " << std::endl;
        }

    return possibleSquares;
}

void MKnight::loadFromSvgFile(int pieceSize)
{
    if (!MKnight::hasFinishedLoading)
    {
        MKnight::blackRenderer.load(QString(":pieces/black/knight.svg"));
        MKnight::whiteRenderer.load(QString(":pieces/white/knight.svg"));
        MKnight::hasFinishedLoading = true;
    }

    applyRenderer((MKnight::BLACK == getColour() ? MKnight::blackRenderer
                                                 : MKnight::whiteRenderer), pieceSize);
}

QChar MKnight::getLetter() const
{
    return QChar('N');
}

}
