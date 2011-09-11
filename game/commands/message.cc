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

#include "message.h"
#include "abstractengine.h"
#include "frontend/miniature.h"
#include "frontend/messagelog.h"

namespace Game { namespace Command {

Message::Message(Target target,
                 const QByteArray &player_name,
                 const QByteArray &message)
    : AbstractCommand(target)
    , m_player_name(player_name)
    , m_message(message)
{}

Message::~Message()
{}

void Message::exec(Dispatcher *,
                   AbstractEngine *target)
{
    if (not target) {
        return;
    }

    target->sendMessage(m_player_name, m_message);
}

void Message::exec(Dispatcher *,
                   Frontend::Miniature *target)
{
    if (not target) {
        return;
    }

    if (Frontend::MessageLog *log = target->messageLog()) {
        log->append(m_player_name, m_message);
    }
}

}} // namespace Command, Game
