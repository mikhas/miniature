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

#include "gameended.h"
#include "frontend/miniature.h"
#include "frontend/messagelog.h"
#include "dispatcher.h"
#include "destroygame.h"

namespace Game { namespace Command {

GameEnded::GameEnded(Target target,
                     uint game_id,
                     Result result,
                     Reason reason,
                     const QByteArray &player_name)
    : AbstractCommand(target)
    , m_game_id(game_id)
    , m_result(result)
    , m_reason(reason)
    , m_player_name(player_name)
{}

GameEnded::~GameEnded()
{}

void GameEnded::exec(Dispatcher *dispatcher,
                     AbstractEngine *target)
{
    if (not dispatcher || not target) {
        return;
    }

    target->endGame(m_reason);

    Command::DestroyGame dg(TargetRegistry, m_game_id);
    dispatcher->sendCommand(&dg);
}

void GameEnded::exec(Dispatcher *dispatcher,
                     Frontend::Miniature *target)
{
    if (not dispatcher || not target) {
        return;
    }

    // QML still has issues with signals and enums?
    // https://bugreports.qt.nokia.com/browse/QTBUG-15983
    emit target->gameEnded(static_cast<int>(m_result), static_cast<int>(m_reason));

    // Game ended, clear message log
    if (Frontend::MessageLog *log = target->messageLog()) {
        log->removeAll();
    }

    target->setActiveGame(0);
    Command::DestroyGame dg(TargetRegistry, m_game_id);
    dispatcher->sendCommand(&dg);
}

}} // namespace Command, Game
