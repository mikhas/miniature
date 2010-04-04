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

#ifndef DASHBOARD_BUTTON_H__
#define DASHBOARD_BUTTON_H__

#include <QtCore>
#include <QtGui>

namespace Miniature
{

/*!
 *  This class is used for buttons in MDashboards. Such a button is constructed from a scene position and four pixmaps
 *  (active, inactive, pressed, flash), whereas the flash pixmap is used for a flashing alert when the user attempted
 *  an illegal action in the UI.
 */
class MDashboardButton
: public QObject, // we emit signals in our event handlers
  public QGraphicsPixmapItem // not a QObject (yet)
{
    Q_OBJECT

public:
    explicit MDashboardButton(const QPoint &origin, const QPixmap &active, const QPixmap &inactive,
                              const QPixmap &pressed, const QPixmap &flash, QGraphicsItem *item = 0,
                              QObject *parent = 0);
    virtual ~MDashboardButton();

    //! \brief Sets the button either active or inactive. An inactive button cannot be pressed.
    void setActive(bool active);

    //! \brief Returns whether the button is active (true) or inactive (false).
    bool isActive() const;

public Q_SLOTS:
    void flash();
    void restore();

Q_SIGNALS:
   void pressed();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    const QPixmap m_active_pixmap;
    const QPixmap m_inactive_pixmap;
    const QPixmap m_pressed_pixmap;
    const QPixmap m_flash_pixmap;
    bool m_active;
};

} // namespace Miniature

#endif // DASHBOARD_BUTTON_H__
