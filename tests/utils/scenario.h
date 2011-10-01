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
private:
    Game::AbstractEngine *m_engine;
    QVector<QByteArray> m_data;
    uint m_count;

public:
    enum PlayMode {
        PauseAtNewlines,
        SkipNewlines
    };

    explicit Scenario(Game::AbstractEngine *engine);

    //! Skip comments and by default, pauses at each newline, continue with another call
    //! play.
    void play(PlayMode mode = PauseAtNewlines);
    void setData(const QVector<QByteArray> &data);
};

} // namespace TestUtils

#endif // SCENARIO_H
