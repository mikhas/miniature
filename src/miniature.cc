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

using namespace Miniature;

MMainWindow::MMainWindow()
: QMainWindow()
{

    m_ui.setupUi(this);
    updatePlayerInfo();

    // Connect game logic w/ player info view.
    connect(&m_game, SIGNAL(playerInfoChanged()),
            this, SLOT(updatePlayerInfo()));

    // Add debugging output to Miniature app.
    connect(&m_game, SIGNAL(sendDebugInfo(QString)),
            this, SLOT(appendDebugOutput(QString)));
    connect(m_ui.board_view, SIGNAL(sendDebugInfo(QString)),
            this, SLOT(appendDebugOutput(QString)));

    // Connect game logic w/ board view.
    connect(&m_game, SIGNAL(positionChanged(const MPosition&)),
            m_ui.board_view, SLOT(drawPosition(const MPosition&)));
    connect(m_ui.board_view, SIGNAL(pieceMoveRequested(QPoint, QPoint)),
            &m_game, SLOT(onPieceMoveRequested(QPoint, QPoint)));


    // Connect moves from game controller with main window.
    connect(&m_game, SIGNAL(pieceMoved(QPoint, QPoint)),
            this, SLOT(updateLastMove(QPoint, QPoint)));

    // Connect menu actions.
    connect(m_ui.new_game, SIGNAL(triggered()),
            &m_game, SLOT(newGame()));
    connect(m_ui.toggle_debug_output, SIGNAL(triggered()),
            this, SLOT(toggleDebugOutput()));

    // Fix the font sizes, the Maemo5 style is totally wrong regarding that.
    QFont small_font("helvetica", 8, QFont::Light);
    QFont big_font("helvetica", 16, QFont::Bold);
    QFont normal_font("helvetica", 14, QFont::DemiBold);
    m_ui.white_name->setFont(big_font);
    m_ui.white_rating->setFont(normal_font);
    m_ui.white_material->setFont(normal_font);

    m_ui.black_name->setFont(big_font);
    m_ui.black_rating->setFont(normal_font);
    m_ui.black_material->setFont(normal_font);
    m_ui.debug->setFont(small_font);
    m_ui.debug->hide();

#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ForcePortraitOrientation, true);
    setAttribute(Qt::WA_Maemo5ForceLandscapeOrientation, false);
#endif
}

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
    m_ui.last_move->setText(QString(tr("Moved from [%1, %2] to [%3, %4]")).arg(from.x())
                                                                          .arg(from.y())
                                                                          .arg(to.x())
                                                                          .arg(to.y()));
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


MDBusAdaptor::MDBusAdaptor(MMainWindow *window) 
: QDBusAbstractAdaptor(window),
  m_window(window)
{}

void MDBusAdaptor::top_application()
{
    qDebug() << __func__;
    m_window->activateWindow();
}

int main (int argc, char ** argv)
{
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
