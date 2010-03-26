/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2010 Collabora Ltd. <http://www.collabora.co.uk/>
 *              Dariusz Mikulski <dariusz.mikulski@collabora.co.uk>
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
#include "tptubeclient.h"

namespace Miniature
{

TpTubeServer::TpTubeServer(QObject *parent)
    : QTcpServer(parent)
{
    qDebug() << "TpTubeServer::TpTubeServer()";
}

void TpTubeServer::incomingConnection(int socketDescriptor)
{
    qDebug() << "TpTubeServer::incomingConnection()";

    TpTubeClient *client = new TpTubeClient(this);
    if(client->setSocketDescriptor(socketDescriptor))
    {
        QObject::connect(client,
            SIGNAL(error(QAbstractSocket::SocketError)),
            SLOT(removeClient()));
        clients << client;
        return;
    }

    client->abort();
    delete client;
}

void TpTubeServer::removeClient()
{
    qDebug() << "TpTubeServer::removeClient()";

    TpTubeClient *client = qobject_cast<TpTubeClient*>(sender());
    client->deleteLater();
    clients.removeAll(client);
}

};
