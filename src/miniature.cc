/* Copyright 2009 Michael Hasselmann <michael@taschenorakel.de>
 * Part of the Miniature project.
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

#include "config.h"
#include "miniature.h"
#include "board.h"

#include <QMenuBar>
#include <QMenu>
#include <QSizePolicy>
#include <QGraphicsTextItem>
#include <QPixmap>
#include <QApplication>

// TODO: remove when replaced with clean solution in ctor, once new packages
// are available.
#ifdef Q_WS_HILDON
//Includes for portrait mode support
#  include <X11/Xlib.h>
#  include <X11/Xatom.h>
#  include <QtGui/QX11Info>
#endif

MiniatureWindow::MiniatureWindow()
: QMainWindow(),
  m_view(0)
{
    setWindowTitle(tr("Miniature - The killer chess app for Maemo"));

    QAction *new_game = new QAction(tr("New &game"), this);
    // TODO: These shortcuts don't seem to work. Find out why.
    new_game->setShortcut(tr("Ctrl+n"));

    QAction *next_move = new QAction(tr("&Next move"), this);
    // TODO: These shortcuts don't seem to work. Find out why.
    next_move->setShortcut(QKeySequence::MoveToNextChar);

    QAction *prev_move = new QAction(tr("&Previous move"), this);
    // TODO: These shortcuts don't seem to work. Find out why.
    prev_move->setShortcut(QKeySequence::MoveToPreviousChar);

    QMenu *game_menu = menuBar()->addMenu(tr("&Game"));
    game_menu->addAction(new_game);
    game_menu->addAction(next_move);
    game_menu->addAction(prev_move);

    // setup the view and hand a pointer to it to the game controller
    m_view = new QGraphicsView(this);
    QSizePolicy policy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    m_view->setSizePolicy(policy);
    m_game.setSceneView(m_view);

    QObject::connect(new_game, SIGNAL(triggered()), &m_game, SLOT(newGame()));
    QObject::connect(next_move, SIGNAL(triggered()), &m_game, SLOT(nextMove()));
    QObject::connect(prev_move, SIGNAL(triggered()), &m_game, SLOT(prevMove()));

// Setting portrait mode only works with git version of Qt4
//    setAttribute(Qt::WA_Maemo5ForcePortraitOrientation, true);
//    setAttribute(Qt::WA_Maemo5ForceLandscapeOrientation, false);

// TODO: replace with clean solution above, once new packages are available.
// taken 99% verbatim from http://taschenorakel.de/michael/2009/11/09/miniature-it-moves/#c592, thanks again gnuton!
#ifdef Q_WS_HILDON
    int value = 1;
    Atom portraitSupport = XInternAtom(QX11Info::display(), "_HILDON_PORTRAIT_MODE_SUPPORT", False);
    Atom portraitRequest = XInternAtom(QX11Info::display(), "_HILDON_PORTRAIT_MODE_REQUEST", False);
//printf ("ps:%d pr:%d, wi=%d\n", (int)portraitSupport, (int)portraitRequest, (int)winId());
    XChangeProperty(QX11Info::display(), winId(), portraitSupport, XA_CARDINAL, 32, PropModeReplace, (uchar *)&value, 1);
    XChangeProperty(QX11Info::display(), winId(), portraitRequest, XA_CARDINAL, 32, PropModeReplace, (uchar *)&value, 1);
#endif
}

MiniatureWindow::~MiniatureWindow()
{}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    MiniatureWindow main;
    main.show();

    return app.exec();
}
