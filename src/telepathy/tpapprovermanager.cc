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

#include "tpapprovermanager.h"
#include "tpapprover.h"

#include <QDBusObjectPath>
#include <QDBusConnection>

#include <TelepathyQt4/Connection>
#include <TelepathyQt4/PendingOperation>
#include <TelepathyQt4/PendingReady>
#include <TelepathyQt4/Debug>
#include <TelepathyQt4/Constants>

namespace TpGame
{

static inline Tp::ChannelClassList channelClassList()
{
    QMap<QString, QDBusVariant> filter0;

    filter0[TELEPATHY_INTERFACE_CHANNEL ".ChannelType"] =
            QDBusVariant(TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAM_TUBE);
    filter0[TELEPATHY_INTERFACE_CHANNEL ".TargetHandleType"] =
            QDBusVariant((uint) Tp::HandleTypeContact);
    filter0[TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAM_TUBE ".Service"] = QDBusVariant("Miniature");
    filter0[TELEPATHY_INTERFACE_CHANNEL ".Requested"] = QDBusVariant(false);

    return Tp::ChannelClassList() << Tp::ChannelClass(filter0);
}

TpApproverManager::TpApproverManager(QObject *parent) : QObject(parent), Tp::AbstractClientApprover(channelClassList())
{
    qDebug() << "TpApproverManager::TpApproverManager()";
}

TpApproverManager::~TpApproverManager()
{
    qDebug() << "TpApproverManager::~TpApproverManager()";
}

void TpApproverManager::addDispatchOperation(const Tp::MethodInvocationContextPtr<> &context,
                                             const QList<Tp::ChannelPtr> &channels,
                                             const Tp::ChannelDispatchOperationPtr &dispatchOperation)
{
    qDebug() << "TpApproverManager::addDispatchOperation()";

    TpApprover *approver = new TpApprover(context, channels, dispatchOperation, this);
    connect(approver, SIGNAL(finished()), SLOT(onFinished()));

    mApprovers << approver;
}

void TpApproverManager::onFinished()
{
    qDebug() << "TpApproverManager::onFinished()";

    TpApprover *approver = qobject_cast<TpApprover*>(sender());

    mApprovers.removeOne(approver);
    approver->deleteLater();
}

} // namespace TpGame
