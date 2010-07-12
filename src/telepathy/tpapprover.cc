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
#include "tpapprover.h"

#include <TelepathyQt4/Contact>
#include <TelepathyQt4/Channel>
#include <TelepathyQt4/Connection>
#include <TelepathyQt4/ReferencedHandles>
#include <TelepathyQt4/PendingReady>
#include <TelepathyQt4/ChannelDispatchOperation>

#ifdef HAVE_MAEMOCONTACTSELECTOR
#include <gtk/gtk.h>
#include <hildon/hildon.h>
#include <libosso-abook/osso-abook.h>
#endif

namespace TpGame
{

TpApprover::TpApprover(const Tp::MethodInvocationContextPtr<> &context,
                       const QList<Tp::ChannelPtr> &channels,
                       const Tp::ChannelDispatchOperationPtr &dispatchOperation,
#ifdef HAVE_MAEMOCONTACTSELECTOR
                       OssoABookAggregator *abook_aggregator,
#endif
                       QObject *parent)
    : QObject(parent),
    mContext(context),
    mChannels(channels),
    mDispatchOp(dispatchOperation)
#ifdef HAVE_MAEMOCONTACTSELECTOR
    , mAbookAggregator(abook_aggregator)
#endif
{
    qDebug() << "TpApprover::TpApprover()";

    mContext->setFinished();

    if (channels.count() == 1)
    {
        mChannel = channels.at(0);
        connect(mChannel->becomeReady(),
                SIGNAL(finished(Tp::PendingOperation *)),
                SLOT(onChannelReady(Tp::PendingOperation *)));
    }
    else
    {
        connect(mDispatchOp->claim (),
                SIGNAL(finished(Tp::PendingOperation *)),
                SLOT(onReadyToBeClosed(Tp::PendingOperation *)));
    }
}

TpApprover::~TpApprover()
{
    qDebug() << "TpApprover::~TpApprover()";
}

void TpApprover::onDispatchOperationReady(Tp::PendingOperation *)
{
    qDebug() << "TpApprover::ondispatchOperationReady()";

    Q_EMIT finished();
}

void TpApprover::onReadyToBeClosed(Tp::PendingOperation *)
{
    qDebug() << "TpApprover::onReadyToBeClosed()";
    for (int i = 0; i < mChannels.size(); ++i)
    {
        mChannels.at(i)->requestClose();
    }

    connect(mDispatchOp->becomeReady(),
            SIGNAL(finished(Tp::PendingOperation *)),
            SLOT(onDispatchOperationReady(Tp::PendingOperation* )));
}

void TpApprover::onChannelReady(Tp::PendingOperation *)
{
#ifdef HAVE_MAEMOCONTACTSELECTOR
    QString contactName;
    Tp::Contacts contacts = mChannel->groupContacts();
    Tp::ContactPtr contact(NULL);
    GList *abook_contacts;
    OssoABookContact *abook_contact = NULL;
    QSet<Tp::ContactPtr>::const_iterator i = contacts.constBegin();
    while (i != contacts.constEnd())
    {
        qDebug() << *i;
        if (*i != mChannel->groupSelfContact())
        {
            contact = *i;
            break;
        }
        ++i;
    }
    qDebug("Incoming channel from '%s'", contact->id().toUtf8().data());

    abook_contacts = osso_abook_aggregator_find_contacts_for_im_contact 
      (mAbookAggregator, contact->id().toUtf8().data(), NULL);
    qDebug ("Abook contacts count: %u\n", g_list_length (abook_contacts));
    if (abook_contacts != NULL)
      abook_contact = OSSO_ABOOK_CONTACT (abook_contacts->data);
    g_list_free (abook_contacts);

    if (abook_contact)
    {
        contactName = osso_abook_contact_get_display_name (abook_contact);
        qDebug("Contact name retrieven from the address book: '%s'", contactName.toUtf8().data());
    }
    else
    {
        contactName = contact->alias();
        qDebug("Contact name retrieven from Telepathy: '%s'", contactName.toUtf8().data());
    }


    QString message("Do you want to play chess with " + contactName + "?");
    GtkWidget *note = hildon_note_new_confirmation (NULL,
                    message.toUtf8().data());

    int ret = gtk_dialog_run (GTK_DIALOG (note));
    gtk_widget_destroy (GTK_WIDGET (note));

    if (ret == GTK_RESPONSE_OK)
    {
        mDispatchOp->handleWith (QString("org.freedesktop.Telepathy.Client.Miniature"));
    }
    else
    {
        connect(mDispatchOp->claim (),
                SIGNAL(finished(Tp::PendingOperation *)),
                SLOT(onReadyToBeClosed(Tp::PendingOperation *)));
    }
#else
    mDispatchOp->handleWith (QString("org.freedesktop.Telepathy.Client.Miniature"));
#endif

    connect(mDispatchOp->becomeReady(),
            SIGNAL(finished(Tp::PendingOperation *)),
            SLOT(onDispatchOperationReady(Tp::PendingOperation* )));
}


} // namespace TpGame
