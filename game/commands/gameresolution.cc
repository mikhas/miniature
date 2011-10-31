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

#include "gameresolution.h"
#include "abstractengine.h"
#include "frontend/miniature.h"

namespace Game { namespace Command {

GameResolution::GameResolution(Target target,
                               Activity activity,
                               Resolution resolution)
    : AbstractCommand(target)
    , m_activity(activity)
    , m_resolution(resolution)
{}

GameResolution::~GameResolution()
{}

void GameResolution::exec(Dispatcher *,
                          AbstractEngine *target)
{
    if (not target) {
        return;
    }

    switch(m_activity) {
    case Accepted: target->acceptGameResolution(m_resolution);
        break;
    case Proposed: target->proposeGameResolution(m_resolution);
        break;
    }
}

void GameResolution::exec(Dispatcher *,
                          Frontend::Miniature *target)
{
    if (not target) {
        return;
    }

    switch(m_activity) {
    case Accepted: emit target->gameResolutionAccepted(m_resolution);
        break;
    case Proposed: emit target->gameResolutionProposed(m_resolution);
        break;
    }
}

}} // namespace Command, Game
