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
#include "move.h"
#include "cliparser.h"

#include <QtCore>
#include <QtGui>
#include <QtTest>

// Need to import symbol so that slot/signal signature stays same, for moc.
using Game::Move;
Q_DECLARE_METATYPE(Move)
class TestGnuChess
    : public QObject
{
    Q_OBJECT

private:
    QScopedPointer<QSignalSpy> m_spy;
    QScopedPointer<Game::Game> m_game;
    QTextStream m_out;
    bool m_result;

    template<class T>
    void compare(const T &a, const T &b)
    {
        m_result = (m_result && a == b);

        if (a != b) {
            m_out << "FAIL: " << __PRETTY_FUNCTION__ << "\n"
                  << "      " << "got " << a << ", expected: " << b << "\n"; 
            cleanup();
        }
    }

    void cleanup()
    {
        if (m_result) {
            m_out << "SUCCESS\n";
        } else {
            m_out << "FAILURE\n";
        }

        m_out.flush();
        qApp->exit();
    }

    void wait(QObject *obj, const char *signal, int timeout = 5000)
    {
        Q_UNUSED(signal)
        QEventLoop loop;
        connect(obj, signal, &loop, SLOT(quit()));
        QTimer::singleShot(timeout, &loop, SLOT(quit()));
        loop.exec();
    }

public:
    explicit TestGnuChess()
        : QObject()
        , m_out(stdout)
        , m_result(true)
    {
        m_out << "Starting TestGnuChess\n";
    }

    Q_SLOT void run()
    {
        qRegisterMetaType<Move>();
        Game::CliParser::setEnabled(false);

        Game::LocalSide *white = new Game::LocalSide("white");
        Game::GnuChess  *black = new Game::GnuChess("black");
        m_spy.reset(new QSignalSpy(black, SIGNAL(moveEnded(Move))));
        m_game.reset(new Game::Game(white, black));
        compare<Game::AbstractSide *>(m_game->activeSide().data(), white);

        m_game->start();
        emit white->moveEnded(Move(Game::Position(), Game::Square(), Game::Square(),
                                   QString("g4")));
        compare<Game::AbstractSide *>(m_game->activeSide().data(), black);

        wait(black, SIGNAL(moveEnded(Move)));        
        compare<int>(m_spy->count(), 1);
        compare<Game::AbstractSide *>(m_game->activeSide().data(), white);

        emit white->moveEnded(Move(Game::Position(), Game::Square(), Game::Square(),
                                   QString("f4")));
        compare<Game::AbstractSide *>(m_game->activeSide().data(), black);

        wait(black, SIGNAL(moveEnded(Move)));        
        compare<int>(m_spy->count(), 2);
        compare<Game::AbstractSide *>(m_game->activeSide().data(), white);

        cleanup();
    }
};

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);

    TestGnuChess testee;
    QTimer::singleShot(0, &testee, SLOT(run()));

    app.exec();
}

#include ".moc/testgnuchess.moc"
