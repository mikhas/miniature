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

#include "position.h"

namespace {
    QString toString(const Game::Square &square)
    {
        QString result;

        switch(square.file) {
        case Game::FileA:
            result.append("a"); break;
        case Game::FileB:
            result.append("b"); break;
        case Game::FileC:
            result.append("c"); break;
        case Game::FileD:
            result.append("d"); break;
        case Game::FileE:
            result.append("e"); break;
        case Game::FileF:
            result.append("f"); break;
        case Game::FileG:
            result.append("g"); break;
        case Game::FileH:
            result.append("h"); break;

        default:
            break;
        }

        switch(square.rank) {
        case Game::Rank1:
            result.append("1"); break;
        case Game::Rank2:
            result.append("2"); break;
        case Game::Rank3:
            result.append("3"); break;
        case Game::Rank4:
            result.append("4"); break;
        case Game::Rank5:
            result.append("5"); break;
        case Game::Rank6:
            result.append("6"); break;
        case Game::Rank7:
            result.append("7"); break;
        case Game::Rank8:
            result.append("8"); break;

        default:
            break;
        }

        return result;
    }

    int toIndex(const Game::Square &square)
    {
        if (square.rank == Game::RankCount || square.file == Game::FileCount) {
            return -1;
        } else {
            return (square.file + square.rank * Game::FileCount);
        }
    }
}

namespace Game {

Piece::Piece()
    : type(None)
    , color(ColorAuto)
{}

Piece::Piece(Type new_type,
             Color new_color)
    : type(new_type)
    , color(new_color)
{}

MovedPiece::MovedPiece()
    : origin(Square(FileCount, RankCount))
    , target(Square(FileCount, RankCount))
{}

MovedPiece::MovedPiece(const Square &new_origin,
                       const Square &new_target)
    : origin(new_origin)
    , target(new_target)
{}

Position::Position()
    : m_board(RankCount * FileCount)
{}

Piece Position::pieceAt(const Square &square) const
{
    const int index(toIndex(square));
    if (index == -1) {
        static Piece invalid;
        return invalid;
    }

    return m_board.at(index);
}

void Position::setPieceAt(const Piece &piece,
                          const Square &square)
{
    const int index(toIndex(square));
    if (index == -1) {
        return;
    }

    m_board[index] = piece;
}

QString moveNotation(const Position &result,
                     const MovedPiece &moved_piece)
{
    Q_UNUSED(result)
    // TODO: This is gnuchess-style notation, allow for different types?
    return QString("%1%2").arg(toString(moved_piece.origin))
                          .arg(toString(moved_piece.target));
}

Position createStartPosition()
{
    Position p;

    for (int index = 0; index < FileCount; ++index) {
        p.setPieceAt(Piece(Piece::Pawn, ColorWhite), Square(static_cast<File>(index), Rank2));
        p.setPieceAt(Piece(Piece::Pawn, ColorBlack), Square(static_cast<File>(index), Rank7));
    }

    p.setPieceAt(Piece(Piece::Rook, ColorWhite), Square(FileA, Rank1));
    p.setPieceAt(Piece(Piece::Rook, ColorWhite), Square(FileH, Rank1));
    p.setPieceAt(Piece(Piece::Rook, ColorBlack), Square(FileA, Rank8));
    p.setPieceAt(Piece(Piece::Rook, ColorBlack), Square(FileH, Rank8));

    p.setPieceAt(Piece(Piece::Knight, ColorWhite), Square(FileB, Rank1));
    p.setPieceAt(Piece(Piece::Knight, ColorWhite), Square(FileG, Rank1));
    p.setPieceAt(Piece(Piece::Knight, ColorBlack), Square(FileB, Rank8));
    p.setPieceAt(Piece(Piece::Knight, ColorBlack), Square(FileG, Rank8));

    p.setPieceAt(Piece(Piece::Bishop, ColorWhite), Square(FileC, Rank1));
    p.setPieceAt(Piece(Piece::Bishop, ColorWhite), Square(FileF, Rank1));
    p.setPieceAt(Piece(Piece::Bishop, ColorBlack), Square(FileC, Rank8));
    p.setPieceAt(Piece(Piece::Bishop, ColorBlack), Square(FileF, Rank8));

    p.setPieceAt(Piece(Piece::Queen, ColorWhite), Square(FileD, Rank1));
    p.setPieceAt(Piece(Piece::Queen, ColorBlack), Square(FileD, Rank8));

    p.setPieceAt(Piece(Piece::King, ColorWhite), Square(FileE, Rank1));
    p.setPieceAt(Piece(Piece::King, ColorBlack), Square(FileE, Rank8));

    return p;
}

bool operator==(const Piece &a,
                const Piece &b)
{
    return ((a.type == b.type) && (a.color == b.color));
}

} // namespace Game
