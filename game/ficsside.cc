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

#include "ficsside.h"
#include "move.h"

namespace Game {

FicsSide::FicsSide(const QString &identifier)
    : AbstractSide(identifier)
    , m_identifier(identifier)
    , m_state(NotReady)
{}

FicsSide::~FicsSide()
{}

const QString &FicsSide::identifier() const
{
    return m_identifier;
}

void FicsSide::init()
{
    // TODO: Login to FICS
    m_state = Ready;
    emit ready();
}

AbstractSide::SideState FicsSide::state() const
{
    return m_state;
}

void FicsSide::startMove(const Move &move)
{
    Q_UNUSED(move)
}

void FicsSide::onCommandFound(Command command,
                               const QString &data)
{
    Q_UNUSED(command)
    Q_UNUSED(data)
}

} // namespace Game
