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
#include <iostream>

namespace Miniature
{

MKnight::MKnight(MColour col)
: MPiece(colour, KNIGHT, width, height),
{}

MKnight::~MKnight()
{}

QList<QPoint> MKnight::getPossibleSquares(QPoint point) const
{
	QList<QPoint> possibleSquares = new QList<QPoint>;

	// north
	if (point.y() + 2 < yDim)
	{
		if (point.x() + 1 < xDim)
		{
			possibleSquares.append(new QPoint(point.x() + 1, point.y() + 2));
		}

		if (point.x() - 1 >= 0)
		{
			possibleSquares.append(new QPoint(point.x() - 1, point.y() + 2));
		}
	}

	// east
	if (point.x() + 2 < xDim)
	{
		if (point.y() + 1 < yDim)
		{
			possibleSquares.append(new QPoint(point.x() + 2, point.y() + 1));
		}

		if (point.y() - 1 >= 0)
		{
			possibleSquares.append(new QPoint(point.x() + 2, point.y() - 1));
		}
	}

	// south
	if (point.y() - 2 >= 0)
	{
		if (point.x() + 1 < xDim)
		{
			possibleSquares.append(new QPoint(point.x() + 1, point.y() - 2));
		}

		if (point.x() - 1 >= 0)
		{
			possibleSquares.append(new QPoint(point.x() - 1, point.y() - 2));
		}
	}

	// west
	if (point.x() - 2 >= 0)
	{
		if (point.y() + 1 < yDim)
		{
			possibleSquares.append(new QPoint(point.x() - 2, point.y() + 1));
		}

		if (point.y() - 1 >= 0)
		{
			possibleSquares.append(new QPoint(point.x() - 2, point.y() - 1));
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


}
