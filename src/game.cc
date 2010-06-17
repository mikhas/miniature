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
#include <mmainwindow.h>

using namespace Miniature;

MGame::MGame(MGameLog *log, QObject *parent)
: QObject(parent),
  m_board_view(0),
  m_log(log),
  m_board(0),
  m_store(new MGameStore(this)),
  m_dashboard(0)
{
    qDebug() << "MGame::MGame()";
    Q_ASSERT(0 != m_log);

    connect(m_store, SIGNAL(whiteToMove(MPosition)),
            this,    SLOT(onWhiteToMove(MPosition)));

    connect(m_store, SIGNAL(blackToMove(MPosition)),
            this,    SLOT(onBlackToMove(MPosition)));

    connect(m_store, SIGNAL(candidatePieceSelected()),
            this,    SLOT(onCandidatePieceSelected()));

    connect(m_store, SIGNAL(moveDiscarded()),
            this,    SLOT(onMoveDiscarded()));

    connect(m_store, SIGNAL(moveConfirmationRequested()),
            this,    SLOT(onMoveConfirmationRequested()));

    connect(m_store, SIGNAL(invalidTargetSelected()),
            this,    SLOT(onInvalidTargetSelected()));
}

MGame::~MGame()
{
    qDebug() << "MGame::~MGame()";
}

void MGame::setBoardView(MBoardView *view)
{
    qDebug() << "MGame::setBoardView()";

    Q_ASSERT_X((0 != view),
               "setBoardView",
               "Invalid board view!");

    delete m_board_view;
    m_board_view = view;

    connect(m_board_view, SIGNAL(sendMessageRequest(QString)),
            this,         SLOT(onSendMessageRequest(QString)));

    setupDashboard();
    setupBoardItem();
}

MBoardView * MGame::getBoardView() const
{
    qDebug() << "MGame::getBoardView()";

    return m_board_view;
}

void MGame::setupBoardItem()
{
    qDebug() << "MGame::setupBoardItem()";

    Q_ASSERT(0 != m_board_view);

    delete m_board;
    m_board = new MGraphicsBoardItem;

    m_board_view->addBoard(m_board);

    // process state transition requests
    connect(m_board, SIGNAL(pieceClicked(MPiece *)),
            m_store, SLOT(onPieceSelected(MPiece *)));
    connect(m_board, SIGNAL(targetClicked(QPoint)),
            m_store, SLOT(onTargetSelected(QPoint)));

    connect(this,    SIGNAL(togglePieceRotations()),
            m_board, SIGNAL(togglePieceRotations()));
}

void MGame::setupDashboard()
{
    qDebug() << "MGame::setupDashboard()";

    // reimpl me!

    Q_ASSERT_X((false),
               "setupDashboard",
               "This code should not be reached!");
}

void MGame::newGame()
{
    qDebug() << "MGame::newGame()";

    m_log->append("New game started.", MGameLog::GAME);
    m_store->setupStartPosition();
}

void MGame::jumpToStart()
{
    qDebug() << "MGame::jumpToStart()";

    m_store->onPositionRequested(0);
}

void MGame::prevMove()
{
    qDebug() << "MGame::prevMove()";

    m_store->onPositionRequested(m_store->getIndex() - 1);
}

void MGame::nextMove()
{
    qDebug() << "MGame::nextMove()";

    m_store->onPositionRequested(m_store->getIndex() + 1);
}

void MGame::jumpToEnd()
{
    qDebug() << "MGame::jumpToEnd()";

// no impl
}

void MGame::abortGame()
{
    qDebug() << "MGame::abortGame()";

    delete this;
}

void MGame::activatePositionPasting(MMainWindow *active_window)
{
    qDebug() << "MGame::activatePositionPasting()";

    // TODO: Get rid of this parent() hack, as it assumes more about parent() than what's healthy. Unless we restrict
    // the MGame ctor to MMainWindows, that is ...
    QShortcut *pasteFen = new QShortcut(active_window);
    pasteFen->setKey(QKeySequence(QKeySequence::Paste));
    pasteFen->setContext(Qt::ApplicationShortcut);

    connect(pasteFen, SIGNAL(activated()),
            this,     SLOT(onPositionPasted()));
}

void MGame::onWhiteToMove(const MPosition &position)
{
    qDebug() << "MGame::onWhiteToMove()";

    m_dashboard = (isWhiteAtBottom() ? m_board_view->getDashboard(MBoardView::ALIGN_BOTTOM)
                                     : m_board_view->getDashboard(MBoardView::ALIGN_TOP));
    startTurn(position);
}

void MGame::onBlackToMove(const MPosition &position)
{
    qDebug() << "MGame::onBlackToMove()";

    m_dashboard = (isWhiteAtBottom() ? m_board_view->getDashboard(MBoardView::ALIGN_TOP)
                                     : m_board_view->getDashboard(MBoardView::ALIGN_BOTTOM));
    startTurn(position);
}

void MGame::onCandidatePieceSelected()
{
    qDebug() << "MGame::onCandidatePieceSelected()";

    Q_CHECK_PTR(m_dashboard);

    m_dashboard->disableConfirmButton();
}

void MGame::onMoveDiscarded()
{
    qDebug() << "MGame::onMoveDiscard()";

    Q_CHECK_PTR(m_dashboard);

    m_dashboard->disableConfirmButton();
}

void MGame::onMoveConfirmationRequested()
{
    qDebug() << "MGame::onMoveConfirmationRequested()";

    Q_CHECK_PTR(m_dashboard);

    m_dashboard->enableConfirmButton();
}

void MGame::onConfirmButtonPressed()
{
    qDebug() << "MGame::onConfirmButtonPressed()";

    endTurn();
    m_store->onCandidateMoveConfirmed();
}

void MGame::onInvalidTargetSelected()
{
    qDebug() << "MGame::onInvalidTargetSelected()";

    Q_CHECK_PTR(m_dashboard);

    m_dashboard->flash();
}

void MGame::onSendMessageRequest(const QString &message)
{
    qDebug() << "MGame::onSendMessageRequest()";

    Q_CHECK_PTR(m_board_view);

    m_board_view->getChatbox()->insertPlainText(QString("%1\n").arg(message));
    m_board_view->getChatbox()->ensureCursorVisible();

    m_log->append(QString("%1\n").arg(message), MGameLog::CHAT);
}

void MGame::endTurn()
{
    qDebug() << "MGame::endTurn()";

    Q_CHECK_PTR(m_dashboard);

    m_dashboard->disableConfirmButton();
    m_dashboard->disableRequestsButton();
    m_dashboard->hideStatus();
}

void MGame::startTurn(const MPosition &position)
{
    qDebug() << "MGame::startTurn()";

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
    qDebug() << "MGame::isWhiteAtBottom()";

    return true;
}

bool MGame::isBlackAtBottom() const
{
    qDebug() << "MGame::isBlackAtBottom()";

    return !isWhiteAtBottom();
}

void MGame::connectDashboardToGame(MDashboardItem *const dashboard)
{
    qDebug() << "MGame::connectDashboardToGame()";

    // Connect move confirmation
    connect(dashboard, SIGNAL(confirmButtonPressed()),
            this,      SLOT(onConfirmButtonPressed()));

    // Connect aborted game
    connect(dashboard, SIGNAL(abortGameConfirmed()),
            this,      SLOT(abortGame()));

    // Connect resign requests
    connect(dashboard, SIGNAL(resignButtonPressed()),
            dashboard, SLOT(showResignConfirmation()));

    connect(dashboard, SIGNAL(resignConfirmed()),
            dashboard, SLOT(onGameLost()));

    // Connect the draw acceptance
    connect(dashboard, SIGNAL(drawAccepted()),
            dashboard, SLOT(onDrawAccepted()));

    connect(dashboard, SIGNAL(abortGameConfirmed()),
            this,      SLOT(abortGame()));

    // Connect abort game requests
    connect(dashboard, SIGNAL(abortGameButtonPressed()),
            dashboard, SLOT(showAbortGameConfirmation()));

    // Connect game log requests
    QSignalMapper *mapper = new QSignalMapper(this);
    mapper->setMapping(dashboard, QApplication::activeWindow());
    connect(dashboard, SIGNAL(showGameLogButtonPressed()),
            mapper,    SLOT(map()));
    connect(mapper, SIGNAL(mapped(QWidget *)),
            m_log,  SLOT(showLog(QWidget *)));
}

void MGame::updatePlayerStatus(const MPosition &position)
{
    qDebug() << "MGame::updatePlayerStatus()";

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
    qDebug() << "MGame::onPositionPasted()";

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
