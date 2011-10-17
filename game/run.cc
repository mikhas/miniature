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

#include "run.h"
#include "dispatcher.h"
#include "frontend/miniature.h"

namespace Game { namespace {
    const char * const orga_name("Miniature Chess");
    const char * const orga_domain("miniature-chess.org");
    const char * const app_name("Miniature");
}

void run(const QUrl &url,
         QObject *parent)
{
    QCoreApplication::setOrganizationName(orga_name);
    QCoreApplication::setOrganizationDomain(orga_domain);
    QCoreApplication::setApplicationName(app_name);

    Dispatcher *dispatcher = createDispatcher(parent);
    Frontend::Miniature *frontend = new Frontend::Miniature(dispatcher, parent);
    dispatcher->setFrontend(frontend);
    frontend->show(url);
}

} // namespace Game
