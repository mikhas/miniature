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

#include <QPixmap>
#include <QUrl>
#include <QPoint>
#include <QPainter>
#include <QImage>
#include <QGraphicsProxyWidget>

#include <iostream>
#include <QTime>

using namespace Miniature;

MBoardView::MBoardView(QWidget *parent)
: QGraphicsView(parent),
  m_board_item(0),
  m_background_page(new QWebPage),
  m_background_image(0),
  m_white_rotated180(false),
  m_black_rotated180(false),
  m_board_item_offset(70)
{
    QGraphicsView::setScene(new QGraphicsScene(this));

    connect(m_background_page, SIGNAL(loadFinished(bool)),
            this, SLOT(onLoadFinished(bool)));

    setupBoardBackground();
}

MBoardView::~MBoardView()
{
    resetCache();
    delete m_background_image;
}

void MBoardView::setScene(QGraphicsScene *scene)
{
    QGraphicsView::setScene(scene);
    setupBoardBackground();
}

void MBoardView::resetCache()
{
    m_cache.clear();
}

void MBoardView::drawBackground(QPainter *painter, const QRectF &region)
{
    if (m_background_page && m_background_image)
    {
        QPainter bg_painter(m_background_image);
        m_background_page->mainFrame()->render(&bg_painter);

        // Get rid of the QWebkit stuff, it seems to slow down mouse event
        // handling for us even when invisible!
        delete m_background_page;
        m_background_page = 0;
    }

    if (m_background_image)
    {
        painter->drawImage(region,
                           *m_background_image,
                           QRectF(region.x(),
                                  region.y() - m_board_item_offset,
                                  region.width(),
                                  region.height()));
    }
}

void MBoardView::setupBoardBackground()
{
    m_board_item = new MGraphicsBoardItem();

    QPalette palette = m_background_page->palette();
    palette.setColor(QPalette::Base, Qt::transparent);
    m_background_page->setPalette(palette);

    m_background_page->mainFrame()->load(QUrl("qrc:/boards/glossy.svg"));

    // propagate state transition requests
    connect(m_board_item, SIGNAL(pieceSelectionRequested(QPoint)),
            this, SLOT(onPieceSelectionRequested(QPoint)));
    connect(m_board_item, SIGNAL(pieceMoveRequested(QPoint, QPoint)),
            this, SLOT(onPieceMoveRequested(QPoint, QPoint)));

    scene()->addItem(m_board_item);
    m_board_item->setPos(QPoint(0, m_board_item_offset));

    connect(m_board_item, SIGNAL(sendDebugInfo(QString)),
            this, SLOT(appendDebugOutput(QString)));
}

void MBoardView::setTopActionArea(QGraphicsProxyWidget *proxy_widget)
{
    Q_CHECK_PTR(proxy_widget);

    scene()->addItem(proxy_widget);
    proxy_widget->setPos(QPoint(0, 0));
}

void MBoardView::setBottomActionArea(QGraphicsProxyWidget *proxy_widget)
{
    Q_CHECK_PTR(proxy_widget);

    scene()->addItem(proxy_widget);
    proxy_widget->setPos(QPoint(0, 550));
}

void MBoardView::resetPieceSelection()
{
    Q_CHECK_PTR(m_board_item);
    m_board_item->resetSelection();
}

void MBoardView::drawPosition(const MPosition &position)
{
    static QTime profiling;
    profiling.restart();

    Q_CHECK_PTR(m_board_item);

    m_board_item->removePieces();
    const int cell_size = m_board_item->getCellSize();

    for(MPosition::MPieces::const_iterator iter = position.begin();
        iter != position.end();
        ++iter)
    {
        MSharedPiece pos_piece = *iter;
        if (pos_piece) // non-empty cell
        {
            QPoint cell = position.indexToPoint(std::distance(position.begin(), iter), cell_size);
            QGraphicsSvgItem* item = 0;

            // Querying cache for the item.
            if (!m_cache.contains(pos_piece.data()))
            {
                //qDebug("MBV::dp - nothing found at %i ", (int) pos_piece);
                item = pos_piece->createSvgItem(cell_size);
                m_cache.insert(pos_piece.data(), item);
                m_board_item->addPiece(item);
            }
            else
            {
                item = m_cache.value(pos_piece.data());
                //qDebug("MBV::dp - cache hit at %i, found %i", (int) pos_piece, (int) item);
            }

            item->setData(0, QVariant((position.getColourToMove() == pos_piece->getColour() ? true
                                                                                            : false)));
            const bool is_white_piece = (MPiece::WHITE == pos_piece->getColour());
            const bool is_black_piece = !is_white_piece;

            const bool have_to_rotate = ((is_white_piece && m_white_rotated180) ||
                                         (is_black_piece && m_black_rotated180));

            const bool is_rotated = item->data(1).toBool();
            const QRectF correction = item->boundingRect();

            // item is not rotated yet
            if(!is_rotated && have_to_rotate)
            {
                item->rotate(180);
                item->translate(-correction.width(), -correction.height());
                item->setData(1, QVariant(true));
            }
            // item was rotated, but needs to be reset
            else if (is_rotated && !have_to_rotate)
            {
                item->rotate(180);
                item->translate(-correction.width(), -correction.height());
                item->setData(1, QVariant(false));
            }

            item->setPos(cell);
            item->show();
        }
    }

    Q_EMIT sendDebugInfo(QString("MBoardView::dp - update duration: %1 ms").arg(profiling.restart()));
}

void MBoardView::drawStartPosition()
{
    MPosition pos;
    drawPosition(pos);
}

void MBoardView::onPieceSelectionRequested(QPoint cell)
{
    // event propagation
    Q_EMIT pieceSelectionRequested(cell);
}

void MBoardView::onPieceMoveRequested(QPoint from, QPoint to)
{
    // event propagation
    Q_EMIT pieceMoveRequested(from, to);
}

void MBoardView::appendDebugOutput(QString msg)
{
    Q_EMIT sendDebugInfo(msg);
}

void MBoardView::onLoadFinished(bool /*ok*/)
{
    m_background_page->setViewportSize(m_background_page->mainFrame()->contentsSize());
    m_background_image = new QImage(m_background_page->mainFrame()->contentsSize(), QImage::Format_ARGB32);
    update(); // schedule a redraw
}

void MBoardView::rotateWhitePieces()
{
    m_white_rotated180 = !m_white_rotated180;
}

void MBoardView::rotateBlackPieces()
{
    m_black_rotated180 = !m_black_rotated180;
}
