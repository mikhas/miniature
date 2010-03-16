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

#include "tpincomingtube.h"

#include <TelepathyQt4/PendingOperation>
#include <TelepathyQt4/PendingReady>

namespace Miniature
{

TpIncomingTube::TpIncomingTube(const Tp::ChannelPtr &channel, QObject *parent)
    : QObject(parent),
    mChannel(channel),
    mStreamTubeInterface(0),
    mTubeInterface(0),
    mTcpSocket(0)
{
    qDebug() << "TpIncomingTube::TpIncomingTube()";

    Tp::Features features;
    features << Tp::Channel::FeatureCore;

    connect(mChannel->becomeReady(features),
            SIGNAL(finished(Tp::PendingOperation *)),
            SLOT(onChannelReady(Tp::PendingOperation *)));
}

TpIncomingTube::~TpIncomingTube()
{
    qDebug() << "TpIncomingTube::~TpIncomingTube()";
}

void TpIncomingTube::onChannelReady(Tp::PendingOperation *op)
{
    qDebug() << "TpIncomingTube::onChannelReady()";

    if(op->isError())
    {
        qDebug() << "Channel ready error";
        return;
    }

    mTubeInterface = mChannel->tubeInterface();
    mStreamTubeInterface = mChannel->streamTubeInterface();

    if(!mStreamTubeInterface || !mTubeInterface)
    {
        qDebug() << "We don't have tube interfaces!";
        return;
    }

    connect(mTubeInterface,
            SIGNAL(TubeChannelStateChanged(uint)),
            SLOT(onTubeChannelStateChanged(uint)));

    QDBusVariant ret = mStreamTubeInterface->Accept(
            Tp::SocketAddressTypeIPv4,
            Tp::SocketAccessControlLocalhost,
            QDBusVariant(""));

    mStreamTubeAddress = qdbus_cast<StreamTubeAddress>(ret.variant());

    qDebug() << "Stream Tube address:"
             << mStreamTubeAddress.address
             << ":"
             << mStreamTubeAddress.port;

    mTcpSocket = new QTcpSocket;

    connect(mTcpSocket,
            SIGNAL(connected()),
            SLOT(onTcpSocketConnected()));
    connect(mTcpSocket,
            SIGNAL(error(QAbstractSocket::SocketError)),
            SLOT(onTcpSocketError(QAbstractSocket::SocketError)));
    connect(mTcpSocket,
            SIGNAL(disconnected()),
            SLOT(onTcpSocketDisconnected()));

    mTcpSocket->connectToHost(mStreamTubeAddress.address, mStreamTubeAddress.port);
}

void TpIncomingTube::onTubeChannelStateChanged(uint state)
{
    qDebug() << "TpIncomingTube::onTubeChannelStateChanged()";

    if(state != Tp::TubeStateOpen)
        return;

    Q_EMIT tubeReady(mTcpSocket, mContact);
}

void TpIncomingTube::onTcpSocketConnected()
{
    qDebug() << "TpIncomingTube::onTcpSocketConnected()";
}

void TpIncomingTube::onTcpSocketError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);

    qDebug() << "TpIncomingtube::onTcpSocketError()";

    qDebug() << "Tcp socket error:" << mTcpSocket->errorString();

    Q_EMIT readyToBeDeleted();
}

void TpIncomingTube::onTcpSocketDisconnected()
{
    qDebug() << "TpIncomingTube::onTcpSocketDisconnected()";

    qDebug() << "Tcp socket disconnected!";

    Q_EMIT readyToBeDeleted();
}

}

