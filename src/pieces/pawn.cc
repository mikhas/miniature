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

#include "pawn.h"
#include <position.h>

namespace Miniature
{

bool MPawn::hasFinishedLoading = false;
QSvgRenderer MPawn::blackRenderer;
QSvgRenderer MPawn::whiteRenderer;

MPawn::MPawn(MColour colour)
: MPiece(colour, PAWN)
{
    loadFromSvgFile();
}

MPawn::~MPawn()
{}

QList<QPoint> MPawn::getPossibleSquares(const MPosition &pos, const QPoint &origin) const
{
    QList<QPoint> possibleSquares;

    // black
    if (colour == BLACK)
    {
        if (origin.y() + 1 < pos.getHeight())
        {
            possibleSquares.append(QPoint(origin.x(), origin.y() + 1));
            possibleSquares.append(QPoint(origin.x(), origin.y() + 2));
            if (origin.x() == 0)
            {
                possibleSquares.append(QPoint(origin.x() + 1, origin.y() + 1));
            }
            else if (origin.x() == pos.getWidth() - 1)
            {
                possibleSquares.append(QPoint(origin.x() - 1, origin.y() + 1));
            }
            else
            {
                possibleSquares.append(QPoint(origin.x() + 1, origin.y() + 1));
                possibleSquares.append(QPoint(origin.x() - 1, origin.y() + 1));
            }
        }
    }
    // white
    else
    {
        if (origin.y() - 1 >= 0)
        {
            possibleSquares.append(QPoint(origin.x(), origin.y() - 1));
            possibleSquares.append(QPoint(origin.x(), origin.y() - 2));
            if (origin.x() == 0)
            {
                possibleSquares.append(QPoint(origin.x() + 1, origin.y() - 1));
            }
            else if (origin.x() == pos.getWidth() - 1)
            {
                possibleSquares.append(QPoint(origin.x() - 1, origin.y() - 1));
            }
            else
            {
                possibleSquares.append(QPoint(origin.x() + 1, origin.y() - 1));
                possibleSquares.append(QPoint(origin.x() - 1, origin.y() - 1));
            }
        }
    }

    return possibleSquares;
}

void MPawn::loadFromSvgFile(int pieceSize)
{
    if (!MPawn::hasFinishedLoading)
    {
        MPawn::blackRenderer.load(QString(":pieces/black/pawn.svg"));
        MPawn::whiteRenderer.load(QString(":pieces/white/pawn.svg"));
        MPawn::hasFinishedLoading = true;
    }

    applyRenderer((MPawn::BLACK == getColour() ? MPawn::blackRenderer
                                               : MPawn::whiteRenderer), pieceSize);
}

QChar MPawn::getLetter() const
{
    return QChar(' ');
}

}
