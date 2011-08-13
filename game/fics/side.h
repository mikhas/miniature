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

#ifndef FICS_SIDE_H
#define FICS_SIDE_H

#include "namespace.h"
#include "abstractside.h"
#include "abstractbackend.h"
#include "dispatcher.h"

#include <QtCore>
#include <QtNetwork/QTcpSocket>

namespace Game { namespace Fics {

//! A game side implementation that uses the FICS backend.
class Side
    : public AbstractSide
{
    Q_OBJECT
    Q_DISABLE_COPY(Side)

private:
    const QString m_identifier;
    const QString m_password;
    SideState m_state;
    SharedBackend m_link;

public:
    //! C'tor
    //! @param identifier the identifier for this side.
    //! @param link the shared FICS link.
    explicit Side(const QString &identifier,
                  const SharedBackend &link);

    //! \reimp
    virtual ~Side();
    virtual void init();
    virtual SideState state() const;
    virtual const QString &identifier() const;
    virtual void runInBackground();
    virtual void runInForeground();
    virtual void startTurn(const Move &move);
    virtual void onCommandFound(ParserCommand command,
                                const QByteArray &data);
    //! \reimp_end
};

} // namespace Fics
} // namespace Game

#endif // FICS_SIDE_H
