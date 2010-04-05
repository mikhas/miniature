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

#ifdef HAVE_MINIATURE_OPENGL
  #include <QGLWidget>
#endif

using namespace Miniature;

namespace
{

    const QSize extent = QSize(480, 800);

}

MBoardView::MBoardView(QWidget *parent)
: QGraphicsView(parent),
  m_central(0),
  m_background_page(new QWebPage),
  m_background_image(0),
  m_board_item_offset(160)
{
    QGraphicsView::setScene(new MScene(this));
    // Ignore the growing property of the scene graph and always only show a
    // fixed area. Also, assume we run in fullscreen (therefore, only works for
    // pub mode.).
    // TODO: make this view reusable for other game modes, too.
    setSceneRect(QRect(QPoint(0, 0), extent));
    scene()->addItem(m_central = new QGraphicsWidget(0, Qt::Widget));
    m_central->setPreferredSize(extent);

    QGraphicsAnchorLayout *layout = 0;
    m_central->setLayout(layout = new QGraphicsAnchorLayout);
    layout->setSpacing(0);

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

#ifdef HAVE_MINIATURE_OPENGL
    setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
#endif

    QGraphicsAnchorLayout *layout = static_cast<QGraphicsAnchorLayout *>(m_central->layout());
    m_top_dashboard = new MDashboardItem(layout, m_central);
    layout->addAnchor(m_top_dashboard, Qt::AnchorTop, layout, Qt::AnchorTop);
    m_top_dashboard->disableConfirmButton();
    m_top_dashboard->setRotation(180);
    m_top_dashboard->setZValue(1);

    m_bottom_dashboard = new MDashboardItem(layout, m_central);
    layout->addAnchor(m_bottom_dashboard, Qt::AnchorBottom, layout, Qt::AnchorBottom);
    m_bottom_dashboard->disableConfirmButton();
    m_bottom_dashboard->setZValue(1);
}

void MBoardView::addBoardItem(MGraphicsBoardItem *item)
{
    QGraphicsAnchorLayout *layout = static_cast<QGraphicsAnchorLayout *>(m_central->layout());
    layout->addAnchor(m_top_dashboard, Qt::AnchorBottom, item, Qt::AnchorTop);
    //layout->addAnchor(m_bottom_dashboard, Qt::AnchorTop, item, Qt::AnchorBottom);
}

MDashboardItem * MBoardView::getTopDashboardItem() const
{
    return m_top_dashboard;
}

MDashboardItem * MBoardView::getBottomDashboardItem() const
{
    return m_bottom_dashboard;
}

void MBoardView::onLoadFinished(bool /*ok*/)
{
    m_background_page->setViewportSize(m_background_page->mainFrame()->contentsSize());
    m_background_image = new QImage(m_background_page->mainFrame()->contentsSize(), QImage::Format_ARGB32_Premultiplied);
    update(); // schedule a redraw
}
