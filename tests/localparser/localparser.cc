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

#include "commandline.h"
#include "testutils.h"
#include "linereader.h"
#include "dispatcher.h"
#include "commands.h"

#include <QtCore>
#include <QtGui>
#include <QtTest>

using Game::ParserCommand;
using Game::CommandLine;

Q_DECLARE_METATYPE(QVector<int>)

namespace Test {

class InputDevice
    : public QIODevice
{
private:
    QByteArray m_buf;

public:
    qint64 readData(char *data,
                    qint64 max_len)
    {
        QByteArray tmp(m_buf.left(max_len));
        m_buf.remove(0, max_len);
        strcpy(data, tmp.data());
        return tmp.size();
    }

    qint64 writeData(const char *data,
                     qint64)
    {
        m_buf = QByteArray(data);
        emit readyRead();
        return m_buf.size();
    }
};

class CountingDispatcher
    : public Game::Dispatcher
{
    Q_OBJECT

public:
    QVector<int> m_counters;

    explicit CountingDispatcher(QObject *parent = 0)
        : Game::Dispatcher(parent)
        , m_counters(4, 0)
    {}

    Q_SIGNAL void ready();

    bool sendCommand(Game::AbstractCommand *cmd)
    {
        if (dynamic_cast<Game::Command::Move *>(cmd)) {
            ++m_counters[0];
        } else if (dynamic_cast<Game::Command::Play *>(cmd)) {
            ++m_counters[1];
        } else if (dynamic_cast<Game::Command::Logout *>(cmd)) {
            ++m_counters[2];
        } else if (dynamic_cast<Game::Command::Login *>(cmd)) {
            ++m_counters[3];
        }

        emit ready();
        return true;
    }
};

namespace {
    void setupLink(Game::CommandLine *command_line,
                   Game::LineReader *tokenizer,
                   Game::ParserCommandFlags flags)
    {
        command_line->setFlags(flags);
        command_line->setEnabled(true);

        QObject::connect(tokenizer,    SIGNAL(tokenFound(QByteArray)),
                         command_line, SLOT(readData(QByteArray)));
    }
}

class LocalParser
    : public QObject
{
    Q_OBJECT

private:
    QScopedPointer<QApplication> m_app;

    Q_SLOT void initTestCase()
    {
        m_app.reset(TestUtils::createApp("localparser"));
    }

    Q_SLOT void testMultiParsing_data()
    {
        // Counters are interpreted as [move, play, logout, login]
        // Make sure that the compared vectors are of same size!
        qRegisterMetaType<QVector<int> >();
        QTest::addColumn<QByteArray>("input");
        QTest::addColumn<QVector<int> >("countersForFirstParser");
        QTest::addColumn<QVector<int> >("countersForSecondParser");

        QTest::newRow("p0 finds 'play'")
                << QByteArray("play\n")
                << (QVector<int>() << 0 << 1 << 0 << 0) << (QVector<int>(4, 0));

        QTest::newRow("p0 finds 'play', but ignores garbage")
                << QByteArray("garbage\nplay\nmore#124garbage\n")
                << (QVector<int>() << 0 << 1 << 0 << 0) << (QVector<int>(4, 0));

        QTest::newRow("p1 finds 'move'")
                << QByteArray("move\n")
                << (QVector<int>(4, 0)) << (QVector<int>() << 1 << 0 << 0 << 0);

        QTest::newRow("p0, p1 find 1x 'play', 2x 'move'")
                << QByteArray("play\nmove e4\nmove f4\n")
                << (QVector<int>() << 0 << 1 << 0 << 0) << (QVector<int>() << 2 << 0 << 0 << 0);

        QTest::newRow("p0, p1 both find 'play', 'move', 'quit'")
                << QByteArray("play\nmove e4\nquit\n")
                << (QVector<int>() << 0 << 1 << 1 << 0) << (QVector<int>() << 1 << 0 << 1 << 0);
    }

    Q_SLOT void testMultiParsing()
    {
        QFETCH(QByteArray, input);
        QFETCH(QVector<int>, countersForFirstParser);
        QFETCH(QVector<int>, countersForSecondParser);

        const int TimeOut(10);
        const QVector<int> emptyCounters(4, 0);

        Game::LineReader tokenizer;
        QIODevice *device = new InputDevice;
        tokenizer.init(device);
        device->open(QIODevice::ReadWrite);

        CountingDispatcher c0;
        Game::CommandLine l0;
        l0.setDispatcher(&c0);
        setupLink(&l0, &tokenizer,
                  Game::ParserCommandFlags(Game::CommandPlay | Game::CommandQuit));

        CountingDispatcher c1;
        Game::CommandLine l1;
        l1.setDispatcher(&c1);
        setupLink(&l1, &tokenizer,
                  Game::ParserCommandFlags(Game::CommandMove | Game::CommandQuit));

        CountingDispatcher c2;
        Game::CommandLine l2;
        l2.setDispatcher(&c2);
        setupLink(&l2, &tokenizer,
                  Game::ParserCommandFlags(Game::CommandNone));

        device->write(input);
        TestUtils::waitForSignal(&c0, SIGNAL(ready()), TimeOut);
        TestUtils::waitForSignal(&c1, SIGNAL(ready()), TimeOut);
        TestUtils::waitForSignal(&c2, SIGNAL(ready()), TimeOut);

        qDebug() << "counted_c0:" << c0.m_counters << "expected:" << countersForFirstParser;
        QCOMPARE(c0.m_counters, countersForFirstParser);
        qDebug() << "counted_c1:" << c1.m_counters << "expected:" << countersForSecondParser;
        QCOMPARE(c1.m_counters, countersForSecondParser);
        QCOMPARE(c2.m_counters, emptyCounters);
    }

    Q_SLOT void testChoppedInput()
    {
        const int TimeOut(20);

        Game::LineReader tokenizer;
        QIODevice *device = new InputDevice;
        tokenizer.init(device);
        device->open(QIODevice::ReadWrite);

        CountingDispatcher counter;
        Game::CommandLine link;
        link.setDispatcher(&counter);
        setupLink(&link, &tokenizer,
                  Game::ParserCommandFlags(Game::CommandMove));

        device->write("mo");
        TestUtils::waitForSignal(&counter, SIGNAL(ready()), TimeOut);
        QCOMPARE(counter.m_counters.at(0), 0);

        device->write("ve e4\n");
        TestUtils::waitForSignal(&counter, SIGNAL(ready()), TimeOut);
        QCOMPARE(counter.m_counters.at(0), 1);
    }

    Q_SLOT void testInputBeforeReady()
    {
        const int TimeOut(20);

        Game::LineReader tokenizer;
        QIODevice *device = new InputDevice;
        tokenizer.init(device);
        device->open(QIODevice::ReadWrite);

        CountingDispatcher counter;
        Game::CommandLine link;
        link.setDispatcher(&counter);
        setupLink(&link, &tokenizer,
                  Game::ParserCommandFlags(Game::CommandQuit));

        link.setEnabled(false);
        device->write("quit\n");
        TestUtils::waitForSignal(&counter, SIGNAL(ready()), TimeOut);
        QCOMPARE(counter.m_counters.at(2), 0);

        link.setEnabled(true);
        device->write("quit\n");
        TestUtils::waitForSignal(&counter, SIGNAL(ready()), TimeOut);
        QCOMPARE(counter.m_counters.at(2), 1);
    }
};

} // namespace Test

QTEST_APPLESS_MAIN(Test::LocalParser)
#include ".moc/localparser.moc"
