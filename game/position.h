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
struct Piece
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

    Type type;
    Color color;

    explicit Piece();
    explicit Piece(Type new_type,
                   Color new_color);
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
struct Position
{
private:
    QVector<Piece> m_board;

public:
    explicit Position();

    Piece pieceAt(const Square &square) const;
    void setPieceAt(const Piece &piece,
                    const Square &square);
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
