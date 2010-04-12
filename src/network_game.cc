/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2010 Michael Hasselmann <michael@taschenorakel.de>
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

#include "network_game.h"

using namespace Miniature;

MNetworkGame::
MNetworkGame(MBoardView *view, MGameLog *log, QObject *parent)
    : MGame(view, log, parent)
{
    m_view->addChatbox();
    setupDashboard(); // cannot be reimpl in derived class if called in ctor, I wish there was "final" ...

    if (isWhiteAtBottom())
        m_dashboard = m_view->getDashboard(MBoardView::ALIGN_BOTTOM);
    else
        m_dashboard = m_view->getDashboard(MBoardView::ALIGN_TOP);
}

MNetworkGame::
~MNetworkGame()
{}

void MNetworkGame::
appendChatMessage(const QString &msg)
{
    m_view->getChatbox()->append(QString("%1\n").arg(msg));
    m_log->append(QString("%1\n").arg(msg), MGameLog::CHAT);
}

void MNetworkGame::
onWhiteToMove(const MPosition &position)
{
    if (isWhiteAtBottom())
    {
        m_board->enable();
        startTurn(position);
    }
}

void MNetworkGame::
onBlackToMove(const MPosition &position)
{
    if (!isWhiteAtBottom())
    {
        m_board->enable();
        startTurn(position);
    }
}

void MNetworkGame::
endTurn()
{
    m_board->disable();
    MGame::endTurn();
}

void MNetworkGame::
setupDashboard()
{
    Q_ASSERT(m_view);

    m_view->addDashboard(MBoardView::ALIGN_BOTTOM);
    connectDashboardToGame(m_view->getDashboard(MBoardView::ALIGN_BOTTOM));
}
