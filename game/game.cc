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

#include "game.h"
#include "dispatcher.h"
#include "commands.h"

#include <iostream>

namespace {
    void printTurnMessage(const QString &name)
    {
        std::cout << "Turn started for "
                  << name.toLatin1().data()
                  << "." << std::endl;
    }
}

namespace Game {

Game *createGame(uint id,
                 Dispatcher *dispatcher,
                 const QByteArray &local_name,
                 const QByteArray &remote_name)
{
    if (not dispatcher) {
        return 0;
    }

    Side local;
    local.name = local_name;

    Side remote;
    remote.name = remote_name;
    Game *game = new Game(id, dispatcher, local, remote);

    return game;
}

class GamePrivate
{
public:
    uint id;
    WeakDispatcher dispatcher;
    Position position;
    Side local; //!< Side of the local player.
    Side remote; //!< Side of the remote player.
    Side *active; //!< Points to active side.
    Game::GameState state; //!< The game's state.
    LocalSideColor local_color; //!< Color of local side.
    uint time; //!< Initial time.
    uint time_increment; //! < Time increment for each move.
    uint rating; //! < Game rating

    explicit GamePrivate(uint new_id,
                         Dispatcher *new_dispatcher,
                         const Side &new_local,
                         const Side &new_remote)
        : id(new_id) // FIXME: create UID if 0
        , dispatcher(new_dispatcher)
        , position()
        , local(new_local)
        , remote(new_remote)
        , active(&local)// FIXME: Set correct active side (could be remote) already during construction!
        , state(Game::Idle)
        , local_color(LocalSideIsWhite)
        , time(0)
        , time_increment(0)
        , rating(0)
    {}
};

Game::Game(uint id,
           Dispatcher *dispatcher,
           const Side &local,
           const Side &remote,
           QObject *parent)
    : QObject(parent)
    , d_ptr(new GamePrivate(id, dispatcher, local, remote))
{}

Game::~Game()
{}

uint Game::id() const
{
    Q_D(const Game);
    return d->id;
}

void Game::play(uint advertisement_id)
{
    Q_D(Game);
    if (d->state != Game::Idle) {
        return;
    }

    d->state = Game::Started;

    // Notify backend, too:
    Command::Play play(TargetEngine, advertisement_id);
    sendCommand(&play);
}

void Game::setPosition(const Position &position)
{
    Q_D(Game);
    if (d->position != position) {
        d->position = position;
        computeActiveSide(d->position.nextToMove());
        emit positionChanged(d->position);
    }
}

Position Game::position() const
{
    Q_D(const Game);
    return d->position;
}

void Game::setLocalSideColor(LocalSideColor color)
{
    Q_D(Game);
    d->local_color = color;
}

LocalSideColor Game::localSideColor() const
{
    Q_D(const Game);
    return d->local_color;
}

Side Game::localSide() const
{
    Q_D(const Game);
    return d->local;
}

Side Game::remoteSide() const
{
    Q_D(const Game);
    return d->remote;
}

Side Game::activeSide() const
{
    Q_D(const Game);
    return (d->active ? *d->active : d->local);
}

void Game::setTime(uint time)
{
    Q_D(Game);
    d->time = time;
}

uint Game::time() const
{
    Q_D(const Game);
    return d->time;
}

void Game::setTimeIncrement(uint time_increment)
{
    Q_D(Game);
    d->time_increment = time_increment;
}

uint Game::timeIncrement() const
{
    Q_D(const Game);
    return d->time_increment;
}

void Game::setRating(uint rating)
{
    Q_D(Game);
    d->rating = rating;
}

uint Game::rating() const
{
    Q_D(const Game);
    return d->rating;
}

void Game::sendCommand(AbstractCommand *command)
{
    Q_D(Game);
    if (Dispatcher *dispatcher = d->dispatcher.data()) {
        dispatcher->sendCommand(command);
    }
}

void Game::computeActiveSide(Color next_to_move)
{
    Q_D(Game);

    switch(next_to_move) {
    case ColorWhite:
        d->active = (d->local_color == LocalSideIsWhite ? &d->local
                                                        : &d->remote);
        break;

    case ColorBlack:
        d->active = (d->local_color == LocalSideIsBlack ? &d->local
                                                        : &d->remote);
        break;

    default:
        break;
    }

    printTurnMessage(d->active->name);
}

} // namespace Game
