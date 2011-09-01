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

#include "invalidmove.h"
#include "commands/move.h"
#include "frontend/miniature.h"
#include "dispatcher.h"

namespace Game { namespace Command {

InvalidMove::InvalidMove(Target t,
                         uint id,
                         const QByteArray &move)
    : AbstractCommand(t)
    , m_target(t)
    , m_game_id(id)
    , m_move(move)
{}

InvalidMove::~InvalidMove()
{}

Target InvalidMove::target() const
{
    return m_target;
}

void InvalidMove::exec(Dispatcher *dispatcher,
                       Frontend::Miniature *target)
{
    if (not dispatcher || not target) {
        return;
    }

    qDebug() << __PRETTY_FUNCTION__;
    emit target->invalidMove(QString(m_move));

    (void) dispatcher->popMove(m_game_id); // the invalid move
    Command::Move m(dispatcher->popMove(m_game_id)); // the last valid move
    dispatcher->sendCommand(&m);
}

}} // namespace Command, Game

