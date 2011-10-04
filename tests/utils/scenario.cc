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

namespace {
    const QByteArray wait_for_input("WAIT_FOR_INPUT");
    const QByteArray wait_for_newline("WAIT_FOR_NEWLINE");
}

namespace TestUtils {

Scenario::Scenario(Game::AbstractEngine *engine)
    : m_engine(engine)
    , m_data()
    , m_expected_response()
    , m_count(0)
    , m_result(Passed)
{}

void Scenario::play(const QByteArray &response)
{
    if (not m_engine) {
        return;
    }

    if (response != m_expected_response) {
        m_result = Failed;
        return;
    }

    for (; m_count < m_data.size(); ++m_count) {
        const QByteArray &token(m_data.at(m_count));

        if (not token.isEmpty() && token.at(0) == '#') {
            continue;
        }

        if (token.startsWith(wait_for_input)) {
            m_expected_response = token.mid(wait_for_input.length() + 1);
            ++m_count;
            return;
        } else if (token.startsWith(wait_for_newline)) {
            m_expected_response = QByteArray("\n");
            ++m_count;
            return;
        }

        m_engine->processToken(token);
    }
}

void Scenario::setData(const QVector<QByteArray> &data)
{
    m_data = data;
}

Scenario::Result Scenario::result() const
{
    return m_result;
}

bool Scenario::finished() const
{
    return (m_count + 1 == m_data.size() || m_result == Failed);
}

} // namespace Miniature
