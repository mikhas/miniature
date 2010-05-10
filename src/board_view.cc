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

#include <QtSvg/QSvgRenderer>

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

MBoardView::MBoardView(QMainWindow *parent)
: QGraphicsView(parent),
  m_board_item_offset(160),
  m_board_item(0),
  m_bottom_dashboard(0),
  m_top_dashboard(0),
  m_chatbox(0),
  m_message(0),
  m_new_message_shortcut(0),
  m_newline_shortcut(0)
{
    MScene *s = 0;
    QGraphicsView::setScene(s = new MScene(this));

    QShortcut *cancel = new QShortcut(this);
    cancel->setKey(Qt::Key_Escape);

    connect(cancel, SIGNAL(activated()),
            s,      SLOT(resetModalItem()));

    setup();
    setupShortcuts();

#ifdef Q_WS_MAEMO_5
    setStyleSheet("* {background:black;}");
#endif
#ifndef Q_WS_MAEMO_5
    setStyleSheet("* {background:transparent;}");
#endif
}

MBoardView::~MBoardView()
{}

void  MBoardView::drawBackground(QPainter *painter, const QRectF &region)
{
    static QSvgRenderer bg(QString(":/boards/glossy.svg"), 0);
    static QImage img = QImage(QSize(480, 480), QImage::Format_ARGB32_Premultiplied);

    QPainter bg_painter(&img);
    bg.render(&bg_painter);

    painter->drawImage(region, img, QRectF(region.x(),
                                           region.y() - (width() <= height() ? m_board_item_offset : 0),
                                           region.width(),
                                           region.height()));
}

void MBoardView::setup()
{
#ifdef HAVE_MINIATURE_OPENGL
    setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
#endif
    update();
}

void MBoardView::enableAutoOrientationSupport()
{
    connect(QApplication::desktop(), SIGNAL(resized(int)),
            this,                    SLOT(onOrientationChanged()),
            Qt::UniqueConnection);

    static_cast<QMainWindow *>(parent())->setAttribute(Qt::WA_Maemo5AutoOrientation, true);
    onOrientationChanged();
}

void MBoardView::applyPortraitLayout()
{
    Q_ASSERT_X((0 != m_board_item),
               "applyPortraitLayout",
               "No board item found - cannot apply layout!");

    Q_ASSERT_X((0 != m_bottom_dashboard || 0 != m_top_dashboard),
               "applyPortraitLayout",
               "No dashboards found - cannot apply layout!");

    setMinimumSize(extent_portrait);
    resize(extent_portrait);

    scene()->setSceneRect(QRect(QPoint(0, 0), extent_portrait));

    m_board_item->setPos(0, 160);

    if (m_bottom_dashboard)
    {
        m_bottom_dashboard->setPos(0, 160 + 480);
        m_bottom_dashboard->applyPortraitLayout();

        if (m_chatbox)
        {
            m_chatbox->setPos(0, 0);
            m_chatbox->resize(480, 160);
        }
    }

    if (m_top_dashboard)
    {
        m_top_dashboard->setPos(0, 0);
        m_top_dashboard->applyPortraitLayout();
    }
}

void MBoardView::applyLandscapeLayout()
{
    Q_ASSERT_X((0 != m_board_item),
               "applyLandscapeLayout",
               "No board item found - cannot apply layout!");

    Q_ASSERT_X((0 != m_bottom_dashboard || 0 != m_top_dashboard),
               "applyLandscapeLayout",
               "No dashboards found - cannot apply layout!");

    setMinimumSize(extent_landscape);
    resize(extent_landscape);

    scene()->setSceneRect(QRect(QPoint(0, 0), extent_landscape));

    m_board_item->setPos(0, 0);

    if (m_bottom_dashboard)
    {
        m_bottom_dashboard->setPos(480, 240);
        m_bottom_dashboard->applyLandscapeLayout();

        if (m_chatbox)
        {
            m_chatbox->setPos(480, 0);
            m_chatbox->resize(800 - 480, 240);
        }
    }

    if (m_top_dashboard)
    {
        m_top_dashboard->setPos(480, 0);
        m_top_dashboard->applyLandscapeLayout();
    }
}

void MBoardView::startNewMessage()
{
    if (width() <= height())
        applyLandscapeLayout();

    QTextEdit *edit = new QTextEdit;
    edit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    if (m_message)
        delete m_message;

    m_message = new QGraphicsProxyWidget;
    m_message->setWidget(edit);
    m_message->resize(width(), text_edit_line_height);
    scene()->addItem(m_message);
    m_message->setPos(0, height() - text_edit_line_height);

    static_cast<MScene *>(scene())->setModalItem(m_message);
    connect(m_message, SIGNAL(visibleChanged()),
            this,      SLOT(destroyMessage()));
}

void MBoardView::setupShortcuts()
{
    if (!m_new_message_shortcut)
    {
        m_new_message_shortcut = new QShortcut(this);
        m_new_message_shortcut->setKey(QKeySequence(QKeySequence::InsertParagraphSeparator));

        connect(m_new_message_shortcut, SIGNAL(activated()),
                this,                   SLOT(onNewMessage()));
    }

    if (!m_newline_shortcut)
    {
        m_newline_shortcut = new QShortcut(this);
        m_newline_shortcut->setKey(QKeySequence(QKeySequence::InsertLineSeparator));

        connect(m_newline_shortcut, SIGNAL(activated()),
                this,               SLOT(onNewlineInMessage()));
    }
}

void MBoardView::addDashboard(Alignment align)
{
    MDashboardItem *dashboard = new MDashboardItem;

    switch (align)
    {
        case ALIGN_BOTTOM:
        {
            delete m_bottom_dashboard;
            m_bottom_dashboard = dashboard;
            scene()->addItem(m_bottom_dashboard);
        } break;

        case ALIGN_TOP:
        {
            delete m_top_dashboard;
            m_top_dashboard = dashboard;
            m_top_dashboard->setRotation(180);
            scene()->addItem(m_top_dashboard);
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
    if (m_chatbox)
        delete m_chatbox;

    m_chatbox = new QGraphicsProxyWidget;
    scene()->addItem(m_chatbox);

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

    delete m_board_item;
    m_board_item = item;
    scene()->addItem(m_board_item);
}

void MBoardView::onNewMessage()
{
    if (!m_message)
        return;

    QTextEdit *edit = static_cast<QTextEdit *>(m_message->widget());
    QString msg = edit->toPlainText().trimmed();
    if (!msg.isEmpty())
        Q_EMIT sendMessageRequest(msg);

    static_cast<MScene *>(scene())->resetModalItem();
}

void MBoardView::onNewlineInMessage()
{
    if (!m_message)
        return;

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

void MBoardView::destroyMessage()
{
    delete m_message;
    m_message = 0;
}
