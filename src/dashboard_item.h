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

#include <QObject>
#include <QPixmap>
#include <QIcon>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QStandardItemModel>

namespace Miniature
{

class MDashboardButton
: public QObject, // we emit signals in our event handlers
  public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit MDashboardButton(const QIcon &icon, QGraphicsItem *item = 0, QObject *parent = 0);
    virtual ~MDashboardButton();

    /*!
     *  Draws a circular-shaped background around your button pixmap. Purely
     *  optional.
     */
    void setupButtonBackground();

    /*!
     *  Let's you change how the background of the button is drawn. Only makes
     *  sense with transparent pixbufs, I think.
     */
    void setBackgroundBrush(const QBrush &brush);

    /*!
     *  Sets the button either active or inactive. An inactive button cannot be
     *  pressed.
     */
    void setActive(bool active);

    /*!
     *  Returns the active state of this button.
     */
    bool isActive() const;

Q_SIGNALS:
   void pressed();
   void released();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    QGraphicsEllipseItem *m_background; /*!< The background item. Usage is
                                             strictly optional. */
    QIcon m_icon; /*!< The icon used for the button. */
    bool m_active; /*!< The attribute for the button's active state. */
};


/*! This class represents the player's "dashboard", that is, the player's
 *  controls. It replaces the former mActionAreas.
 */
class MDashboardItem
: public QGraphicsObject
{
    Q_OBJECT

public:
    explicit MDashboardItem(QGraphicsItem *parent = 0);
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

Q_SIGNALS:
    void confirmButtonPressed();
    void requestButtonPressed();
    void takebackButtonPressed();
    void avatarButtonPressed();
    void fullscreenButtonPressed();

public Q_SLOTS:
    void enableConfirmButton();
    void disableConfirmButton();
    void showRequestsMenu();
    void toggleFullscreen();

protected:

    /*!
     *  This item does not paint anything itself (yet).
     */
    virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) {}

private:
    /*!
     *  We create our own 'buttons' from the Maemo5 media player icons and
     *  Maemo 5 stock icons. Both are put in the graphics view via
     *  QGraphicsPixmapItems (which also handles the mouse events).
     */
    MDashboardButton * createButtonWithBackground(const QPoint &origin, const QIcon &icon);

    /*!
     *  This method tried to wrap libosso-abook in order to get the avatar from
     *  a given contact. Sadly, it won't compile with g++ so it probably needs
     *  to be moved into a gcc-compiled object file of its own.
     */
    QPixmap * getContactsAvatar(const QString &nick);

    MDashboardButton *m_confirm; /*!< The 'play' button that needs to be pressed to confirm a move. */
    MDashboardButton *m_request; /*!< The 'stop' button to request some sort of game resolution, by negotiation. */
    MDashboardButton *m_takeback; /*!< The 'rewind' button to ask for taking back a move. */
    MDashboardButton *m_avatar; /*!< The avatar from the contact list. */
    MDashboardButton *m_fullscreen; /*!< The 'toggle fullscreen' button. */
    bool m_fullscreen_enabled; /*!< The attribute to decide between fullscreen and windowed mode. */
};

} // namespace Miniature

#endif // DASHBOARD_ITEM_H__

