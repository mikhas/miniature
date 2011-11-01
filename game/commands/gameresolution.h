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

#ifndef GAMERESOLUTION_H
#define GAMERESOLUTION_H

#include "abstractcommand.h"
#include "namespace.h"

#include <QtCore>

namespace Game { namespace Command {

//! Used to propose and accept game resolutions.
class GameResolution
    : public AbstractCommand
{
public:
    enum Activity {
        Proposed,
        Accepted,
        Rejected
    };

private:
    Activity m_activity;
    Resolution m_resolution;

public:
    //! \reimp
    explicit GameResolution(Target target,
                            Activity activity,
                            Resolution resolution);
    virtual ~GameResolution();
    virtual void exec(Dispatcher *dispatcher,
                      AbstractEngine *target);
    virtual void exec(Dispatcher *dispatcher,
                      Frontend::Miniature *target);
    //! \reimp_end
};

}} // namespace Command, Game

#endif // GAMERESOLUTION_H
