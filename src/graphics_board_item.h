/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2009 Michael Hasselmann <michael@taschenorakel.de>
 * Copyright (C) 2009 Mathias Hasselmann <mathias.hasselmann@maemo.org>
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

#ifndef GRAPHICS_BOARD_ITEM_H__
#define GRAPHICS_BOARD_ITEM_H__

#include <QGraphicsItem>
#include <QGraphicsSvgItem>
#include <QGraphicsRectItem>
#include <QTimeLine>
#include <QGraphicsSceneMouseEvent>
#include <QWebPage>
#include <QWebFrame>
#include <QUrl>
#include <QVector>

#include <QTime>

namespace Miniature
{

/* This class represents the board. It's our primary item in the scene graph,
 * as it not only renders the board but also controls the mouse event handling
 * (= fingertouch handling on Maemo) for all pieces.  Due to the poor rendering
 * qualities of QtSvg we went for a QtWebkit hack, that is, we use a QWebFrame
 * to display the SVG board.
 */
class MGraphicsBoardItem
: public QGraphicsObject
{
    Q_OBJECT

public:
    MGraphicsBoardItem(QGraphicsItem *parent = 0);
    MGraphicsBoardItem(const QString &fileName, QGraphicsItem *parent = 0);
    ~MGraphicsBoardItem();

    // TODO: rename to cellSize(), more qt-ish
    int getCellSize() const;
    QWebFrame* frame() const { return m_page.mainFrame(); }

    void loadFromUri(QUrl uri);
    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

    void addPiece(QGraphicsSvgItem *piece);
    void removePieces();

Q_SIGNALS:
    void pieceMoveRequested(QPoint from, QPoint to);
    void loadFinished(bool ok);
    void sendDebugInfo(QString msg);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    void setupFrameAndTimeLine();
    void resetFrame();
    void putFrameAt(QPointF pos);

    // This class uses a QWebPage to render our SVG board. It's a hack, yep.
    QWebPage m_page;
    int m_selection_duration;
    QGraphicsRectItem *m_frame;
    int m_frame_outline;
    QTimeLine *m_time_line;

    // We need a safe way to remember the pieces on the board, so that we can
    // clear the board correctly.
    typedef QVector<QGraphicsSvgItem*> MPiecesList;
    MPiecesList m_pieces;

private Q_SLOTS:
    /* Reduces opacity of the selection frame. */
    void fadeOutFrame(int step);

    /* Resizes viewport of the internal QWebPage, after loading the SVG board has finished. */
    void onPageLoaded(bool ok);

};

};

#endif
