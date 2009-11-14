/* Copyright 2009 Michael Hasselmann <michael@taschenorakel.de>
 * Part of the Miniature project.
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

#ifndef BOARD_VIEW_H__
#define BOARD_VIEW_H__

#include "position.h"

#include <QString>
#include <QPixmap>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>


namespace Miniature
{
/* This class represents a board, using QGraphicsItems. Each figure is added as
 * a child to the board itself. Also, the aim is to not extend the state of its
 * parent class, QGraphicsPixmapItem (read: no private member vars).
 */
// TODO: Dont derive from QGraphicsPixmapItem, derive from a view widget.
class MBoardView
: public QGraphicsPixmapItem
{
// Be careful, QGraphicsItems *do no* inherit from QObject.

public:
    explicit MBoardView(QGraphicsItem *parent = 0);
    explicit MBoardView(const QPixmap &pixmap, QGraphicsItem *parent = 0);
    virtual ~MBoardView();

    /* Removes (= deletes) all pieces from the board. */
    void clear();

    /* Draws a chess position on this board, by extracting the FEN [1]
     * representation from the given MPosition.
     * Each figure (= QGraphicsSvgItem) that is added becomes a child of the board.
     * [1] http://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
     */
    void drawPosition(MPosition &position);
    void drawStartPosition();

private:
    QString getFileNameForPiece(QChar fenFigure) const;
};

}; // namespace Miniature
#endif
