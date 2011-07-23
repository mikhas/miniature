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
#include "ficsside.h"
#include "move.h"

#include <QtCore>
#include <QtGui>
#include <QtTest>

using Game::Move;
Q_DECLARE_METATYPE(Move)

using Game::AbstractLink;

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
        Game::AbstractSide *white = new Game::LocalSide("white");
        Game::AbstractSide *black = new Game::LocalSide("white");
        Game::Game subject(white, black);
        QCOMPARE(subject.activeSide().data(), white);

        // Trying to submit move before starting game => ignore:
        emit white->moveEnded(Game::Move());
        QCOMPARE(subject.activeSide().data(), white);

        subject.start();

        // Switch sides after submitting move:
        emit white->moveEnded(Game::Move());
        QCOMPARE(subject.activeSide().data(), black);

        // Trying to submit move twice in a row => ignore:
        emit white->moveEnded(Game::Move());
        QCOMPARE(subject.activeSide().data(), black);

        // Switch sides again after submitting move:
        emit black->moveEnded(Game::Move());
        QCOMPARE(subject.activeSide().data(), white);
    }

    Q_SLOT void testFicsGame()
    {
        Game::AbstractSide *local = new Game::LocalSide("local");

        // TODO: Stub the shared link.
        Game::FicsLink *link = new Game::FicsLink;
        link->login("guest", "");
        TestUtils::waitForSignal(link, SIGNAL(stateChanged(State)), 5000);
        Game::AbstractSide *fics = new Game::FicsSide("FICS", link);

        Game::Game subject(local, fics);
        subject.start();

        emit local->moveEnded(Game::Move());
        QCOMPARE(subject.activeSide().data(), fics);
    }

    Q_SLOT void testCli()
    {
#ifndef MINIATURE_CLI_ENABLED
        QSKIP("Miniature CLI support not enabled, skipping test.", SkipAll);
#else
        QProcess p;
        p.setEnvironment(QStringList(QString("LD_LIBRARY_PATH=%1")
            .arg(MINIATURE_LIB_PATH)));

        p.start(QString(MINIATURE_BIN));
        p.waitForStarted();
        QCOMPARE(p.state(), QProcess::Running);
        p.write("quit\n");
        p.waitForFinished();
        QCOMPARE(p.state(), QProcess::NotRunning);
#endif
    }
};

QTEST_APPLESS_MAIN(TestGame)
#include ".moc/testgame.moc"
