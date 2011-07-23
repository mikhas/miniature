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

#ifndef FICSSIDE_H
#define FICSSIDE_H

#include "namespace.h"
#include "abstractside.h"

#include <QtCore>

namespace Game
{

//! Implement a backend for FICS (freechess.org).
//! Can be feeded by Game::FicsParser.
class FicsSide
    : public AbstractSide
{
private:
    const QString m_identifier;
    SideState m_state;

public:
    //! \reimp
    explicit FicsSide(const QString &identifier);
    virtual ~FicsSide();
    virtual void init();
    virtual SideState state() const;
    virtual const QString &identifier() const;
    virtual void startMove(const Move &move);
    //! \reimp_end

private:
    //! Handle input from FICS.
    //! @command the found command.
    //! @data the data that belongs to the command.
    Q_SLOT void onCommandFound(Command command,
                               const QString &data);
};

} // namespace Game

#endif // FICSSIDE_H
