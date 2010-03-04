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

#include "tptubesclienthandler.h"
#include "tphelpers.h"

#include <TelepathyQt4/Constants>
#include <TelepathyQt4/Debug>
#include <TelepathyQt4/Channel>
#include <TelepathyQt4/Account>
#include <TelepathyQt4/PendingReady>

#include <QDBusVariant>

namespace Miniature
{

static inline Tp::ChannelClassList channelClassList()
{
    QMap<QString, QDBusVariant> filter0;

    filter0[TELEPATHY_INTERFACE_CHANNEL ".ChannelType"] =
            QDBusVariant(TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAM_TUBE);
    filter0[TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAM_TUBE ".Service"] = QDBusVariant("Miniature");

    return Tp::ChannelClassList() << Tp::ChannelClass(filter0);
}

TpTubesClientHandler::TpTubesClientHandler(QObject *parent)
    : QObject(parent),
    Tp::AbstractClientHandler(channelClassList(), false)
{
    qDebug() << "TpTubesClientHandler::TpTubesClientHandler()";

    Tp::enableDebug(false);
    Tp::enableWarnings(true);

    Tp::registerTypes();
}

TpTubesClientHandler::~TpTubesClientHandler()
{
    qDebug() << "TpTubesClientHandler::~TpTubesClientHandler()";
}

bool TpTubesClientHandler::bypassApproval() const
{
    qDebug() << "TpTubesClientHandler::bypassApproval()";

    return false;
}

void TpTubesClientHandler::handleChannels(const Tp::MethodInvocationContextPtr<> &context,
                                          const Tp::AccountPtr &account,
                                          const Tp::ConnectionPtr &connection,
                                          const QList<Tp::ChannelPtr> &channels,
                                          const QList<Tp::ChannelRequestPtr> &requestedSatisfied,
                                          const QDateTime &userActionTime,
                                          const QVariantMap &handlerInfo)
{
    qDebug() << "TpTubesClientHandler::handleChannels()";

    Q_UNUSED(account);
    Q_UNUSED(connection);
    Q_UNUSED(requestedSatisfied);
    Q_UNUSED(userActionTime);
    Q_UNUSED(handlerInfo);

    Q_FOREACH(const Tp::ChannelPtr &channel, channels)
    {
        qDebug() << "!!! Channel: " << channel;

        QVariantMap properties = channel->immutableProperties();

        if(properties[TELEPATHY_INTERFACE_CHANNEL ".ChannelType"] ==
            TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAM_TUBE)
        {
            qDebug() << "!!! Channel is a stream tube. Handling: " << channel;

            if(properties[TELEPATHY_INTERFACE_CHANNEL ".Requested"].toBool())
            {
                qDebug() << "Outgoing channel !";
            }
            else
            {
                qDebug() << "Incoming channel !";
            }

            mChannel = channel;

            Tp::Features features;
            features << Tp::Channel::FeatureCore;
            connect(mChannel->becomeReady(features), SIGNAL(finished(Tp::PendingOperation *)), this, SLOT(onChannelReady(Tp::PendingOperation *)));
        }
    }

    context->setFinished();

    qDebug() << "!!! handleChannels DONE !!!";
}

void TpTubesClientHandler::onChannelReady(Tp::PendingOperation *op)
{
    qDebug() << "TpTubesClientHandler::onChannelReady()";

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

    Tp::Client::ChannelTypeStreamTubeInterface *streamTubeInterface = mChannel->streamTubeInterface();
    Tp::Client::ChannelInterfaceTubeInterface *tubeInterface = mChannel->tubeInterface();

    if(streamTubeInterface && tubeInterface)
    {
        qDebug() << "We have tube control interfaces. Continue.";

        QDBusVariant ret = streamTubeInterface->Accept(
                Tp::SocketAddressTypeIPv4,
                Tp::SocketAccessControlLocalhost,
                QDBusVariant(""));

        StreamTubeAddress streamTubeAddress = qdbus_cast<StreamTubeAddress>(ret.variant());
        qDebug () << "Stream tube address:"
                << streamTubeAddress.address
                << ":"
                << streamTubeAddress.port;
    }

    qDebug() << "? ERROR ?";
}

};
