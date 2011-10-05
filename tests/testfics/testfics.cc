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

namespace Game { namespace {
    void acceptAllMessages(Fics::Engine *fics) {
        fics->setMessageFilter(Fics::Engine::MessageFilterFlags(Fics::Engine::WaitingForGames
                                                                | Fics::Engine::WaitingForSeeks
                                                                | Fics::Engine::InGame
                                                                | Fics::Engine::PlayRequest));
    }

    bool loadIntoCache(const QString &fileName,
                       QVector<QByteArray> *cache)
    {
        QFile f(fileName);

        if(not cache || not f.open(QIODevice::ReadOnly)) {
            return false;
        }

        QByteArray line = f.readLine();
        while (not line.isNull()) {
            cache->append(line.trimmed());
            line = f.readLine();
        }

        return (not cache->isEmpty());
    }
} // namespace

class DummyFrontend
    : public Frontend::Miniature
{
public:
    QVector<Record> m_received_records;
    QVector<Seek> m_received_seeks;
    uint m_game_id;
    QWeakPointer<Game> m_game;

    explicit DummyFrontend(Dispatcher *dispatcher,
                           QObject *parent = 0)
        : Miniature::Miniature(dispatcher, parent)
        , m_received_records()
        , m_received_seeks()
        , m_game_id(0)
    {}

    virtual ~DummyFrontend()
    {}

    virtual void handleRecord(const Record &r)
    {
        m_received_records.append(r);
    }

    virtual void handleSeek(const Seek &s)
    {
        m_received_seeks.append(s);
    }
};

class DummyRegistry
    : public Registry
{
    explicit DummyRegistry(Dispatcher *dispatcher,
                           QObject *parent = 0)
        : Registry(dispatcher, parent)
    {}
};

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

class TestFics
    : public QObject
{
    Q_OBJECT

private:
    QScopedPointer<QApplication> m_app;
    QVector<QByteArray> m_session_log;
    QVector<QByteArray> m_play_log;
    QVector<QByteArray> m_commands_log;

    Q_SLOT void initTestCase()
    {
        m_app.reset(TestUtils::createApp("testfics"));

        // TODO: Introduce label suport for files, so that I can load a specific
        // scenario from a file (that's what I actually want, instead of having
        // too many different files.
        QVERIFY(loadIntoCache(MINIATURE_FICS_SESSION_LOG, &m_session_log));
        QVERIFY(loadIntoCache(MINIATURE_FICS_PLAY_LOG, &m_play_log));
        QVERIFY(loadIntoCache(MINIATURE_FICS_COMMANDS_LOG, &m_commands_log));
    }

    Q_SLOT void testParsing()
    {
        Dispatcher dispatcher;
        Registry *registry = new Registry(&dispatcher);
        dispatcher.resetRegistry(registry);

        Fics::Engine *fics = new Fics::Engine(&dispatcher);
        dispatcher.setBackend(fics);

        DummyFrontend frontend(&dispatcher);
        dispatcher.setFrontend(&frontend);
        acceptAllMessages(fics);

        QCOMPARE(frontend.m_received_records.size(), 0);
        QCOMPARE(frontend.m_received_seeks.size(), 0);

        fics->processToken(m_session_log.at(15));
        QCOMPARE(frontend.m_received_records.size(), 0);

        fics->processToken(m_session_log.at(175));
        QCOMPARE(frontend.m_received_records.size(), 1);

        fics->processToken(m_session_log.at(635));
        QCOMPARE(frontend.m_received_seeks.size(), 1);

        // Emable backend to parse create-game token:
        fics->play(1);

        fics->processToken(m_play_log.at(3));
        Game *game = registry->game(414u);
        QVERIFY(game);

        fics->processToken(m_play_log.at(8));
        QCOMPARE(game->position().pieceAt(toSquare("g4")),
                 Piece(Piece::Pawn, ColorWhite, toSquare("g4")));
        QCOMPARE(game->position().nextToMove(), ColorBlack);

        fics->processToken(m_play_log.at(10));
        QCOMPARE(game->position().pieceAt(toSquare("e5")),
                 Piece(Piece::Pawn, ColorBlack, toSquare("e5")));

        Position pos(game->position());
        // Constructing an invalid move:
        pos.setMovedPiece(MovedPiece(Piece(Piece::Pawn, ColorWhite, toSquare("f6")),
                                     toSquare("f2")));
        game->setPosition(pos);
        Command::Move move(TargetEngine, 414u, pos);
        dispatcher.sendCommand(&move);

        fics->processToken(m_play_log.at(39));
        QVERIFY(not game->position().pieceAt(toSquare("f6")).valid());
        QCOMPARE(game->position().pieceAt(toSquare("f2")),
                 Piece(Piece::Pawn, ColorWhite, toSquare("f2")));

        // TODO: turn parsing of FICS commands into data-driven tests,
        // dont use session or play logs for parsing tests.
        acceptAllMessages(fics);
        QSignalSpy game_started(&frontend, SIGNAL(gameStarted()));
        QSignalSpy game_ended(&frontend, SIGNAL(gameEnded(int,int)));

        fics->processToken(m_commands_log.at(0));
        fics->processToken(m_commands_log.at(1));
        fics->processToken(m_commands_log.at(3));

        TestUtils::waitForSignal(&frontend, SIGNAL(gameEnded(int,int)));
        QCOMPARE(game_started.count(), 1);
        QCOMPARE(game_ended.count(), 1);

        const QList<QVariant> &signal_args(game_ended.takeFirst());
        QCOMPARE(signal_args.at(0).toInt(),
                 static_cast<int>(ResultWhiteWins));
        QCOMPARE(signal_args.at(1).toInt(),
                 static_cast<int>(ReasonForfeitByDisconnect));

        acceptAllMessages(fics);

        fics->processToken(m_commands_log.at(6));
        game = registry->game(92u);
        QVERIFY(game);
        QCOMPARE(game->id(), 92u);

        fics->processToken(m_commands_log.at(7));
        QCOMPARE(game->position().movedPiece().type(), Piece::King);
        QCOMPARE(game->position().movedPiece().origin(), toSquare("e8"));
        QCOMPARE(game->position().movedPiece().target(), toSquare("g8"));

        fics->processToken(m_commands_log.at(8));
        QCOMPARE(game->position().movedPiece().type(), Piece::King);
        QCOMPARE(game->position().movedPiece().origin(), toSquare("e1"));
        QCOMPARE(game->position().movedPiece().target(), toSquare("c1"));
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

        TestUtils::ScenarioLoader sl;
        TestUtils::Scenario sc = sl.load(&testee, "fics-login");

        QSignalSpy login_succeeded(&frontend, SIGNAL(loginSucceeded()));
        frontend.login("guest", "");

        // Enter the scenario's feedback loop:
        while (not sc.finished()) {
            sc.play(testee.response());
        }

        TestUtils::waitForSignal(&frontend, SIGNAL(loginSucceeded()));
        QVERIFY(sc.finished());
        QCOMPARE(sc.result(), TestUtils::Scenario::Passed);
        QCOMPARE(login_succeeded.count(), 1);
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
        const int failed_login_index = 4;

        // Enter the scenario's feedback loop:
        for (int i = 0; i < max_loop_count && not sc.finished(); ++i) {
            sc.play(testee.response());

            if (i == failed_login_index) {
                // We want to see whether we can recover from a failed login:
                frontend.login("MiniatureTest", "TestMiniature");
            }
        }

        TestUtils::waitForSignal(&frontend, SIGNAL(loginFailed()));
        TestUtils::waitForSignal(&frontend, SIGNAL(loginSucceeded()));
        QVERIFY(sc.finished());
        QCOMPARE(sc.result(), TestUtils::Scenario::Passed);
        QCOMPARE(login_failed.count(), 1);
        QCOMPARE(login_succeeded.count(), 1);
        QCOMPARE(frontend.localSide()->id(), QString("MiniatureTest"));
    }
};


} // namespace Game

QTEST_APPLESS_MAIN(Game::TestFics)
#include ".moc/testfics.moc"
