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

#ifndef DASHBOARD_ITEM_H__
#define DASHBOARD_ITEM_H__

#include <QGraphicsObject>
#include <QGraphicsPixmapItem>

namespace Miniature
{

class MDashboardItem
: public QGraphicsObject
{
    Q_OBJECT

public:
    explicit MDashboardItem(QGraphicsItem* parent = 0);
    virtual ~MDashboardItem();

    /*!
     *  Overriden from QGraphicsItem, sets the effective size of the dashboard,
     *  effectively.
     */
    virtual QRectF boundingRect() const;

    /*!
     *  Initializes the UI of the dashboard.
     */
    void setupUi();

protected:

    /*!
     *  This item does not paint anything itself (yet).
     */
    virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) {}

private:
    /*!
     *  We create our own 'buttons' from the Maemo5 media player icons and QGraphicsPixmapItems.
     */
    void setupButton(QGraphicsPixmapItem *item, const QPoint &origin, const QPixmap &pixmap);

    /*!
     *  This method tried to wrap libosso-abook in order to get the avatar from
     *  a given contact. Sadly, it won't compile with g++ so it probably needs
     *  to be moved into a gcc-compiled object file of its own.
     */
    QPixmap * getContactsAvatar(const QString &nick);

    QGraphicsPixmapItem *m_confirm; /*!< The 'play' button that needs to be pressed to confirm a move. */
    QGraphicsPixmapItem *m_request; /*!< The 'stop' button to request some sort of game resolution, by negotiation. */
    QGraphicsPixmapItem *m_take_back; /*!< The 'back' button to ask for taking back a move. */
    QGraphicsPixmapItem *m_avatar; /*!< The avatar from the contact list. */
};

} // namespace Miniature

#endif // DASHBOARD_ITEM_H__

