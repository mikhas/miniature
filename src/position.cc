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

// mStorage impl
mStorage::mStorage(int idx, MPiece *ptr)
: index(idx),
  piece(ptr)
{}

mStorage::~mStorage()
{
    // No need to delete. MPieces are owned by the MGraphicsBoard instances.
    piece = 0;
}

bool mStorage::empty() const
{
    return (0 == piece);
}

// MPosition impl
MPosition::MPosition(int width, int height, int /*cell_size*/)
: m_width(width),
  m_height(height),
  m_white_king(-1, -1),
  m_black_king(-1, -1),
  m_white_qs_castle(true),
  m_white_ks_castle(true),
  m_black_qs_castle(true),
  m_black_ks_castle(true),
  m_in_check(false),
  m_colour_to_move(MPiece::WHITE), // TODO: do we need a "none" flag?
  m_position(width * height)
{}

MPosition::~MPosition()
{
    // TODO: is this necessary?
    reset();
}

void MPosition::movePiece(const QPoint &origin, const QPoint &target)
{
    mStorage origin_storage = store(origin);

    QChar letter = QChar(origin_storage.empty() ? ' ' : origin_storage.piece->getLetter());

    // update position in QGraphicsView
    origin_storage.piece->moveTo(QPoint(target.x() * 60, target.y() * 60)); // TODO: replace magic numbers!

    // move piece by updating the index to the index of origin, then restore it to target
    origin_storage.index = indexFromPoint(target);
    restore(&origin_storage);

    // update king/rook state for castling
    if (!origin_storage.empty())
    {
        MPiece::MType type = origin_storage.piece->getType();
        MPiece::MColour colour = origin_storage.piece->getColour();

        if (type == MPiece::KING)
        {
            kingMoved(colour);
        }
        else if (type == MPiece::ROOK)
        {
            rookMoved(colour, origin);
        }
    }
}

MPiece * MPosition::pieceAt(const QPoint &cell) const
{
    return m_position[indexFromPoint(cell)];
}

MPosition::MPieces::const_iterator MPosition::begin() const
{
    return m_position.begin();
}

MPosition::MPieces::const_iterator MPosition::end() const
{
    return m_position.end();
}

int MPosition::indexFromPoint(const QPoint &cell) const
{
    /* We assume that the top-left cell is (0,0) and that point.x() indicates
     * the column and point.y() indicates the row.
     */
    return (cell.x() + cell.y() * m_width);
}

QPoint MPosition::indexToPoint(int index, int scaling) const
{
    /* We assume that the top-left cell is (0,0) and that index % m_width
     * indicates the column and index / m_width indicates the row.
     */
    return QPoint((index % m_width) * scaling, (index / m_width) * scaling);
}

QString MPosition::mapToString(const QPoint &cell) const
{
    const int small_letter_a = 97;
    return QString("%1%2").arg(static_cast<char>(small_letter_a + cell.x()))
                          .arg(8 - cell.y());
}

mStorage MPosition::store(const QPoint &cell)
{
    const int index = indexFromPoint(cell);

    mStorage storage = mStorage(index, m_position[index]);

    if (!storage.empty())
    {
        storage.piece->hide();
    }

    m_position[index] = 0; // remove from position
    return storage;
}

void MPosition::restore(mStorage* const storage)
{
    // delete whatever is at the storage's location
    removePieceAt(indexToPoint(storage->index));

    if (!storage->empty())
    {
        storage->piece->show();
        if (storage->piece->getType() == MPiece::KING)
        {
            updateKingPosition(storage->piece->getColour(), indexToPoint(storage->index));
        }
    }
    m_position[storage->index] = storage->piece;
}

void MPosition::addPieceAt(MPiece* piece, const QPoint &target)
{
    Q_CHECK_PTR(piece);

    removePieceAt(target);
    piece->moveTo(QPoint(target.x() * 60, target.y() * 60)); // TODO: remove magic numbers!
    m_position[indexFromPoint(target)] = piece;

    if (piece->getType() == MPiece::KING)
    {
        if (piece->getColour() == MPiece::WHITE)
        {
            m_white_king = target;
        }
        else
        {
            m_black_king = target;
        }
    }
}

void MPosition::removePieceAt(const QPoint &target)
{
    mStorage removed = store(target);
    if(!removed.empty())
    {
        removed.piece->hide();
    }
}

void MPosition::reset()
{
    // TODO: check whether this leaks
    m_position = MPieces(m_position.size());
    m_colour_to_move = MPiece::WHITE;
    m_in_check = true;
    resetCastling();
}

void MPosition::updatePieces()
{
    for(MPieces::iterator iter = m_position.begin();
        iter != m_position.end();
        ++iter)
    {
        MPiece *piece = *iter;
        if (piece)
        {
            piece->moveTo(piece->mapFromCell(indexToPoint(std::distance(m_position.begin(), iter))));
            piece->show();
        }
    }
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

void MPosition::updateKingPosition(MPiece::MColour colour, const QPoint &target)
{
    if (colour == MPiece::WHITE)
    {
        m_white_king = target;
    }
    else
    {
        m_black_king = target;
    }
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

void MPosition::rookMoved(MPiece::MColour colour, const QPoint &target)
{
    int index = indexFromPoint(target);
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

bool MPosition::inCheck() const
{
    return m_in_check;
}

void MPosition::setInCheck(bool check)
{
    m_in_check = check;
}
