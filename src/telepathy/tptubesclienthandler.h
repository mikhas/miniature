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

#ifndef TPTUBESCLIENTHANDLER_H
#define TPTUBESCLIENTHANDLER_H

#include <TelepathyQt4/AbstractClientHandler>
#include <TelepathyQt4/PendingOperation>
#include <TelepathyQt4/Channel>

#include <QTcpSocket>

namespace TpGame
{

class OutgoingTube;
class TpIncomingTube;
class TubeClient;

class TpTubesClientHandler : public QObject, public Tp::AbstractClientHandler
{
    Q_OBJECT
public:
    TpTubesClientHandler(QObject *parent = 0);
    virtual ~TpTubesClientHandler();

    virtual bool bypassApproval() const;
    virtual void handleChannels(const Tp::MethodInvocationContextPtr<> &context,
                                const Tp::AccountPtr &account,
                                const Tp::ConnectionPtr &connection,
                                const QList<Tp::ChannelPtr> &channels,
                                const QList<Tp::ChannelRequestPtr> &requestedSatisfied,
                                const QDateTime &userActionTime,
                                const QVariantMap &handlerInfo);

Q_SIGNALS:
    void newOutgoingTube(TubeClient *, const Tp::ContactPtr &);
    void newIncomingTube(TubeClient *, const Tp::ContactPtr &);
    void disconnected();

private Q_SLOTS:
    void deleteOutgoingTube();
    void deleteIncomingTube();

private:
    QList<OutgoingTube*> mOutgoingTubes;
    QList<TpIncomingTube*> mIncomingTubes;
};

} // namespace TpGame

#endif // TPTUBESCLIENTHANDLER_H
