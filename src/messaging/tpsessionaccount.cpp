/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "tpsessionaccount_p.h"
#include "telepathyhelpers_maemo6_p.h"
#include "telepathyengine_maemo6_p.h"

#include <TelepathyQt4/Message>
/**
 * \class TpSessionAccount
 * \headerfile <tpsessionaccount.h>
 *
 * TpSessionAccount class represents every account you have. As example account for “Ring” connection manager represents your cellular
 * account and you may send and receive SMS with it. Gabble represents your GoogleTalk account if you have defined them.
 * TpSessionAccounts are created by TpSession class,they are not intended to be created stand-alone

 */
/**
 * \fn void TpSessionAccount::accountReady(TpSessionAccount *);
 *
 * Emitted when the account becomes ready
 *
 * \param  TpSessionAccount  pointer to account become ready
 */
/**
 * \fn void TpSessionAccount::channelReady(TpSessionAccount *);
 *
 * Emitted when the account Manager becomes ready
 *
 * \param  TpSession  pointer to TpSession class
 */
/**
 * \fn void TpSessionAccount::messageReceived(const Tp::ReceivedMessage &,TpSessionAccount *);
 *
 * Emitted when any of Account Managers recived message
 *
 * \param  Tp::ReceivedMessage  Message received
 * \param  TpSessionAccount  pointer to account received message
 */

/**
 * \fn void TpSessionAccount::newChannel(TpSessionAccount *,QString CjhannelType,QString peerId,const Tp::ChannelDetails &);
 * \param  TpSession  pointer to TpSession class
 * \param  ChannelType type of Channel, TELEPATHY_INTERFACE_CHANNEL_TYPE_TEXT for text channel, TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAMED_MEDIA for steram media, as exmple for incoming call
 * \param  peedId PeerId, as example caller telephone number
 * \param channeDetails needed if you would like to create a channel. For text chanels TpSession creates channel automatically. For calls, Maemo Call UI handles callcreation
 */
/**
 * Construct a new TpSessionAccount object. This constructor is called by TpSession class when new account is created or fetched from account manager. It is not inended to be used stand alone
 *
 * \param am          Telepathy-Qt4 account manager for this account
 * \param objectPath  Dbus object path tonew account
 */
TpSessionAccount::TpSessionAccount(Tp::AccountManagerPtr am, const QString &objectPath)
    : ready(false)
    , mAcc(Tp::Account::create(am->dbusConnection(), am->busName(), objectPath))
{
    QDEBUG_FUNCTION_BEGIN
    connect(mAcc->becomeReady(), SIGNAL(finished(Tp::PendingOperation *)), SLOT(onReady(Tp::PendingOperation *)));
    qDebug() << "TpSessionAccount::TpSessionAccount objectPath=" << objectPath;
    QDEBUG_FUNCTION_END
}


void TpSessionAccount::onReady(Tp::PendingOperation *op)
{
    Q_UNUSED(op);

    QDEBUG_FUNCTION_BEGIN
    acc = mAcc.data();
    qDebug() << "TpSessionAccount::onReady cmName=" << acc->cmName() << "haveConnection=" << (acc->haveConnection() ? (acc->connection()->isReady() ? "Ready" : "notReady") : "no");

    if (acc->haveConnection()) {

        connect(acc->connection()->becomeReady(Tp::Connection::FeatureRoster | Tp::Connection::FeatureSelfContact),
                SIGNAL(finished(Tp::PendingOperation *)),
                SLOT(onContactsConnectionReady(Tp::PendingOperation *)));
        if (acc->connection()->isReady() && acc->connection()->interfaces().contains(TELEPATHY_INTERFACE_CONNECTION_INTERFACE_REQUESTS)) {
            qDebug() << "TpSessionAccount::onReady: connecting to Connection.Interface.NewChannels";
            connect(acc->connection()->requestsInterface(),
                    SIGNAL(NewChannels(const Tp::ChannelDetailsList&)),
                    SLOT(onNewChannels(const Tp::ChannelDetailsList&)));
        }
    } else { // If there is no connection, we are ready now, else we are ready when contacts connection is ready
        ready = true;
        emit accountReady(this);
    }
    QDEBUG_FUNCTION_END
}

void TpSessionAccount::onContactsConnectionReady(Tp::PendingOperation *op)
{
    QDEBUG_FUNCTION_BEGIN
    if (op->isError()) {
        qWarning() << "Connection cannot become ready" << acc->cmName();
        return;
    }

    if (acc->connection()->interfaces().contains(TELEPATHY_INTERFACE_CONNECTION_INTERFACE_REQUESTS)) {
        qDebug() << "TpSessionAccount::onContactsConectionReady: connecting to Connection.Interface.NewChannels";
        connect(acc->connection()->requestsInterface(),
                SIGNAL(NewChannels(const Tp::ChannelDetailsList&)),
                SLOT(onNewChannels(const Tp::ChannelDetailsList&)));
    } else qDebug() << "TpSessionAccount::onContactsConnectionReady: does NO have CONNECTION_INTERFACE_REQUESTS";
    Tp::PendingReady *pr = qobject_cast<Tp::PendingReady *>(op);
    contactsConn = Tp::ConnectionPtr(qobject_cast<Tp::Connection *>(pr->object()));
#if 0
    connect(contactsConn->contactManager(),
            SIGNAL(presencePublicationRequested(const Tp::Contacts &)),
            SLOT(onPresencePublicationRequested(const Tp::Contacts &)));
#endif
    qDebug() << "TpSessionAccount::onContactsConnectionReady " << acc->cmName() ;
    //    RosterItem *item;

    myContacts = contactsConn->contactManager()->allKnownContacts();
    foreach (const Tp::ContactPtr &contact, myContacts) {
        qDebug() << "id=" << contact->id() << " alias=" << contact->alias() << " presence=" << contact->presenceStatus();

	const QString id(contact->id());
	bool isChannelAdded(false);

	SendJobList::Iterator it = jobs.begin(), end = jobs.end();
	for (; it != end; ++it) {
	    if (id == (*it).address) {
		(*it).contactReady = true;
		if (!isChannelAdded) { 
		    addOutgoingChannel(contact);
		    isChannelAdded = true;
		}
	    }
	}
    }

    QSet<QString> addresses;
    foreach (const SendJob &job, jobs) {
	if (!job.contactReady && !addresses.contains(job.address)) {
	    addresses.insert(job.address);
	    makeContactFromAddress(job.address);
	}
    }

    ready = true;
    emit accountReady(this);
    QDEBUG_FUNCTION_END
}


/**
 * Fetch Tp::ContactPtr for contact with given address. Contact is searched among contacts returned by contact manager for ths account.
 * All connecions managers does not return contacts, as example Ring telephony contact manager does not. Gabble for Googletalk or Spirit for Skype does
 * return contacts-
 *
 * \param id            Contact address/id, as example email address, telephone number etc. Only exact matches
 * \return      TpContactPtr, if nontact is not returned TpContactPtr.isNull() is true
 */

Tp::ContactPtr TpSessionAccount::getContactFromAddress(const QString &id) const
{
    QDEBUG_FUNCTION_BEGIN
    Tp::ContactPtr p;

    foreach(const Tp::ContactPtr &contact, myContacts) {
        if (contact->id() == id) {
            p = contact;
            break;
        }
    }
    QDEBUG_FUNCTION_END
    return p;
}

/**
 * Fetch TpSessionChannel for with given address. Contact is searched among active channels for this account.
 *
 *
 * \param id            Contact address/id, as example email address, telephone number etc. Only exact matches
 * \return          Pointer to TpSessionChannel or NULL if nit found
 */

TpSessionChannel* TpSessionAccount::getChannelFromPeerAddress(const QString &id) const
{
    QDEBUG_FUNCTION_BEGIN
    TpSessionChannel *p = 0;

    foreach(TpSessionChannel* channel, myChannels) {
        if (channel->peerId() == id) {
            p = channel;
            break;
        }
    }

    QDEBUG_FUNCTION_END
    return p;
}
/**
 * Creates new contact with given address. This function is Acynchronous, it sends request to contact manager for contact creation,
 *
 * \param address           Contact address/id, as example email address, telephone number etc.
 */

void TpSessionAccount::makeContactFromAddress(const QString &address)
{
    QDEBUG_FUNCTION_BEGIN

    Tp::PendingContacts *pc = contactsConn->contactManager()->contactsForIdentifiers(QStringList(address));
    connect(pc, SIGNAL(finished(Tp::PendingOperation *)), SLOT(onNewContactRetrieved(Tp::PendingOperation *)));

    QDEBUG_FUNCTION_END
}

void TpSessionAccount::onNewContactRetrieved(Tp::PendingOperation *op)
{
    QDEBUG_FUNCTION_BEGIN

    Tp::PendingContacts *pcontacts = qobject_cast<Tp::PendingContacts *>(op);

    if (!pcontacts)
	return;

    QHash<QString, QPair<QString, QString> > invalidIds = pcontacts->invalidIdentifiers();
    {
	QHash<QString, QPair<QString, QString> >::ConstIterator it = invalidIds.begin(), end = invalidIds.end();
	for (; it != end; ++it) {
	    qWarning() << "Unable to add contact " << it.key() << ":" << it.value().first << ":" << it.value().second;
	    QString id(it.key());
	    for (int i = jobs.count() - 1; i >= 0; --i) {
		if (id == jobs[i].address) {
		    jobs[i].sendRequest->setFinished(id, false);
		    jobs.removeAt(i);
		}
	    }
	}
    }

    foreach (Tp::ContactPtr contact, pcontacts->contacts()) {
	const QString id(contact->id()); 
	bool found(false);

	SendJobList::Iterator it = jobs.begin(), end = jobs.end();
	for (; it != end; ++it) {
	    if (id == (*it).address) {
		(*it).contactReady = true;
		found = true;
	    }
	}
	if (found)
	    addOutgoingChannel(contact);
    }

    QDEBUG_FUNCTION_END
}
/**
 * Send message to given address. This function is compled Acynchronous function that may produce multiple state transitions beforecomletion.
 * If there is already existing TpSessionChannel for this contact, it simply queues message for sending and no forther transitions are needed
 * If there are no hannel, it first check is there contact for this address, if is, it requests new channel to be created for ths channel and message
 * is left waiting untill channel is created. If there is no contact, it sends request fr contact creation and when contact is created state machine
 * proceeds to channel creation.
 *
 * MessageSent() signal is emitted when completed
 */

bool TpSessionAccount::sendMessage(SendRequest *sendRequest)
{
    int jobCount(0);

    foreach (const QString &address, sendRequest->to()) {
	if (TpSessionChannel *channel = getChannelFromPeerAddress(address)) {
	    Tp::PendingSendMessage *pendingMessage = channel->sendMessage(sendRequest->text()); // We have already channel
	    if (pendingMessage->isFinished()) {
		sendRequest->finished(pendingMessage, true);
		if (!pendingMessage->isError())
		    jobCount++;
	    } else {
		connect(pendingMessage, SIGNAL(finished(Tp::PendingOperation *)),
			sendRequest, SLOT(finished(Tp::PendingOperation *)));
		jobCount++;
	    }
	} else {
	    SendJob job(address, sendRequest);
	    Tp::ContactPtr p = getContactFromAddress(address); // Do we have contact ready ?
	    if (p.isNull()) { // If not, create it
		makeContactFromAddress(address); // Create and after created, send
	    } else {
		addOutgoingChannel(p); // Create channel and when ready, send
		job.contactReady = true;
	    }
	    jobs << job;
	    jobCount++;
	}
    }
    return jobCount > 0;
}

void TpSessionAccount::addOutgoingChannel(const Tp::ContactPtr &contact)
{
    QDEBUG_FUNCTION_BEGIN

    qDebug() << "TpSessionAccount::addOutgoingChannel";

    TpSessionChannel *newChannel = new TpSessionChannel(contact->manager()->connection(), contact);
    connect(newChannel, SIGNAL(messageReceived(const Tp::ReceivedMessage &, TpSessionChannel *)),
            SLOT(onMessageReceived(const Tp::ReceivedMessage &, TpSessionChannel *)));
    connect(newChannel, SIGNAL(channelReady(TpSessionChannel *)),
            SLOT(onOutgoingChannelReady(TpSessionChannel*)));
    myChannels += newChannel;

    QDEBUG_FUNCTION_END
}

void TpSessionAccount::onOutgoingChannelReady(TpSessionChannel *ch)
{
    QDEBUG_FUNCTION_BEGIN

    qDebug() << "TpSessionAccoiunt::onOutgoingChannelReady" << ch->peerId();

    emit channelReady(this);

    SendJobList readyList;
    for (int i = jobs.count() - 1; i >= 0; --i) {
	if (ch->peerId() == jobs[i].address) {
	    readyList.prepend(jobs[i]);
	    jobs.removeAt(i);
	}
    }

    foreach (const SendJob &job, readyList) {
	SendRequest *sendRequest = job.sendRequest;
	Tp::PendingSendMessage *pendingMessage = ch->sendMessage(sendRequest->text());
	if (pendingMessage->isFinished()) {
	    sendRequest->finished(pendingMessage);
	} else {
	    connect(pendingMessage, SIGNAL(finished(Tp::PendingOperation *)),
		    sendRequest, SLOT(finished(Tp::PendingOperation *)));
	}
    }

    QDEBUG_FUNCTION_END
}


void TpSessionAccount::onMessageSent(const Tp::Message &msg, Tp::MessageSendingFlags, const QString &flags)
{
    Q_UNUSED(flags);
    QDEBUG_FUNCTION_BEGIN

    qDebug() << "TpSessionAccount::onMessageSent";
    emit messageSent(msg, this);

    QDEBUG_FUNCTION_END
};

void TpSessionAccount::onMessageReceived(const Tp::ReceivedMessage &msg, TpSessionChannel *ch)
{
    Q_UNUSED(ch);
    QDEBUG_FUNCTION_BEGIN

    qDebug() << "TpSessionAccount::onMessageReceived " << msg.text();
    emit messageReceived(msg, this);

    QDEBUG_FUNCTION_END
};

void TpSessionAccount::onNewChannels(const Tp::ChannelDetailsList &channels)
{
    QDEBUG_FUNCTION_BEGIN

    Tp::TextChannelPtr myIngoingTextChannel;
    qDebug() << "TpSessionAccount::onNewChannels";
    foreach (const Tp::ChannelDetails &details, channels) {
        QString channelType = details.properties.value(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".ChannelType")).toString();
        QString targetId = details.properties.value(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".TargetID")).toString();
        bool requested = details.properties.value(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".Requested")).toBool();
        qDebug() << " channelType:" << channelType << " requested  :" << requested << " targetId" << targetId;

        emit newChannel(this, channelType, targetId, details);
        if (channelType == TELEPATHY_INTERFACE_CHANNEL_TYPE_TEXT && !requested) {

            myIngoingTextChannel = Tp::TextChannel::create(acc->connection(), details.channel.path(), details.properties);
            qDebug() << "TpSessionAccount::onNewChannels path=" << "path " << myIngoingTextChannel->objectPath();

            TpSessionChannel* newChannel = new TpSessionChannel(myIngoingTextChannel);
            connect(newChannel, SIGNAL(messageReceived(const Tp::ReceivedMessage &, TpSessionChannel *)),
                    SLOT(onMessageReceived(const Tp::ReceivedMessage &, TpSessionChannel *)));
            myChannels += newChannel;
        }
        if (channelType == TELEPATHY_INTERFACE_CHANNEL_TYPE_STREAMED_MEDIA && !requested) {
            qDebug() << "Incoming call" ;
        }
    }

    QDEBUG_FUNCTION_END
}
