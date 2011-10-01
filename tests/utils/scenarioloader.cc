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

#include "scenarioloader.h"

namespace TestUtils {

bool loadFromFile(const QString &fileName,
                  QVector<QByteArray> *data)
{
    QFile f(fileName);

    if(not data || not f.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray line = f.readLine();
    while (not line.isNull()) {
        data->append(line.trimmed());
        line = f.readLine();
    }

    return (not data->isEmpty());
}

ScenarioLoader::ScenarioLoader()
{}

Scenario ScenarioLoader::load(Game::AbstractEngine *engine,
                              const QString &name)
{
    Q_UNUSED(name)

    QVector<QByteArray> data;
    QString filename = QString("%1/%2.log")
        .arg(TEST_SCENARIOS_DIR)
        .arg(name);

    if (not loadFromFile(filename, &data)) {
        qWarning() << __PRETTY_FUNCTION__
                   << "Invalid scenario name:" << name
                   << "- unable to load from file:" << filename;
    }

    Scenario sc(engine);
    sc.setData(data);

    return sc;
}

} // namespace TestUtils
