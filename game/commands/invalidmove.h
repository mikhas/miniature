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

#ifndef INVALIDMOVE_H
#define INVALIDMOVE_H

#include "abstractcommand.h"
#include "dispatcher.h"

#include <QtCore>

namespace Game { namespace Frontend {
class Miniature;
}

namespace Command {

//! Command to start a game. Takes an (optional) advertisement id, in case this
//! is a reponse to a previous game advertisement.
class InvalidMove
    : public AbstractCommand
{
private:
    Target m_target;
    uint m_game_id;
    QByteArray m_move;

public:
    //! \reimp
    explicit InvalidMove(Target t,
                  uint id,
                  const QByteArray &move);
    virtual ~InvalidMove();
    virtual Target target() const;
    virtual void exec(Dispatcher *dispatcher,
                      Frontend::Miniature *target);
    //! \reimp_end
};

}} // namespace Command, Game

#endif // INVALIDMOVE_H
