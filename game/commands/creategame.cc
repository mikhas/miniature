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

#include "creategame.h"
#include "registry.h"
#include "game.h"

namespace Game { namespace Command {

CreateGame::CreateGame(Target t,
                       uint id,
                       const WeakDispatcher &dispatcher,
                       const QByteArray &local_id,
                       const QByteArray &remote_id,
                       LocalSideColor color)
    : AbstractCommand(t)
    , m_target(t)
    , m_game_id(id)
    , m_dispatcher(dispatcher)
    , m_local_id(local_id)
    , m_remote_id(remote_id)
    , m_color(color)
{}

CreateGame::~CreateGame()
{}

Target CreateGame::target() const
{
    return m_target;
}

void CreateGame::exec(Registry *target)
{
    // TODO: Dont register game on frontend? Target should be game registry, use new command to inform frontend and backend about sides?
    Game *game = createGame(m_game_id, m_dispatcher.data(), m_local_id, m_remote_id);
    game->setPosition(createStartPosition());
    game->setLocalSideColor(m_color);
    target->registerGame(game);
}

}} // namespace Command, Game

