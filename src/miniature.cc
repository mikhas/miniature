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
#include <iostream>

#include "config.h"
#include "miniature.h"
#include <QGraphicsSvgItem>
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

QString MiniatureWindow::formatTimerInfo(QString time_remaining, bool isWhite) const
{
    QString style = QString("font-size:12px; color:%1").arg(isWhite ? "white" : "black");
    return QString("<td width='60' style='%1' align='center'>%2</td>").arg(style)
                                                                      .arg(time_remaining);
}

void MiniatureWindow::drawChessPosition(QGraphicsPixmapItem* board, QString fen) const
{
    const int cell_size = 45;
    int x_pos = 0;
    int y_pos = 0;
    int count_cells = 0;

    for(int idx = 0; idx < fen.length() && 64 > count_cells; ++idx)
    {
        QChar curr = fen.at(idx);
        QString file_name;

        if ('/' == curr)
        {
            y_pos += cell_size;
            x_pos = 0;
        }
        else if (curr.isDigit())
        {
            x_pos += curr.digitValue() * cell_size;
            count_cells += curr.digitValue();
        }
        // check all black figures
        else if ('r' == curr)
        {
            file_name = QString(":/figures/black/rook.svg");
        }
        else if ('n' == curr)
        {
            file_name = QString(":/figures/black/knight.svg");
        }
        else if ('b' == curr)
        {
            file_name = QString(":/figures/black/bishop.svg");
        }
        else if ('q' == curr)
        {
            file_name = QString(":/figures/black/queen.svg");
        }
        else if ('k' == curr)
        {
            file_name = QString(":/figures/black/king.svg");
        }
        else if ('p' == curr)
        {
            file_name = QString(":/figures/black/pawn.svg");
        }
        // check all white figures
        else if ('R' == curr)
        {
            file_name = QString(":/figures/white/rook.svg");
        }
        else if ('N' == curr)
        {
            file_name = QString(":/figures/white/knight.svg");
        }
        else if ('B' == curr)
        {
            file_name = QString(":/figures/white/bishop.svg");
        }
        else if ('Q' == curr)
        {
            file_name = QString(":/figures/white/queen.svg");
        }
        else if ('K' == curr)
        {
            file_name = QString(":/figures/white/king.svg");
        }
        else if ('P' == curr)
        {
            file_name = QString(":/figures/white/pawn.svg");
        }

        if (!file_name.isEmpty())
        {
            std::cout << "x_pos: " << x_pos << ", y_pos: " << y_pos << std::endl;
            QGraphicsSvgItem *figure = new QGraphicsSvgItem(file_name, board);
            figure->setPos(QPointF(x_pos, y_pos - 4));
            x_pos += cell_size;
            ++count_cells;
        }

    }
}

QGraphicsScene* MiniatureWindow::createScene() const
{
    QGraphicsScene *scene = new QGraphicsScene;
    scene->setBackgroundBrush(Qt::black);

    QGraphicsPixmapItem *board = scene->addPixmap(QPixmap(":boards/default.png"));
    board->setPos(QPointF(0,65));
    board->setZValue(0);
    drawChessPosition(board, QString("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));

    QGraphicsTextItem *white = new QGraphicsTextItem;
    white->setHtml(formatPlayerInfo("andybehr", 9999, 28, "left"));
    white->setPos(QPointF(10,0));
    white->setZValue(1);
    scene->addItem(white);

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

    QGraphicsTextItem *timer_black = new QGraphicsTextItem;
    timer_black->setHtml(formatTimerInfo("00:32", true));
    //timer_black->setPlainText("00:32");
    timer_white->setDefaultTextColor(Qt::white);
    timer_black->setPos(QPointF(185,32));
    timer_black->setZValue(2);
    scene->addItem(timer_black);

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
