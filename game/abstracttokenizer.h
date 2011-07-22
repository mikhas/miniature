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

#ifndef ABSTRACTTOKENIZER_H
#define ABSTRACTTOKENIZER_H

#include <QtCore>

namespace Game {

class AbstractTokenizer;
typedef QSharedPointer<AbstractTokenizer> SharedTokenizer;

//! Async device reader, tokenizes input into lines.
class AbstractTokenizer
    : public QObject
{
    Q_OBJECT

public:
    //! C'tor
    //! @param device the input device, takes ownership.
    //! @param parent the owner of this instance (optional).
    explicit AbstractTokenizer(QIODevice *device,
                               QObject *parent = 0);
    virtual ~AbstractTokenizer() = 0;

    //! Initiates the source, starts reading from input device.
    virtual void init() = 0;

    //! Emitted when a token was found.
    //! @param token the token that was found.
    Q_SIGNAL void tokenFound(const QByteArray &token);
};

} // namespace Game

#endif // ABSTRACTTOKENIZER_H
