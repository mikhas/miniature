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

#include "position.h"
#include "graphics_board_item.h"

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
/* This class represents a board, using QGraphicsItems. Each figure is added as
 * a child to the board itself.
 * The state of this class should not be extended if possible.
 */
class MBoardView
: public QGraphicsView
{
    Q_OBJECT

public:
    explicit MBoardView(QWidget *parent = 0);
    virtual ~MBoardView();

    virtual void setScene(QGraphicsScene* scene);
    // Effect becomes visible when the next position is drawn.
    void rotateBlackPieces();
    // Effect becomes visible when the next position is drawn.
    void rotateWhitePieces();

    /* Draws a chess position on this board. */
    void drawPosition(const MPosition &position);
    void drawStartPosition();
    void resetCache();

    /* Takes ownership of the proxy widget. */
    void setTopActionArea(QGraphicsProxyWidget *proxy_widget);
    /* Takes ownership of the proxy widget. */
    void setBottomActionArea(QGraphicsProxyWidget *proxy_widget);
    /* Fwd method to MBoardView */
    void resetPieceSelection();

Q_SIGNALS:
    void pieceSelectionRequested(QPoint cell);
    void pieceMoveRequested(QPoint from, QPoint to);
    void sendDebugInfo(QString msg);

protected:
    virtual void drawBackground(QPainter *painter, const QRectF &region);

private:
    void setupBoardBackground();

    /* Store a reference to the board item in the scene graph. */
    MGraphicsBoardItem* m_board_item;

    // A MPiece*-to-QGraphicsSvgItem* mapping, which allows us to cleanly cache
    // SVG items and to only insert them in the MGraphicsBoardItem once. Done
    // for mouse event speed optimisation.
    typedef QHash<MPiece*, QGraphicsSvgItem*> MSvgItemCache;
    MSvgItemCache m_cache;

    QWebPage* m_background_page;
    QImage* m_background_image;

    bool m_white_rotated180;
    bool m_black_rotated180;

    // Indicates where to position the board item, and where to draw the board
    // background.
    const int m_board_item_offset;

private Q_SLOTS:
    void onPieceSelectionRequested(QPoint cell);
    void onPieceMoveRequested(QPoint from, QPoint to);
    void appendDebugOutput(QString msg);
    void onLoadFinished(bool ok);
};

}; // namespace Miniature
#endif
