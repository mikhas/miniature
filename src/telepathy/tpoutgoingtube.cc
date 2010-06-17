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

#include "tpoutgoingtube.h"
#include "tphelpers.h"

#include <TelepathyQt4/PendingReady>

#include <QHostAddress>
#include <QNetworkInterface>

namespace TpGame
{

OutgoingTube::OutgoingTube(const Tp::ChannelPtr &channel, QObject *parent)
    : QObject(parent),
    mChannel(channel)
{
    qDebug() << "TpOutgoingTube::TpOutgoingTube()";
    qDBusRegisterMetaType<StreamTubeAddress>();

    Tp::Features features;
    features << Tp::Channel::FeatureCore;
    connect(mChannel->becomeReady(features),
            SIGNAL(finished(Tp::PendingOperation *)),
            SLOT(onChannelReady(Tp::PendingOperation *)));
}

OutgoingTube::~OutgoingTube()
{
    qDebug() << "TpOutgoingTube::~TpOutgoingTube()";
}

void OutgoingTube::onChannelReady(Tp::PendingOperation *op)
{
    qDebug() << "TpOutgoingTube::onChannelReady()";

    if(op->isError())
    {
        qDebug() << "Channel ready error: " << op->errorName() << op->errorMessage();
        return;
    }

    Tp::Contacts contacts = mChannel->groupContacts();

    Q_FOREACH(Tp::ContactPtr contact, contacts)
    {
        qDebug() << contact->id();
    }

    // upgrade contacts from contact manager ?
    connect(mChannel.data(),
            SIGNAL(invalidated(Tp::DBusProxy *, const QString &, const QString &)),
            SLOT(onChannelInvalidated(Tp::DBusProxy *, const QString &, const QString &)));

    mTubeInterface = mChannel->tubeInterface();
    mStreamTubeInterface = mChannel->streamTubeInterface();

    if(mStreamTubeInterface && mTubeInterface)
    {
        qDebug() << "We have tube control interfaces. Offering tube.";

        connect(mTubeInterface,
                SIGNAL(TubeChannelStateChanged(uint)),
                SLOT(onTubeChannelStateChanged(uint)));

        if(!server.listen())
        {
            qDebug() << "Error server listening";
            return;
        }

        QVariantMap parameters;

        QString ipAddress;
        QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
        // use the first non-localhost IPv4 address
        for (int i = 0; i < ipAddressesList.size(); ++i) {
            if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
                ipAddressesList.at(i).toIPv4Address()) {
                ipAddress = ipAddressesList.at(i).toString();
                break;
            }
        }
        // if we did not find one, use IPv4 localhost
        if (ipAddress.isEmpty())
            ipAddress = QHostAddress(QHostAddress::LocalHost).toString();

        StreamTubeAddress streamTubeAddress;
        streamTubeAddress.address = ipAddress;
        streamTubeAddress.port = server.serverPort();

        qDebug() << "Server address:" << streamTubeAddress.address << "port:" << streamTubeAddress.port;

        QDBusVariant address;
        address.setVariant(qVariantFromValue(streamTubeAddress));

        qDebug() << "Address qdbusvariant:" << address.variant().toString();

        QDBusPendingReply<> ret = mStreamTubeInterface->Offer(
                uint(Tp::SocketAddressTypeIPv4),
                address,
                uint(Tp::SocketAccessControlPort),
                parameters);

        qDebug() << "Parameters:" << parameters;

        connect(new QDBusPendingCallWatcher(ret, this),
                SIGNAL(finished(QDBusPendingCallWatcher *)),
                SLOT(onOfferTubeFinished(QDBusPendingCallWatcher *)));
        connect(mStreamTubeInterface,
                SIGNAL(NewRemoteConnection(uint, QDBusVariant, uint)),
                SLOT(onNewRemoteConnection(uint, QDBusVariant, uint)));
    }
    else
    {
        qDebug() << "? ERROR ?";
    }
}

void OutgoingTube::onChannelInvalidated(Tp::DBusProxy *proxy, const QString &errorName, const QString &errorMessage)
{
    Q_UNUSED(proxy);

    qDebug() << "Tube channel invalidated:" << errorName << errorMessage;

    Q_EMIT readyToBeDeleted();
}

void OutgoingTube::onTubeChannelStateChanged(uint state)
{
    qDebug() << "OutgoingTube::onTubeChannelStateChanged()";
    qDebug() << "Tube state changed:" << state;

    if(state != Tp::TubeStateOpen)
        return;

    Q_EMIT tubeReady(server.getClient(), mContact);

    Q_EMIT statusChanged(state);
}

void OutgoingTube::onOfferTubeFinished(QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply<void> reply = *watcher;

    if(reply.isError())
    {
        qDebug() << "Offer tube failed:" << reply.error().message() << "Error name:" << reply.error().name() << "Error string:" << reply.error().errorString(reply.error().type());
        Q_EMIT readyToBeDeleted();
        return;
    }

    qDebug() << "Offer tube succeeded !!!!";
}

void OutgoingTube::onNewRemoteConnection(uint handle, QDBusVariant connectionParam, uint connectionId)
{
    QVariant v = connectionParam.variant();
    qDebug() << "variant =" << v << "handle:" << handle << "connectionId:" << connectionId;

    //Tp::SocketAddressIPv4 ipv4address = qdbus_cast<Tp::SocketAddressIPv4>(v);

    //qDebug() << "NewRemoteConnection:" << ipv4address.port << ipv4address.address;

    // \todo:
    // Start server listening!
}

} // namespace TpGame
