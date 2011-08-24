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

private:
    enum SquareColor {
        SquareColorTransparent,
        SquareColorBlue,
        SquareColorGreen,
        SquareColorRed
    };

    Position m_position;
    QVector<SquareColor> m_square_color;

public:
    enum Roles {
        RolePiece = Qt::UserRole + 1,
        RolePieceImage,
        RolePieceColor,
        RoleSquareColor
    };

    explicit ChessBoard(QObject *parent = 0);

    Q_INVOKABLE void set(int index,
                         const QString &data,
                         const QString &role);

    Q_INVOKABLE QVariant get(int index,
                             const QString &role) const;

    void triggerDataChanged();
    void setPosition(const Position &position);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index,
                  int role) const;

private:
    QString fromSquareColor(SquareColor sq) const;
};

}} // namespace Game, Frontend

#endif // CHESSBOARD_H
