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

#include <QtCore>

namespace Game {

//! Command parser interface. Can retrieve input from command line or graphical
//! user interface and translate input into proper commands. Each AbstractSide
//! backend might require its own parser implementation.
//!
//! For streamed input, it can make sense to use an AbstractTokenizer. Simply
//! connect a tokenizer's tokenFound signal to the processToken slot.
//! For each instance, the range of accepted commands can be specified through
//! setFlags. If two instances accept the same command - say, while reading
//! from the same tokenizer - then both instances may emit a commandFound,
//! should the command be found in the common input stream.
//! To retrieve commands, clients hook up to the commandFound signal.
//! A graphical user interface can inject commands through processToken.
class AbstractParser
    : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(AbstractParser)

public:
    //! C'tor
    //! @param parent the owner of this instance (optional).
    explicit AbstractParser(QObject *parent = 0);

    virtual ~AbstractParser() = 0;

    //! Specify commands that should be handled by this instance.
    //! @param flags the command flags.
    virtual void setFlags(CommandFlags flags) = 0;

    //! Enables command parsing. Initializes input device backend for tokenizer
    //! if required.
    //! @param enable whether to enable command parsing.
    virtual void setEnabled(bool enable) = 0;

    //! Emitted when a command was found in tokenizer stream.
    //! @param cmd the found command.
    //! @param data the data for this command.
    Q_SIGNAL void commandFound(Command cmd,
                               const QByteArray &data = QByteArray());

    //! Processes a token. Graphical user interface may want to call this
    //! method directly.
    //! @param token the token to be processed. If the token could be
    //!              translated into a command, commandFound will be emitted.
    Q_SLOT virtual void processToken(const QByteArray &token) = 0;
};

} // namespace Game

#endif // ABSTRACTPARSER_H
