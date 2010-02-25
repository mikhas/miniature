/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2009 Collabora Ltd. <http://www.collabora.co.uk/>
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

#include <TelepathyQt4/Constants>
#include <TelepathyQt4/Debug>
#include <TelepathyQt4/Channel>

#include <QDBusVariant>

namespace Miniature
{

static inline Tp::ChannelClassList channelClassList()
{
    QMap<QString, QDBusVariant> filter0;

    filter0[TELEPATHY_INTERFACE_CHANNEL ".ChannelType"] =
            QDBusVariant(TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAM_TUBE);
    filter0[TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAM_TUBE ".Service"] = QDBusVariant("Miniature");
    filter0[TELEPATHY_INTERFACE_CHANNEL ".Requested"] = QDBusVariant(true);

    return Tp::ChannelClassList() << Tp::ChannelClass(filter0);
}

TpTubesClientHandler::TpTubesClientHandler()
    : Tp::AbstractClientHandler(channelClassList(), false)
{
    Tp::enableDebug(true);
    Tp::enableWarnings(true);

    Tp::registerTypes();
}

TpTubesClientHandler::~TpTubesClientHandler()
{

}

bool TpTubesClientHandler::bypassApproval() const
{
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
    Q_UNUSED(account);
    Q_UNUSED(connection);
    Q_UNUSED(requestedSatisfied);
    Q_UNUSED(userActionTime);
    Q_UNUSED(handlerInfo);

    Q_FOREACH(const Tp::ChannelPtr &channel, channels)
    {
        qDebug() << "Incoming channel: " << channel;

        QVariantMap properties = channel->immutableProperties();

        if(properties[TELEPATHY_INTERFACE_CHANNEL ".ChannelType"] ==
            TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAM_TUBE)
        {
            qDebug() << "Channel is a stream tube. Handling: " << channel;
        }
    }

    context->setFinished();
}

};
