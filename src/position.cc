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
MStorage::MStorage(int index, MSharedPiece piece)
: m_index(index),
  m_piece(piece)
{}

MStorage::~MStorage()
{}

bool MStorage::empty() const
{
    return m_piece.isNull();
}

// MPosition impl
MPosition::MPosition(int width, int height)
: m_width(width),
  m_height(height),
  m_colour_to_move(MPiece::WHITE),
  m_position(width * height)
{}

MPosition::MPosition(QString /*fen*/, int width, int height)
: m_width(width),
  m_height(height),
  m_colour_to_move(MPiece::WHITE),
  m_position(width * height)
{}

MPosition::~MPosition()
{
    reset();
}

void MPosition::movePiece(QPoint from, QPoint to)
{
    // TODO: Castling & pawn promotion

    // since the storage takes ownership it will delete the stored piece at the
    // end of scope, if need be.
    MStorage from_storage = store(from);
    MStorage to_storage = store(to);

    QChar letter = QChar(from_storage.empty() ? ' ' : from_storage.m_piece->getLetter());

    from_storage.m_index = indexFromPoint(to);
    restore(&from_storage);

    // TODO: fix ambigious moves, e.g., when two pieces of the same kind can move to a location.
    m_move_notation = QString("%1%2%3").arg(letter)
                                       .arg(to_storage.empty() ? "" : "x")
                                       .arg(convertToChessCell(to));
}

MSharedPiece MPosition::pieceAt(QPoint pos) const
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
                pieceAt(QPoint(x_pos, y_pos)).clear();
                ++count_cells;
            }
        }
        else
        {
            MSharedPiece piece = pieceAt(QPoint(x_pos, y_pos));
            piece = MSharedPiece(MPiece::createFromFenPiece(curr)); // TODO: include board dimension, too

            ++x_pos;
            ++count_cells;
        }
    }

    // TODO Set player-to-move, castle options, etc.
}

QString MPosition::convertToChessCell(QPoint location) const
{
    const int small_letter_a = 97;
    return QString("%1%2").arg(static_cast<char>(small_letter_a + location.x()))
                          .arg(8 - location.y());
}

MStorage MPosition::store(const QPoint &location)
{
    const int index = indexFromPoint(location);

    MStorage storage = MStorage(index, m_position[index]);
    m_position[index].clear(); // drop ref

    return storage;
}

void MPosition::restore(MStorage* const storage)
{
    // delete whatever is at the storage's location, to prevent mem leakage
    MStorage removed = store(indexToPoint(storage->m_index)); // TODO: optimise back&forth conversion?

    m_position[storage->m_index] = storage->m_piece;

    // drop the shared ref
    storage->m_piece.clear();
}

void MPosition::addPieceAt(MPiece* piece, QPoint pos)
{
    // Prevents potential memory leak by adding pieces on top of each other.
    //MStorage removed = store(pos);
    m_position[indexFromPoint(pos)] = MSharedPiece(piece);

    if (piece && piece->getType() == MPiece::KING)
    {
        if (piece->getColour() == MPiece::WHITE)
        {
            m_white_king = pos;
        }
        else
        {
            m_black_king = pos;
        }
    }
}

void MPosition::reset()
{
    // TODO: check whether this leaks
    m_position = MPieces(m_position.size());
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

void MPosition::resetCastling()
{
    m_white_qs_castle = true;
    m_white_ks_castle = true;
    m_black_qs_castle = true;
    m_black_ks_castle = true;
}

void MPosition::kingMoved(MPiece::MColour colour)
{
	if (colour == MPiece::WHITE)
	{
		m_white_ks_castle = false;
		m_white_qs_castle = false;
	}
	else if (colour == MPiece::BLACK)
	{
		m_black_qs_castle = false;
		m_black_ks_castle = false;
	}
}

void MPosition::rookMoved(MPiece::MColour colour, QPoint location)
{
	int index = indexFromPoint(location);
	if (colour == MPiece::WHITE)
	{
		if (index == 56)
		{
			m_white_qs_castle = false;
		}
		else if (index == 63)
		{
			m_white_ks_castle = false;
		}
	}
	else if (colour == MPiece::BLACK)
	{
		if (index == 0)
		{
			m_black_qs_castle = false;
		}
		else if (index == 7)
		{
			m_black_ks_castle = false;
		}
	}
}

bool MPosition::canWhiteCastleQueenside() const
{
    return m_white_qs_castle;
}

bool MPosition::canWhiteCastleKingside() const
{
    return m_white_ks_castle;
}

bool MPosition::canBlackCastleQueenside() const
{
    return m_black_qs_castle;
}

bool MPosition::canBlackCastleKingside() const
{
    return m_black_ks_castle;
}
