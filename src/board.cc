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

#include "board.h"

#include <QGraphicsSvgItem>

MiniatureBoard::MiniatureBoard(QGraphicsItem *parent)
: QGraphicsPixmapItem(parent)
{}

MiniatureBoard::MiniatureBoard(const QPixmap &pixmap, QGraphicsItem *parent)
: QGraphicsPixmapItem(pixmap, parent)
{}

MiniatureBoard::~MiniatureBoard()
{}

void MiniatureBoard::clear()
{
    QList<QGraphicsItem*> children = childItems();
    for(QList<QGraphicsItem*>::iterator iter = children.begin();
        iter != children.end();
        ++iter)
    {
        Q_CHECK_PTR(*iter); // This might be paranoid ...
        delete (*iter);
    }
}

void MiniatureBoard::drawPosition(QString fen)
{
    if (fen.isEmpty())
    {
        // We have to decide whether we want to treat this as error. Currently,
        // it's not (calling this method with an empty string).
        return;
    }

    clear();

    // TODO: Derive cell_size from "board size * .125"
    const int cell_size = 60;

    /* (x_pos, y_pos) always tells us where to draw the current figure (top
     * left corner of a cell).
     */
    int x_pos = 0;
    int y_pos = 0;

    /* We count cells so we know when to stop reading the FEN string. A FEN
     * string can contain comments and game state information which we do not need.
     * That is why we stop after we have seen 64 cells.
     */
    int count_cells = 0;

    for(int idx = 0; idx < fen.length() && 64 > count_cells; ++idx)
    {
        QChar curr = fen.at(idx);
        QString file_name;

        // scanned one row
        if ('/' == curr)
        {
            y_pos += cell_size;
            x_pos = 0;
        }
        else if (curr.isDigit())
        {
            /* This is the nice part about FEN: series of empty squares use a
             * run-length-encoding. And that's stuff from the 19th century!
             */
            x_pos += curr.digitValue() * cell_size;
            count_cells += curr.digitValue();
        }
        else
        {
            // fetch the SVG file and add it to the board
            file_name = getFileNameForFigure(curr);
            if (!file_name.isEmpty())
            {
                QGraphicsSvgItem *figure = new QGraphicsSvgItem(file_name, this);

                // Make the SVG figure 90% of the size of a cell.
                QRectF extent = figure->boundingRect();
                qreal ratio = 1;
                if (0 < extent.width())
                {
                    ratio = cell_size / extent.width();
                }
                figure->scale(ratio * .9, ratio * .9);

                // We have a 10% margin, from the scaling above. Now we center the
                // figure.
                figure->setPos(QPointF(x_pos + (.05 * cell_size), y_pos + (.05 * cell_size)));
                x_pos += cell_size;
                ++count_cells;
            }
        }

    }
}

QString MiniatureBoard::getFileNameForFigure(QChar fenFigure) const
{
    QString file_name;

    // check all black figures
    if ('r' == fenFigure)
    {
        file_name = QString(":/figures/black/rook.svg");
    }
    else if ('n' == fenFigure)
    {
        file_name = QString(":/figures/black/knight.svg");
    }
    else if ('b' == fenFigure)
    {
        file_name = QString(":/figures/black/bishop.svg");
    }
    else if ('q' == fenFigure)
    {
        file_name = QString(":/figures/black/queen.svg");
    }
    else if ('k' == fenFigure)
    {
        file_name = QString(":/figures/black/king.svg");
    }
    else if ('p' == fenFigure)
    {
        file_name = QString(":/figures/black/pawn.svg");
    }
    // check all white figures
    else if ('R' == fenFigure)
    {
        file_name = QString(":/figures/white/rook.svg");
    }
    else if ('N' == fenFigure)
    {
        file_name = QString(":/figures/white/knight.svg");
    }
    else if ('B' == fenFigure)
    {
        file_name = QString(":/figures/white/bishop.svg");
    }
    else if ('Q' == fenFigure)
    {
        file_name = QString(":/figures/white/queen.svg");
    }
    else if ('K' == fenFigure)
    {
        file_name = QString(":/figures/white/king.svg");
    }
    else if ('P' == fenFigure)
    {
        file_name = QString(":/figures/white/pawn.svg");
    }

    return file_name;
}
