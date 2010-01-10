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

#ifndef MINIATURE_H__
#define MINIATURE_H__

#include "game.h"
#include "board_view.h"
#include "miniature.ui.h"
#include "about.ui.h"

#include <QtGui>
#include <QtDBus>

#define SERVICE_NAME "org.maemo.miniature"
#define SERVICE_PATH "/org/maemo/miniature"

class MMainWindow
: public QMainWindow
{
    Q_OBJECT

public:
    MMainWindow();
    ~MMainWindow();

    virtual void show();

public Q_SLOTS:
    void appendDebugOutput(QString msg);
    void toggleDebugOutput();
    void showAboutDialog();

private:
    Ui::MMainWindow m_ui;
    Ui::AboutDialog m_about_dialog;

    /* Our internal game controller */
    Miniature::MGame* m_game;
};

class MDBusAdaptor
: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.maemo.miniature")

public:
    MDBusAdaptor(MMainWindow *window);

public Q_SLOTS:
    void top_application();

private:
    MMainWindow *m_window;
};

#endif // MINIATURE_H__
