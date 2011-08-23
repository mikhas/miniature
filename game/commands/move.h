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

#ifndef MOVECOMMAND_H
#define MOVECOMMAND_H

#include "abstractcommand.h"
#include "abstractbackend.h" // FIXME: PlayerRecord

#include "position.h"
#include "square.h"

#include <QtCore>

namespace Game {

class AbstractBackend;
class Game;
class Frontend;

namespace Command {

//! Command to move a piece.
class Move
    : public AbstractCommand
{
private:
    const Target m_target;
    const uint m_game_id;
    const Position m_result;
    const MovedPiece m_moved_piece;
    PlayerRecord m_white;
    PlayerRecord m_black;

public:
    //! \reimp
    //! C'tor
    //! @param result the resulting position.
    //! @param moved_piece the moved piece, consisting of origin and target
    //!        square.
    explicit Move(Target target,
                  uint game_id,
                  const Position &result,
                  const MovedPiece &moved_piece);

    virtual ~Move();
    virtual Target target() const;
    virtual bool exec(Game *target);
    virtual bool exec(AbstractBackend *target);
    virtual bool exec(Frontend *target);
    //! \reimp_end

    uint gameId() const;
    Position result() const;
    MovedPiece movedPiece() const;

    PlayerRecord white() const;
    void setWhite(const PlayerRecord &player_record);

    PlayerRecord black() const;
    void setBlack(const PlayerRecord &player_record);
};

}} // namespace Command, Game

#endif // PLAYCOMMAND_H
