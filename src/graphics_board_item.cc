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

#include "graphics_board_item.h"
#include <QPainterPath>
#include <QGraphicsScene>
#include <cmath>

using namespace Miniature;

MGraphicsBoardItem::MGraphicsBoardItem(QGraphicsItem *parent)
: QGraphicsSvgItem(parent)
{}

MGraphicsBoardItem::MGraphicsBoardItem(const QString &fileName, QGraphicsItem *parent)
: QGraphicsSvgItem(fileName, parent)
{}

MGraphicsBoardItem::~MGraphicsBoardItem()
{}

void MGraphicsBoardItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QList<QGraphicsItem*> selection = scene()->selectedItems();
    if(1 != selection.size())
    {
        event = event;
        QPainterPath area;
        QPointF pos = event->pos();
        area.addRect(pos.x() - 1, pos.y() - 1, 2, 2);
        scene()->setSelectionArea(area);
    }
}

void MGraphicsBoardItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    const int cell_size = floor(boundingRect().height() / 8);

    QList<QGraphicsItem*> selection = scene()->selectedItems();
    if(1 == selection.size())
    {
        int src_x = floor((selection[0])->pos().x() / cell_size);
        int src_y = floor((selection[0])->pos().y() / cell_size);

        int dst_x = floor(event->pos().x() / cell_size);
        int dst_y = floor(event->pos().y() / cell_size);

        //std::cout << "cell_size: " << cell_size << ", src: (" << src_x << ", " << src_y << "), dst: (" << dst_x << ", " << dst_y << ")" << std::endl;

        (selection[0])->setPos(QPointF(dst_x * cell_size, dst_y * cell_size));
        scene()->clearSelection();

        Q_EMIT pieceMoved(QPoint(src_x, src_y), QPoint(dst_x, dst_y));
    }
}
