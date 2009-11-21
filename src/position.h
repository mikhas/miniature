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

#include <QObject>
#include <QString>
#include <QVector>
#include <QPoint>
//#include "piece_includes.h"

namespace Miniature
{

/* This class represents a chess position.*/
// TODO: move validation belongs in logic unit
// TODO: kill off FEN support needed for pieces pool manager
// TODO: give each MPiece the correct shared svg renderer so that the board can
//       draw by using MPiece, too
class MPosition
: public QObject
{
    Q_OBJECT

public:
    explicit MPosition(int width = 8, int height = 8, QObject *parent = 0); // construct the start position
    explicit MPosition(QString fen, int width = 8, int height = 8, QObject *parent = 0);
    ~MPosition();

    enum MPieceTypes {BROOK, BKNIGHT, BBISHOP, BQUEEN, BKING, BPAWN,
                      WROOK, WKNIGHT, WBISHOP, WQUEEN, WKING, WPAWN,
                      UNKNOWN_PIECE};

    typedef QVector<MPiece*> MPiecesRow;
    typedef QVector<MPiecesRow> MPiecesGrid;

    QString convertToFen() const;
    MPieceTypes lookupPieceType(QChar fenPiece) const;
    MPiecesGrid getPosition() const;

public Q_SLOTS:
    void onPieceMoved(QPoint from, QPoint to);

Q_SIGNALS:
    void invalidMove(QPoint from, QPoint to);
    void positionChanged(const MPiecesGrid &position);

private:
    QString getDefaultStartPosition() const;
    void convertFromFen(QString fen);

    // TODO: add variables for castle options, player-to-move, half-move-counter(?), en-passant options, etc.
    void movePiece(QPoint from, QPoint to);
    bool verifyMove(QPoint from, QPoint to) const;
    MPiece* pieceAt(QPoint pos) const;

    MPiecesGrid m_position;

};


}; // namespace Miniature

#endif
