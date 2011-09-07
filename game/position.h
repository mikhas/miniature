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

    explicit Piece(Type type,
                   Color color,
                   const Square &square);

    bool valid() const;

    Type type() const;
    Color color() const;

    Square square() const;
    Piece & setSquare(const Square &square);
};

//! Convenience type that can be used to describe a move.
class MovedPiece
{
private:
    Piece m_piece;
    Square m_origin;

public:
    explicit MovedPiece();
    explicit MovedPiece(const Piece &piece,
                        const Square &origin);

    Piece piece() const;
    bool valid() const;
    Piece::Type type() const;
    Color color() const;

    Square origin() const;
    Square target() const;
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
    MovedPiece m_moved_piece;
    CastlingFlags m_castling_flags;
    Color m_next_to_move;
    File m_double_pawn_push;

public:
    explicit Position();

    QVector<Piece> pieces() const;
    void addPiece(const Piece &piece);
    Piece pieceAt(const Square &square) const;

    MovedPiece movedPiece() const;
    void setMovedPiece(const MovedPiece &moved_piece);

    CastlingFlags castlingFlags() const;
    void setCastlingFlags(const CastlingFlags &flags);

    Color nextToMove() const;
    void setNextToMove(Color color);

    File doublePawnPush() const;
    void setDoublePawnPush(File file);

    bool valid() const;

private:
    void setPiece(const Piece &piece);
    void clearSquare(const Square &square);
};

//! A human readable notation of a move, based on the resulting position and
//! its last moved piece.
QString moveNotation(const MovedPiece &moved_piece);

//! Constructs start position.
Position createStartPosition();

bool operator==(const Position &a,
                const Position &b);

bool operator!=(const Position &a,
                const Position &b);

bool operator==(const Piece &a,
                const Piece &b);

bool operator!=(const Piece &a,
                const Piece &b);

Piece toPiece(char ch,
              Color override = ColorNone);


} // namespace Game

#endif // POSITION_H
