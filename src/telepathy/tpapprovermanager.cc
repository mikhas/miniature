/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2010 Collabora Ltd. <http://www.collabora.co.uk/>
 * @author Dariusz Mikulski <dariusz.mikulski@collabora.co.uk>
 * @author Alban Crequy <alban.crequy@collabora.co.uk>
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

#include <config.h>

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

#ifdef HAVE_MAEMOCONTACTSELECTOR
static void
aggregator_ready_cb (OssoABookWaitable *aggregator G_GNUC_UNUSED,
                     const GError      *error G_GNUC_UNUSED,
                     gpointer           user_data)
{
    TpApproverManager *mgr = (TpApproverManager *) user_data;
    mgr->setContactAggregatorReady();
}
#endif

TpApproverManager::TpApproverManager(QObject *parent) : QObject(parent), Tp::AbstractClientApprover(channelClassList())
{
    qDebug() << "TpApproverManager::TpApproverManager()";
#ifdef HAVE_MAEMOCONTACTSELECTOR
    abook_aggregator =
            OSSO_ABOOK_AGGREGATOR (osso_abook_aggregator_get_default (NULL));
    
    osso_abook_waitable_call_when_ready (OSSO_ABOOK_WAITABLE (abook_aggregator),
                    aggregator_ready_cb, (gpointer) this, NULL);
#endif
}

TpApproverManager::~TpApproverManager()
{
    qDebug() << "TpApproverManager::~TpApproverManager()";
}

void TpApproverManager::setContactAggregatorReady()
{
  Q_EMIT contactAggregatorReady();
}

void TpApproverManager::addDispatchOperation(const Tp::MethodInvocationContextPtr<> &context,
                                             const QList<Tp::ChannelPtr> &channels,
                                             const Tp::ChannelDispatchOperationPtr &dispatchOperation)
{
    qDebug() << "TpApproverManager::addDispatchOperation()";

#ifdef HAVE_MAEMOCONTACTSELECTOR
    TpApprover *approver = new TpApprover(context, channels, dispatchOperation, abook_aggregator, this);
#else
    TpApprover *approver = new TpApprover(context, channels, dispatchOperation, this);
#endif
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
