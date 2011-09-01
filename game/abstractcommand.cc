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

#include "abstractcommand.h"
#include "frontend/miniature.h"

namespace Game {

AbstractCommand::AbstractCommand(Target target)
    : m_target(target)
{}

AbstractCommand::~AbstractCommand()
{}

Target AbstractCommand::target() const
{
    return m_target;
}

bool AbstractCommand::valid() const
{
    return (m_target != TargetNone);
}

void AbstractCommand::exec(Dispatcher *,
                           Registry *)
{}

void AbstractCommand::exec(Dispatcher *,
                           AbstractEngine *)
{}

void AbstractCommand::exec(Dispatcher *,
                           Frontend::Miniature *)
{}

} // namespace Game
