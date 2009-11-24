/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2009 Michael Hasselmann <michael@taschenorakel.de>
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

#include "logic_analyzer.h"
#include <iterator>

#include <QList>
#include <QPoint>

#include <iostream>
#include <cstdlib>

namespace
{

int max (int a, int b)
{
    return (b < a) ? a : b;
}

int min (int a, int b)
{
    return (a < b) ? a : b;
}

} // namespace

namespace Miniature
{


MLogicAnalyzer::MLogicAnalyzer(const MConstraintList &constraints, QObject *parent)
: QObject(parent),
  m_constraints(constraints)
{}

MLogicAnalyzer::~MLogicAnalyzer()
{}

bool MLogicAnalyzer::verifyMove(const MPosition &pos, QPoint from, QPoint to) const
{
    // DEBUG
    std::cout << "verify move ... " << std::endl;
    std::cout << "index of (" << from.x() << ", " << from.y() << "): " << pos.indexFromPoint(from) << std::endl;
    for (MPosition::MPieces::const_iterator iter = pos.begin();
         iter != pos.end();
         ++iter)
    {
        if (*iter) {std::cout << " r ";} else {std::cout << " - ";}
        if (7 == (std::distance(pos.begin(), iter) % 8)) {std::cout << std::endl;}
    }
    // TODO: add the rest of the validation. Currently only returns how a piece
    // moves on an empty board.
    MPiece* piece = pos.pieceAt(from);
    if (piece)
    {
        // DEBUG
        std::cout << "found valid piece ... " << std::endl;
        QList<QPoint> list = piece->getPossibleSquares(from);
	if(piece->getType() == MPiece::ROOK)
	{
		// apply rook constraint
        list = applyConStraight(pos, list, from);
	}
	else if (piece->getType() == MPiece::KNIGHT)
	{
	    // apply knight constraint
	    list = applyConKnight(pos, list, from);
	}
	else if (piece->getType() == MPiece::BISHOP)
	{
	    // apply bishop constraint
	    list = applyConDiagonal(pos, list, from);
	}
	else if (piece->getType() == MPiece::QUEEN)
	{
	    // apply queen constraints
		list = applyConStraight(pos, list, from);
	    list = applyConDiagonal(pos, list, from);
	}

        // DEBUG
        for (QList<QPoint>::const_iterator iter = list.begin(); iter != list.end(); ++iter)
        {
            std::cout << "(" << (*iter).x() << ", " << (*iter).y() << ") ";
        }
        std::cout << std::endl;

        return (list.contains(to));
    }

    return false;
}


QList<QPoint> MLogicAnalyzer::applyConKnight(const MPosition &pos, const QList<QPoint> &moveList, QPoint from) const
{
    MPiece *piece = pos.pieceAt(from);
    Q_CHECK_PTR(piece);

    QList<QPoint> newMoveList;
    MPiece::MColour currColour = piece->getColour();

    for(QList<QPoint>::const_iterator iter = moveList.begin();
        iter != moveList.end();
        ++iter)
    {
        QPoint cell = *iter;
        MPiece* currPiece = pos.pieceAt(cell);

        if (!currPiece)
        {
            newMoveList.append(cell);
        }
        else if (currPiece->getColour() != currColour)
        {
            newMoveList.append(cell);
        }
    }

    return newMoveList;    
}	

	
QList<QPoint> MLogicAnalyzer::applyConStraight(const MPosition &pos, const QList<QPoint> &moveList, QPoint from) const
{
    int xMax = 7;
    int xMin = 0;
    int yMax = 7;
    int yMin = 0;

    MPiece *piece = pos.pieceAt(from);
    Q_CHECK_PTR(piece);

    QList<QPoint> newMoveList;
    MPiece::MColour currColour = piece->getColour();

    for(QList<QPoint>::const_iterator iter = moveList.begin();
        iter != moveList.end();
        ++iter)
    {
        QPoint cell = *iter;
        MPiece* currPiece = pos.pieceAt(cell);

        // vertical
        if (cell.y() == from.y())
        {
            if (!currPiece) // cell empty
            {
                continue;
            }
            else
            {
                if (currPiece->getColour() == currColour)
                {
                    if (cell.x() > from.x()) {xMax = min(xMax, cell.x() - 1);}
                    else {xMin = max(xMin, cell.x() + 1);}
                }
                else
                {
                    if (cell.x() > from.x()) {xMax = min(xMax, cell.x());}
                    else {xMin = max(xMin, cell.x());}
                }
            }
        }
        // horizontal
        else if (cell.x() == from.x())
        {
            if (!currPiece)
            {
                continue;
            }
            else
            {
                if (currPiece->getColour() == currColour)
                {
                    if (cell.y() > from.y()) {yMax = min(yMax, cell.y() - 1);}
                    else {yMin = max(yMin, cell.y() + 1);}
                }
                else
                {
                    if (cell.y() > from.y()) {yMax = min(yMax, cell.y());}
                    else {yMin = max(yMin, cell.y());}
                }
            }
        }
    }
    
    for(QList<QPoint>::const_iterator iter = moveList.begin();
        iter != moveList.end();
        ++iter)
    {
        QPoint cell = *iter;

        // vertical
        if (cell.y() == from.y())
        {
            if (cell.x() <= xMax && cell.x() >= xMin)
            {
                std::cout << "A) appending cell(x,y): " << cell.x() << ", " << cell.y() << std::endl;
                newMoveList.append(cell);
            }
        }
        // horizontal
        else if (cell.x() == from.x())
        {
            if (cell.y() <= yMax && cell.y() >= yMin)
            {
                std::cout << "B) appending cell(x,y): " << cell.x() << ", " << cell.y() << std::endl;
                newMoveList.append(cell);
            }
        }
        else
        {
            newMoveList.append(cell);
        }
    }

    return newMoveList;
}

QList<QPoint> MLogicAnalyzer::applyConDiagonal(const MPosition &pos, const QList<QPoint> &moveList, QPoint from) const
{
	/* moveList is supposed to contain only legal moves on an empty board and is supposed to be sorted. Sort order
	 * should be:
	 * 1) North-east -> south-east -> south-west -> north-west
	 * 2) Squares closer to the origin square first
	 */

	bool neObstacle = false;
	bool seObstacle = false;
	bool swObstacle = false;
	bool nwObstacle = false;

	MPiece *piece = pos.pieceAt(from);
	Q_CHECK_PTR(piece);

	QList<QPoint> newMoveList;
	MPiece::MColour currColour = piece->getColour();

    for(QList<QPoint>::const_iterator iter = moveList.begin();
        iter != moveList.end();
        ++iter)
    {
        QPoint cell = *iter;
        MPiece* currPiece = pos.pieceAt(cell);

        // north-east
		if ((from.x() < cell.x()) && (from.y() > cell.y()))
		{
			if (neObstacle)
			{
				continue;
			}

            if (!currPiece)
            {
            	newMoveList.append(cell);
            	continue;
            }

            neObstacle = true;
            if (currColour != currPiece->getColour())
            {
            	newMoveList.append(cell);
            }
		}
        // south-east
		else if ((from.x() < cell.x()) && (from.y() < cell.y()))
		{
			if (seObstacle)
			{
				continue;
			}

            if (!currPiece)
            {
            	newMoveList.append(cell);
            	continue;
            }

            seObstacle = true;
            if (currColour != currPiece->getColour())
            {
            	newMoveList.append(cell);
            }
		}
        // south-west
		else if ((from.x() > cell.x()) && (from.y() < cell.y()))
		{
			if (swObstacle)
			{
				continue;
			}

            if (!currPiece)
            {
            	newMoveList.append(cell);
            	continue;
            }

            swObstacle = true;
            if (currColour != currPiece->getColour())
            {
            	newMoveList.append(cell);
            }
		}
        // north-west
		else if ((from.x() > cell.x()) && (from.y() > cell.y()))
		{
			if (nwObstacle)
			{
				continue;
			}

            if (!currPiece)
            {
            	newMoveList.append(cell);
            	continue;
            }

            nwObstacle = true;
            if (currColour != currPiece->getColour())
            {
            	newMoveList.append(cell);
            }
		}
    }

    return newMoveList;
}

}
