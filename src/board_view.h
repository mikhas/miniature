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

#ifndef BOARD_VIEW_H__
#define BOARD_VIEW_H__

#include <position.h>
#include <graphics_board_item.h>
#include <dashboard_item.h>
#include <scene.h>

#include <QGraphicsSvgItem>
#include <QString>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QHash>

#include <QPainter>
#include <QWebPage>
#include <QWebFrame>
#include <QImage>

namespace Miniature
{
/*! \brief Miniature's scene view
 *
 *  This class' intent is to represent a chess board and the necessary UI
 *  elements for one (P2P, online) or two players (local).
 */
class MBoardView
: public QGraphicsView
{
    Q_OBJECT

public:
    //! This type is used to decide where to place a dashboard or chat item in a board view.
    enum Alignment
    {
        ALIGN_BOTTOM = 0,
        ALIGN_TOP    = 1
    };

    explicit MBoardView(QWidget *parent = 0);
    virtual ~MBoardView();

    /*!
     *  Overriden from QGraphicsView, performs the extra steps necessary
     *  (setup of board background mainly).
     *
     *  @param[in] scene The scene graph instance to be shown in the view.
     */
    virtual void setScene(QGraphicsScene* scene);

    /*!
     *  The board item is a special scene item, as it contains all MPieces and
     *  is also reponsible for related event propagation. This method also
     *  positions the the board item.
     *
     *  @param[in] item The board item instance.
     */
    void addBoard(MGraphicsBoardItem *item);

    /*!
     *  Creates a new dashboard item (internally) and inserts it into the
     *  view's layout according to \a align. Deletes previously added
     *  dashboard.
     *  \sa MBoardView::getDashboard
     */
    void addDashboard(Alignment align);

    /*!
     *  Returns the dashboard item according to its layout position.
     *  Can return 0, in which case there is no item for the queried
     *  alignment!
     *  @param[in] align The alignment.
     */
    MDashboardItem * getDashboard(Alignment align) const;

    /*!
     *  Creates a new chatbox (proxy) widget (internally) and inserts it into
     *  the view's layout. Deletes previously added chatbox.
     */
    void addChatbox();

    /*!
     *  Returns the chatbox widget according to its layout position.
     *  Can return 0, in which case there is no widget!
     */
    QTextEdit * getChatbox() const;

protected:
    /*!
     *  Overriden from QGraphicsView, enables background caching.
     *
     *  @param[in] painter The painter used to draw the background.
     *  @param[in] region The region in which the (partial!) background needs
     *             to be drawn.
     */
    virtual void drawBackground(QPainter *painter, const QRectF &region);

private Q_SLOTS:
    void onLoadFinished(bool ok);

private:
    void setup();

    QGraphicsWidget *m_central;
    QWebPage* m_background_page;
    QImage* m_background_image;

    const int m_board_item_offset;

    MDashboardItem *m_bottom_dashboard; /*!< The bottom player's dashboard item. >*/
    MDashboardItem *m_top_dashboard; /*!< The top player's dashboard item. >*/
    QTextEdit *m_chatbox; /*!< The player's chatbox (not useful for local games). >*/
};

}; // namespace Miniature
#endif
