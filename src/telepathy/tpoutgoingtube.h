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

#ifndef TPOUTGOINGTUBE_H
#define TPOUTGOINGTUBE_H

#include "tptubeserver.h"

#include <QObject>
#include <QTcpSocket>

#include <TelepathyQt4/Channel>
#include <TelepathyQt4/Types>
#include <TelepathyQt4/PendingOperation>
#include <TelepathyQt4/Constants>


namespace TpGame
{

class OutgoingTube
    : public QObject
{
    Q_OBJECT

public:
    explicit OutgoingTube(const Tp::ChannelPtr &channel, QObject *parent = 0);
    virtual ~OutgoingTube();

Q_SIGNALS:
    void tubeReady(QTcpSocket *socket, const Tp::ContactPtr &);
    void statusChanged(uint);
    void readyToBeDeleted();

private Q_SLOTS:
    void onChannelReady(Tp::PendingOperation *);
    void onChannelInvalidated(Tp::DBusProxy *, const QString &, const QString &);
    void onOfferTubeFinished(QDBusPendingCallWatcher *);
    void onNewRemoteConnection(uint, QDBusVariant, uint);
    void onTubeStateChanged(uint);

private:
    Tp::ChannelPtr mChannel;
    Tp::Client::ChannelInterfaceTubeInterface *mTubeInterface;
    Tp::Client::ChannelTypeStreamTubeInterface *mStreamTubeInterface;
    TubeServer server;
};

} // namespace TpGame

#endif // TPOUTGOINGTUBE_H
