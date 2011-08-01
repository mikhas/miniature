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

#include "logincommand.h"
#include "abstractbackend.h"

namespace Game {

LoginCommand::LoginCommand(Target target,
                           const QString &username,
                           const QString &password)
    : AbstractCommand(target)
    , m_target(target)
    , m_username(username)
    , m_password(password)
{}

LoginCommand::~LoginCommand()
{}

Target LoginCommand::target() const
{
    return m_target;
}

bool LoginCommand::exec(AbstractBackend *target)
{
    if (not target) {
        return false;
    }

    target->login(m_username, m_password);
    return true;
}

} // namespace Game
