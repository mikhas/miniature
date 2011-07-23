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
    const QString CmdNew("new");
    const QString CmdQuit("quit");
    const QString CmdMove("move");
}

namespace Game {

class LocalParserPrivate
{
public:
    CommandFlags flags;
    SharedTokenizer tokenizer;
    bool enabled;

    explicit LocalParserPrivate(CommandFlags new_flags,
                                const SharedTokenizer &new_tokenizer)
        : flags(new_flags)
        , tokenizer(new_tokenizer)
        , enabled(false)
    {}
};

LocalParser::LocalParser(CommandFlags flags,
                         const SharedTokenizer &tokenizer,
                         QObject *parent)
    : AbstractParser(flags, tokenizer, parent)
    , d_ptr(new LocalParserPrivate(flags, tokenizer))
{
    if (not tokenizer.isNull()) {
        connect(tokenizer.data(), SIGNAL(tokenFound(QByteArray)),
                this,             SLOT(onTokenFound(QByteArray)),
                Qt::UniqueConnection);
    }
}

LocalParser::~LocalParser()
{}

void LocalParser::setEnabled(bool enable)
{
    Q_D(LocalParser);
    d->enabled = enable;

    if (d->enabled && not d->tokenizer.isNull()) {
        d->tokenizer->init();
    }
}

void LocalParser::onTokenFound(const QByteArray &token)
{
    Q_D(const LocalParser);
    if (not d->enabled) {
        return;
    }

    QString result(token);

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
