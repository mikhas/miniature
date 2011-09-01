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
#include "dispatcher.h"

namespace Game { namespace Command {

CreateGame::CreateGame(Target target,
                       uint id,
                       const QByteArray &local_id,
                       const QByteArray &remote_id,
                       LocalSideColor color)
    : AbstractCommand(target)
    , m_game_id(id)
    , m_local_id(local_id)
    , m_remote_id(remote_id)
    , m_color(color)
{}

CreateGame::~CreateGame()
{}

void CreateGame::exec(Dispatcher *dispatcher,
                      Registry *target)
{
    if (not dispatcher || not target) {
        return;
    }

    Game *game = createGame(m_game_id, dispatcher, m_local_id, m_remote_id);
    game->setLocalSideColor(m_color);
    target->registerGame(game);
}

}} // namespace Command, Game
