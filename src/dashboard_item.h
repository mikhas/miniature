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

#include <QtCore>
#include <QtGui>

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

public Q_SLOTS:
    void flash();
    void storeBackgroundBrush();
    void restoreBackgroundBrush();

Q_SIGNALS:
   void pressed();
   void released();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    QGraphicsEllipseItem *m_background; /*!< The background item. Usage is
                                             strictly optional. */
    QBrush m_stored_background_brush; /*!< The stored background brush so that
                                           we can restore it, e.g., after
                                           flashing the background. */
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

    /*!
     *  Resets the UI elements to the state *after* setupUi.
     */
    void resetUi();

Q_SIGNALS:
    void confirmButtonPressed();   /*!< confirmed a move >*/
    void requestsButtonPressed();  /*!< wants to open requests dialog >*/
    void takebackButtonPressed();  /*!< wants to take back a move >*/
    void avatarButtonPressed();    /*!< (n/a) >*/
    void drawButtonPressed();      /*!< wants to propose a draw >*/
    void drawAccepted();           /*!< accepted a draw >*/
    void adjournButtonPressed();   /*!< wants to adjourn the game >*/
    void adjournAccepted();        /*!< accepted to adjourn the game >*/
    void resignButtonPressed();    /*!< the player wants to give up >*/
    void resignConfirmed();        /*!< the player gave up >*/
    void abortGameButtonPressed(); /*!< the player wants to end the game >*/
    void abortGameConfirmed();     /*!< the player aborted the game >*/

public Q_SLOTS:
    void enableConfirmButton();
    void disableConfirmButton();

    void enableRequestsButton();
    void disableRequestsButton();
    void showRequestsMenu();

    void showGameMenu();

    void drawOffered();
    void onDrawAccepted();

    void adjournOffered();
    void onAdjournAccepted();

    void showResignConfirmation();
    void showAbortGameConfirmation();

    void onGameWon();
    void onGameLost();

    void setStatusText(const QString &text);
    void hideStatus();
    void fadeOutStatus();

    void flash();
    void appendToLastMovesList(const QString &move_notation);

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
     *  Helper method to create the acceptance dialogs when sth. was offered.
     *  @param[in] title the title of the dialog
     *  @param[in] button the button to use, can be null
     */
    void showConfirmationDialog(const QString &title, QPushButton *button);

    /*
     *  Changes the UI after a game ended. Players can only abort the game from
     *  that point on, but my choose to discuss the chess position still.
     */
    void showEndGameUi();

    /*!
     *  This method tried to wrap libosso-abook in order to get the avatar from
     *  a given contact. Sadly, it won't compile with g++ so it probably needs
     *  to be moved into a gcc-compiled object file of its own.
     */
    QPixmap * getContactsAvatar(const QString &nick);

    MDashboardButton *m_confirm; /*!< The 'play' button that needs to be pressed to confirm a move. */
    MDashboardButton *m_requests; /*!< The 'stop' button to request some sort of game resolution, by negotiation. */
    MDashboardButton *m_takeback; /*!< The 'rewind' button to ask for taking back a move. */
    MDashboardButton *m_avatar; /*!< The avatar from the contact list. */
    QGraphicsProxyWidget *m_requests_dialog; /*!< The proxy widget for the resolution menu that allows a player to send a request. */
    QGraphicsProxyWidget *m_game_dialog; /*!< The proxy widget for the game menu that allows a player to leave the current screen. */
    QGraphicsProxyWidget *m_offers_dialog; /*!< The proxy widget for the dialog to either accept or decline an offer. */
    QGraphicsTextItem *m_status; /*!< The status label below (ontop) the board. */
    QPropertyAnimation *m_status_anim; /*!< The fade out animation for the status label. */
    QGraphicsTextItem *m_last_moves; /*!< The text item that shows the last n moves of a player. */
};

} // namespace Miniature

#endif // DASHBOARD_ITEM_H__

