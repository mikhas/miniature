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

#include <QTreeWidgetItem>
#include <QStringList>

using namespace Miniature;

MMainWindow::MMainWindow()
: QMainWindow(),
  m_game(0)
{
    m_ui.setupUi(this);
    m_game = new MGame(m_ui.board_view, this);

    // Add debugging output to Miniature app.
    connect(m_game, SIGNAL(sendDebugInfo(QString)),
            this, SLOT(appendDebugOutput(QString)));

    // Connect actions.
    connect(m_ui.new_game, SIGNAL(triggered()),
            m_game, SLOT(newGame()));

    // Connect game navi actions
    connect(m_ui.jump_to_start, SIGNAL(triggered()),
            m_game, SLOT(jumpToStart()));
    connect(m_ui.prev_move, SIGNAL(triggered()),
            m_game, SLOT(prevMove()));
    connect(m_ui.next_move, SIGNAL(triggered()),
            m_game, SLOT(nextMove()));
    connect(m_ui.jump_to_end, SIGNAL(triggered()),
            m_game, SLOT(jumpToEnd()));

    connect(m_ui.toggle_piece_rotations, SIGNAL(triggered()),
            m_game, SLOT(onPieceRotationsToggled()));

    connect(m_ui.toggle_debug_output, SIGNAL(triggered()),
            this, SLOT(toggleDebugOutput()));

    // Connect action to show about dialog
    connect(m_ui.about, SIGNAL(triggered()),
            this, SLOT(showAboutDialog()));

    QFont small_font("helvetica", 8, QFont::Light);
    m_ui.debug->setFont(small_font);
    m_ui.debug->hide();

#ifdef Q_WS_MAEMO_5
    // TODO: query orientation before
    setAttribute(Qt::WA_Maemo5ForcePortraitOrientation, true);
    setAttribute(Qt::WA_Maemo5ForceLandscapeOrientation, false);
#endif

#ifndef Q_WS_MAEMO_5
    // TODO: Cant I just use this in general?
    resize(480, -1);
#endif
}

MMainWindow::~MMainWindow()
{}

void MMainWindow::show()
{
    QMainWindow::show();
}

void MMainWindow::appendDebugOutput(QString msg)
{
    m_ui.debug->append(msg);
}

void MMainWindow::toggleDebugOutput()
{
    static bool toggled = true;

    if (toggled)
    {
        m_ui.debug->show();
    }
    else
    {
        m_ui.debug->hide();
    }

    toggled = !toggled;
}

void MMainWindow::showAboutDialog()
{
    QFont font = QFont("helvetica", 14, QFont::Normal);
    QDialog dialog;
    m_about_dialog.setupUi(&dialog);
    m_about_dialog.slogan->setFont(font);
    m_about_dialog.version->setFont(font);
    m_about_dialog.version->setText(QString("%1: %2").arg(tr("Version"))
                                                     .arg(PACKAGE_VERSION));
    dialog.exec();
}

MDBusAdaptor::MDBusAdaptor(MMainWindow *window) 
: QDBusAbstractAdaptor(window),
  m_window(window)
{}

void MDBusAdaptor::top_application()
{
    qDebug() << __func__;
    m_window->activateWindow();
}

int main(int argc, char ** argv)
{
    // raster backend is faster than native, but not as disruptive as opengl
    QApplication::setGraphicsSystem(QString("raster"));

    QApplication app(argc, argv);
    MMainWindow window;

    QDBusConnection session(QDBusConnection::sessionBus());

    if (!session.registerService(SERVICE_NAME))
    {
        qWarning("Cannot register D-Bus service. Trying to activate other instance.");
        session.call(QDBusMessage::createMethodCall(SERVICE_NAME, SERVICE_PATH,
                                                    SERVICE_NAME, "top_application"));
        return 1;
    }

    if (!session.registerObject(SERVICE_PATH, &window))
    {
        qFatal("Cannot register D-Bus service. Aborting.");
        return 1;
    }

    new MDBusAdaptor(&window);
    window.show();

    return app.exec();
}
