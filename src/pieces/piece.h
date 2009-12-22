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

#ifndef PIECE_H__
#define PIECE_H__

#include <QList>
#include <QPoint>
#include <QChar>
#include <QGraphicsObject>
#include <QSvgRenderer>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QPropertyAnimation>
#include <QImage>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QTimeLine>

namespace Miniature
{

/* QGV items are quite inconistent: Some are QObjects (SvgItem, QGO, ...), others aren't. */
/*
class MGraphicsPixmapObject
: public QObject,
  public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit MGraphicsPixmapObject(QGraphicsItem *item, QObject *parent = 0)
    : QObject(parent),
      QGraphicsPixmapItem(item)
    {}

    ~MGraphicsPixmapObject()
    {}
};
*/

/* Abstract base class for all pieces. Each piece animation is a member of
 * piece (at some point, there should be a piece animation class), and
 * animations are initialized during piece construction. This serves for
 * caching purposes. Could be made static member vars even.
 */
class MPiece
: public QGraphicsObject
{
    Q_OBJECT

public:
    enum MType {ROOK, KNIGHT, BISHOP, QUEEN, KING, PAWN, NONE};
    enum MColour {BLACK, WHITE};

    MPiece(MColour colour, MType pieceType, int xDimension, int yDimension, int width = 60, int height = 60);
    virtual ~MPiece();

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    virtual QRectF boundingRect() const;

    virtual QList<QPoint> getPossibleSquares(QPoint) const = 0;
    virtual QChar getLetter() const = 0;

    MType getType() const;
    MColour getColour() const;

    bool isSelected() const;

    // This method was briefly necessary for a hack because setPos(.) is
    // non-virtual. Now it stays here because its name makes the intend
    // clearer. And perhaps I need a setPos(.) wrapper again at one point
    // (animations come to mind).
    void moveTo(const QPoint &target);

    // mapTo/mapFrom follows the QGraphicsItem API naming conventions, although
    // our mapping is from a QGI coord system to a MPosition coord system.
    QPoint mapToCell() const;
    QPoint mapFromCell(const QPoint &cell) const;

public Q_SLOTS:
    void select();
    void deSelect();

    void showGhostAt(const QPoint &target);
    void hideGhost();

    void fadeOutGhost(qreal timer_value);

    void rotate0();
    void rotate180();
    void rotate(bool flip);
    void onRotationFinished();

    void toggleRotations();

protected:
    void applyRenderer(QSvgRenderer &renderer, int pieceSize);

    MColour colour;
    MType type;
    int xDim;
    int yDim;

    // Disable setPos(.) API - use moveTo(.) instead!
    void setPos(const QPointF &pos);
    void setPos(qreal x, qreal y);

    // The selection that is drawn around a piece when selected.
    QGraphicsRectItem *selection;

    QImage image;
    QGraphicsPixmapItem *ghost;
    QGraphicsPixmapItem *dropShadow;

    // Flipping a piece, animated.
    bool rotated;
    QPropertyAnimation *rotationAnimForward;
    QPropertyAnimation *rotationAnimForwardCcw;
    QPropertyAnimation *rotationAnimBackward;
    QPropertyAnimation *rotationAnimBackwardCcw;

    // Fade out the ghost piece ... somehow, QPropertyAnimations did not work
    QTimeLine *ghostFadeOutTimer;

    bool enableRotations;
};

} // namespace Miniature

#endif /* PIECE_H__ */
