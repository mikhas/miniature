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

#include "activategame.h"
#include "registry.h"
#include "frontend/miniature.h"

namespace Game { namespace Command {

ActivateGame::ActivateGame(Target target,
                           Game *game)
    : AbstractCommand(target)
    , m_game(game)
{}

ActivateGame::~ActivateGame()
{}

void ActivateGame::exec(Dispatcher *,
                        Frontend::Miniature *target)
{
    if (not target) {
        return;
    }

    target->setActiveGame(m_game.data());
    emit target->gameStarted();
}

}} // namespace Command, Game

