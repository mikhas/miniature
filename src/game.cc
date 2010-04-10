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

#include <game.h>
#include <position.h>
#include <pieces.h>
#include <main.h>

#include <QList>

#include <QFontDatabase>

using namespace Miniature;


MGame::MGame(MBoardView *view, MGameLog *log, QObject *parent)
: QObject(parent),
  m_view(view),
  m_log(log),
  m_board_item(0),
  m_game_store(new MGameStore(this)),
  m_current_dashboard(0)
{
    Q_CHECK_PTR(m_view);
    Q_CHECK_PTR(m_log);

    connect(m_game_store, SIGNAL(whiteToMove(MPosition)),
            this,         SLOT(onWhiteToMove(MPosition)));

    connect(m_game_store, SIGNAL(blackToMove(MPosition)),
            this,         SLOT(onBlackToMove(MPosition)));

    connect(m_game_store, SIGNAL(candidatePieceSelected()),
            this,         SLOT(onCandidatePieceSelected()));

    connect(m_game_store, SIGNAL(moveDiscarded()),
            this,         SLOT(onMoveDiscarded()));

    connect(m_game_store, SIGNAL(moveConfirmationRequested()),
            this,         SLOT(onMoveConfirmationRequested()));

    connect(m_game_store, SIGNAL(invalidTargetSelected()),
            this,         SLOT(onInvalidTargetSelected()));

    MDashboardItem *top = m_view->getTopDashboardItem();
    MDashboardItem *bottom = m_view->getBottomDashboardItem();

    connectDashboardToGame(top);
    connectDashboardToGame(bottom);

    connectDashboards(top, bottom);
    connectDashboards(bottom, top);

    setupPositionPasting();
}

MGame::~MGame()
{}

void MGame::setupBoardItem()
{
    Q_ASSERT(m_view);

    if (m_board_item)
    {
        delete m_board_item;
        m_board_item = 0;
    }

    m_board_item = new MGraphicsBoardItem;
    m_view->addBoardItem(m_board_item);

    // process state transition requests
    connect(m_board_item, SIGNAL(pieceClicked(MPiece *)),
            m_game_store, SLOT(onPieceSelected(MPiece *)));
    connect(m_board_item, SIGNAL(targetClicked(QPoint)),
            m_game_store, SLOT(onTargetSelected(QPoint)));

    // connecting signals to signals (which act as if a slot would re-emit
    // them) is a nice feature
    connect(this, SIGNAL(turnOfTopPlayer()),
            m_board_item, SIGNAL(rotatePieces180()));

    connect(this, SIGNAL(turnOfBottomPlayer()),
            m_board_item, SIGNAL(rotatePieces0()));

    connect(this, SIGNAL(togglePieceRotations()),
            m_board_item, SIGNAL(togglePieceRotations()));
}

void MGame::newGame()
{
    m_log->append("New game started.", MGameLog::GAME);
    setupBoardItem();
    m_game_store->setupStartPosition();
}

void MGame::jumpToStart()
{
    m_game_store->onPositionRequested(0);
}

void MGame::prevMove()
{
    m_game_store->onPositionRequested(m_game_store->getIndex() - 1);
}

void MGame::nextMove()
{
    m_game_store->onPositionRequested(m_game_store->getIndex() + 1);
}

void MGame::jumpToEnd()
{
// no impl
}

void MGame::abortGame()
{
    deleteLater();
}

void MGame::setupPositionPasting()
{
    // TODO: Get rid of this parent() hack, as it assumes more about parent() than what's healthy. Unless we restrict
    // the MGame ctor to MMainWindows, that is ...
    QShortcut *pasteFen = new QShortcut(static_cast<MMainWindow *>(parent()));
    pasteFen->setKey(QKeySequence(QKeySequence::Paste));
    pasteFen->setContext(Qt::ApplicationShortcut);

    connect(pasteFen, SIGNAL(activated()),
            this,     SLOT(onPositionPasted()));
}

void MGame::onWhiteToMove(const MPosition &position)
{
    MDashboardItem *dashboard = (isWhiteAtBottom() ? m_view->getBottomDashboardItem()
                                                   : m_view->getTopDashboardItem());
    startTurn(position, dashboard);
}

void MGame::onBlackToMove(const MPosition &position)
{
    MDashboardItem *dashboard = (isWhiteAtBottom() ? m_view->getTopDashboardItem()
                                                   : m_view->getBottomDashboardItem());
    startTurn(position, dashboard);
}

void MGame::onCandidatePieceSelected()
{
    Q_CHECK_PTR(m_current_dashboard);

    m_current_dashboard->disableConfirmButton();
}

void MGame::onMoveDiscarded()
{
    Q_CHECK_PTR(m_current_dashboard);

    m_current_dashboard->disableConfirmButton();
}

void MGame::onMoveConfirmationRequested()
{
    Q_CHECK_PTR(m_current_dashboard);

    m_current_dashboard->enableConfirmButton();
}

void MGame::onInvalidTargetSelected()
{
    Q_CHECK_PTR(m_current_dashboard);

    m_current_dashboard->flash();
}

void MGame::startTurn(const MPosition &position, MDashboardItem *const dashboard)
{
    if (m_current_dashboard)
    {
        m_current_dashboard->disableConfirmButton();
        m_current_dashboard->disableRequestsButton();
        m_current_dashboard->hideStatus();
    }

    // uhm, this could be done nicer
    if (0 == m_game_store->getIndex())
    {
        MPosition pos_copy = MPosition(position);
        m_board_item->updateFromPosition(&pos_copy);
    }

    m_current_dashboard = dashboard;
    m_current_dashboard->enableRequestsButton();

    updatePlayerStatus(position);
}

bool MGame::isWhiteAtBottom() const
{
    return true;
}

bool MGame::isBlackAtBottom() const
{
    return !isWhiteAtBottom();
}

void MGame::connectDashboardToGame(MDashboardItem *const dashboard)
{
    // Connect move confirmation
    connect(dashboard,    SIGNAL(confirmButtonPressed()),
            m_game_store, SLOT(onCandidateMoveConfirmed()));

    // Connect aborted game
    connect(dashboard, SIGNAL(abortGameConfirmed()),
            this,      SLOT(abortGame()));
}

void MGame::connectDashboards(MDashboardItem *const first, MDashboardItem *const second)
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
            second, SLOT(onAdjournAccepted()));
    connect(first, SIGNAL(adjournAccepted()),
            first, SLOT(onAdjournAccepted()));

    // Connect resigned game
    connect(first,  SIGNAL(resignConfirmed()),
            second, SLOT(onGameWon()));
    connect(first,  SIGNAL(resignConfirmed()),
            first,  SLOT(onGameLost()));
}

void MGame::updatePlayerStatus(const MPosition &position)
{
    Q_CHECK_PTR(m_current_dashboard);

    m_current_dashboard->appendToLastMovesList(QString("42. Rc1"));
    QString status;

    if (position.inCheck())
        status = tr("Check!");
    else
        status = tr("Turn started");

    m_current_dashboard->setStatusText(status);
    m_log->append(QString("%1 - %2").arg(m_game_store->getIndex()).arg(status), MGameLog::GAME);
    m_log->append(position.asFen(), MGameLog::FEN);

}

void MGame::onPositionPasted()
{
    if(QApplication::clipboard()->mimeData()->hasText())
    {
        MPosition pos = MPosition::fromFen(QApplication::clipboard()->text());
        if (pos.isValid())
        {
            setupBoardItem();
            m_game_store->reset(pos);
            m_log->append("New position pasted.", MGameLog::GAME);
        }
    }
}
