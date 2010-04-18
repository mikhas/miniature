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

MGraphicsBoardItem::MGraphicsBoardItem(int size, QGraphicsItem *parent)
: QGraphicsObject(parent),
  m_active(true),
  m_board_size(size)
{}

MGraphicsBoardItem::~MGraphicsBoardItem()
{}

int MGraphicsBoardItem::getCellSize() const
{
    return floor(boundingRect().height() / 8);
}

void MGraphicsBoardItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();

    if (m_active)
        Q_EMIT targetClicked(QPoint(floor(event->pos().x() / getCellSize()),
                                    floor(event->pos().y() / getCellSize())));
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

void MGraphicsBoardItem::disable()
{
    m_active = false;
}

void MGraphicsBoardItem::enable()
{
    m_active = true;
}

void MGraphicsBoardItem::addPiece(MPiece *const piece)
{
    Q_CHECK_PTR(piece);

    // Qt doesn't even get event propagation right =) But I can help out here!
    connect(piece, SIGNAL(pieceClicked(MPiece *)),
            this,  SLOT(onPieceClicked(MPiece *)));

    // Enable rotation for this piece.
    connect(this, SIGNAL(pieceRotationRequested180()),
            piece, SLOT(rotate180()));

    connect(this, SIGNAL(pieceRotationRequested0()),
            piece, SLOT(rotate0()));

    connect(this, SIGNAL(togglePieceRotations()),
            piece, SLOT(toggleRotations()));

    piece->setParentItem(this);
    piece->show();
}

void MGraphicsBoardItem::onPieceClicked(MPiece *piece)
{
    if (m_active)
        Q_EMIT pieceClicked(piece);
}

void MGraphicsBoardItem::rotatePieces0()
{
    if (m_active)
        Q_EMIT pieceRotationRequested0();
}

void MGraphicsBoardItem::rotatePieces180()
{
    if (m_active)
        Q_EMIT pieceRotationRequested180();
}

void MGraphicsBoardItem::updateFromPosition(MPosition *const position)
{
    for (MPosition::MPieces::iterator iter = position->begin();
         iter != position->end();
         ++iter)
    {
        if (*iter)
            addPiece(*iter);
    }

    // We could handle this ourselves in the above loop, but this is really not performance-
    // critical, nor does iterating twice add much overhead.
    position->updatePieces();
}
