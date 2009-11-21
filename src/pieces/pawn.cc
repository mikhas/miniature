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

namespace Miniature
{

MPawn::MPawn(MColour col)
: colour(col), type(PAWN), xDim(8), yDim(8)
{}

MPawn::MPawn(MColour col, int boardWidth, int boardLength)
: colour(col), type(PAWN), xDim(boardWidth), yDim(boardLength)
{}

MPawn::~MPawn()
{}

QList<QPoint> MPawn::getPossibleSquares(QPoint point) const
{
	QList<QPoint> possibleSquares = new QList<QPoint>;

	// white
	if (colour == WHITE)
	{
		if (point.y() + 1 < yDim)
		{
			possibleSquares.append(new QPoint(point.x(), point.y() + 1));
			if (point.x() == 0)
			{
				possibleSquares.append(new QPoint(point.x() + 1, point.y() + 1));
			}
			else if (point.x() == 7)
			{
				possibleSquares.append(new QPoint(point.x() - 1, point.y() + 1));
			}
			else
			{
				possibleSquares.append(new QPoint(point.x() + 1, point.y() + 1));
				possibleSquares.append(new QPoint(point.x() - 1, point.y() + 1));
			}
		}

		// pawn is at the base line
		if (point.y() == 1)
		{
			possibleSquares.append(new QPoint(point.x(), point.y() + 2));
		}
	}
	// black
	else
	{
		if (point.y() - 1 >= 0)
		{
			possibleSquares.append(new QPoint(point.x(), point.y() - 1));
			if (point.x() == 0)
			{
				possibleSquares.append(new QPoint(point.x() + 1, point.y() - 1));
			}
			else if (point.x() == 7)
			{
				possibleSquares.append(new QPoint(point.x() - 1, point.y() - 1));
			}
			else
			{
				possibleSquares.append(new QPoint(point.x() + 1, point.y() - 1));
				possibleSquares.append(new QPoint(point.x() - 1, point.y() - 1));
			}
		}

		// pawn is at the base line
		if (point.y() == 6)
		{
			possibleSquares.append(new QPoint(point.x(), point.y() - 2));
		}
	}

	return possibleSquares;
}

}
