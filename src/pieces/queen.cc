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

namespace Miniature
{

MQueen::MQueen(MColour col)
: colour(col), type(QUEEN), xDim(8), yDim(8)
{}

MQueen::MQueen(MColour col, int boardWidth, int boardLength)
: colour(col), type(QUEEN), xDim(boardWidth), yDim(boardLength)
{}

MQueen::~MQueen()
{}

QList<QPoint> MQueen::getPossibleSquares(QPoint point) const
{
	QList<QPoint> possibleSquares = new QList<QPoint>;

	// north
	int i = 1;
	while (point.y() + i < yDim)
	{
		possibleSquares.append(new QPoint(point.x(), point.y() + i));
		++i;
	}

	// north-east
	i = 1;
	while ((point.x() + i < xDim) && (point.y() + i < yDim))
	{
		possibleSquares.append(new QPoint(point.x() + i, point.y() + i));
		++i;
	}

	// east
	i = 1;
	while (point.x() + i < xDim)
	{
		possibleSquares.append(new QPoint(point.x() + 1, point.y()));
		++i;
	}

	// south-east
	i = 1;
	while ((point.x() + i < xDim) && (point.y() - i >= 0))
	{
		possibleSquares.append(new QPoint(point.x() + i, point.y() - i));
		++i;
	}

	// south
	i = 1;
	while (point.y() - i >= 0)
	{
		possibleSquares.append(new QPoint(point.x(), point.y() - i));
		++i;
	}

	// south-west
	i = 1;
	while ((point.x() - i >= 0) && (point.y() - i >= 0))
	{
		possibleSquares.append(new QPoint(point.x() - i, point.y() - i));
		++i;
	}

	// west
	i = 1;
	while (point.x() - i >= 0)
	{
		possibleSquares.append(new QPoint(point.x() - 1, point.y()));
		++i;
	}

	// north-west
	i = 1;
	while ((point.x() - i >= 0) && (point.y() + i < yDim))
	{
		possibleSquares.append(new QPoint(point.x() - i, point.y() + i));
		++i;
	}

	return possibleSquares;
}

}
