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

#include <QtCore>
#include <QtGui>

using namespace Miniature;

MScene::
MScene(QObject *parent)
: QGraphicsScene(parent),
  m_modal_item(0),
  m_blur_item(new QGraphicsPixmapItem)
{
    setup();
}

MScene::
MScene(const QRectF &region, QObject *parent)
: QGraphicsScene(region, parent),
  m_modal_item(0),
  m_blur_item(new QGraphicsPixmapItem)
{
    setup();
}

MScene::
~MScene()
{}

void MScene::
setup()
{
    QGraphicsBlurEffect *effect = new QGraphicsBlurEffect;
    effect->setBlurRadius(12);
    m_blur_item->setGraphicsEffect(effect);
    addItem(m_blur_item);
    m_blur_item->hide();
}

void MScene::
setModalItem(QGraphicsItem *item)
{
    m_blur_item->hide();

    if(m_modal_item)
    {
        m_modal_item->hide();
        setFocusItem(0);
    }

    m_modal_item = item;
    setFocusItem(m_modal_item);

    QGraphicsView *view = views()[0];
    if (m_modal_item && view)
    {
        view->repaint();
        QRectF r = view->rect();
        m_blur_item->setPixmap(QPixmap::grabWindow(view->winId(), r.x(), r.y(), r.width(), r.height()));
        m_blur_item->show();
    }
}

void MScene::
resetModalItem()
{
    setModalItem(0);
}

void MScene::
keyPressEvent(QKeyEvent *event)
{
    // Assume that only those 3 keys are useful for a focus-in:
    if (!focusItem() && (event->key() & (Qt::Key_Return | Qt::Key_Enter | Qt::Key_Space)))
    {
        // Allow other components to intercept a key press event if there is no focused item:
        Q_EMIT focusInOnKeyPressed();
        return;
    }

    QGraphicsScene::keyPressEvent(event);
    event->accept();
}


void MScene::
mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF target = event->scenePos();

    // If another item holds the mouse already, we know that the coords are in
    // that item's coords, hence we need to translate it to scene coords.
    if (mouseGrabberItem())
    {
        target = mouseGrabberItem()->mapToScene(event->scenePos());
    }

    if(m_modal_item && !(m_modal_item->boundingRect().contains(m_modal_item->mapFromScene(target))))
    {
        event->accept();
        m_modal_item->hide();
        m_modal_item = 0;
        m_blur_item->hide();
    }
    else
    {
        // Needs to be the last method call since it could lead to signal emission,
        // therefore become racy w.r.t m_modal_item (might get replaced etc.)
        QGraphicsScene::mousePressEvent(event);
    }

}
