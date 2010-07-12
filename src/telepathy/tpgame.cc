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

#include <config.h>
#include "tpgame.h"

#include "tpapprovermanager.h"
#include "tptubesclienthandler.h"
#include "tptextclienthandler.h"
#include "tptubeclient.h"

#ifdef HAVE_MAEMOCONTACTSELECTOR
#include "contact-chooser.h"
#include <telepathy-glib/dbus.h>
#include <telepathy-glib/account.h>
#include <TelepathyQt4/Account>
#endif

#include <QTcpSocket>

namespace TpGame
{

Game::Game(QObject *parent)
    : QObject(parent),
#ifndef HAVE_MAEMOCONTACTSELECTOR
      m_account_manager(new AccountManager(this)),
      m_accounts_dialog(new AccountSelectionDlg(m_account_manager)),
#endif
      mClient(0)
#ifdef HAVE_MAEMOCONTACTSELECTOR
      ,
      contact_window(0),
      contact_view(0),
      selected_master_contact(0),
      selected_contact(0),
      requested_account(0),
      pcr(0)
#endif
{
}

Game::~Game()
{
    qDebug() << "Game::~Game()";
    if (mClient)
    {
        mClient->close();
        mClient = 0;
    }
}

void Game::hostGame(TubeClient *tube_client, const Tp::ContactPtr &contact)
{
    qDebug() << "TpGame::hostGame()";
#ifndef HAVE_MAEMOCONTACTSELECTOR
    Q_UNUSED(tube_client);
    Q_UNUSED(contact);
    m_account_manager->disconnect();
    connect(m_account_manager, SIGNAL(accountNamesChanged(QStringList)),
            this,              SLOT(onAccountNamesChanged(QStringList)),
            Qt::UniqueConnection);
#else
    newIncomingTube(tube_client, contact);
#endif
}

void Game::onAccountNamesChanged(const QStringList &account_names)
{
    qDebug() << "TpGame::onAccountNameListChanged()";

    Q_FOREACH(QString name, account_names)
    {
#ifndef HAVE_MAEMOCONTACTSELECTOR
        m_account_manager->ensureContactListForAccount(name);
#endif
    }

    Q_EMIT initialized();
}

void Game::onPendingChannelRequestFinished(
        Tp::PendingOperation *op)
{
    if(op->isError())
    {
        qDebug() << "Game::onPendingChannelRequestFinished failure: " << op->errorName();
        return;
    }
    qDebug() << "Game::onPendingChannelRequestFinished success";
#ifdef HAVE_MAEMOCONTACTSELECTOR
  gtk_widget_destroy (contact_window);
  contact_window = NULL;
#endif
}

#ifdef HAVE_MAEMOCONTACTSELECTOR
static void
contact_activated_cb (OssoABookContactView *view,
    GtkTreePath *path G_GNUC_UNUSED,
    gpointer data)
{
  GList *selection;
  OssoABookContact *contact, *roster_contact = NULL;
  McAccount *mcaccount;
  GList *roster, *l;
  guint count = 0;
  Game *game = (Game *)data;

  qDebug() << "Contact selected";

  selection = osso_abook_contact_view_get_selection (view);

  /* Only one contact is allowed to be selected at one time. */
  contact = (OssoABookContact *) selection->data;

  roster = osso_abook_contact_get_roster_contacts (contact);

  for (l = roster; l != NULL; l = l->next)
    {
      if (miniature_contact_chooser_can_send_to_roster_contact (
              MINIATURE_CONTACT_CHOOSER (game->contact_view),
              OSSO_ABOOK_CONTACT (l->data)))
        {
          count++;
          roster_contact = (OssoABookContact *)l->data;
          qDebug ("Selected contact: %s", osso_abook_contact_get_bound_name (roster_contact));
        }
    }

  qDebug("Contact selected: %d roster items", count);

  if (count == 0)
    {
      g_list_free (selection);
      return;
    }
  if (game->selected_contact != NULL)
    g_object_unref (game->selected_contact);

  if (game->selected_master_contact != NULL)
    g_object_unref (game->selected_master_contact);

  game->selected_contact = (OssoABookContact *) g_object_ref (roster_contact);
  game->selected_master_contact = (OssoABookContact *) g_object_ref (contact);

  mcaccount = osso_abook_contact_get_account (game->selected_contact);
  TpDBusDaemon *dbus = tp_dbus_daemon_dup (NULL);
  TpAccount *tpaccount = tp_account_new (dbus, tp_proxy_get_object_path (mcaccount), NULL);
  game->requested_account = Tp::Account::create
      (tp_proxy_get_bus_name (tpaccount), tp_proxy_get_object_path (mcaccount));
  g_object_unref (dbus);

  QVariantMap req;
  req.insert(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".ChannelType"),
             TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAM_TUBE);
  req.insert(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".TargetHandleType"),
             Tp::HandleTypeContact);
  req.insert(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".TargetID"),
             QLatin1String(osso_abook_contact_get_bound_name (roster_contact)));
  req.insert(QLatin1String(TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAM_TUBE ".Service"),
             "Miniature");

  game->pcr = game->requested_account->ensureChannel(req);
  QObject::connect(game->pcr, SIGNAL(finished(Tp::PendingOperation *)),
          game, SLOT(onPendingChannelRequestFinished(Tp::PendingOperation *)));

  g_list_free (selection);
}

static gboolean
delete_event_window_cb (GtkWidget *window G_GNUC_UNUSED,
                        GdkEvent *event G_GNUC_UNUSED,
                        gpointer data)
{
  Game *game = (Game *)data;
  game->delete_event_window();
  return FALSE;
}

void Game::delete_event_window()
{
  Q_EMIT disconnected();
}
#endif

void Game::joinGame()
{
    qDebug() << "TpGame::joingGame()";

#ifdef HAVE_MAEMOCONTACTSELECTOR
    GtkWidget *vbox;

    contact_window = hildon_stackable_window_new ();
    gtk_window_set_title (GTK_WINDOW (contact_window), "Select a contact");
    gtk_container_set_border_width (GTK_CONTAINER (contact_window), 5);

    vbox = gtk_vbox_new (FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (vbox), 0);

    contact_view = miniature_contact_chooser_new ();
    g_signal_connect (contact_view, "contact-activated",
        G_CALLBACK (contact_activated_cb), (gpointer) this);
    gtk_box_pack_start (GTK_BOX (vbox), contact_view, TRUE, TRUE, 0);
    gtk_widget_show (contact_view);

    g_signal_connect (G_OBJECT (contact_window), "delete-event",
        G_CALLBACK (delete_event_window_cb), (gpointer) this);

    gtk_container_add (GTK_CONTAINER (contact_window), vbox);
    gtk_widget_show (vbox);
    gtk_widget_show (contact_window);
#else
    m_accounts_dialog->show();
#endif
    
    Q_EMIT initialized();
}

void Game::tubeClientDestroyed()
{
    mClient = 0;
}

void Game::newIncomingTube(TubeClient *client, const Tp::ContactPtr &)
{
    // TODO: impl
    qDebug() << "TpGame::newIncomingTube()";

    mClient = client;
    mStream.setDevice(mClient);

    connect(mClient, SIGNAL(readyToTransfer()), SLOT(readPackets()));
    connect(mClient, SIGNAL(destroyed()), SLOT(tubeClientDestroyed()));

    Q_EMIT connected();
}

void Game::setupOutgoingTube(TubeClient *client, const Tp::ContactPtr &)
{
    // TODO: impl
    qDebug() << "TpGame::newOutgoingTube()";

    mClient = client;
    mStream.setDevice(mClient);

    connect(mClient, SIGNAL(readyToTransfer()), SLOT(readPackets()));
    connect(mClient, SIGNAL(destroyed()), SLOT(tubeClientDestroyed()));

#ifndef HAVE_MAEMOCONTACTSELECTOR
    m_accounts_dialog->hide();
#endif

    Q_EMIT connected();
}

void Game::readPackets()
{
    qDebug() << "Game::readPackets()";

    if (mClient->bytesAvailable() == 0)
    {
        qDebug() << "Game::readPackets()";
        return;
    }

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
            bool whiteChoosed = false;
            mStream >> whiteChoosed;

            qDebug() << testString;

            Q_EMIT receivedNewGame(whiteChoosed);

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

            QString fenPos;
            mStream >> fenPos;

            qDebug() << "Fen position:" << fenPos;

            Q_EMIT receivedMove(fenPos);

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

    /* read next packet if there is still packets to read */
    readPackets();
}

void Game::sendNewGame(bool whiteChoosed)
{
    qDebug() << "Game::sendNewGame()";

    if(!mClient)
        return;

    mStream << commandToString(TpGame::NewGame);
    mStream << QString("Start new game");
    mStream << whiteChoosed;
}

void Game::sendNewGameAccept()
{
    qDebug() << "Game::sendNewGameAccept()";

    if(!mClient)
        return;

    mStream << commandToString(TpGame::NewGameAccept);
    mStream << QString("New game accept");
}

void Game::sendMove(QString &fenPos)
{
    qDebug() << "Game::sendMove()" << fenPos;

    if(!mClient)
        return;

    mStream << commandToString(TpGame::Move);
    mStream << fenPos;
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
