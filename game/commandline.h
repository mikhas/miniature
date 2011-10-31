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

#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include "namespace.h"
#include "abstractengine.h"

#include <QtCore>

namespace Game {

//! Available game commands, used for example by command line parser.
enum ParserCommand
{
    CommandNone = 0, //!< Can be used as a default value or for unrecognized commands.
    CommandPlay = 0x1,
    CommandMove = 0x2,
    CommandQuit = 0x4,
    CommandLogin = 0x8,
    CommandSeek = 0x10,
    CommandJoin = 0x20,
    CommandObserve = 0x40
};

Q_DECLARE_FLAGS(ParserCommandFlags, ParserCommand)

class CommandLinePrivate;
class Dispatcher;
class AbstractCommand;

//! Reads input from local input device and translates it into commands.
//! Used for local sides and local command line interface.
class CommandLine
    : public AbstractEngine
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(CommandLine)
    Q_DISABLE_COPY(CommandLine)

private:
    const QScopedPointer<CommandLinePrivate> d_ptr;

public:
    //! \reimp
    explicit CommandLine(QObject *parent = 0);
    virtual ~CommandLine();
    virtual void setDispatcher(Dispatcher *dispatcher);
    virtual void setEnabled(bool enable);
    virtual void readData(const QByteArray &token);
    virtual void writeData(const QByteArray &data);
    //! \reimp_end

    virtual void setFlags(ParserCommandFlags flags);

private:
    void sendCommand(AbstractCommand *command);
};

} // namespace Game

Q_DECLARE_METATYPE(Game::ParserCommandFlags)

#endif // COMMANDLINE_H
