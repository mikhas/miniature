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

namespace Game {

LocalSide::LocalSide(const QString &identifier)
    : AbstractSide(identifier)
    , m_identifier(identifier)
    , m_state(NotReady)
#ifdef MINIATURE_CLI_ENABLED
    , m_parser(CommandFlags(CommandMove))
#endif
{
    connect(&m_parser, SIGNAL(commandFound(Command,QString)),
            this,      SLOT(onCommandFound(Command,QString)));
}

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

void LocalSide::startMove(const Move &move)
{
    if (not move.notation.isEmpty()) {
        static QTextStream out(stdout);
        out << move.notation;
        out.flush();
    }

    m_parser.readInput();
}

void LocalSide::onCommandFound(Command command,
                               const QString &data)
{
    switch(command) {
    case CommandQuit:
        qApp->exit();
        break;

    case CommandMove:
        emit moveEnded(Move(Position(), Square(), Square(), data));
        break;

    default:
        break;
    }
}

} // namespace Game
