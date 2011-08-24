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

#include "chessboardelement.h"

namespace Game { namespace Frontend {

ChessBoardElement::ChessBoardElement(QObject *parent)
    : QObject(parent)
    , m_piece()
    , m_piece_image()
    , m_piece_color()
    , m_square_color()
{}

void ChessBoardElement::setPiece(const QString &piece)
{
    m_piece = piece;
}

QString ChessBoardElement::piece() const
{
    return m_piece;
}

void ChessBoardElement::setPieceImage(const QString &piece_image)
{
    m_piece_image = piece_image;
}

QString ChessBoardElement::pieceImage() const
{
    return m_piece_image;
}

void ChessBoardElement::setPieceColor(const QString &piece_color)
{
    m_piece_color = piece_color;
}

QString ChessBoardElement::pieceColor() const
{
    return m_piece_color;
}

void ChessBoardElement::setSquareColor(const QString &square_color)
{
    m_square_color = square_color;
}

QString ChessBoardElement::squareColor() const
{
    return m_square_color;
}

}} // namespace GameGame, Frontend
