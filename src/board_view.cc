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

#include <iostream>
#include <QTime>

using namespace Miniature;

MBoardView::MBoardView(QWidget *parent)
: QGraphicsView(parent),
  m_board_item(0),
  m_background_page(new QWebPage),
  m_background_image(0)
{
    QGraphicsView::setScene(new QGraphicsScene(this));

    connect(m_background_page, SIGNAL(loadFinished(bool)),
            this, SLOT(onLoadFinished(bool)));

    setBoardBackground();

}

MBoardView::~MBoardView()
{
    for(MSvgItemCache::iterator iter = m_cache.begin();
        iter != m_cache.end();
        ++iter)
    {
        delete iter.value();
    }

    delete m_background_image;
}

void MBoardView::setScene(QGraphicsScene *scene)
{
    QGraphicsView::setScene(scene);
    setBoardBackground();
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

    painter->drawImage(region, *m_background_image);
}

void MBoardView::setBoardBackground()
{
    m_board_item = new MGraphicsBoardItem();

    QPalette palette = m_background_page->palette();
    palette.setColor(QPalette::Base, Qt::transparent);
    m_background_page->setPalette(palette);

    m_background_page->mainFrame()->load(QUrl("qrc:/boards/glossy.svg"));

    connect(m_board_item, SIGNAL(pieceMoveRequested(QPoint, QPoint)),
            this, SLOT(onPieceMoveRequested(QPoint, QPoint)));

    scene()->addItem(m_board_item);

    connect(m_board_item, SIGNAL(sendDebugInfo(QString)),
            this, SLOT(appendDebugOutput(QString)));
}

void MBoardView::drawPosition(const MPosition &position)
{
    static QTime profiling;
    profiling.restart();

    Q_CHECK_PTR(m_board_item);

    m_board_item->removePieces();

    for(MPosition::MPieces::const_iterator iter = position.begin();
        iter != position.end();
        ++iter)
    {
        MPiece* pos_piece = *iter;
        if (pos_piece) // non-empty cell
        {
            QPoint cell = position.indexToPoint(std::distance(position.begin(), iter), m_board_item->getCellSize());
            QGraphicsSvgItem* item = 0;

            // Querying cache for the item.
            if (!m_cache.contains(pos_piece))
            {
                //qDebug("MBV::dp - nothing found at %i ", (int) pos_piece);
                item = pos_piece->createSvgItem(m_board_item->getCellSize());
                m_cache.insert(pos_piece, item);
                m_board_item->addPiece(item);
            }
            else
            {
                item = m_cache.value(pos_piece);
                //qDebug("MBV::dp - cache hit at %i, found %i", (int) pos_piece, (int) item);
            }

            item->setData(0, QVariant((position.getColourToMove() == pos_piece->getColour() ? true
                                                                                            : false)));
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

void MBoardView::onPieceMoveRequested(QPoint from, QPoint to)
{
    // event propagation
    Q_EMIT pieceMoveRequested(from, to);
}

void MBoardView::appendDebugOutput(QString msg)
{
    Q_EMIT sendDebugInfo(msg);
}

void MBoardView::onLoadFinished (bool /*ok*/)
{
    m_background_page->setViewportSize(m_background_page->mainFrame()->contentsSize());
    m_background_image = new QImage(m_background_page->mainFrame()->contentsSize(), QImage::Format_ARGB32);
    setCacheMode(QGraphicsView::CacheBackground); // calls drawBackground & caches the pixmap

}
