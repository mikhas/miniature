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

#include "cliparser.h"

#include <iostream>

namespace {
    const QString CmdNew("new");
    const QString CmdQuit("quit");
    const QString CmdMove("move");
    QFutureWatcher<QString> g_input_watcher;
    bool g_waiting_for_input = false;
    bool g_enabled = true;

    QString readFromStdIn()
    {
        std::string cmd;
        getline(std::cin, cmd);
        return QString(cmd.c_str());
    }
}

namespace Game {

CliParser::CliParser(CommandFlags flags,
                     QObject *parent)
    : QObject(parent)
    , m_flags(flags)
{
    connect(&g_input_watcher, SIGNAL(finished()),
            this,             SLOT(onInputReady()));
}

CliParser::~CliParser()
{}

void CliParser::readInput()
{
    if (g_enabled && not g_waiting_for_input) {
        QTimer::singleShot(0, this, SLOT(asyncReadInput())); // wait for mainloop
    }
}

void CliParser::onInputReady()
{
    const QString result(g_input_watcher.future().result().toLower());
    g_waiting_for_input = false;

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
    } else if (g_enabled) {
        QTimer::singleShot(0, this, SLOT(asyncReadInput())); // wait for mainloop
    }
}

void CliParser::setEnabled(bool enable)
{
    g_enabled = enable;
}

void CliParser::asyncReadInput()
{
    g_waiting_for_input = true;
    QFuture<QString> future = QtConcurrent::run(readFromStdIn);
    g_input_watcher.setFuture(future);
}

} // namespace Game
