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

        Registry *registry = new Registry(&dispatcher, m_app.data());
        dispatcher.resetRegistry(registry);

        Frontend::Miniature *miniature = new Frontend::Miniature(&dispatcher, m_app.data());
        dispatcher.setFrontend(miniature);

        Command::CreateGame cg(TargetRegistry, 999u, WeakDispatcher(&dispatcher),
                            "  white", "black", LocalSideIsBlack);
        dispatcher.sendCommand(&cg);

        QVERIFY(miniature->activeGame());
        Position pos(miniature->activeGame()->position());
        QCOMPARE(pos, createStartPosition());
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
