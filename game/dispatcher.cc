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
#include "frontend/miniature.h"
#include "abstractengine.h"
#include "fics/engine.h"
#include "game.h"
#include "registry.h"
#include "commands.h"

namespace Game {

Dispatcher *createDispatcher(QObject *owner)
{
    Dispatcher *dispatcher = new Dispatcher(owner);
    Fics::Engine *engine = new Fics::Engine(owner);

    // Use FICS backend, by default:
    dispatcher->setBackend(engine);

    return dispatcher;
}

class DispatcherPrivate
{
public:
    typedef QVector<Command::Move> MoveStack;

    QWeakPointer<Frontend::Miniature> active_frontend;
    QWeakPointer<AbstractEngine> active_backend;
    QScopedPointer<Registry> registry;
    QHash<uint, MoveStack> move_stacks;

    explicit DispatcherPrivate(Dispatcher *q)
        : active_frontend()
        , active_backend()
        , registry(new Registry)
    {
        registry->setDispatcher(q);
    }
};

Dispatcher::Dispatcher(QObject *parent)
    : QObject(parent)
    , d_ptr(new DispatcherPrivate(this))
{}

Dispatcher::~Dispatcher()
{}

bool Dispatcher::sendCommand(AbstractCommand *command)
{
    if (not command) {
        return false;
    }

    Q_D(Dispatcher);
    bool result = false;

    switch(command->target()) {
    case TargetEngine:
        if (AbstractEngine *backend = d->active_backend.data()) {
            result = true;
            command->exec(this, backend);
        }
        break;

    case TargetFrontend:
        if (Frontend::Miniature *frontend = d->active_frontend.data()) {
            result = true;
            command->exec(this, frontend);
        }
        break;

    case TargetRegistry:
        if (Registry *registry = d->registry.data()) {
            result = true;
            command->exec(this, registry);
        }
        break;

    default:
        break;
    }

    return result;
}

void Dispatcher::setFrontend(Frontend::Miniature *frontend)
{
    Q_D(Dispatcher);
    d->active_frontend = QWeakPointer<Frontend::Miniature>(frontend);
}

void Dispatcher::setBackend(AbstractEngine *backend)
{
    Q_D(Dispatcher);

    AbstractEngine *const old = d->active_backend.data();
    if (old == backend) {
        return; // Nothing to do!
    }

    if (old) {
        // Effectively disables old backend:
        old->setDispatcher(0);
    }

    d->active_backend = QWeakPointer<AbstractEngine>(backend);

    if (backend) {
        // Effectively enables new backend:
        backend->setDispatcher(this);
    }
}

void Dispatcher::resetRegistry(Registry *registry)
{
    Q_D(Dispatcher);
    d->registry.reset(registry);
}

void Dispatcher::pushMove(uint game_id,
                                 const Command::Move &move)
{
    Q_D(Dispatcher);

    if (d->registry->isRegisteredGame(game_id)) {
        d->move_stacks[game_id].append(move);
    }
}

Command::Move Dispatcher::popMove(uint game_id)
{
    Q_D(Dispatcher);

    if (not d->registry->isRegisteredGame(game_id)) {
        return Command::Move();
    }

    DispatcherPrivate::MoveStack &ms(d->move_stacks[game_id]);
    Command::Move move(ms.last());
    ms.pop_back();

    return move;
}

} // namespace Game
