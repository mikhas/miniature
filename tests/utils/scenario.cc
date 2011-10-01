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

#include "scenario.h"

#include <abstractengine.h>

namespace TestUtils {

Scenario::Scenario(Game::AbstractEngine *engine)
    : m_engine(engine)
    , m_data()
    , m_count(0)
{}


void Scenario::play(PlayMode mode)
{
    if (not m_engine) {
        return;
    }

    for (; m_count < m_data.size(); ++m_count) {
        const QByteArray &token(m_data.at(m_count));

        if (not token.isEmpty() && token.at(0) == '#') {
            continue;
        }

        if (token == QByteArray("\n") && mode == PauseAtNewlines) {
            return;
        }

        m_engine->processToken(token);
    }
}

void Scenario::setData(const QVector<QByteArray> &data)
{
    m_data = data;
}

} // namespace Miniature
