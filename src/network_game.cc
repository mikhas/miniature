/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2010 Michael Hasselmann <michael@taschenorakel.de>
 * Copyright (C) 2010 Collabora Ltd. <http://www.collabora.co.uk/>
 * @author Michael Hasselmann <michael@taschenorakel.de>
 * @author Alban Crequy <alban.crequy@collabora.co.uk>
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
MNetworkGame(const MSharedGameLog &log, QObject *parent)
    : MGame(log, parent)
    , m_tp_game(0)
    , m_white_at_bottom(true)
{}

MNetworkGame::
~MNetworkGame()
{}

void MNetworkGame::
hostGame(TpGame::TubeClient *tube_client, const Tp::ContactPtr &contact)
{
    qDebug() << "MNetworkGame::hostGame()";
    delete m_tp_game;
    m_tp_game = new TpGame::Game(this);
    connect(m_tp_game, SIGNAL(disconnected()), this, SIGNAL(disconnected()), Qt::UniqueConnection);
    connect(m_tp_game, SIGNAL(connected()), SIGNAL(connected()), Qt::UniqueConnection);
    connect(m_tp_game, SIGNAL(connected()), SLOT(hostGameConnected()), Qt::UniqueConnection);
    connect(m_tp_game, SIGNAL(receivedNewGame(bool)), SLOT(receivedNewGame(bool)), Qt::UniqueConnection);
    connect(m_tp_game, SIGNAL(receivedMove(const QString&)), SLOT(receivedMove(const QString&)), Qt::UniqueConnection);
    m_tp_game->hostGame(tube_client, contact);
}

void MNetworkGame::
setupOutgoingTube(TpGame::TubeClient *tube_client, const Tp::ContactPtr &contact)
{
    qDebug() << "MNetworkGame::setupOutgoingTube()";
    m_tp_game->setupOutgoingTube(tube_client, contact);
}

void MNetworkGame::
joinGame()
{
    delete m_tp_game;
    m_tp_game = new TpGame::Game(this);
    connect(m_tp_game, SIGNAL(disconnected()), this, SIGNAL(disconnected()), Qt::UniqueConnection);
    connect(m_tp_game, SIGNAL(connected()), SIGNAL(connected()), Qt::UniqueConnection);
    connect(m_tp_game, SIGNAL(connected()), SLOT(joinGameConnected()), Qt::UniqueConnection);
    connect(m_tp_game, SIGNAL(receivedMove(const QString&)), SLOT(receivedMove(const QString&)), Qt::UniqueConnection);
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
    m_white_at_bottom = true;
}

void MNetworkGame::
joinGameConnected()
{
    m_white_at_bottom = false;

    QMessageBox msgBox;
    msgBox.setText("Select pieces color.");
    msgBox.setInformativeText("Do you want to play white or black?");
    QPushButton *whiteButton = msgBox.addButton(tr("White"), QMessageBox::ActionRole);
    QPushButton *blackButton = msgBox.addButton(tr("Black"), QMessageBox::ActionRole);
    msgBox.exec();

    if(msgBox.clickedButton() == whiteButton)
    {
        m_white_at_bottom = true;
        newGame();
    }
    if(msgBox.clickedButton() == blackButton)
        newGame();
}

bool MNetworkGame::
isWhiteAtBottom() const
{
    return m_white_at_bottom;
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
receivedNewGame(bool white_chosed)
{
    m_white_at_bottom = !white_chosed;
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
receivedMove(const QString &fen)
{
    MPosition pos(pos.fromFen(fen));
    qDebug() << "fenPos:" << pos.asFen();
    endTurn();
    m_store->onCandidateMoveConfirmed(pos);
}
