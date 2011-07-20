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

#include "directinputdevice.h"

#include <stdio.h>
#include <unistd.h>

namespace Game {

// TODO: Support flexible input buffering - say, timeout vs max buf size, whatever is reached first.
DirectInputDevice::DirectInputDevice(QObject *parent)
    : QIODevice(parent)
    , m_buf()
    , m_notifier()
    , m_terminal_attributes()
{
    tcgetattr(fileno(stdin), &m_terminal_attributes);
}

DirectInputDevice::~DirectInputDevice()
{
    // Restore terminal attributes
    tcsetattr(fileno(stdin), TCSANOW, &m_terminal_attributes);
}

bool DirectInputDevice::open(OpenMode mode)
{
    if (m_notifier.isNull()) {
        m_notifier.reset(new QSocketNotifier(fileno(stdin), QSocketNotifier::Read));
        connect(m_notifier.data(), SIGNAL(activated(int)),
                this,              SLOT(onSocketActivated(int)),
                Qt::UniqueConnection);
    }

    if (mode & QIODevice::ReadOnly) {
        bool result = QIODevice::open(mode);

        // Taken from http://www.dreamincode.net/code/snippet5395.htm
        struct termios attr;
        tcgetattr(fileno(stdin), &attr);
        attr.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(fileno(stdin), TCSANOW, &attr);

        return result;
    }

    return false;
}

void DirectInputDevice::close()
{
    QIODevice::close();

    // Restore terminal attributes & get rid of socket notifier:
    tcsetattr(fileno(stdin), TCSANOW, &m_terminal_attributes);
    m_notifier.reset();
}

qint64 DirectInputDevice::readData(char *data,
                                   qint64 maxSize)
{
    if (not data) {
        return -1;
    }

    if (m_buf.size() == 0) {
        return -1;
    }

    const qint64 cpy_size(qMin<qint64>(maxSize, m_buf.size()));
    const QByteArray &tmp(m_buf.left(cpy_size));
    m_buf.remove(0, cpy_size);
    strcpy(data, tmp.data());
    return tmp.size();
}

qint64 DirectInputDevice::writeData(const char *,
                                    qint64)
{
    return -1;
}

qint64 DirectInputDevice::bytesAvailable() const
{
    return m_buf.size();
}

void DirectInputDevice::onSocketActivated(int)
{
    const int ch = getchar();
    if (ch != EOF) {
        m_buf.append(static_cast<char>(ch));
        emit readyRead();
    } else {
        qDebug() << __PRETTY_FUNCTION__ << "closing shop";
        close();
    }
}

} // namespace Game
