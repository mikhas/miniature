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

namespace Miniature
{

TpOutgoingTube::TpOutgoingTube(const Tp::ChannelPtr &channel, QObject *parent)
    : QObject(parent),
    mChannel(channel)
{
    qDBusRegisterMetaType<StreamTubeAddress>();

    Tp::Features features;
    features << Tp::Channel::FeatureCore;
    connect(mChannel->becomeReady(features),
            SIGNAL(finished(Tp::PendingOperation *)),
            SLOT(onChannelReady(Tp::PendingOperation *)));
}

TpOutgoingTube::~TpOutgoingTube()
{

}

void TpOutgoingTube::onChannelReady(Tp::PendingOperation *op)
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
                SLOT(onTubeStateChanged(uint)));

        QVariantMap parameters;

        StreamTubeAddress streamTubeAddress;
        streamTubeAddress.address = QHostAddress(QHostAddress::LocalHost).toString();
        streamTubeAddress.port = 1234;

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

void TpOutgoingTube::onChannelInvalidated(Tp::DBusProxy *proxy, const QString &errorName, const QString &errorMessage)
{
    Q_UNUSED(proxy);

    qDebug() << "Tube channel invalidated:" << errorName << errorMessage;

    Q_EMIT readyToBeDeleted();
}

void TpOutgoingTube::onTubeStateChanged(uint state)
{
    qDebug() << "Tube state changed:" << state;

    Q_EMIT statusChanged(state);
}

void TpOutgoingTube::onOfferTubeFinished(QDBusPendingCallWatcher *watcher)
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

void TpOutgoingTube::onNewRemoteConnection(uint handle, QDBusVariant connectionParam, uint connectionId)
{
    QVariant v = connectionParam.variant();
    qDebug() << "variant =" << v << "handle:" << handle << "connectionId:" << connectionId;

    Tp::SocketAddressIPv4 ipv4address = qdbus_cast<Tp::SocketAddressIPv4>(v);

    qDebug() << "NewRemoteConnection:" << ipv4address.port << ipv4address.address;

    // \todo:
    // Start server listening!
}


};
