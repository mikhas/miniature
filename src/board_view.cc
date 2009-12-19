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

using namespace Miniature;

MBoardView::MBoardView(QWidget *parent)
: QGraphicsView(parent),
  m_background_page(new QWebPage),
  m_background_image(0),
  m_board_item_offset(80)
{
    QGraphicsView::setScene(new QGraphicsScene(this));

    connect(m_background_page, SIGNAL(loadFinished(bool)),
            this, SLOT(onLoadFinished(bool)));

    setup();

#ifdef Q_WS_MAEMO_5
    setStyleSheet("* {background:black;}");
#endif
#ifndef Q_WS_MAEMO_5
    setStyleSheet("* {background:transparent;}");
#endif
}

MBoardView::~MBoardView()
{
    delete m_background_image;
}

void MBoardView::setScene(QGraphicsScene *scene)
{
    QGraphicsView::setScene(scene);
    setup();
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

void MBoardView::setup()
{
    QPalette palette = m_background_page->palette();
    palette.setColor(QPalette::Base, Qt::transparent);
    m_background_page->setPalette(palette);

    m_background_page->mainFrame()->load(QUrl("qrc:/boards/glossy.svg"));
}

void MBoardView::addBoardItem(MGraphicsBoardItem *item)
{
    scene()->addItem(item);
    item->setPos(QPoint(0, m_board_item_offset));
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
    proxy_widget->setPos(QPoint(0, m_board_item_offset + 480 + 5));
}

void MBoardView::onLoadFinished(bool /*ok*/)
{
    m_background_page->setViewportSize(m_background_page->mainFrame()->contentsSize());
    m_background_image = new QImage(m_background_page->mainFrame()->contentsSize(), QImage::Format_ARGB32);
    update(); // schedule a redraw
}
