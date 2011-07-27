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

#ifndef ABSTRACTLINK_H
#define ABSTRACTLINK_H

#include "abstractparser.h"
#include <QtCore>

namespace Game {

class AbstractLink;
typedef QSharedPointer<AbstractLink> SharedLink;

//! Extends AbstractParser into a bidirectional link. Can be used to transmit
//! and receive commands from a remote server.
class AbstractLink
    : public AbstractParser
{
    Q_OBJECT
    Q_DISABLE_COPY(AbstractLink)

public:
    //! The link's state.
    enum State {
        StateIdle,
        StateLoginPending,
        StateLoginFailed,
        StateReady,
    };

    Q_ENUMS(State)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)

    //! \reimp
    explicit AbstractLink(QObject *parent = 0);
    virtual ~AbstractLink() = 0;
    //! \reimp_end

    //! Returns state of this link.
    virtual State state() const = 0;

    //! Emitted whenever state changes.
    Q_SIGNAL void stateChanged(State state);

    //! Login to remote server.
    //! @param username the username of the account.
    //! @param password the password of the account.
    virtual void login(const QString &username,
                       const QString &password) = 0;
};

} // namespace Game

#endif // ABSTRACTLINK_H
