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
#include "commands.h"
#include "registry.h"
#include "frontend/miniature.h"

#include <QtCore>
#include <QtGui>
#include <QtTest>

using namespace Game;

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
        Dispatcher dispatcher;

        Frontend::Miniature *miniature = new Frontend::Miniature(&dispatcher, m_app.data());
        dispatcher.setFrontend(miniature);

        const QByteArray local("local");
        const QByteArray remote("remote");
        Command::CreateGame cg(TargetRegistry, 999u, WeakDispatcher(&dispatcher),
                               local, remote, LocalSideIsBlack);
        dispatcher.sendCommand(&cg);

        QVERIFY(miniature->activeGame());
        QCOMPARE(miniature->activeGame()->id(), 999u);
        Position pos0(miniature->activeGame()->position());
        QCOMPARE(pos0, createStartPosition());
        QCOMPARE(miniature->activeGame()->activeSide().name, remote);

        // TODO: turn into data-driven test?
        const Piece &pg4(Piece(Piece::Pawn, ColorWhite).setSquare(toSquare("g4")));
        MovedPiece mp0(pg4, toSquare("g2"));
        pos0.setMovedPiece(mp0);

        Command::Move mc0(TargetFrontend, 999u, pos0);
        dispatcher.sendCommand(&mc0);

        QCOMPARE(miniature->activeGame()->position().pieceAt(toSquare("g4")), pg4);
        QCOMPARE(miniature->activeGame()->activeSide().name, local);

        Position pos1(miniature->activeGame()->position());
        const Piece &pe5(Piece(Piece::Pawn, ColorBlack).setSquare(toSquare("e5")));
        MovedPiece mp1(pe5, toSquare("e7"));
        pos1.setMovedPiece(mp1);

        Command::Move mc1(TargetFrontend, 999u, pos1);
        dispatcher.sendCommand(&mc1);

        QCOMPARE(miniature->activeGame()->position().pieceAt(toSquare("e5")), pe5);
        QCOMPARE(miniature->activeGame()->activeSide().name, remote);
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
