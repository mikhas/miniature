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

#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <QtCore>

namespace Game {

class Dispatcher;
typedef QWeakPointer<Dispatcher> WeakDispatcher;

class DispatcherPrivate;
class AbstractCommand;
class AbstractEngine;
class Registry;

namespace Frontend {
    class Miniature;
}

Dispatcher *createDispatcher(QObject *owner = 0);

//! Dispatches commands.
class Dispatcher
    : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Dispatcher)
    Q_DECLARE_PRIVATE(Dispatcher)

private:
    const QScopedPointer<DispatcherPrivate> d_ptr;

public:
    explicit Dispatcher(QObject *parent = 0);
    virtual ~Dispatcher();

    virtual bool sendCommand(AbstractCommand *command);
    virtual void setFrontend(Frontend::Miniature *frontend);
    virtual void setBackend(AbstractEngine *backend);

    //! Optional. Can be used to replace original registry.
    //! @param registry the new registry instance, Dispatcher claims ownership.
    virtual void resetRegistry(Registry *registry);
};

} // namespace Game

#endif // DISPATCHER_H
