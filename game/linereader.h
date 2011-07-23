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

#include "abstracttokenizer.h"

#include <QtCore>

namespace Game {

//! Async device reader, tokenizes input into lines.
class LineReader
    : public AbstractTokenizer
{
    Q_OBJECT

private:
    QScopedPointer<QIODevice> m_device;
    QByteArray m_buffer;
    bool m_init;

public:
    //! \reimp
    explicit LineReader(QIODevice *device,
                        QObject *parent = 0);
    virtual ~LineReader();
    void init();
    //! \reimp_end

private:
    //! Processes input from command line and emits lineFound when a line was
    //! read.
    Q_SLOT void onReadyRead();
};

} // namespace Game

#endif // SHAREDLINEREADER_H
