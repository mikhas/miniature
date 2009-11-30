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

// MStorage impl
MStorage::MStorage(int index, MPiece *piece)
: m_index(index),
  m_piece(piece)
{}

MStorage::~MStorage()
{
    if(m_piece)
    {
        delete m_piece;
        m_piece = 0;
    }
}

bool MStorage::empty() const
{
    return (0 == m_piece);
}

// MPosition impl
MPosition::MPosition(int width, int height)
: m_width(width),
  m_height(height),
  m_colour_to_move(MPiece::WHITE),
  m_position(width * height, 0)
{}

MPosition::MPosition(QString /*fen*/, int width, int height)
: m_width(width),
  m_height(height),
  m_colour_to_move(MPiece::WHITE),
  m_position(width * height, 0)
{}

// TODO: clean up pieces => mem leak
MPosition::~MPosition()
{}

QString MPosition::movePiece(QPoint from, QPoint to)
{
    // since the storage takes ownership it will delete the stored piece at the
    // end of scope, if need be.
    MStorage storage = store(to);

    // TODO: Castling & pawn promotion
    addPieceAt(pieceAt(from), to);
    removePieceAt(from);

    return QString("e4");
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

MStorage MPosition::store(const QPoint &location)
{
    const int index = indexFromPoint(location);

    MStorage storage = MStorage(index, m_position[index]);
    m_position[index] = 0;

    return storage;
}

void MPosition::restore(const MStorage &storage)
{
    // delete whatever is at the storage's location, to prevent mem leakage
    if (m_position[storage.m_index])
    {
        delete m_position[storage.m_index];
        m_position[storage.m_index] = 0;
    }

    m_position[storage.m_index] = storage.m_piece;
}

void MPosition::addPieceAt(MPiece* piece, QPoint pos)
{
    m_position[indexFromPoint(pos)] = piece;

    if (piece && piece->getType() == MPiece::KING)
    {
        if (piece->getColour() == MPiece::WHITE)
        {
            m_white_king = pos;
        }
        else
        {
            m_white_king = pos;
        }
    }
}

void MPosition::removePieceAt(QPoint pos)
{
    addPieceAt(0, pos);
}

void MPosition::reset()
{
    m_position = MPieces(m_position.size(), 0);
    m_colour_to_move = MPiece::WHITE;
}

QPoint MPosition::getKing(MPiece::MColour colour) const
{
    if (colour == MPiece::WHITE)
    {
        return m_white_king;
    }
    else
    {
        return m_black_king;
    }
}

MPiece::MColour MPosition::getColourToMove() const
{
    return m_colour_to_move;
}

void MPosition::nextColour()
{
    m_colour_to_move = (m_colour_to_move == MPiece::WHITE ? MPiece::BLACK
                                                          : MPiece::WHITE);
}
