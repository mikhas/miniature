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

#include "localparser.h"
#include "testutils.h"
#include "linereader.h"

#include <QtCore>
#include <QtGui>
#include <QtTest>

using Game::Command;
using Game::LocalParser;

Q_DECLARE_METATYPE(QVector<int>)

class TestInputDevice
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

class CommandCounter
    : public QObject
{
    Q_OBJECT

public:
    QVector<int> m_counters;

    explicit CommandCounter(QObject *parent = 0)
        : QObject(parent)
        , m_counters(3, 0)
    {}

    Q_SIGNAL void ready();

    Q_SLOT void onCommandFound(Command cmd,
                               const QByteArray &)
    {
        switch (cmd) {
        case Game::CommandMove:
            ++m_counters[0];
            break;

        case Game::CommandNew:
            ++m_counters[1];
            break;

        case Game::CommandQuit:
            ++m_counters[2];
            break;

        default:
            break;
        }

        emit ready();
    }
};

namespace {
    void setupParser(Game::AbstractParser *parser,
                     CommandCounter *counter,
                     Game::LineReader *tokenizer,
                     Game::CommandFlags flags)
    {
        parser->setFlags(flags);
        parser->setEnabled(true);

        QObject::connect(tokenizer, SIGNAL(tokenFound(QByteArray)),
                         parser,    SLOT(processToken(QByteArray)));

        QObject::connect(parser,  SIGNAL(commandFound(Command, QByteArray)),
                         counter, SLOT(onCommandFound(Command, QByteArray)));
    }
}

class TestLocalParser
    : public QObject
{
    Q_OBJECT

private:
    QScopedPointer<QApplication> m_app;

    Q_SLOT void initTestCase()
    {
        m_app.reset(TestUtils::createApp("testcommandparser"));
    }

    Q_SLOT void testMultiParsing_data()
    {
        // counters are interpreted as [move, new, quit]
        qRegisterMetaType<QVector<int> >();
        QTest::addColumn<QByteArray>("input");
        QTest::addColumn<QVector<int> >("countersForFirstParser");
        QTest::addColumn<QVector<int> >("countersForSecondParser");

        QTest::newRow("p0 finds 'new'")
                << QByteArray("new\n")
                << (QVector<int>() << 0 << 1 << 0) << (QVector<int>(3, 0));

        QTest::newRow("p0 finds 'new', but ignores garbage")
                << QByteArray("garbage\nnew\nmore#124garbage\n")
                << (QVector<int>() << 0 << 1 << 0) << (QVector<int>(3, 0));

        QTest::newRow("p1 finds 'move'")
                << QByteArray("move\n")
                << (QVector<int>(3, 0)) << (QVector<int>() << 1 << 0 << 0);

        QTest::newRow("p0, p1 find 1x 'new', 2x 'move'")
                << QByteArray("new\nmove e4\nmove f4\n")
                << (QVector<int>() << 0 << 1 << 0) << (QVector<int>() << 2 << 0 << 0);

        QTest::newRow("p0, p1 both find 'new', 'move', 'quit'")
                << QByteArray("new\nmove e4\nquit\n")
                << (QVector<int>() << 0 << 1 << 1) << (QVector<int>() << 1 << 0 << 1);
    }

    Q_SLOT void testMultiParsing()
    {
        QFETCH(QByteArray, input);
        QFETCH(QVector<int>, countersForFirstParser);
        QFETCH(QVector<int>, countersForSecondParser);

        const int TimeOut(10);
        const QVector<int> emptyCounters(3, 0);

        Game::LineReader tokenizer;
        QIODevice *device = new TestInputDevice;
        tokenizer.init(device);
        device->open(QIODevice::ReadWrite);

        Game::LocalParser p0;
        CommandCounter c0;
        setupParser(&p0, &c0, &tokenizer,
                    Game::CommandFlags(Game::CommandNew | Game::CommandQuit));

        Game::LocalParser p1;
        CommandCounter c1;
        setupParser(&p1, &c1, &tokenizer,
                    Game::CommandFlags(Game::CommandMove | Game::CommandQuit));

        Game::LocalParser p2;
        CommandCounter c2;
        setupParser(&p2, &c2, &tokenizer,
                    Game::CommandFlags(Game::CommandNone));

        device->write(input);
        TestUtils::waitForSignal(&c0, SIGNAL(ready()), TimeOut);
        TestUtils::waitForSignal(&c1, SIGNAL(ready()), TimeOut);
        TestUtils::waitForSignal(&c2, SIGNAL(ready()), TimeOut);

        QCOMPARE(c0.m_counters, countersForFirstParser);
        QCOMPARE(c1.m_counters, countersForSecondParser);
        QCOMPARE(c2.m_counters, emptyCounters);
    }

    Q_SLOT void testChoppedInput()
    {
        const int TimeOut(20);

        Game::LineReader tokenizer;
        QIODevice *device = new TestInputDevice;
        tokenizer.init(device);
        device->open(QIODevice::ReadWrite);

        Game::LocalParser parser;
        CommandCounter counter;
        setupParser(&parser, &counter, &tokenizer,
                    Game::CommandFlags(Game::CommandMove));

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
        QIODevice *device = new TestInputDevice;
        tokenizer.init(device);
        device->open(QIODevice::ReadWrite);

        Game::LocalParser parser;
        CommandCounter counter;
        setupParser(&parser, &counter, &tokenizer,
                    Game::CommandFlags(Game::CommandQuit));

        parser.setEnabled(false);
        device->write("quit\n");
        TestUtils::waitForSignal(&counter, SIGNAL(ready()), TimeOut);
        QCOMPARE(counter.m_counters.at(2), 0);

        parser.setEnabled(true);
        device->write("quit\n");
        TestUtils::waitForSignal(&counter, SIGNAL(ready()), TimeOut);
        QCOMPARE(counter.m_counters.at(2), 1);
    }
};

QTEST_APPLESS_MAIN(TestLocalParser)
#include ".moc/testlocalparser.moc"
