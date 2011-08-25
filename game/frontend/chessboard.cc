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

    int indexFromSquare(const Square &square)
    {
        return square.rank * 8 + square.file;
    }

    QString fromPiece(const Piece &piece)
    {
        QString type;

        switch (piece.type()) {
        case Piece::Pawn: type = "Pawn"; break;
        case Piece::Rook: type = "Rook"; break;
        case Piece::Knight: type = "Knight"; break;
        case Piece::Bishop: type = "Bishop"; break;
        case Piece::Queen: type = "Queen"; break;
        case Piece::King: type = "Knight"; break;

        default:
            return "emptySquare";
        }

        return QString("%1%2").arg(fromColor(piece.color()))
                .arg(type);
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
        case Piece::King: type = "knight"; break;

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

void ChessBoard::set(int,
                     const QString &,
                     const QString &)
{}

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

void ChessBoard::setPosition(const Position &position)
{
    const bool was_initial_position(m_position.pieces().count() == 0
                                    && position.pieces().count() > 0);
    const bool was_castling(not was_initial_position
                            && m_position.castlingFlags() != position.castlingFlags());
    m_position = position;

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

    switch(role) {
    case RolePiece: return fromPiece(p);
    case RolePieceImage: return imageFromPiece(p);
    case RolePieceColor: return fromColor(p.color());
    case RoleSquareColor: return fromSquareColor(m_square_color.at(index.row()));
    }

    return QVariant();
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
