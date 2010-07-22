/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2010 Collabora Ltd. <http://www.collabora.co.uk/>
 * @author Dariusz Mikulski <dariusz.mikulski@collabora.co.uk>
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

#ifndef TPGAME_H
#define TPGAME_H

#include <config.h>

#include "tpaccountmanager.h"
#include "accountselectiondlg.h"

#ifdef HAVE_MAEMOCONTACTSELECTOR
#include <gtk/gtk.h>
#include <libosso-abook/osso-abook.h>
#endif

#include <TelepathyQt4/shared-ptr.h>
#include <TelepathyQt4/ClientRegistrar>
#include <TelepathyQt4/Account>
#include <TelepathyQt4/PendingChannelRequest>

#include <QtCore>
#include <QTcpSocket>
#include <QDataStream>

namespace TpGame
{

class TubeClient;

class Game
    : public QObject
{
    Q_OBJECT

public:
    Game(QObject *parent = 0);
    ~Game();

public Q_SLOTS:
    void hostGame(TubeClient *, const Tp::ContactPtr &);
    void joinGame();
    void setupOutgoingTube(TubeClient *, const Tp::ContactPtr &contact);

    void sendNewGame(bool whiteChoosed = false);
    void sendNewGameAccept();
    void sendMove(const QString &fenPos);
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

    void receivedNewGame(bool whiteChoosed);
    void receivedNewGameAccept();
    void receivedMove(const QString &fenPos);
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
    void readPackets();
    void tubeClientDestroyed();
    void onPendingChannelRequestFinished(Tp::PendingOperation *op);

private:
    //void initConnections(TubeClient*);
#ifndef HAVE_MAEMOCONTACTSELECTOR
    AccountManager *m_account_manager;
    AccountSelectionDlg *m_accounts_dialog;
#endif
    TubeClient *mClient;
    QDataStream mStream;

#ifdef HAVE_MAEMOCONTACTSELECTOR
public:
    // This is private really, but mixing GLib and QT does not allow it to
    // be  private
    void delete_event_window();
    GtkWidget *contact_window;
    GtkWidget *contact_view;
    OssoABookContact *selected_master_contact;
    OssoABookContact *selected_contact;
    Tp::AccountPtr requested_account;
    Tp::PendingChannelRequest *pcr;
#endif
};

} // namespace TpGame

#endif // TPGAME_H
