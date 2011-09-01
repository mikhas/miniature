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
#include "abstractengine.h"// FIXME: PlayerRecord

#include "position.h"
#include "square.h"

#include <QtCore>

namespace Game { namespace Command {

//! Command to move a piece.
class Move
    : public AbstractCommand
{
private:
    uint m_game_id;
    Position m_result;
    MovedPiece m_moved_piece;
    Side m_white;
    Side m_black;

public:
    //! \reimp
    Move();

    //! C'tor
    //! @param game_id the game id.
    //! @param result the resulting position.
    explicit Move(Target target,
                  uint game_id,
                  const Position &result);

    virtual ~Move();
    virtual void exec(Dispatcher *dispatcher,
                      AbstractEngine *target);
    virtual void exec(Dispatcher *dispatcher,
                      Frontend::Miniature *target);
    //! \reimp_end

    void setWhite(const Side &player_record);
    void setBlack(const Side &player_record);
    bool valid();
};

}} // namespace Command, Game

#endif // PLAYCOMMAND_H
