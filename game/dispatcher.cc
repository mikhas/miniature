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

namespace Game {

class DispatcherPrivate
{
public:
    // TODO: If games of list is not shared, then better provide means to query game list, say from GameManager.
    QList<QPointer<Game> > games; // active game == QList::last()
    SharedParser parser;
    SharedParser local_side_parser;
    SharedParser gnuchess_parser;
    SharedBackend server_link;

    explicit DispatcherPrivate()
        : server_link(new FicsLink)
    {}
};

Dispatcher::Dispatcher(QObject *parent)
    : QObject(parent)
    , d_ptr(new DispatcherPrivate())
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
    case TargetServer:
        d->server_link->setEnabled(true);
        result = command->exec(d->server_link.data());
        break;

    default:
        break;
    }

    return result;
}

} // namespace Game
