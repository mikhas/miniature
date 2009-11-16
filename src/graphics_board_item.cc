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

#include "graphics_board_item.h"

#include <QPainterPath>
#include <QGraphicsScene>
#include <QTime>
#include <QGraphicsItemAnimation>

#include <cmath>

using namespace Miniature;

MGraphicsBoardItem::MGraphicsBoardItem(QGraphicsItem *parent)
: QGraphicsSvgItem(parent),
  m_selection_duration(2000),
  m_frame(0),
  m_frame_outline(4),
  m_time_line(0)
{
    setupFrameAndTimeLine();
}

MGraphicsBoardItem::MGraphicsBoardItem(const QString &fileName, QGraphicsItem *parent)
: QGraphicsSvgItem(fileName, parent),
  m_selection_duration(2000),
  m_frame(0),
  m_frame_outline(4),
  m_time_line(0)
{
    setupFrameAndTimeLine();
}

MGraphicsBoardItem::~MGraphicsBoardItem()
{}

int MGraphicsBoardItem::getCellSize() const
{
    return floor(boundingRect().height() / 8);
}

void MGraphicsBoardItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    static QGraphicsItem *active_item = 0;
    static QTime duration;

    // auto-deselect
    if (m_selection_duration < duration.elapsed())
    {
        active_item = 0;
        resetFrame();
    }

    if(!active_item) // try to find an active item
    {
        active_item = (scene()->items(event->pos().x() - 1, event->pos().y() -1, 2, 2))[0];
        if (active_item == this)
        {
           active_item = 0;
           resetFrame();
           //std::cout << "mouse_pressed: selected board!" << std::endl;
        }
        else // valid selection!
        {
            duration.start();
            putFrameAt(active_item->pos());
        }
        //std::cout << "mouse_pressed: try to select" << std::endl;
    }
    else // we have an active item.
    {
        const int cell_size = getCellSize();
        //std::cout << "mouse_pressed: sth was already selected" << std::endl;

        int src_x = floor(active_item->pos().x() / cell_size);
        int src_y = floor(active_item->pos().y() / cell_size);

        int dst_x = floor(event->pos().x() / cell_size);
        int dst_y = floor(event->pos().y() / cell_size);

        if((src_x != dst_x) || (src_y != dst_y))
        {
            active_item->setPos(QPointF(dst_x * cell_size, dst_y * cell_size));
            Q_EMIT pieceMoved(QPoint(src_x, src_y), QPoint(dst_x, dst_y));
        }

        active_item = 0;
        resetFrame();
    }
}

void MGraphicsBoardItem::setupFrameAndTimeLine()
{
    // take a guess on cell size if need be
    const int frame_size = (getCellSize() > m_frame_outline ? getCellSize() : 60) - m_frame_outline;

    m_frame = new QGraphicsRectItem(QRectF(0, 0, frame_size, frame_size), this);
    m_frame->setPen(QPen(Qt::darkBlue, m_frame_outline, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin));
    m_frame->hide();

    m_time_line = new QTimeLine(m_selection_duration, this);
    m_time_line->setFrameRange(0, 39);

    QObject::connect(m_time_line, SIGNAL(frameChanged(int)), this, SLOT(fadeOutFrame(int)));
}

void MGraphicsBoardItem::fadeOutFrame(int step)
{
    // empirical values for nice fading:
    if (step > 30) // start fading after ca. 1.5 sec
    {
        // fade-out in 10% steps
        m_frame->setOpacity(1.0 - (0.1 * (step - 30)));
    }

    if (39 == step)
    {
        resetFrame();
    }
}

void MGraphicsBoardItem::putFrameAt(QPointF pos)
{
    Q_CHECK_PTR(m_frame);
    Q_CHECK_PTR(m_time_line);

    int frame_outline_correction = floor(m_frame_outline * 0.5);
    m_frame->setPos(pos.x() + frame_outline_correction, pos.y() + frame_outline_correction);
    m_frame->show();
    m_time_line->start();
}

void MGraphicsBoardItem::resetFrame()
{
    m_time_line->stop();
    m_frame->hide();
    m_frame->setOpacity(1);
}
