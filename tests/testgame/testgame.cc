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
#include "dispatcher.h"

#include <QtCore>
#include <QtGui>
#include <QtTest>

class TestGame
    : public QObject
{
    Q_OBJECT

private:
    QScopedPointer<QApplication> m_app;

    Q_SLOT void initTestCase()
    {
        m_app.reset(TestUtils::createApp("testgame"));
    }

    Q_SLOT void testLocalGame()
    {
        // After each valid move, active side should switch. So we can test a
        // valid move sequence simply by checking the active side against the
        // expected value.
        Game::Dispatcher dispatcher;
        Game::Side *white = new Game::Side("white");
        Game::Side *black = new Game::Side("black");
        Game::Game subject(0, &dispatcher, white, black);
        QCOMPARE(subject.activeSide(), white);

        // Trying to submit move before starting game => ignore:
        emit white->turnEnded(Game::Position(), Game::MovedPiece());
        QCOMPARE(subject.activeSide(), white);

        subject.play();

        // Switch sides after submitting move:
        emit white->turnEnded(Game::Position(), Game::MovedPiece());
        QCOMPARE(subject.activeSide(), black);

        // Trying to submit move twice in a row => ignore:
        emit white->turnEnded(Game::Position(), Game::MovedPiece());
        QCOMPARE(subject.activeSide(), black);

        // Switch sides again after submitting move:
        emit black->turnEnded(Game::Position(), Game::MovedPiece());
        QCOMPARE(subject.activeSide(), white);
    }

    Q_SLOT void testCli()
    {
        QProcess p;
        p.setEnvironment(QStringList(QString("LD_LIBRARY_PATH=%1")
            .arg(MINIATURE_LIB_PATH)));

        p.start(QString(MINIATURE_BIN));
        p.waitForStarted();
        QCOMPARE(p.state(), QProcess::Running);
        p.write("quit\n");
        p.waitForFinished();
        QCOMPARE(p.state(), QProcess::NotRunning);
    }
};

QTEST_APPLESS_MAIN(TestGame)
#include ".moc/testgame.moc"
