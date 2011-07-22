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

namespace {
    const QString CmdNew("new");
    const QString CmdQuit("quit");
    const QString CmdMove("move");
}

namespace Game {

class CommandParserPrivate
{
public:
    CommandFlags flags;
    SharedTokenizer tokenizer;
    bool enabled;

    explicit CommandParserPrivate(CommandFlags new_flags,
                                  const SharedTokenizer &new_tokenizer)
        : flags(new_flags)
        , tokenizer(new_tokenizer)
        , enabled(false)
    {}
};

CommandParser::CommandParser(CommandFlags flags,
                             const SharedTokenizer &tokenizer,
                             QObject *parent)
    : QObject(parent)
    , d_ptr(new CommandParserPrivate(flags, tokenizer))
{
    if (not tokenizer.isNull()) {
        connect(tokenizer.data(), SIGNAL(tokenFound(QByteArray)),
                this,             SLOT(onTokenFound(QByteArray)));
    }
}

CommandParser::~CommandParser()
{}

void CommandParser::setEnabled(bool enable)
{
    Q_D(CommandParser);
    d->enabled = enable;

    if (d->enabled && not d->tokenizer.isNull()) {
        d->tokenizer->init();
    }
}

void CommandParser::onTokenFound(const QByteArray &line)
{
    Q_D(const CommandParser);
    if (not d->enabled) {
        return;
    }

    QString result(line);

    if ((d->flags & CommandNew)
        && result == CmdNew ) {
        emit commandFound(CommandNew);
    } else if ((d->flags & CommandQuit)
               && result == CmdQuit) {
        emit commandFound(CommandQuit);
    } else if ((d->flags & CommandMove)
               && result.left(CmdMove.size()) == CmdMove) {
        emit commandFound(CommandMove,
                          QString(result.right(result.size() - CmdMove.size() - 1)));
    }
}

} // namespace Game
