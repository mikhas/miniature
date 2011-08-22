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
#include "frontend.h"
#include "fics/backend.h"
#include "game.h"

#include <QtCore>
#include <QtGui>
#include <QtTest>

namespace {
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
}

namespace Game {
class DummyFrontend
    : public Frontend
{
public:
    QVector<Record> m_received_records;
    QVector<Seek> m_received_seeks;
    uint m_game_id;

    explicit DummyFrontend(Dispatcher *dispatcher,
                           QObject *parent = 0)
        : Frontend(dispatcher, parent)
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

    virtual void registerGame(Game *game)
    {
        m_game_id = game->id();
    }
};

}

class TestFics
    : public QObject
{
    Q_OBJECT

private:
    QScopedPointer<QApplication> m_app;
    QVector<QByteArray> m_session_log;
    QVector<QByteArray> m_play_log;
    QTextStream m_out;

    Q_SLOT void initTestCase()
    {
        m_app.reset(TestUtils::createApp("testfics"));

        QVERIFY(loadIntoCache(MINIATURE_FICS_SESSION_LOG, &m_session_log));
        QVERIFY(loadIntoCache(MINIATURE_FICS_PLAY_LOG, &m_play_log));
    }

    Q_SLOT void testParsing()
    {
        Game::Dispatcher dispatcher;

        Game::Fics::Backend *fics = new Game::Fics::Backend(&dispatcher);
        fics->enableTesting();
        dispatcher.setActiveBackend(fics);

        Game::DummyFrontend frontend(&dispatcher);
        dispatcher.setActiveFrontend(&frontend);

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
        QCOMPARE(fics->state(), Game::Fics::Backend::StatePlayPending);

        fics->processToken(m_play_log.at(3));
        QCOMPARE(frontend.m_game_id, 414u);
    }
};

QTEST_APPLESS_MAIN(TestFics)
#include ".moc/testfics.moc"
