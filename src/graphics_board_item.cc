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
#include <QTime>

#include <cmath>

using namespace Miniature;

MGraphicsBoardItem::MGraphicsBoardItem(int board_size, QGraphicsObject *parent)
: QGraphicsObject(parent),
  m_board_size(board_size),
  m_active_item(0)
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
    static QTime profiling;
    profiling.restart();

    if(!m_active_item) // try to find an active item
    {
        m_active_item = (scene()->items(mapToScene(event->pos().x() - 1, event->pos().y() -1, 2, 2)))[0];

        // Do not select board or piece from the wrong colour.
        if (m_active_item == this || !m_active_item->data(0).toBool())
        {
            resetSelection();
           //std::cout << "mouse_pressed: selected board!" << std::endl;
        }
        else // valid selection!
        {
            const int cell_size = getCellSize();
            int src_x = floor(m_active_item->pos().x() / cell_size);
            int src_y = floor(m_active_item->pos().y() / cell_size);

            Q_EMIT pieceSelectionRequested(QPoint(src_x, src_y));
        }
        //std::cout << "mouse_pressed: try to select" << std::endl;
    }
    else
    {
        const int cell_size = getCellSize();
        //std::cout << "mouse_pressed: sth was already selected" << std::endl;

        int src_x = floor(m_active_item->pos().x() / cell_size);
        int src_y = floor(m_active_item->pos().y() / cell_size);

        int dst_x = floor(event->pos().x() / cell_size);
        int dst_y = floor(event->pos().y() / cell_size);

        if((src_x != dst_x) || (src_y != dst_y))
        {
            //active_item->setPos(QPointF(dst_x * cell_size, dst_y * cell_size));
            Q_EMIT pieceMoveRequested(QPoint(src_x, src_y), QPoint(dst_x, dst_y));
        }

        resetSelection(); // TODO: nope, the board item should not deselect pieces ... but I cant see the correct logic.
    }

    Q_EMIT sendDebugInfo(QString("MGBI::mpe - update duration: %1 ms").arg(profiling.restart()));
}

QRectF MGraphicsBoardItem::boundingRect() const
{
    return QRectF(0, 0, m_board_size, m_board_size);
}

void MGraphicsBoardItem::addPiece(QGraphicsSvgItem *piece)
{
    m_pieces.append(piece);
    piece->setData(0, QVariant(true)); // allowed to move
    piece->setData(1, QVariant(false)); // not rotated
    piece->setParentItem(this);
}

void MGraphicsBoardItem::removePieces()
{
    for(MPiecesList::iterator iter = m_pieces.begin();
        iter != m_pieces.end();
        ++iter)
    {
        (*iter)->hide();
    }
}

void MGraphicsBoardItem::resetSelection()
{
    m_active_item = 0;
}
