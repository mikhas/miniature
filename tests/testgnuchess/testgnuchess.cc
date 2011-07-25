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
#include "game.h"
#include "localside.h"
#include "gnuchess.h"
#include "move.h"
#include "localparser.h"

#include <QtCore>
#include <QtGui>
#include <QtTest>

// Need to import symbol so that slot/signal signature stays same, for moc.
using Game::Move;
Q_DECLARE_METATYPE(Move)

class TestGnuChess
    : public QObject
{
    Q_OBJECT

private:
    QScopedPointer<QApplication> m_app;
    QTextStream m_out;

    Q_SLOT void initTestCase()
    {
        m_app.reset(TestUtils::createApp("testgnuchess"));
    }

    Q_SLOT void testShortMatch()
    {
        qRegisterMetaType<Move>();

        Game::LocalSide *white = new Game::LocalSide("white");
        Game::GnuChess  *black = new Game::GnuChess("black");
        QSignalSpy whiteSpy(white, SIGNAL(turnEnded(Move)));
        QSignalSpy blackSpy(black, SIGNAL(turnEnded(Move)));
        Game::Game game(white, black);

        QCOMPARE(game.side(Game::SideActive).data(), white);

        game.start();
        emit white->turnEnded(Move(Game::Position(), Game::Square(), Game::Square(),
                                   QString("g4")));
        TestUtils::waitForSignal(black, SIGNAL(turnEnded(Move)));
        QCOMPARE(game.side(Game::SideActive).data(), white);
        QCOMPARE(whiteSpy.count(), 1);
        QCOMPARE(blackSpy.count(), 1);

        emit white->turnEnded(Move(Game::Position(), Game::Square(), Game::Square(),
                                   QString("f4")));
        TestUtils::waitForSignal(black, SIGNAL(turnEnded(Move)));
        QCOMPARE(game.side(Game::SideActive).data(), white);
        QCOMPARE(whiteSpy.count(), 2);
        QCOMPARE(blackSpy.count(), 2);
    }
};

QTEST_APPLESS_MAIN(TestGnuChess)
#include ".moc/testgnuchess.moc"
