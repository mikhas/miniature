/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2009 Dennis Stötzel <kore@meeQ.de>
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

#include "piece.h"

#include <QGraphicsScene>
#include <QBrush>
#include <QBitmap>
#include <QList>
#include <QTime>
#include <QGraphicsRotation>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneMouseEvent>

namespace Miniature
{

MPiece::
MPiece(MColour colour, MType pieceType, int xDimension, int yDimension, int width, int height)
: QGraphicsObject(),
  colour(colour),
  type(pieceType),
  xDim(xDimension),
  yDim(yDimension),
  selection(new QGraphicsRectItem(QRect(0, 0, width, height), this)), // TODO: get rid of magic numbers
  image(width, height, QImage::Format_ARGB32_Premultiplied),
  ghost(new QGraphicsPixmapItem(this)),
  dropShadow(new QGraphicsPixmapItem(this)),
  ghostFadeOutTimer(new QTimeLine(250, this)),
  rotated(false),
  enableRotations(true)
{
    // Initialize QImage so that we dont get artifacts from previous images.
    image.fill(Qt::transparent);

    initializeSelection();
    initializeEffects();
}

MPiece::
~MPiece()
{}

void MPiece::
paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter->drawImage(QRectF(0, 0, 60, 60), image);
}

QRectF MPiece::
boundingRect() const
{
    return QRectF(0, 0, 60, 60);
}

void MPiece::
initializeSelection()
{
    selection->setFlag(QGraphicsItem::ItemStacksBehindParent);
    selection->setBrush(QBrush(QColor::fromRgbF(0, .5, 0, .5)));
    selection->setEnabled(false);
    selection->hide();
}

void MPiece::
initializeEffects()
{
    ghost->setFlag(QGraphicsItem::ItemStacksBehindParent);
    ghost->setEnabled(false);
    ghost->hide();

    dropShadow->setFlag(QGraphicsItem::ItemStacksBehindParent);
    dropShadow->setEnabled(false);
    dropShadow->hide();

    ghostFadeOutTimer->setDirection(QTimeLine::Backward);
    connect(ghostFadeOutTimer, SIGNAL(valueChanged(qreal)),
            this, SLOT(fadeOutGhost(qreal)));
}

MPiece::MColour MPiece::
getColour() const
{
    return colour;
}

MPiece::MType MPiece::
getType() const
{
    return type;
}

void MPiece::
select()
{
    selection->show();
}

void MPiece::
deSelect()
{
    selection->hide();
}

bool MPiece::
isSelected() const
{
    return selection->isVisible();
}

void MPiece::
showGhostAt(const QPoint &target)
{
    // transfers ownership, which probably makes this reparenting ugly
    ghost->setParentItem(parentItem());

    ghost->setPos(mapFromCell(target));
    ghost->setRotation(rotated ? 180 : 0);

    ghost->setOpacity(0.4);
    ghost->show();
}

void MPiece::
hideGhost()
{
    ghostFadeOutTimer->start();
}

void MPiece::
fadeOutGhost(qreal timer_value)
{
    const qreal new_value = timer_value * 0.4;
    if (0.01 >= new_value && ghost->isVisible())
    {
        ghost->setOpacity(0);
        ghost->hide();
    }
    else
    {
        ghost->setOpacity(new_value);
    }
}

void MPiece::updatePieceInView(MPiece *piece, const QPoint &target)
{
    Q_CHECK_PTR(piece);

    piece->moveTo(piece->mapFromCell(target));
}

void MPiece::
moveTo(const QPoint &target)
{
    QPointF origin = pos();
    QGraphicsObject::setPos(target);
}

QPoint MPiece::
mapToCell() const
{
    return QPoint(pos().x() / 60, pos().y() / 60); // TODO: remove magic numbers!
}

QPoint MPiece::
mapFromCell(const QPoint &cell) const
{
    return QPoint(cell.x() * 60, cell.y() * 60); // TODO: remove magic numbers!
}

void MPiece::
applyRenderer(QSvgRenderer &renderer, int pieceSize)
{
    QPainter painter(&image);
    renderer.render(&painter);

    QPixmap ghost_pixmap = QPixmap(image.size());
    ghost_pixmap.fill(Qt::transparent);
    QPainter ghost_painter(&ghost_pixmap);
    renderer.render(&ghost_painter);

    ghost->setPixmap(ghost_pixmap);
    ghost->setOpacity(0);
    const QRectF extent = ghost->boundingRect();
    ghost->setTransformOriginPoint(extent.width() * .5, extent.height() * .5);

    QBitmap shadowMask = ghost_pixmap.mask();
    QPixmap shadowPixmap = QPixmap(ghost_pixmap.size());
    shadowPixmap.fill(Qt::transparent);

    QPainter shadowPainter(&shadowPixmap);
    shadowPainter.setPen(QColor(60, 60, 60));
    shadowPainter.drawPixmap(0, 0, shadowMask);

    dropShadow->setPixmap(shadowPixmap);
    dropShadow->setTransformOriginPoint(extent.width() * .5, extent.height() * .5);

    Q_UNUSED(pieceSize);
}

void MPiece::
rotate0()
{
    rotate(false);
}

void MPiece::
rotate180()
{
    rotate(true);
}

void MPiece::
rotate(bool flip)
{
    if (rotated == flip || !enableRotations)
    {
        return; // dont rotate then
    }

    rotated = flip;

    const int center = boundingRect().width() * 0.5;
    setTransformOriginPoint(center, center);

    setRotation(flip ? 180 : 0);
}

void MPiece::
toggleRotations()
{
    enableRotations = !enableRotations;
}

void MPiece::
mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();
    Q_EMIT pieceClicked(this);
}

} // namespace Miniature
