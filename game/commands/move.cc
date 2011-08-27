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

#include "move.h"
#include "frontend/miniature.h"
#include "game.h"

namespace Game { namespace Command {

Move::Move(Target target,
           uint game_id,
           const Position &result)
    : AbstractCommand(target)
    , m_target(target)
    , m_game_id(game_id)
    , m_result(result)
    , m_white()
    , m_black()
{}

Move::~Move()
{}

Target Move::target() const
{
    return m_target;
}

void Move::exec(AbstractEngine *target)
{
    if (not target) {
        return;
    }

    qDebug() << __PRETTY_FUNCTION__
             << moveNotation(m_result.movedPiece());
    target->movePiece(m_result.movedPiece());
}

void Move::exec(Frontend::Miniature *target)
{
    if (not target) {
            return;
    }

    if (Game *g = target->activeGame()) {
        qDebug() << __PRETTY_FUNCTION__
                 << moveNotation(m_result.movedPiece());
        g->setPosition(m_result);
    }
}

uint Move::gameId() const
{
    return m_game_id;
}

Position Move::result() const
{
    return m_result;
}

void Move::setWhite(const PlayerRecord &player_record)
{
    m_white = player_record;
}

PlayerRecord Move::white() const
{
    return m_white;
}

void Move::setBlack(const PlayerRecord &player_record)
{
    m_black = player_record;
}

PlayerRecord Move::black() const
{
    return m_black;
}

}} // namespace Command, Game

