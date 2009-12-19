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
#include <QList>
#include <QGraphicsRotation>
#include <QTime>
#include <QTimeLine>

namespace Miniature
{

MPiece::
MPiece(MColour colour, MType pieceType, int width, int height)
: QGraphicsSvgItem(),
  colour(colour),
  type(pieceType),
  xDim(width),
  yDim(height),
  selection(new QGraphicsRectItem(this)), // TODO: get rid of magic numbers
  rotated(false),
  rotationAnimForward(new QPropertyAnimation(this, "rotation")),
  rotationAnimForwardCcw(new QPropertyAnimation(this, "rotation")),
  rotationAnimBackward(new QPropertyAnimation(this, "rotation")),
  rotationAnimBackwardCcw(new QPropertyAnimation(this, "rotation"))
{
    selection->setFlag(QGraphicsItem::ItemStacksBehindParent);
    selection->setBrush(QBrush(QColor::fromRgbF(0, .5, 0, .5)));
    selection->setEnabled(false);
    selection->hide();

    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));

    const int duration = 750;
    QEasingCurve curve = QEasingCurve::OutQuart;

    rotationAnimForward->setDuration(duration);
    rotationAnimForward->setStartValue(0);
    rotationAnimForward->setEndValue(180);
    rotationAnimForward->setEasingCurve(curve);

    rotationAnimForwardCcw->setDuration(duration);
    rotationAnimForwardCcw->setStartValue(0);
    rotationAnimForwardCcw->setEndValue(-180);
    rotationAnimForwardCcw->setEasingCurve(curve);

    rotationAnimBackward->setDuration(duration);
    rotationAnimBackward->setStartValue(180);
    rotationAnimBackward->setEndValue(360);
    rotationAnimBackward->setEasingCurve(curve);

    rotationAnimBackwardCcw->setDuration(duration);
    rotationAnimBackwardCcw->setStartValue(180);
    rotationAnimBackwardCcw->setEndValue(0);
    rotationAnimBackwardCcw->setEasingCurve(curve);
}

MPiece::
~MPiece()
{}

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
moveTo(const QPoint &target)
{
    QGraphicsSvgItem::setPos(target);
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
    this->setSharedRenderer(&renderer);

    QRectF extent = this->boundingRect();
    qreal ratio = 1;
    if (0 < extent.width())
    {
        ratio = pieceSize / static_cast<qreal>(extent.width());
    }
    this->scale(ratio, ratio);
    this->selection->setRect(extent);
}

void MPiece::
flipOneEighty()
{
    static const int flip_range = RAND_MAX / 2;
    const bool flipped = qrand() / flip_range;

    const int center = boundingRect().width() * 0.5;
    setTransformOriginPoint(center, center);

    QPropertyAnimation *anim;
    anim = (rotated ? (flipped ? rotationAnimBackward
                               : rotationAnimBackwardCcw)
                    : (flipped ? rotationAnimForward
                               : rotationAnimForwardCcw));

    rotated = !rotated;

    anim->start();
}

} // namespace Miniature
