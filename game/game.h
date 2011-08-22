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

#ifndef GAME_H
#define GAME_H

#include "side.h"
#include "namespace.h"

#include <QtCore>

namespace Game {

class Game;
class GamePrivate;
class Dispatcher;
class AbstractCommand;

//! Creates a game
//! @param id the game id. If 0 then an unique identifier will be created.
//! @param dispatcher the dispatcher.
//! @param local_identifier the identifier for the local side.
//! @param remote_identifier the identifier for the remote side.
Game *createGame(uint id,
                 Dispatcher *dispatcher,
                 const QString &local_identifier,
                 const QString &remote_identifier);

//! A Game instance represents exactly one game. A game goes from Idle =>
//! Started => Ended and cannot be restarted.
class Game
    : public QObject
{
    Q_OBJECT

private:
    const QScopedPointer<GamePrivate> d_ptr;
    Q_DECLARE_PRIVATE(Game)
    Q_DISABLE_COPY(Game)

public:
    enum GameState {
        Idle,
        Started,
        Ended
    };

    //! C'tor
    //! @param id the game id. If 0 then an unique identifier will be created.
    //! @param local the local side, Game takes ownership.
    //! @param remote the remote side, Game takes ownership.
    //! @param parent the optional parent that owns this instance.
    explicit Game(uint id,
                  Dispatcher *dispatcher,
                  Side *local,
                  Side *remote,
                  QObject *parent = 0);
    virtual ~Game();

    //! Returns this game's id.
    uint id() const;

    //! Plays a game.
    //! @param advertisement_id the id of the game advertisement (optional).
    void play(uint advertisement_id = 0);

    //! Returns local side.
    WeakSide localSide() const;

    //! Returns remote side.
    WeakSide remoteSide() const;

    //! Returns active side.
    WeakSide activeSide() const;

private:
    //! One side ended turn and submitted a move.
    //! @param move the submitted move.
    Q_SLOT void onTurnEnded(const Position &result,
                            const MovedPiece &moved_piece);

    //! Connects common parts for each side with controller.
    //! @param side the side to connect.
    void connectSide(Side *side);

    void sendCommand(AbstractCommand *command);
};

} // namespace Game

#endif // GAME_H
