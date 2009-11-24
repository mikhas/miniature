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

#include "miniature.ui.h"
#include "game.h"
#include "player_info.h"

#include <QDBusAbstractAdaptor>
#include <QMainWindow>
#include <QPoint>
#include <QUrl>

namespace Miniature
{

class MMainWindow
: public QMainWindow
{
    Q_OBJECT

public:
    MMainWindow();
    virtual ~MMainWindow();

public Q_SLOTS:
    void updatePlayerInfo();
    void updateLastMove(QPoint from, QPoint to);

private:
    /* Our internal game controller */
    Miniature::MGame m_game;

    /* A reference to the internal scene view */
    //QGraphicsView* m_view;

    Ui::MMainWindow m_ui;
};

class MApplication
: public QApplication
{
    friend class MApplicationAdaptor;

public:
    MApplication(int &argc, char **argv);
    virtual ~MApplication() {}

    void open(const QUrl &url);

private:
    MMainWindow m_window;
};

class MApplicationAdaptor
: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.maemo.miniature")

public:
    MApplicationAdaptor(MApplication *application);
    virtual ~MApplicationAdaptor() {}

public Q_SLOTS:
    void top_application();
    void mime_open(const QString &url);

private:
    MApplication *m_application;
};

}; // namespace Miniature

#endif // MINIATURE_H__
