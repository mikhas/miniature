/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2011 Michael Hasselmann <michael@taschenorakel.de>
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

#ifndef SIDE_H
#define SIDE_H

#include "namespace.h"
#include "abstractbackend.h"
#include "position.h"
#include "square.h"

#include <QtCore>

namespace Game {

class Side;
typedef QWeakPointer<Side> WeakSide;

class SidePrivate;

//! Represents a game side. Updated by Game instance.
class Side
    : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Side)
    Q_DECLARE_PRIVATE(Side)

private:
    const QScopedPointer<SidePrivate> d_ptr;

public:
    //! C'tor
    //! @param identifier the identifier for this side.
    explicit Side(const QString &identifier);

    //! D'tor
    virtual ~Side();

    //! Returns identifier for this side.
    virtual QString identifier() const;

    //! Emitted when a move has been submitted. If it is was this side's turn
    //! and the submitted move is invalid, then startMove will be called again
    //! (as if the turn had just started). Otherwise incorrectly sent moves
    //! will be silently discarded.
    //! @param result the resulting position, after applying moved_piece.
    //! @param moved_piece the last moved piece, leading to result.
    Q_SIGNAL void turnEnded(const Position &result,
                            const MovedPiece &moved_piece);

    //! Starts new turn for this side. If move is the same as last move, then
    //! the last submitted move was invalid.
    //! @param result the resulting position, after applying moved_piece.
    //! @param moved_piece the last moved piece, leading to result.
    virtual void startTurn(const Position &result,
                           const MovedPiece &moved_piece);

    //! Moves a piece, usually based on position from startTurn.
    //! @returns whether move was valid.
    //! @param moved_piece the moved piece.
    //! @param manual_promotion only relevant if pawn is moved and reaches
    //!        final rank. Replaces pawn with auto promoted piece if set to
    //!        PromotionNone (default).
    virtual bool move(const MovedPiece &moved_piece,
                      Promotion manual_promotion = PromotionNone);

    //! Confirms last valid move.
    virtual void confirmMove();

    //! Sets auto promotion for any pawn that reaches final rank. Initial value
    //! should be PromotionQueen by default.
    virtual void setAutoPromotion(Promotion promotion);

    //! Returns position from the side's point of view.
    virtual Position position() const;

};

} // namespace GAME

#endif // SIDE_H
