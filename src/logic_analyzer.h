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
    enum MState
    {
        INVALID   = 0x000,
        VALID     = 0x001,
        CHECK     = 0x002,
        CHECKMATE = 0x004,
        STALEMATE = 0x008,
        CAPTURE   = 0x010,
        PROMOTION = 0x020
    };
    typedef QFlags<MState> MStateFlags;

    MLogicAnalyzer(const MConstraintList &constraints);
    virtual ~MLogicAnalyzer();

    MStateFlags verifyMove(MPosition * const position, const QPoint &origin, const QPoint &target);

private:
    MConstraintList m_constraints;

    bool cellUnderAttack(const MPosition &pos, QPoint cell, MPiece::MColour colour) const;
    bool moveResultsInCheck(MPosition &pos, QPoint from, QPoint to);
    bool nextPlayerInCheck(MPosition &pos, QPoint from, QPoint to);
    bool moveCheckUndo(MPosition &pos, QPoint from, QPoint to, MPiece::MColour colour);
    QList<QPoint> applyConStraight(const MPosition &pos, const QList<QPoint> &moveList, QPoint from) const;
    QList<QPoint> applyConDiagonal(const MPosition &pos, const QList<QPoint> &moveList, QPoint from) const;
    QList<QPoint> applyConKnight(const MPosition &pos, const QList<QPoint> &moveList, QPoint from) const;
    QList<QPoint> applyConKing(const MPosition &pos, const QList<QPoint> &moveList, QPoint from) const;
    QList<QPoint> applyConKingCastle(const MPosition &pos, const QList<QPoint> &moveList, QPoint from) const;
    QList<QPoint> applyConPawnBaseline(const MPosition &pos, const QList<QPoint> &moveList, QPoint from) const;
    QList<QPoint> applyConPawnObstacle(const MPosition &pos, const QList<QPoint> &moveList, QPoint from) const;
    QList<QPoint> applyConPawnCapture(const MPosition &pos, const QList<QPoint> &moveList, QPoint from) const;
};

}

#endif // LOGIC_ANALYZER_H__
