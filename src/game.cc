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
#include "position.h"

#include <QList>

using namespace Miniature;

MGame::MGame(QObject *parent)
: QObject(parent),
  m_game(createDummyGame()),
  m_half_move(-1),
  m_board_view(0)
{}

void MGame::set_board_view(MBoardView* board_view)
{
    Q_CHECK_PTR(board_view);
    m_board_view = board_view;
}

MGame::~MGame()
{}

void MGame::newGame()
{
    Q_CHECK_PTR(m_board_view);

    m_board_view->drawStartPosition();
    m_half_move = -1;
}

void MGame::nextMove()
{
    if (0 < m_game.size() && m_half_move < static_cast<int>(m_game.size()) - 1)
    {
        ++m_half_move;
        MPosition pos = MPosition(m_game[m_half_move]);
        m_board_view->drawPosition(pos);
    }
    else // Complain!
    {
    }
}

void MGame::prevMove()
{
    if (m_half_move > 0)
    {
       --m_half_move;
        MPosition pos = MPosition(m_game[m_half_move]);
        m_board_view->drawPosition(pos);
    }
    else // Complain!
    {
        newGame();
    }
}

/*
QGraphicsScene* MGame::createScene()
{
    Q_CHECK_PTR(m_board);

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
*/

std::vector<QString> MGame::createDummyGame() const
{
    std::vector<QString> game;
    game.push_back(QString("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1"));
    game.push_back(QString("rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1"));
    game.push_back(QString("rnbqkbnr/pppp1ppp/8/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 0 1"));
    game.push_back(QString("r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 0 1"));
    return game;
}
