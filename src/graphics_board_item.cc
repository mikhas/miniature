/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2009 Michael Hasselmann <michael@taschenorakel.de>
 * Copyright (C) 2009 Mathias Hasselmann <mathias.hasselmann@maemo.org>
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

#include "graphics_board_item.h"

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QTime>
#include <QGraphicsRotation>
#include <QPropertyAnimation>
#include <QVector3D>

#include <cmath>

using namespace Miniature;

MGraphicsBoardItem::MGraphicsBoardItem(int board_size, QGraphicsObject *parent)
: QGraphicsObject(parent),
  m_board_size(board_size)
{
    setFiltersChildEvents(true);
}

MGraphicsBoardItem::~MGraphicsBoardItem()
{}

int MGraphicsBoardItem::getCellSize() const
{
    return floor(boundingRect().height() / 8);
}

void MGraphicsBoardItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();

    QGraphicsItem *clicked_item = (scene()->items(mapToScene(event->pos().x() - 1, event->pos().y() -1, 2, 2)))[0];
    MPiece *piece = dynamic_cast<MPiece*>(clicked_item);
    if (piece)
    {
        Q_EMIT pieceClicked(piece);
    }
    else
    {
        Q_EMIT targetClicked(QPoint(floor(event->pos().x() / getCellSize()),
                                    floor(event->pos().y() / getCellSize())));
    }
}

QRectF MGraphicsBoardItem::boundingRect() const
{
    return QRectF(0, 0, m_board_size, m_board_size);
}

void MGraphicsBoardItem::hidePieces()
{
    if(!scene())
    {
        return; // pieces can not be visible yet!
    }

    // Save childItems() in a temporary variable *outside* the block context
    // since otherwise the returned list would be destroyed right after the
    // iterator assignment in the for header, and not just at the end of the
    // loop block!
    QList<QGraphicsItem *> children = childItems();

    for (QList<QGraphicsItem *>::iterator iter = children.begin();
         iter != children.end();
         ++iter)
    {
        (*iter)->hide();
    }
}

void MGraphicsBoardItem::flipOneEighty()
{
    QList<QGraphicsItem *> children = childItems();
    for (QList<QGraphicsItem *>::iterator iter = children.begin();
         iter != children.end();
         ++iter)
    {
        MPiece *piece = dynamic_cast<MPiece *>(*iter);
        if (piece)
        {
            piece->flipOneEighty();
        }
    }
}
