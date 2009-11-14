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
#include "board_view.h"

#include <QGraphicsTextItem>
#include <QPixmap>
#include <QApplication>

// TODO: remove when replaced with clean solution in ctor, once new packages
// are available. With Qt4.6 for Maemo there is now higher level API, try it out!
// Q_WS_HILDON is not valid for Qt4.6, so if you want to see portrait mode
// there (warning: the WM wont like it), define the Q_WS_HILDON macro manually.

#ifdef Q_WS_HILDON
//Includes for portrait mode support
#  include <X11/Xlib.h>
#  include <X11/Xatom.h>
#  include <QtGui/QX11Info>
#endif

using namespace Miniature;

MMainWindow::MMainWindow()
: QMainWindow()
{
    m_ui.setupUi(this);
    updatePlayerInfo();

    // Connect game logic w/ player info view.
    QObject::connect(&m_game, SIGNAL(playerInfoChanged()),
                     this, SLOT(updatePlayerInfo()));

    // Connect game logic w/ board view.
    QObject::connect(&m_game, SIGNAL(positionChanged(const MPosition&)),
                     m_ui.board_view, SLOT(drawPosition(const MPosition&)));

    // Connect menu actions.
    QObject::connect(m_ui.new_game, SIGNAL(triggered()),
                     &m_game, SLOT(newGame()));
    QObject::connect(m_ui.next_move, SIGNAL(triggered()),
                     &m_game, SLOT(nextMove()));
    QObject::connect(m_ui.prev_move, SIGNAL(triggered()),
                     &m_game, SLOT(prevMove()));

    // Fix the font sizes, the Maemo5 style is totally wrong regarding that.
    QFont big_font("helvetica", 16, QFont::Bold);
    QFont normal_font("helvetica", 14, QFont::DemiBold);
    m_ui.white_name->setFont(big_font);
    m_ui.white_rating->setFont(normal_font);
    m_ui.white_material->setFont(normal_font);

    m_ui.black_name->setFont(big_font);
    m_ui.black_rating->setFont(normal_font);
    m_ui.black_material->setFont(normal_font);

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

MMainWindow::~MMainWindow()
{}

void MMainWindow::updatePlayerInfo()
{
    MPlayerInfo info = m_game.getPlayerInfo();

    m_ui.white_name->setText(info.white_name);
    m_ui.white_rating->setText(QString("(%1)").arg(info.white_rating));
    m_ui.white_material->setText(QString("%1").arg(info.white_material));

    m_ui.black_name->setText(info.black_name);
    m_ui.black_rating->setText(QString("(%1)").arg(info.black_rating));
    m_ui.black_material->setText(QString("%1").arg(info.black_material));
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    MMainWindow main;
    main.show();

    return app.exec();
}
