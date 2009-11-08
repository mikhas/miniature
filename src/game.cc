/* Copyright 2009 Michael Hasselmann <michael@taschenorakel.de>
 * Part of the Miniature project.
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

#include "game.h"

#include <QList>

MiniatureGame::MiniatureGame(QObject *parent)
: QObject(parent),
  m_game(createDummyGame()),
  m_view(0),
  m_board(0)
{
    m_board = new MiniatureBoard(QPixmap(":boards/default.png"));
}

MiniatureGame::~MiniatureGame()
{}

void MiniatureGame::setSceneView(QGraphicsView* view)
{
    m_view = view;
}

void MiniatureGame::newGame()
{
    Q_CHECK_PTR(m_view);
    Q_CHECK_PTR(m_board);

    QGraphicsScene *scene = 0;
    m_view->setScene(scene = createScene());
    // Adding a margin of 10 to the scene graph helps to remove the scrollbars from the view.
    m_view->resize(scene->width() + 10, scene->height() + 10);
    m_board->drawPosition(getDefaultStartPosition());
}

void MiniatureGame::nextMove()
{
/*
    // Keep a temporary pointer to the scene
    QGraphicsScene *temp_scene = m_board->scene();

    // Although this looks fatal, the doc says it's OK: removeItem only deletes
    // the children of m_board, not m_board itself.
    temp_scene->removeItem(m_board);
    Q_CHECK_PTR(m_board); // But who knows when the impl changes again ;)
    addBoardToSceneGraph(temp_scene);
*/
    m_board->drawPosition(convertToFen(++m_half_move));
}

void MiniatureGame::prevMove()
{
    // Although this looks fatal, the doc says it's OK: removeItem only deletes
    // the children of m_board, not m_board itself.
/*
    m_board->scene()->removeItem(m_board);
    Q_CHECK_PTR(m_board); // But who knows when the impl changes again ;)
    addBoardToSceneGraph(m_board->scene());
*/
    if (m_half_move > 0)
    {
        m_board->drawPosition(convertToFen(--m_half_move));
    }
    else // Complain!
    {
    }
}

QString MiniatureGame::getDefaultStartPosition() const
{
    return QString("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

QString MiniatureGame::convertToFen(unsigned int half_move) const
{
    if (half_move < m_game.size())
    {
        return m_game[half_move];
    }

    return QString();
}

// TODO: Find out how to properly render left/right aligned multi-line text without using this HTML hack.
QString MiniatureGame::formatPlayerInfo(QString name, int rating, int turn, QString alignment) const
{
    QString style = QString("font-size:12px; color:white;");
    // setHtml + CSS is nice for text formatting. However, the CSS support is
    // limited. It does not support "text-align", so a td tag is used instead.
    return QString("<td width='160' style='%1' align='%2'>%3<br />(%4)<br />%5</td>").arg(style)
                                                                                     .arg(alignment)
                                                                                     .arg(name)
                                                                                     .arg(rating)
                                                                                     .arg(turn);
}

// TODO: Find out how to properly render left/right aligned multi-line text without using this HTML hack.
QString MiniatureGame::formatTimerInfo(QString time_remaining, bool isWhite) const
{
    QString style = QString("font-size:12px; color:%1").arg(isWhite ? "white" : "black");
    return QString("<td width='60' style='%1' align='center'>%2</td>").arg(style)
                                                                      .arg(time_remaining);
}

void MiniatureGame::addBoardToSceneGraph(QGraphicsScene *scene)
{
    Q_CHECK_PTR(scene);

    scene->addItem(dynamic_cast<QGraphicsPixmapItem*>(m_board));
    m_board->setPos(QPointF(0,65));
    m_board->setZValue(0);
}

QGraphicsScene* MiniatureGame::createScene()
{
    Q_CHECK_PTR(m_board);

    /* This scene graph creation features a lot of magic values, sadly. Maybe
     * that's what you get with pixel-based layouts.
     */

    QGraphicsScene *scene = new QGraphicsScene;
    scene->setBackgroundBrush(Qt::black);

    // board
    addBoardToSceneGraph(scene);

    // white player card
    QGraphicsTextItem *white = new QGraphicsTextItem;
    white->setHtml(formatPlayerInfo("andybehr", 9999, 28, "left"));
    white->setPos(QPointF(10,0));
    white->setZValue(1);
    scene->addItem(white);

    // timer for white player card
    QGraphicsPixmapItem *timer_bg = scene->addPixmap(QPixmap(":timer_background.gif"));
    timer_bg->setPos(QPointF(95,30));
    timer_bg->setZValue(1);

    QGraphicsTextItem *timer_white = new QGraphicsTextItem;
    timer_white->setHtml(formatTimerInfo("00:17", false));
    //timer_white->setPlainText("00:17");
    timer_white->setDefaultTextColor(Qt::black);
    timer_white->setPos(QPointF(95,32));
    timer_white->setZValue(2);
    scene->addItem(timer_white);

    // black player card
    QGraphicsTextItem *timer_black = new QGraphicsTextItem;
    timer_black->setHtml(formatTimerInfo("00:32", true));
    //timer_black->setPlainText("00:32");
    timer_white->setDefaultTextColor(Qt::white);
    timer_black->setPos(QPointF(185,32));
    timer_black->setZValue(2);
    scene->addItem(timer_black);

    // timer for black player card
    QGraphicsTextItem *black = new QGraphicsTextItem;
    black->setHtml(formatPlayerInfo("mikhas", 9999, 28, "right"));
    black->setPos(QPointF(180,0));
    black->setZValue(1);
    scene->addItem(black);

    return scene;
}

std::vector<QString> MiniatureGame::createDummyGame() const
{
    std::vector<QString> game;
    game.push_back(QString("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
    game.push_back(QString("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1"));
    game.push_back(QString("rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1"));
    game.push_back(QString("rnbqkbnr/pppp1ppp/8/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 0 1"));
    game.push_back(QString("r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 0 1"));
    return game;
}
