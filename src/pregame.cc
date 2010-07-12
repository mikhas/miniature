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

#include <config.h>

#include <pregame.h>
#include <telepathy/tptubesclienthandler.h>
#include <telepathy/tpapprovermanager.h>

using namespace Miniature;

namespace
{
    const QLatin1String path_prefix("/usr/share/themes/alpha/backgrounds/");
    const QString local_game_filename(path_prefix + "app_install_games.png");
    const QString host_game_filename(path_prefix + "app_install_network.png");
    const QString join_game_filename(path_prefix + "app_install_network.png");
    const QString fics_game_filename(path_prefix + "app_install_network.png");
}

MPreGame::
MPreGame(QObject *parent)
    : QObject(parent)
    , m_log(new MGameLog)
    , m_main(new MMainWindow(m_log))
    , m_game(0)
    , m_window(0)
    , m_wait_contact_ui(new Ui_WaitContactWidget)
    , m_client_registrar(Tp::ClientRegistrar::create())
{
    /* Telepathy handlers for network games */
    Tp::SharedPtr<TpGame::TpTubesClientHandler> client =
            Tp::SharedPtr<TpGame::TpTubesClientHandler>(new TpGame::TpTubesClientHandler(0));

    connect(client.data(), SIGNAL(newIncomingTube(TpGame::TubeClient *, const Tp::ContactPtr &)),
            this,          SLOT(newIncomingTube(TpGame::TubeClient *, const Tp::ContactPtr &)),
            Qt::UniqueConnection);

    connect(client.data(), SIGNAL(newOutgoingTube(TpGame::TubeClient *, const Tp::ContactPtr &)),
            this,          SLOT(newOutgoingTube(TpGame::TubeClient *, const Tp::ContactPtr &)),
            Qt::UniqueConnection);

    connect(client.data(), SIGNAL(newOutgoingChannel(const char *)),
            this,          SLOT(newOutgoingChannel(const char *)),
            Qt::UniqueConnection);

    connect(client.data(), SIGNAL(disconnected()),
            this, SLOT(enableCentralMenu()),
            Qt::UniqueConnection);

    m_client_registrar->registerClient(Tp::AbstractClientPtr::dynamicCast(client), "Miniature");

    Tp::SharedPtr<TpGame::TpApproverManager> approverManager;
    approverManager = Tp::SharedPtr<TpGame::TpApproverManager>(new TpGame::TpApproverManager(0));
    m_client_registrar->registerClient(Tp::AbstractClientPtr::dynamicCast(approverManager), "MiniatureApprover");
}

MPreGame::
~MPreGame()
{}

void MPreGame::
enableCentralMenu()
{
    /* Remove previous games if any */
    // TODO: Fix the object ownership so we dont need to use deleteLater at all.
    if (m_game != 0)
    {
        m_game->deleteLater();
        m_game = 0;
    }
    if (m_window != 0)
    {
        m_window->deleteLater();
        m_window = 0;
    }

    /* setup the menu UI */
    m_local_game_button = new MIconicButton(QPixmap(local_game_filename),
        tr("Local Game"));
    m_join_game_button = new MIconicButton(QPixmap(join_game_filename),
        tr("Join P2P Game"));
    m_fics_game_button = new MIconicButton(QPixmap(fics_game_filename),
        tr("Join FICS Game"));

    QWidget *central_menu = new QWidget;
    QVBoxLayout *vbox = new QVBoxLayout;
    central_menu->setLayout(vbox);
    vbox->setAlignment(Qt::AlignCenter);

    QWidget *buttons = new QWidget;
    vbox->addWidget(buttons);

    QLayout *box = 0;
    box = (m_main->width() >= m_main->height() ? static_cast<QLayout *>(new QHBoxLayout) 
                                               : static_cast<QLayout *>(new QVBoxLayout));
    buttons->setLayout(box);

    box->addWidget(m_local_game_button);
    box->addWidget(m_join_game_button);
    // TODO: Get FICS mode to work!
    // box->addWidget(m_fics_game_button);

    MMainWindow::setupPreGameUi(m_main, central_menu);

    connect(m_local_game_button, SIGNAL(pressed()),
            this,         SLOT(onStartLocalGame()),
            Qt::UniqueConnection);

    connect(m_join_game_button, SIGNAL(pressed()),
            this,        SLOT(onJoinGame()),
            Qt::UniqueConnection);
}

void MPreGame::
enableCentralWaitContact()
{
    QWidget *central_wait_contact = new QWidget;
    m_wait_contact_ui->setupUi(central_wait_contact);

    MMainWindow::setupPreGameUi(m_main, central_wait_contact);

    connect(m_wait_contact_ui->cancelButton, SIGNAL(pressed()),
            this,        SLOT(onCancelButton()),
            Qt::UniqueConnection);
}

void MPreGame::
windowDestroyed()
{
    m_window = 0;
}

void MPreGame::
gameDestroyed()
{
    m_game = 0;
    enableCentralMenu();
}

void MPreGame::
setupGame(MGame *game)
{
    if (m_game != 0)
        m_game->deleteLater();
    m_game = game;

    if (m_window != 0)
        m_window->deleteLater();
    m_window = new QMainWindow;

    m_game->setBoardView(new MBoardView(m_window));

    MMainWindow::setupGameUi(m_window, m_game->getBoardView());

    connect(m_window, SIGNAL(destroyed()),
            SLOT(windowDestroyed()),
            Qt::UniqueConnection);

    connect(m_game, SIGNAL(destroyed()),
            SLOT(gameDestroyed()),
            Qt::UniqueConnection);

    connect(m_game,   SIGNAL(destroyed()),
            m_window, SLOT(close()),
            Qt::UniqueConnection);

    connect(m_game, SIGNAL(destroyed()),
            m_main, SLOT(show()),
            Qt::UniqueConnection);

    connect(m_game, SIGNAL(disconnected()), SLOT(onStartScreenRequested()), Qt::UniqueConnection);
    connect(m_game, SIGNAL(connected()), SLOT(runGame()), Qt::UniqueConnection);
}

void MPreGame::
runGame()
{
    m_window->show();
    m_main->hide();
}

void MPreGame::
onStartScreenRequested()
{
    enableCentralMenu();
    m_main->show();
}

void MPreGame::
onStartLocalGame()
{
    setupGame(new MLocalGame(m_log));

    m_window->show();
    m_main->hide();
    m_game->newGame();

#ifdef Q_WS_MAEMO_5
    m_window->setAttribute(Qt::WA_Maemo5PortraitOrientation, true);
#endif

    m_game->getBoardView()->applyPortraitLayout();
}

void MPreGame::
onJoinGame()
{
    setupGame(new MNetworkGame(m_log));

    // TODO: Let's move this in the MGame interface, to avoid the dynamic cast:
    MNetworkGame *game = qobject_cast<MNetworkGame*>(m_game);
    if(!game)
    {
        qWarning() << "This should never happen!";
        enableCentralMenu();
    }

    game->joinGame();

#ifdef HAVE_MAEMOCONTACTSELECTOR
    m_main->hide();
#endif
}

void MPreGame::
onCancelButton()
{
    enableCentralMenu();
    m_main->show();
}

void MPreGame::newOutgoingChannel(const char *name)
{
    qDebug() << "Show waiting window here";
    qDebug() << "Name: " << name;
    enableCentralWaitContact();
    m_main->show();
}

void MPreGame::newOutgoingTube(TpGame::TubeClient *client, const Tp::ContactPtr &contact)
{
    qDebug() << "MPreGame::newOutgoingTube()";
    /* m_game is already setup by onJoinGame() */
    if (!m_game)
    {
        qWarning() << "MPreGame::newOutgoingTube: This should never happen!";
        return;
    }

    MNetworkGame *game = qobject_cast<MNetworkGame*>(m_game);
    game->setupOutgoingTube(client, contact);
}

void MPreGame::newIncomingTube(TpGame::TubeClient *client, const Tp::ContactPtr &contact)
{
    qDebug() << "MPreGame::newIncomingTube()";
    /* Prepare hosting games */
    setupGame(new MNetworkGame(m_log));
    
    MNetworkGame *game = qobject_cast<MNetworkGame*>(m_game);
    if(!game)
    {
        qWarning() << "This should never happen!";
    }
    game->hostGame(client, contact);
}

void MPreGame::
onJoinFicsGame()
{
    // TODO: implement!
}

