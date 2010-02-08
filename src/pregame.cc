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
#include <game.h>

#include <QtCore>
#include <QtGui>

using namespace Miniature;

MPreGame::
MPreGame(QObject *parent)
: QObject(parent),
  m_main_window(new MMainWindow)
{
    m_main_window->registerActions();
    m_main_window->connectActions();
}

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
}

void MPreGame::
onNewGameRequested()
{
    // Prevent further signal emission.
    QWidget *source = static_cast<QWidget *>(sender());
    source->hide();

    QMainWindow *window = new QMainWindow(m_main_window);

    MBoardView *view = new MBoardView(window);
    MGame *game = new Miniature::MGame(view, window);
    game->newGame();

    MMainWindow::setupGameUi(window, view);
    window->show();

    // Allow signal emission only after this window is gone.
    connect(window, SIGNAL(destroyed()), source, SLOT(show()));
}

void MPreGame::
onNewP2PGameRequested()
{
    // Prevent further signal emission.
    QWidget *source = static_cast<QWidget *>(sender());
    source->hide();

    QMainWindow *window = new QMainWindow(m_main_window);

    //MBoardView *view = new MBoardView(window);

    QMessageBox::critical(window, tr("Not implemented yet!"), tr("Sorry, but this feature is currently missing ..."));

    //TODO: Activate me!
    //MGame *game = new Miniature::MGame(view, window);
    //game->newP2PGame();

    //MMainWindow::setupGameUi(window, view);
    //window->show();

    // Allow signal emission only after this window is gone.
    //connect(window, SIGNAL(destroyed()), source, SLOT(show()));
}