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

#ifndef TESTUTILS_H
#define TESTUTILS_H

#include "abstractbackend.h"

#include <QtCore>
#include <QtGui>
#include <QtTest>

#define CREATE_EMPTY_DEFAULT_SLOTS \
    Q_SLOT void initTestCase() {} \
    Q_SLOT void cleanupTestCase() {} \
    Q_SLOT void init() {} \
    Q_SLOT void cleanup() {}

namespace TestUtils {

// Wait for signal or timeout. Does not require event loop.
void waitForSignal(const QSignalSpy &spy,
                   int timeout = 1000) {
    QElapsedTimer timer;
    timer.start();
    while (spy.count() == 0 && timer.elapsed() < timeout) {
        QTest::qWait(20);
    }
}

// Requires event loop. Use SIGNAL macro for signal parameter.
void waitForSignal(QObject *obj,
                   const char *signal,
                   int timeout = 1000)
{
    Q_UNUSED(signal)
    QEventLoop loop;
    QObject::connect(obj, signal, &loop, SLOT(quit()));
    QTimer::singleShot(timeout, &loop, SLOT(quit()));
    loop.exec();
}

QApplication *createApp(const QString &app_name)
{
    static int argc = 1; \
    static char* argv[] = { app_name.toLatin1().data() };
    return new QApplication(argc, argv);

}

class DummyBackend
    : public Game::AbstractBackend
{
public:
    explicit DummyBackend(QObject *parent = 0)
        : Game::AbstractBackend(parent)
    {}

    virtual ~DummyBackend()
    {}

    virtual Game::AbstractBackend::State state() const
    {
        return Game::AbstractBackend::StateIdle;
    }

    virtual void login(const QString &,
                       const QString &)
    {
        emit stateChanged(Game::AbstractBackend::StateLoginFailed);
    }

    virtual void setEnabled(bool)
    {}

    virtual void processToken(const QByteArray &)
    {}

    virtual void setFlags(Game::CommandFlags)
    {}
};

} // namespace TestUtils

#endif // TESTUTILS_H
