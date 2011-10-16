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
#include "frontend/miniature.h"
#include "frontend/sideelement.h"
#include "fics/engine.h"
#include "game.h"
#include "position.h"
#include "registry.h"
#include "commands.h"
#include "utils/scenario.h"
#include "utils/scenarioloader.h"

#include <QtCore>
#include <QtGui>
#include <QtTest>

namespace Game {

class Testee
    : public Fics::Engine
{
private:
    QByteArray m_response;
    bool m_debug_output_enabled;

public:
    explicit Testee(Dispatcher *dispatcher,
                    QObject *parent = 0)
        : Fics::Engine(dispatcher, parent)
        , m_response()
        , m_debug_output_enabled(false)
    {}

    QByteArray response()
    {
        QByteArray result = m_response;
        m_response.clear();

        if (m_debug_output_enabled) {
            qDebug() << __PRETTY_FUNCTION__ << result;
        }

        return result;
    }

    virtual void writeToChannel(const QByteArray &data)
    {
        if (m_debug_output_enabled) {
            qDebug() << __PRETTY_FUNCTION__ << data;
        }

        m_response.append(data);
    }

    void enableDebugOutput(bool enable)
    {
        m_debug_output_enabled = enable;
    }
};

namespace {
    bool playLoginScenario(Frontend::Miniature *frontend,
                           Testee *engine)
    {
        if (not frontend || not engine) {
            return false;
        }

        TestUtils::ScenarioLoader sl;
        TestUtils::Scenario sc = sl.load(engine, "fics-login");

        QSignalSpy login_succeeded(frontend, SIGNAL(loginSucceeded()));
        frontend->login("guest", "");

        // Enter the scenario's feedback loop:
        while (not sc.finished()) {
            sc.play();
            sc.respond(engine->response());
        }

        // Clear last response, by quering it:
        (void) engine->response();

        return (login_succeeded.count() == 1
                && sc.finished()
                && sc.result() == TestUtils::Scenario::Passed);

    }
} // namespace


namespace Test {

class Fics
    : public QObject
{
    Q_OBJECT

private:
    QScopedPointer<QApplication> m_app;


    Q_SLOT void initTestCase()
    {
        m_app.reset(TestUtils::createApp("fics"));
    }

    Q_SLOT void testLogin()
    {
        Dispatcher dispatcher;
        Testee testee(&dispatcher);
        Frontend::Miniature frontend(&dispatcher);

        dispatcher.setBackend(&testee);
        dispatcher.setFrontend(&frontend);

        // Prevents engine from connecting to FICS:
        testee.setChannelEnabled(false);

        QVERIFY(playLoginScenario(&frontend, &testee));
        QCOMPARE(frontend.localSide()->id(), QString("GuestNFYR"));
    }

    Q_SLOT void testRegisteredLogin()
    {
        Dispatcher dispatcher;
        Testee testee(&dispatcher);
        Frontend::Miniature frontend(&dispatcher);

        dispatcher.setBackend(&testee);
        dispatcher.setFrontend(&frontend);

        // Prevents engine from connecting to FICS:
        testee.setChannelEnabled(false);

        TestUtils::ScenarioLoader sl;
        TestUtils::Scenario sc = sl.load(&testee, "fics-registered-login");

        QSignalSpy login_succeeded(&frontend, SIGNAL(loginSucceeded()));
        QSignalSpy login_failed(&frontend, SIGNAL(loginFailed()));

        frontend.login("MiniatureTest", "wrongpw");

        const int max_loop_count = 10;
        const int failed_login_index = 2;

        // Enter the scenario's feedback loop:
        for (int i = 0; i < max_loop_count && not sc.finished(); ++i) {
            sc.play();

            if (i == failed_login_index) {
                // We want to see whether we can recover from a failed login:
                frontend.login("MiniatureTest", "TestMiniature");
            }

            sc.respond(testee.response());
        }

        TestUtils::waitForSignal(&frontend, SIGNAL(loginFailed()));
        TestUtils::waitForSignal(&frontend, SIGNAL(loginSucceeded()));
        QVERIFY(sc.finished());
        QCOMPARE(sc.result(), TestUtils::Scenario::Passed);
        QCOMPARE(login_failed.count(), 1);
        QCOMPARE(login_succeeded.count(), 1);
        QCOMPARE(frontend.localSide()->id(), QString("MiniatureTest"));
    }

    Q_SLOT void testGame()
    {
        Dispatcher dispatcher;
        Testee testee(&dispatcher);
        Frontend::Miniature frontend(&dispatcher);

        dispatcher.setBackend(&testee);
        dispatcher.setFrontend(&frontend);

        // Prevents engine from connecting to FICS:
        testee.setChannelEnabled(false);

        // Brings FICS engine into a ready state:
        QVERIFY(playLoginScenario(&frontend, &testee));

        testee.enableDebugOutput(true);
        TestUtils::ScenarioLoader sl;
        TestUtils::Scenario sc = sl.load(&testee, "fics-game");

        QSignalSpy game_started(&frontend, SIGNAL(gameStarted()));
        QSignalSpy game_ended(&frontend, SIGNAL(gameEnded(int,int)));

        sc.play();

        frontend.play(11);
        sc.respond(testee.response());
        sc.play();

        QCOMPARE(frontend.activeGame()->id(), 414u);
        QCOMPARE(frontend.localSide()->id(), QString("GuestNFYR"));
        QCOMPARE(frontend.remoteSide()->id(), QString("testonetwo"));

        QVERIFY(frontend.localSide()->active());

        // 1. g2-g4
        frontend.selectSquare(54);
        frontend.selectSquare(38);
        frontend.confirmMove();
        sc.respond(testee.response());
        sc.play();

        QVERIFY(frontend.localSide()->active());

        // Illegal move: 2. f2-f6
        frontend.selectSquare(53);
        frontend.selectSquare(21);
        frontend.confirmMove();
        sc.respond(testee.response());
        sc.play();

        QVERIFY(frontend.localSide()->active());

        // 2. f2-f4
        frontend.selectSquare(53);
        frontend.selectSquare(37);
        frontend.confirmMove();
        sc.respond(testee.response());
        sc.play();

        QVERIFY(sc.finished());
        QCOMPARE(sc.result(), TestUtils::Scenario::Passed);
        QCOMPARE(game_started.count(), 1);
        QCOMPARE(game_ended.count(), 1);

        const QList<QVariant> &signal_args(game_ended.takeFirst());
        QCOMPARE(signal_args.at(0).toInt(),
                 static_cast<int>(ResultBlackWins));
        QCOMPARE(signal_args.at(1).toInt(),
                 static_cast<int>(ReasonCheckmated));

    }

    Q_SLOT void testCastling()
    {
        Dispatcher dispatcher;
        Testee testee(&dispatcher);
        Frontend::Miniature frontend(&dispatcher);

        dispatcher.setBackend(&testee);
        dispatcher.setFrontend(&frontend);

        // Prevents engine from connecting to FICS:
        testee.setChannelEnabled(false);

        // Brings FICS engine into a ready state:
        QVERIFY(playLoginScenario(&frontend, &testee));

        testee.enableDebugOutput(true);
        TestUtils::ScenarioLoader sl;
        TestUtils::Scenario sc = sl.load(&testee, "fics-castling");

        sc.play();
        frontend.seek(12, 12, "unrated", "auto");

        sc.respond(testee.response());
        sc.play();

        // Nf3
        frontend.selectSquare(62);
        frontend.selectSquare(45);
        frontend.confirmMove();

        sc.respond(testee.response());
        sc.play();

        // g3
        frontend.selectSquare(54);
        frontend.selectSquare(46);
        frontend.confirmMove();

        sc.respond(testee.response());
        sc.play();

        // Bg2
        frontend.selectSquare(61);
        frontend.selectSquare(54);
        frontend.confirmMove();

        sc.respond(testee.response());
        sc.play();

        // O-O
        frontend.selectSquare(60);
        frontend.selectSquare(62);
        frontend.confirmMove();

        sc.respond(testee.response());
        sc.play();

        // Sc3
        frontend.selectSquare(57);
        frontend.selectSquare(42);
        frontend.confirmMove();

        sc.respond(testee.response());
        sc.play();

        // d3
        frontend.selectSquare(51);
        frontend.selectSquare(43);
        frontend.confirmMove();

        sc.respond(testee.response());
        sc.play();

        const Piece white_king(Piece::King, ColorWhite, toSquare("g1"));
        const Piece white_rook(Piece::Rook, ColorWhite, toSquare("f1"));
        const Piece black_king(Piece::King, ColorBlack, toSquare("c8"));
        const Piece black_rook(Piece::Rook, ColorBlack, toSquare("d8"));

        QCOMPARE(frontend.activeGame()->position().pieceAt(toSquare("g1")), white_king);
        QCOMPARE(frontend.activeGame()->position().pieceAt(toSquare("f1")), white_rook);
        QCOMPARE(frontend.activeGame()->position().pieceAt(toSquare("c8")), black_king);
        QCOMPARE(frontend.activeGame()->position().pieceAt(toSquare("d8")), black_rook);

        QVERIFY(sc.finished());
        QCOMPARE(sc.result(), TestUtils::Scenario::Passed);
    }
};


}} // namespace Test, Game

QTEST_APPLESS_MAIN(Game::Test::Fics)
#include ".moc/fics.moc"
