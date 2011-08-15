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
#include "side.h"
#include "gnuchess.h"
#include "commandline.h"
#include "dispatcher.h"

#include <QtCore>
#include <QtGui>
#include <QtTest>

using Game::Position;
using Game::MovedPiece;

Q_DECLARE_METATYPE(Position)
Q_DECLARE_METATYPE(MovedPiece)

class TestGnuChess
    : public QObject
{
    Q_OBJECT

private:
    QScopedPointer<QApplication> m_app;
    QTextStream m_out;

    Q_SLOT void initTestCase()
    {
        qRegisterMetaType<Position>();
        qRegisterMetaType<MovedPiece>();
        m_app.reset(TestUtils::createApp("testgnuchess"));
    }

    Q_SLOT void testShortMatch()
    {
        Game::Dispatcher dispatcher;
        Game::Side *white = new Game::Side("white");
        Game::GnuChess  *black = new Game::GnuChess("black");
        QSignalSpy whiteSpy(white, SIGNAL(turnEnded(Position,MovedPiece)));
        QSignalSpy blackSpy(black, SIGNAL(turnEnded(Position,MovedPiece)));
        Game::Game game(&dispatcher, white, black);

        QCOMPARE(game.activeSide().data(), white);

        game.play();

        const Game::MovedPiece g2g4(Game::Square(Game::FileG, Game::Rank2),
                                    Game::Square(Game::FileG, Game::Rank4));
        emit white->turnEnded(Game::Position(), g2g4);
        TestUtils::waitForSignal(black, SIGNAL(turnEnded(Position,MovedPiece)));
        QCOMPARE(game.activeSide().data(),white);
        QCOMPARE(whiteSpy.count(), 1);
        QCOMPARE(blackSpy.count(), 1);

        const Game::MovedPiece f2f4(Game::Square(Game::FileF, Game::Rank2),
                                    Game::Square(Game::FileF, Game::Rank4));
        emit white->turnEnded(Game::Position(), f2f4);
        TestUtils::waitForSignal(black, SIGNAL(turnEnded(Position,MovedPiece)));
        QCOMPARE(game.activeSide().data(), white);
        QCOMPARE(whiteSpy.count(), 2);
        QCOMPARE(blackSpy.count(), 2);
    }

    Q_SLOT void testRunInBackgroundForeground()
    {
        Game::GnuChess subject("GnuChess");
        QSignalSpy spy(&subject, SIGNAL(turnEnded(Position,MovedPiece)));

        const Game::MovedPiece d2d4(Game::Square(Game::FileD, Game::Rank2),
                                    Game::Square(Game::FileD, Game::Rank4));
        subject.startTurn(Game::Position(), d2d4);

        // If running in background, we don't want gnuchess to compute moves:
        subject.runInBackground();
        TestUtils::waitForSignal(&subject, SIGNAL(turnEnded(Position,MovedPiece)), 1000);
        QCOMPARE(spy.count(), 0);

        subject.runInForeground();
        TestUtils::waitForSignal(&subject, SIGNAL(turnEnded(Position,MovedPiece)), 1000);
        QCOMPARE(spy.count(), 1);
    }

    Q_SLOT void testIllegalMove()
    {
        Game::GnuChess subject("GnuChess");
        QSignalSpy spy(&subject, SIGNAL(turnEnded(Position,MovedPiece)));

        // Gnuchess should ignore illegal moves:
        subject.startTurn(Game::Position(), Game::MovedPiece());
        TestUtils::waitForSignal(&subject, SIGNAL(turnEnded(Position,MovedPiece)), 1000);
        QCOMPARE(spy.count(), 0);
    }
};

QTEST_APPLESS_MAIN(TestGnuChess)
#include ".moc/testgnuchess.moc"
