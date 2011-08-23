/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2011 Michael Hasselmann <michael@taschenorakel.de>
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

#ifndef POSITION_H
#define POSITION_H

#include "square.h"
#include <QtCore>

namespace Game {

//! Represents a piece.
class Piece
{
public:
    enum Type {
        None,
        Pawn,
        Knight,
        Bishop,
        Rook,
        Queen,
        King
    };

private:
    Type m_type;
    Color m_color;
    Square m_square;

public:
    explicit Piece();
    explicit Piece(Type type,
                   Color color);

    Type type() const;
    Color color() const;

    Square square() const;
    void setSquare(const Square &square);
};

//! Convenience type that can be used to describe a move.
struct MovedPiece
{
    const Square origin;
    const Square target;

    explicit MovedPiece();
    explicit MovedPiece(const Square &new_origin,
                        const Square &new_target);
};

//! Represents a game position.
class Position
{
public:
    enum Castling {
        CanWhiteCastleShort = 0x1,
        CanWhiteCastleLong = 0x2,
        CanBlackCastleShort = 0x4,
        CanBlackCastleLong = 0x8
    };

    Q_DECLARE_FLAGS(CastlingFlags, Castling)

private:
    QVector<Piece> m_pieces;
    CastlingFlags m_castling_flags;
    Color m_next_to_move;
    File m_double_pawn_push;

public:
    explicit Position();

    QVector<Piece> pieces() const;
    void addPiece(const Piece &piece);
    Piece pieceAt(const Square &square) const;

    CastlingFlags castlingFlags() const;
    void setCastlingFlags(const CastlingFlags &flags);

    Color nextToMove() const;
    void setNextToMove(Color color);

    File doublePawnPush() const;
    void setDoublePawnPush(File file);
};

//! A human readable notation of a move, based on the resulting position and
//! the last moved piece.
QString moveNotation(const Position &result,
                     const MovedPiece &moved_piece);

//! Constructs start position.
Position createStartPosition();

bool operator==(const Piece &a,
                const Piece &b);

} // namespace Game

#endif // POSITION_H
