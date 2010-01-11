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

#ifndef LOGIC_ANALYZER_H__
#define LOGIC_ANALYZER_H__

#include "position.h"

namespace Miniature
{

typedef int MConstraintList;

/* This class performs logical checks on a given position and move.
 * Currently, it only validates simple moves.
 */
class MLogicAnalyzer
{
public:
    typedef QList<QPoint> mCellList;
    typedef QListIterator<QPoint> mCellListIter;

    enum mMoveState
    {
        INVALID_MOVE             = 0x000,
        VALID_MOVE               = 0x001,
        CAPTURE                  = 0x002,
        PROMOTION                = 0x004,
        CASTLE_KINGSIDE          = 0x008,
        CASTLE_KINGSIDE_ALLOWED  = 0x010,
        CASTLE_QUEENSIDE         = 0x020,
        CASTLE_QUEENSIDE_ALLOWED = 0x040,
        PAWN_DOUBLEMOVE          = 0x080,
        EN_PASSANT_ALLOWED       = 0x100
    };
    typedef QFlags<mMoveState> mMoveFlags;

    enum mPositionState
    {
        INVALID_POSITION = 0x000,
        VALID_POSITION   = 0x001,
        IN_CHECK         = 0x002,
        IN_CHECKMATE     = 0x004,
        IN_STALEMATE     = 0x008
    };
    typedef QFlags<mPositionState> mPositionFlags;

    MLogicAnalyzer(const MConstraintList &constraints);
    virtual ~MLogicAnalyzer();

    mMoveFlags verifyMove(MPosition * const position, const QPoint &origin, const QPoint &target) const;
    mPositionFlags verifyPosition(MPosition * const position) const;

private:
    MConstraintList m_constraints;

    bool cellUnderAttack(const MPosition &position, const QPoint &origin, MPiece::MColour colour) const;
    bool isPieceInList(const MPosition &position, const MPiece& piece, const mCellList &list) const;
    bool moveResultsInCheck(const MPosition &position, const QPoint &origin, const QPoint &target) const;
    bool nextPlayerInCheck(const MPosition &position, const QPoint &origin, const QPoint &target) const;
    bool inCheck(const MPosition &position) const;
    bool moveCheckUndo(const MPosition &position, const QPoint &origin, const QPoint &target, MPiece::MColour colour) const;

    mCellList applyConStraight(const MPosition &pos, const mCellList &moveList, QPoint from) const;
    mCellList applyConDiagonal(const MPosition &pos, const mCellList &moveList, QPoint from) const;
    mCellList applyConKnight(const MPosition &pos, const mCellList &moveList, QPoint from) const;
    mCellList applyConKing(const MPosition &pos, const mCellList &moveList, QPoint from) const;
    mCellList applyConKingCastle(const MPosition &pos, const mCellList &moveList, const QPoint &from, mMoveFlags &flags) const;
    mCellList applyConPawnBaseline(const MPosition &pos, const mCellList &moveList, QPoint from) const;
    mCellList applyConPawnObstacle(const MPosition &pos, const mCellList &moveList, QPoint from) const;
    mCellList applyConPawnCapture(const MPosition &pos, const mCellList &moveList, const QPoint &from, mMoveFlags &flags) const;
};

}

#endif // LOGIC_ANALYZER_H__
