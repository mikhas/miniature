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

#ifndef LOCALSIDE_H
#define LOCALSIDE_H

#include "abstractside.h"

#include <QtCore>

namespace Game {

//! Represents a local, human-controlled side.
class LocalSide
    : public AbstractSide
{
    Q_OBJECT

private:
    const QString m_identifier;

public:
    //! \reimp
    explicit LocalSide(const QString &identifier);

    //! \reimp
    virtual ~LocalSide();

    //! \reimp
    virtual const QString &identifier() const;

    //! \reimp
    virtual void startMove(const Move &move);
};

} // namespace Game

#endif // LOCALSIDE_H
