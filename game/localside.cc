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

#include "localside.h"
#include "move.h"

namespace Game {

LocalSide::LocalSide(const QString &identifier)
    : AbstractSide(identifier)
    , m_identifier(identifier)
    , m_state(NotReady)
{}

LocalSide::~LocalSide()
{}

const QString &LocalSide::identifier() const
{
    return m_identifier;
}

void LocalSide::init()
{
    // Nothing to do, just report readiness:
    m_state = Ready;
    emit ready();
}

AbstractSide::SideState LocalSide::state() const
{
    return m_state;
}

void LocalSide::startTurn(const Move &move)
{
    if (not move.notation.isEmpty()) {
        static QTextStream out(stdout);
        out << move.notation;
        out.flush();
    }
}

void LocalSide::onCommandFound(Command command,
                               const QByteArray &data)
{
    switch(command) {
    case CommandQuit:
        qApp->exit();
        break;

    case CommandMove:
        emit turnEnded(Move(Position(), Square(), Square(), QString(data)));
        break;

    default:
        break;
    }
}

} // namespace Game
