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

#include "localparser.h"

namespace {
    typedef QHash<Game::Command, QByteArray> CommandLookupTable;
    CommandLookupTable createCommandLoopupTable()
    {
        CommandLookupTable result;
        result.insert(Game::CommandNew, "new");
        result.insert(Game::CommandQuit, "quit");
        result.insert(Game::CommandMove, "move");
        result.insert(Game::CommandLogin, "login");

        return result;
    }

    const CommandLookupTable cmd_lookup_table = createCommandLoopupTable();
}

namespace Game {

class LocalParserPrivate
{
public:
    CommandFlags flags;
    bool enabled;

    explicit LocalParserPrivate()
        : flags(CommandNone)
        , enabled(false)
    {}

    bool isCommand(Command cmd,
                   const QByteArray &token) const
    {
        const QByteArray &val(cmd_lookup_table.value(cmd));
        return ((flags & cmd) && token == val);
    }

    bool startsWithCommand(Command cmd,
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

LocalParser::LocalParser(QObject *parent)
    : AbstractParser(parent)
    , d_ptr(new LocalParserPrivate)
{}

LocalParser::~LocalParser()
{}

void LocalParser::setFlags(CommandFlags flags)
{
    Q_D(LocalParser);
    d->flags = flags;
}

void LocalParser::setEnabled(bool enable)
{
    Q_D(LocalParser);
    d->enabled = enable;
}

void LocalParser::processToken(const QByteArray &token)
{
    Q_D(const LocalParser);
    if (not d->enabled) {
        return;
    }

    // Byte representation of command:
    QByteArray value;

    if (d->startsWithCommand(CommandMove, token, &value)) {
        emit commandFound(CommandMove, d->extractData(value, token));
    } else if (d->isCommand(CommandNew, token)) {
        emit commandFound(CommandNew);
    } else if (d->isCommand(CommandQuit, token)) {
        emit commandFound(CommandQuit);
    } else if (d->startsWithCommand(CommandLogin, token, &value)) {
        emit commandFound(CommandLogin, d->extractData(value, token));
    }
}

} // namespace Game
