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
// Disabled. This code still crashes randomly ...

/*
    if(!scene())
    {
        return; // pieces can not be visible yet!
    }

    for (QList<QGraphicsItem *>::iterator iter = childItems().begin();
         iter != childItems().end();
         ++iter)
    {
        // prevent it from crashing
        if(!(*iter)->isVisible())
        {
            (*iter)->hide();
        }
    }
*/
}
