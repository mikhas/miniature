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

#include "commandline.h"
#include "dispatcher.h"
#include "commands.h"
#include "registry.h"

namespace {
    typedef QHash<Game::ParserCommand, QByteArray> CommandLookupTable;
    CommandLookupTable createCommandLoopupTable()
    {
        CommandLookupTable result;
        result.insert(Game::CommandPlay, "play");
        result.insert(Game::CommandQuit, "quit");
        result.insert(Game::CommandMove, "move");
        result.insert(Game::CommandLogin, "login");

        return result;
    }

    const CommandLookupTable cmd_lookup_table = createCommandLoopupTable();
}

namespace Game {

class CommandLinePrivate
{
public:
    WeakDispatcher dispatcher;
    Registry registry;
    ParserCommandFlags flags;
    bool enabled;

    explicit CommandLinePrivate(Dispatcher *new_dispatcher)
        : dispatcher(new_dispatcher)
        , registry(new_dispatcher)
        , flags(CommandNone)
        , enabled(false)
    {}

    bool isCommand(ParserCommand cmd,
                   const QByteArray &token) const
    {
        const QByteArray &val(cmd_lookup_table.value(cmd));
        return ((flags & cmd) && token == val);
    }

    bool startsWithCommand(ParserCommand cmd,
                           const QByteArray &token,
                           QByteArray *val) const
    {
        *val = cmd_lookup_table.value(cmd);
        return ((flags & cmd) && token.left(val->size()) == *val);
    }

    QByteArray extractData(const QByteArray &val,
                           const QByteArray &token) const
    {
        return token.right(token.size() - val.size() - 1);
    }
};

CommandLine::CommandLine(Dispatcher *dispatcher,
                         QObject *parent)
    : AbstractEngine(parent)
    , d_ptr(new CommandLinePrivate(dispatcher))
{}

CommandLine::~CommandLine()
{}

void CommandLine::setFlags(ParserCommandFlags flags)
{
    Q_D(CommandLine);
    d->flags = flags;
}

void CommandLine::setEnabled(bool enable)
{
    Q_D(CommandLine);
    d->enabled = enable;
}

void CommandLine::processToken(const QByteArray &token)
{
    Q_D(CommandLine);
    if (not d->enabled) {
        return;
    }

    // Byte representation of command:
    QByteArray value;

    if (d->startsWithCommand(CommandMove, token, &value)) {
        const QByteArray &data(d->extractData(value, token));
        // FIXME: Convert into move command, use proper game id?
        qDebug() << __PRETTY_FUNCTION__ << data;
        Command::Move move(TargetRegistry, 0, Position());
        sendCommand(&move);
    } else if (d->startsWithCommand(CommandPlay, token, &value)) {
        const QByteArray &data(d->extractData(value, token));

        // We leave it to the specific backend to deal with invalid
        // advertisement id's, it's optional anyway.
        // TODO: Impl createGame command for CLI?
        d->registry.registerGame(createGame(0, d->dispatcher.data(), "local", "remote"));
        Command::Play play(TargetRegistry, data.toUInt());
        sendCommand(&play);
    } else if (d->isCommand(CommandQuit, token)) {
        Command::Logout logout(TargetEngine);
        sendCommand(&logout);
        qApp->quit();
    } else if (d->startsWithCommand(CommandLogin, token, &value)) {
        const QByteArray &data(d->extractData(value, token));
        const QList<QByteArray> &list(data.split(' '));

        const QString username(data.isEmpty() ? "guest" : list.at(0));
        const QString password(list.size() > 1 ? list.at(1) : "");

        Command::Login login(TargetEngine, username, password);
        sendCommand(&login);
    }
}

void CommandLine::sendCommand(AbstractCommand *command)
{
    Q_D(CommandLine);
    if (Dispatcher *dispatcher = d->dispatcher.data()) {
        dispatcher->sendCommand(command);
    }
}

} // namespace Game
