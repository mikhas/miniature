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

#ifndef MINIATURE_H__
#define MINIATURE_H__

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>

class MiniatureWindow
: public QMainWindow
{
    Q_OBJECT

public:
    MiniatureWindow();
    virtual ~MiniatureWindow();
    virtual void show();

private:
    // Formats player info, as seen on the mock-up. I am not sure the last line means turn or sth else.
    QString formatPlayerInfo(QString name, int rating, int turn, QString alignment) const;
    QString formatTimerInfo(QString time_remaining, bool isWhite) const;

    /* Creates a scene graph containing all UI elements of the main view
     * (player cards, timer, board).
     */
    QGraphicsScene* createScene() const;

    QGraphicsView scene_view;
};

#endif // MINIATURE_H__
