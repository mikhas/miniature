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

    initConnections(client);

    Q_EMIT connected();
}

void Game::newOutgoingTube(TubeClient *client, const Tp::ContactPtr &)
{
    // TODO: impl
    qDebug() << "TpGame::newOutgoingTube()";

    initConnections(client);

    Q_EMIT connected();
}

void Game::initConnections(TubeClient *client)
{
    if(!client)
        return;

    mClient = client;

    connect(mClient, SIGNAL(receivedNewGame()), SIGNAL(receivedNewGame()), Qt::UniqueConnection);
    connect(mClient, SIGNAL(receivedNewGameAccept()), SIGNAL(receivedNewGameAccept()), Qt::UniqueConnection);
    connect(mClient, SIGNAL(receivedMove()), SIGNAL(receivedMove()), Qt::UniqueConnection);
    connect(mClient, SIGNAL(receivedTakeBack()), SIGNAL(receivedTakeBack()), Qt::UniqueConnection);
    connect(mClient, SIGNAL(receivedTakeBackAccept()), SIGNAL(receivedTakeBackAccept()), Qt::UniqueConnection);
    connect(mClient, SIGNAL(receivedDraw()), SIGNAL(receivedDraw()), Qt::UniqueConnection);
    connect(mClient, SIGNAL(receivedDrawAccept()), SIGNAL(receivedDrawAccept()), Qt::UniqueConnection);
    connect(mClient, SIGNAL(receivedResign()), SIGNAL(receivedResign()), Qt::UniqueConnection);
    connect(mClient, SIGNAL(receivedAdjourn()), SIGNAL(receivedAdjournAccept()), Qt::UniqueConnection);
}

void Game::sendNewGame()
{
    if(mClient)
        mClient->sendNewGame();
}

void Game::sendNewGameAccept()
{
    if(mClient)
        mClient->sendNewGameAccept();
}

void Game::sendMove()
{
    if(mClient)
        mClient->sendMove();
}

void Game::sendTakeBack()
{
    if(mClient)
        mClient->sendTakeBack();
}

void Game::sendTakeBackAccept()
{
    if(mClient)
        mClient->sendTakeBackAccept();
}

void Game::sendDraw()
{
    if(mClient)
        mClient->sendDraw();
}

void Game::sendDrawAccept()
{
    if(mClient)
        mClient->sendDrawAccept();
}

void Game::sendResign()
{
    if(mClient)
        mClient->sendResign();
}

void Game::sendAdjourn()
{
    if(mClient)
        mClient->sendAdjourn();
}

void Game::sendAdjournAccept()
{
    if(mClient)
        mClient->sendAdjournAccept();
}

} // namespace TpGame
