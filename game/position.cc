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
    : m_type(None)
    , m_color(ColorNone)
    , m_square(FileCount, RankCount)
{}

Piece::Piece(Type type,
             Color color)
    : m_type(type)
    , m_color(color)
    , m_square(FileCount, RankCount)
{}

Piece::Type Piece::type() const
{
    return m_type;
}

Color Piece::color() const
{
    return m_color;
}

Square Piece::square() const
{
    return m_square;
}

void Piece::setSquare(const Square &square)
{
    m_square = square;
}

MovedPiece::MovedPiece()
    : m_piece()
    , m_origin(Square(FileCount, RankCount))
{}

MovedPiece::MovedPiece(const Piece &piece,
                       const Square &origin)
    : m_piece(piece)
    , m_origin(origin)
{}

Piece MovedPiece::piece() const
{
    return m_piece;
}

Square MovedPiece::origin() const
{
    return m_origin;
}

Square MovedPiece::target() const
{
    return m_piece.square();
}

Position::Position()
    : m_pieces()
    , m_castling_flags(CanWhiteCastleShort | CanWhiteCastleLong
                       | CanBlackCastleShort | CanBlackCastleLong)
    , m_next_to_move(ColorNone)
    , m_double_pawn_push(FileCount) // mark as invalid;
{}

QVector<Piece> Position::pieces() const
{
    return m_pieces;
}

void Position::addPiece(const Piece &piece)
{
    if (piece.type() == Piece::None
        || piece.square().file == FileCount
        || piece.square().rank == RankCount) {
        qWarning() << __PRETTY_FUNCTION__
                   << "Refusing to add invalid piece to position.";
        return;
    }

    m_pieces.append(piece);
}

Piece Position::pieceAt(const Square &square) const
{
    foreach(const Piece &piece, m_pieces) {
        if (piece.square() == square) {
            return piece;
        }
    }

    return Piece();
}

Position::CastlingFlags Position::castlingFlags() const
{
    return m_castling_flags;
}

void Position::setCastlingFlags(const CastlingFlags &flags)
{
    m_castling_flags = flags;
}

Color Position::nextToMove() const
{
    return m_next_to_move;
}

void Position::setNextToMove(Color color)
{
    m_next_to_move = color;
}

File Position::doublePawnPush() const
{
    return m_double_pawn_push;
}

void Position::setDoublePawnPush(File file)
{
    m_double_pawn_push = file;
}

QString moveNotation(const Position &result,
                     const MovedPiece &moved_piece)
{
    Q_UNUSED(result)
    // TODO: This is gnuchess-style notation, allow for different types?
    return QString("%1%2").arg(toString(moved_piece.origin()))
                          .arg(toString(moved_piece.target()));
}

Position createStartPosition()
{
    Position p;

    Piece wp(Piece::Pawn, ColorWhite);
    Piece wr(Piece::Rook, ColorWhite);
    Piece wn(Piece::Knight, ColorWhite);
    Piece wb(Piece::Bishop, ColorWhite);
    Piece wq(Piece::Queen, ColorWhite);
    Piece wk(Piece::King, ColorWhite);

    Piece bp(Piece::Pawn, ColorBlack);
    Piece br(Piece::Rook, ColorBlack);
    Piece bn(Piece::Knight, ColorBlack);
    Piece bb(Piece::Bishop, ColorBlack);
    Piece bq(Piece::Queen, ColorBlack);
    Piece bk(Piece::King, ColorBlack);

    for (int index = 0; index < FileCount; ++index) {
        wp.setSquare(Square(static_cast<File>(index), Rank2));
        p.addPiece(wp);

        bp.setSquare(Square(static_cast<File>(index), Rank7));
        p.addPiece(bp);
    }

    wr.setSquare(Square(FileA, Rank1)); p.addPiece(wr);
    wr.setSquare(Square(FileH, Rank1)); p.addPiece(wr);
    br.setSquare(Square(FileA, Rank8)); p.addPiece(br);
    br.setSquare(Square(FileH, Rank8)); p.addPiece(br);

    wn.setSquare(Square(FileB, Rank1)); p.addPiece(wn);
    wn.setSquare(Square(FileG, Rank1)); p.addPiece(wn);
    bn.setSquare(Square(FileB, Rank8)); p.addPiece(bn);
    bn.setSquare(Square(FileG, Rank8)); p.addPiece(bn);

    wb.setSquare(Square(FileC, Rank1)); p.addPiece(wb);
    wb.setSquare(Square(FileF, Rank1)); p.addPiece(wb);
    bb.setSquare(Square(FileC, Rank8)); p.addPiece(bb);
    bb.setSquare(Square(FileF, Rank8)); p.addPiece(bb);

    wq.setSquare(Square(FileD, Rank1)); p.addPiece(wq);
    bq.setSquare(Square(FileD, Rank8)); p.addPiece(bq);

    wk.setSquare(Square(FileE, Rank1)); p.addPiece(wk);
    bk.setSquare(Square(FileE, Rank8)); p.addPiece(bk);

    return p;
}

bool operator==(const Piece &a,
                const Piece &b)
{
    return ((a.type() == b.type()) && (a.color() == b.color()));
}

Piece toPiece(char ch)
{
    Piece::Type t;
    Color c;

    switch (ch) {
    case 'P': t = Piece::Pawn; c = ColorWhite; break;
    case 'R': t = Piece::Rook; c = ColorWhite; break;
    case 'N': t = Piece::Knight; c = ColorWhite; break;
    case 'B': t = Piece::Bishop; c = ColorWhite; break;
    case 'Q': t = Piece::Queen; c = ColorWhite; break;
    case 'K': t = Piece::King; c = ColorWhite; break;

    case 'p': t = Piece::Pawn; c = ColorBlack; break;
    case 'r': t = Piece::Rook; c = ColorBlack; break;
    case 'n': t = Piece::Knight; c = ColorBlack; break;
    case 'b': t = Piece::Bishop; c = ColorBlack; break;
    case 'q': t = Piece::Queen; c = ColorBlack; break;
    case 'k': t = Piece::King; c = ColorBlack; break;

    default: t = Piece::None; c = ColorNone; break;
    }

    return Piece(t, c);
}

} // namespace Game
