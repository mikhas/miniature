/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2009 Dennis Stötzel <kore@meeQ.de>
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

#include "rook.h"
#include <iostream>

namespace Miniature
{

bool MRook::hasFinishedLoading = false;
QSvgRenderer MRook::blackRenderer;
QSvgRenderer MRook::whiteRenderer;

MRook::MRook(MColour colour, int width, int height)
: MPiece(colour, ROOK, width, height),
  castle(true)
{
}

MRook::~MRook()
{}

QList<QPoint> MRook::getPossibleSquares(QPoint point) const
{
    QList<QPoint> possibleSquares;

    // north
    int i = 1;
    while (point.y() + i < yDim)
    {
        possibleSquares.append(QPoint(point.x(), point.y() + i));
        ++i;
    }

    // east
    i = 1;
    while (point.x() + i < xDim)
    {
        possibleSquares.append(QPoint(point.x() + i, point.y()));
        ++i;
    }

    // south
    i = 1;
    while (point.y() - i >= 0)
    {
        possibleSquares.append(QPoint(point.x(), point.y() - i));
        ++i;
    }

    // west
    i = 1;
    while (point.x() - i >= 0)
    {
        possibleSquares.append(QPoint(point.x() - i, point.y()));
        ++i;
    }

    for (QList<QPoint>::iterator iter = possibleSquares.begin();
         iter != possibleSquares.end();
         ++iter)
    {
        std::cout << "    (" << (*iter).x() << ", " << (*iter).y() << "), " << std::endl;
    }

    return possibleSquares;
}

void MRook::hasMoved()
{
    castle = false;
}

bool MRook::canCastle() const
{
    return castle;
}

// TODO: hand out cloned pixmap items instead, saves scaling and maybe more
QGraphicsSvgItem* MRook::createSvgItem(int pieceSize) const
{
    QGraphicsSvgItem* svgItem = new QGraphicsSvgItem;

    if (!MRook::hasFinishedLoading)
    {
        MRook::blackRenderer.load(QString(":pieces/black/rook.svg"));
        MRook::whiteRenderer.load(QString(":pieces/white/rook.svg"));
        MRook::hasFinishedLoading = true;
    }

    applyRenderer(svgItem, (MRook::BLACK == getColour() ? MRook::blackRenderer
                                                        : MRook::whiteRenderer), pieceSize);

    return svgItem;
}

}
