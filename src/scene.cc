/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2010 Michael Hasselmann <michael@taschenorakel.de>
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

#include <scene.h>

#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>

using namespace Miniature;

MScene::
MScene(QObject *parent)
: QGraphicsScene(parent),
  m_modal_item(0)
{}

MScene::
MScene(const QRectF &region, QObject *parent)
: QGraphicsScene(region, parent),
  m_modal_item(0)
{}

MScene::
~MScene()
{}

void MScene::
setModalItem(QGraphicsItem *item)
{
    if(m_modal_item)
    {
        m_modal_item->hide();
    }

    m_modal_item = item;
}

void MScene::
mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // It is neccessary to call this first, else the event position is not updated.
    QGraphicsScene::mousePressEvent(event);

    QPointF target = event->pos();

    // If another item holds the mouse already, we know that the coords are in
    // that item's coords, hence we need to translate it to scene coords.
    if (mouseGrabberItem())
    {
        target = mouseGrabberItem()->mapToScene(event->pos());
    }

    if(m_modal_item && !(m_modal_item->boundingRect().contains(m_modal_item->mapFromScene(target))))
    {
        m_modal_item->hide();
        m_modal_item = 0;
    }
}
