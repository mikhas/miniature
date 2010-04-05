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

MLogicAnalyzer::mMoveFlags MLogicAnalyzer::verifyMove(MPosition * const position, const QPoint &origin, const QPoint &target) const
{
    mMoveFlags flags = MLogicAnalyzer::INVALID_MOVE;

    MPiece *piece = position->pieceAt(origin);
    if (!piece)
    {
        return flags;
    }

    // This switch should be rewritten using polymorphism. Each piece should
    // know its list of contraints, IMO.
    mCellList list = piece->getPossibleSquares(*position, origin);
    switch(piece->getType())
    {
        case MPiece::ROOK:
        {
            // apply rook constraint
            list = applyConStraight(*position, list, origin);

            //flags |= MLogicAnalyzer::ROOK_MOVED;
        } break;

        case MPiece::KNIGHT:
        {
            // apply knight constraint
            list = applyConKnight(*position, list, origin);
        } break;

        case MPiece::BISHOP:
        {
            // apply bishop constraint
            list = applyConDiagonal(*position, list, origin);
        } break;

        case MPiece::QUEEN:
        {
            // apply queen constraints
            list = applyConStraight(*position, list, origin);
            list = applyConDiagonal(*position, list, origin);
        } break;

        case MPiece::KING:
        {
            // apply king constraint
            list = applyConKing(*position, list, origin);
            list = applyConKingCastle(*position, list, origin, flags);

            // TODO: this double-checking is annoying. perhaps the constraint
            // rules need the full context to work correctly: position, list
            // (as out param), origin, target, flags (as return param).
            if (CASTLE_KINGSIDE_ALLOWED & flags &&
                2 == (target.x() - origin.x()))
            {
                flags |= CASTLE_KINGSIDE;
            }

            if (CASTLE_QUEENSIDE_ALLOWED & flags &&
                2 == (origin.x() - target.x()))
            {
                flags |= CASTLE_QUEENSIDE;
            }
        } break;


        case MPiece::PAWN:
        {
            // apply pawn constraints
            list = applyConPawnBaseline(*position, list, origin);
            list = applyConPawnObstacle(*position, list, origin);
            list = applyConPawnCapture(*position, list, origin, flags);

            // check for promotion
            if (target.y() == (piece->getColour() == MPiece::BLACK ? 7 : 0))
            {
                flags |= MLogicAnalyzer::PROMOTION;
            }

            // check for double move, probably belongs into applyConPawnBaseline?
            if (2 == qAbs(target.y() - origin.y()))
            {
                flags |= MLogicAnalyzer::PAWN_DOUBLEMOVE;
            }

        } break;

        default: case MPiece::NONE: break; // nothing to do!
    }

    if (list.contains(target))
    {
        // check whether the resulting position leaves the player to move in check => invalid move
        // Why is it that kings can move *into* chess?
        if (!moveResultsInCheck(*position, origin, target))
        {
            flags |= MLogicAnalyzer::VALID_MOVE;
        }
    }

    return flags;
}

MLogicAnalyzer::mPositionFlags MLogicAnalyzer::verifyPosition(MPosition * const position) const
{
    mPositionFlags flags = MLogicAnalyzer::INVALID_POSITION;

    // check whether the resulting position leaves the next player in check => check
    if (inCheck(*position))
    {
       flags |= MLogicAnalyzer::IN_CHECK;
    }

    return flags;
}

bool MLogicAnalyzer::cellUnderAttack(const MPosition &position, const QPoint &cell, MPiece::MColour colour) const
{
    MPiece::MColour oppositeColor = ((colour == MPiece::WHITE) ? MPiece::BLACK : MPiece::WHITE);

    /* The idea is to make a reverse search, starting from cell. First, look
     * whether a piece could reach the cell by *pretending we'd want to move that
     * piece somewhere, starting *from* cell. Then, we look whether the constrained
     * list contains a piece of that type.
     */

    {
        // Attacked by a queen?
        MQueen piece(oppositeColor);
        mCellList list = piece.getPossibleSquares(position, cell);
        list = applyConStraight(position, list, cell);
        list = applyConDiagonal(position, list, cell);

        if (isPieceInList(position, piece, list))
            return true;
    }

    {
        // Attacked by a rook?
        MRook piece(oppositeColor);
        mCellList list = piece.getPossibleSquares(position, cell);
        list = applyConStraight(position, list, cell);

        if (isPieceInList(position, piece, list))
            return true;
    }

    {
        // Attacked by a bishop?
        MBishop piece(oppositeColor);
        mCellList list = piece.getPossibleSquares(position, cell);
        list = applyConDiagonal(position, list, cell);

        if (isPieceInList(position, piece, list))
            return true;
    }

    {
        // Attacked by a knight?
        MKnight piece(oppositeColor);
        mCellList list = piece.getPossibleSquares(position, cell);
        list = applyConKnight(position, list, cell);

        if (isPieceInList(position, piece, list))
            return true;
    }

    {
        // Attacked by a pawn?
        // The pawn is the only unit that cannot move reversivly, so the
        // generic approach fails here. Luckily, pawn attacks are dead-simple.
        MPawn piece(oppositeColor);
        mCellList list;
        if (MPiece::WHITE == colour)
        {
            // can be attacked from {north,south}-west
            list << QPoint(cell.x() -1, cell.y() + (MPiece::WHITE == colour ? -1 : 1));

            // can be attacked from {south,north}-east
            list << QPoint(cell.x() +1, cell.y() + (MPiece::WHITE == colour ? -1 : 1));
        }

        if (isPieceInList(position, piece, list))
            return true;
    }

    {
        // Attacked by a king?
        MKing piece(oppositeColor);
        mCellList list = piece.getPossibleSquares(position, cell);
        list = applyConKing(position, list, cell);

        if (isPieceInList(position, piece, list))
            return true;
    }

    return false;
}

bool MLogicAnalyzer::isPieceInList(const MPosition &position, const MPiece& piece, const mCellList &list) const
{
    mCellListIter iter(list);
    while (iter.hasNext())
    {
        MPiece *foundPiece = position.pieceAt(iter.next());
        if (foundPiece &&
            piece.getType() == foundPiece->getType() &&
            piece.getColour() == foundPiece->getColour())
        {
            return true;
        }
    }

    return false;
}

bool MLogicAnalyzer::moveCheckUndo(const MPosition &position, const QPoint &origin, const QPoint &target, MPiece::MColour colour) const
{
    MPosition moveCheck = MPosition(position);
    moveCheck.movePiece(origin, target);

    QPoint king = moveCheck.getKing(colour);
    std::cout << "king at: " << king.x() << "," << king.y() << std::endl;

    // perform check
    return cellUnderAttack(moveCheck, king, colour);
}

bool MLogicAnalyzer::moveResultsInCheck(const MPosition &position, const QPoint &origin, const QPoint &target) const
{
    return moveCheckUndo(position, origin, target,
                         position.getColourToMove());
}

bool MLogicAnalyzer::nextPlayerInCheck(const MPosition &position, const QPoint &origin, const QPoint &target) const
{
    return moveCheckUndo(position, origin, target,
                         ((position.getColourToMove() == MPiece::WHITE) ? MPiece::BLACK : MPiece::WHITE));
}

bool MLogicAnalyzer::inCheck(const MPosition &position) const
{
    MPiece::MColour colour = position.getColourToMove();
    return cellUnderAttack(position, position.getKing(colour), colour);
}

MLogicAnalyzer::mCellList MLogicAnalyzer::applyConKnight(const MPosition &pos, const mCellList &moveList, QPoint from) const
{
    MPiece *piece = pos.pieceAt(from);

    mCellList newMoveList;

    MPiece::MColour currColour = MPiece::WHITE;
    if (piece)
    {
        currColour = piece->getColour();
    }

    for(mCellList::const_iterator iter = moveList.begin();
        iter != moveList.end();
        ++iter)
    {
        QPoint cell = *iter;
        MPiece *currPiece = pos.pieceAt(cell);

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


MLogicAnalyzer::mCellList MLogicAnalyzer::applyConStraight(const MPosition &pos, const mCellList &moveList, QPoint from) const
{
    int xMax = 7;
    int xMin = 0;
    int yMax = 7;
    int yMin = 0;

    MPiece *piece = pos.pieceAt(from);

    mCellList newMoveList;
    MPiece::MColour currColour = MPiece::WHITE;
    if (piece)
    {
        currColour = piece->getColour();
    }

    for(mCellList::const_iterator iter = moveList.begin();
        iter != moveList.end();
        ++iter)
    {
        QPoint cell = *iter;
        MPiece *currPiece = pos.pieceAt(cell);

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

    for(mCellList::const_iterator iter = moveList.begin();
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

MLogicAnalyzer::mCellList MLogicAnalyzer::applyConDiagonal(const MPosition &pos, const mCellList &moveList, QPoint from) const
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

    mCellList newMoveList;
    MPiece::MColour currColour = MPiece::WHITE;
    if (piece)
    {
        currColour = piece->getColour();
    }

    for(mCellList::const_iterator iter = moveList.begin();
        iter != moveList.end();
        ++iter)
    {
        QPoint cell = *iter;
        MPiece *currPiece = pos.pieceAt(cell);

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

MLogicAnalyzer::mCellList MLogicAnalyzer::applyConKing(const MPosition &pos, const mCellList &moveList, QPoint from) const
{
    MPiece *piece = pos.pieceAt(from);
    mCellList newMoveList;

    for(mCellList::const_iterator iter = moveList.begin();
        iter != moveList.end();
        ++iter)
    {
        QPoint cell = *iter;
        MPiece *currPiece = pos.pieceAt(cell);

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
            newMoveList.append(cell);
        }
    }

    return newMoveList;
}

MLogicAnalyzer::mCellList MLogicAnalyzer::applyConKingCastle(const MPosition &pos,
                                                             const mCellList &moveList,
                                                             const QPoint &from,
                                                             mMoveFlags &flags) const
{
    MPiece *piece = pos.pieceAt(from);
    MPiece::MColour currColour = MPiece::WHITE;
    if (piece)
    {
       currColour = piece->getColour();
    }
    mCellList newMoveList;

    for(mCellList::const_iterator iter = moveList.begin();
        iter != moveList.end();
        ++iter)
    {
        QPoint cell = *iter;
        MPiece *currPiece = pos.pieceAt(cell);

        if (from.y() == cell.y())
        {
            // kingside
            if (from.x() == cell.x() - 2)
            {
                MPiece *cellInBetween = pos.pieceAt(QPoint(cell.x() - 1, cell.y()));
                if (!currPiece && !cellInBetween)
                {
                    if (currColour == MPiece::WHITE)
                    {
                        std::cout << "white" << std::endl;
                        if (pos.canWhiteCastleKingside())
                        {
                            flags |= CASTLE_KINGSIDE_ALLOWED;
                            std::cout << "castle kingside" << std::endl;
                            newMoveList.append(cell);
                        }
                    }
                    else if (currColour == MPiece::BLACK)
                    {
                        std::cout << "black" << std::endl;
                        if (pos.canBlackCastleKingside())
                        {
                            flags |= CASTLE_KINGSIDE_ALLOWED;
                            std::cout << "castle kingside" << std::endl;
                            newMoveList.append(cell);
                        }
                    }
                }
            }
            // queenside
            else if (from.x() == cell.x() + 2)
            {
                MPiece *cellInBetween = pos.pieceAt(QPoint(cell.x() + 1, cell.y()));
                MPiece *cellNextToRook = pos.pieceAt(QPoint(cell.x() - 1, cell.y()));
                if (!cellNextToRook && !currPiece && !cellInBetween)
                {
                    if (currColour == MPiece::WHITE)
                    {
                        std::cout << "white" << std::endl;
                        if (pos.canWhiteCastleQueenside())
                        {
                            flags |= CASTLE_QUEENSIDE_ALLOWED;
                            std::cout << "castle queenside" << std::endl;
                            newMoveList.append(cell);
                        }
                    }
                    else if (currColour == MPiece::BLACK)
                    {
                        std::cout << "black" << std::endl;
                        if (pos.canBlackCastleQueenside())
                        {
                            flags |= CASTLE_QUEENSIDE_ALLOWED;
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

MLogicAnalyzer::mCellList MLogicAnalyzer::applyConPawnBaseline(const MPosition &pos, const mCellList &moveList, QPoint from) const
{
    MPiece *piece = pos.pieceAt(from);

    mCellList newMoveList;
    MPiece::MColour currColour = MPiece::WHITE;
    if (piece)
    {
        currColour = piece->getColour();
    }

    for(mCellList::const_iterator iter = moveList.begin();
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


MLogicAnalyzer::mCellList MLogicAnalyzer::applyConPawnObstacle(const MPosition &pos, const mCellList &moveList, QPoint from) const
{
    // moveList is supposed to be sorted in a way that the one-step move comes before the two-step move if there's any.
    bool obstacle = false;

    mCellList newMoveList;

    for(mCellList::const_iterator iter = moveList.begin();
        iter != moveList.end();
        ++iter)
    {
        QPoint cell = *iter;
        MPiece *currPiece = pos.pieceAt(cell);

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

MLogicAnalyzer::mCellList MLogicAnalyzer::applyConPawnCapture(const MPosition &pos, const mCellList &moveList, const QPoint &from, mMoveFlags &flags) const
{
    MPiece *piece = pos.pieceAt(from);

    mCellList newMoveList;
    MPiece::MColour currColour = MPiece::WHITE;
    if (piece)
    {
        currColour = piece->getColour();
    }

    const QPoint enPassentCapture = pos.getPawnDoubleMove();

    for(mCellList::const_iterator iter = moveList.begin();
        iter != moveList.end();
        ++iter)
    {
        QPoint cell = *iter;
        MPiece *currPiece = pos.pieceAt(cell);

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

    // allow capturing "en-passant"
    if ((enPassentCapture.y() == from.y()) &&
        (enPassentCapture.x() == (from.x() + 1) || enPassentCapture.x() == (from.x() - 1)))
    {
        MPiece *enPassentPawn = pos.pieceAt(enPassentCapture);
        if (enPassentPawn && currColour != enPassentPawn->getColour())
        {
            newMoveList.append(QPoint(enPassentCapture.x(),
                                      enPassentCapture.y() + ((MPiece::WHITE == currColour) ? -1 : 1)));
            flags |= EN_PASSANT_ALLOWED;
        }
    }

    return newMoveList;
}

}
