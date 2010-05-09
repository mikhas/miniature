/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2010 Collabora Ltd. <http://www.collabora.co.uk/>
 *              Mateu Batle <mateu.batle@collabora.co.uk>
 * Based on code from telepathy protocol in kopete (Collabora copyright)
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

#include "tptextclienthandler.h"
#include "tphelpers.h"
#include "tpchatsession.h"

#include <TelepathyQt4/Constants>
#include <TelepathyQt4/Debug>
#include <TelepathyQt4/Account>
#include <TelepathyQt4/PendingReady>
#include <TelepathyQt4/TextChannel>

#include <QDBusVariant>

namespace TpGame
{

static inline Tp::ChannelClassList channelClassList()
{
    QMap<QString, QDBusVariant> filter0;

    filter0[TELEPATHY_INTERFACE_CHANNEL ".ChannelType"] =
            QDBusVariant(TELEPATHY_INTERFACE_CHANNEL_TYPE_TEXT);
    filter0[TELEPATHY_INTERFACE_CHANNEL ".TargetHandleType"] = QDBusVariant(Tp::HandleTypeContact);

    return Tp::ChannelClassList() << Tp::ChannelClass(filter0);
}

TextClientHandler::TextClientHandler(QObject *parent)
    : QObject(parent),
    Tp::AbstractClientHandler(channelClassList(), false)
{
    qDebug() << "TpTextClientHandler::TpTextClientHandler()";

    Tp::enableDebug(false);
    Tp::enableWarnings(true);

    Tp::registerTypes();
}

TextClientHandler::~TextClientHandler()
{
    qDebug() << "TpTextClientHandler::~TpTextClientHandler()";
}

bool TextClientHandler::bypassApproval() const
{
    qDebug() << "TpTextClientHandler::bypassApproval()";

    return true;
}

void TextClientHandler::handleChannels(const Tp::MethodInvocationContextPtr<> &context,
                                          const Tp::AccountPtr &account,
                                          const Tp::ConnectionPtr &connection,
                                          const ChannelList &channels,
                                          const ChannelRequestList &requested_satisfied,
                                          const QDateTime &user_action_time,
                                          const QVariantMap &handler_info)
{
    qDebug() << "TpTextClientHandler::handleChannels()";

    Q_UNUSED(account);
    Q_UNUSED(connection);
    Q_UNUSED(requested_satisfied);
    Q_UNUSED(user_action_time);
    Q_UNUSED(handler_info);

    if (channels.size() != 1) {
        // Not expected more than one channel here
        qDebug() << "Got multiple channels to handle - currently not supported";
        context->setFinishedWithError(TELEPATHY_ERROR_NOT_IMPLEMENTED,
                "Handling multiple channels is currently not implemented");
        return;
    }

    Tp::ChannelPtr channel = channels[0];

    qDebug() << "!!! Channel: " << channel;

    QVariantMap properties = channel->immutableProperties();

    qDebug() << "properties of the channel " << properties;

    if(properties[TELEPATHY_INTERFACE_CHANNEL ".ChannelType"] ==
        TELEPATHY_INTERFACE_CHANNEL_TYPE_TEXT)
    {
        qDebug() << "!!! Channel is type text. Handling: " << channel;

        QString contactId;
        if (properties[TELEPATHY_INTERFACE_CHANNEL ".Requested"].toBool())
            contactId = properties[TELEPATHY_INTERFACE_CHANNEL ".TargetID"].toString();
        else
            contactId = properties[TELEPATHY_INTERFACE_CHANNEL ".InitiatorID"].toString();

        qDebug() << "Handling step1:" << contactId;

        QObject * parent = qobject_cast<QObject*>(account.data());
        ChatSession * chatSession = new ChatSession(parent, account);

        Tp::TextChannelPtr textChannel =
            Tp::TextChannelPtr(qobject_cast<Tp::TextChannel*>(channel.data()));
        chatSession->setTextChannel(textChannel);

        context->setFinished();

    } else {
        qDebug() << "Not expected channel type";
        context->setFinishedWithError(TELEPATHY_ERROR_NOT_IMPLEMENTED,
                "Not expected channel type");
        return;
    }

    qDebug() << "!!! handleChannels DONE !!!";
}

} // namespace TpGame
