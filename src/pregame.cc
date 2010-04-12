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

MPreGame::
MPreGame(QObject *parent)
: QObject(parent),
  m_log(new MGameLog(this)),
  m_main_window(new MMainWindow(m_log))
{}

MPreGame::
~MPreGame()
{
    delete m_main_window;
}

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

    MIconicButton *new_game = new MIconicButton(QPixmap("/usr/share/themes/alpha/backgrounds/app_install_games.png"),
                                                tr("Local Game"), m_main_window);
    hbox->addWidget(new_game);

    MIconicButton *new_p2p_game = new MIconicButton(QPixmap("/usr/share/themes/alpha/backgrounds/app_install_network.png"),
                                                    QString("P2P Game"), m_main_window);
    hbox->addWidget(new_p2p_game);

    MMainWindow::setupPreGameUi(m_main_window, buttons);
    m_main_window->show();

    connect(new_game, SIGNAL(pressed()),
            this, SLOT(onNewGameRequested()));

    connect(new_p2p_game, SIGNAL(pressed()),
            this, SLOT(onNewP2PGameRequested()));

    m_log->append("Miniature started.", MGameLog::PREGAME);
}

void MPreGame::
setupGame(QWidget *const source, QMainWindow *const window, MBoardView *const view, MGame *const game)
{
    // Prevent further signal emission - we only want one game to be started!
    source->setEnabled(false);

    game->newGame();

    MMainWindow::setupGameUi(window, view);
    window->show();

    connect(game, SIGNAL(destroyed()), window, SLOT(close()));
    connect(game, SIGNAL(destroyed()), m_main_window, SLOT(show()));

    // Allow signal emission only after this window is gone.
    QSignalMapper *map = new QSignalMapper(window);
    connect(game, SIGNAL(destroyed()), map, SLOT(map()));
    map->setMapping(game, source);
    connect(map, SIGNAL(mapped(QWidget *)), this, SLOT(enableWidget(QWidget *)));
}

void MPreGame::
onNewGameRequested()
{
    QMainWindow *window = new QMainWindow(m_main_window);
    MBoardView *view = new MBoardView(window);
    setupGame(static_cast<QWidget *>(sender()), window, view,
              new MLocalGame(view, m_log, window));
}

void MPreGame::
onNewP2PGameRequested()
{
    QMainWindow *window = new QMainWindow(m_main_window);
    MBoardView *view = new MBoardView(window);
    setupGame(static_cast<QWidget *>(sender()), window, view,
              new MNetworkGame(view, m_log, window));
}

void MPreGame::
enableWidget(QWidget *widget)
{
    widget->setEnabled(true);
}
