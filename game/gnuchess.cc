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

GnuChessParser::GnuChessParser(QObject *parent)
    : AbstractParser(parent)
{}

GnuChessParser::~GnuChessParser()
{}

void GnuChessParser::setFlags(CommandFlags)
{}

void GnuChessParser::setEnabled(bool)
{}

void GnuChessParser::processToken(const QByteArray &)
{}

GnuChess::GnuChess(const QString &identifier,
                   const SharedParser &parser)
    : AbstractSide(identifier, parser)
    , m_identifier(identifier)
    , m_state(NotReady)
    , m_proc()
    , m_parser(parser)
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

void GnuChess::init()
{
    if (m_proc.state() != QProcess::NotRunning) {
        qWarning() << __PRETTY_FUNCTION__
                   << "Process already running. Did you call init() twice?";
    }

    m_proc.start(GnuChessCmd, QIODevice::ReadWrite | QIODevice::Unbuffered);
    m_proc.setReadChannel(QProcess::StandardOutput);

    if (m_proc.state() != QProcess::Running) {
        m_proc.waitForStarted();
    }

    m_state = Ready;
    emit ready();
}

AbstractSide::SideState GnuChess::state() const
{
    return m_state;
}


const QString &GnuChess::identifier() const
{
    return m_identifier;
}

void GnuChess::runInBackground()
{
    if (m_state == NotReady) {
        return;
    }

    if (kill(m_proc.pid(), "SIGTSTP")) {
        m_state = RunInBackground;
        disconnect(m_parser.data(), SIGNAL(commandFound(Command, QByteArray)),
                   this,            SLOT(onCommandFound(Command, QByteArray)));
    }
}

void GnuChess::runInForeground()
{
    if (m_state != RunInBackground) {
        return;
    }

    if (kill(m_proc.pid(), "SIGCONT")) {
        m_state = Ready;
        connect(m_parser.data(), SIGNAL(commandFound(Command, QByteArray)),
                this,            SLOT(onCommandFound(Command, QByteArray)),
                Qt::UniqueConnection);
    }
}

void GnuChess::startTurn(const Move &move)
{
    m_proc.write(move.notation.toLatin1().append("\n"));
    m_proc.waitForBytesWritten();
}

void GnuChess::onCommandFound(Command,
                              const QByteArray &)
{}

void GnuChess::onReadyRead()
{
    while (m_proc.canReadLine()) {
        QString result(m_proc.readLine());
        if (result.startsWith(CmdMove)) {
            emit turnEnded(Move(Position(), Square(), Square(),
                                QString(result.right(result.size() - CmdMove.size() - 1))));
        }
    }
}

} // namespace Game
