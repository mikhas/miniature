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

#ifndef GRAPHICS_BOARD_ITEM_H__
#define GRAPHICS_BOARD_ITEM_H__

#include "position.h"

#include <QGraphicsItem>
#include <QGraphicsWidget>
#include <QGraphicsSvgItem>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QUrl>
#include <QVector>
#include <QPainter>

namespace Miniature
{

/* This class represents the board. It's our primary item in the scene graph,
 * as it not only renders the board but also controls the mouse event handling
 * (= fingertouch handling on Maemo) for all pieces.
 * We use the QGraphicsObject so that we can use signals and slots.
 */
class MGraphicsBoardItem
: public QGraphicsWidget
{
    Q_OBJECT

public:
    MGraphicsBoardItem(int size = 480, QGraphicsItem *parent = 0, Qt::WindowFlags flags = Qt::Widget);
    ~MGraphicsBoardItem();

    // TODO: rename to cellSize(), more qt-ish
    int getCellSize() const;

    // This object doesnt paint anything itself
    void virtual paint(QPainter*, const QStyleOptionGraphicsItem *, QWidget *) {}
    virtual QRectF boundingRect() const;

    void hidePieces();

    // Takes ownership and connects propagated signals.
    void addPiece(MPiece *const piece);

Q_SIGNALS:
    void pieceClicked(MPiece *piece);
    void targetClicked(const QPoint &cell);
    void rotatePieces180();
    void rotatePieces0();
    void togglePieceRotations();

public Q_SLOT:
    void onMouseButtonPressed(QGraphicsSceneMouseEvent *event);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    const int m_board_size;
};

};

#endif
