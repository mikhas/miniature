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
#include "game.h"
#include "localside.h" // TODO: Have a side factory to prevent this class from depending on those headers
#include "gnuchess.h" // TODO: see above
#include "linereader.h"
#include "directinputdevice.h"
#include "localparser.h"

using Game::Command;

namespace Miniature {

GameManager::GameManager(QObject *parent)
    : QObject(parent)
{}

GameManager::~GameManager()
{}

void GameManager::startGame(GameMode mode)
{
    switch(mode) {
    case GameModeLocalEngine:
        m_games.append(QPointer<Game::Game>(createLocalEngineGame()));
        break;

    default:
        break;
    }
}

Game::Game *GameManager::createLocalEngineGame()
{
    Game::SharedTokenizer tokenizer(new Game::LineReader(new Game::DirectInputDevice)); // TODO: hide dependencies behind factory!
    static Game::LocalParser game_parser(Game::CommandFlags(Game::CommandNew | Game::CommandQuit),
                                         tokenizer);
    static Game::LocalParser local_parser(Game::CommandFlags(Game::CommandMove),
                                          tokenizer);

    Game::LocalSide *local = new Game::LocalSide("white");
    Game::GnuChess *remote = new Game::GnuChess("black");
    Game::Game *game = new Game::Game(local, remote, this);

    connect(&local_parser, SIGNAL(commandFound(Command, QString)),
            local,         SLOT(onCommandFound(Command, QString)));

    connect(&game_parser, SIGNAL(commandFound(Command, QString)),
            game,         SLOT(onCommandFound(Command, QString)));

    local_parser.setEnabled(true);
    game_parser.setEnabled(true);

    return game;
}


} // namespace Miniature
