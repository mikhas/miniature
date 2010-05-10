/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2010 Michael Hasselmann <michael@taschenorakel.de>
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

#include <pregame.h>

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
    : QObject(parent),
      m_main(&m_log),
      m_local_game(tr("Local Game"),
                   QPixmap(local_game_filename),
                   &m_main),
      m_host_game(tr("Host P2P Game"),
                  QPixmap(host_game_filename),
                  &m_main),
      m_join_game(tr("Join P2P Game"),
                  QPixmap(join_game_filename),
                  &m_main),
      m_fics_game(tr("Join FICS Game"),
                  QPixmap(fics_game_filename),
                  &m_main)
{}

MPreGame::
~MPreGame()
{}

void MPreGame::
onStartScreenRequested()
{
    QWidget *central = new QWidget;
    QVBoxLayout *vbox = new QVBoxLayout;
    central->setLayout(vbox);
    vbox->setAlignment(Qt::AlignCenter);

    QWidget *buttons = new QWidget;
    vbox->addWidget(buttons);

    QHBoxLayout *hbox = new QHBoxLayout;
    buttons->setLayout(hbox);

    hbox->addWidget(m_local_game.getButton());
    hbox->addWidget(m_host_game.getButton());
    hbox->addWidget(m_join_game.getButton());
    hbox->addWidget(m_fics_game.getButton());

    MMainWindow::setupPreGameUi(&m_main, buttons);
    m_main.show();

    connect(m_local_game.getButton(), SIGNAL(pressed()),
            this,                     SLOT(onStartLocalGame()),
            Qt::UniqueConnection);

    connect(m_host_game.getButton(), SIGNAL(pressed()),
            this,                    SLOT(onHostGame()),
            Qt::UniqueConnection);

    connect(m_join_game.getButton(), SIGNAL(pressed()),
            this,                    SLOT(onJoinGame()),
            Qt::UniqueConnection);

    m_log.append("Miniature started.", MGameLog::PREGAME);
}

void MPreGame::
setupGame(MGameConfig *config)
{
    // Prevent further signal emission - we only want one game to be started!
    config->getButton()->disable();

    MMainWindow::setupGameUi(config->getWindow(), config->getBoardView());
    config->getWindow()->show();
    config->getGame()->newGame();
    m_main.hide();

    connect(config->getGame(),   SIGNAL(destroyed()),
            config->getWindow(), SLOT(close()),
            Qt::UniqueConnection);

    connect(config->getGame(), SIGNAL(destroyed()),
            &m_main,           SLOT(show()),
            Qt::UniqueConnection);

    // Allow signal emission from button only after the game is gone.
    connect(config->getGame(),   SIGNAL(destroyed()),
            config->getButton(), SLOT(enable()),
            Qt::UniqueConnection);
}

void MPreGame::
onStartLocalGame()
{
    m_local_game.setGame(new MLocalGame(&m_log));
    setupGame(&m_local_game);

    m_local_game.getWindow()->setAttribute(Qt::WA_Maemo5PortraitOrientation, true);
    m_local_game.getBoardView()->applyPortraitLayout();
}

void MPreGame::
onHostGame()
{
    m_host_game.setGame(new MNetworkGame(&m_log));
    setupGame(&m_host_game);

    static_cast<MNetworkGame*>(m_host_game.getGame())->hostGame();
    m_host_game.getBoardView()->enableAutoOrientationSupport();
}

void MPreGame::
onJoinGame()
{
    m_join_game.setGame(new MNetworkGame(&m_log));
    setupGame(&m_join_game);

    static_cast<MNetworkGame*>(m_join_game.getGame())->joinGame();
    m_join_game.getBoardView()->enableAutoOrientationSupport();
}

void MPreGame::
onJoinFicsGame()
{
    // TODO: implement!
}

MPreGame::MGameConfig::
MGameConfig(const QString &button_label,const QPixmap &button_icon, MMainWindow *main)
    : m_main(main),
      m_window(0),
      m_game(0),
      m_button(new MIconicButton(button_icon, button_label, main))
{
    Q_ASSERT(0 != m_main);
    Q_ASSERT(0 != m_button);
}

MPreGame::MGameConfig::
~MGameConfig()
{}

void MPreGame::MGameConfig::
setGame(MGame *game)
{
    delete m_game;
    m_game = game;

    delete m_window;
    m_window = new QMainWindow(m_main);

    m_game->setParent(m_window);
    m_game->setBoardView(new MBoardView(m_window));
}

QMainWindow * MPreGame::MGameConfig::
getWindow() const
{
    return m_window;
}

MBoardView * MPreGame::MGameConfig::
getBoardView() const
{
    return m_game->getBoardView();
}

MGame * MPreGame::MGameConfig::
getGame() const
{
    return m_game;
}

MIconicButton * MPreGame::MGameConfig::
getButton() const
{
    return m_button;
}
