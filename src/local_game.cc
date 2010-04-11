/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2009 Michael Hasselmann <michael@taschenorakel.de>
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

#include <local_game.h>

using namespace Miniature;

MLocalGame::MLocalGame(MBoardView *view, MGameLog *log, QObject *parent)
: MGame(view, log, parent)
{
    setupDashboard(); // cannot be reimpl in derived class if called in ctor, I wish there was "final" ...
}

MLocalGame::~MLocalGame()
{}

void MLocalGame::setupDashboard()
{
    Q_ASSERT(m_view);

    MDashboardItem *top = m_view->getTopDashboardItem();
    MDashboardItem *bottom = m_view->getBottomDashboardItem();

    connectDashboardToGame(top);
    connectDashboardToGame(bottom);

    connectDashboards(top, bottom);
    connectDashboards(bottom, top);
}

void MLocalGame::connectDashboards(MDashboardItem *const first, MDashboardItem *const second)
{
    // Connect draw requests
    connect(first,  SIGNAL(drawButtonPressed()),
            second, SLOT(drawOffered()));

    // Connect adjourn requests
    connect(first,  SIGNAL(adjournButtonPressed()),
            second, SLOT(adjournOffered()));

    // Connect resign requests
    connect(first,  SIGNAL(resignButtonPressed()),
            first,  SLOT(showResignConfirmation()));

    // Connect abort game requests
    connect(first,  SIGNAL(abortGameButtonPressed()),
            first,  SLOT(showAbortGameConfirmation()));

    // Connect game log requests
    QSignalMapper *mapper = new QSignalMapper(this);
    mapper->setMapping(first, QApplication::activeWindow());
    connect(first,  SIGNAL(showGameLogButtonPressed()),
            mapper, SLOT(map()));
    connect(mapper, SIGNAL(mapped(QWidget *)),
            m_log,  SLOT(showLog(QWidget *)));

    // Connect the draw acceptance
    connect(first,  SIGNAL(drawAccepted()),
            second, SLOT(onDrawAccepted()));
    connect(first, SIGNAL(drawAccepted()),
            first, SLOT(onDrawAccepted()));

    // Connect the adjourn game acceptance
    connect(first,  SIGNAL(adjournAccepted()),
            second, SLOT(drawOffered()));

    // Connect resigned game
    connect(first,  SIGNAL(resignConfirmed()),
            second, SLOT(onGameWon()));
    connect(first,  SIGNAL(resignConfirmed()),
            first,  SLOT(onGameLost()));

    // Connect abort game
    connect(first, SIGNAL(abortGameConfirmed()),
            this,  SLOT(abortGame()));
}

void MLocalGame::startTurn(const MPosition &position, MDashboardItem *const dashboard)
{
    if (m_current_dashboard)
    {
        m_current_dashboard->disableConfirmButton();
        m_current_dashboard->disableRequestsButton();
        m_current_dashboard->hideStatus();
    }

    // TODO: will this really work for swapped colours?
    if (isWhiteAtBottom() && MPiece::WHITE == position.getColourToMove())
        m_board_item->rotatePieces0();
    else
        m_board_item->rotatePieces180();

    MGame::startTurn(position, dashboard);
}
