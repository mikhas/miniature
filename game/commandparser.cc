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

#include "commandparser.h"
#include "linereader.h"

namespace {
    const QString CmdNew("new");
    const QString CmdQuit("quit");
    const QString CmdMove("move");

#ifdef MINIATURE_CLI_ENABLED
    // Shared among all CliParser instances, which allows us to filter the same
    // differently, depending on the commands we're interested in.
    Game::LineReader g_line_reader;
#endif
}

namespace Game {

CommandParser::CommandParser(CommandFlags flags,
                             QObject *parent)
    : QObject(parent)
    , m_flags(flags)
    , m_waiting_for_input(false)
{
#ifdef MINIATURE_CLI_ENABLED
    connect(&g_line_reader, SIGNAL(lineFound(QByteArray)),
                this,       SLOT(onLineFound(QByteArray)));
#endif
}

CommandParser::~CommandParser()
{}

void CommandParser::readInput()
{
#ifdef MINIATURE_CLI_ENABLED
    g_line_reader.init();
#endif
    m_waiting_for_input = true;
}

void CommandParser::setInputDevice(const QSharedPointer<QIODevice> &device)
{
#ifndef MINIATURE_CLI_ENABLED
    Q_UNUSED(device)
#else
    g_line_reader.setInputDevice(device);
#endif
}

void CommandParser::onLineFound(const QByteArray &line)
{
    if (not m_waiting_for_input) {
        return;
    }

    QString result(line);

    if ((m_flags & CommandNew)
        && result == CmdNew ) {
        emit commandFound(CommandNew);
    } else if ((m_flags & CommandQuit)
               && result == CmdQuit) {
        emit commandFound(CommandQuit);
    } else if ((m_flags & CommandMove)
               && result.left(CmdMove.size()) == CmdMove) {
        emit commandFound(CommandMove,
                          QString(result.right(result.size() - CmdMove.size() - 1)));
    }
}

} // namespace Game
