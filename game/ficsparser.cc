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

#include "ficsparser.h"

namespace Game
{

class FicsParserPrivate
{
public:
    CommandFlags flags;
    SharedTokenizer tokenizer;
    bool enabled;

    explicit FicsParserPrivate(CommandFlags new_flags,
                               const SharedTokenizer &new_tokenizer)
        : flags(new_flags)
        , tokenizer(new_tokenizer)
        , enabled(false)
    {}
};

FicsParser::FicsParser(CommandFlags flags,
                       const SharedTokenizer &tokenizer,
                       QObject *parent)
    : AbstractParser(flags, tokenizer, parent)
    , d_ptr(new FicsParserPrivate(flags, tokenizer))
{
    if (not tokenizer.isNull()) {
        connect(tokenizer.data(), SIGNAL(tokenFound(QByteArray)),
                this,             SLOT(processToken(QByteArray)),
                Qt::UniqueConnection);
    }
}

FicsParser::~FicsParser()
{}

void FicsParser::setEnabled(bool enable)
{
    Q_D(FicsParser);
    d->enabled = enable;
}

void FicsParser::processToken(const QByteArray &token)
{
    emit commandFound(CommandNone, token);
}

}
