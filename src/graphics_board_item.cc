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
  m_board_size(board_size),
  m_active_item(0),
  m_selection(0),
  m_cancel(0)
{
    setFiltersChildEvents(true);
    const int cell_size = getCellSize();

    m_selection = new QGraphicsRectItem(QRectF(0, 0, cell_size, cell_size), this);
    m_selection->setFlag(QGraphicsItem::ItemStacksBehindParent);
    m_selection->setBrush(QBrush(QColor::fromRgbF(0, .5, 0, .5)));
    m_selection->hide();

    m_cancel = new QGraphicsRectItem(QRectF(0, 0, cell_size, cell_size), this);
    m_cancel->setBrush(QBrush(QColor::fromRgbF(0.7, 0, 0.4, 0.5)));
    m_cancel->hide();
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

    QGraphicsItem *clicked_item = (scene()->items(mapToScene(event->pos().x() - 1, event->pos().y() -1, 2, 2)))[0];

    if (clicked_item == m_cancel)
    {
        resetSelection();
        m_cancel->hide();
        Q_EMIT undoMoveRequest();
    }
    else if(!m_active_item) // try to find an active item
    {
        m_active_item = clicked_item;

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

            Q_EMIT pieceSelectionRequest(QPoint(src_x, src_y));
            // TODO: make it possible to set the background of a piece from the
            // outside, perhaps even subclass QGraphicsSvgItem for the pieces.
            m_cancel->hide();
            m_selection->setPos(m_active_item->pos());
            m_selection->show();
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
            m_cancel->setPos(m_active_item->pos());
            m_cancel->show();

            Q_EMIT pieceMoveRequest(QPoint(src_x, src_y), QPoint(dst_x, dst_y));
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
    m_selection->hide();
}
