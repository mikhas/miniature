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

//#include "miniature.ui.h"
#include "game.h"
#include "player_info.h"
#include "board_view.h"
#include "miniature_ui.h"

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

public Q_SLOTS:
    void updatePlayerInfo();
    void updateLastMove(int half_move, const QString &last_move);
    void appendDebugOutput(QString msg);
    void toggleDebugOutput();
    void showBoard();
    void showMoveList();
    void clearMoveList();

private:
    void setupMoveListView();
    /* Every but the board is created with the Qt designer, to save time when
     * porting to other platforms where we might have to make some changes to the
     * layout. We currently manually disable the auto-connect feature, in the
     * UI file itself.
     */
    Ui::MMainWindow m_ui;

    /* Our internal game controller */
    Miniature::MGame m_game;

    enum StackedWidgets {STACKED_BOARD, STACKED_MOVE_LIST};
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
