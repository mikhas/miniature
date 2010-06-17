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

#ifndef TPGAME_H
#define TPGAME_H

#include "tpaccountmanager.h"
#include "accountselectiondlg.h"

#include <TelepathyQt4/ClientRegistrar>
#include <QtCore>
#include <QTcpSocket>

namespace TpGame
{

class TubeClient;

class Game
    : public QObject
{
    Q_OBJECT

public:
    Game(QObject *parent = 0);

public Q_SLOTS:
    void hostGame();
    void joinGame();

    void sendNewGame();
    void sendNewGameAccept();
    void sendMove();
    void sendTakeBack();
    void sendTakeBackAccept();
    void sendDraw();
    void sendDrawAccept();
    void sendResign();
    void sendAdjourn();
    void sendAdjournAccept();

Q_SIGNALS:
    void initialized();
    void disconnected();
    void connected();

    void receivedNewGame();
    void receivedNewGameAccept();
    void receivedMove();
    void receivedTakeBack();
    void receivedTakeBackAccept();
    void receivedDraw();
    void receivedDrawAccept();
    void receivedResign();
    void receivedAdjourn();
    void receivedAdjournAccept();

private Q_SLOTS:
    void onAccountNamesChanged(const QStringList &account_names);
    void newIncomingTube(TubeClient *, const Tp::ContactPtr &contact);
    void newOutgoingTube(TubeClient *, const Tp::ContactPtr &contact);

private:
    void initConnections(TubeClient*);
    AccountManager *m_account_manager;
    AccountSelectionDlg *m_accounts_dialog;
    Tp::ClientRegistrarPtr m_client_registrar;
    TubeClient *mClient;
};

} // namespace TpGame

#endif // TPGAME_H
