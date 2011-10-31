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

#include "abstractengine.h"

namespace Game {

GameInfo::GameInfo()
    : id(0)
    , valid(false)
    , white()
    , black()
    , rating(RatingAny)
    , mode(ModeNone)
    , time(0)
    , time_increment(0)
{}

AbstractEngine::AbstractEngine(QObject *parent)
    : QObject(parent)
{}

AbstractEngine::~AbstractEngine()
{}

void AbstractEngine::login(const QString &,
                           const QString &)
{}

void AbstractEngine::logout()
{}

void AbstractEngine::seek(uint, uint, Rating, Color)
{}

void AbstractEngine::play(uint)
{}

void AbstractEngine::endGame(Reason)
{}

void AbstractEngine::proposeGameResolution(Resolution)
{}

void AbstractEngine::acceptGameResolution(Resolution)
{}

void AbstractEngine::movePiece(const MovedPiece &)
{}

void AbstractEngine::sendMessage(const QByteArray &,
                                 const QByteArray &)
{}

} // namespace Game
