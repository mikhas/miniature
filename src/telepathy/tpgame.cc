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

#include "contact-chooser.h"

#include <TelepathyQt4/Account>

#include <telepathy-glib/dbus.h>
#include <telepathy-glib/account.h>

#include <QTcpSocket>

namespace TpGame
{

Game::Game(QObject *parent)
    : QObject(parent),
      m_account_manager(new AccountManager(this)),
      m_accounts_dialog(new AccountSelectionDlg(m_account_manager)),
      m_client_registrar(Tp::ClientRegistrar::create()),
      mClient(0),
      contact_window(0),
      contact_view(0),
      selected_master_contact(0),
      selected_contact(0),
      requested_account(0)
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

    m_client_registrar->registerClient(Tp::AbstractClientPtr::dynamicCast(client), "Miniature");

#ifdef ENABLE_CHAT_SESSION
    // TODO: client name should be a ctor arg
    // Do not handle text channels now: it would get all text channels and the
    // user's messages would be lost

    Tp::SharedPtr<TextClientHandler> textClient = Tp::SharedPtr<TextClientHandler>(new TextClientHandler(0));
    m_client_registrar->registerClient(Tp::AbstractClientPtr::dynamicCast(textClient), "miniature_text_handler");
#endif

    Tp::SharedPtr<TpApproverManager> approverManager;
    approverManager = Tp::SharedPtr<TpApproverManager>(new TpApproverManager(0));
    m_client_registrar->registerClient(Tp::AbstractClientPtr::dynamicCast(approverManager), "MiniatureApprover");
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

  game->requested_account->ensureChannel(req);

  g_list_free (selection);

  gtk_widget_destroy (game->contact_window);
  game->contact_window = NULL;
}

void Game::joinGame()
{
    qDebug() << "TpGame::joingGame()";

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

    gtk_container_add (GTK_CONTAINER (contact_window), vbox);
    gtk_widget_show (vbox);
    gtk_widget_show (contact_window);
    
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

    m_accounts_dialog->hide();

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
