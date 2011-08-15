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

namespace {
    const char * const GnuChessCmd = "/usr/games/gnuchess";
    const QString CmdMove("My move is :");

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

GnuChess::GnuChess(const QString &identifier)
    : Side(identifier)
    , m_proc()
{
    connect(&m_proc, SIGNAL(readyRead()),
            this,    SLOT(onReadyRead()),
            Qt::UniqueConnection);

    m_proc.start(GnuChessCmd, QIODevice::ReadWrite | QIODevice::Unbuffered);
    m_proc.setReadChannel(QProcess::StandardOutput);

    if (m_proc.state() != QProcess::Running) {
        m_proc.waitForStarted();
    }
}

GnuChess::~GnuChess()
{
    m_proc.kill();
    m_proc.waitForFinished();
}

void GnuChess::runInBackground()
{
    kill(m_proc.pid(), "SIGTSTP");
}

void GnuChess::runInForeground()
{
    kill(m_proc.pid(), "SIGCONT");
}

void GnuChess::startTurn(const Position &result,
                         const MovedPiece &moved_piece)
{
    m_proc.write(moveNotation(result, moved_piece).toLatin1());
    m_proc.write("\n");
    m_proc.waitForBytesWritten();
}

void GnuChess::onReadyRead()
{
    while (m_proc.canReadLine()) {
        QString result(m_proc.readLine());
        if (result.startsWith(CmdMove)) {
            emit turnEnded(Position(), MovedPiece());
        }
    }
}

} // namespace Game
