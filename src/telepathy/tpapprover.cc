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

#include "tpapprover.h"

#include <TelepathyQt4/Contact>
#include <TelepathyQt4/Connection>
#include <TelepathyQt4/ReferencedHandles>
#include <TelepathyQt4/PendingReady>
#include <TelepathyQt4/ChannelDispatchOperation>

namespace TpGame
{

TpApprover::TpApprover(const Tp::MethodInvocationContextPtr<> &context,
                       const QList<Tp::ChannelPtr> &channels,
                       const Tp::ChannelDispatchOperationPtr &dispatchOperation,
                       QObject *parent)
    : QObject(parent),
    mContext(context),
    mChannels(channels),
    mDispatchOp(dispatchOperation)
{
    qDebug() << "TpApprover::TpApprover()";

    connect(mDispatchOp->becomeReady(),
            SIGNAL(finished(Tp::PendingOperation *)),
            SLOT(onDispatchOperationReady(Tp::PendingOperation* )));
}

TpApprover::~TpApprover()
{
    qDebug() << "TpApprover::~TpApprover()";
}

void TpApprover::onDispatchOperationReady(Tp::PendingOperation *)
{
    qDebug() << "TpApprover::ondispatchOperationReady()";

    mContext->setFinished();

    Q_EMIT finished();
}

} // namespace TpGame
