/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2010 Collabora Ltd. <http://www.collabora.co.uk/>
 *              Dariusz Mikulski <dariusz.mikulski@collabora.co.uk>
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

#include "tpgame.h"
#include "tpapprovermanager.h"
#include "tptubesclienthandler.h"
#include "tptextclienthandler.h"
#include "tptubeclient.h"

#include <QTcpSocket>

namespace TpGame
{

Game::Game(QObject *parent)
    : QObject(parent),
      m_account_manager(new AccountManager(this)),
      m_accounts_dialog(new AccountSelectionDlg(m_account_manager)),
      m_client_registrar(Tp::ClientRegistrar::create()),
      mClient(0)
{
    qDebug() << "Registering client handler.";

    Tp::SharedPtr<TpTubesClientHandler> client = Tp::SharedPtr<TpTubesClientHandler>(new TpTubesClientHandler(0));

    connect(client.data(), SIGNAL(newIncomingTube(TubeClient *, const Tp::ContactPtr &)),
            this,          SLOT(newIncomingTube(TubeClient *, const Tp::ContactPtr &)),
            Qt::UniqueConnection);

    connect(client.data(), SIGNAL(newOutgoingTube(TubeClient *, const Tp::ContactPtr &)),
            this,          SLOT(newOutgoingTube(TubeClient *, const Tp::ContactPtr &)),
            Qt::UniqueConnection);

    connect(client.data(), SIGNAL(disconnected()), this, SIGNAL(disconnected()), Qt::UniqueConnection);

    // TODO: client name should be a ctor arg
    m_client_registrar->registerClient(Tp::AbstractClientPtr::dynamicCast(client), "miniature_handler");

    Tp::SharedPtr<TextClientHandler> textClient = Tp::SharedPtr<TextClientHandler>(new TextClientHandler(0));
    m_client_registrar->registerClient(Tp::AbstractClientPtr::dynamicCast(textClient), "miniature_text_handler");

//    Tp::SharedPtr<TpApproverManager> approverManager;
//    approverManager = Tp::SharedPtr<TpApproverManager>(new TpApproverManager(0));
//    mClientRegistrar->registerClient(Tp::AbstractClientPtr::dynamicCast(approverManager), "miniature_approver");
}

void Game::hostGame()
{
    qDebug() << "TpGame::hostGame()";
    m_account_manager->disconnect();
    connect(m_account_manager, SIGNAL(accountNamesChanged(QStringList)),
            this,              SLOT(onAccountNamesChanged(QStringList)),
            Qt::UniqueConnection);
}

void Game::onAccountNamesChanged(const QStringList &account_names)
{
    qDebug() << "TpGame::onAccountNameListChanged()";

    Q_FOREACH(QString name, account_names)
        m_account_manager->ensureContactListForAccount(name);

    Q_EMIT initialized();
}

void Game::joinGame()
{
    qDebug() << "TpGame::joingGame()";
    m_accounts_dialog->show();

    Q_EMIT initialized();
}

void Game::newIncomingTube(TubeClient *client, const Tp::ContactPtr &)
{
    // TODO: impl
    qDebug() << "TpGame::newIncomingTube()";

    mClient = client;
    mStream.setDevice(mClient);

    connect(mClient, SIGNAL(readyToTransfer()), SLOT(readPackets()));

    Q_EMIT connected();
}

void Game::newOutgoingTube(TubeClient *client, const Tp::ContactPtr &)
{
    // TODO: impl
    qDebug() << "TpGame::newOutgoingTube()";

    mClient = client;
    mStream.setDevice(mClient);

    connect(mClient, SIGNAL(readyToTransfer()), SLOT(readPackets()));

    sendNewGame(); // for testing only

    Q_EMIT connected();
}

void Game::readPackets()
{
    qDebug() << "Game::readPackets()";

    QString command;
    mStream >> command;

    TpGame::Command c = commandFromString(command);

    switch(c)
    {
        case TpGame::NewGame:
        {
            qDebug() << "Receiving NewGame";

            QString testString;
            mStream >> testString;

            qDebug() << testString;

            Q_EMIT receivedNewGame();

            sendNewGameAccept();
            break;
        }

        case TpGame::NewGameAccept:
        {
            qDebug() << "Receiving NewGameAccept";

            QString testString;
            mStream >> testString;

            qDebug() << testString;

            Q_EMIT receivedNewGameAccept();
            break;
        }

        case TpGame::Move:
        {
            qDebug() << "Receiving Move";

            Q_EMIT receivedMove();

            break;
        }

        case TpGame::TakeBack:
        {
            qDebug() << "Receiving TakeBack";

            Q_EMIT receivedTakeBack();

            break;
        }

        case TpGame::TakeBackAccept:
        {
            qDebug() << "Receiving TakeBackAccept";

            Q_EMIT receivedTakeBackAccept();

            break;
        }

        case TpGame::Draw:
        {
            qDebug() << "Receiving Draw";

            Q_EMIT receivedDraw();

            break;
        }

        case TpGame::DrawAccept:
        {
            qDebug() << "Receiving DrawAccept";

            Q_EMIT receivedDrawAccept();

            break;
        }

        case TpGame::Resign:
        {
            qDebug() << "Receiving Resign";

            Q_EMIT receivedResign();

            break;
        }

        case TpGame::Adjourn:
        {
            qDebug() << "Receiving Adjourn";

            Q_EMIT receivedAdjourn();

            break;
        }

        case TpGame::AdjournAccept:
        {
            qDebug() << "Receiving AdjournAccept";

            Q_EMIT receivedAdjournAccept();

            break;
        }

        case TpGame::Nop:
        {
            // nothing;
            break;
        }
    }
}

void Game::sendNewGame()
{
    qDebug() << "Game::sendNewGame()";

    if(!mClient)
        return;

    mStream << commandToString(TpGame::NewGame);
    mStream << QString("Start new game");
}

void Game::sendNewGameAccept()
{
    qDebug() << "Game::sendNewGameAccept()";

    if(!mClient)
        return;

    mStream << commandToString(TpGame::NewGameAccept);
    mStream << QString("New game accept");
}

void Game::sendMove()
{
    qDebug() << "Game::sendMove()";

    if(!mClient)
        return;

    mStream << commandToString(TpGame::Move);
}

void Game::sendTakeBack()
{
    qDebug() << "Game::sendTakeBack()";

    if(!mClient)
        return;

    mStream << commandToString(TpGame::TakeBack);
}

void Game::sendTakeBackAccept()
{
    qDebug() << "Game::sendTakeBackAccept()";

    if(!mClient)
        return;

    mStream << commandToString(TpGame::TakeBackAccept);
}

void Game::sendDraw()
{
    qDebug() << "Game::sendDraw()";

    if(!mClient)
        return;

    mStream << commandToString(TpGame::Draw);
}

void Game::sendDrawAccept()
{
    qDebug() << "Game::sendDrawAccept()";

    if(!mClient)
        return;

    mStream << commandToString(TpGame::DrawAccept);
}

void Game::sendResign()
{
    qDebug() << "Game::sendResign()";

    if(!mClient)
        return;

    mStream << commandToString(TpGame::Resign);
}

void Game::sendAdjourn()
{
    qDebug() << "Game::sendAdjourn()";

    if(!mClient)
        return;

    mStream << commandToString(TpGame::Adjourn);
}

void Game::sendAdjournAccept()
{
    qDebug() << "Game::sendAdjournAccept()";

    if(!mClient)
        return;

    mStream << commandToString(TpGame::AdjournAccept);
}

} // namespace TpGame
