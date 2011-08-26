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

#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include "position.h"
#include "square.h"

#include <QtCore>

namespace Game { namespace Frontend {

//! Represents a chessboard, implementing the QAbstractListModel API.
//! get/set are added for convenience to to match QML's ListView/ListModel API.
class ChessBoard
    : public QAbstractListModel
{
    Q_OBJECT
    Q_DISABLE_COPY(ChessBoard)

public:
    enum Orientation {
        WhiteAtBottom,
        WhiteAtTop
    };

private:
    enum SquareColor {
        SquareColorTransparent,
        SquareColorBlue,
        SquareColorGreen,
        SquareColorRed
    };

    Position m_position;
    Position m_undo_position;
    QVector<SquareColor> m_square_color;
    Piece m_selected_piece;
    Orientation m_orienation;

public:
    enum Roles {
        RolePiece = Qt::UserRole + 1,
        RolePieceImage,
        RolePieceColor,
        RoleSquareColor
    };

    enum Undo {
        NoUndo,
        SaveUndo
    };

    explicit ChessBoard(QObject *parent = 0);

    Q_INVOKABLE QVariant get(int index,
                             const QString &role) const;

    void triggerDataChanged();

    Position position() const;
    void setPosition(const Position &position,
                     Undo undo = SaveUndo);

    void setOrientation(Orientation orientation);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index,
                  int role) const;

    void selectPiece(const Square &target);
    void undo();

    void commitMove(Undo undo = NoUndo);
    int adjustedIndex(int index) const;

private:
    QString fromSquareColor(SquareColor sq) const;
};

}} // namespace Game, Frontend

#endif // CHESSBOARD_H
