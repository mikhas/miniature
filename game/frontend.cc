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

#include "frontend.h"
#include "commands/logincommand.h"
#include "commands/logoutcommand.h"
#include "directinputdevice.h"

namespace Game { namespace {
    const CommandFlags all_commands(CommandNew | CommandQuit | CommandLogin
                                    | CommandSeek | CommandJoin | CommandObserve
                                    | CommandMove);
}

Frontend::Frontend(Dispatcher *dispatcher,
                   QObject *parent)
    : QObject(parent)
    , m_dispatcher(dispatcher)
    , m_command_line()
    , m_line_reader()
{
    m_command_line.setFlags(all_commands);
    connect(&m_command_line, SIGNAL(commandFound(Command,QByteArray)),
            this,            SLOT(onCommandFound(Command,QByteArray)),
            Qt::UniqueConnection);

    connect(&m_line_reader, SIGNAL(tokenFound(QByteArray)),
            &m_command_line, SLOT(processToken(QByteArray)),
            Qt::UniqueConnection);
}

Frontend::~Frontend()
{}

void Frontend::show()
{
    m_line_reader.init(new DirectInputDevice);
    m_command_line.setEnabled(true);

    QTextStream out(stdout);
    out << "Welcome to Miniature!\n";
}

void Frontend::onCommandFound(Command cmd,
                              const QByteArray &data)
{
    Dispatcher *dispatcher = m_dispatcher.data();

    switch(cmd) {
    case CommandLogin: {
        QList<QByteArray> list = data.split(' ');
        const QString username(data.isEmpty() ? "guest" : list.at(0));
        const QString password(list.size() > 1 ? list.at(1) : "");

        if (dispatcher) {
            LoginCommand login(TargetBackendFics, username, password);
            dispatcher->sendCommand(&login);
        }
    } break;

    case CommandJoin:
        break;

    case CommandNew:
        break;

    case CommandQuit: {
        if (dispatcher) {
            LogoutCommand logout(TargetBackendFics);
            dispatcher->sendCommand(&logout);
        }
        qApp->quit();
    } break;

    default:
        break;
    }
}

} // namespace Game
