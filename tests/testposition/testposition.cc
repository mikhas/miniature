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
        const Position p(createStartPosition());
        const Piece none;

        // Invalid access:
        QCOMPARE(p.pieceAt(Square(FileCount, RankCount)), none);

        // Default-constructed piece represents empty square, too:
        QCOMPARE(p.pieceAt(Square(FileA, Rank3)), none);

        QCOMPARE(p.pieceAt(Square(FileC, Rank2)), Piece(Piece::Pawn, ColorWhite));
        QCOMPARE(p.pieceAt(Square(FileC, Rank7)), Piece(Piece::Pawn, ColorBlack));
        QCOMPARE(p.pieceAt(Square(FileD, Rank1)), Piece(Piece::Queen, ColorWhite));
        QCOMPARE(p.pieceAt(Square(FileF, Rank8)), Piece(Piece::Bishop, ColorBlack));
        QCOMPARE(p.pieceAt(Square(FileA, Rank1)), Piece(Piece::Rook, ColorWhite));
        QCOMPARE(p.pieceAt(Square(FileH, Rank8)), Piece(Piece::Rook, ColorBlack));
    }
};

QTEST_APPLESS_MAIN(TestPosition)
#include ".moc/testposition.moc"
