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

#include <QTcpSocket>

namespace TpGame
{

Game::Game(QObject *parent)
    : QObject(parent),
      m_account_manager(new AccountManager(this)),
      m_accounts_dialog(new AccountSelectionDlg(m_account_manager)),
      m_client_registrar(Tp::ClientRegistrar::create())
{
    qDebug() << "Registering client handler.";

    Tp::SharedPtr<TpTubesClientHandler> client = Tp::SharedPtr<TpTubesClientHandler>(new TpTubesClientHandler(0));

    connect(client.data(), SIGNAL(newIncomingTube(QTcpSocket *, const Tp::ContactPtr &)),
            this,          SLOT(newIncomingTube(QTcpSocket *, const Tp::ContactPtr &)),
            Qt::UniqueConnection);

    connect(client.data(), SIGNAL(newOutgoingTube(QTcpSocket *, const Tp::ContactPtr &)),
            this,          SLOT(newOutgoingTube(QTcpSocket *, const Tp::ContactPtr &)),
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

void Game::newIncomingTube(QTcpSocket *, const Tp::ContactPtr &)
{
    // TODO: impl
    qDebug() << "TpGame::newIncomingTube()";

    Q_EMIT connected();
}

void Game::newOutgoingTube(QTcpSocket *, const Tp::ContactPtr &)
{
    // TODO: impl
    qDebug() << "TpGame::newOutgoingTube()";

    Q_EMIT connected();
}

} // namespace TpGame
