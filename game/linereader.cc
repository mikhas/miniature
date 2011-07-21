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

#include "linereader.h"
#include "directinputdevice.h"

namespace Game
{

LineReader::LineReader(QObject *parent)
    : QObject(parent)
    , m_device(new DirectInputDevice)
    , m_buffer()
    , m_init(false)
{}

LineReader::~LineReader()
{
    if (not m_device.isNull()) {
        m_device->close();
    }
}

void LineReader::init()
{
    if (m_init || m_device.isNull()) {
        return;
    }

    m_init = true;
    connect(m_device.data(), SIGNAL(readyRead()),
            this,            SLOT(onReadyRead()));

    m_device->open(QIODevice::ReadOnly | QIODevice::Text | QIODevice::Unbuffered);
}

void LineReader::setInputDevice(const QSharedPointer<QIODevice> &device)
{
    if (m_device == device) {
        return;
    }

    if (not m_device.isNull()) {
        disconnect(m_device.data(), SIGNAL(readyRead()), this, 0);
    }

    m_device->close();
    m_device = device;

    if (m_device.isNull()) {
        return;
    }

    // re-init
    m_init = false;
    init();
}

void LineReader::onReadyRead()
{
    static QTextStream out(stdout);
    if (m_device.isNull()) {
        return;
    }

    int next_newline_pos = -1;
    do {
        const QByteArray tmp(m_device->read(256));
        out << tmp;
        out.flush();

        m_buffer.append(tmp);
        next_newline_pos = m_buffer.indexOf('\n');
        QByteArray line(m_buffer.left(next_newline_pos));
        m_buffer.remove(0, next_newline_pos == -1 ? -1 : next_newline_pos + 1);

        if (not line.isEmpty()) {
            emit lineFound(line);
        }
    } while (next_newline_pos != -1);
}

} // namespace Game
