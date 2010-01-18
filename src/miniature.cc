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

MMainWindow::~MMainWindow()
{}

void MMainWindow::setup()
{
    m_view = new Miniature::MBoardView(this);
    m_game = new Miniature::MGame(m_view, this);
}

void MMainWindow::appendDebugOutput(const QString &msg)
{
    Q_UNUSED(msg);
    // no impl
}

void MMainWindow::toggleDebugOutput()
{
    // no impl
}

QDBusAppActivator::QDBusAppActivator(MMainWindow *window)
: QDBusAbstractAdaptor(window),
  m_window(window)
{}

void QDBusAppActivator::top_application()
{
    m_window->activateWindow();
}

int main(int argc, char ** argv)
{
    // raster backend is faster than native, but not as disruptive as opengl
    QApplication::setGraphicsSystem(QString("raster"));

    QApplication app(argc, argv);
    MMainWindow window;
    window.show();

    return app.exec();
}
