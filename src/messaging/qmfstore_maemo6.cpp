/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "qmessagemanager.h"
#include "qmfhelpers_maemo6_p.h"

#include <qmailstore.h>

#include <QCoreApplication>
#include <QEvent>

#include "qmfstore_maemo6_p.h"
#include "qmessage_p.h"

using namespace QTM_PREPEND_NAMESPACE(QmfHelpers);

QTM_BEGIN_NAMESPACE

Q_SCOPED_STATIC_DEFINE(QMFStore,QMFStorePrivate,storeInstance);

QMailStore *QMFStorePrivate::convert(QMFStore *store)
{
    return store->d_ptr->_store;
}

void QMFStorePrivate::registerMessageStatus(QMailStore *store, const QString &field)
{
    store->registerMessageStatusFlag(field);
}

void QMFStorePrivate::createNonexistentFolder(QMailStore *store, const QString &path, quint64 status)
{
    QMailFolderKey pathKey(QMailFolderKey::path(path));
    QMailFolderKey accountKey(QMailFolderKey::parentAccountId(QMailAccountId()));

    if (store->countFolders(pathKey & accountKey) == 0) {
        QMailFolder folder;
        folder.setPath(path);
        folder.setStatus(status);

        if (!store->addFolder(&folder)) {
            qWarning() << "Unable to add folder for:" << path;
        }
    }
}

void QMFStorePrivate::messagesAdded(const QMailMessageIdList &ids)
{
    processFilters(ids, &QMFStorePrivate::messageAdded);
}

void QMFStorePrivate::messagesRemoved(const QMailMessageIdList &ids)
{
    processFilters(ids, &QMFStorePrivate::messageRemoved);
}

void QMFStorePrivate::messagesUpdated(const QMailMessageIdList &ids)
{
    processFilters(ids, &QMFStorePrivate::messageUpdated);
}

void QMFStorePrivate::processFilters(const QMailMessageIdList &ids, void (QMFStorePrivate::*signal)(const QMessageId &, const QMessageManager::NotificationFilterIdSet &))
{
    QMap<QMailMessageId, QMessageManager::NotificationFilterIdSet> matches;

    // Copy the filter map to protect against modification during traversal
    QMap<QMessageManager::NotificationFilterId, QMailMessageKey> filters(_filters);
    QMap<QMessageManager::NotificationFilterId, QMailMessageKey>::const_iterator it = filters.begin(), end = filters.end();
    for ( ; it != end; ++it) {
	const QMailMessageKey &key(it.value());

	QSet<QMailMessageId> filteredIds;
	if (!key.isEmpty()) {
	    // Empty key matches all messages; otherwise get the filtered set
	    filteredIds = QMailStore::instance()->queryMessages(key).toSet();
	}

	foreach (const QMailMessageId &id, ids) {
	    if (key.isEmpty() || filteredIds.contains(id)) {
		matches[id].insert(it.key());
	    }
	}
    }

    QMap<QMailMessageId, QMessageManager::NotificationFilterIdSet>::const_iterator mit = matches.begin(), mend = matches.end();
    for ( ; mit != mend; ++mit) {
	emit (this->*signal)(QmfHelpers::convert(mit.key()), mit.value());
    }
}

namespace QmfHelpers {

QMailStore *convert(QMFStore *store)
{
    return QMFStorePrivate::convert(store);
}

}

QMFStore::QMFStore(QObject *parent)
    : QObject(parent),
      d_ptr(new QMFStorePrivate)
{
    connect(d_ptr, SIGNAL(messageAdded(QMessageId, QMessageManager::NotificationFilterIdSet)), this, SIGNAL(messageAdded(QMessageId, QMessageManager::NotificationFilterIdSet)));
    connect(d_ptr, SIGNAL(messageRemoved(QMessageId, QMessageManager::NotificationFilterIdSet)), this, SIGNAL(messageRemoved(QMessageId, QMessageManager::NotificationFilterIdSet)));
    connect(d_ptr, SIGNAL(messageUpdated(QMessageId, QMessageManager::NotificationFilterIdSet)), this, SIGNAL(messageUpdated(QMessageId, QMessageManager::NotificationFilterIdSet)));
}

QMFStore::~QMFStore()
{
    delete d_ptr;
}

QMessageManager::Error QMFStore::error() const
{
    return convert(d_ptr->_store->lastError());
}

QMessageIdList QMFStore::queryMessages(const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, 
				       uint offset, QMessageManager::Error &error) const
{
    if (filter.matchFlags() != 0) {
        error = QMessageManager::NotYetImplemented;
        return QMessageIdList();
    }
    
    error = QMessageManager::NoError;
    return convert(d_ptr->_store->queryMessages(convert(filter), convert(sortOrder), limit, offset));
}

QMessageIdList QMFStore::queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, 
				       const QMessageSortOrder &sortOrder, uint limit, uint offset, QMessageManager::Error &error) const
{
    if (filter.matchFlags() != 0) {
        error = QMessageManager::NotYetImplemented;
        return QMessageIdList();
    }
    
    error = QMessageManager::NoError;
    QMessageIdList ids(convert(d_ptr->_store->queryMessages(convert(filter), convert(sortOrder))));
    
    Qt::CaseSensitivity searchOptions(Qt::CaseInsensitive);
    if (matchFlags & QMessageDataComparator::MatchCaseSensitive) {
        searchOptions = Qt::CaseSensitive;
    }
    
    QMessageIdList result;
    foreach (QMessageId id, ids) {
        QMessage message(id);
        QMessageContentContainer bodyContainer(message.find(message.bodyId()));
        if (bodyContainer.textContent().contains(body, searchOptions)) {
            result.append(id);
            if (limit && ((uint)result.count() >= offset + limit)) {
                break;
            }
        }
    }
    
    if (offset) {
        return result.mid(offset, (limit ? limit : -1));
    } else {
        return result;
    }
}

QMessageFolderIdList QMFStore::queryFolders(const QMessageFolderFilter &filter, const QMessageFolderSortOrder &sortOrder, 
					    uint limit, uint offset, QMessageManager::Error &error) const
{
    if (filter.matchFlags() != 0) {
        error = QMessageManager::NotYetImplemented;
        return QMessageFolderIdList();
    }
    
    error = QMessageManager::NoError;
    return convert(d_ptr->_store->queryFolders(convert(filter), convert(sortOrder), limit, offset));
}

QMessageAccountIdList QMFStore::queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountSortOrder &sortOrder, 
					      uint limit, uint offset, QMessageManager::Error &error) const
{
    if (filter.matchFlags() != 0) {
        error = QMessageManager::NotYetImplemented;
        return QMessageAccountIdList();
    }

    error = QMessageManager::NoError;
    return convert(d_ptr->_store->queryAccounts(convert(filter), convert(sortOrder), limit, offset));
}

int QMFStore::countMessages(const QMessageFilter& filter, QMessageManager::Error &error) const
{
    if (filter.matchFlags() != 0) {
        error = QMessageManager::NotYetImplemented;
        return 0;
    }
    
    error = QMessageManager::NoError;
    return d_ptr->_store->countMessages(convert(filter));
}

int QMFStore::countFolders(const QMessageFolderFilter& filter, QMessageManager::Error &error) const
{
    if (filter.matchFlags() != 0) {
        error = QMessageManager::NotYetImplemented;
        return 0;
    }
    
    error = QMessageManager::NoError;
    return d_ptr->_store->countFolders(convert(filter));
}

int QMFStore::countAccounts(const QMessageAccountFilter& filter, QMessageManager::Error &error) const
{
    if (filter.matchFlags() != 0) {
        error = QMessageManager::NotYetImplemented;
        return 0;
    }
    
    error = QMessageManager::NoError;
    return d_ptr->_store->countAccounts(convert(filter));
}

bool QMFStore::removeMessage(const QMessageId& id, QMessageManager::RemovalOption option, QMessageManager::Error &error)
{
    error = QMessageManager::NoError;
    return d_ptr->_store->removeMessage(convert(id), convert(option));
}

bool QMFStore::removeMessages(const QMessageFilter& filter, QMessageManager::RemovalOption option, QMessageManager::Error &error)
{
    error = QMessageManager::NoError;
    return d_ptr->_store->removeMessages(convert(filter), convert(option));
}

struct TextPartLocator
{
    QMailMessagePart::Location _location;

    bool operator()(const QMailMessagePart &part)
    {
        if (part.contentType().type().toLower() == "text") {
            _location = part.location();
            return false;
        }

        return true;
    }
};


bool QMFStore::addMessage(QMessage &m, QMessageManager::Error &error)
{
    QMailMessage msg(convert(m));

    error = QMessageManager::NoError;

    bool retVal = d_ptr->_store->addMessage(&msg);
    if (retVal) {
	// Update message
	QMessagePrivate *p = QMessagePrivate::implementation(m);
	p->_id = convert(msg.id());
	p->_contentIdentifier = msg.contentIdentifier();
	p->_contentScheme = msg.contentScheme();
    }

    return retVal;
}

bool QMFStore::updateMessage(QMessage &m, QMessageManager::Error &error)
{
    QMailMessage msg(convert(m));

    error = QMessageManager::NoError;

    return d_ptr->_store->updateMessage(&msg);
}

QMessage QMFStore::message(const QMessageId& id, QMessageManager::Error &error) const
{
    error = QMessageManager::NoError;
    return convert(d_ptr->_store->message(convert(id)));
}

QMessageFolder QMFStore::folder(const QMessageFolderId& id, QMessageManager::Error &error) const
{
    error = QMessageManager::NoError;
    return convert(d_ptr->_store->folder(convert(id)));
}

QMessageAccount QMFStore::account(const QMessageAccountId& id, QMessageManager::Error &error) const
{
    error = QMessageManager::NoError;
    return convert(d_ptr->_store->account(convert(id)));
}

QMessageManager::NotificationFilterId QMFStore::registerNotificationFilter(const QMessageFilter &filter)
{
    QMessageManager::NotificationFilterId filterId = ++d_ptr->_filterId;
    d_ptr->_filters.insert(filterId, QmfHelpers::convert(filter));
    return filterId;
}

void QMFStore::unregisterNotificationFilter(QMessageManager::NotificationFilterId notificationFilterId)
{
    d_ptr->_filters.remove(notificationFilterId);
}

QMessageAccountId QMFStore::defaultAccount() const
{
    QMailAccountKey typeKey(QMailAccountKey::messageType(convert(QMessage::Email)));
    QMailAccountKey enabledKey(QMailAccountKey::status(QMailAccount::Enabled));

    // See if there is a preferred sender
    QMailAccountKey preferredKey(QMailAccountKey::status(QMailAccount::PreferredSender));
    foreach (const QMailAccountId &id, d_ptr->_store->queryAccounts(typeKey & enabledKey & preferredKey)) {
	return convert(id);
    }

    // See if there are any sending accounts for this type
    QMailAccountKey capableKey(QMailAccountKey::status(QMailAccount::CanTransmit));
    foreach (const QMailAccountId &id, d_ptr->_store->queryAccounts(typeKey & enabledKey & capableKey)) {
	return convert(id);
    }

    return QMessageAccountId();
}

QByteArray QMFStore::getMimePart(const QMessageId &messageId, const QMessageContentContainerId &contId)
{
    QByteArray partData;
    QMailMessageId id(QmfHelpers::convert(messageId));
    QMailMessagePart::Location location(QmfHelpers::convert(contId));

    QMailMessage message = d_ptr->_store->message(id);

    if (message.contains(location)) {
	QMailMessagePart &part = message.partAt(location);
	if (part.contentAvailable()) {
	    partData = part.body().data(QMailMessageBodyFwd::Decoded);
	}
    }
    return partData;
}

QMFStore* QMFStore::instance()
{
    static bool initialised(false);

    QMFStore* store(QMFStorePrivate::storeInstance());

    if (!initialised) {
        initialised = true;

        QMailStore *mailStore(convert(store));

        // Connect the store's signals
        QMFStorePrivate *storeImpl(store->d_ptr);
        connect(mailStore, SIGNAL(messagesAdded(QMailMessageIdList)), storeImpl, SLOT(messagesAdded(QMailMessageIdList)));
        connect(mailStore, SIGNAL(messagesRemoved(QMailMessageIdList)), storeImpl, SLOT(messagesRemoved(QMailMessageIdList)));
        connect(mailStore, SIGNAL(messagesUpdated(QMailMessageIdList)), storeImpl, SLOT(messagesUpdated(QMailMessageIdList)));

        // Perform any initialisation tasks
        QMFStorePrivate::registerMessageStatus(mailStore, "QMessage::HighPriority");
        QMFStorePrivate::registerMessageStatus(mailStore, "QMessage::LowPriority");

        // Create the standard folders if they do not exist
        typedef QPair<const char*, quint64> FolderAttributes;
        foreach (const FolderAttributes &folder, QList<FolderAttributes>() << FolderAttributes("Inbox", QMailFolder::Incoming)
                                                                           << FolderAttributes("Outbox", QMailFolder::Outgoing)
                                                                           << FolderAttributes("Drafts", QMailFolder::Outgoing | QMailFolder::Trash)
                                                                           << FolderAttributes("Sent", QMailFolder::Outgoing | QMailFolder::Sent)
                                                                           << FolderAttributes("Trash", QMailFolder::Trash)) {
            QMFStorePrivate::createNonexistentFolder(mailStore, folder.first, folder.second);
        }
    }

    return store;
}

#include "moc_qmfstore_maemo6_p.cpp"
    
QTM_END_NAMESPACE
