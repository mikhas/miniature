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
#include <termios.h>

namespace Game {

class DirectInputDevice
    : public QIODevice
{
    Q_OBJECT
    Q_DISABLE_COPY(DirectInputDevice)

private:
    QByteArray m_buf;
    QScopedPointer<QSocketNotifier> m_notifier;
    struct termios m_terminal_attributes;

public:
    //! \reimp
    explicit DirectInputDevice(QObject *parent = 0);
    virtual ~DirectInputDevice();

    //! \reimp
    //! Opens non-blocking read from stdin.
    //! @param mode the open mode, must contain QIODevice::ReadOnly
    virtual bool open(OpenMode mode);

    //! \reimp
    virtual void close();

    //! \reimp
    virtual qint64 readData(char *data,
                            qint64 maxSize);
    //! \reimp
    //! Writing to stdin is not supported.
    virtual qint64 writeData(const char *,
                             qint64);

    //! \reimp
    qint64 bytesAvailable() const;

private:
    //! Gets called whenever new input is available on stdin.
    Q_SLOT void onSocketActivated(int socket);
};

} // namespace Game

#endif // DIRECTINPUTDEVICE_H
