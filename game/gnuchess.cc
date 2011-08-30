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

#include "gnuchess.h"
#include "commands.h"

namespace {
    const char * const GnuChessCmd = "/usr/games/gnuchess";

    // matches "My move is : Nf6"
    const QRegExp match_move("My move is : (\\w+=)");

    bool kill(Q_PID pid,
              const QString &signal)
    {
        QStringList args;
        args.append("-s");
        args.append(signal);
        args.append(QString("%1").arg(pid));

        return (0 == QProcess::execute("kill", args));
    }
}

namespace Game {

GnuChess::GnuChess(Dispatcher *dispatcher,
                   QObject *parent)
    : AbstractEngine(parent)
    , m_proc()
    , m_dispatcher(dispatcher)
{
    connect(&m_proc, SIGNAL(readyRead()),
            this,    SLOT(onReadyRead()),
            Qt::UniqueConnection);
}

GnuChess::~GnuChess()
{
    m_proc.kill();
    m_proc.waitForFinished();
}

void GnuChess::setEnabled(bool enable)
{
    if (enable) {
        m_proc.start(GnuChessCmd, QIODevice::ReadWrite | QIODevice::Unbuffered);
        m_proc.setReadChannel(QProcess::StandardOutput);

        if (m_proc.state() != QProcess::Running) {
            m_proc.waitForStarted();
        }
    } else {
        m_proc.terminate();
    }
}

void GnuChess::play(uint)
{
    m_proc.write("new\n");
}

void GnuChess::movePiece(const MovedPiece &moved_piece)
{
    m_proc.write(moveNotation(moved_piece).toLatin1());
    m_proc.write("\n");
}

void GnuChess::processToken(const QByteArray &token)
{
    bool valid = match_move.exactMatch(token);
    if (valid) {
        // TODO: parse & send move correctly. Also need to do sth. about the game id.
        Command::Move m(TargetFrontend, 999u, Position());
        sendCommand(&m);
    }
}

void GnuChess::runInBackground()
{
    kill(m_proc.pid(), "SIGTSTP");
}

void GnuChess::runInForeground()
{
    kill(m_proc.pid(), "SIGCONT");
}

void GnuChess::onReadyRead()
{
    while (m_proc.canReadLine()) {
        processToken(m_proc.readLine());
    }
}

void GnuChess::sendCommand(AbstractCommand *command)
{
    if (Dispatcher *dispatcher = m_dispatcher.data()) {
        dispatcher->sendCommand(command);
    }
}

} // namespace Game
