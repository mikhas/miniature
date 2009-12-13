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

MLogicAnalyzer::MStateFlags MLogicAnalyzer::verifyMove(MPosition &pos, QPoint from, QPoint to)
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
    MSharedPiece piece = pos.pieceAt(from);
    if (!piece.isNull())
    {
        // DEBUG
        //std::cout << "found valid piece ... " << std::endl;
        QList<QPoint> list = piece->getPossibleSquares(from);
        if(piece->getType() == MPiece::ROOK)
        {
        	// apply rook constraint
        	list = applyConStraight(pos, list, from);

        	//flags |= MLogicAnalyzer::ROOK_MOVED;
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
        	// DEBUG
//        	for (QList<QPoint>::const_iterator iter = list.begin(); iter != list.end(); ++iter)
//        	{
//        	    std::cout << "1 (" << (*iter).x() << ", " << (*iter).y() << ") ";
//        	}
//       	std::cout << std::endl;
        	list = applyConKing(pos, list, from);
        	// DEBUG
//        	for (QList<QPoint>::const_iterator iter = list.begin(); iter != list.end(); ++iter)
//        	{
//          	std::cout << "2 (" << (*iter).x() << ", " << (*iter).y() << ") ";
//        	}
//        	std::cout << std::endl;
        	list = applyConKingCastle(pos, list, from);
        	// DEBUG
//        	for (QList<QPoint>::const_iterator iter = list.begin(); iter != list.end(); ++iter)
//        	{
//        		std::cout << "3 (" << (*iter).x() << ", " << (*iter).y() << ") ";
//        	}
//        	std::cout << std::endl;

        	//flags |= MLogicAnalyzer::KING_MOVED;
        }
        else if (piece->getType() == MPiece::PAWN)
        {
        	// apply pawn constraints
        	list = applyConPawnBaseline(pos, list, from);
        	list = applyConPawnObstacle(pos, list, from);
        	list = applyConPawnCapture(pos, list, from);

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
        	// check whether the resulting position leaves the player to move in check => invalid move
        	if (!moveResultsInCkeck(pos, from, to))
        	{
        		flags |= MLogicAnalyzer::VALID;

        		// check whether the resulting position leaves the next player in check => check
        		if (nextPlayerInCkeck(pos, from, to))
        		{
        			flags |= MLogicAnalyzer::CHECK;
        		}
        	}
        }
    }

    return flags;
}

bool MLogicAnalyzer::cellUnderAttack(const MPosition &pos, QPoint cell, MPiece::MColour colour) const
{
    // north
    int i = 1;
    while (cell.y() - i >= 0)
    {
        MSharedPiece piece = pos.pieceAt(QPoint(cell.x(), cell.y() - i));

        if (piece.isNull())
        {
            ++i;
            continue;
        }
        else if (colour == piece->getColour())
        {
            break;
        }
        else
        {
            MPiece::MType currType = piece->getType();
            if ((currType == MPiece::ROOK) || (currType == MPiece::QUEEN))
            {
                return true;
            }
            ++i;
        }
    }

    // north-east
    i = 1;
    while ((cell.y() - i >= 0) && (cell.x() + i < 8))
    {
        MSharedPiece piece = pos.pieceAt(QPoint(cell.x() + i, cell.y() - i));

        if (piece.isNull())
        {
            ++i;
            continue;
        }
        else if (colour == piece->getColour())
        {
            break;
        }
        else
        {
            MPiece::MType currType = piece->getType();
            if ((currType == MPiece::BISHOP) || (currType == MPiece::QUEEN))
            {
                return true;
            }
            if ((currType == MPiece::PAWN) && (i == 1))
            {
                if (colour == MPiece::WHITE)
                {
                    return true;
                }
            }
            ++i;
        }
    }

    // east
    i = 1;
    while (cell.x() + i < 8)
    {
        MSharedPiece piece = pos.pieceAt(QPoint(cell.x() + i, cell.y()));

        if (piece.isNull())
        {
            ++i;
            continue;
        }
        else if (colour == piece->getColour())
        {
            break;
        }
        else
        {
            MPiece::MType currType = piece->getType();
            if ((currType == MPiece::ROOK) || (currType == MPiece::QUEEN))
            {
                return true;
            }
            ++i;
        }
    }

    // south-east
    i = 1;
    while ((cell.y() + i < 8) && (cell.x() + i < 8))
    {
        MSharedPiece piece = pos.pieceAt(QPoint(cell.x() + i, cell.y() + i));

        if (piece.isNull())
        {
            ++i;
            continue;
        }
        else if (colour == piece->getColour())
        {
            break;
        }
        else
        {
            MPiece::MType currType = piece->getType();
            if ((currType == MPiece::BISHOP) || (currType == MPiece::QUEEN))
            {
                return true;
            }
            if ((currType == MPiece::PAWN) && (i == 1))
            {
                if (colour == MPiece::BLACK)
                {
                    return true;
                }
            }
            ++i;
        }
    }

    // south
    i = 1;
    while (cell.y() + i < 8)
    {
        MSharedPiece piece = pos.pieceAt(QPoint(cell.x(), cell.y() + i));

        if (piece.isNull())
        {
            ++i;
            continue;
        }
        else if (colour == piece->getColour())
        {
            break;
        }
        else
        {
            MPiece::MType currType = piece->getType();
            if ((currType == MPiece::ROOK) || (currType == MPiece::QUEEN))
            {
                return true;
            }
            ++i;
        }
    }

    // south-west
    i = 1;
    while ((cell.y() + i < 8) && (cell.x() - i >= 0))
    {
        MSharedPiece piece = pos.pieceAt(QPoint(cell.x() - i, cell.y() + i));

        if (piece.isNull())
        {
            ++i;
            continue;
        }
        else if (colour == piece->getColour())
        {
            break;
        }
        else
        {
            return true;
        }
    }

    // west
    i = 1;
    while (cell.x() - i >= 0)
    {
        MSharedPiece piece = pos.pieceAt(QPoint(cell.x() - i, cell.y()));

        if (piece.isNull())
        {
            ++i;
            continue;
        }
        else if (colour == piece->getColour())
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
                    return true;
                }
                if ((currType == MPiece::PAWN) && (i == 1))
                {
                    if (colour == MPiece::BLACK)
                    {
                        return true;
                    }
                }
                ++i;
            }
            ++i;
        }
    }

    // north-west
    i = 1;
    while ((cell.y() - i >= 0) && (cell.x() - i >= 0))
    {
        MSharedPiece piece = pos.pieceAt(QPoint(cell.x() - i, cell.y() - i));

        if (piece.isNull())
        {
            ++i;
            continue;
        }
        else if (colour == piece->getColour())
        {
            break;
        }
        else
        {
            MPiece::MType currType = piece->getType();
            if ((currType == MPiece::BISHOP) || (currType == MPiece::QUEEN))
            {
                return true;
            }
            if ((currType == MPiece::PAWN) && (i == 1))
            {
                if (colour == MPiece::WHITE)
                {
                    return true;
                }
            }
            ++i;
        }
    }

    // knights
    // north
    if (cell.y() - 2 >= 0)
    {
        if (cell.x() - 1 >= 0)
        {
            MSharedPiece piece = pos.pieceAt(QPoint(cell.x() - 1, cell.y() - 2));
            if (!piece.isNull())
            {
                if ((piece->getType() == MPiece::KNIGHT) && (piece->getColour() != colour))
                {
                    return true;
                }
            }
        }
        else if (cell.x() + 1 < 8)
        {
            MSharedPiece piece = pos.pieceAt(QPoint(cell.x() + 1, cell.y() - 2));
            if (!piece.isNull())
            {
                if ((piece->getType() == MPiece::KNIGHT) && (piece->getColour() != colour))
                {
                    return true;
                }
            }
        }
    }

    // east
    if (cell.x() + 2 < 8)
    {
        if (cell.y() - 1 >= 0)
        {
            MSharedPiece piece = pos.pieceAt(QPoint(cell.x() + 2, cell.y() - 1));
            if (!piece.isNull())
            {
                if ((piece->getType() == MPiece::KNIGHT) && (piece->getColour() != colour))
                {
                    return true;
                }
            }
        }
        else if (cell.y() + 1 < 8)
        {
            MSharedPiece piece = pos.pieceAt(QPoint(cell.x() + 2, cell.y() + 1));
            if (!piece.isNull())
            {
                if ((piece->getType() == MPiece::KNIGHT) && (piece->getColour() != colour))
                {
                    return true;
                }
            }
        }
    }

    // south
    if (cell.y() + 2 < 8)
    {
        if (cell.x() - 1 >= 0)
        {
            MSharedPiece piece = pos.pieceAt(QPoint(cell.x() - 1, cell.y() + 2));
            if (!piece.isNull())
            {
                if ((piece->getType() == MPiece::KNIGHT) && (piece->getColour() != colour))
                {
                    return true;
                }
            }
        }
        else if (cell.x() + 1 < 8)
        {
            MSharedPiece piece = pos.pieceAt(QPoint(cell.x() + 1, cell.y() + 2));
            if (!piece.isNull())
            {
                if ((piece->getType() == MPiece::KNIGHT) && (piece->getColour() != colour))
                {
                    return true;
                }
            }
        }
    }

    // west
    if (cell.x() - 2 >= 0)
    {
        if (cell.y() - 1 >= 0)
        {
            MSharedPiece piece = pos.pieceAt(QPoint(cell.x() - 2, cell.y() - 1));
            if (!piece.isNull())
            {
                if ((piece->getType() == MPiece::KNIGHT) && (piece->getColour() != colour))
                {
                    return true;
                }
            }
        }
        else if (cell.y() + 1 < 8)
        {
            MSharedPiece piece = pos.pieceAt(QPoint(cell.x() - 2, cell.y() + 1));
            if (!piece.isNull())
            {
                if ((piece->getType() == MPiece::KNIGHT) && (piece->getColour() != colour))
                {
                    return true;
                }
            }
        }
    }

    return false;
}


bool MLogicAnalyzer::moveCheckUndo(MPosition &pos, QPoint from, QPoint to, MPiece::MColour colour)
{
	// move piece
	MStorage sourcePiece = pos.store(from);
	MStorage targetPiece = pos.store(to);
	int restoreIndex = sourcePiece.m_index;
	sourcePiece.m_index = targetPiece.m_index;
	pos.restore(&sourcePiece);

	QPoint king = pos.getKing(colour);
	std::cout << "king at: " << king.x() << "," << king.y() << std::endl;

	// perform check
	bool ret = cellUnderAttack(pos, king, colour);

	//restore old position
	MStorage tempPiece = pos.store(to);
	pos.restore(&targetPiece);
	tempPiece.m_index = restoreIndex;
	pos.restore(&tempPiece);

	return ret;
}

bool MLogicAnalyzer::moveResultsInCkeck(MPosition &pos, QPoint from, QPoint to)
{
	MPiece::MColour col = pos.getColourToMove();

	return moveCheckUndo(pos, from, to, col);
}

bool MLogicAnalyzer::nextPlayerInCkeck(MPosition &pos, QPoint from, QPoint to)
{
	MPiece::MColour col = (pos.getColourToMove() == MPiece::WHITE) ? MPiece::BLACK : MPiece::WHITE;

	return moveCheckUndo(pos, from, to, col);
}


QList<QPoint> MLogicAnalyzer::applyConKnight(const MPosition &pos, const QList<QPoint> &moveList, QPoint from) const
{
    MSharedPiece piece = pos.pieceAt(from);

    QList<QPoint> newMoveList;

    MPiece::MColour currColour = MPiece::WHITE;
    if (!piece.isNull())
    {
        currColour = piece->getColour();
    }

    for(QList<QPoint>::const_iterator iter = moveList.begin();
        iter != moveList.end();
        ++iter)
    {
        QPoint cell = *iter;
        MSharedPiece currPiece = pos.pieceAt(cell);

        if (currPiece.isNull())
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

    MSharedPiece piece = pos.pieceAt(from);

    QList<QPoint> newMoveList;
    MPiece::MColour currColour = MPiece::WHITE;
    if (!piece.isNull())
    {
        currColour = piece->getColour();
    }

    for(QList<QPoint>::const_iterator iter = moveList.begin();
        iter != moveList.end();
        ++iter)
    {
        QPoint cell = *iter;
        MSharedPiece currPiece = pos.pieceAt(cell);

        // vertical
        if (cell.y() == from.y())
        {
            if (currPiece.isNull()) // cell empty
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
            if (currPiece.isNull())
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

    MSharedPiece piece = pos.pieceAt(from);

    QList<QPoint> newMoveList;
    MPiece::MColour currColour = MPiece::WHITE;
    if (!piece.isNull())
    {
        currColour = piece->getColour();
    }

    for(QList<QPoint>::const_iterator iter = moveList.begin();
        iter != moveList.end();
        ++iter)
    {
        QPoint cell = *iter;
        MSharedPiece currPiece = pos.pieceAt(cell);

        // north-east
        if ((from.x() < cell.x()) && (from.y() > cell.y()))
        {
            if (neObstacle)
            {
                continue;
            }

            if (currPiece.isNull())
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

            if (currPiece.isNull())
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

            if (currPiece.isNull())
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

            if (currPiece.isNull())
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
    MSharedPiece piece = pos.pieceAt(from);
    QList<QPoint> newMoveList;

    for(QList<QPoint>::const_iterator iter = moveList.begin();
        iter != moveList.end();
        ++iter)
    {
        QPoint cell = *iter;
        MSharedPiece currPiece = pos.pieceAt(cell);

        if (((from.y() == cell.y()) && ((from.x() == cell.x() + 1) || (from.x() == cell.x() - 1))) || (from.y() != cell.y()))
        {
            if (currPiece.isNull())
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
        	newMoveList.append(cell);
        }
    }

    return newMoveList;
}

QList<QPoint> MLogicAnalyzer::applyConKingCastle(const MPosition &pos, const QList<QPoint> &moveList, QPoint from) const
{
    MSharedPiece piece = pos.pieceAt(from);
    MPiece::MColour currColour = MPiece::WHITE;
    if (!piece.isNull())
    {
       currColour = piece->getColour();
    }
    QList<QPoint> newMoveList;

    for(QList<QPoint>::const_iterator iter = moveList.begin();
        iter != moveList.end();
        ++iter)
    {
        QPoint cell = *iter;
        MSharedPiece currPiece = pos.pieceAt(cell);

        if (from.y() == cell.y())
        {
        	// kingside
        	if (from.x() == cell.x() - 2)
        	{
        		MSharedPiece cellInBetween = pos.pieceAt(QPoint(cell.x() - 1, cell.y()));
        		if (currPiece.isNull() && cellInBetween.isNull())
        		{
        			if (currColour == MPiece::WHITE)
        			{
        				std::cout << "white" << std::endl;
        				if (pos.canWhiteCastleKingside())
        				{
        					std::cout << "castle kingside" << std::endl;
        					newMoveList.append(cell);
        				}
        			}
        			else if (currColour == MPiece::BLACK)
        			{
        				std::cout << "black" << std::endl;
        				if (pos.canBlackCastleKingside())
        				{
        					std::cout << "castle kingside" << std::endl;
        					newMoveList.append(cell);
        				}
        			}
        		}
        	}
        	// queenside
        	else if (from.x() == cell.x() + 2)
        	{
        		MSharedPiece cellInBetween = pos.pieceAt(QPoint(cell.x() + 1, cell.y()));
        		MSharedPiece cellNextToRook = pos.pieceAt(QPoint(cell.x() - 1, cell.y()));
        		if (cellNextToRook.isNull() && currPiece.isNull() && cellInBetween.isNull())
        		{
        			if (currColour == MPiece::WHITE)
        			{
        				std::cout << "white" << std::endl;
        				if (pos.canWhiteCastleQueenside())
        				{
        					std::cout << "castle queenside" << std::endl;
        					newMoveList.append(cell);
        				}
        			}
        			else if (currColour == MPiece::BLACK)
        			{
        				std::cout << "black" << std::endl;
        				if (pos.canBlackCastleQueenside())
        				{
        					std::cout << "castle queenside" << std::endl;
        					newMoveList.append(cell);
        				}
        			}
        		}
        	}
        	else
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

QList<QPoint> MLogicAnalyzer::applyConPawnBaseline(const MPosition &pos, const QList<QPoint> &moveList, QPoint from) const
{
    MSharedPiece piece = pos.pieceAt(from);

    QList<QPoint> newMoveList;
    MPiece::MColour currColour = MPiece::WHITE;
    if (!piece.isNull())
    {
        currColour = piece->getColour();
    }

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
        MSharedPiece currPiece = pos.pieceAt(cell);

        if (from.x() == cell.x())
        {
            if (!obstacle)
            {
                if (currPiece.isNull())
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
    MSharedPiece piece = pos.pieceAt(from);

    QList<QPoint> newMoveList;
    MPiece::MColour currColour = MPiece::WHITE;
    if (!piece.isNull())
    {
        currColour = piece->getColour();
    }


    for(QList<QPoint>::const_iterator iter = moveList.begin();
        iter != moveList.end();
        ++iter)
    {
        QPoint cell = *iter;
        MSharedPiece currPiece = pos.pieceAt(cell);

        if (from.x() != cell.x())
        {
            if (!currPiece.isNull())
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
