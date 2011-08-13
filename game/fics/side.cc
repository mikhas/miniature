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

#include "fics/side.h"
#include "move.h"

namespace Game { namespace Fics {

Side::Side(const QString &identifier,
           const SharedBackend &link)
    : AbstractSide(identifier, link)
    , m_identifier(identifier)
    , m_state(NotReady)
    , m_link(link)
{}

Side::~Side()
{}

void Side::init()
{
    m_state = Ready;
    emit ready();
}

AbstractSide::SideState Side::state() const
{
    return m_state;
}

const QString &Side::identifier() const
{
    return m_identifier;
}

void Side::runInBackground()
{
    if (m_state == NotReady) {
        return;
    }

    m_state = RunInBackground;
    disconnect(m_link.data(), SIGNAL(commandFound(ParserCommand, QByteArray)),
               this,          SLOT(onCommandFound(ParserCommand, QByteArray)));
}

void Side::runInForeground()
{
    if (m_state == RunInBackground) {
        m_state = Ready;
        connect(m_link.data(), SIGNAL(commandFound(ParserCommand, QByteArray)),
                this,          SLOT(onCommandFound(ParserCommand, QByteArray)),
                Qt::UniqueConnection);
    }
}

void Side::startTurn(const Move &move)
{
    Q_UNUSED(move)
}

void Side::onCommandFound(ParserCommand command,
                          const QByteArray &data)
{
    Q_UNUSED(command)
    Q_UNUSED(data)
}

} // namespace Fics
} // namespace Game
