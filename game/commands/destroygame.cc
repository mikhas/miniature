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

#include "destroygame.h"
#include "registry.h"
#include "game.h"

namespace Game { namespace Command {

DestroyGame::DestroyGame(Target target,
                         uint game_id)
    : AbstractCommand(target)
    , m_game_id(game_id)
{}

DestroyGame::~DestroyGame()
{}

void DestroyGame::exec(Dispatcher *,
                       Registry *target)
{
    if (not target) {
        return;
    }

    if (Game *g = target->game(m_game_id)) {
        target->unregisterGame(g);
        delete g;
    }
}

}} // namespace Command, Game
