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

#ifndef TPINCOMINGTUBE_H
#define TPINCOMINGTUBE_H

#include <QObject>
#include <QTcpSocket>

#include <TelepathyQt4/Channel>
#include <TelepathyQt4/Types>

#include "tphelpers.h"

namespace TpGame
{

class TubeClient;

class TpIncomingTube : public QObject
{
    Q_OBJECT
public:
    TpIncomingTube(const Tp::ChannelPtr &channel, QObject *parent = 0);
    ~TpIncomingTube();

Q_SIGNALS:
    void tubeReady(TpGame::TubeClient *, const Tp::ContactPtr &);
    void readyToBeDeleted();

private Q_SLOTS:
    void onChannelReady(Tp::PendingOperation *);
    void onTubeChannelStateChanged(uint);
    void onTcpSocketConnected();
    void onTcpSocketError(QAbstractSocket::SocketError);
    void onTcpSocketDisconnected();

private:
    Tp::ChannelPtr mChannel;
    Tp::Client::ChannelTypeStreamTubeInterface *mStreamTubeInterface;
    Tp::Client::ChannelInterfaceTubeInterface *mTubeInterface;
    Tp::ContactPtr mContact;
    QTcpSocket *mTcpSocket;
    StreamTubeAddress mStreamTubeAddress;
    TubeClient *mClient;
};

} // namespace TpGame

#endif // TPINCOMINGTUBE_H
