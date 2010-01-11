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
#include <QTextStream>

using namespace Miniature;

// mStorage impl
mStorage::mStorage(int idx, MPiece *ptr)
: index(idx),
  piece(ptr)
{}

mStorage::~mStorage()
{
    // No need to delete. MPieces are owned by the MGraphicsBoard instances.
}

bool mStorage::empty() const
{
    return (0 == piece);
}

// MPosition impl
const QPoint MPosition::m_invalid_target = QPoint(-1, -1);

MPosition::MPosition(int width, int height, int /*cell_size*/)
: m_width(width),
  m_height(height),
  m_pawn_double_move(m_invalid_target),
  m_white_king(m_invalid_target),
  m_black_king(m_invalid_target),
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

MPiece * MPosition::movePiece(const QPoint &origin, const QPoint &target)
{
    Q_ASSERT(*this != MPosition::invalid);

    mStorage origin_storage = store(origin);
    MPiece *piece = origin_storage.piece;

    Q_CHECK_PTR(piece);

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

    return piece;
}

MPiece * MPosition::pieceAt(const QPoint &cell) const
{
    // I wanted to avoid this boundaries check, but it is quite reasonable to
    // expect out-of-boundaries access, especially from the logic
    // analyzer. (4, -1) for example.
    const int index = indexFromPoint(cell);
    if (0 <= index && index < m_position.size())
    {
        return m_position[index];
    }

    return 0;
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

    if (0 <= index && index < m_position.size())
    {
        mStorage storage = mStorage(index, m_position[index]);
        m_position[index] = 0; // remove from position
        return storage;
    }

    return mStorage(-1, 0);
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

MPiece * MPosition::removePieceAt(const QPoint &target)
{
    mStorage removed = store(target);
    return removed.piece;
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

QPoint MPosition::getPawnDoubleMove() const
{
    return m_pawn_double_move;
}

void MPosition::setPawnDoubleMove(const QPoint& target)
{
    m_pawn_double_move = QPoint(target);
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

bool MPosition::isValid() const
{
    return (m_white_king != m_invalid_target) && (m_black_king != m_invalid_target);
}

void MPosition::print() const
{
    QTextStream cout(stdout);
    int i = 0;
    for(MPieces::const_iterator iter = m_position.begin();
        iter != m_position.end();
        ++iter)
    {
         MPiece *piece = *iter;
         if (piece)
         {
             switch(piece->getType())
             {
                 case MPiece::PAWN:   cout << (piece->getColour() == MPiece::WHITE ? " P " : " p "); break;
                 case MPiece::KNIGHT: cout << (piece->getColour() == MPiece::WHITE ? " N " : " n "); break;
                 case MPiece::ROOK:   cout << (piece->getColour() == MPiece::WHITE ? " R " : " r "); break;
                 case MPiece::BISHOP: cout << (piece->getColour() == MPiece::WHITE ? " B " : " b "); break;
                 case MPiece::QUEEN:  cout << (piece->getColour() == MPiece::WHITE ? " Q " : " q "); break;
                 case MPiece::KING:   cout << (piece->getColour() == MPiece::WHITE ? " K " : " k "); break;

                 default: break;
             }
         }
         else
         {
             cout << " . ";
         }

         if (7 == i++ % 8)
         {
             cout << "\n";
         }
    }

    cout << "\n" << "pawn double move: (" << m_pawn_double_move.x() << ", " << m_pawn_double_move.y() << ")\n----\n";
    cout.flush();
}
