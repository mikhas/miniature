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

#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <namespace.h>
#include <game.h>
#include <abstracttokenizer.h>
#include <localparser.h>
#include <ficsside.h>
#include <QtCore>

namespace Miniature {

using Game::Command;

//! Responsible for initiating games. Also maintain a list of active games.
class GameManager
    : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(GameManager)

public:
    enum GameMode {
        GameModeLocalEngine,
        GameModeLocalHuman,
        GameModeRemoteFics,
        GameModeRemoteP2P
    };

private:
    QList<QPointer<Game::Game> > m_games;
    Game::SharedTokenizer m_tokenizer;
    Game::LocalParser m_parser;
    Game::LocalParser m_local_side_parser;
    QSharedPointer<Game::AbstractLink> m_fics_link;

public:
    enum Parser {
        ParserGameManager,
        ParserLocalSide
    };

    explicit GameManager(QObject *parent = 0);
    virtual ~GameManager();

    //! Starts game with specified game mode.
    //! @param mode the game mode
    Q_SLOT void startGame(GameMode mode);

    //! Test API, exposes the internally used parser instance.
    const Game::AbstractParser &parser(Parser type) const;

private:        
    Game::Game *createLocalEngineGame();
    Game::Game *createRemoteFicsGame();
    void connectToParser(Game::AbstractSide *side);

    //! Handle input from command line interface.
    //! @command the found command.
    //! @data the data that belongs to the command.
    Q_SLOT void onCommandFound(Command command,
                               const QByteArray &data);
};

} // namespace Miniature

#endif // GAMEMANAGER_H
