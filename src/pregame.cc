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
      m_game_config(&m_main)
{
    m_game_config.setGame(new MNetworkGame(&m_log));
    m_game_config.setupGame();

    MNetworkGame *game = qobject_cast<MNetworkGame*>(m_game_config.getGame());
    if(!game)
    {
        qWarning() << "This should never happen!";
    }

    game->hostGame();

    connect(&m_game_config, SIGNAL(restart()),
            this,          SLOT(onStartScreenRequested()),
            Qt::UniqueConnection);

    m_local_game = new MIconicButton(QPixmap(local_game_filename),
        tr("Local Game"));
    m_join_game = new MIconicButton(QPixmap(join_game_filename),
        tr("Join P2P Game"));
    m_fics_game = new MIconicButton(QPixmap(fics_game_filename),
        tr("Join FICS Game"));

    QWidget *central = new QWidget;
    QVBoxLayout *vbox = new QVBoxLayout;
    central->setLayout(vbox);
    vbox->setAlignment(Qt::AlignCenter);

    QWidget *buttons = new QWidget;
    vbox->addWidget(buttons);

    QHBoxLayout *hbox = new QHBoxLayout;
    buttons->setLayout(hbox);

    hbox->addWidget(m_local_game);
    hbox->addWidget(m_join_game);
    hbox->addWidget(m_fics_game);

    MMainWindow::setupPreGameUi(&m_main, central);

    connect(m_local_game, SIGNAL(pressed()),
            this,         SLOT(onStartLocalGame()),
            Qt::UniqueConnection);

    connect(m_join_game, SIGNAL(pressed()),
            this,        SLOT(onJoinGame()),
            Qt::UniqueConnection);
}

MPreGame::
~MPreGame()
{}

void MPreGame::
onStartScreenRequested()
{

    m_main.show();

    m_log.append("Miniature started.", MGameLog::PREGAME);
}

void MPreGame::
onStartLocalGame()
{
    m_game_config.setGame(new MLocalGame(&m_log));
    m_game_config.setupGame();
    m_game_config.runGame();
    m_game_config.getGame()->newGame();

#ifdef Q_WS_MAEMO_5
    m_game_config.getWindow()->setAttribute(Qt::WA_Maemo5PortraitOrientation, true);
#endif

    m_game_config.getBoardView()->applyPortraitLayout();
}

void MPreGame::
onJoinGame()
{
    m_game_config.setGame(new MNetworkGame(&m_log));
    m_game_config.setupGame();

    MNetworkGame *game = qobject_cast<MNetworkGame*>(m_game_config.getGame());
    if(!game)
    {
        qWarning() << "This should never happen!";
        onStartScreenRequested();
    }

    game->joinGame();
}

void MPreGame::
onJoinFicsGame()
{
    // TODO: implement!
}

