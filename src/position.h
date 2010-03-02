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

#ifndef POSITION_H__
#define POSITION_H__

#include "pieces.h"

#include <QString>
#include <QVector>
#include <QPoint>
#include <QSharedPointer>

namespace Miniature
{

/* This class represents a simple piece storage. */
class mStorage
{
public:
    mStorage(int index, MPiece *piece);
    ~mStorage();

    bool empty() const;

    int index;
    MPiece *piece;
};

/* This class represents a chess position.*/
// TODO: provide a MPosition* clone() for copying instead of relying on copy
// ctor/copy assignment op? Qt very rarely allows copy contruction so it could
// be confusing that MPosition does allow it.
// OR: Make MPosition a subclass of QSharedData, to be used inside of a
// QSharedDataPointer. Reasoning: QSharedData uses copy ctors. Much better than
// to declare MPosition a "value type".

class MPosition
{
public:
    typedef QVector<MPiece*> MPieces;

    // construct empty position
    explicit MPosition(int width = 8, int height = 8, int cell_size = 60);
    ~MPosition();

    QString mapToString(const QPoint &cell) const;

    /* Stores the contents of what was found at the given location, and removes
     * it from m_position. */
    mStorage store(const QPoint &location);
    /* Restores a MStorage to m_position, using the original location. If the
     * original location is non-empty it gets overwritten. Thus, re-applying
     * restore over the same MStorage is safe.
     */
    void restore(mStorage* const storage);

    /* Takes no ownership of piece! */
    void addPieceAt(MPiece* piece, const QPoint &target);
    /* Returns the pointer to the removed piece. */
    MPiece * removePieceAt(const QPoint &target);
    void reset();

    // TODO: add variables for castle options, player-to-move, half-move-counter(?), en-passant options, etc.
    /* Moves a piece and returns the traditional chess notation for a move,
     * e.g., "1. (4,6) to (6,6)" = "1. e4"
     */
    MPiece * movePiece(const QPoint &origin, const QPoint &target);
    MPiece * pieceAt(const QPoint &cell) const;

    MPieces::const_iterator begin() const;
    MPieces::const_iterator end() const;

    int indexFromPoint(const QPoint &cell) const;
    // This conversion is also used by MBoardView to figure out the correct
    // position of a piece.
    QPoint indexToPoint(int index, int scaling = 1) const;

    // Moves all pieces on the scene graph to the cell position that was
    // remembered by the MPosition instance.
    void updatePieces();

    // Returns the pawn's position that can be taken en-passant, or (-1,-1)
    // (m_invalid_target)
    QPoint getPawnDoubleMove() const;
    void setPawnDoubleMove(const QPoint& target);

    QPoint getKing(MPiece::MColour colour) const;
    MPiece::MColour getColourToMove() const;
    void nextColour();
    void resetCastling();
    void kingMoved(MPiece::MColour colour);
    void rookMoved(MPiece::MColour colour, const QPoint &target);
    bool canWhiteCastleQueenside() const;
    bool canWhiteCastleKingside() const;
    bool canBlackCastleQueenside() const;
    bool canBlackCastleKingside() const;
    // Returns whether the currently moving side is in check.
    bool inCheck() const;
    void setInCheck(bool check);

    // A position is invalid if one of the two kings is located in an invalid
    // cell (-1,-1) - see m_invalid_target.
    bool isValid() const;

    // Print the position, for debugging purposes.
    void print() const;
    QString asFen() const;

private:
    QString getDefaultStartPosition() const;
    void updateKingPosition(MPiece::MColour colour, const QPoint &target);

    int m_width;
    int m_height;
    QPoint m_pawn_double_move;
    QPoint m_white_king;
    QPoint m_black_king;
    // TODO: unify all booleans into a bitset?
    bool m_white_qs_castle;
    bool m_white_ks_castle;
    bool m_black_qs_castle;
    bool m_black_ks_castle;
    bool m_in_check;
    MPiece::MColour m_colour_to_move;
    MPieces m_position;
    QString m_move_notation;

    static const QPoint m_invalid_target;
};

} // namespace Miniature

#endif
