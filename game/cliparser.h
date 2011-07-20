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

#ifndef CLIPARSER_H
#define CLIPARSER_H

#include <QtCore>

namespace Game {

enum Command {
    CommandNone = 0,
    CommandNew = 0x1,
    CommandMove = 0x2,
    CommandQuit = 0x4
};

Q_ENUMS(Command)
Q_DECLARE_FLAGS(CommandFlags, Command)

//! Listens on stdin and parses for commands.
class CliParser
    : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(CliParser)

private:
    CommandFlags m_flags;
    bool m_waiting_for_input;

public:
    explicit CliParser(CommandFlags flags,
                       QObject *parent = 0);
    virtual ~CliParser();

    //! Reads input from command line, non-blocking.
    void readInput();

    Q_SIGNAL void commandFound(Command cmd,
                               const QString &data = QString());

    //! Sets shared input device for all parser instances, defaults to
    //! a non-blocking reading device for standard input. An empty shared
    //! pointer will disable all input handling.
    //! @param device the shared device.
    static void setInputDevice(const QSharedPointer<QIODevice> &device);

private:
    Q_SLOT void onLineFound(const QByteArray &line);
};

} // namespace Game

#endif // CLIPARSER_H
