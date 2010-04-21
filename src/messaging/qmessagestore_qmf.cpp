/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessagestore.h"
#include "qmessagemanager.h"
#include "qmfhelpers_p.h"

#include <qmailstore.h>

#include <QCoreApplication>
#include <QEvent>

#include "qmessagestore_qmf_p.h"

using namespace QTM_PREPEND_NAMESPACE(QmfHelpers);


QTM_BEGIN_NAMESPACE

Q_SCOPED_STATIC_DEFINE(QMessageStore,QMessageStorePrivate,storeInstance);

QMailStore *QMessageStorePrivate::convert(QMessageStore *store)
{
    return store->d_ptr->_store;
}

QMailStore *QMessageStorePrivate::convert(QMessageManager &manager)
{
    return manager.store->d_ptr->_store;
}

void QMessageStorePrivate::registerMessageStatus(QMailStore *store, const QString &field)
{
    store->registerMessageStatusFlag(field);
}

void QMessageStorePrivate::createNonexistentFolder(QMailStore *store, const QString &path, quint64 status)
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

void QMessageStorePrivate::messagesAdded(const QMailMessageIdList &ids)
{
    processFilters(ids, &QMessageStorePrivate::messageAdded);
}

void QMessageStorePrivate::messagesRemoved(const QMailMessageIdList &ids)
{
    processFilters(ids, &QMessageStorePrivate::messageRemoved);
}

void QMessageStorePrivate::messagesUpdated(const QMailMessageIdList &ids)
{
    processFilters(ids, &QMessageStorePrivate::messageUpdated);
}

namespace QmfHelpers {

QMailStore *convert(QMessageStore *store)
{
    return QMessageStorePrivate::convert(store);
}

QMailStore *convert(QMessageManager &manager)
{
    return QMessageStorePrivate::convert(manager);
}

}

QMessageStore::QMessageStore(QObject *parent)
    : QObject(parent),
      d_ptr(new QMessageStorePrivate)
{
    connect(d_ptr, SIGNAL(messageAdded(QMessageId, QMessageManager::NotificationFilterIdSet)), this, SIGNAL(messageAdded(QMessageId, QMessageManager::NotificationFilterIdSet)));
    connect(d_ptr, SIGNAL(messageRemoved(QMessageId, QMessageManager::NotificationFilterIdSet)), this, SIGNAL(messageRemoved(QMessageId, QMessageManager::NotificationFilterIdSet)));
    connect(d_ptr, SIGNAL(messageUpdated(QMessageId, QMessageManager::NotificationFilterIdSet)), this, SIGNAL(messageUpdated(QMessageId, QMessageManager::NotificationFilterIdSet)));
}

QMessageStore::~QMessageStore()
{
    delete d_ptr;
}

QMessageManager::Error QMessageStore::error() const
{
    if (d_ptr->_error != QMessageManager::NoError) {
        return d_ptr->_error;
    }

    return convert(d_ptr->_store->lastError());
}

QMessageIdList QMessageStore::queryMessages(const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    if (filter.matchFlags() != 0) {
        d_ptr->_error = QMessageManager::NotYetImplemented;
        return QMessageIdList();
    }
    
    d_ptr->_error = QMessageManager::NoError;
    return convert(d_ptr->_store->queryMessages(convert(filter), convert(sortOrder), limit, offset));
}

QMessageIdList QMessageStore::queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    if (filter.matchFlags() != 0) {
        d_ptr->_error = QMessageManager::NotYetImplemented;
        return QMessageIdList();
    }
    
    d_ptr->_error = QMessageManager::NoError;
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

QMessageFolderIdList QMessageStore::queryFolders(const QMessageFolderFilter &filter, const QMessageFolderSortOrder &sortOrder, uint limit, uint offset) const
{
    if (filter.matchFlags() != 0) {
        d_ptr->_error = QMessageManager::NotYetImplemented;
        return QMessageFolderIdList();
    }
    
    d_ptr->_error = QMessageManager::NoError;
    return convert(d_ptr->_store->queryFolders(convert(filter), convert(sortOrder), limit, offset));
}

QMessageAccountIdList QMessageStore::queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountSortOrder &sortOrder, uint limit, uint offset) const
{
    if (filter.matchFlags() != 0) {
        d_ptr->_error = QMessageManager::NotYetImplemented;
        return QMessageAccountIdList();
    }
    
    d_ptr->_error = QMessageManager::NoError;
    return convert(d_ptr->_store->queryAccounts(convert(filter), convert(sortOrder), limit, offset));
}

int QMessageStore::countMessages(const QMessageFilter& filter) const
{
    if (filter.matchFlags() != 0) {
        d_ptr->_error = QMessageManager::NotYetImplemented;
        return 0;
    }
    
    d_ptr->_error = QMessageManager::NoError;
    return d_ptr->_store->countMessages(convert(filter));
}

int QMessageStore::countFolders(const QMessageFolderFilter& filter) const
{
    if (filter.matchFlags() != 0) {
        d_ptr->_error = QMessageManager::NotYetImplemented;
        return 0;
    }
    
    d_ptr->_error = QMessageManager::NoError;
    return d_ptr->_store->countFolders(convert(filter));
}

int QMessageStore::countAccounts(const QMessageAccountFilter& filter) const
{
    if (filter.matchFlags() != 0) {
        d_ptr->_error = QMessageManager::NotYetImplemented;
        return 0;
    }
    
    d_ptr->_error = QMessageManager::NoError;
    return d_ptr->_store->countAccounts(convert(filter));
}

bool QMessageStore::removeMessage(const QMessageId& id, QMessageManager::RemovalOption option)
{
    d_ptr->_error = QMessageManager::NoError;
    return d_ptr->_store->removeMessage(convert(id), convert(option));
}

bool QMessageStore::removeMessages(const QMessageFilter& filter, QMessageManager::RemovalOption option)
{
    d_ptr->_error = QMessageManager::NoError;
    return d_ptr->_store->removeMessages(convert(filter), convert(option));
}

bool QMessageStore::addMessage(QMessage *m)
{
    // Ensure that the size estimate is updated if necessary
    (void)m->size();

    QMailMessage *msg(convert(m));

    d_ptr->_error = QMessageManager::NoError;
    return d_ptr->_store->addMessage(msg);
}

bool QMessageStore::updateMessage(QMessage *m)
{
    // Ensure that the size estimate is updated if necessary
    (void)m->size();

    QMailMessage msg(convert(*m));

    d_ptr->_error = QMessageManager::NoError;
    return d_ptr->_store->updateMessage(&msg);
}

QMessage QMessageStore::message(const QMessageId& id) const
{
    d_ptr->_error = QMessageManager::NoError;
    return convert(d_ptr->_store->message(convert(id)));
}

QMessageFolder QMessageStore::folder(const QMessageFolderId& id) const
{
    d_ptr->_error = QMessageManager::NoError;
    return convert(d_ptr->_store->folder(convert(id)));
}

QMessageAccount QMessageStore::account(const QMessageAccountId& id) const
{
    d_ptr->_error = QMessageManager::NoError;
    return convert(d_ptr->_store->account(convert(id)));
}

QMessageManager::NotificationFilterId QMessageStore::registerNotificationFilter(const QMessageFilter &filter)
{
    QMessageManager::NotificationFilterId filterId = ++d_ptr->_filterId;
    d_ptr->_filters.insert(filterId, QmfHelpers::convert(filter));
    return filterId;
}

void QMessageStore::unregisterNotificationFilter(QMessageManager::NotificationFilterId notificationFilterId)
{
    d_ptr->_filters.remove(notificationFilterId);
}

QMessageStore* QMessageStore::instance()
{
    static bool initialised(false);

    QMessageStore* store(QMessageStorePrivate::storeInstance());

    if (!initialised) {
        initialised = true;

        QMailStore *mailStore(convert(store));

        // Connect the store's signals
        QMessageStorePrivate *storeImpl(store->d_ptr);
        connect(mailStore, SIGNAL(messagesAdded(QMailMessageIdList)), storeImpl, SLOT(messagesAdded(QMailMessageIdList)));
        connect(mailStore, SIGNAL(messagesRemoved(QMailMessageIdList)), storeImpl, SLOT(messagesRemoved(QMailMessageIdList)));
        connect(mailStore, SIGNAL(messagesUpdated(QMailMessageIdList)), storeImpl, SLOT(messagesUpdated(QMailMessageIdList)));

        // Perform any initialisation tasks
        QMessageStorePrivate::registerMessageStatus(mailStore, "QMessage::HighPriority");
        QMessageStorePrivate::registerMessageStatus(mailStore, "QMessage::LowPriority");

        // Create the standard folders if they do not exist
        typedef QPair<const char*, quint64> FolderAttributes;
        foreach (const FolderAttributes &folder, QList<FolderAttributes>() << FolderAttributes("Inbox", QMailFolder::Incoming)
                                                                           << FolderAttributes("Outbox", QMailFolder::Outgoing)
                                                                           << FolderAttributes("Drafts", QMailFolder::Outgoing | QMailFolder::Trash)
                                                                           << FolderAttributes("Sent", QMailFolder::Outgoing | QMailFolder::Sent)
                                                                           << FolderAttributes("Trash", QMailFolder::Trash)) {
            QMessageStorePrivate::createNonexistentFolder(mailStore, folder.first, folder.second);
        }
    }

    return store;
}

#include "moc_qmessagestore_qmf_p.cpp"
    
QTM_END_NAMESPACE
