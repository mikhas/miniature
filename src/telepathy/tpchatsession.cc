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

#include "tpchatsession.h"
#include "tphelpers.h"
#include "tpaccountitem.h"

#include <TelepathyQt4/PendingReady>
#include <TelepathyQt4/ContactManager>
#include <TelepathyQt4/Account>

namespace Miniature
{
    
TpChatSession::TpChatSession(QObject *parent, Tp::AccountPtr account) :
    QObject(parent),
    m_account(account)
{
    qDebug() << "TpChatSession::TpChatSession()";
}

TpChatSession::~TpChatSession()
{
    qDebug() << "TpChatSession::~TpChatSession()";
    
    // When the ChatSession is closed, we should close the channel so that a new one is launched if
    // the same contact tries to contact us again.
    if (!m_textChannel.isNull()) {
        m_textChannel->requestClose();
        // FIXME: Can we connect the signal that this is done to the parent slot somewhere for error
        // handling?
    }
}

void TpChatSession::sendMessage(const QString &message)
{
    qDebug() << "TpChatSession::sendMessage()" << message;

    if (!m_textChannel) {
        qWarning() << "Message not sent because channel does not yet exist.";

        // TODO Indicate that the message sending failed.
        
        return;
    }

    m_textChannel->send(message);
}

void TpChatSession::messageSent(const Tp::Message &message,
                                       Tp::MessageSendingFlags flags,
                                       const QString &sentMessageToken)
{
    qDebug() << "TpChatSession::messageSent" << message.text();

    Q_UNUSED(flags);
    Q_UNUSED(sentMessageToken);
    Q_UNUSED(message);

    // TODO process outgoing messages with status ...
}

void TpChatSession::messageReceived(const Tp::ReceivedMessage &message)
{
    qDebug() << "TpChatSession::messageReceived" << message.text();

    Q_UNUSED(message);

    // TODO process received message

    // Acknowledge the receipt of this message, 
    // so it won't be redespatched to us when we close the channel.
    m_textChannel->acknowledge(QList<Tp::ReceivedMessage>() << message);
}

void TpChatSession::setTextChannel(/*Tp::ContactPtr contact, */const Tp::TextChannelPtr &textChannel)
{
    qDebug() << "TpChatSession::setTextChannel";

#if 0
    // TOOD watch for contact presence
    
    if (contact.isNull()) {
        qWarning() << "Null contact. Fail.";
        return;
    }
    m_contact = contact;

    connect(m_contact.data(), SIGNAL(simplePresenceChanged(const QString &, uint, const QString &)),
            this, SLOT(onContactPresenceChanged(const QString &, uint, const QString &)));        
#endif
    
    m_textChannel = textChannel;

    // We must get the text channel ready with the required features.
    Tp::Features features;
    features << Tp::TextChannel::FeatureCore
             << Tp::TextChannel::FeatureMessageCapabilities
             << Tp::TextChannel::FeatureMessageQueue
             << Tp::TextChannel::FeatureMessageSentSignal;

    if (m_textChannel->isReady(features)) {
        qDebug() << "Already ready.";
    } else {
        qDebug() << "Not already ready.";
    }

    connect(m_textChannel->becomeReady(features),
            SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(onTextChannelReady(Tp::PendingOperation*)));
}

void TpChatSession::onTextChannelReady(Tp::PendingOperation *op)
{
    qDebug() << "TpChatSession::onTextChannelReady";

    Q_UNUSED(op);
    
    if (op->isError()) {
        qWarning() << "Text channel failed to become ready:"
                                       << op->errorName()
                                       << op->errorMessage();
        return;
    }

    QObject::connect(m_textChannel.data(),
                     SIGNAL(messageSent(const Tp::Message &, Tp::MessageSendingFlags, const QString &)),
                     this,
                     SLOT(messageSent(const Tp::Message &, Tp::MessageSendingFlags, const QString &)));
    
    QObject::connect(m_textChannel.data(),
                     SIGNAL(messageReceived(const Tp::ReceivedMessage &)),
                     this,
                     SLOT(messageReceived(const Tp::ReceivedMessage &)));

    // Check for messages already there in the message queue.
    Q_FOREACH (const Tp::ReceivedMessage &message, m_textChannel->messageQueue()) {
        messageReceived(message);
    }

#if 1
    // JUST FOR TESTING
    if (!m_textChannel.isNull()) {
        qDebug() << "saying hello";
        QString msg("hello");
        m_textChannel->send(msg);
        qDebug() << "message sent " << msg;
    } else {
        qDebug() << "error not saying hello";
    }
#endif
}

void TpChatSession::onContactPresenceChanged(const QString &, uint type, const QString &)
{
    qDebug() << "TpChatSession::onContactPresenceChanged";

    Q_UNUSED(type);
#if 0
    if(!account()->isConnected() || type == Tp::ConnectionPresenceTypeOffline) {
        // TODO tell player about the other player leaved chat session 
    }
#endif    
}

};
