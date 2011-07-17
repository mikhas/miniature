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
}

namespace Game {

GnuChess::GnuChess(const QString &identifier)
    : AbstractSide(identifier)
    , m_identifier(identifier)
    , m_state(NotReady)
    , m_proc()
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

    qDebug() << __PRETTY_FUNCTION__;
}

AbstractSide::SideState GnuChess::state() const
{
    return m_state;
}


const QString &GnuChess::identifier() const
{
    return m_identifier;
}

void GnuChess::startMove(const Move &move)
{
    Q_UNUSED(move)
}

void GnuChess::onReadyRead()
{
    while (m_proc.canReadLine()) {
        qWarning() << __PRETTY_FUNCTION__ << __LINE__;
        qWarning() << m_proc.readLine();
    }
}

} // namespace Game
