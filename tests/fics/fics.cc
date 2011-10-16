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

namespace Game { namespace Test {

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

class Setup
{
private:
    Dispatcher m_dispatcher;
    Testee m_testee;
    Frontend::Miniature m_frontend;
    TestUtils::ScenarioLoader m_loader;

public:
    explicit Setup()
        : m_dispatcher()
        , m_testee(&m_dispatcher)
        , m_frontend(&m_dispatcher)
        , m_loader()
    {
        m_dispatcher.setBackend(&m_testee);
        m_dispatcher.setFrontend(&m_frontend);

        // Prevents engine from connecting to FICS:
        m_testee.setChannelEnabled(false);
    }

    TestUtils::Scenario loadScenario(const QString &name)
    {
        return m_loader.load(&m_testee, name);
    }

    Dispatcher * dispatcher()
    {
        return &m_dispatcher;
    }

    Testee * testee()
    {
        return &m_testee;
    }

    Frontend::Miniature * frontend()
    {
        return &m_frontend;
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
        Setup s;

        QVERIFY(playLoginScenario(s.frontend(), s.testee()));
        QCOMPARE(s.frontend()->localSide()->id(), QString("GuestNFYR"));
    }

    Q_SLOT void testRegisteredLogin()
    {
        Setup s;
        TestUtils::Scenario sc = s.loadScenario("fics-registered-login");

        QSignalSpy login_succeeded(s.frontend(), SIGNAL(loginSucceeded()));
        QSignalSpy login_failed(s.frontend(), SIGNAL(loginFailed()));

        s.frontend()->login("MiniatureTest", "wrongpw");

        const int max_loop_count = 10;
        const int failed_login_index = 2;

        // Enter the scenario's feedback loop:
        for (int i = 0; i < max_loop_count && not sc.finished(); ++i) {
            sc.play();

            if (i == failed_login_index) {
                // We want to see whether we can recover from a failed login:
                s.frontend()->login("MiniatureTest", "TestMiniature");
            }

            sc.respond(s.testee()->response());
        }

        TestUtils::waitForSignal(s.frontend(), SIGNAL(loginFailed()));
        TestUtils::waitForSignal(s.frontend(), SIGNAL(loginSucceeded()));
        QVERIFY(sc.finished());
        QCOMPARE(sc.result(), TestUtils::Scenario::Passed);
        QCOMPARE(login_failed.count(), 1);
        QCOMPARE(login_succeeded.count(), 1);
        QCOMPARE(s.frontend()->localSide()->id(), QString("MiniatureTest"));
    }

    Q_SLOT void testGame()
    {
        Setup s;

        // Brings FICS engine into a ready state:
        QVERIFY(playLoginScenario(s.frontend(), s.testee()));

        s.testee()->enableDebugOutput(true);
        TestUtils::Scenario sc = s.loadScenario("fics-game");

        QSignalSpy game_started(s.frontend(), SIGNAL(gameStarted()));
        QSignalSpy game_ended(s.frontend(), SIGNAL(gameEnded(int,int)));

        sc.play();

        s.frontend()->play(11);
        sc.respond(s.testee()->response());
        sc.play();

        QCOMPARE(s.frontend()->activeGame()->id(), 414u);
        QCOMPARE(s.frontend()->localSide()->id(), QString("GuestNFYR"));
        QCOMPARE(s.frontend()->remoteSide()->id(), QString("testonetwo"));

        QVERIFY(s.frontend()->localSide()->active());

        // 1. g2-g4
        s.frontend()->selectSquare(54);
        s.frontend()->selectSquare(38);
        s.frontend()->confirmMove();
        sc.respond(s.testee()->response());
        sc.play();

        QVERIFY(s.frontend()->localSide()->active());

        // Illegal move: 2. f2-f6
        s.frontend()->selectSquare(53);
        s.frontend()->selectSquare(21);
        s.frontend()->confirmMove();
        sc.respond(s.testee()->response());
        sc.play();

        QVERIFY(s.frontend()->localSide()->active());

        // 2. f2-f4
        s.frontend()->selectSquare(53);
        s.frontend()->selectSquare(37);
        s.frontend()->confirmMove();
        sc.respond(s.testee()->response());
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
        Setup s;

        // Brings FICS engine into a ready state:
        QVERIFY(playLoginScenario(s.frontend(), s.testee()));

        s.testee()->enableDebugOutput(true);
        TestUtils::Scenario sc = s.loadScenario("fics-castling");

        sc.play();
        s.frontend()->seek(12, 12, "unrated", "auto");

        sc.respond(s.testee()->response());
        sc.play();

        // Nf3
        s.frontend()->selectSquare(62);
        s.frontend()->selectSquare(45);
        s.frontend()->confirmMove();

        sc.respond(s.testee()->response());
        sc.play();

        // g3
        s.frontend()->selectSquare(54);
        s.frontend()->selectSquare(46);
        s.frontend()->confirmMove();

        sc.respond(s.testee()->response());
        sc.play();

        // Bg2
        s.frontend()->selectSquare(61);
        s.frontend()->selectSquare(54);
        s.frontend()->confirmMove();

        sc.respond(s.testee()->response());
        sc.play();

        // O-O
        s.frontend()->selectSquare(60);
        s.frontend()->selectSquare(62);
        s.frontend()->confirmMove();

        sc.respond(s.testee()->response());
        sc.play();

        // Sc3
        s.frontend()->selectSquare(57);
        s.frontend()->selectSquare(42);
        s.frontend()->confirmMove();

        sc.respond(s.testee()->response());
        sc.play();

        // d3
        s.frontend()->selectSquare(51);
        s.frontend()->selectSquare(43);
        s.frontend()->confirmMove();

        sc.respond(s.testee()->response());
        sc.play();

        const Piece white_king(Piece::King, ColorWhite, toSquare("g1"));
        const Piece white_rook(Piece::Rook, ColorWhite, toSquare("f1"));
        const Piece black_king(Piece::King, ColorBlack, toSquare("c8"));
        const Piece black_rook(Piece::Rook, ColorBlack, toSquare("d8"));

        QCOMPARE(s.frontend()->activeGame()->position().pieceAt(toSquare("g1")), white_king);
        QCOMPARE(s.frontend()->activeGame()->position().pieceAt(toSquare("f1")), white_rook);
        QCOMPARE(s.frontend()->activeGame()->position().pieceAt(toSquare("c8")), black_king);
        QCOMPARE(s.frontend()->activeGame()->position().pieceAt(toSquare("d8")), black_rook);

        QVERIFY(sc.finished());
        QCOMPARE(sc.result(), TestUtils::Scenario::Passed);
    }
};


}} // namespace Test, Game

QTEST_APPLESS_MAIN(Game::Test::Fics)
#include ".moc/fics.moc"
