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
#include "miniature.h"

#include <QColor>

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

    int indexFromSquare(const Square &square,
                        ChessBoard::Orientation o)
    {
        const int index(square.rank * 8 + square.file);
        return (o == ChessBoard::WhiteAtBottom ? index : qAbs<int>(index - 63));
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

ChessBoard::MarkedMove::MarkedMove()
    : origin(-1)
    , target(-1)
{}

ChessBoard::ChessBoard(QObject *parent)
    : QAbstractListModel(parent)
    , m_position()
    , m_marked_move()
    , m_orienation(WhiteAtBottom)
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
    m_position = position;
    m_selected_piece = Piece();
    m_marked_move = MarkedMove();

    // TODO: Optimize model updates. Currently, the whole model is invalided with each position change.
    triggerDataChanged();
}

void ChessBoard::setOrientation(Orientation orientation)
{
    m_orienation = orientation;
}

int ChessBoard::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 64;
}

QVariant ChessBoard::data(const QModelIndex &index,
                          int role) const
{
    const int row = adjustedIndex(index.row());
    const Piece &piece = m_position.pieceAt(toSquare(row));
    const MovedPiece &moved_piece = m_position.movedPiece();
    const int last_move_origin = fromSquare(moved_piece.origin());
    const int last_move_target = fromSquare(moved_piece.target());

    switch(role) {
    case RolePiece: return fromPiece(piece);
    case RolePieceImage: return imageFromPiece(piece);
    case RolePieceColor: return fromColor(piece.color());
    case RoleSquareColor: {
        QColor square_color(Qt::transparent);
        if (m_marked_move.origin == row || last_move_origin == row) {
            square_color = QColor("cornflowerblue");
        } else if (m_marked_move.target == row || last_move_target == row) {
            square_color = QColor("olivedrab");
        }

        return square_color;
    }
    }

    return QVariant();
}

bool ChessBoard::selectSquare(int index,
                              Color color)
{
    bool result = true;
    const int adjusted(adjustedIndex(index));
    const Piece &p(m_position.pieceAt(toSquare(adjusted)));
    const int old_origin = adjustedIndex(m_marked_move.origin);
    const int old_target = adjustedIndex(m_marked_move.target);

    if (p.valid()) {
        if (p.color() == color
            && p.color() == m_position.nextToMove()) {
            m_selected_piece = p;
            m_marked_move.origin = adjusted;
            m_marked_move.target = -1;
        } else if (m_selected_piece.valid()) {
            m_marked_move.target = adjusted;
        } else {
            result = false;
        }
    } else {
        if (not m_selected_piece.valid()) {
            result = false;
        } else {
            m_marked_move.target = adjusted;
        }
    }

    emit dataChanged(this->index(index, 0),
                     this->index(index, 0));

    if (old_origin > -1) {
        emit dataChanged(this->index(old_origin, 0),
                         this->index(old_origin, 0));
    }

    if (old_target > -1) {
        emit dataChanged(this->index(old_target, 0),
                         this->index(old_target, 0));
    }

    return result;
}

bool ChessBoard::isValidMove() const
{
    // TODO: be more strict?
    return (m_marked_move.origin > -1
            && m_marked_move.target > -1);
}

Position ChessBoard::confirmMove() const
{
    if (not isValidMove()) {
        return Position();
    }

    MovedPiece m(Piece(m_selected_piece.type(), m_selected_piece.color(), toSquare(m_marked_move.target)),
                 toSquare(m_marked_move.origin));
    Position pos(m_position);
    pos.setMovedPiece(m);

    return pos;
}

int ChessBoard::adjustedIndex(int index) const
{
    // Invalid index remains invalid:
    if (index == -1) {
        return -1;
    }

    return (m_orienation == WhiteAtBottom ? index
                                          : qAbs<int>(index - 63));
}

}} // namespace Game, Frontend
