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
#include <QSharedPointer>
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
// TODO typedef QSharedPointer<MapiFolder> MailFolderPtr class for ref counting com objects, 
//   giving MapiFolders a parent store object, store objects a parent session,
//   and automatically releasing all store and folder objects when logging out of the session.

class MapiFolder;
class MapiStore;
typedef QSharedPointer<MapiFolder> MapiFolderPtr;
typedef QSharedPointer<MapiStore> MapiStorePtr;

static QString stringFromLpctstr(LPCTSTR lpszValue)
{
    if (::IsBadStringPtr(lpszValue, (UINT_PTR)-1))
        return QString::null;
    if (lpszValue)
        return QString::fromWCharArray(lpszValue);
    return QString::null;
}

class MapiFolder {
public:
    MapiFolder();
    MapiFolder(LPMAPIFOLDER folder, const QString &name = QString());
    ~MapiFolder() { release(); };
    void findSubFolders();
    MapiFolderPtr nextSubFolder();
    QMessageIdList queryMessages(const QMessageFilterKey &key = QMessageFilterKey(), const QMessageSortKey &sortKey = QMessageSortKey(), uint limit = 0, uint offset = 0) const;
    bool isValid() const { return _isValid; }
    LPMAPIFOLDER folder() { return _folder; }
    QString name() const { return _name; }
    LPMAPITABLE subFolders() { return _subFolders; }

    static MapiFolderPtr null() { return MapiFolderPtr(new MapiFolder()); }

private:
    void release();

    bool _isValid;
    LPMAPIFOLDER _folder;
    QString _name;
    LPMAPITABLE _subFolders;
    uint _itemCount;
    MapiEntryId _entryId;
    MapiRecordKey _recordKey;

    enum columnOrder { entryIdColumn = 0, nameColumn };
};

MapiFolder::MapiFolder() 
{
    _isValid = false;
}

void MapiFolder::findSubFolders()
{
    LPMAPITABLE subFolders(0);

    _isValid = false;
    if (!_folder || (_folder->GetHierarchyTable(0, &subFolders) != S_OK))
        return;

    // Order of properties must be consistent with columnOrder enum.
    const int nCols(5);
    SizedSPropTagArray(nCols, columns) = {nCols, {PR_ENTRYID, PR_DISPLAY_NAME, PR_RECORD_KEY, PR_CONTENT_COUNT, PR_SUBFOLDERS}};
    if (subFolders->SetColumns(reinterpret_cast<LPSPropTagArray>(&columns), 0) == S_OK) {
        _subFolders = subFolders;
        _isValid = true;
    }
}

MapiFolder::MapiFolder(LPMAPIFOLDER folder, const QString &name)
{
    _isValid = false;
    _folder = folder;
    _name = name;
    findSubFolders();
}

MapiFolderPtr MapiFolder::nextSubFolder()
{
    if (!_isValid || !_folder || !_subFolders)
        return MapiFolder::null();

    DWORD objectType;
    LPSRowSet rows(0);
    LPMAPIFOLDER subFolder(0);
    QString name;

    if (_subFolders->QueryRows(1, 0, &rows) == S_OK) {
        if (rows->cRows == 1) {
            LPSPropValue entryId(&rows->aRow[0].lpProps[entryIdColumn]);
            ULONG cbEntryId(entryId->Value.bin.cb);
            LPENTRYID lpEntryId(reinterpret_cast<LPENTRYID>(entryId->Value.bin.lpb));
            if (_folder->OpenEntry(cbEntryId, lpEntryId, 0, 0, &objectType, reinterpret_cast<LPUNKNOWN*>(&subFolder)) == S_OK) {
                name = stringFromLpctstr(rows->aRow[0].lpProps[nameColumn].Value.LPSZ);
                // TODO: Make a copy of entry id, record key, message count, and hasSubFolers property values.
            }
        }
        FreeProws(rows);
    }
    MAPIFreeBuffer(rows);
    return MapiFolderPtr(new MapiFolder(subFolder, name));
}

QMessageIdList MapiFolder::queryMessages(const QMessageFilterKey &key, const QMessageSortKey &sortKey, uint limit, uint offset) const
{
    Q_UNUSED(key)
    Q_UNUSED(sortKey)
    Q_UNUSED(offset)

    if (!_isValid)
        return QMessageIdList();

    QMessageIdList result;
    LPMAPITABLE messagesTable;
    if (_folder->GetContentsTable(MAPI_UNICODE, &messagesTable) != S_OK)
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
/*
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
*/
            /**** XXX Test copy end ****/
            result.append(QMessageId(encodedId.toBase64()));
        }
        FreeProws(rows);
        if (limit && !workingLimit)
            break;
        if (rows->cRows != 1)
            break;
    }
    MAPIFreeBuffer(rows);

    return result;
//    return QMessageIdList(); // gets limit messages in each folder TODO remove this line
}

void MapiFolder::release()
{
    if (!_isValid)
        return;
    qDebug() << "Releasing" << _name;
    if (_subFolders)
        _subFolders->Release();
    _subFolders = 0;
    if (_folder)
        _folder->Release();
    _folder = 0;
}

class MapiStore {
public:
    MapiStore();
    MapiStore(LPMDB store);
    ~MapiStore();

    bool isValid();
    MapiFolderPtr rootFolder();

    static MapiStorePtr defaultStore(IMAPISession *mapiSession);
    static MapiStorePtr null() { return MapiStorePtr(new MapiStore()); }

private:
    void release();

    bool _isValid;
    LPMDB _store;
};

MapiStore::MapiStore()
    : _isValid(false)
{
}

MapiStore::MapiStore(LPMDB store)
    :_isValid(true),
     _store(store)
{
}

bool MapiStore::isValid()
{
    return _isValid;
}

MapiFolderPtr MapiStore::rootFolder()
{
    MapiFolderPtr result(MapiFolder::null());
    if (!_isValid)
        return result;

    SPropValue *rgProps(0);
    ULONG rgTags[] = {1, PR_IPM_SUBTREE_ENTRYID};
    ULONG cCount;

    if (_store->GetProps(reinterpret_cast<LPSPropTagArray>(rgTags), MAPI_UNICODE, &cCount, &rgProps) != S_OK)
        return result;

    // TODO:See MS KB article 312013, OpenEntry is not re-entrant, also true of MAPI functions in general?
    // TODO:See ms859378, GetPropsExample for alternative memory allocation technique
    LPMAPIFOLDER mapiFolder;
    ULONG ulObjectType;
    ULONG cbEntryId(rgProps[0].Value.bin.cb);
    LPENTRYID lpEntryId(reinterpret_cast<LPENTRYID>(rgProps[0].Value.bin.lpb));

//XXX NO_CACHE    if (mapiStore->OpenEntry(cbEntryId, lpEntryId, 0, (ULONG)0x00000200, &ulObjectType, mapiFolderPtr) != S_OK)
    if (_store->OpenEntry(cbEntryId, lpEntryId, 0, 0, &ulObjectType, reinterpret_cast<LPUNKNOWN FAR *>(&mapiFolder)) != S_OK)
        mapiFolder = 0;

    MAPIFreeBuffer(rgProps);
    if (mapiFolder)
        result = MapiFolderPtr(new MapiFolder(mapiFolder));
    return result;
}

MapiStorePtr MapiStore::defaultStore(IMAPISession *mapiSession)
{
    IMAPITable *mapiMessageStoresTable(0);
    const int nCols(4);
    enum { defaultStoreColumn = 0, nameColumn, entryIdColumn, recordKeyColumn };
    SizedSPropTagArray(nCols, columns) = {nCols, {PR_DEFAULT_STORE, PR_DISPLAY_NAME, PR_ENTRYID, PR_RECORD_KEY}};
    LPSRowSet rows(0);
    MapiStorePtr result(MapiStore::null());

    if (mapiSession->GetMsgStoresTable(0, &mapiMessageStoresTable) != S_OK)
        return result;
    if (mapiMessageStoresTable->SetColumns(reinterpret_cast<LPSPropTagArray>(&columns), 0) != S_OK) {
        mapiMessageStoresTable->Release();
        return result;
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
            LPMDB mapiStore;
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
            if (mapiSession->OpenMsgStore(0, cbEntryId, lpEntryId, 0, flags, &mapiStore) == S_OK)
                result = MapiStorePtr(new MapiStore(mapiStore));
            FreeProws(rows);
            break;
        }
    }
    mapiMessageStoresTable->Release();
    return result;
}

void MapiStore::release()
{
    qDebug() << "Releasing MAPI Store";
    if (_isValid)
        _store->Release();
    _isValid = false;
}

MapiStore::~MapiStore()
{
    release();
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

    bool _mapiInitialized;
    IMAPISession* _mapiSession;
};

QMessageStorePrivatePlatform::QMessageStorePrivatePlatform(QMessageStorePrivate *d, QMessageStore *q)
    :d_ptr(d), 
     q_ptr(q)
{
    _mapiSession = 0;
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
    if (!_mapiInitialized)
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
    if (MAPILogonEx(0, (LPTSTR)0, 0, MAPI_EXTENDED | MAPI_USE_DEFAULT | MAPI_NEW_SESSION, &_mapiSession) != S_OK) {
        lastError = QMessageStore::ContentInaccessible;
        return false;
    }
    return true;
}

void QMessageStorePrivatePlatform::logout()
{
    if (_mapiSession)
        _mapiSession->Release();
    _mapiSession = 0;
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
    uint workingLimit(offset + limit); // TODO: Improve this it's horribly inefficient
    QMessageIdList result;
    QStringList path;
    QList<MapiFolderPtr> folders; // depth first search;

    if(!d_ptr->p_ptr->_mapiInitialized || !d_ptr->p_ptr->login()) {
		qWarning() << "Failed to initialize MAPI";
        return QMessageIdList();
	}

    MapiStorePtr mapiStore(MapiStore::defaultStore(d_ptr->p_ptr->_mapiSession));
    if (!mapiStore->isValid()) {
		qWarning() << "Failed to open default MAPI store";
        return QMessageIdList();
    }

    folders.append(mapiStore->rootFolder());
    path.append(folders.back()->name());
    while (!folders.isEmpty()) {
        if (!folders.back()->isValid()) {
            folders.pop_back();
            path.pop_back();
            continue;
        }

        MapiFolderPtr folder(folders.back()->nextSubFolder());
        if (folder->isValid() && (!limit || workingLimit)) {
            uint oldCount(result.count());
            folders.append(folder);
            path.append(folder->name());
            qDebug() << "Path: " << path; // XXX todo remove debug;
            result.append(folder->queryMessages(QMessageFilterKey(), QMessageSortKey(), workingLimit, 0));
            if (limit)
                workingLimit -= (result.count() - oldCount);
        } else {
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
