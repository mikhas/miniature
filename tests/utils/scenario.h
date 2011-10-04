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

#ifndef SCENARIO_H
#define SCENARIO_H

#include <QtCore>

namespace Game {
    class AbstractEngine;
}

namespace TestUtils {

class Scenario
{
public:
    enum Result {
        Passed,
        Failed
    };

private:
    Game::AbstractEngine *m_engine;
    QVector<QByteArray> m_data;
    QByteArray m_expected_response;
    int m_count;
    Result m_result;

public:
    explicit Scenario(Game::AbstractEngine *engine);

    void play(const QByteArray &response = QByteArray());
    void setData(const QVector<QByteArray> &data);
    Result result() const;
};

} // namespace TestUtils

#endif // SCENARIO_H
