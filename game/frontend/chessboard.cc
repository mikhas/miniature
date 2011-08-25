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

#include "chessboard.h"
#include "frontend.h"

namespace Game { namespace Frontend { namespace {
    typedef QHash<Piece::Type, QString> PieceMap;
    PieceMap pieceMap()
    {
        PieceMap map;

        map.insert(Piece::Pawn, "Pawn");
        map.insert(Piece::Rook, "Rook");
        map.insert(Piece::Knight, "Knight");
        map.insert(Piece::Bishop, "Bishop");
        map.insert(Piece::Queen, "Queen");
        map.insert(Piece::King, "King");
        map.insert(Piece::None, "emptySquare");

        return map;
    }

    const PieceMap piece_map(pieceMap());

    int indexFromSquare(const Square &square)
    {
        return square.rank * 8 + square.file;
    }

    QString fromPiece(const Piece &piece)
    {
        return QString("%1%2").arg(fromColor(piece.color()))
                              .arg(piece_map.value(piece.type()));
    }

    Piece toPiece(const QString &str)
    {
        static const QRegExp match("(black|white)(\\w)");
        if (not match.exactMatch(str)) {
            return Piece();
        }

        return Piece(piece_map.key(match.cap(2)), (match.cap(1) == "white" ? ColorWhite : ColorBlack));
    }

    QString imageFromPiece(const Piece &piece)
    {
        QString type;

        switch (piece.type()) {
        case Piece::Pawn: type = "pawn"; break;
        case Piece::Rook: type = "rook"; break;
        case Piece::Knight: type = "knight"; break;
        case Piece::Bishop: type = "bishop"; break;
        case Piece::Queen: type = "queen"; break;
        case Piece::King: type = "king"; break;

        default:
            return "emptysquare.png";
        }

        return QString("%1/%2.png").arg(fromColor(piece.color()))
                .arg(type);
    }
}

ChessBoard::ChessBoard(QObject *parent)
    : QAbstractListModel(parent)
    , m_position()
    , m_undo_position()
    , m_square_color(64, SquareColorTransparent)
{
    // QML cannot cope with c-style-variable-names!
    QHash<int, QByteArray> roles;
    roles[RolePiece] = "piece";
    roles[RolePieceImage] = "pieceImage";
    roles[RolePieceColor] = "pieceColor";
    roles[RoleSquareColor] = "squareColor";
    setRoleNames(roles);
}

QVariant ChessBoard::get(int index,
                         const QString &role) const
{
    const QModelIndex idx(this->index(index, 0));
    return data(idx, roleNames().key(role.toLatin1()));
}

void ChessBoard::triggerDataChanged()
{
    emit dataChanged(index(0, 0), index(63, 0));
}

Position ChessBoard::position() const
{
    return m_position;
}

void ChessBoard::setPosition(const Position &position)
{
    const bool was_initial_position(m_position.pieces().count() == 0
                                    && position.pieces().count() > 0);
    const bool was_castling(not was_initial_position
                            && m_position.castlingFlags() != position.castlingFlags());

    m_undo_position = m_position;
    m_position = position;
    m_selected_piece = Piece();

    // Requires full board update:
    if (was_initial_position || was_castling) {
        emit dataChanged(index(0, 0), index(63, 0));
    } else {
        const MovedPiece &m(m_position.movedPiece());
        const Piece &p(m.piece());

        const QModelIndex &origin(index(indexFromSquare(m.origin()), 0));
        const QModelIndex &target(index(indexFromSquare(p.square()), 0));

        emit dataChanged(origin, origin);
        emit dataChanged(target, target);

        // TODO: Handle en-passant
    }
}

int ChessBoard::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 64;
}

QVariant ChessBoard::data(const QModelIndex &index,
                          int role) const
{
    const Piece &p(m_position.pieceAt(toSquare(index.row())));
    const bool valid_selection(p.type() != Piece::None
                               && p == m_selected_piece);
    const bool wrong_color(m_selected_piece.type() != Piece::None
                           && m_selected_piece.color() != m_position.nextToMove());

    switch(role) {
    case RolePiece: return fromPiece(p);
    case RolePieceImage: return imageFromPiece(p);
    case RolePieceColor: return fromColor(p.color());
    case RoleSquareColor: return (valid_selection ? "green"
                                                  : wrong_color ? "red"
                                                                : fromSquareColor(m_square_color.at(index.row())));
    }

    return QVariant();
}

void ChessBoard::selectPiece(const Square &target)
{
    m_selected_piece = m_position.pieceAt(target);
}

void ChessBoard::undo()
{
    if (m_undo_position.pieces().count() > 0
        && m_position != m_undo_position) {
        m_position = m_undo_position;
        triggerDataChanged();
    }
}

void ChessBoard::commitMove(Undo undo)
{
    if (undo == NoUndo) {
        m_undo_position = Position();
    }

    m_position.setNextToMove(m_position.nextToMove() == ColorWhite ? ColorBlack
                                                                   : ColorWhite);
}

QString ChessBoard::fromSquareColor(SquareColor sq) const
{
    switch(sq) {
    case SquareColorTransparent: return "transparent"; break;
    case SquareColorGreen: return "green";
    case SquareColorBlue: return "blue";
    case SquareColorRed: return "red";
    }

    return "";
}

}} // namespace Game, Frontend
