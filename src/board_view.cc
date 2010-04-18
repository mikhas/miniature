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

#include <QtCore>
#include <QtGui>

#ifdef HAVE_MINIATURE_OPENGL
  #include <QGLWidget>
#endif

using namespace Miniature;

namespace
{

    const QSize extent_portrait  = QSize(480, 800);
    const QSize extent_landscape = QSize(800, 480);
    const QSize extent_scene     = QSize(extent_landscape.width(), extent_portrait.height());
    const QSize extent_chatbox   = QSize(480, 160);
    const int text_edit_line_height = 64; // TODO: find out how to query this.
}

MBoardView::MBoardView(QWidget *parent)
: QGraphicsView(parent),
  m_central(0),
  m_background_page(new QWebPage),
  m_background_image(0),
  m_board_item_offset(160),
  m_board_item(0),
  m_bottom_dashboard(0),
  m_top_dashboard(0),
  m_chatbox(0),
  m_message(0)
{
    setStyleSheet("* {border: 6px solid green;}");
    MScene *s = 0;
    QGraphicsView::setScene(s = new MScene(this));
    scene()->addItem(m_central = new QGraphicsWidget(0, Qt::Widget));

    QShortcut *cancel = new QShortcut(this);
    cancel->setKey(Qt::Key_Escape);

    connect(cancel, SIGNAL(activated()),
            s,      SLOT(resetModalItem()));

    QGraphicsAnchorLayout *l = 0;
    m_central->setLayout(l = new QGraphicsAnchorLayout);
    l->setSpacing(0);

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

void  MBoardView::drawBackground(QPainter *painter, const QRectF &region)
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

void MBoardView::enableAutoOrientationSupport()
{
    setAttribute(Qt::WA_Maemo5AutoOrientation, true);
    connect(QApplication::desktop(), SIGNAL(resized(int)),
            this,                    SLOT(onOrientationChanged()),
            Qt::UniqueConnection);
}

void MBoardView::applyPortraitLayout()
{
    Q_CHECK_PTR(m_central);
    Q_CHECK_PTR(m_board_item);

    Q_ASSERT_X((0 != m_bottom_dashboard || 0 != m_top_dashboard),
               "applyPortraitLayout",
               "No dashboards found - cannot apply layout!");

    QGraphicsAnchorLayout *layout = static_cast<QGraphicsAnchorLayout *>(m_central->layout());
    for (int idx = 0; idx < layout->count(); ++idx)
        layout->removeAt(idx);

    static_cast<QMainWindow *>(parent())->setAttribute(Qt::WA_Maemo5PortraitOrientation, true);
    setMinimumSize(extent_portrait);
    resize(extent_portrait);

    scene()->setSceneRect(QRect(QPoint(0, 0), extent_portrait));
    m_central->setPreferredSize(extent_portrait);

    if (m_bottom_dashboard)
    {
        layout->addAnchor(m_bottom_dashboard, Qt::AnchorTop,
                          m_board_item,       Qt::AnchorBottom);
        m_bottom_dashboard->applyPortraitLayout();
    }

    if (m_top_dashboard)
    {
        layout->addAnchor(m_top_dashboard, Qt::AnchorBottom,
                          m_board_item,    Qt::AnchorTop);
        m_top_dashboard->applyPortraitLayout();
    }

    update();
}

void MBoardView::applyLandscapeLayout()
{
    Q_CHECK_PTR(m_central);
    Q_CHECK_PTR(m_board_item);

    Q_ASSERT_X((0 != m_bottom_dashboard || 0 != m_top_dashboard),
               "applyLandscapeLayout",
               "No dashboards found - cannot apply layout!");

    QGraphicsAnchorLayout *layout = static_cast<QGraphicsAnchorLayout *>(m_central->layout());
    for (int idx = 0; idx < layout->count(); ++idx)
        layout->removeAt(idx);

    static_cast<QMainWindow *>(parent())->setAttribute(Qt::WA_Maemo5LandscapeOrientation, true);
    setMinimumSize(extent_landscape);
    resize(extent_landscape);

    scene()->setSceneRect(QRect(QPoint(0, 0), extent_landscape));
    m_central->setPreferredSize(extent_landscape);

    layout->addAnchor(m_board_item, Qt::AnchorLeft,
                      layout,       Qt::AnchorLeft);
    layout->addAnchor(m_board_item, Qt::AnchorTop,
                      layout,       Qt::AnchorTop);
    layout->addAnchor(m_board_item, Qt::AnchorBottom,
                      layout,       Qt::AnchorBottom);

    if (m_bottom_dashboard)
    {
        qDebug() << "bottom db found";
        layout->addAnchor(m_bottom_dashboard, Qt::AnchorBottom,
                          layout,             Qt::AnchorBottom);
        layout->addAnchor(m_bottom_dashboard, Qt::AnchorRight,
                          layout,             Qt::AnchorRight);
        m_bottom_dashboard->applyLandscapeLayout();
    }

    if (m_top_dashboard)
    {
        qDebug() << "top db found";
        layout->addAnchor(m_top_dashboard, Qt::AnchorTop,
                          layout,          Qt::AnchorTop);
        layout->addAnchor(m_top_dashboard, Qt::AnchorRight,
                          layout,          Qt::AnchorRight);
        m_top_dashboard->applyLandscapeLayout();
    }

    update();
}

void MBoardView::startNewMessage()
{
    setAttribute(Qt::WA_Maemo5LandscapeOrientation, true);
    applyLandscapeLayout();

    QTextEdit *edit = new QTextEdit;
    edit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QShortcut *new_message = new QShortcut(edit);
    new_message->setKey(QKeySequence(QKeySequence::InsertParagraphSeparator));

    QShortcut *newline = new QShortcut(edit);
    newline->setKey(QKeySequence(QKeySequence::InsertLineSeparator));

    connect(new_message, SIGNAL(activated()),
            this,        SLOT(onNewMessage()));

    connect(newline, SIGNAL(activated()),
            this,    SLOT(onNewlineInMessage()));

    if (m_message)
        delete m_message;

    m_message = new QGraphicsProxyWidget;
    m_message->setWidget(edit);
    m_message->resize(width(), text_edit_line_height);
    scene()->addItem(m_message);
    m_message->setPos(0, height() - text_edit_line_height);

    static_cast<MScene *>(scene())->setModalItem(m_message);
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

    if (m_chatbox)
        delete m_chatbox;

    m_chatbox = new QGraphicsProxyWidget;
    m_chatbox->setPreferredSize(extent_chatbox);
    layout->addAnchor(m_chatbox, Qt::AnchorTop, layout, Qt::AnchorTop);

    QTextEdit *edit = new QTextEdit;
    m_chatbox->setWidget(edit);
    edit->setTextInteractionFlags(Qt::TextBrowserInteraction);
    // It's not my fault if this is the only thing that works, right?
    edit->setStyleSheet("color: white; background: black; border:none; padding: 10px;");

    // Enable typing of chat messages:
    connect(static_cast<MScene *>(scene()), SIGNAL(focusInOnKeyPressed()),
            this,                           SLOT(startNewMessage()),
            Qt::UniqueConnection);
}

QTextEdit * MBoardView::getChatbox() const
{
    return static_cast<QTextEdit *>(m_chatbox->widget());
}

void MBoardView::addBoard(MGraphicsBoardItem *item)
{
    Q_ASSERT_X((0 != item),
               "addBoard",
               "No board item specified, invalid view.");

    Q_ASSERT_X((0 != m_top_dashboard || 0 != m_bottom_dashboard),
               "addBoard",
               "No player's dashboard found, cannot attach board.");

    delete m_board_item;
    m_board_item = item;

    applyPortraitLayout();
}

void MBoardView::onLoadFinished(bool /*ok*/)
{
    m_background_page->setViewportSize(m_background_page->mainFrame()->contentsSize());
    m_background_image = new QImage(m_background_page->mainFrame()->contentsSize(), QImage::Format_ARGB32_Premultiplied);
    update(); // schedule a redraw
}

void MBoardView::onNewMessage()
{
    enableAutoOrientationSupport();

    QTextEdit *edit = static_cast<QTextEdit *>(m_message->widget());
    QString msg = edit->toPlainText().trimmed();
    if (!msg.isEmpty())
        Q_EMIT sendMessageRequest(msg);

    static_cast<MScene *>(scene())->resetModalItem();
    m_message->close();
}

void MBoardView::onNewlineInMessage()
{
    if (m_message->boundingRect().height() + (2 * text_edit_line_height) < height())
    {
        m_message->resize(width(), m_message->boundingRect().height() + text_edit_line_height);
        m_message->setPos(0, m_message->pos().y() - text_edit_line_height);
    }

    QTextEdit *edit = static_cast<QTextEdit *>(m_message->widget());
    edit->insertPlainText("\n");
    edit->ensureCursorVisible();
}

void MBoardView::onOrientationChanged()
{
    if (QApplication::desktop()->height() > QApplication::desktop()->width())
        applyPortraitLayout();
    else
        applyLandscapeLayout();
}
