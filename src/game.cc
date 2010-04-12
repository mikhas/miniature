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

#include <game.h>
#include <pieces.h>
#include <main.h>

using namespace Miniature;

MGame::MGame(MBoardView *view, MGameLog *log, QObject *parent)
: QObject(parent),
  m_view(view),
  m_log(log),
  m_board(0),
  m_store(new MGameStore(this)),
  m_dashboard(0)
{
    Q_CHECK_PTR(m_view);
    Q_CHECK_PTR(m_log);

    connect(m_store, SIGNAL(whiteToMove(MPosition)),
            this,         SLOT(onWhiteToMove(MPosition)));

    connect(m_store, SIGNAL(blackToMove(MPosition)),
            this,         SLOT(onBlackToMove(MPosition)));

    connect(m_store, SIGNAL(candidatePieceSelected()),
            this,         SLOT(onCandidatePieceSelected()));

    connect(m_store, SIGNAL(moveDiscarded()),
            this,         SLOT(onMoveDiscarded()));

    connect(m_store, SIGNAL(moveConfirmationRequested()),
            this,         SLOT(onMoveConfirmationRequested()));

    connect(m_store, SIGNAL(invalidTargetSelected()),
            this,         SLOT(onInvalidTargetSelected()));

    setupPositionPasting();
}

MGame::~MGame()
{}

void MGame::setupBoardItem()
{
    Q_ASSERT(m_view);

    if (m_board)
    {
        delete m_board;
        m_board = 0;
    }

    m_board = new MGraphicsBoardItem;
    m_view->addBoard(m_board);

    // process state transition requests
    connect(m_board, SIGNAL(pieceClicked(MPiece *)),
            m_store, SLOT(onPieceSelected(MPiece *)));
    connect(m_board, SIGNAL(targetClicked(QPoint)),
            m_store, SLOT(onTargetSelected(QPoint)));

    connect(this, SIGNAL(togglePieceRotations()),
            m_board, SIGNAL(togglePieceRotations()));
}

void MGame::setupDashboard()
{
    // reimpl me!
}

void MGame::newGame()
{
    m_log->append("New game started.", MGameLog::GAME);
    setupBoardItem();
    m_store->setupStartPosition();
}

void MGame::jumpToStart()
{
    m_store->onPositionRequested(0);
}

void MGame::prevMove()
{
    m_store->onPositionRequested(m_store->getIndex() - 1);
}

void MGame::nextMove()
{
    m_store->onPositionRequested(m_store->getIndex() + 1);
}

void MGame::jumpToEnd()
{
// no impl
}

void MGame::abortGame()
{
    delete this;
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
    m_dashboard = (isWhiteAtBottom() ? m_view->getDashboard(MBoardView::ALIGN_BOTTOM)
                                     : m_view->getDashboard(MBoardView::ALIGN_TOP));
    startTurn(position);
}

void MGame::onBlackToMove(const MPosition &position)
{
    m_dashboard = (isWhiteAtBottom() ? m_view->getDashboard(MBoardView::ALIGN_TOP)
                                     : m_view->getDashboard(MBoardView::ALIGN_BOTTOM));
    startTurn(position);
}

void MGame::onCandidatePieceSelected()
{
    Q_CHECK_PTR(m_dashboard);

    m_dashboard->disableConfirmButton();
}

void MGame::onMoveDiscarded()
{
    Q_CHECK_PTR(m_dashboard);

    m_dashboard->disableConfirmButton();
}

void MGame::onMoveConfirmationRequested()
{
    Q_CHECK_PTR(m_dashboard);

    m_dashboard->enableConfirmButton();
}

void MGame::onConfirmButtonPressed()
{
    endTurn();
    m_store->onCandidateMoveConfirmed();
}

void MGame::onInvalidTargetSelected()
{
    Q_CHECK_PTR(m_dashboard);

    m_dashboard->flash();
}

void MGame::endTurn()
{
    Q_CHECK_PTR(m_dashboard);

    m_dashboard->disableConfirmButton();
    m_dashboard->disableRequestsButton();
    m_dashboard->hideStatus();
}

void MGame::startTurn(const MPosition &position)
{
    // uhm, this could be done nicer
    if (0 == m_store->getIndex())
    {
        MPosition pos_copy = MPosition(position);
        m_board->updateFromPosition(&pos_copy);
    }

    m_dashboard->enableRequestsButton();
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
    connect(dashboard, SIGNAL(confirmButtonPressed()),
            this,      SLOT(onConfirmButtonPressed()));

    // Connect aborted game
    connect(dashboard, SIGNAL(abortGameConfirmed()),
            this,      SLOT(abortGame()));

    // Connect resign requests
    connect(dashboard,  SIGNAL(resignButtonPressed()),
            dashboard,  SLOT(showResignConfirmation()));

    connect(dashboard,  SIGNAL(resignConfirmed()),
            dashboard,  SLOT(onGameLost()));

    // Connect the draw acceptance
    connect(dashboard, SIGNAL(drawAccepted()),
            dashboard, SLOT(onDrawAccepted()));

    connect(dashboard, SIGNAL(abortGameConfirmed()),
            this,      SLOT(abortGame()));

    // Connect abort game requests
    connect(dashboard,  SIGNAL(abortGameButtonPressed()),
            dashboard,  SLOT(showAbortGameConfirmation()));

    // Connect game log requests
    QSignalMapper *mapper = new QSignalMapper(this);
    mapper->setMapping(dashboard, QApplication::activeWindow());
    connect(dashboard,  SIGNAL(showGameLogButtonPressed()),
            mapper,     SLOT(map()));
    connect(mapper, SIGNAL(mapped(QWidget *)),
            m_log,  SLOT(showLog(QWidget *)));
}

void MGame::updatePlayerStatus(const MPosition &position)
{
    Q_CHECK_PTR(m_dashboard);

    m_dashboard->appendToLastMovesList(QString("42. Rc1"));
    QString status;

    if (position.inCheck())
        status = tr("Check!");
    else
        status = tr("Turn started");

    m_dashboard->setStatusText(status);
    m_log->append(QString("%1 - %2").arg(m_store->getIndex()).arg(status), MGameLog::GAME);
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
            m_store->reset(pos);
            m_log->append("New position pasted.", MGameLog::GAME);
        }
    }
}
