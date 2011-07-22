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

#include "game.h"

#include <QtCore>

namespace Miniature {

//! Controls game.
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

public:
    explicit GameManager(QObject *parent = 0);
    virtual ~GameManager();

    //! Starts game with specified game mode.
    //! @param mode the game mode
    Q_SLOT void startGame(GameMode mode);

private:
    Game::Game *createLocalEngineGame();
};

} // namespace Miniature

#endif // GAMEMANAGER_H
