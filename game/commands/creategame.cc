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
                       const GameInfo &game_info,
                       LocalSideColor color)
    : AbstractCommand(target)
    , m_game_info(game_info)
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

    Game *game = createGame(m_game_info.id, dispatcher,
                            (m_color == LocalSideIsWhite ? m_game_info.white.name
                                                         : m_game_info.black.name),
                            (m_color == LocalSideIsBlack ? m_game_info.white.name
                                                         : m_game_info.black.name));

    game->setLocalSideColor(m_color);
    game->setTime(m_game_info.time);
    game->setTimeIncrement(m_game_info.time_increment);
    game->setRating(m_game_info.rating);
    target->registerGame(game);
}

}} // namespace Command, Game
