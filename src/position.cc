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

#include "position.h"

using namespace Miniature;

MPosition::MPosition()
: m_position(getDefaultStartPosition())
{}

MPosition::MPosition(QString fen)
: m_position(fen)
{}

MPosition::~MPosition()
{}

QString MPosition::getDefaultStartPosition() const
{
    return QString("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

QString MPosition::convertToFen() const
{
    return m_position;
}

MPosition::MPieceTypes MPosition::lookupPieceType(QChar fenPiece) const
{
    if ('r' == fenPiece) return BROOK;
    if ('n' == fenPiece) return BKNIGHT;
    if ('b' == fenPiece) return BBISHOP;
    if ('q' == fenPiece) return BQUEEN;
    if ('k' == fenPiece) return BKING;
    if ('p' == fenPiece) return BPAWN;

    if ('R' == fenPiece) return WROOK;
    if ('N' == fenPiece) return WKNIGHT;
    if ('B' == fenPiece) return WBISHOP;
    if ('Q' == fenPiece) return WQUEEN;
    if ('K' == fenPiece) return WKING;
    if ('P' == fenPiece) return WPAWN;

    // Complain?
    return UNKNOWN_PIECE;
}
