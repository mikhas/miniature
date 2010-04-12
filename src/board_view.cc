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
    const QSize chatbox_extent = QSize(480, 160);

}

MBoardView::MBoardView(QWidget *parent)
: QGraphicsView(parent),
  m_central(0),
  m_background_page(new QWebPage),
  m_background_image(0),
  m_board_item_offset(160),
  m_bottom_dashboard(0),
  m_top_dashboard(0),
  m_chatbox(0)
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
}

void MBoardView::addDashboard(Alignment align)
{
    QGraphicsAnchorLayout *layout = static_cast<QGraphicsAnchorLayout *>(m_central->layout());
    MDashboardItem *dashboard = new MDashboardItem(layout, m_central);

    switch (align)
    {
        case ALIGN_BOTTOM:
        {
            delete m_bottom_dashboard;
            m_bottom_dashboard = dashboard;
            layout->addAnchor(m_bottom_dashboard, Qt::AnchorBottom, layout, Qt::AnchorBottom);
        } break;

        case ALIGN_TOP:
        {
            delete m_top_dashboard;
            m_top_dashboard = dashboard;
            m_top_dashboard->setRotation(180);
            layout->addAnchor(m_top_dashboard, Qt::AnchorTop, layout, Qt::AnchorTop);
        } break;
    }
}

MDashboardItem * MBoardView::getDashboard(Alignment align) const
{
    MDashboardItem *dashboard = 0;

    switch (align)
    {
        case ALIGN_BOTTOM:
        {
            dashboard = m_bottom_dashboard;
        } break;

        case ALIGN_TOP:
        {
            dashboard = m_top_dashboard;
        } break;
    }

    return dashboard;
}

void MBoardView::addChatbox()
{
    QGraphicsAnchorLayout *layout = static_cast<QGraphicsAnchorLayout *>(m_central->layout());
    QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget;
    layout->addAnchor(proxy, Qt::AnchorTop, layout, Qt::AnchorTop);

    delete m_chatbox;
    m_chatbox = new QTextEdit;
    /* does not work ...
    QPalette palette;
    m_chatbox->setBackgroundRole(QPalette::Window);
    m_chatbox->setForegroundRole(QPalette::WindowText);
    palette.setColor(m_chatbox->backgroundRole(), Qt::black);
    palette.setColor(m_chatbox->foregroundRole(), Qt::white);
    m_chatbox->setPalette(palette);
    */
    /* does not work, either ...
    m_chatbox->setTextBackgroundColor(Qt::black);
    m_chatbox->setTextColor(Qt::white);
    */

    // It's not my fault if this is the only thing that works, right?
    m_chatbox->setStyleSheet("color: white; background: black; border:none; padding: 10px;");

    proxy->setWidget(m_chatbox);
    proxy->setPreferredSize(chatbox_extent);
}

QTextEdit * MBoardView::getChatbox() const
{
    return m_chatbox;
}

void MBoardView::addBoard(MGraphicsBoardItem *item)
{
    Q_ASSERT_X((0 != m_top_dashboard || 0 != m_bottom_dashboard),
               "addBoard",
               "No player's dashboard found, cannot attach board.");

    QGraphicsAnchorLayout *layout = static_cast<QGraphicsAnchorLayout *>(m_central->layout());

    if (m_top_dashboard)
        layout->addAnchor(m_top_dashboard, Qt::AnchorBottom, item, Qt::AnchorTop);
    else if (m_bottom_dashboard)
        layout->addAnchor(m_bottom_dashboard, Qt::AnchorTop, item, Qt::AnchorBottom);

}

void MBoardView::onLoadFinished(bool /*ok*/)
{
    m_background_page->setViewportSize(m_background_page->mainFrame()->contentsSize());
    m_background_image = new QImage(m_background_page->mainFrame()->contentsSize(), QImage::Format_ARGB32_Premultiplied);
    update(); // schedule a redraw
}
