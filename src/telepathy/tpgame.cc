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

namespace Miniature
{

TpGame::TpGame(QObject *parent) : QObject(parent)
{
    mTpAccountManager = new TpAccountManager(this);
    mAccountsDialog = new AccountSelectionDlg(mTpAccountManager);

    qDebug() << "Registering client handler.";
    mClientRegistrar = Tp::ClientRegistrar::create();
    
    Tp::SharedPtr<TpTubesClientHandler> client = Tp::SharedPtr<TpTubesClientHandler>(new TpTubesClientHandler(0));
    mClientRegistrar->registerClient(Tp::AbstractClientPtr::dynamicCast(client), "miniature_handler");

    Tp::SharedPtr<TpTextClientHandler> textClient = Tp::SharedPtr<TpTextClientHandler>(new TpTextClientHandler(0));
    mClientRegistrar->registerClient(Tp::AbstractClientPtr::dynamicCast(textClient), "miniature_text_handler");

//    Tp::SharedPtr<TpApproverManager> approverManager;
//    approverManager = Tp::SharedPtr<TpApproverManager>(new TpApproverManager(0));
//    mClientRegistrar->registerClient(Tp::AbstractClientPtr::dynamicCast(approverManager), "miniature_approver");
}

void TpGame::hostGame()
{
    qDebug() << "TpGame::hostGame()";
    mTpAccountManager->disconnect();
    QObject::connect(mTpAccountManager, SIGNAL(onAccountNameListChanged(const QList<QString>)), this, SLOT(onAccountNameListChanged(const QList<QString>)));
}

void TpGame::onAccountNameListChanged(const QList<QString> accounts)
{
    qDebug() << "TpGame::onAccountNameListChanged()";
    
    Q_FOREACH(QString accountName, accounts)
    {
        mTpAccountManager->ensureContactListForAccount(accountName);
    }

    Q_EMIT initialized();
}

void TpGame::joinGame()
{
    qDebug() << "TpGame::joingGame()";
    mAccountsDialog->show();

    Q_EMIT initialized();
}

};
