/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2009 Michael Hasselmann <michael@taschenorakel.de>
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

#include "config.h"
#include "miniature.h"
#include "board_view.h"

#include <QDBusConnection>
#include <QMessageBox>

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
    connect(&m_game, SIGNAL(playerInfoChanged()),
            this, SLOT(updatePlayerInfo()));

    // Connect game logic w/ board view.
    connect(&m_game, SIGNAL(positionChanged(const MPosition&)),
            m_ui.board_view, SLOT(drawPosition(const MPosition&)));

    // Connect moves from board with main window.
    connect(m_ui.board_view, SIGNAL(pieceMoved(QPoint, QPoint)),
            this, SLOT(updateLastMove(QPoint, QPoint)));

    // Connect menu actions.
    connect(m_ui.new_game, SIGNAL(triggered()),
            &m_game, SLOT(newGame()));
    connect(m_ui.next_move, SIGNAL(triggered()),
            &m_game, SLOT(nextMove()));
    connect(m_ui.prev_move, SIGNAL(triggered()),
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

#ifdef Q_WS_MAEMO_5
//    setAttribute(Qt::WA_Maemo5ForcePortraitOrientation, true);
//    setAttribute(Qt::WA_Maemo5ForceLandscapeOrientation, false);
#endif

// taken 99% verbatim from http://taschenorakel.de/michael/2009/11/09/miniature-it-moves/#c592, thanks again gnuton!
#ifdef Q_WS_HILDON
    int value = 1;
    Atom portraitSupport = XInternAtom(QX11Info::display(), "_HILDON_PORTRAIT_MODE_SUPPORT", False);
    Atom portraitRequest = XInternAtom(QX11Info::display(), "_HILDON_PORTRAIT_MODE_REQUEST", False);
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

void MMainWindow::updateLastMove(QPoint from, QPoint to)
{
    m_ui.last_move->setText(tr("Moved from [%1, %2] to [%3, %4]").
                            arg(from.x()).arg(from.y()).
                            arg(to.x()).arg(to.y()));
}

void MApplication::open(const QUrl &url)
{
    QMessageBox(QMessageBox::Information,
                tr("Not implemented yet"),
                tr("Reading of PGN chess files is not implemented yet.\n"
                   "Cannot read information at %1.").arg(url.toString())).
                exec();
}

MApplicationAdaptor::MApplicationAdaptor(MApplication *application)
: QDBusAbstractAdaptor(application), m_application(application)
{
}

void MApplicationAdaptor::top_application()
{
    m_application->m_window.raise();
    m_application->m_window.showNormal();
}

void MApplicationAdaptor::mime_open(const QString &url)
{
    m_application->open(url);
}

MApplication::MApplication(int &argc, char **argv)
: QApplication(argc, argv)
{
    connect(this, SIGNAL(lastWindowClosed()), this, SLOT(quit()));
    m_window.show();
}

int main(int argc, char **argv)
{
    MApplication app(argc, argv);
    new MApplicationAdaptor(&app);

    QDBusConnection session(QDBusConnection::sessionBus());

    if (!session.registerService("org.maemo.miniature") ||
        !session.registerObject("/org/maemo/miniature", &app)) {
        qWarning("Cannot register D-Bus service. Aborting.");
        return 1;
    }

    if (argc > 1)
        app.open(QUrl(argv[1]));

    return MApplication(argc, argv).exec();
}
