/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2009 Michael Hasselmann <michael@taschenorakel.de>
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

#ifndef POSITION_H__
#define POSITION_H__

#include "pieces.h"

#include <QString>
#include <QVector>
#include <QPoint>

namespace Miniature
{

/* This class represents a chess position.*/
// TODO: kill off FEN support needed for pieces pool manager
// TODO: give each MPiece the correct shared svg renderer so that the board can
//       draw by using MPiece, too
class MPosition
{

public:
    // construct empty position
    explicit MPosition(int width = 8, int height = 8);
    explicit MPosition(QString fen, int width = 8, int height = 8);
    ~MPosition();

    // TODO: Remove!
    enum MPieceTypes {BROOK, BKNIGHT, BBISHOP, BQUEEN, BKING, BPAWN,
                      WROOK, WKNIGHT, WBISHOP, WQUEEN, WKING, WPAWN,
                      UNKNOWN_PIECE};

    typedef QVector<MPiece*> MPieces;

    // These 3 methods are potentially deprecated!
    QString convertToFen() const;
    void convertFromFen(QString fen);
    MPieceTypes lookupPieceType(QChar fenPiece) const;

    void putPieceAt(MPiece* piece, QPoint pos);
    // TODO: add variables for castle options, player-to-move, half-move-counter(?), en-passant options, etc.
    void movePiece(QPoint from, QPoint to);
    MPiece* pieceAt(QPoint pos) const;

    MPieces::const_iterator begin() const;
    MPieces::const_iterator end() const;

    int indexFromPoint(QPoint point) const;
    // This conversion is also used by MBoardView to figure out the correct
    // position of a piece.
    QPoint indexToPoint(int index, int scaling = 1) const;

private:
    QString getDefaultStartPosition() const;

    const int m_width;
    const int m_height;
    MPieces m_position;

};

}; // namespace Miniature

#endif
