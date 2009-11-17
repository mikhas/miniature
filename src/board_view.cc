/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
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

#include "board_view.h"
#include "pieces_pool_manager.h"

#include <QGraphicsSvgItem>
#include <QPixmap>

using namespace Miniature;

MBoardView::MBoardView(QWidget *parent)
: QGraphicsView(parent),
  m_board_item(0)
{
  QGraphicsView::setScene(new QGraphicsScene(this));
  setBoardBackground();
}

MBoardView::MBoardView(QGraphicsScene *scene, QWidget *parent)
: QGraphicsView(scene, parent),
  m_board_item(0)
{
  setBoardBackground();
}

MBoardView::~MBoardView()
{}

void MBoardView::setScene(QGraphicsScene *scene)
{
    QGraphicsView::setScene(scene);
    setBoardBackground();

}

void MBoardView::setBoardBackground()
{
    m_board_item = new MGraphicsBoardItem(":boards/glossy.svg");
    connect(m_board_item, SIGNAL(pieceMoved(QPoint, QPoint)),
            this, SLOT(onPieceMoved(QPoint, QPoint)));

    scene()->addItem(m_board_item);
}

void MBoardView::clear()
{
    /* Releasing the resources from the pieces pool, so that we can start to
     * consume them again.
     */
    m_pieces_pool_manager.releaseAll();

    /* We clear the board by simply hiding all pieces. We don't know whether we
     * get the same pieces back from the pool manager, so having a hide/show
     * transaction is a safe route. */
    QList<QGraphicsItem*> children = m_board_item->childItems();
    for(QList<QGraphicsItem*>::iterator iter = children.begin();
        iter != children.end();
        ++iter)
    {
        Q_CHECK_PTR(*iter); // This might be paranoid ...
        (*iter)->hide();
    }
}

void MBoardView::drawPosition(const MPosition &position)
{
    Q_CHECK_PTR(m_board_item);

    QString fen = position.convertToFen();

    if (fen.isEmpty())
    {
        // We have to decide whether we want to treat this as error. Currently,
        // it's not (calling this method with an empty string).
        return;
    }

    clear();

    /* (x_pos, y_pos) always tells us where to draw the current piece (top
     * left corner of a cell).
     */
    int x_pos = 0;
    int y_pos = 0;

    /* We count cells so we know when to stop reading the FEN string. A FEN
     * string can contain comments and game state information which we do not need.
     * That is why we stop after we have seen 64 cells.
     */
    int count_cells = 0;

    for(int idx = 0; idx < fen.length() && 64 > count_cells; ++idx)
    {
        QChar curr = fen.at(idx);

        // scanned one row
        if ('/' == curr)
        {
            y_pos += m_board_item->getCellSize();
            x_pos = 0;
        }
        else if (curr.isDigit())
        {
            /* This is the nice part about FEN: series of empty squares use a
             * run-length-encoding. And that's stuff from the 19th century!
             */
            x_pos += curr.digitValue() * m_board_item->getCellSize();
            count_cells += curr.digitValue();
        }
        else
        {
            // fetch the SVG file and add it to the board
            MGraphicsChessPieceItem *piece = m_pieces_pool_manager.take(position.lookupPieceType(curr));
            if (piece)
            {
                piece->setPos(QPointF(x_pos, y_pos));
                piece->show();
                piece->setParentItem(m_board_item); // hm, only important when we first use a piece ...
                x_pos += m_board_item->getCellSize();
                ++count_cells;
            }
            else // Complain!
            {}
        }

    }
}

void MBoardView::drawStartPosition()
{
    MPosition pos;
    drawPosition(pos);
}

void MBoardView::onPieceMoved(QPoint from, QPoint to)
{
    // event propagation
    Q_EMIT pieceMoved(from, to);
}
