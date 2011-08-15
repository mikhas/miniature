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

#include "dispatcher.h"
#include "abstractcommand.h"
#include "frontend.h"
#include "abstractbackend.h"
#include "fics/backend.h"
#include "game.h"

namespace Game {

Dispatcher *createDispatcher(QObject *owner)
{
    Dispatcher *dispatcher = new Dispatcher(owner);

    // Use FICS backend, by default:
    dispatcher->setActiveBackend(new Fics::Backend(dispatcher, owner));

    return dispatcher;
}

class DispatcherPrivate
{
public:
    QWeakPointer<Frontend> active_frontend;
    QWeakPointer<AbstractBackend> active_backend;
    QWeakPointer<Game> active_game;

    explicit DispatcherPrivate()
        : active_frontend()
        , active_backend()
        , active_game()
    {}
};

Dispatcher::Dispatcher(QObject *parent)
    : QObject(parent)
    , d_ptr(new DispatcherPrivate)
{}

Dispatcher::~Dispatcher()
{}

bool Dispatcher::sendCommand(AbstractCommand *command)
{
    if (not command) {
        return false;
    }

    bool result = false;

    Q_D(Dispatcher);

    switch(command->target()) {
    case TargetBackend:
        if (AbstractBackend *backend = d->active_backend.data()) {
            result = command->exec(backend);
        }
        break;

    case TargetFrontend:
        if (Frontend *frontend = d->active_frontend.data()) {
            result = command->exec(frontend);
        }
        break;

    case TargetGame:
        if (Game *game = d->active_game.data()) {
            result = command->exec(game);
        }
        break;

    default:
        break;
    }

    return result;
}

void Dispatcher::setActiveFrontend(Frontend *frontend)
{
    Q_D(Dispatcher);
    d->active_frontend = QWeakPointer<Frontend>(frontend);
}

void Dispatcher::setActiveBackend(AbstractBackend *backend)
{
    Q_D(Dispatcher);

    if (AbstractBackend *old = d->active_backend.data()) {
        old->setEnabled(false);
    }

    d->active_backend = QWeakPointer<AbstractBackend>(backend);

    if (backend) {
        backend->setEnabled(true);
    }
}

void Dispatcher::setActiveGame(Game *game)
{
    Q_D(Dispatcher);
    d->active_game = QWeakPointer<Game>(game);
}

} // namespace Game
