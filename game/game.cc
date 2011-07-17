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
#include "localside.h"

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

Game::Game(AbstractSide *local,
           AbstractSide *remote,
           QObject *parent)
    : QObject(parent)
    , m_local(local)
    , m_remote(remote)
    , m_active(m_local) // FIXME: Set correct active side (could be remote) already during construction!
{
    if (m_local.isNull() || m_remote.isNull()) {
        qCritical() << __PRETTY_FUNCTION__
                    << "Creating Game instance with invalid sides is not allowed.";
    }

    // Take over ownership:
    m_local->setParent(0);
    m_remote->setParent(0);

    connect(m_local.data(), SIGNAL(moveEnded(Move)),
            this,           SLOT(onMoveEnded(Move)),
            Qt::UniqueConnection);

    connect(m_remote.data(), SIGNAL(moveEnded(Move)),
            this,            SLOT(onMoveEnded(Move)),
            Qt::UniqueConnection);

#ifdef MINIATURE_CLI_ENABLED
    std::cout << "Welcome to Miniature!" << std::endl;
    QTimer::singleShot(0, this, SLOT(waitForInput())); // wait for mainloop
#endif
}

Game::~Game()
{}

void Game::start()
{
    m_active = m_local;
    m_active->startMove(Move());
    printTurnMessage(m_active->identifier());
}

const SharedAbstractSide &Game::activeSide() const
{
    return m_active;
}

void Game::onMoveEnded(const Move &move)
{
    if (sender() != m_active.data()) {
        qWarning() << __PRETTY_FUNCTION__
                   << "Move received from inactive side, ignoring.";
        return;
    }

    m_active = (m_active == m_local ? m_remote
                                    : m_local);

    m_active->startMove(move);
    printTurnMessage(m_active->identifier());
}

void Game::waitForInput()
{
#ifdef MINIATURE_CLI_ENABLED
    std::string cmd;

    getline(std::cin, cmd);
    if (cmd == "start") {
        start();
    } else {
        QTimer::singleShot(0, this, SLOT(waitForInput())); // wait for mainloop
    }
#endif
}

} // namespace Game
