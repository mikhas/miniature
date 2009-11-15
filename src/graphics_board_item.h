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

#ifndef GRAPHICS_BOARD_ITEM_H__
#define GRAPHICS_BOARD_ITEM_H__

#include <QGraphicsSvgItem>
#include <QGraphicsSceneMouseEvent>

namespace Miniature
{

class MGraphicsBoardItem
: public QGraphicsSvgItem
{
    Q_OBJECT

public:
    MGraphicsBoardItem(QGraphicsItem *parent = 0);
    MGraphicsBoardItem(const QString &fileName, QGraphicsItem *parent = 0);
    ~MGraphicsBoardItem();

    int getCellSize() const;

Q_SIGNALS:
    void pieceMoved(QPoint from, QPoint to);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
};

};

#endif
