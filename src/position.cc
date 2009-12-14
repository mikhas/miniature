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
{
    // drops refcount of m_piece, too
}

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

MPosition::~MPosition()
{
    reset();
}

void MPosition::movePiece(QPoint from, QPoint to)
{
    MStorage from_storage = store(from);

    QChar letter = QChar(from_storage.empty() ? ' ' : from_storage.m_piece->getLetter());

    // update position in QGraphicsView
    from_storage.m_piece->moveTo(QPoint(to.x() * 60, to.y() * 60)); // TODO: replace magic numbers!

    // move piece by updating the index to the index of "to", the restore it to "to"
    from_storage.m_index = indexFromPoint(to);
    restore(&from_storage);


    // update king/rook state for castling
    MPiece::MType type = MPiece::PAWN; // that is, neither king or rook
    MPiece::MColour colour = MPiece::WHITE;
    if (!from_storage.empty())
    {
        type = from_storage.m_piece->getType();
        colour = from_storage.m_piece->getColour();
    }

    if (type == MPiece::KING)
    {
        //from_storage.m_piece->hasMoved();
        kingMoved(colour);
    }
    else if (type == MPiece::ROOK)
    {
        //from_storage.m_piece->hasMoved();
        rookMoved(colour, from);
    }
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

QString MPosition::convertToChessCell(QPoint location) const
{
    const int small_letter_a = 97;
    return QString("%1%2").arg(static_cast<char>(small_letter_a + location.x()))
                          .arg(8 - location.y());
}

MStorage MPosition::store(const QPoint &location)
{
    const int index = indexFromPoint(location);

    MSharedPiece piece = m_position[index];
    MStorage storage = MStorage(index, piece);

    if (!piece.isNull())
    {
        piece->hide();
    }

    m_position[index].clear(); // remove from position
    return storage;
}

void MPosition::restore(MStorage* const storage)
{
    // delete whatever is at the storage's location
    removePieceAt(indexToPoint(storage->m_index));

    if (!storage->empty())
    {
        storage->m_piece->show();
        if (storage->m_piece->getType() == MPiece::KING)
        {
            updateKingPosition(storage->m_piece->getColour(), indexToPoint(storage->m_index));
        }
    }
    m_position[storage->m_index] = storage->m_piece;
}

void MPosition::addPieceAt(MPiece* piece, const QPoint &target)
{
    Q_CHECK_PTR(piece);

    piece->moveTo(QPoint(target.x() * 60, target.y() * 60)); // TODO: remove magic numbers!

    m_position[indexFromPoint(target)] = MSharedPiece(piece);

    if (piece && piece->getType() == MPiece::KING)
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
    MStorage removed = store(target);
    if(!removed.m_piece.isNull())
    {
        removed.m_piece->hide();
    }
}

void MPosition::reset()
{
    // TODO: check whether this leaks
    m_position = MPieces(m_position.size());
    m_colour_to_move = MPiece::WHITE;
}

void MPosition::updatePieces()
{
    for(MPieces::iterator iter = m_position.begin();
        iter != m_position.end();
        ++iter)
    {
        MSharedPiece piece = *iter;
        if (!piece.isNull())
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

void MPosition::updateKingPosition(MPiece::MColour colour, QPoint to)
{
    if (colour == MPiece::WHITE)
    {
        m_white_king = to;
    }
    else
    {
        m_black_king = to;
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
