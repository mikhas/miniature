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

#include <QPixmap>
#include <QUrl>
#include <QPoint>

#include <iostream>

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
    m_board_item = new MGraphicsBoardItem();

    m_board_item->loadFromUri(QUrl("qrc:/boards/glossy.svg"));
    connect(m_board_item, SIGNAL(pieceMoveRequested(QPoint, QPoint)),
            this, SLOT(onPieceMoveRequested(QPoint, QPoint)));

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

    clear();

    for(MPosition::MPieces::const_iterator iter = position.begin();
        iter != position.end();
        ++iter)
    {
        if (*iter)
        {
            QPoint cell = position.indexToPoint(std::distance(position.begin(), iter), m_board_item->getCellSize());
            // TODO: put shared SVG renderer in MPiece and kill the pool manager.
            MGraphicsChessPieceItem *piece = m_pieces_pool_manager.take(MPosition::BROOK);
            if (piece)
            {
                piece->setPos(cell);
                piece->show();
                piece->setParentItem(m_board_item); // hm, only important when we first use a piece ...
            }
        }
    }
}

void MBoardView::drawStartPosition()
{
    MPosition pos;
    drawPosition(pos);
}

void MBoardView::onPieceMoveRequested(QPoint from, QPoint to)
{
    // event propagation
    std::cout << "MBoardView::onPieceMoveRequested emits pieceMoveRequested" << std::endl;
    Q_EMIT pieceMoveRequested(from, to);
}
