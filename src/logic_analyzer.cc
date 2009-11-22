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

#include "logic_analyzer.h"
#include <iterator>

#include <QList>
#include <QPoint>

#include <iostream>

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
        return (list.contains(to));
    }

    return false;
}

}
