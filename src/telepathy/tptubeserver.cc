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

#include "tptubeserver.h"

namespace TpGame
{

TubeServer::TubeServer(QObject *parent)
    : QTcpServer(parent)
{
    qDebug() << "TpTubeServer::TpTubeServer()";
}

void TubeServer::incomingConnection(int socketDescriptor)
{
    qDebug() << "TpTubeServer::incomingConnection()";

    TubeClient *client = new TubeClient(this);
    if(client->setSocketDescriptor(socketDescriptor))
    {
        connect(client, SIGNAL(error(QAbstractSocket::SocketError)),
                this,   SLOT(removeClient()),
                Qt::UniqueConnection);

        clients << client;
        return;
    }

    client->abort();
    delete client;
}

void TubeServer::removeClient()
{
    qDebug() << "TpTubeServer::removeClient()";

    TubeClient *client = static_cast<TubeClient*>(sender());
    client->deleteLater();
    clients.removeAll(client);
    Q_EMIT disconnected();
}

TubeClient* TubeServer::getClient()
{
    if(clients.isEmpty())
        return 0;

    return clients.first();
}

} // namespace TpGame
