/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2010 Collabora Ltd. <http://www.collabora.co.uk/>
 * @author Dariusz Mikulski <dariusz.mikulski@collabora.co.uk>
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

#ifndef TPTUBESERVER_H
#define TPTUBESERVER_H

#include "tptubeclient.h"
#include <QTcpServer>

namespace TpGame
{

class TubeClient;

class TubeServer
    : public QTcpServer
{
    Q_OBJECT

public:
    TubeServer(QObject *parent = 0);
    TubeClient* getClient();

Q_SIGNALS:
    void disconnected();

protected:
    void incomingConnection(int socketDescriptor);

private Q_SLOTS:
    void removeClient();

private:
    QList<TubeClient*> clients;

};

} // namespace TpGame

#endif // TPTUBESERVER_H
