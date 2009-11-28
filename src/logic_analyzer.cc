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


MLogicAnalyzer::MLogicAnalyzer(const MConstraintList &constraints)
: m_constraints(constraints)
{}

MLogicAnalyzer::~MLogicAnalyzer()
{}

MLogicAnalyzer::MStateFlags MLogicAnalyzer::verifyMove(const MPosition &pos, QPoint from, QPoint to) const
{
    MStateFlags flags = MLogicAnalyzer::INVALID;
    // DEBUG
    //std::cout << "verify move ... " << std::endl;
    //std::cout << "index of (" << from.x() << ", " << from.y() << "): " << pos.indexFromPoint(from) << std::endl;
/*
    for (MPosition::MPieces::const_iterator iter = pos.begin();
         iter != pos.end();
         ++iter)
    {
        if (*iter) {std::cout << " r ";} else {std::cout << " - ";}
        if (7 == (std::distance(pos.begin(), iter) % 8)) {std::cout << std::endl;}
    }
*/
    // TODO: add the rest of the validation. Currently only returns how a piece
    // moves on an empty board.
    MPiece* piece = pos.pieceAt(from);
    if (piece)
    {
        // DEBUG
        //std::cout << "found valid piece ... " << std::endl;
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
    else if (piece->getType() == MPiece::KING)
	{
	    // apply king constraint
	    list = applyConKing(pos, list, from);
	}
	else if (piece->getType() == MPiece::PAWN)
	{
	    // apply pawn constraints
		//std::cout << "1. con" << std::endl;
		list = applyConPawnBaseline(pos, list, from);
		//std::cout << "2. con" << std::endl;
	    list = applyConPawnObstacle(pos, list, from);
	    //std::cout << "3. con" << std::endl;
	    list = applyConPawnCapture(pos, list, from);
	    //std::cout << "nach 3. con" << std::endl;

            // check for promotion
            if (to.y() == (piece->getColour() == MPiece::BLACK ? 7 : 0))
            {
                flags |= MLogicAnalyzer::PROMOTION;
            }
	}

        // DEBUG
        for (QList<QPoint>::const_iterator iter = list.begin(); iter != list.end(); ++iter)
        {
            //std::cout << "(" << (*iter).x() << ", " << (*iter).y() << ") ";
        }
        //std::cout << std::endl;

        if (list.contains(to))
        {
             flags |= MLogicAnalyzer::VALID;
        }
    }

    return flags;
}

bool MLogicAnalyzer::checkCheck(const MPosition &pos, QPoint king)
{
	MPiece::MColour kingColour = pos.pieceAt(king)->getColour();

	// north
	int i = 1;
	while (king.y() - i >= 0)
	{
		MPiece *piece = pos.pieceAt(QPoint(king.x(), king.y() - i));

		if (!piece)
		{
			++i;
			continue;
		}
		else if (kingColour == piece->getColour())
		{
			break;
		}
		else
		{
			MPiece::MType currType = piece->getType();
			if ((currType == MPiece::ROOK) || (currType == MPiece::QUEEN))
			{
				return false;
			}
			++i;
		}
	}

	// north-east
	i = 1;
	while ((king.y() - i >= 0) && (king.x() + i < 8))
	{
		MPiece *piece = pos.pieceAt(QPoint(king.x() + i, king.y() - i));

		if (!piece)
		{
			++i;
			continue;
		}
		else if (kingColour == piece->getColour())
		{
			break;
		}
		else
		{
			MPiece::MType currType = piece->getType();
			if ((currType == MPiece::BISHOP) || (currType == MPiece::QUEEN))
			{
				return false;
			}
			if ((currType == MPiece::PAWN) && (i == 1))
			{
				if (kingColour == MPiece::WHITE)
				{
					return false;
				}
			}
			++i;
		}
	}

	// east
	i = 1;
	while (king.x() + i < 8)
	{
		MPiece *piece = pos.pieceAt(QPoint(king.x() + i, king.y()));

		if (!piece)
		{
			++i;
			continue;
		}
		else if (kingColour == piece->getColour())
		{
			break;
		}
		else
		{
			MPiece::MType currType = piece->getType();
			if ((currType == MPiece::ROOK) || (currType == MPiece::QUEEN))
			{
				return false;
			}
			++i;
		}
	}

	// south-east
	i = 1;
	while ((king.y() + i < 8) && (king.x() + i < 8))
	{
		MPiece *piece = pos.pieceAt(QPoint(king.x() + i, king.y() + i));

		if (!piece)
		{
			++i;
			continue;
		}
		else if (kingColour == piece->getColour())
		{
			break;
		}
		else
		{
			MPiece::MType currType = piece->getType();
			if ((currType == MPiece::BISHOP) || (currType == MPiece::QUEEN))
			{
				return false;
			}
			if ((currType == MPiece::PAWN) && (i == 1))
			{
				if (kingColour == MPiece::BLACK)
				{
					return false;
				}
			}
			++i;
		}
	}

	// south
	i = 1;
	while (king.y() + i < 8)
	{
		MPiece *piece = pos.pieceAt(QPoint(king.x(), king.y() + i));

		if (!piece)
		{
			++i;
			continue;
		}
		else if (kingColour == piece->getColour())
		{
			break;
		}
		else
		{
			MPiece::MType currType = piece->getType();
			if ((currType == MPiece::ROOK) || (currType == MPiece::QUEEN))
			{
				return false;
			}
			++i;
		}
	}

	// south-west
	i = 1;
	while ((king.y() + i < 8) && (king.x() - i >= 0))
	{
		MPiece *piece = pos.pieceAt(QPoint(king.x() - i, king.y() + i));

		if (!piece)
		{
			++i;
			continue;
		}
		else if (kingColour == piece->getColour())
		{
			break;
		}
		else
		{
			return false;
		}
	}

	// west
	i = 1;
	while (king.x() - i >= 0)
	{
		MPiece *piece = pos.pieceAt(QPoint(king.x() - i, king.y()));

		if (!piece)
		{
			++i;
			continue;
		}
		else if (kingColour == piece->getColour())
		{
			break;
		}
		else
		{
			MPiece::MType currType = piece->getType();
			if ((currType == MPiece::ROOK) || (currType == MPiece::QUEEN))
			{
				MPiece::MType currType = piece->getType();
				if ((currType == MPiece::BISHOP) || (currType == MPiece::QUEEN))
				{
					return false;
				}
				if ((currType == MPiece::PAWN) && (i == 1))
				{
					if (kingColour == MPiece::BLACK)
					{
						return false;
					}
				}
				++i;
			}
			++i;
		}
	}

	// north-west
	i = 1;
	while ((king.y() - i >= 0) && (king.x() - i >= 0))
	{
		MPiece *piece = pos.pieceAt(QPoint(king.x() - i, king.y() - i));

		if (!piece)
		{
			++i;
			continue;
		}
		else if (kingColour == piece->getColour())
		{
			break;
		}
		else
		{
			MPiece::MType currType = piece->getType();
			if ((currType == MPiece::BISHOP) || (currType == MPiece::QUEEN))
			{
				return false;
			}
			if ((currType == MPiece::PAWN) && (i == 1))
			{
				if (kingColour == MPiece::WHITE)
				{
					return false;
				}
			}
			++i;
		}
	}

	// knights
	// north
	if (king.y() - 2 >= 0)
	{
		if (king.x() - 1 >= 0)
		{
			MPiece *piece = pos.pieceAt(QPoint(king.x() - 1, king.y() - 2));
			if (piece)
			{
				if ((piece->getType() == MPiece::KNIGHT) && (piece->getColour() != kingColour))
				{
					return false;
				}
			}
		}
		else if (king.x() + 1 < 8)
		{
			MPiece *piece = pos.pieceAt(QPoint(king.x() + 1, king.y() - 2));
			if (piece)
			{
				if ((piece->getType() == MPiece::KNIGHT) && (piece->getColour() != kingColour))
				{
					return false;
				}
			}
		}
	}

	// east
	if (king.x() + 2 < 8)
	{
		if (king.y() - 1 >= 0)
		{
			MPiece *piece = pos.pieceAt(QPoint(king.x() + 2, king.y() - 1));
			if (piece)
			{
				if ((piece->getType() == MPiece::KNIGHT) && (piece->getColour() != kingColour))
				{
					return false;
				}
			}
		}
		else if (king.y() + 1 < 8)
		{
			MPiece *piece = pos.pieceAt(QPoint(king.x() + 2, king.y() + 1));
			if (piece)
			{
				if ((piece->getType() == MPiece::KNIGHT) && (piece->getColour() != kingColour))
				{
					return false;
				}
			}
		}
	}

	// south
	if (king.y() + 2 < 8)
	{
		if (king.x() - 1 >= 0)
		{
			MPiece *piece = pos.pieceAt(QPoint(king.x() - 1, king.y() + 2));
			if (piece)
			{
				if ((piece->getType() == MPiece::KNIGHT) && (piece->getColour() != kingColour))
				{
					return false;
				}
			}
		}
		else if (king.x() + 1 < 8)
		{
			MPiece *piece = pos.pieceAt(QPoint(king.x() + 1, king.y() + 2));
			if (piece)
			{
				if ((piece->getType() == MPiece::KNIGHT) && (piece->getColour() != kingColour))
				{
					return false;
				}
			}
		}
	}

	// west
	if (king.x() - 2 >= 0)
	{
		if (king.y() - 1 >= 0)
		{
			MPiece *piece = pos.pieceAt(QPoint(king.x() - 2, king.y() - 1));
			if (piece)
			{
				if ((piece->getType() == MPiece::KNIGHT) && (piece->getColour() != kingColour))
				{
					return false;
				}
			}
		}
		else if (king.y() + 1 < 8)
		{
			MPiece *piece = pos.pieceAt(QPoint(king.x() - 2, king.y() + 1));
			if (piece)
			{
				if ((piece->getType() == MPiece::KNIGHT) && (piece->getColour() != kingColour))
				{
					return false;
				}
			}
		}
	}

	return true;
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
                //std::cout << "A) appending cell(x,y): " << cell.x() << ", " << cell.y() << std::endl;
                newMoveList.append(cell);
            }
        }
        // horizontal
        else if (cell.x() == from.x())
        {
            if (cell.y() <= yMax && cell.y() >= yMin)
            {
                //std::cout << "B) appending cell(x,y): " << cell.x() << ", " << cell.y() << std::endl;
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
		else
		{
			newMoveList.append(cell);
		}
    }

    return newMoveList;
}

QList<QPoint> MLogicAnalyzer::applyConKing(const MPosition &pos, const QList<QPoint> &moveList, QPoint from) const
{
	MPiece *piece = pos.pieceAt(from);
	Q_CHECK_PTR(piece);

	QList<QPoint> newMoveList;

    for(QList<QPoint>::const_iterator iter = moveList.begin();
        iter != moveList.end();
        ++iter)
    {
        QPoint cell = *iter;
        MPiece* currPiece = pos.pieceAt(cell);

        if (((from.y() == cell.y()) && ((from.x() == cell.x() + 1) || (from.x() == cell.x() - 1))) || (from.y() != cell.y()))
        {
		    if (!currPiece)
		    {
               newMoveList.append(cell);
		    }
            else if (piece->getColour() != currPiece->getColour())
            {
                newMoveList.append(cell);
            }
        }
        else
        {
            // TODO uncomment when castling is done
            //newMoveList.append(cell);
        }
	}

    return newMoveList;
}

QList<QPoint> MLogicAnalyzer::applyConPawnBaseline(const MPosition &pos, const QList<QPoint> &moveList, QPoint from) const
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

        if (currColour == MPiece::WHITE)
        {
        	if (from.y() - cell.y() == 2)
        	{
        		if (from.y() == 6)
        		{
        			newMoveList.append(cell);
        			//std::cout << "1. Con: added (" << cell.x() << "," << cell.y() << ")" << std::endl;
        		}
        	}
        	else
        	{
        		newMoveList.append(cell);
        		//std::cout << "1. Con: added (" << cell.x() << "," << cell.y() << ")" << std::endl;
        	}
        }
        else
        {
        	if (cell.y() - from.y() == 2)
        	{
        		if (from.y() == 1)
        		{
        			newMoveList.append(cell);
        		}
        	}
        	else
        	{
        		newMoveList.append(cell);
        	}
        }
    }

    return newMoveList;
}


QList<QPoint> MLogicAnalyzer::applyConPawnObstacle(const MPosition &pos, const QList<QPoint> &moveList, QPoint from) const
{
	// moveList is supposed to be sorted in a way that the one-step move comes before the two-step move if there's any.
	bool obstacle = false;

	QList<QPoint> newMoveList;

    for(QList<QPoint>::const_iterator iter = moveList.begin();
        iter != moveList.end();
        ++iter)
    {
        QPoint cell = *iter;
        MPiece* currPiece = pos.pieceAt(cell);

        if (from.x() == cell.x())
        {
        	if (!obstacle)
        	{
        		if (!currPiece)
        		{
        			newMoveList.append(cell);
        			//std::cout << "2. Con: added (" << cell.x() << "," << cell.y() << ")" << std::endl;
        			continue;
        		}
        		else
        		{
        			obstacle = true;
        		}
        	}
        }
        else
        {
        	newMoveList.append(cell);
        	//std::cout << "2. Con: added (" << cell.x() << "," << cell.y() << ")" << std::endl;
        }
    }

    return newMoveList;
}
QList<QPoint> MLogicAnalyzer::applyConPawnCapture(const MPosition &pos, const QList<QPoint> &moveList, QPoint from) const
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

        if (from.x() != cell.x())
        {
        	if (currPiece)
        	{
        		if (currColour != currPiece->getColour())
        		{
        			newMoveList.append(cell);
        		}
        	}
        }
        else
        {
        	newMoveList.append(cell);
        	//std::cout << "added (" << cell.x() << "," << cell.y() << ")" << std::endl;
        }
    }

    return newMoveList;
}

}
