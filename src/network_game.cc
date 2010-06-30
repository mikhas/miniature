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
#include <QMessageBox>

using namespace Miniature;

MNetworkGame::
MNetworkGame(MGameLog *log, QObject *parent)
    : MGame(log, parent),
      m_tp_game(0),
      mIsWhiteAtBottom(true)
{}

MNetworkGame::
~MNetworkGame()
{}

void MNetworkGame::
hostGame()
{
    delete m_tp_game;
    m_tp_game = new TpGame::Game(this);
    connect(m_tp_game, SIGNAL(disconnected()), this, SIGNAL(disconnected()), Qt::UniqueConnection);
    connect(m_tp_game, SIGNAL(connected()), SIGNAL(connected()), Qt::UniqueConnection);
    connect(m_tp_game, SIGNAL(connected()), SLOT(hostGameConnected()), Qt::UniqueConnection);
    connect(m_tp_game, SIGNAL(receivedNewGame(bool)), SLOT(receivedNewGame(bool)), Qt::UniqueConnection);
    connect(m_tp_game, SIGNAL(receivedMove(QString&)), SLOT(receivedMove(QString&)), Qt::UniqueConnection);
    m_tp_game->hostGame();
}

void MNetworkGame::
joinGame()
{
    delete m_tp_game;
    m_tp_game = new TpGame::Game(this);
    connect(m_tp_game, SIGNAL(disconnected()), this, SIGNAL(disconnected()), Qt::UniqueConnection);
    connect(m_tp_game, SIGNAL(connected()), SIGNAL(connected()), Qt::UniqueConnection);
    connect(m_tp_game, SIGNAL(connected()), SLOT(joinGameConnected()), Qt::UniqueConnection);
    m_tp_game->joinGame();
}

void MNetworkGame::
disconnect()
{
    if(m_tp_game)
        delete m_tp_game;

    Q_EMIT disconnected();
}

void MNetworkGame::
setupDashboard()
{
    qDebug() << "MNetworkGame::setupDashboard()";

    Q_ASSERT(0 != m_board_view);

    m_board_view->addChatbox();
    m_board_view->addDashboard(MBoardView::ALIGN_BOTTOM);
    connectDashboardToGame(m_board_view->getDashboard(MBoardView::ALIGN_BOTTOM));

    m_dashboard = m_board_view->getDashboard(MBoardView::ALIGN_BOTTOM);
}

void MNetworkGame::
onWhiteToMove(const MPosition &position)
{
//    if (isWhiteAtBottom())
//    {
        m_board->enable();
        startTurn(position);
//    }
}

void MNetworkGame::
onBlackToMove(const MPosition &position)
{
//    if (!isWhiteAtBottom())
//    {
        m_board->enable();
        startTurn(position);
//    }
}

void MNetworkGame::
endTurn()
{
    m_board->disable();
    MGame::endTurn();
}

void MNetworkGame::
hostGameConnected()
{
    mIsWhiteAtBottom = true;
}

void MNetworkGame::
joinGameConnected()
{
    mIsWhiteAtBottom = false;

    QMessageBox msgBox;
    msgBox.setText("Select pieces color.");
    msgBox.setInformativeText("Do you want to play white or black?");
    QPushButton *whiteButton = msgBox.addButton(tr("White"), QMessageBox::ActionRole);
    QPushButton *blackButton = msgBox.addButton(tr("Black"), QMessageBox::ActionRole);
    msgBox.exec();

    if(msgBox.clickedButton() == whiteButton)
    {
        mIsWhiteAtBottom = true;
        newGame();
    }
    if(msgBox.clickedButton() == blackButton)
        newGame();
}

bool MNetworkGame::
isWhiteAtBottom() const
{
    return mIsWhiteAtBottom;
}

bool MNetworkGame::
isBlackAtBottom() const
{
    return !isWhiteAtBottom();
}

void MNetworkGame::
newGame()
{
    MGame::newGame();

    m_tp_game->sendNewGame(isWhiteAtBottom());

    m_board_view->enableAutoOrientationSupport();
}

void MNetworkGame::
receivedNewGame(bool whiteChoosed)
{
    mIsWhiteAtBottom = !whiteChoosed;
    newGame();
}

void MNetworkGame::
onConfirmButtonPressed()
{
    endTurn();
    MPosition pos = m_store->onCandidateMoveConfirmed();

    QString fenPos = pos.asFen();
    m_tp_game->sendMove(fenPos);
}

void MNetworkGame::
receivedMove(QString &fenPos)
{
    MPosition pos;
    pos = pos.fromFen(fenPos);
    qDebug() << "fenPos:" << pos.asFen();
    endTurn();
    m_store->onCandidateMoveConfirmed(pos);
}

