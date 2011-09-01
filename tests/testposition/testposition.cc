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

#include "testutils.h"
#include "position.h"

#include <QtCore>
#include <QtGui>

using namespace Game;

class TestPosition
    : public QObject
{
    Q_OBJECT

private:
    QScopedPointer<QApplication> m_app;

    Q_SLOT void initTestCase()
    {
        m_app.reset(TestUtils::createApp("testposition"));
    }

    Q_SLOT void testStartPosition()
    {
        const Position pos(createStartPosition());
        const Piece none;

        // Invalid access:
        QCOMPARE(pos.pieceAt(Square(FileCount, RankCount)), none);

        // Default-constructed piece represents empty square, too:
        QCOMPARE(pos.pieceAt(Square(FileA, Rank3)), none);

        QCOMPARE(pos.pieceAt(toSquare("c2")),
                 Piece(Piece::Pawn, ColorWhite).setSquare(toSquare("c2")));
        QCOMPARE(pos.pieceAt(toSquare("c7")),
                 Piece(Piece::Pawn, ColorBlack).setSquare(toSquare("c7")));
        QCOMPARE(pos.pieceAt(toSquare("d1")),
                 Piece(Piece::Queen, ColorWhite).setSquare(toSquare("d1")));
        QCOMPARE(pos.pieceAt(toSquare("f8")),
                 Piece(Piece::Bishop, ColorBlack).setSquare(toSquare("f8")));
        QCOMPARE(pos.pieceAt(toSquare("a1")),
                 Piece(Piece::Rook, ColorWhite).setSquare(toSquare("a1")));
        QCOMPARE(pos.pieceAt(toSquare("h8")),
                 Piece(Piece::Rook, ColorBlack).setSquare(toSquare("h8")));
    }

    Q_SLOT void testCastlingNotation()
    {
        MovedPiece wk_long_castling(Piece(Piece::King, ColorWhite, toSquare("c1")), toSquare("e1"));
        QCOMPARE(moveNotation(wk_long_castling), QString("0-0-0"));

        MovedPiece wk_short_castling(Piece(Piece::King, ColorWhite, toSquare("g1")), toSquare("e1"));
        QCOMPARE(moveNotation(wk_short_castling), QString("0-0"));

        MovedPiece bk_long_castling(Piece(Piece::King, ColorBlack, toSquare("c8")), toSquare("e8"));
        QCOMPARE(moveNotation(bk_long_castling), QString("0-0-0"));

        MovedPiece bk_short_castling(Piece(Piece::King, ColorBlack, toSquare("g8")), toSquare("e8"));
        QCOMPARE(moveNotation(bk_short_castling), QString("0-0"));

        MovedPiece fake_castling(Piece(Piece::King, ColorBlack, toSquare("c6")), toSquare("e6"));
        QVERIFY(moveNotation(fake_castling) != QString("0-0-0"));

    }
};

QTEST_APPLESS_MAIN(TestPosition)
#include ".moc/testposition.moc"
