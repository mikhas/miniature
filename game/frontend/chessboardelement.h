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

#ifndef CHESSBOARDELEMENT_H
#define CHESSBOARDELEMENT_H

#include <QtCore>

namespace Game { namespace Frontend {

//! An element of the ChessBoard model.
class ChessBoardElement
    : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ChessBoardElement)

    Q_PROPERTY(QString piece READ piece
                             WRITE setPiece)

    Q_PROPERTY(QString pieceImage READ pieceImage
                                  WRITE setPieceImage)

    Q_PROPERTY(QString pieceColor READ pieceColor
                                  WRITE setPieceColor)

    Q_PROPERTY(QString squareColor READ squareColor
                                   WRITE setSquareColor)

private:
    QString m_piece;
    QString m_piece_image;
    QString m_piece_color;
    QString m_square_color;

public:
    explicit ChessBoardElement(QObject *parent = 0);

    Q_INVOKABLE void setPiece(const QString &piece);
    Q_INVOKABLE QString piece() const;

    Q_INVOKABLE void setPieceImage(const QString &piece_image);
    Q_INVOKABLE QString pieceImage() const;

    Q_INVOKABLE void setPieceColor(const QString &piece_color);
    Q_INVOKABLE QString pieceColor() const;

    Q_INVOKABLE void setSquareColor(const QString &square_color);
    Q_INVOKABLE QString squareColor() const;
};

}} // namespace Game, Frontend

#endif // CHESSBOARDELEMENT_H
