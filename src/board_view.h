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
#include <QGraphicsView>
#include <QGraphicsScene>


namespace Miniature
{
/* This class represents a board, using QGraphicsItems. Each figure is added as
 * a child to the board itself.
 * The state of this class should not be extended if possible.
 */
class MBoardView
: public QGraphicsView
{
    Q_OBJECT

public:
    explicit MBoardView(QWidget *parent = 0);
    explicit MBoardView(QGraphicsScene *scene, QWidget *parent = 0);
    virtual ~MBoardView();

    virtual void setScene(QGraphicsScene* scene);

    /* Removes (= deletes) all pieces from the board. */
    void clear();

public Q_SLOTS:
    /* Draws a chess position on this board, by extracting the FEN [1]
     * representation from the given MPosition.
     * Each figure (= QGraphicsSvgItem) that is added becomes a child of the board.
     * [1] http://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
     */
    void drawPosition(const MPosition &position);
    void drawStartPosition();

private:
    void setBoardBackground();
    QString getFileNameForPiece(QChar fenFigure) const;

    /* Store a reference to the board item in the scene graph. */
    QGraphicsItem* m_board_item;
};

}; // namespace Miniature
#endif
