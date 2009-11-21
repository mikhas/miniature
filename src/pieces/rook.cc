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

#include "rook.h"

namespace Miniature
{

MRook::MRook(MColour col)
: colour(col), type(ROOK), xDim(8), yDim(8), canCastle(true)
{}

MRook::MRook(MColour col, int boardWidth, int boardLength)
: colour(col), type(ROOK), xDim(boardWidth), yDim(boardLength), canCastle(true)
{}

MRook::MRook(MColour col, int boardWidth, int boardLength, bool canCastle)
: colour(col), type(ROOK), xDim(boardWidth), yDim(boardLength), canCastle(canCastle)
{}

MRook::~MRook()
{}

QList<QPoint> MRook::getPossibleSquares(QPoint point) const
{
	QList<QPoint> possibleSquares = new QList<QPoint>;

	// north
	int i = 1;
	while (point.y() + i < yDim)
	{
		possibleSquares.append(new QPoint(point.x(), point.y() + i));
		++i;
	}

	// east
	i = 1;
	while (point.x() + i < xDim)
	{
		possibleSquares.append(new QPoint(point.x() + 1, point.y()));
		++i;
	}

	// south
	i = 1;
	while (point.y() - i >= 0)
	{
		possibleSquares.append(new QPoint(point.x(), point.y() - i));
		++i;
	}

	// west
	i = 1;
	while (point.x() - i >= 0)
	{
		possibleSquares.append(new QPoint(point.x() - 1, point.y()));
		++i;
	}

	return possibleSquares;
}

bool MRook::isMoveAllowed (QPoint from, QPoint to) const
{
	// Test for dimensions
	if ((from.x() < 0)
			|| (from.x() >= xDim)
			|| (from.y() < 0)
			|| (from.y() >= yDim))
	{
		return false;
	}

	// Test if the move is only vertical or horizontal
	if ((from.x() != to.x()) && (from.y() != to.y()))
	{
		return false;
	}

	return true;
}

void MRook::hasMoved()
{
	canCastle = false;
}

bool MRook::canCastle()
{
	return canCastle;
}


}
