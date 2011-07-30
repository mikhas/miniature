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

#include "gamemanager.h"

#include <game.h>
#include <localside.h> // TODO: Have a side factory to prevent this class from depending on those headers
#include <gnuchess.h> // TODO: see above
#include <commands/logincommand.h>
#include <directinputdevice.h>
#include <iostream>

using Game::Command;

namespace Miniature {

GameManager::GameManager(QObject *parent)
    : QObject(parent)
    , m_games()
    , m_dispatcher(new Game::Dispatcher)
    , m_frontend(m_dispatcher.data())
{
    m_dispatcher->setFrontend(&m_frontend);
    m_frontend.show();
}

GameManager::~GameManager()
{}

void GameManager::startGame(GameMode mode)
{
    Game::Game *game = 0;

    switch(mode) {
    case GameModeLocalEngine:
        game = createLocalEngineGame();
        break;

    case GameModeRemoteFics:
        game = createRemoteFicsGame();
        break;

    default:
        break;
    }

    // Suspend sides in other game instances first:
    // TODO: provide activation API (for suspended games).
    foreach(QPointer<Game::Game> current, m_games) {
        if (not current) {
            continue;
        }

        current->side(Game::SideLocal)->runInBackground();
        current->side(Game::SideRemote)->runInBackground();
    }

    game->start();
    m_games.append(QPointer<Game::Game>(game));
}

Game::Game *GameManager::createLocalEngineGame()
{
    return new Game::Game(0, 0, this);
}

Game::Game *GameManager::createRemoteFicsGame()
{
    return new Game::Game(0, 0, this);
}

} // namespace Miniature
