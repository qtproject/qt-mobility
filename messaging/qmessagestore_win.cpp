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
#include "qmessagestore.h"
#include "qmessagestore_p.h"
#include "qmessage_p.h"
#include <QSharedDataPointer>
#include <QMap>

#include <qdebug.h>

#include <MAPIUtil.h>

typedef QByteArray MapiRecordKey;
typedef QByteArray MapiEntryId;

// TODO Retrieve record key, message count, and hasSubfolders flag for message folders.
// TODO Open message given it's entry id, needs minimal QMessage implementation first (constructor, destructor, copy constructor, assigment)
// TODO Find a message's entryid given its record key, store record key, and parent record key, with or without using cached folder map.
// TODO determine if it is neccessary to use MAPI_MODIFY when opening folder in order to modify a message in a folder?
// TODO ref counted mapi store with auto logging in/out
// TODO MailFolderRef class for ref counting com objects, 
//   giving MapiFolders a parent store object, store objects a parent session,
//   and automatically releasing all store and folder objects when logging out of the session.

class MapiFolderData : public QSharedData
{
public:
    bool _isValid;
    LPMAPIFOLDER _folder;
    QString _name;
    LPMAPITABLE _subFolders;
    uint _itemCount;
    MapiEntryId _entryId;
    MapiRecordKey _recordKey;
};

class MapiFolder {
public:
    MapiFolder();
    MapiFolder(LPMAPIFOLDER folder, const QString &name = QString());
    void findSubFolders();
    MapiFolder nextSubFolder();
    QMessageIdList queryMessages(const QMessageFilterKey &key = QMessageFilterKey(), const QMessageSortKey &sortKey = QMessageSortKey(), uint limit = 0, uint offset = 0) const;
    void release();
    bool isValid() const { return d->_isValid; }
    LPMAPIFOLDER folder() { return d->_folder; }
    QString name() const { return d->_name; }
    LPMAPITABLE subFolders() { return d->_subFolders; }

    static MapiFolder rootFolder(LPMDB mapiStore);

private:
    QSharedDataPointer<MapiFolderData> d;

    enum columnOrder { entryIdColumn = 0, nameColumn };
};

class QMessageStorePrivatePlatform
{
public:
    QMessageStorePrivatePlatform(QMessageStorePrivate *d, QMessageStore *q);
    ~QMessageStorePrivatePlatform();

    bool login();
    void logout();

    QMessageStorePrivate *d_ptr;
    QMessageStore *q_ptr;
    QMessageStore::ErrorCode lastError;

    bool mapiInitialized;
    IMAPISession* mapiSession;
    LPMDB mapiStore;

    bool openDefaultMapiStore();

    QMap<MapiEntryId, MapiFolder > mapiFolderMap;
};

static QString stringFromLpctstr(LPCTSTR lpszValue)
{
    if (::IsBadStringPtr(lpszValue, (UINT_PTR)-1))
        return QString::null;
    if (lpszValue)
        return QString::fromWCharArray(lpszValue);
    return QString::null;
}

MapiFolder::MapiFolder() 
{
    d = new MapiFolderData();
    d->_isValid = false;
}

MapiFolder MapiFolder::rootFolder(LPMDB mapiStore)
{
    if (!mapiStore)
        return MapiFolder();

    SPropValue *rgProps(0);
    ULONG rgTags[] = {1, PR_IPM_SUBTREE_ENTRYID};
    ULONG cCount;
   
    if (mapiStore->GetProps(reinterpret_cast<LPSPropTagArray>(rgTags), MAPI_UNICODE, &cCount, &rgProps) != S_OK)
        return MapiFolder();

    // TODO:See MS KB article 312013, OpenEntry is not re-entrant, also true of MAPI functions in general?
    // TODO:See ms859378, GetPropsExample for alternative memory allocation technique
    LPMAPIFOLDER mapiFolder;
    ULONG ulObjectType;
    LPUNKNOWN FAR *mapiFolderPtr(reinterpret_cast<LPUNKNOWN FAR *>(&mapiFolder));
    ULONG cbEntryId(rgProps[0].Value.bin.cb);
    LPENTRYID lpEntryId(reinterpret_cast<LPENTRYID>(rgProps[0].Value.bin.lpb));

//XXX NO_CACHE    if (mapiStore->OpenEntry(cbEntryId, lpEntryId, 0, (ULONG)0x00000200, &ulObjectType, mapiFolderPtr) != S_OK)
    if (mapiStore->OpenEntry(cbEntryId, lpEntryId, 0, 0, &ulObjectType, mapiFolderPtr) != S_OK)
        mapiFolder = 0;

    MAPIFreeBuffer(rgProps);
    if (mapiFolder)
        return MapiFolder(mapiFolder);
    return MapiFolder();
}

void MapiFolder::findSubFolders()
{
    LPMAPITABLE subFolders(0);

    d->_isValid = false;
    if (!d->_folder || (d->_folder->GetHierarchyTable(0, &subFolders) != S_OK))
        return;

    // Order of properties must be consistent with columnOrder enum.
    const int nCols(5);
    SizedSPropTagArray(nCols, columns) = {nCols, {PR_ENTRYID, PR_DISPLAY_NAME, PR_RECORD_KEY, PR_CONTENT_COUNT, PR_SUBFOLDERS}};
    if (subFolders->SetColumns(reinterpret_cast<LPSPropTagArray>(&columns), 0) == S_OK) {
        d->_subFolders = subFolders;
        d->_isValid = true;
    }
}

MapiFolder::MapiFolder(LPMAPIFOLDER folder, const QString &name)
{
    d = new MapiFolderData();
    d->_isValid = false;
    d->_folder = folder;
    d->_name = name;
    findSubFolders();
}

MapiFolder MapiFolder::nextSubFolder()
{
    if (!d->_isValid || !d->_folder || !d->_subFolders)
        return MapiFolder();

    DWORD objectType;
    LPSRowSet rows(0);
    LPMAPIFOLDER subFolder(0);
    QString name;

    if (d->_subFolders->QueryRows(1, 0, &rows) == S_OK) {
        if (rows->cRows == 1) {
            LPSPropValue entryId(&rows->aRow[0].lpProps[entryIdColumn]);
            ULONG cbEntryId(entryId->Value.bin.cb);
            LPENTRYID lpEntryId(reinterpret_cast<LPENTRYID>(entryId->Value.bin.lpb));
            if (d->_folder->OpenEntry(cbEntryId, lpEntryId, 0, 0, &objectType, reinterpret_cast<LPUNKNOWN*>(&subFolder)) == S_OK) {
                name = stringFromLpctstr(rows->aRow[0].lpProps[nameColumn].Value.LPSZ);
                // TODO: Make a copy of entry id, record key, message count, and hasSubFolers property values.
            }
        }
        FreeProws(rows);
    }
    MAPIFreeBuffer(rows);
    return MapiFolder(subFolder, name);
}

QMessageIdList MapiFolder::queryMessages(const QMessageFilterKey &key, const QMessageSortKey &sortKey, uint limit, uint offset) const
{
    Q_UNUSED(key)
    Q_UNUSED(sortKey)
    Q_UNUSED(offset)

    if (!d->_isValid)
        return QMessageIdList();

    QMessageIdList result;
    LPMAPITABLE messagesTable;
    if (d->_folder->GetContentsTable(MAPI_UNICODE, &messagesTable) != S_OK)
        return QMessageIdList(); // TODO retry? assert?

    // TODO remove flags, sender, subject
    const int nCols(5);
    enum { entryIdColumn = 0, recordKeyColumn, flagsColumn, senderColumn, subjectColumn };
    SizedSPropTagArray(nCols, columns) = {nCols, {PR_ENTRYID, PR_RECORD_KEY, PR_MESSAGE_FLAGS, PR_SENDER_NAME, PR_SUBJECT}};
    if (messagesTable->SetColumns(reinterpret_cast<LPSPropTagArray>(&columns), 0) != S_OK)
        return QMessageIdList();

    LPSRowSet rows(0);
    uint workingLimit(limit);

    while (messagesTable->QueryRows(1, 0, &rows) == S_OK) {
        if (rows->cRows == 1) {
            if (limit)
                --workingLimit;
            LPSPropValue entryIdProp(&rows->aRow[0].lpProps[entryIdColumn]);
            ULONG cbEntryId(entryIdProp->Value.bin.cb);
            LPENTRYID lpEntryId(reinterpret_cast<LPENTRYID>(entryIdProp->Value.bin.lpb));
            bool read(rows->aRow[0].lpProps[flagsColumn].Value.ul & MSGFLAG_READ);
            QString sender = stringFromLpctstr(rows->aRow[0].lpProps[senderColumn].Value.LPSZ);
            QString subject = stringFromLpctstr(rows->aRow[0].lpProps[subjectColumn].Value.LPSZ);
            qDebug() << ((!read) ? '*' : ' ') << sender.leftJustified(25, ' ', true) << subject.leftJustified(46, ' ' , true); //XXX
            LPSPropValue recordKeyProp(&rows->aRow[0].lpProps[recordKeyColumn]);
            QByteArray recordKey(reinterpret_cast<const char*>(recordKeyProp->Value.bin.lpb), recordKeyProp->Value.bin.cb);
            QByteArray entryId(reinterpret_cast<const char*>(entryIdProp->Value.bin.lpb), entryIdProp->Value.bin.cb);
            QByteArray encodedId;
            QDataStream encodedIdStream(&encodedId, QIODevice::WriteOnly);
            encodedIdStream << recordKey;
            encodedIdStream << entryId;
            QMessageId messageId(encodedId.toBase64()); // TODO This is too inefficient maybe QMessageStore should be a friend of the id classes

            /**** XXX Test copy begin ****/
            QByteArray recordKeyTest;
            QByteArray entryIdTest;
            QDataStream testStream(encodedId);
            testStream >> recordKeyTest;
            testStream >> entryIdTest;
            qDebug() << "encodedIdStream" << encodedId.toBase64();
            qDebug() << "recordKeyA" << recordKey.toBase64();
            qDebug() << "recordKeyB" << recordKeyTest.toBase64();
            qDebug() << "entryIdA" << entryId.toBase64();
            qDebug() << "entryIdB" << entryIdTest.toBase64();
            Q_ASSERT( recordKey == recordKeyTest);
            Q_ASSERT( entryId == entryIdTest);
            /**** XXX Test copy end ****/

            result.append(QMessageId(encodedId.toBase64()));
        }
        FreeProws(rows);
        if (limit && !workingLimit)
            break;
        if (!rows->cRows)
            break;
    }
    MAPIFreeBuffer(rows);

    return result;
//    return QMessageIdList(); // gets limit messages in each folder TODO remove this line
}

void MapiFolder::release()
{
    if (!d->_isValid)
        return;
    if (d->_subFolders)
        d->_subFolders->Release();
    d->_subFolders = 0;
    if (d->_folder)
        d->_folder->Release();
    d->_folder = 0;
}

QMessageStorePrivatePlatform::QMessageStorePrivatePlatform(QMessageStorePrivate *d, QMessageStore *q)
    :d_ptr(d), 
     q_ptr(q)
{
    mapiSession = 0;
    mapiStore = 0;
    mapiInitialized = false;
    lastError = QMessageStore::NoError;
#ifndef QT_NO_THREAD
    // Note MAPIINIT is ignored on Windows Mobile but used on Outlook 2007 see
    // msdn ms862621 vs cc842343
    MAPIINIT_0 MAPIINIT = { 0, MAPI_MULTITHREAD_NOTIFICATIONS };
    if (MAPIInitialize(&MAPIINIT) == S_OK)
        mapiInitialized = true;
#else
    if (MAPIInitialize(0) == S_OK)
        mapiInitialized = true;
#endif
    if (!mapiInitialized)
        lastError = QMessageStore::ContentInaccessible;
}

QMessageStorePrivatePlatform::~QMessageStorePrivatePlatform()
{
    logout();
    MAPIUninitialize();
}

bool QMessageStorePrivatePlatform::login()
{
    // Attempt to start a MAPI session on the default profile
    if (MAPILogonEx(0, (LPTSTR)0, 0, MAPI_EXTENDED | MAPI_USE_DEFAULT | MAPI_NEW_SESSION, &mapiSession) != S_OK) {
        lastError = QMessageStore::ContentInaccessible;
        return false;
    }
    return true;
}

void QMessageStorePrivatePlatform::logout()
{
    if (mapiStore)
        mapiStore->Release();
    mapiStore = 0;
    if (mapiSession)
        mapiSession->Release();
    mapiSession = 0;
}

QMessageStorePrivate::QMessageStorePrivate()
    :p_ptr(0),
     q_ptr(0)
{
}

bool QMessageStorePrivatePlatform::openDefaultMapiStore()
{
    if (!mapiInitialized || !mapiSession)
        return false;

    IMAPITable *mapiMessageStoresTable(0);
    const int nCols(4);
    enum { defaultStoreColumn = 0, nameColumn, entryIdColumn, recordKeyColumn };
    SizedSPropTagArray(nCols, columns) = {nCols, {PR_DEFAULT_STORE, PR_DISPLAY_NAME, PR_ENTRYID, PR_RECORD_KEY}};
    LPSRowSet rows(0);
    bool result(false);

    if (mapiStore)
        mapiStore->Release();
    mapiStore = 0;
    
    if (mapiSession->GetMsgStoresTable(0, &mapiMessageStoresTable) != S_OK)
        return false;
    if (mapiMessageStoresTable->SetColumns(reinterpret_cast<LPSPropTagArray>(&columns), 0) != S_OK) {
        mapiMessageStoresTable->Release();
        return false;
    }

    // TODO: Consider handling MAPI_E_BUSY by calling IMAPITable::WaitForCompletion
    while (true) {
        if (mapiMessageStoresTable->QueryRows(1, 0, &rows) != S_OK) {
            MAPIFreeBuffer(rows);
            break;
        }
        if (rows->cRows != 1) {
            FreeProws(rows);
            break;
        }
        if (rows->aRow[0].lpProps[defaultStoreColumn].Value.b) {
            // default store found
            ULONG flags(MDB_NO_DIALOG | MAPI_BEST_ACCESS);
            ULONG cbEntryId(rows->aRow[0].lpProps[entryIdColumn].Value.bin.cb);
            LPENTRYID lpEntryId(reinterpret_cast<LPENTRYID>(rows->aRow[0].lpProps[entryIdColumn].Value.bin.lpb));
            /**** XXX Test copy begin ****/
            SPropValue storeRecordKey;
            SPropValue storeEntryKey;
            PropCopyMore(&storeEntryKey, &(rows->aRow[0].lpProps[entryIdColumn]), MAPIAllocateMore, 0);
            PropCopyMore(&storeRecordKey, &(rows->aRow[0].lpProps[recordKeyColumn]), MAPIAllocateMore, 0);
            qDebug() << "rows->aRow[0].lpProps[recordKeyColumn].Value.bin.cb" << rows->aRow[0].lpProps[recordKeyColumn].Value.bin.cb;
            qDebug() << "storeRecordKey.Value.bin.cb" << storeRecordKey.Value.bin.cb;
            QByteArray storeRecordKeyA((const char*)rows->aRow[0].lpProps[recordKeyColumn].Value.bin.lpb, rows->aRow[0].lpProps[recordKeyColumn].Value.bin.cb);
            QByteArray storeRecordKeyB((const char*)storeRecordKey.Value.bin.lpb, storeRecordKey.Value.bin.cb);
            qDebug() << "pre  copy key" << storeRecordKeyA.toHex();
            qDebug() << "post copy key" << storeRecordKeyB.toHex();
            /**** XXX Test copy end ****/
            result = (mapiSession->OpenMsgStore(0, cbEntryId, lpEntryId, 0, flags, &mapiStore) == S_OK);
            FreeProws(rows);
            break;
        }
    }
    mapiMessageStoresTable->Release();
    return result;
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
    uint workingLimit(offset + limit); // TODO: Improve this it's horribly inefficient
    QMessageIdList result;
    QStringList path;
    QList<MapiFolder> folders; // depth first search;

    if(!d_ptr->p_ptr->mapiInitialized || !d_ptr->p_ptr->login() || !d_ptr->p_ptr->openDefaultMapiStore()) {
        // XXX TODO change to log
		qDebug() << "Failed to initialize MAPI";
        return QMessageIdList();
	}

    folders.append(MapiFolder::rootFolder(d_ptr->p_ptr->mapiStore));
    path.append(folders.back().name());
    while (!folders.isEmpty()) {
        if (!folders.back().isValid()) {
            folders.pop_back();
            path.pop_back();
            continue;
        }

        MapiFolder folder(folders.back().nextSubFolder());
        if (folder.isValid() && (!limit || workingLimit)) {
            uint oldCount(result.count());
            folders.append(folder);
            path.append(folder.name());
            qDebug() << "Path: " << path; // XXX todo remove debug;
            result.append(folder.queryMessages(QMessageFilterKey(), QMessageSortKey(), workingLimit, 0));
            if (limit)
                workingLimit -= (result.count() - oldCount);
        } else {
            folders.back().release();
            folders.pop_back();
            path.pop_back();
        }
    }
	d_ptr->p_ptr->logout();

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
    Q_UNUSED(limit)
    Q_UNUSED(offset)
    return QMessageFolderIdList(); // stub
}
#endif

QMessageAccountIdList QMessageStore::queryAccounts(const QMessageAccountFilterKey &key, const QMessageAccountSortKey &sortKey, uint limit, uint offset) const
{
    Q_UNUSED(key)
    Q_UNUSED(sortKey)
    Q_UNUSED(limit)
    Q_UNUSED(offset)
    return QMessageAccountIdList(); // stub
}

int QMessageStore::countMessages(const QMessageFilterKey& key) const
{
    Q_UNUSED(key)
    return 0; // stub
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
int QMessageStore::countFolders(const QMessageFolderFilterKey& key) const
{
    Q_UNUSED(key)
    return 0; // stub
}
#endif

int QMessageStore::countAccounts(const QMessageAccountFilterKey& key) const
{
    Q_UNUSED(key)
    return 0; // stub
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
    /* Begin debug code */
    QByteArray recordKey;
    QByteArray entryId;
    qDebug() << "id to string" << id.toString();

    QDataStream idStream(QByteArray::fromBase64(id.toString().toLatin1()));
    idStream >> recordKey;
    if (!idStream.atEnd())
        idStream >> entryId;

    qDebug() << "QMessageStore::message recordKey" << recordKey.toBase64();
    qDebug() << "QMessageStore::message entryId" << entryId.toBase64();
    //TODO fall back recordKey if entryId is invalid.
    /* end debug code */

//    result.d_ptr->setId(id); TODO need to implement QMessage copy constructor etc
    return result;
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
QMessageFolder QMessageStore::folder(const QMessageFolderId& id) const
{
    Q_UNUSED(id)
    return QMessageFolder(); // stub
}
#endif

QMessageAccount QMessageStore::account(const QMessageAccountId& id) const
{
    Q_UNUSED(id)
    return QMessageAccount(); // stub
}

void QMessageStore::startNotifications(const QMessageFilterKey &key)
{
    Q_UNUSED(key)    
}

void QMessageStore::stopNotifications()
{
}
