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

LineReader::LineReader(QIODevice *device,
                       QObject *parent)
    : AbstractTokenizer(device, parent)
    , m_device(device)
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

void LineReader::onReadyRead()
{
    if (m_device.isNull()) {
        return;
    }

    int next_newline_pos = -1;
    const bool enable_echo = true;
    do {
        const QByteArray line(scanLine(&next_newline_pos, m_device.data(), &m_buffer, enable_echo));

        if (not line.isEmpty()) {
            emit tokenFound(line);
        }
    } while (next_newline_pos != -1);
}

} // namespace Game
