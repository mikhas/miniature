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

#if 0
TpChatSession::TpChatSession(const Kopete::Contact *user, Kopete::ContactPtrList others, Kopete::Protocol *protocol)
        : Kopete::ChatSession(user, others, protocol),
        m_pendingChannelRequest(0)
{
    kDebug();
    Kopete::ChatSessionManager::self()->registerChatSession(this);

    QObject::connect(this,
                     SIGNAL(messageSent(Kopete::Message&, Kopete::ChatSession*)),
                     SLOT(sendMessage(Kopete::Message&)));

    setComponentData (protocol->componentData ());

    // HACK: This breaks on 1-many chats.
    // Set the m_contact to be the target contact's Tp::ContactPtr.
    if (others.size() <= 0) {
        kWarning() << "Invalid number of people in the chat. Aborting share-my-desktop.";
        return;
    }

    if (others.size() > 1) {
        kWarning() << "Share-My-Desktop only supports 1-1 chats at the moment.";
        return;
    }

    // Cast the only member of the chat to a TelepathyContact.
    TelepathyContact *tpContact = qobject_cast<TelepathyContact*>(others.at(0));

    // Check the member really is a telepathy contact.
    if (!tpContact) {
        kWarning() << "Chat member is not a telepathy contact. Aborting share-my-desktop.";
        return;
    }

    // Get the internal Tp::Contact and save it as a member variable.
    m_contact = tpContact->internalContact();

    connect(m_contact.data(), SIGNAL(simplePresenceChanged(const QString &, uint, const QString &)),
            this, SLOT(onContactPresenceChanged(const QString &, uint, const QString &)));
    // Only at this point do we allow the share-my-desktop button to be shown.
    setXMLFile ("telepathychatui.rc");
}
#endif

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

void TpChatSession::createTextChannel(Tp::ContactPtr contact)
{
    qDebug() << "TpChatSession::createTextChannel()";

    if (contact.isNull()) {
        qWarning() << "Null contact. Fail.";
        return;
    }
    m_contact = contact;
    
    QString preferredHandler("org.freedesktop.Telepathy.Client.miniature_text_handler");

    if (m_account.isNull()) {
        qWarning() << "Null account. Fail.";
        return;
    }

    connect(m_contact.data(), SIGNAL(simplePresenceChanged(const QString &, uint, const QString &)),
            this, SLOT(onContactPresenceChanged(const QString &, uint, const QString &)));        

    m_pendingChannelRequest =
            m_account->ensureTextChat(m_contact, QDateTime::currentDateTime(), preferredHandler);

    m_channelRequest = m_pendingChannelRequest->channelRequest();
    qDebug() << "m_channelRequest:" << m_channelRequest.data();

    connect(m_pendingChannelRequest,
            SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(onEnsureChannelFinished(Tp::PendingOperation*)));
}

void TpChatSession::onEnsureChannelFinished(Tp::PendingOperation* op)
{
    qDebug() << "TpChatSession::onEnsureChannelFinished()";

    if (op->isError()) {
        qWarning() << "Ensuring Channel Failed:" << op->errorName() << op->errorMessage();
        return;
    }

    m_channelRequest = m_pendingChannelRequest->channelRequest();
    m_pendingChannelRequest = 0;
}

void TpChatSession::sendMessage(const QString &message)
{
    qDebug() << "TpChatSession::sendMessage()" << message;

    if (!m_textChannel) {
        qWarning() << "Message not sent because channel does not yet exist.";

        // TODO Indicate that the message sending failed.
        // message.setState(Kopete::Message::StateError);
        // appendMessage(message);

        // FIXME: Only call messageSucceeded() if there are no messages in the process of being sent.
        //messageSucceeded();
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
    
    // FIXME: We need to process outgoing messages better, so we can display when they fail etc

    //Kopete::Message::MessageType messageType = Kopete::Message::TypeNormal;
    /*
     if(message.messageType() == Tp::ChannelTypeMessageAction)
     {
      messageType = Kopete::Message::TypeAction;
     }
    */
#if 0
    Kopete::Message newMessage(myself(), members());
    newMessage.setPlainBody(message.text());
    newMessage.setDirection(Kopete::Message::Outbound);
    newMessage.setType(messageType);

    appendMessage(newMessage);
    messageSucceeded();
#endif    
}

void TpChatSession::messageReceived(const Tp::ReceivedMessage &message)
{
    qDebug() << "TpChatSession::messageReceived" << message.text();

    Q_UNUSED(message);
    
#if 0
    Kopete::Message::MessageType messageType = Kopete::Message::TypeNormal;

    Kopete::Message newMessage(members().first(), myself());
    newMessage.setPlainBody(message.text());
    newMessage.setDirection(Kopete::Message::Inbound);
    newMessage.setType(messageType);

    appendMessage(newMessage);
#endif    

    // Acknowledge the receipt of this message, so it won't be redespatched to us when we close
    // the channel.
    m_textChannel->acknowledge(QList<Tp::ReceivedMessage>() << message);
}

Tp::ChannelRequestPtr TpChatSession::channelRequest()
{
    qDebug() << " TpChatSession::channelRequest";
    
    return m_channelRequest;
}

void TpChatSession::setTextChannel(Tp::ContactPtr contact, const Tp::TextChannelPtr &textChannel)
{
    qDebug() << "TpChatSession::setTextChannel";

    if (contact.isNull()) {
        qWarning() << "Null contact. Fail.";
        return;
    }
    m_contact = contact;

    connect(m_contact.data(), SIGNAL(simplePresenceChanged(const QString &, uint, const QString &)),
            this, SLOT(onContactPresenceChanged(const QString &, uint, const QString &)));        
    
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
}

Tp::PendingChannelRequest *TpChatSession::pendingChannelRequest()
{
    qDebug() << "TpChatSession::pendingChannelRequest";
    
    return m_pendingChannelRequest;
}


void TpChatSession::onContactPresenceChanged(const QString &, uint type, const QString &)
{
    qDebug() << "TpChatSession::onContactPresenceChanged";

    Q_UNUSED(type);
#if 0
    if(!account()->isConnected() || type == Tp::ConnectionPresenceTypeOffline)
#endif    
}

};
