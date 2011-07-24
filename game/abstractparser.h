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

#ifndef ABSTRACTPARSER_H
#define ABSTRACTPARSER_H

#include "namespace.h"
#include "abstracttokenizer.h"

#include <QtCore>

namespace Game {

//! Command parser interface.
//!
//! Retreives input from tokenizer (async) and translates it into commands.
//! Clients of this interface can list the commands they're interested in
//! through the AbstractCommpandParser's c'tor (derived classes need to honor
//! the command filtering). To retrieve commands, clients hook up to the
//! commandFound signal.
class AbstractParser
    : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(AbstractParser)

public:
    //! C'tor
    //! @param flags the commands that should be filtered. Other commands will
    //!        be ignored by this instance.
    //! @param tokenizer the (shared) tokenizer for this parser. Multiple
    //!                  parsers can be attached to the same tokenizer stream.
    //! @param parent the owner of this instance (optional).
    explicit AbstractParser(CommandFlags flags,
                            const SharedTokenizer &tokenizer,
                            QObject *parent = 0);
    virtual ~AbstractParser() = 0;

    //! Enables command parsing. Initializes input device backend for tokenizer
    //! if required.
    //! @param enable whether to enable command parsing.
    virtual void setEnabled(bool enable) = 0;

    //! Emitted when a command was found in tokenizer stream.
    //! @param cmd the found command.
    //! @param data the data for this command.
    Q_SIGNAL void commandFound(Command cmd,
                               const QByteArray &data = QByteArray());
};

} // namespace Game

#endif // ABSTRACTPARSER_H
