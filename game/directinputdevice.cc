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

#ifdef Q_OS_LINUX
#include <stdio.h>
#include <unistd.h>
#endif

namespace Game {

DirectInputDevice::DirectInputDevice(QObject *parent)
    : QIODevice(parent)
    , m_buf()
    , m_max_buf_size(8)
    , m_buf_timer()
    , m_notifier()
    , m_terminal_attributes()
{
#ifdef Q_OS_LINUX
    tcgetattr(fileno(stdin), &m_terminal_attributes);

    m_buf_timer.setInterval(100);
    m_buf_timer.setSingleShot(true);
    connect(&m_buf_timer, SIGNAL(timeout()),
            this,         SIGNAL(readyRead()));
#endif
}

DirectInputDevice::~DirectInputDevice()
{
#ifdef Q_OS_LINUX
    // Restore terminal attributes
    tcsetattr(fileno(stdin), TCSANOW, &m_terminal_attributes);
#endif
}

bool DirectInputDevice::open(OpenMode mode)
{
#ifndef Q_OS_LINUX
    Q_UNUSED(mode)
#else
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

#endif
    return false;
}

void DirectInputDevice::close()
{
#ifdef Q_OS_LINUX
    QIODevice::close();

    // Restore terminal attributes & get rid of socket notifier:
    tcsetattr(fileno(stdin), TCSANOW, &m_terminal_attributes);
    m_notifier.reset();
#endif
}

qint64 DirectInputDevice::readData(char *data,
                                   qint64 buf_size)
{
#ifndef Q_OS_LINUX
    Q_UNUSED(data)
    Q_UNUSED(buf_size)
#else
    if (not data) {
        return -1;
    }

    if (m_buf.size() == 0) {
        return -1;
    }

    const qint64 cpy_size(qMin<qint64>(buf_size, m_buf.size()));
    const QByteArray &tmp(m_buf.left(cpy_size));
    m_buf.remove(0, cpy_size);
    strcpy(data, tmp.data());
    return tmp.size();
#endif
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

void DirectInputDevice::setLimits(qint64 buf_size,
                                  int timeout)
{
    m_max_buf_size = buf_size;
    m_buf_timer.setInterval(timeout);
}

void DirectInputDevice::onSocketActivated(int)
{
#ifdef Q_OS_LINUX
    const int ch = getchar();
    if (ch != EOF) {
        m_buf.append(static_cast<char>(ch));

        if (openMode() & QIODevice::Unbuffered) {
            emit readyRead();
        } else if (m_buf.size() >= m_max_buf_size) {
                m_buf_timer.stop();
                emit readyRead();
        } else if (not m_buf_timer.isActive()) {
            m_buf_timer.start();
        }
    } else {
        qDebug() << __PRETTY_FUNCTION__ << "closing shop";
        close();
    }
#endif
}

} // namespace Game
