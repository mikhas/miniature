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
                 const QString &local_identifier,
                 const QString &remote_identifier)
{
    if (not dispatcher) {
        return 0;
    }

    Side *local = new Side(local_identifier);
    Side *remote = new Side(remote_identifier);
    Game *game = new Game(id, dispatcher, local, remote);

    return game;
}

class GamePrivate
{
public:
    uint id;
    WeakDispatcher dispatcher;
    Position position;
    const QScopedPointer<Side> local; //!< Side of the local player.
    const QScopedPointer<Side> remote; //!< Side of the remote player.
    Side *active; //!< Points to active side.
    Game::GameState state; //!< The game's state.

    explicit GamePrivate(uint new_id,
                         Dispatcher *new_dispatcher,
                         Side *new_local,
                         Side *new_remote)
        : id(new_id) // FIXME: create UID if 0
        , dispatcher(new_dispatcher)
        , position()
        , local(new_local)
        , remote(new_remote)
        , active(new_local)// FIXME: Set correct active side (could be remote) already during construction!
        , state(Game::Idle)
    {
        if (local.isNull() || remote.isNull()) {
            qCritical() << __PRETTY_FUNCTION__
                        << "Creating Game instance with invalid sides is not allowed.";
        }

        // Take over ownership:
        local->setParent(0);
        remote->setParent(0);
    }
};

Game::Game(uint id,
           Dispatcher *dispatcher,
           Side *local,
           Side *remote,
           QObject *parent)
    : QObject(parent)
    , d_ptr(new GamePrivate(id, dispatcher, local, remote))
{
    Q_D(Game);

    connectSide(d->local.data());
    connectSide(d->remote.data());
}

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
    d->active = d->local.data();
    d->active->startTurn(Position(), MovedPiece());
    printTurnMessage(d->active->identifier());

    // Notify backend, too:
    Command::Play play(TargetBackend, advertisement_id);
    sendCommand(&play);
}

void Game::setPosition(const Position &position)
{
    Q_D(Game);
    d->position = position;
}

Position Game::position() const
{
    Q_D(const Game);
    return d->position;
}

Side * Game::localSide() const
{
    Q_D(const Game);
    return d->local.data();
}

Side * Game::remoteSide() const
{
    Q_D(const Game);
    return d->local.data();
}

Side * Game::activeSide() const
{
    Q_D(const Game);
    return d->active;
}

void Game::onTurnEnded(const Position &result,
                       const MovedPiece &moved_piece)
{
    // TOOD: Validate move, call startTurn again with last valid move, on (still active) side.
    Q_D(Game);

    if (d->state != Game::Started) {
        qWarning() << __PRETTY_FUNCTION__
                   << "Game not started yet, ignoring.";
        return;
    }

    if (sender() != d->active) {
        qWarning() << __PRETTY_FUNCTION__
                   << "Move received from inactive side, ignoring.";
        return;
    }

    d->active = (d->active == d->local.data() ? d->remote.data()
                                              : d->local.data());

    d->active->startTurn(result, moved_piece);
    printTurnMessage(d->active->identifier());
}

void Game::connectSide(Side *side)
{
    connect(side, SIGNAL(turnEnded(Position,MovedPiece)),
            this, SLOT(onTurnEnded(Position,MovedPiece)),
            Qt::UniqueConnection);
}

void Game::sendCommand(AbstractCommand *command)
{
    Q_D(Game);
    if (Dispatcher *dispatcher = d->dispatcher.data()) {
        dispatcher->sendCommand(command);
    }
}

} // namespace Game
