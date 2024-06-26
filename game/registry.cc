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

#include "registry.h"
#include "game.h"
#include "namespace.h"
#include "side.h"
#include "frontend/miniature.h"
#include "commands.h"

namespace Game {

Registry::Registry(QObject *parent)
    : QObject(parent)
    , m_games()
{}

Registry::~Registry()
{}

void Registry::setDispatcher(Dispatcher *dispatcher)
{
    m_dispatcher = WeakDispatcher(dispatcher);
}

void Registry::registerGame(Game *game)
{
    if (not game || m_games.contains(game)) {
        return;
    }

    m_games.append(game);

    // TODO: Who gets to decide about game activation?
    Command::ActivateGame ag(TargetFrontend, game);
    if (Dispatcher *dispatcher = m_dispatcher.data()) {
        dispatcher->sendCommand(&ag);
    }
}

void Registry::unregisterGame(Game *game)
{
    if (not game) {
        return;
    }

    int index = m_games.indexOf(game);
    while (-1 != index) {
        m_games.remove(index);
        index = m_games.indexOf(game);
    }
}

Game * Registry::game(uint id) const
{
    foreach(Game *g, m_games) {
        if (g->id() == id) {
            return g;
        }
    }

    return 0;
}

bool Registry::isRegisteredGame(uint id) const
{
    return (game(id) != 0);
}

} // namespace Game
