/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2009 Michael Hasselmann <michael@taschenorakel.de>
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

#include "logic_analyzer.h"
#include <iterator>

#include <QList>
#include <QPoint>

#include <iostream>

namespace
{

int max (int a, int b)
{
    return (b < a) ? a : b;
}

int min (int a, int b)
{
    return (a < b) ? a : b;
}

} // namespace

namespace Miniature
{


MLogicAnalyzer::MLogicAnalyzer(const MConstraintList &constraints, QObject *parent)
: QObject(parent),
  m_constraints(constraints)
{}

MLogicAnalyzer::~MLogicAnalyzer()
{}

bool MLogicAnalyzer::verifyMove(const MPosition &pos, QPoint from, QPoint to) const
{
    // DEBUG
    std::cout << "verify move ... " << std::endl;
    std::cout << "index of (" << from.x() << ", " << from.y() << "): " << pos.indexFromPoint(from) << std::endl;
    for (MPosition::MPieces::const_iterator iter = pos.begin();
         iter != pos.end();
         ++iter)
    {
        if (*iter) {std::cout << " r ";} else {std::cout << " - ";}
        if (7 == (std::distance(pos.begin(), iter) % 8)) {std::cout << std::endl;}
    }
    // TODO: add the rest of the validation. Currently only returns how a piece
    // moves on an empty board.
    MPiece* piece = pos.pieceAt(from);
    if (piece)
    {
        // DEBUG
        std::cout << "found valid piece ... " << std::endl;
        QList<QPoint> list = piece->getPossibleSquares(from);
        // apply rook constraint
        list = applyConStraight(pos, list, from);

        // DEBUG
        for (QList<QPoint>::const_iterator iter = list.begin(); iter != list.end(); ++iter)
        {
            std::cout << "(" << (*iter).x() << ", " << (*iter).y() << ") ";
        }
        std::cout << std::endl;

        return (list.contains(to));
    }

    return false;
}

QList<QPoint> MLogicAnalyzer::applyConStraight(const MPosition &pos, const QList<QPoint> &moveList, QPoint from) const
{
    int xMax = from.x();
    int xMin = from.x();
    int yMax = from.y();
    int yMin = from.y();

    MPiece *piece = pos.pieceAt(from);
    Q_CHECK_PTR(piece);

    QList<QPoint> newMoveList;
    MPiece::MColour currColour = piece->getColour();

    for(QList<QPoint>::const_iterator iter = moveList.begin();
        iter != moveList.end();
        ++iter)
    {
        QPoint cell = *iter;
        MPiece* currPiece = pos.pieceAt(cell);

        // vertical
        if (cell.y() == from.y())
        {
            if (!currPiece) // cell empty
            {
                continue;
            }
            else
            {
                if (currPiece->getColour() == currColour)
                {
                    if (cell.x() > from.x()) {xMax = max(xMax, cell.x() - 1);}
                    else {xMin = min(xMin, cell.x() + 1);}
                }
                else
                {
                    if (cell.x() > from.x()) {xMax = max(xMax, cell.x());}
                    else {xMin = min(xMin, cell.x());}
                }
            }
        }
        // horizontal
        else if (cell.x() == from.x())
        {
            if (!currPiece)
            {
                continue;
            }
            else
            {
                if (currPiece->getColour() == currColour)
                {
                    if (cell.y() > from.y()) {yMax = max(yMax, cell.y() - 1);}
                    else {yMin = min(yMin, cell.y() + 1);}
                }
                else
                {
                    if (cell.y() > from.y()) {yMax = max(yMax, cell.y());}
                    else {yMin = min(yMin, cell.y());}
                }
            }
        }
    }

    for(QList<QPoint>::const_iterator iter = moveList.begin();
        iter != moveList.end();
        ++iter)
    {
        QPoint cell = *iter;

        // vertical
        if (cell.y() == from.y())
        {
            if (cell.x() <= xMax && cell.x() >= xMin)
            {
                std::cout << "A) appending cell(x,y): " << cell.x() << ", " << cell.y() << std::endl;
                newMoveList.append(cell);
            }
        }
        // horizontal
        else if (cell.x() == from.x())
        {
            if (cell.y() <= yMax && cell.y() >= yMin)
            {
                std::cout << "B) appending cell(x,y): " << cell.x() << ", " << cell.y() << std::endl;
                newMoveList.append(cell);
            }
        }
        else
        {
            newMoveList.append(cell);
        }
    }

    return newMoveList;
}

}
