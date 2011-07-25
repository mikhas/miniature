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

#include "abstracttokenizer.h"
#include "linereader.h"
#include "directinputdevice.h"

namespace Game
{

const QByteArray scanLine(int *newline_pos,
                          QIODevice *device,
                          QByteArray *buffer,
                          bool echo_enabled,
                          QVector<char> extra_delimiter)
{
    const static int max_read = 2 << 12;

    if (not newline_pos || not device || not buffer) {
        return QByteArray();
    }

    if (echo_enabled) {
        static QTextStream out(stdout);
        const QByteArray tmp(device->read(max_read));
        out << tmp;
        out.flush();
        buffer->append(tmp);
    } else {
        buffer->append(device->read(max_read));
    }

    QByteArray line;
    int index = 0;
    bool found_cr = false;
    bool found_lf = false;
    for (; index < buffer->length(); ++index) {
        char curr = buffer->at(index);

        if (found_cr || curr == '\n' || extra_delimiter.contains(curr)) {
            found_lf = true;
            break;
        }

        if (curr == '\r') {
            found_cr = true;
            continue;
        }

        found_cr = false;
        line.append(curr);
    }

    if (not found_lf) {
        *newline_pos = -1;
        return QByteArray();
    }

    *newline_pos = index;

    // It's possible that buffer only contains CR or LF, in which case we still
    // want to remove one (or two) extra leading characters from buffer:
    buffer->remove(0, index > 1 ? (index + (found_cr ? 2 : 1)) : 1);
    return line;
}

AbstractTokenizer *createLocalInputTokenizer()
{
    return new Game::LineReader(new Game::DirectInputDevice);
}

AbstractTokenizer::AbstractTokenizer(QIODevice *,
                                     QObject *parent)
    : QObject(parent)
{}

AbstractTokenizer::~AbstractTokenizer()
{}

} // namespace Game
