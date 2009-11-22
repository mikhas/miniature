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

#include "position.h"

using namespace Miniature;

MPosition::MPosition(int width, int height)
: m_width(width),
  m_height(height),
  m_position(width * height)
{}

MPosition::MPosition(QString /*fen*/, int width, int height)
: m_width(width),
  m_height(height),
  m_position(width * height)
{
}

MPosition::~MPosition()
{}

void MPosition::movePiece(QPoint from, QPoint to)
{
    // TODO: Castling & pawn promotion
    // TODO: use shared_ptr, risk of mleaks when removing pieces
    putPieceAt(pieceAt(from), to);
    putPieceAt(0, from);
}

MPiece* MPosition::pieceAt(QPoint pos) const
{
    return m_position[indexFromPoint(pos)];
}

MPosition::MPieces::const_iterator MPosition::begin() const
{
    return m_position.begin();
}

MPosition::MPieces::const_iterator MPosition::end() const
{
    return m_position.end();
}

int MPosition::indexFromPoint(QPoint point) const
{
    /* We assume that the top-left cell is (0,0) and that point.x() indicates
     * the column and point.y() indicates the row.
     */
    return (point.x() + point.y() * m_width);
}

QPoint MPosition::indexToPoint(int index, int scaling) const
{
    /* We assume that the top-left cell is (0,0) and that index % m_width
     * indicates the column and index / m_width indicates the row.
     */
    return QPoint((index % m_width) * scaling, (index / m_width) * scaling);
}

void MPosition::convertFromFen(QString fen)
{
    int x_pos = 0;
    int y_pos = 0;
    int count_cells = 0;

    for(int idx = 0; idx < fen.length() && count_cells < 64; ++idx)
    {
        QChar curr = fen.at(idx);
        if (curr == '/')
        {
            x_pos = 0;
            ++y_pos;
        }
        else if (curr.isDigit())
        {
            for (int j = 0; j < curr; ++j)
            {
                ++x_pos;
                MPiece *empty = pieceAt(QPoint(x_pos, y_pos));
                empty = 0;
                ++count_cells;
            }
        }
        else
        {
            MPiece *piece = pieceAt(QPoint(x_pos, y_pos));
            piece = MPiece::createFromFenPiece(curr); // TODO: include board dimension, too

            ++x_pos;
            ++count_cells;
        }
    }

    // TODO Set player-to-move, castle options, etc.
}

void MPosition::putPieceAt(MPiece* piece, QPoint pos)
{
    m_position[indexFromPoint(pos)] = piece;
}

// TODO: Remove me once the pieces pool manager is gone ...
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
