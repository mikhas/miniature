/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2010 Michael Hasselmann <michael@taschenorakel.de>
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

#include <dashboard_button.h>

using namespace Miniature;

namespace
{

const int flash_duration = 200;

}

MDashboardButton::
MDashboardButton(const QPoint &origin, const QPixmap &active, const QPixmap &inactive, const QPixmap &pressed,
                 const QPixmap &flash, QGraphicsItem *item, QObject *parent)
: QObject(parent),
  QGraphicsPixmapItem(active, item),
  m_active_pixmap(active),
  m_inactive_pixmap(inactive),
  m_pressed_pixmap(pressed),
  m_flash_pixmap(flash),
  m_active(true)
{
    setEnabled(true); // receive scene events
    setPos(origin);
    show();
}

MDashboardButton::
~MDashboardButton()
{}

void MDashboardButton::
setActive(bool active)
{
    setEnabled(active);
    m_active = active;
    restore();
}

void MDashboardButton::
flash()
{
    setPixmap(m_flash_pixmap);
    QTimer::singleShot(flash_duration, this, SLOT(restore()));
}

void MDashboardButton::
restore()
{
    setPixmap((m_active ? m_active_pixmap : m_inactive_pixmap));
}

void MDashboardButton::
mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();
    setPixmap(m_pressed_pixmap);
}

void MDashboardButton::
mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();
    restore();
    Q_EMIT pressed();
}
