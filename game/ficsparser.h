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

#ifndef FICSPARSER_H
#define FICSPARSER_H

#include "abstractparser.h"

namespace Game
{

class FicsParserPrivate;

class FicsParser
    : public AbstractParser
{
    Q_OBJECT
    Q_DISABLE_COPY(FicsParser)
    Q_DECLARE_PRIVATE(FicsParser)

private:
    const QScopedPointer<FicsParserPrivate> d_ptr;

public:
    explicit FicsParser(CommandFlags flags,
                        const SharedTokenizer &tokenizer,
                        QObject *parent = 0);
    virtual ~FicsParser();
    virtual void setEnabled(bool enable);

private:
    Q_SLOT void onTokenFound(const QByteArray &token);
};

} // namespace Game

#endif // FICSPARSER_H
