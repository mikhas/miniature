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

#ifndef TPAPPROVERMANAGER_H
#define TPAPPROVERMANAGER_H

#include <QObject>
#include <TelepathyQt4/AbstractClientApprover>

namespace TpGame
{

class TpApprover;

class TpApproverManager : public QObject, public Tp::AbstractClientApprover
{
    Q_OBJECT

public:
    TpApproverManager(QObject *parent = 0);
    virtual ~TpApproverManager();

    virtual void addDispatchOperation(const Tp::MethodInvocationContextPtr<> &context,
                                      const QList<Tp::ChannelPtr> &channels,
                                      const Tp::ChannelDispatchOperationPtr &dispatchOperation);

private Q_SLOTS:
    void onFinished();

private:
    QList<TpApprover *> mApprovers;
};

} // namespace TpGame

#endif // TPAPPROVERMANAGER_H
