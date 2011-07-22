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
#include "move.h"

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

class GamePrivate
{
public:
    SharedAbstractSide local; //!< Side of the local player.
    SharedAbstractSide remote; //!< Side of the remote player.
    SharedAbstractSide active; //!< Points to active side.
    Game::GameState state; //!< The game's state.

    explicit GamePrivate(AbstractSide *new_local,
                         AbstractSide *new_remote)
        : local(new_local)
        , remote(new_remote)
        , active(local)// FIXME: Set correct active side (could be remote) already during construction!
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

Game::Game(AbstractSide *local,
           AbstractSide *remote,
           QObject *parent)
    : QObject(parent)
    , d_ptr(new GamePrivate(local, remote))
{
    Q_D(Game);

    connectSide(d->local);
    connectSide(d->remote);

    std::cout << "Welcome to Miniature!" << std::endl;
}

Game::~Game()
{}

void Game::start()
{
    Q_D(Game);

    // A Game instance represents exactly one game. Restarts will need to
    // create new game instances.
    if (d->state == Game::Idle) {
        d->local->init();
        d->remote->init();
    }
}

const SharedAbstractSide &Game::activeSide() const
{
    Q_D(const Game);
    return d->active;
}

void Game::onCommandFound(Command command,
                          const QString &data)
{
    Q_UNUSED(data)

    switch(command) {
    case CommandNew:
        start();
        break;

    case CommandQuit:
        qApp->exit();
        break;

    default:
        break;
    }
}

void Game::onMoveEnded(const Move &move)
{
    Q_D(Game);

    if (d->state != Game::Started) {
        qWarning() << __PRETTY_FUNCTION__
                   << "Game not started yet, ignoring.";
        return;
    }

    if (sender() != d->active.data()) {
        qWarning() << __PRETTY_FUNCTION__
                   << "Move received from inactive side, ignoring.";
        return;
    }

    d->active = (d->active == d->local ? d->remote
                                       : d->local);

    d->active->startMove(move);
    printTurnMessage(d->active->identifier());
}

void Game::connectSide(const SharedAbstractSide &side)
{
    connect(side.data(), SIGNAL(moveEnded(Move)),
            this,        SLOT(onMoveEnded(Move)),
            Qt::UniqueConnection);

    connect(side.data(), SIGNAL(ready()),
            this,        SLOT(onSideReady()));
}

void Game::onSideReady()
{
    Q_D(Game);

    if (d->local->state() == AbstractSide::NotReady
        || d->remote->state() == AbstractSide::NotReady) {
        return;
    }

    d->state = Game::Started;
    d->active = d->local;
    d->active->startMove(Move());
    printTurnMessage(d->active->identifier());
}

} // namespace Game
