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

#include "localside.h"

#include <iostream>

namespace Game {

LocalSide::LocalSide(const QString &identifier)
    : AbstractSide(identifier)
    , m_identifier(identifier)
{}

LocalSide::~LocalSide()
{}

const QString &LocalSide::identifier() const
{
    return m_identifier;
}

void LocalSide::startMove(const Move &move)
{
    Q_UNUSED(move)
    QTimer::singleShot(0, this, SLOT(waitForInput())); // wait for mainloop
}

void LocalSide::waitForInput()
{
#ifdef MINIATURE_CLI_ENABLED
    const std::string moveCmd("move");
    std::string cmd;

    getline(std::cin, cmd);
    if (cmd == "quit") {
        std::cout << "Shutting down. Good bye!" << std::endl;
        qApp->exit();
    } else if (not cmd.compare(0, moveCmd.size(), moveCmd)) {
        const Move m(Position(), Square(), Square(),
                     QString(cmd.substr(moveCmd.size()).c_str()));
        emit moveEnded(m);
    } else {
        QTimer::singleShot(0, this, SLOT(waitForInput())); // wait for mainloop
    }
#endif
}

} // namespace Game
