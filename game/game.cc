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

Game::Game(QObject *parent)
    : QObject(parent)
    , m_local(new LocalSide("White"))
    , m_remote(new LocalSide("Black"))
    , m_active(m_local.get()) // FIXME: Set correct active side (could be remote) already during construction!
{
    connect(m_local.get(), SIGNAL(moveEnded(AbstractSide, Move)),
            this,          SLOT(onMoveEnded(AbstractSide, Move)),
            Qt::UniqueConnection);

    connect(m_remote.get(), SIGNAL(moveEnded(AbstractSide, Move)),
            this,           SLOT(onMoveEnded(AbstractSide, Move)),
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
    m_active = m_local.get();
    m_active->startMove(Move());
    printTurnMessage(m_active->identifier());
}

const AbstractSide &Game::activeSide() const
{
    return *m_active;
}

void Game::onMoveEnded(const AbstractSide &side,
                       const Move &move)
{
    if (&side != m_active) {
        qWarning() << __PRETTY_FUNCTION__
                   << "Move received from inactive side, ignoring.";
        return;
    }

    m_active = (m_active == m_local.get() ? m_remote.get()
                                          : m_local.get());

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
