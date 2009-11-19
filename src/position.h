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

#include <QString>
#include <QList>
#include <QPoint>

namespace Miniature
{


    class MPiece
{
public:

    enum MPieceTypes {ROOK, KNIGHT, BISHOP, QUEEN, KING, PAWN};
    enum MColours {BLACK, WHITE};

    MPiece(QChar);
    MPiece(MColours col, MPieceTypes typ);
    ~MPiece();



    MPieceTypes getType() const;
    MColours getColour() const;

private:
    MPieceTypes type;
    MColours colour;
};


class MPosition
{
Q_OBJECT

public:
    MPosition(); // construct the start position
    explicit MPosition(QString fen);
    // TODO: constructor from a MPiecesGrid
    ~MPosition();

    enum MPieceTypes {BROOK, BKNIGHT, BBISHOP, BQUEEN, BKING, BPAWN,
                      WROOK, WKNIGHT, WBISHOP, WQUEEN, WKING, WPAWN,
                      UNKNOWN_PIECE};

    typedef QList<QList<MPiece*> > MPiecesGrid;

    QString convertToFen() const;
    MPieceTypes lookupPieceType(QChar fenPiece) const;
    MPiecesGrid getPositionList() const;

Q_SIGNALS:
    void confirmedPosition(MPiecesGrid newPosition);

private:
    QString getDefaultStartPosition() const;
    MPiecesGrid convertFenToList(QString fen) const;
    // TODO: replace with a 8x8 grid
    QString m_position;
    MPiecesGrid m_position_list;
    // TODO: add variables for castle options, player-to-move, half-move-counter(?), en-passant options, etc.
    void movePiece(QPoint from, QPoint to);
    bool verifyMove(QPoint from, QPoint to) const;
    bool verifyMovePawn(QPoint from, QPoint to) const;
    bool verifyMoveRook(QPoint from, QPoint to) const;
    bool verifyMoveKnight(QPoint from, QPoint to) const;
    bool verifyMoveBishop(QPoint from, QPoint to) const;
    bool verifyMoveQueen(QPoint from, QPoint to) const;
    bool verifyMoveKing(QPoint from, QPoint to) const;

 private Q_SLOTS:
    void onPieceMoved(QPoint from, QPoint to);
};




/*class MGameLogic
{
  Q_OBJECT

  public:

  public Q_SLOT:

  Q_SIGNALS:


  private:

};*/

}; // namespace Miniature

#endif

