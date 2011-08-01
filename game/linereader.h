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

#ifndef LINEREADER_H
#define LINEREADER_H

#include <QtCore>

namespace Game {

//! Scans and returns a line from device, filling up buffer with partial reads.
//! Newlines are normalized: CR => LF, CRLF => LF, LF => LF. This only affects
//! the buffer as the returned scanned line will never return newlines.
//! @param newline_pos the position of the next newline read from device,
//!                    or -1 if none found.
//! @param device the IO device which provides the input.
//! @param buffer the buffer to fill up with partial reads (that is, if no
//!               newline was found so far). Whenever a line is scaned, the
//!               line is removed from the buffer.
//! @param echo_enabled whether to display read data on stdout.
const QByteArray scanLine(int *newline_pos,
                          QIODevice *device,
                          QByteArray *buffer,
                          bool echo_enabled = false,
                          QVector<char> extra_delimiter = QVector<char>());

//! Async device reader, tokenizes input into lines.
class LineReader
    : public QObject
{
    Q_OBJECT

private:
    QScopedPointer<QIODevice> m_device;
    QByteArray m_buffer;
    bool m_init;

public:
    //! C'tor
    //! @param parent the owner of this instance (optional).
    explicit LineReader(QObject *parent = 0);
    virtual ~LineReader();

    //! Initiates the source, starts reading from input device.
    //! @param device the input device, takes ownership.
    virtual void init(QIODevice *device);

    //! Emitted when a token was found.
    //! @param token the token that was found.
    Q_SIGNAL void tokenFound(const QByteArray &token);

private:
    //! Processes input from command line and emits lineFound when a line was
    //! read.
    Q_SLOT void onReadyRead();
};

} // namespace Game

#endif // LINEREADER_H
