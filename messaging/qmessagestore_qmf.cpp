/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessagestore.h"
#include "qmfhelpers_p.h"

#include <qmailstore.h>

#include <QCoreApplication>
#include <QEvent>

using namespace QmfHelpers;

class QMessageStorePrivate : public QObject
{
    Q_OBJECT

public:
    QMessageStorePrivate() 
        : QObject(), 
          _store(QMailStore::instance()), 
          _error(QMessageStore::NoError), 
          _notify(true),
          _filterId(0)
    {
    }

    QMailStore *_store;
    QMessageStore::ErrorCode _error;
    bool _notify;
    QMessageStore::NotificationFilterId _filterId;
    QMap<QMessageStore::NotificationFilterId, QMailMessageKey> _filters;

    static QMailStore *convert(QMessageStore *store);

    Q_SCOPED_STATIC_DECLARE(QMessageStore,storeInstance);

    static void registerMessageStatus(QMailStore *store, const QString &field);
    static void createNonexistentFolder(QMailStore *store, const QString &path, quint64 status);
    
public slots:
    void messagesAdded(const QMailMessageIdList &ids);
    void messagesRemoved(const QMailMessageIdList &ids);
    void messagesUpdated(const QMailMessageIdList &ids);

signals:
    void messageAdded(const QMessageId &id, const QSet<QMessageStore::NotificationFilterId> &matchingFilterIds);
    void messageRemoved(const QMessageId &id, const QSet<QMessageStore::NotificationFilterId> &matchingFilterIds);
    void messageUpdated(const QMessageId &id, const QSet<QMessageStore::NotificationFilterId> &matchingFilterIds);

private:
    void processFilters(const QMailMessageIdList &ids, void (QMessageStorePrivate::*signal)(const QMessageId &, const QSet<QMessageStore::NotificationFilterId> &))
    {
        QMap<QMailMessageId, QSet<QMessageStore::NotificationFilterId> > matches;

        // Copy the filter map to protect against modification during traversal
        QMap<QMessageStore::NotificationFilterId, QMailMessageKey> filters(_filters);
        QMap<QMessageStore::NotificationFilterId, QMailMessageKey>::const_iterator it = filters.begin(), end = filters.end();
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

        QMap<QMailMessageId, QSet<QMessageStore::NotificationFilterId> >::const_iterator mit = matches.begin(), mend = matches.end();
        for ( ; mit != mend; ++mit) {
            emit (this->*signal)(QmfHelpers::convert(mit.key()), mit.value());
        }
    }
};

Q_SCOPED_STATIC_DEFINE(QMessageStore,QMessageStorePrivate,storeInstance);

QMailStore *QMessageStorePrivate::convert(QMessageStore *store)
{
    return store->d_ptr->_store;
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

}

QMessageStore::QMessageStore(QObject *parent)
    : QObject(parent),
      d_ptr(new QMessageStorePrivate)
{
    connect(d_ptr, SIGNAL(messageAdded(QMessageId, QSet<QMessageStore::NotificationFilterId>)), this, SIGNAL(messageAdded(QMessageId, QSet<QMessageStore::NotificationFilterId>)));
    connect(d_ptr, SIGNAL(messageRemoved(QMessageId, QSet<QMessageStore::NotificationFilterId>)), this, SIGNAL(messageRemoved(QMessageId, QSet<QMessageStore::NotificationFilterId>)));
    connect(d_ptr, SIGNAL(messageUpdated(QMessageId, QSet<QMessageStore::NotificationFilterId>)), this, SIGNAL(messageUpdated(QMessageId, QSet<QMessageStore::NotificationFilterId>)));
}

QMessageStore::~QMessageStore()
{
    delete d_ptr;
}

QMessageStore::ErrorCode QMessageStore::lastError() const
{
    if (d_ptr->_error != QMessageStore::NoError) {
        return d_ptr->_error;
    }

    return convert(d_ptr->_store->lastError());
}

QMessageIdList QMessageStore::queryMessages(const QMessageFilter &filter, const QMessageOrdering &ordering, uint limit, uint offset) const
{
    if (filter.options() != 0) {
        d_ptr->_error = QMessageStore::NotYetImplemented;
        return QMessageIdList();
    }
    
    d_ptr->_error = QMessageStore::NoError;
    return convert(d_ptr->_store->queryMessages(convert(filter), convert(ordering), limit, offset));
}

QMessageIdList QMessageStore::queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::Options options, const QMessageOrdering &ordering, uint limit, uint offset) const
{
    Q_UNUSED(filter)
    Q_UNUSED(ordering)
    Q_UNUSED(body)
    Q_UNUSED(options)
    Q_UNUSED(limit)
    Q_UNUSED(offset)
    return QMessageIdList(); // stub
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
QMessageFolderIdList QMessageStore::queryFolders(const QMessageFolderFilter &filter, const QMessageFolderOrdering &ordering, uint limit, uint offset) const
{
    if (filter.options() != 0) {
        d_ptr->_error = QMessageStore::NotYetImplemented;
        return QMessageFolderIdList();
    }
    
    d_ptr->_error = QMessageStore::NoError;
    return convert(d_ptr->_store->queryFolders(convert(filter), convert(ordering), limit, offset));
}
#endif

QMessageAccountIdList QMessageStore::queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountOrdering &ordering, uint limit, uint offset) const
{
    if (filter.options() != 0) {
        d_ptr->_error = QMessageStore::NotYetImplemented;
        return QMessageAccountIdList();
    }
    
    d_ptr->_error = QMessageStore::NoError;
    return convert(d_ptr->_store->queryAccounts(convert(filter), convert(ordering), limit, offset));
}

int QMessageStore::countMessages(const QMessageFilter& filter) const
{
    if (filter.options() != 0) {
        d_ptr->_error = QMessageStore::NotYetImplemented;
        return 0;
    }
    
    d_ptr->_error = QMessageStore::NoError;
    return d_ptr->_store->countMessages(convert(filter));
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
int QMessageStore::countFolders(const QMessageFolderFilter& filter) const
{
    if (filter.options() != 0) {
        d_ptr->_error = QMessageStore::NotYetImplemented;
        return 0;
    }
    
    d_ptr->_error = QMessageStore::NoError;
    return d_ptr->_store->countFolders(convert(filter));
}
#endif

int QMessageStore::countAccounts(const QMessageAccountFilter& filter) const
{
    if (filter.options() != 0) {
        d_ptr->_error = QMessageStore::NotYetImplemented;
        return 0;
    }
    
    d_ptr->_error = QMessageStore::NoError;
    return d_ptr->_store->countAccounts(convert(filter));
}

bool QMessageStore::removeMessage(const QMessageId& id, QMessageStore::RemovalOption option)
{
    d_ptr->_error = QMessageStore::NoError;
    return d_ptr->_store->removeMessage(convert(id), convert(option));
}

bool QMessageStore::removeMessages(const QMessageFilter& filter, QMessageStore::RemovalOption option)
{
    d_ptr->_error = QMessageStore::NoError;
    return d_ptr->_store->removeMessages(convert(filter), convert(option));
}

bool QMessageStore::addMessage(QMessage *m)
{
    // Ensure that the size estimate is updated if necessary
    (void)m->size();

    QMailMessage msg(convert(*m));

    d_ptr->_error = QMessageStore::NoError;
    return d_ptr->_store->addMessage(&msg);
}

bool QMessageStore::updateMessage(QMessage *m)
{
    // Ensure that the size estimate is updated if necessary
    (void)m->size();

    QMailMessage msg(convert(*m));

    d_ptr->_error = QMessageStore::NoError;
    return d_ptr->_store->updateMessage(&msg);
}

QMessage QMessageStore::message(const QMessageId& id) const
{
    d_ptr->_error = QMessageStore::NoError;
    return convert(d_ptr->_store->message(convert(id)));
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
QMessageFolder QMessageStore::folder(const QMessageFolderId& id) const
{
    d_ptr->_error = QMessageStore::NoError;
    return convert(d_ptr->_store->folder(convert(id)));
}
#endif

QMessageAccount QMessageStore::account(const QMessageAccountId& id) const
{
    d_ptr->_error = QMessageStore::NoError;
    return convert(d_ptr->_store->account(convert(id)));
}

QMessageStore::NotificationFilterId QMessageStore::registerNotificationFilter(const QMessageFilter &filter)
{
    QMessageStore::NotificationFilterId filterId = ++d_ptr->_filterId;
    d_ptr->_filters.insert(filterId, QmfHelpers::convert(filter));
    return filterId;
}

void QMessageStore::unregisterNotificationFilter(NotificationFilterId notificationFilterId)
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
    
#include "qmessagestore_qmf.moc"

