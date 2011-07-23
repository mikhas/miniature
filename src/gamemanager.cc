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
#include <iostream>

using Game::Command;

namespace {
    const Game::CommandFlags main_commands(Game::CommandNew | Game::CommandQuit
                                           | Game::CommandLogin | Game::CommandSeek
                                           | Game::CommandJoin | Game::CommandObserve);
}

namespace Miniature {

GameManager::GameManager(QObject *parent)
    : QObject(parent)
    , m_games()
    , m_tokenizer(Game::createLocalInputTokenizer())
    , m_parser(main_commands, m_tokenizer)
    , m_fics_link()
{
    connect(&m_parser, SIGNAL(commandFound(Command, QString)),
            this,      SLOT(onCommandFound(Command, QString)));

    m_parser.setEnabled(true);
    std::cout << "Welcome to Miniature!" << std::endl;
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

    game->start();
    m_games.append(QPointer<Game::Game>(game));
}

Game::Game *GameManager::createLocalEngineGame()
{
    static Game::LocalParser local_parser(Game::CommandFlags(Game::CommandMove),
                                          m_tokenizer);
    local_parser.setEnabled(true);

    Game::LocalSide *local = new Game::LocalSide("white");
    Game::GnuChess *remote = new Game::GnuChess("black");

    connect(&local_parser, SIGNAL(commandFound(Command, QString)),
            local,         SLOT(onCommandFound(Command, QString)));

    return new Game::Game(local, remote, this);
}

Game::Game *GameManager::createRemoteFicsGame()
{
    static Game::LocalParser local_parser(Game::CommandFlags(Game::CommandMove),
                                          m_tokenizer);
    local_parser.setEnabled(true);

    Game::LocalSide *local = new Game::LocalSide("white");
    Game::FicsSide *remote = new Game::FicsSide("FICS", m_fics_link.data());

    connect(&local_parser, SIGNAL(commandFound(Command, QString)),
            local,         SLOT(onCommandFound(Command, QString)));

    return new Game::Game(local, remote, this);
}

void GameManager::onCommandFound(Command command,
                                 const QString &data)
{
    Q_UNUSED(data)

    switch(command) {
    case Game::CommandLogin:
        m_fics_link = QSharedPointer<Game::FicsLink>(new Game::FicsLink);
        m_fics_link->login("guest", "");
        break;

    case Game::CommandJoin:
        startGame(GameModeRemoteFics);
        break;

    case Game::CommandNew:
        startGame(GameModeLocalEngine);
        break;

    case Game::CommandQuit:
        qApp->exit();
        break;

    default:
        break;
    }
}

} // namespace Miniature
