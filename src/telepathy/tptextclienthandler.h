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

#ifndef TPTEXTCLIENTHANDLER_H
#define TPTEXTCLIENTHANDLER_H

#include <TelepathyQt4/AbstractClientHandler>
#include <TelepathyQt4/PendingOperation>
#include <TelepathyQt4/Channel>

#include <QTcpSocket>

namespace Miniature
{

class TpOutgoingTube;
class TpIncomingTube;

class TpTextClientHandler : public QObject, public Tp::AbstractClientHandler
{
    Q_OBJECT
public:
    TpTextClientHandler(QObject *parent = 0);
    virtual ~TpTextClientHandler();

    virtual bool bypassApproval() const;
    virtual void handleChannels(const Tp::MethodInvocationContextPtr<> &context,
                                const Tp::AccountPtr &account,
                                const Tp::ConnectionPtr &connection,
                                const QList<Tp::ChannelPtr> &channels,
                                const QList<Tp::ChannelRequestPtr> &requestedSatisfied,
                                const QDateTime &userActionTime,
                                const QVariantMap &handlerInfo);
};

};

#endif // TPTEXTCLIENTHANDLER_H
