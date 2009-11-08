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

#include "config.h"
#include "miniature.h"
#include "board.h"

#include <QGraphicsTextItem>
#include <QPixmap>
#include <QApplication>

MiniatureWindow::MiniatureWindow()
: QMainWindow()
{
    setWindowTitle(tr("Miniature - The killer chess app for Maemo"));

// Setting portrait mode only works with git version of Qt4
//    setAttribute(Qt::WA_Maemo5ForcePortraitOrientation, true);
//    setAttribute(Qt::WA_Maemo5ForceLandscapeOrientation, false);

}

MiniatureWindow::~MiniatureWindow()
{}

// TODO: Find out how to properly render left/right aligned multi-line text without using this HTML hack.
QString MiniatureWindow::formatPlayerInfo(QString name, int rating, int turn, QString alignment) const
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
QString MiniatureWindow::formatTimerInfo(QString time_remaining, bool isWhite) const
{
    QString style = QString("font-size:12px; color:%1").arg(isWhite ? "white" : "black");
    return QString("<td width='60' style='%1' align='center'>%2</td>").arg(style)
                                                                      .arg(time_remaining);
}

QGraphicsScene* MiniatureWindow::createScene() const
{
    /* This scene graph creation features a lot of magic values, sadly. Maybe
     * that's what you get with pixel-based layouts.
     */

    QGraphicsScene *scene = new QGraphicsScene;
    scene->setBackgroundBrush(Qt::black);

    // board
    MiniatureBoard *board = new MiniatureBoard(QPixmap(":boards/default.png"));
    scene->addItem(dynamic_cast<QGraphicsPixmapItem*>(board));
    board->setPos(QPointF(0,65));
    board->setZValue(0);
    board->drawPosition(QString("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));

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

void MiniatureWindow::show()
{
    QGraphicsView *view = new QGraphicsView(this);
    QSizePolicy policy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    view->setSizePolicy(policy);
    QGraphicsScene *scene = 0;
    view->setScene(scene = createScene());
    // Adding a margin of 10 to the scene graph helps to remove the scrollbars from the view.
    view->resize(scene->width() + 10, scene->height() + 10);

    QMainWindow::show();
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    MiniatureWindow main;
    main.show();

    return app.exec();
}
