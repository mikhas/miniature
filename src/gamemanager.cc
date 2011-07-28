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

namespace {
    const Game::CommandFlags game_manager_commands(Game::CommandNew | Game::CommandQuit
                                                   | Game::CommandLogin | Game::CommandSeek
                                                   | Game::CommandJoin | Game::CommandObserve);
    const Game::CommandFlags local_side_commands(Game::CommandMove);
}

namespace Miniature {

GameManager::GameManager(QObject *parent)
    : QObject(parent)
    , m_games()
    , m_commandline(new Game::CommandLine)
    , m_local_side_commandline(new Game::CommandLine)
    , m_fics_link()
    , m_dispatcher(new Game::Dispatcher)
{
    m_commandline->setFlags(game_manager_commands);
    m_local_side_commandline->setFlags(local_side_commands);

    Game::LineReader tokenizer;
    connect(&tokenizer,           SIGNAL(tokenFound(QByteArray)),
            m_commandline.data(), SLOT(processToken(QByteArray)));

    connect(&tokenizer,                      SIGNAL(tokenFound(QByteArray)),
            m_local_side_commandline.data(), SLOT(processToken(QByteArray)));

    connect(m_commandline.data(), SIGNAL(commandFound(Command, QByteArray)),
            this,                 SLOT(onCommandFound(Command, QByteArray)));

    m_commandline->setEnabled(true);
    m_local_side_commandline->setEnabled(true);
    tokenizer.init(new Game::DirectInputDevice);
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
    Game::LocalSide *local = new Game::LocalSide("white", m_local_side_commandline);
    Game::GnuChess *remote = new Game::GnuChess("black");

    return new Game::Game(local, remote, this);
}

Game::Game *GameManager::createRemoteFicsGame()
{
    Game::LocalSide *local = new Game::LocalSide("white", m_local_side_commandline);
    Game::FicsSide *remote = new Game::FicsSide("FICS", m_fics_link);

    return new Game::Game(local, remote, this);
}

void GameManager::onCommandFound(Command command,
                                 const QByteArray &data)
{
    Q_UNUSED(data)

    switch(command) {
    case Game::CommandLogin: {
        QList<QByteArray> list = data.split(' ');
        const QString username(data.isEmpty() ? "guest" : list.at(0));
        const QString password(list.size() > 1 ? list.at(1) : "");

        Game::LoginCommand login(Game::TargetServer, username, password);
        m_dispatcher->sendCommand(&login);
    } break;

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
