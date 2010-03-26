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

#ifndef TPCHATSESSION_H
#define TPCHATSESSION_H

#include <TelepathyQt4/ChannelRequest>
#include <TelepathyQt4/Contact>
#include <TelepathyQt4/Message>
#include <TelepathyQt4/PendingChannelRequest>
#include <TelepathyQt4/TextChannel>

namespace Miniature
{

class TpChatSession : public QObject
{
    Q_OBJECT
    
public:
    TpChatSession(QObject *parent, Tp::AccountPtr account);
    virtual ~TpChatSession();

    void setTextChannel(/*Tp::ContactPtr contact,*/const Tp::TextChannelPtr &textChannel);
    
private Q_SLOTS:
    void onTextChannelReady(Tp::PendingOperation *op);

    void sendMessage(const QString &message);
    void messageSent(const Tp::Message &message,
                     Tp::MessageSendingFlags flags,
                     const QString &sentMessageToken);
    void messageReceived(const Tp::ReceivedMessage &message);

	void onContactPresenceChanged(const QString &, uint, const QString &);

private:
    Tp::AccountPtr m_account;
    Tp::ContactPtr m_contact;
    Tp::TextChannelPtr m_textChannel;
};

};

#endif // TPCHATSESSION_H
