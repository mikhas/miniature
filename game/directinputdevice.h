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

#ifndef DIRECTINPUTDEVICE_H
#define DIRECTINPUTDEVICE_H

#include <QtCore>
#ifdef Q_OS_LINUX
#include <termios.h>
#else
struct termios {};
#endif

namespace Game {
//! A direct input device for stdin, based on QIODevice. Supports unbuffered,
//! non-blocking read (use QIODevice::Unbuffered | QIODevice::ReadyOnly).
//! Open the device and connect to the readyRead signal, then extract data.
//!
//! Can be used in a semi-direct mode (if opened with QIODevice::ReadyOnly but
//! without QIODevice::Unbuffered), where the readyRead signal is emitted after
//! a timeout (default: 100ms) or after a certain buffer limit (default: 8) is
//! reached. \sa setLimits
class DirectInputDevice
    : public QIODevice
{
    Q_OBJECT
    Q_DISABLE_COPY(DirectInputDevice)

private:
    QByteArray m_buf;
    qint64 m_max_buf_size;
    QTimer m_buf_timer;
    QScopedPointer<QSocketNotifier> m_notifier;
    struct termios m_terminal_attributes;

public:
    //! \reimp
    explicit DirectInputDevice(QObject *parent = 0);
    virtual ~DirectInputDevice();

    //! Opens non-blocking read from stdin.
    //! @param mode the open mode, must contain QIODevice::ReadOnly
    virtual bool open(OpenMode mode);
    virtual void close();
    virtual qint64 readData(char *data,
                            qint64 buf_size);

    //! Writing to stdin is not supported.
    virtual qint64 writeData(const char *,
                             qint64);
    virtual qint64 bytesAvailable() const;
    //! \reimp_end

    //! Sets buffer limit and timeout. Triggers readyRead on whatever is hit
    //! first. Allows for semi-direct input (can be more efficient for larger
    //! streams of data).
    //! @param buf_size the maximum buffer size.
    //! @param timeout the timeout, in ms.
    void setLimits(qint64 buf_size,
                   int timeout);

private:
    //! Gets called whenever new input is available on stdin.
    Q_SLOT void onSocketActivated(int);
};

} // namespace Game

#endif // DIRECTINPUTDEVICE_H
