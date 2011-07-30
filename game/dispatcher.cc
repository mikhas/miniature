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
#include "abstractbackend.h"
#include "ficsside.h"
#include "linereader.h"
#include "directinputdevice.h"
#include "frontend.h"

namespace Game {

class DispatcherPrivate
{
public:
    FicsBackend fics;
    QWeakPointer<Frontend> frontend;

    explicit DispatcherPrivate(Dispatcher *q)
        : fics(q)
        , frontend()
    {}
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

    bool result = false;

    Q_D(Dispatcher);

    switch(command->target()) {
    case TargetBackendFics:
        d->fics.setEnabled(true);
        result = command->exec(&d->fics);
        break;

    default:
        break;
    }

    return result;
}

void Dispatcher::setFrontend(Frontend *frontend)
{
    Q_D(Dispatcher);
    d->frontend = QWeakPointer<Frontend>(frontend);
}

} // namespace Game
