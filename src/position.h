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
#include <QSharedPointer>
#include <QGraphicsView>

namespace Miniature
{

typedef QSharedPointer<MPiece> MSharedPiece;

/* This class represents a simple piece storage. */
class MStorage
{
public:
    MStorage(int index, MSharedPiece piece);
    ~MStorage();

    bool empty() const;

    int m_index;
    MSharedPiece m_piece;
};

/* This class represents a chess position.*/
// TODO: provide a MPosition* clone() for copying instead of relying on copy
// ctor/copy assignment op? Qt very rarely allows copy contruction so it could
// be confusing that MPosition does allow it.
// OR: Make MPosition a subclass of QSharedData, to be used inside of a
// QSharedDataPointer. Reasoning: QSharedData uses copy ctors. Much better than
// to declare MPosition a "value type".

// TODO: kill off FEN support needed for pieces pool manager
class MPosition
{
public:
    // construct empty position
    explicit MPosition(QGraphicsSvgItem *board, int width = 8, int height = 8);

    ~MPosition();

    typedef QVector<MSharedPiece> MPieces;

    // deprecated?
    QString convertToFen() const;
    // deprecated?
    void convertFromFen(QString fen);
    QString convertToChessCell(QPoint location) const;

    /* Stores the contents of what was found at the given location, and removes
     * it from m_position. */
    MStorage store(const QPoint &location);
    /* Restores a MStorage to m_position, using the original location. If the
     * original location is non-empty it gets overwritten. Thus, re-applying
     * restore over the same MStorage is safe. Releases ownership over the stored
     * piece. */
    // TODO: offer MStorage swapping?
    void restore(MStorage* const storage);

    /* Takes ownership of piece. */
    void addPieceAt(MPiece* piece, QPoint pos);
    void reset();

    // TODO: add variables for castle options, player-to-move, half-move-counter(?), en-passant options, etc.
    /* Moves a piece and returns the traditional chess notation for a move,
     * e.g., "1. (4,6) to (6,6)" = "1. e4"
     */
    void movePiece(QPoint from, QPoint to);
    MSharedPiece pieceAt(QPoint pos) const;

    MPieces::const_iterator begin() const;
    MPieces::const_iterator end() const;

    int indexFromPoint(QPoint point) const;
    // This conversion is also used by MBoardView to figure out the correct
    // position of a piece.
    QPoint indexToPoint(int index, int scaling = 1) const;
    QPoint getKing(MPiece::MColour colour) const;
    MPiece::MColour getColourToMove() const;
    void nextColour();
    void resetCastling();
    void kingMoved(MPiece::MColour colour);
    void rookMoved(MPiece::MColour colour, QPoint location);
    bool canWhiteCastleQueenside() const;
    bool canWhiteCastleKingside() const;
    bool canBlackCastleQueenside() const;
    bool canBlackCastleKingside() const;

private:
    QString getDefaultStartPosition() const;
    void updateKingPosition(MPiece::MColour colour, QPoint to);

    int m_width;
    int m_height;
    QPoint m_white_king;
    QPoint m_black_king;
    bool m_white_qs_castle;
    bool m_white_ks_castle;
    bool m_black_qs_castle;
    bool m_black_ks_castle;
    MPiece::MColour m_colour_to_move;
    MPieces m_position;
    QString m_move_notation;

    // Since our pieces are now QGraphicsSvgItems we need to know where to draw
    // them, too.
    QGraphicsSvgItem *m_board;
};

}; // namespace Miniature

#endif
