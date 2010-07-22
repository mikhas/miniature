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
    m_tp_game.reset(new TpGame::Game);

    connectTpGameSignals();

    connect(m_tp_game.get(), SIGNAL(connected()),
            this,            SLOT(onHostGameConnected()),
            Qt::UniqueConnection);

    connect(m_tp_game.get(), SIGNAL(newGameReceived(bool)),
            this,            SLOT(onNewGameReceived(bool)),
            Qt::UniqueConnection);

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
    m_tp_game.reset(new TpGame::Game);
    connectTpGameSignals();

    connect(m_tp_game.get(), SIGNAL(connected()),
            this,            SLOT(onJoinGameConnected()),
            Qt::UniqueConnection);

    m_tp_game->joinGame();
}

void MNetworkGame::
disconnect()
{
    m_tp_game.reset(0);
    Q_EMIT disconnected();
}

void MNetworkGame::
connectTpGameSignals()
{
    connect(m_tp_game.get(), SIGNAL(disconnected()),
            this,            SIGNAL(disconnected()),
            Qt::UniqueConnection);

    connect(m_tp_game.get(), SIGNAL(connected()),
            this,            SIGNAL(connected()),
            Qt::UniqueConnection);

    connect(m_tp_game.get(), SIGNAL(newGameReceived(bool)),
            this,            SLOT(onNewGameReceived(bool)),
            Qt::UniqueConnection);

    connect(m_tp_game.get(), SIGNAL(moveReceived(QString)),
            this,            SLOT(onMoveReceived(QString)),
            Qt::UniqueConnection);
}

void MNetworkGame::
setupDashboard()
{
    qDebug() << "MNetworkGame::setupDashboard()";

    Q_ASSERT(m_board_view);

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
onHostGameConnected()
{
    m_white_at_bottom = true;
}

void MNetworkGame::
onJoinGameConnected()
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
onNewGameReceived(bool has_white_chosen)
{
    m_white_at_bottom = !has_white_chosen;
    newGame();
}

void MNetworkGame::
onConfirmButtonPressed()
{
    endTurn();
    m_store->onCandidateMoveConfirmed();
    m_tp_game->sendMove(m_store->getCurrentPosition().asFen());
}

void MNetworkGame::
onMoveReceived(const QString &fen)
{
    MPosition pos(pos.fromFen(fen));
    qDebug() << "fenPos:" << pos.asFen();
    endTurn();
    m_store->onCandidateMoveConfirmed(pos);
}
