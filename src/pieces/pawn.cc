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

#include <iostream>

namespace Miniature
{

bool MPawn::hasFinishedLoading = false;
QSvgRenderer MPawn::blackRenderer;
QSvgRenderer MPawn::whiteRenderer;

MPawn::MPawn(MColour colour, int width, int height)
: MPiece(colour, PAWN, width, height)
{}

MPawn::~MPawn()
{}

QList<QPoint> MPawn::getPossibleSquares(QPoint point) const
{
	QList<QPoint> possibleSquares;

	// black
	if (colour == BLACK)
	{
		if (point.y() + 1 < yDim)
		{
			possibleSquares.append(QPoint(point.x(), point.y() + 1));
			possibleSquares.append(QPoint(point.x(), point.y() + 2));
			if (point.x() == 0)
			{
				possibleSquares.append(QPoint(point.x() + 1, point.y() + 1));
			}
			else if (point.x() == xDim - 1)
			{
				possibleSquares.append(QPoint(point.x() - 1, point.y() + 1));
			}
			else
			{
				possibleSquares.append(QPoint(point.x() + 1, point.y() + 1));
				possibleSquares.append(QPoint(point.x() - 1, point.y() + 1));
			}
		}
	}
	// white
	else
	{
		if (point.y() - 1 >= 0)
		{
			possibleSquares.append(QPoint(point.x(), point.y() - 1));
			possibleSquares.append(QPoint(point.x(), point.y() - 2));
			if (point.x() == 0)
			{
				possibleSquares.append(QPoint(point.x() + 1, point.y() - 1));
			}
			else if (point.x() == xDim - 1)
			{
				possibleSquares.append(QPoint(point.x() - 1, point.y() - 1));
			}
			else
			{
				possibleSquares.append(QPoint(point.x() + 1, point.y() - 1));
				possibleSquares.append(QPoint(point.x() - 1, point.y() - 1));
			}
		}
	}

    for (QList<QPoint>::iterator iter = possibleSquares.begin();
         iter != possibleSquares.end();
         ++iter)
    {
        std::cout << "    (" << (*iter).x() << ", " << (*iter).y() << "), " << std::endl;
    }

	return possibleSquares;
}

// TODO: hand out cloned pixmap items instead, saves scaling and maybe more
QGraphicsSvgItem* MPawn::createSvgItem(int pieceSize) const
{
    QGraphicsSvgItem* svgItem = new QGraphicsSvgItem;

    if (!MPawn::hasFinishedLoading)
    {
    	MPawn::blackRenderer.load(QString(":pieces/black/pawn.svg"));
        MPawn::whiteRenderer.load(QString(":pieces/white/pawn.svg"));
        MPawn::hasFinishedLoading = true;
    }

    applyRenderer(svgItem, (MPawn::BLACK == getColour() ? MPawn::blackRenderer
                                                        : MPawn::whiteRenderer), pieceSize);

    return svgItem;
}

}
