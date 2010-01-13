/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2010 Michael Hasselmann <michael@taschenorakel.de>
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

#ifndef HALF_MOVE_H__
#define HALF_MOVE_H__

#include "position.h"
#include "logic_analyzer.h"

namespace Miniature
{

/** This class represents the delta between two MPositions, p and p', whereas
  * p' can be obtained by applying an instance of mHalfMove on p.  mHalfMove
  * guarantees that p' is the result of a valid transition.
  *
  * However, the main use case of this class is to provide UNDO of non-trivial
  * moves (capturing, en-passant, castling, promotions, moves from/to checked
  * positions).
  *
  * Rationale: In the initial design, UNDO was provided by simply rendering p
  * again (iterating over all cells & pieces on the board), which was a clean
  * UNDO. Unfortunately, for whatever reason (slow iteration over
  * QGraphicsItem::childItems() due to value-type lists and addtional dynamic
  * type casting?), this mechanism wasn't fast enough. There were
  * user-noticable delays on the N900 when selecting a piece in an attempt to
  * move it (roughly 250ms). Hence the decision to only update the delta, but
  * this came with the cost to add proper book-keeping of the state transition
  * from p to p', so that p' could be reversed into p. Especially capturing of
  * pieces (capture, en-passant, promotion) became hackish to undo, so this
  * class' intent is to encapsulate that.
  */
class mHalfMove
{

public:
    explicit mHalfMove(const MPosition &position);
    explicit mHalfMove(const MPosition &position, const QPoint &origin, const QPoint &target);
    virtual ~mHalfMove();

    const MPosition& getPosition() const;

    bool select(const QPoint& origin);
    void deSelect();
    bool applyToTarget(const QPoint &target);
    bool apply();
    void undo();

    // Checks integrity of the mHalfMove instance, *not* the stored move!
    bool isValid() const;
    bool isValidOrigin() const;
    bool isValidTarget() const;
    // Checks whether the piece at target was already selected.
    bool isSelected(MPiece *piece) const;


private:
    /* The current position, which can be: a) the one given in the ctor, b) the
     * one after applying the move, c) the one after undoing the move (equal to
     * position in a)) */
    MPosition m_position;

    /* The current logic in MPosition does not allow clean rollbacks, that is
     * why we remember the original position, too. I don't like it either. */
    MPosition m_position_origin;

    /* We need to remember origin and target of a move */
    QPoint m_origin;
    QPoint m_target;

    /* We need to remember the last captured piece during a turn so we can redraw it on undo. */
    // TODO: check whether auto_ptr would work here.
    MPiece *m_captured_piece;

    /* We need to remember new pieces created by promotions. */
    // TODO: check whether auto_ptr would work here.
    MPiece *m_promotion;

    /* We need to remember the currently selected piece on the board. */
    // TODO: check whether auto_ptr would work here.
    MPiece *m_selected_piece;

    /* We need to remember the rook's position after a castling, else we cannot undo */
    // TODO: check whether auto_ptr would work here.
    MPiece *m_rook;
    QPoint m_rook_origin;

    /* We need to remember en-passant captures separately from normal captures. */
    // TODO: check whether auto_ptr would work here.
    MPiece *m_en_passant;

    /* The logic analyzer validates moves and positions. */
    MLogicAnalyzer m_logic_analyzer;
};

} // namespace Miniature

#endif
