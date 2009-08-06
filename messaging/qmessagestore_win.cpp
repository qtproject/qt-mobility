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
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessagestore_p.h"
#include "qmessage_p.h"
#include "qmessageid_p.h"
#include "qmessageaccountid_p.h"
#include "qmessagefolder_p.h"
#include "qmessageaccount_p.h"
#include "winhelpers_p.h"

#include <qdebug.h>

class QMessageStorePrivatePlatform
{
public:
    QMessageStorePrivatePlatform(QMessageStorePrivate *d, QMessageStore *q);
    ~QMessageStorePrivatePlatform();

    QMessageStorePrivate *d_ptr;
    QMessageStore *q_ptr;
    QMessageStore::ErrorCode lastError;

    bool _mapiInitialized;
};

QMessageStorePrivatePlatform::QMessageStorePrivatePlatform(QMessageStorePrivate *d, QMessageStore *q)
    :d_ptr(d), 
     q_ptr(q)
{
    _mapiInitialized = false;
    lastError = QMessageStore::NoError;
#ifndef QT_NO_THREAD
    // Note MAPIINIT is ignored on Windows Mobile but used on Outlook 2007 see
    // msdn ms862621 vs cc842343
    MAPIINIT_0 MAPIINIT = { 0, MAPI_MULTITHREAD_NOTIFICATIONS };
    if (MAPIInitialize(&MAPIINIT) == S_OK)
        _mapiInitialized = true;
#else
    if (MAPIInitialize(0) == S_OK)
        _mapiInitialized = true;
#endif
    if (!_mapiInitialized) {
        qWarning() << "Failed to initialize MAPI";
        lastError = QMessageStore::ContentInaccessible;
    }
}

QMessageStorePrivatePlatform::~QMessageStorePrivatePlatform()
{
    MAPIUninitialize();
}

QMessageStorePrivate::QMessageStorePrivate()
    :p_ptr(0),
     q_ptr(0)
{
}

void QMessageStorePrivate::initialize(QMessageStore *store)
{
    q_ptr = store;
    p_ptr = new QMessageStorePrivatePlatform(this, store);
}

Q_GLOBAL_STATIC(QMessageStorePrivate,data);

QMessageStore::QMessageStore(QObject *parent)
    : QObject(parent),
      d_ptr(data())
{
    Q_ASSERT(d_ptr != 0);
    Q_ASSERT(d_ptr->q_ptr == 0); // QMessageStore should be singleton
    d_ptr->initialize(this);
}

QMessageStore::~QMessageStore()
{
    d_ptr = 0; // should be cleaned up by automatically
}

QMessageStore* QMessageStore::instance()
{
    QMessageStorePrivate *d = data();
    Q_ASSERT(d != 0);
    if (!d->q_ptr)
        d->initialize(new QMessageStore());
    return d->q_ptr;
}

QMessageStore::ErrorCode QMessageStore::lastError() const
{
    return d_ptr->p_ptr->lastError;
}

QMessageIdList QMessageStore::queryMessages(const QMessageFilterKey &key, const QMessageSortKey &sortKey, uint limit, uint offset) const
{
    Q_UNUSED(key)
    Q_UNUSED(sortKey)
    QMessageIdList result;
    d_ptr->p_ptr->lastError = QMessageStore::NoError;

    MapiSessionPtr mapiSession(new MapiSession(&d_ptr->p_ptr->lastError, d_ptr->p_ptr->_mapiInitialized));
    if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
        return result;

    MapiStorePtr mapiStore(mapiSession->defaultStore(&d_ptr->p_ptr->lastError));
    if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
        return result;

    uint workingLimit(offset + limit); // TODO: Improve this it's horribly inefficient
    QStringList path;
    QList<MapiFolderPtr> folders; // depth first search;

    folders.append(mapiStore->rootFolder(&d_ptr->p_ptr->lastError));
    if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
        return result;
    path.append(folders.back()->name());
    while (!folders.isEmpty() && (!limit || workingLimit)) {
        if (!folders.back()->isValid()) {
            folders.pop_back();
            path.pop_back();
            continue;
        }

        MapiFolderPtr folder(folders.back()->nextSubFolder(&d_ptr->p_ptr->lastError));
        if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
            return result;
        if (folder->isValid() && (!limit || workingLimit)) {
            uint oldCount(result.count());
            folders.append(folder);
            path.append(folder->name());
            qDebug() << "Path: " << path; // TODO remove debug;
            result.append(folder->queryMessages(&d_ptr->p_ptr->lastError, QMessageFilterKey(), QMessageSortKey(), workingLimit, 0));
            if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
                return result;
            if (limit)
                workingLimit -= (result.count() - oldCount);
        } else {
            folders.pop_back();
            path.pop_back();
        }
    }
    folders.clear();

    return result.mid(offset); // stub
}

QMessageIdList QMessageStore::queryMessages(const QString &body, const QMessageFilterKey &key, const QMessageSortKey &sortKey,  QMessageDataComparator::Options options, uint limit, uint offset) const
{
    Q_UNUSED(key)
    Q_UNUSED(sortKey)
    Q_UNUSED(body)
    Q_UNUSED(options)
    Q_UNUSED(limit)
    Q_UNUSED(offset)
    return QMessageIdList(); // stub
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
QMessageFolderIdList QMessageStore::queryFolders(const QMessageFolderFilterKey &key, const QMessageFolderSortKey &sortKey, uint limit, uint offset) const
{
    Q_UNUSED(key)
    Q_UNUSED(sortKey)
    QMessageFolderIdList result;
    d_ptr->p_ptr->lastError = QMessageStore::NoError;

    MapiSessionPtr mapiSession(new MapiSession(&d_ptr->p_ptr->lastError, d_ptr->p_ptr->_mapiInitialized));
     if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
        return result;

    MapiStorePtr mapiStore(mapiSession->defaultStore(&d_ptr->p_ptr->lastError));
    if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
        return result;

    return mapiStore->folderIds(&d_ptr->p_ptr->lastError).mid(offset, limit);
}
#endif

QMessageAccountIdList QMessageStore::queryAccounts(const QMessageAccountFilterKey &key, const QMessageAccountSortKey &sortKey, uint limit, uint offset) const
{
    Q_UNUSED(key)
    Q_UNUSED(sortKey)
    Q_UNUSED(limit)
    Q_UNUSED(offset)
    //TODO: For Windows desktop there is only one 'account' the defaultStore.
    //TODO: But for Windows mobile an account for the 'SMS' message store should also be handled/returned.
    QMessageAccountIdList result;
    d_ptr->p_ptr->lastError = QMessageStore::NoError;

    MapiSessionPtr mapiSession(new MapiSession(&d_ptr->p_ptr->lastError, d_ptr->p_ptr->_mapiInitialized));
    if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
        return result;

    MapiStorePtr mapiStore(mapiSession->defaultStore(&d_ptr->p_ptr->lastError));
    if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
        return result;

    result.append(QMessageAccountId(mapiStore->id()));
    // TODO for Windows Mobile only, also include the store named "SMS" if it exists.
    return result;
}

int QMessageStore::countMessages(const QMessageFilterKey& key) const
{
    return queryMessages(key).count();
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
int QMessageStore::countFolders(const QMessageFolderFilterKey& key) const
{
    return queryFolders(key).count();
}
#endif

int QMessageStore::countAccounts(const QMessageAccountFilterKey& key) const
{
    return queryAccounts(key).count();
}

bool QMessageStore::removeMessage(const QMessageId& id, RemovalOption option)
{
    Q_UNUSED(id)
    Q_UNUSED(option)
    return false; // stub
}

bool QMessageStore::removeMessages(const QMessageFilterKey& key, QMessageStore::RemovalOption option)
{
    Q_UNUSED(key)
    Q_UNUSED(option)
    return true; // stub
}

bool QMessageStore::addMessage(QMessage *m)
{
    Q_UNUSED(m)
    return true; // stub
}

bool QMessageStore::updateMessage(QMessage *m)
{
    Q_UNUSED(m)
    return true; // stub
}

QMessage QMessageStore::message(const QMessageId& id) const
{
    QMessage result;
    QMessageId newId(id);
    QMessageStore::ErrorCode ignoredError;
    d_ptr->p_ptr->lastError = QMessageStore::NoError;

    MapiSessionPtr mapiSession(new MapiSession(&d_ptr->p_ptr->lastError, d_ptr->p_ptr->_mapiInitialized));
    if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
        return result;

    MapiEntryId entryId(QMessageIdPrivate::entryId(id));
    MapiRecordKey messageRecordKey(QMessageIdPrivate::messageRecordKey(id));
    MapiRecordKey folderRecordKey(QMessageIdPrivate::folderRecordKey(id));
    MapiRecordKey storeRecordKey(QMessageIdPrivate::storeRecordKey(id));
    MapiStorePtr mapiStore(mapiSession->findStore(&d_ptr->p_ptr->lastError, QMessageAccountIdPrivate::from(storeRecordKey)));
    if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
        return result;

    LPMESSAGE message;
#if 0 // force lookup using record keys, TODO remove
    if (true) {
#else
    if (mapiSession->openEntry(&ignoredError, entryId, &message) != S_OK) {
#endif
        MapiFolderPtr parentFolder(mapiStore->findFolder(&d_ptr->p_ptr->lastError, folderRecordKey));
        if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
            return result;

        qDebug() << "parentFolder" << parentFolder->name(); // TODO remove this
        entryId = parentFolder->messageEntryId(&d_ptr->p_ptr->lastError, messageRecordKey);
        if (!entryId.count() || (mapiSession->openEntry(&d_ptr->p_ptr->lastError, entryId, &message) != S_OK)) {
            d_ptr->p_ptr->lastError = InvalidId;
            return result;
        }

        newId = QMessageIdPrivate::from(messageRecordKey, folderRecordKey, storeRecordKey, entryId);
    }

    const int nCols(4);
    enum { recordKeyColumn = 0, flagsColumn, senderColumn, subjectColumn };
    SizedSPropTagArray(nCols, columns) = {nCols, {PR_RECORD_KEY, PR_MESSAGE_FLAGS, PR_SENDER_NAME, PR_SUBJECT}};
    ULONG count;
    LPSPropValue properties;
    if (message->GetProps(reinterpret_cast<LPSPropTagArray>(&columns), MAPI_UNICODE, &count, &properties) == S_OK) {
        QString sender(stringFromLpctstr(properties[senderColumn].Value.LPSZ));
        QMessageAddress from(sender, QMessageAddress::Email);
        QString subject(stringFromLpctstr(properties[subjectColumn].Value.LPSZ));
        QMessage::StatusFlags status;
        if (properties[flagsColumn].Value.ul & MSGFLAG_READ)
            status |= QMessage::Read;
        result = QMessagePrivate::from(newId, status, from, subject);
    } else {
        d_ptr->p_ptr->lastError = InvalidId;
    }
    message->Release();
    return result;
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
QMessageFolder QMessageStore::folder(const QMessageFolderId& id) const
{
    QMessageFolder result;
    d_ptr->p_ptr->lastError = QMessageStore::NoError;

    MapiSessionPtr mapiSession(new MapiSession(&d_ptr->p_ptr->lastError, d_ptr->p_ptr->_mapiInitialized));
    if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
        return result;

    // Get the store key, TODO move QMessageIdPrivate definition into qmessage_p.h and use it
    MapiRecordKey folderRecordKey;
    MapiRecordKey storeRecordKey;
    QDataStream idStream(QByteArray::fromBase64(id.toString().toLatin1()));
    idStream >> folderRecordKey;
    idStream >> storeRecordKey;
    QMessageAccountId accountId(storeRecordKey.toBase64());
    MapiStorePtr mapiStore(mapiSession->findStore(&d_ptr->p_ptr->lastError, accountId));
    if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
        return result;

    result = mapiStore->folderFromId(&d_ptr->p_ptr->lastError, id);
    return result;
}
#endif

QMessageAccount QMessageStore::account(const QMessageAccountId& id) const
{
    QMessageAccount result;
    d_ptr->p_ptr->lastError = QMessageStore::NoError;

    MapiSessionPtr mapiSession(new MapiSession(&d_ptr->p_ptr->lastError, d_ptr->p_ptr->_mapiInitialized));
    if (d_ptr->p_ptr->lastError != QMessageStore::NoError)
        return result;

    MapiStorePtr mapiStore(mapiSession->findStore(&d_ptr->p_ptr->lastError, id));
    if (mapiStore->isValid()) {
        QMessageAccountId id(mapiStore->id());
        QString name(mapiStore->name());
        QMessage::TypeFlags types(QMessage::Email);
        if (mapiStore->name() == "SMS") { // On Windows Mobile SMS store is named "SMS"
            types = QMessage::TypeFlags(QMessage::Sms);
        }
        result = QMessageAccountPrivate::from(id, name, types);
    }
    return result;
}

void QMessageStore::startNotifications(const QMessageFilterKey &key)
{
    Q_UNUSED(key)    
}

void QMessageStore::stopNotifications()
{
}
