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
#include "position.h"
#include "namespace.h"

#include <QtCore>

namespace Game {

class Game;
class GamePrivate;
class Dispatcher;
class AbstractCommand;

typedef QWeakPointer<Game> WeakGame;

//! Creates a game
//! @param id the game id. If 0 then an unique identifier will be created.
//! @param dispatcher the dispatcher.
//! @param local_name the player'name.
//! @param remote_name the opponent's name.
Game *createGame(uint id,
                 Dispatcher *dispatcher,
                 const QByteArray &local_name,
                 const QByteArray &remote_name);

//! A Game instance represents exactly one game. A game goes from Idle =>
//! Started => Ended and cannot be restarted.
class Game
    : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Game)
    Q_DISABLE_COPY(Game)
    Q_PROPERTY(Position position READ position
                                 WRITE setPosition
                                 NOTIFY positionChanged)

private:
    const QScopedPointer<GamePrivate> d_ptr;

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
                  const Side &local,
                  const Side &remote,
                  QObject *parent = 0);
    virtual ~Game();

    //! Returns this game's id.
    uint id() const;

    //! Plays a game.
    //! @param advertisement_id the id of the game advertisement (optional).
    void play(uint advertisement_id = 0);

    // TODO: provide an applyMove method that does validation etc.
    //! Sets position.
    //! @param position the position.
    void setPosition(const Position &position);

    //! Returns position.
    Position position() const;

    //! Sets the color for the local side.
    //! @param color the color.
    void setLocalSideColor(LocalSideColor color);

    //! Returns the color for the local side.
    LocalSideColor localSideColor() const;

    //! Emitted whenever position changes.
    //! @param position the new position.
    Q_SIGNAL void positionChanged(const Position &position);

    //! Returns local side.
    Side localSide() const;

    //! Returns remote side.
    Side remoteSide() const;

    //! Returns active side.
    Side activeSide() const;

private:
    void sendCommand(AbstractCommand *command);
    void computeActiveSide(Color next_to_move);
};

} // namespace Game

#endif // GAME_H
