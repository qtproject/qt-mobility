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
#include "winhelpers_p.h"
#include "qmessageid_p.h"
#include "qmessagefolderid_p.h"
#include "qmessageaccountid_p.h"
#include "qmessage_p.h"
#include "qmessagefolder_p.h"
#include "qmessageaccount_p.h"
#include <qdebug.h>

// TODO Retrieve message count, and hasSubfolders flag for message folders.
// TODO determine if it is neccessary to use MAPI_MODIFY when opening folder in order to modify a message in a folder?
// TODO review error handling make sure lastError is being updated appropriately
// TODO Consider wrapping LPMAPITABLE and LPSRowSet
// TODO proper iterators for folders (for sub folders, messages, their entry ids and their record ids)

QString stringFromLpctstr(LPCTSTR lpszValue)
{
    if (::IsBadStringPtr(lpszValue, (UINT_PTR)-1)) // Don't crash when MAPI returns a bad string (and it does).
        return QString::null;
    if (lpszValue)
        return QString::fromUtf16(lpszValue);
    return QString::null;
}

MapiFolder::MapiFolder()
    :_valid(false),
     _folder(0),
     _subFolders(0)
{
}

void MapiFolder::findSubFolders()
{
    LPMAPITABLE subFolders(0);

    if (!_folder || (_folder->GetHierarchyTable(0, &subFolders) != S_OK))
        return;

    // Order of properties must be consistent with columnOrder enum.
    const int nCols(5);
    SizedSPropTagArray(nCols, columns) = {nCols, {PR_ENTRYID, PR_DISPLAY_NAME, PR_RECORD_KEY, PR_CONTENT_COUNT, PR_SUBFOLDERS}};
    if (subFolders->SetColumns(reinterpret_cast<LPSPropTagArray>(&columns), 0) == S_OK)
        _subFolders = subFolders;
}

MapiFolder::MapiFolder(LPMAPIFOLDER folder, MapiRecordKey key, MapiRecordKey parentStoreKey, const QString &name)
    :_valid(true),
     _folder(folder),
     _key(key),
     _parentStoreKey(parentStoreKey),
     _name(name),
     _subFolders(0)
{
    if (!folder)
        _valid = false;
    findSubFolders();
}

MapiFolder::~MapiFolder()
{
    if (_valid)
        qDebug() << "Releasing" << _name; // TODO remove
    if (_subFolders)
        _subFolders->Release();
    _subFolders = 0;
    if (_folder)
        _folder->Release();
    _folder = 0;
    _valid = false;
}

MapiFolderPtr MapiFolder::nextSubFolder()
{
    if (!_valid || !_folder || !_subFolders)
        return MapiFolder::null();

    DWORD objectType;
    LPSRowSet rows(0);
    LPMAPIFOLDER subFolder(0);
    MapiRecordKey folderKey;
    QString name;

    if (_subFolders->QueryRows(1, 0, &rows) == S_OK) {
        if (rows->cRows == 1) {
            LPSPropValue entryId(&rows->aRow[0].lpProps[entryIdColumn]);
            ULONG cbEntryId(entryId->Value.bin.cb);
            LPENTRYID lpEntryId(reinterpret_cast<LPENTRYID>(entryId->Value.bin.lpb));
            LPSPropValue recordKeyProp(&rows->aRow[0].lpProps[recordKeyColumn]);
            folderKey = MapiRecordKey(reinterpret_cast<const char*>(recordKeyProp->Value.bin.lpb), recordKeyProp->Value.bin.cb);
            if (_folder->OpenEntry(cbEntryId, lpEntryId, 0, 0, &objectType, reinterpret_cast<LPUNKNOWN*>(&subFolder)) == S_OK) {
                name = stringFromLpctstr(rows->aRow[0].lpProps[nameColumn].Value.LPSZ);
                // TODO: Make a copy of message count, and hasSubFolders property values.
            } else {
                subFolder = 0;
            }
        }
        FreeProws(rows);
    }
    MAPIFreeBuffer(rows);
    if (!subFolder)
        return MapiFolder::null();
    return MapiFolderPtr(new MapiFolder(subFolder, folderKey, _parentStoreKey, name));
}

QMessageIdList MapiFolder::queryMessages(const QMessageFilterKey &key, const QMessageSortKey &sortKey, uint limit, uint offset) const
{
    Q_UNUSED(key)
    Q_UNUSED(sortKey)
    Q_UNUSED(offset)

    if (!_valid || !_folder)
        return QMessageIdList();

    QMessageIdList result;
    LPMAPITABLE messagesTable;
    if (_folder->GetContentsTable(MAPI_UNICODE, &messagesTable) != S_OK)
        return QMessageIdList(); // TODO set last error to content inaccessible, and review all != S_OK result handling

    // TODO remove flags, sender, subject, they are just used for debugging
    const int nCols(5);
    enum { entryIdColumn = 0, recordKeyColumn, flagsColumn, senderColumn, subjectColumn };
    SizedSPropTagArray(nCols, columns) = {nCols, {PR_ENTRYID, PR_RECORD_KEY, PR_MESSAGE_FLAGS, PR_SENDER_NAME, PR_SUBJECT}};
    if (messagesTable->SetColumns(reinterpret_cast<LPSPropTagArray>(&columns), 0) != S_OK)
        return QMessageIdList();

    LPSRowSet rows(0);
    uint workingLimit(limit);

    while (messagesTable->QueryRows(1, 0, &rows) == S_OK) {
        ULONG cRows(rows->cRows);
        if (cRows == 1) {
            if (limit)
                --workingLimit;
            LPSPropValue entryIdProp(&rows->aRow[0].lpProps[entryIdColumn]);
            /* Begin test code TODO remove */
            bool read(rows->aRow[0].lpProps[flagsColumn].Value.ul & MSGFLAG_READ);
            QString sender = stringFromLpctstr(rows->aRow[0].lpProps[senderColumn].Value.LPSZ);
            QString subject = stringFromLpctstr(rows->aRow[0].lpProps[subjectColumn].Value.LPSZ);
            qDebug() << ((!read) ? '*' : ' ') << sender.leftJustified(25, ' ', true) << subject.leftJustified(46, ' ' , true);
            /* End test code */

            LPSPropValue recordKeyProp(&rows->aRow[0].lpProps[recordKeyColumn]);
            MapiRecordKey recordKey(reinterpret_cast<const char*>(recordKeyProp->Value.bin.lpb), recordKeyProp->Value.bin.cb);
            MapiEntryId entryId(reinterpret_cast<const char*>(entryIdProp->Value.bin.lpb), entryIdProp->Value.bin.cb);
            result.append(QMessageIdPrivate::from(recordKey, _key, _parentStoreKey, entryId));
        }
        FreeProws(rows);
        if (limit && !workingLimit)
            break;
        if (cRows != 1)
            break;
    }
    MAPIFreeBuffer(rows);

#if 0 // gets limit messages in each folder, useful for testing, TODO remove this line
    return QMessageIdList(); }
#else
    return result;
#endif
}

MapiEntryId MapiFolder::messageEntryId(const MapiRecordKey &messageKey)
{
    MapiEntryId result;
    MapiRecordKey key(messageKey);

    LPMAPITABLE messagesTable;
    if (_folder->GetContentsTable(MAPI_UNICODE, &messagesTable) != S_OK)
        return result; // TODO set last error to content inaccessible, and review all != S_OK result handling

    SRestriction restriction;
    SPropValue keyProp;
    restriction.rt = RES_PROPERTY;
    restriction.res.resProperty.relop = RELOP_EQ;
    restriction.res.resProperty.ulPropTag = PR_RECORD_KEY;
    restriction.res.resProperty.lpProp = &keyProp;
    
    keyProp.ulPropTag = PR_RECORD_KEY;
    keyProp.Value.bin.cb = key.count();
    keyProp.Value.bin.lpb = reinterpret_cast<LPBYTE>(key.data());

    ULONG flags(0);
    if (messagesTable->Restrict(&restriction, flags) != S_OK)
        return result; // TODO error handling, framework fault

    const int nCols(2);
    enum { entryIdColumn = 0, recordKeyColumn };
    SizedSPropTagArray(nCols, columns) = {nCols, {PR_ENTRYID, PR_RECORD_KEY}};
    if (messagesTable->SetColumns(reinterpret_cast<LPSPropTagArray>(&columns), 0) != S_OK)
        return result; // TODO error handling, framework fault

    LPSRowSet rows(0);
    if (messagesTable->QueryRows(1, 0, &rows) == S_OK) {
        if (rows->cRows == 1) {
            LPSPropValue entryIdProp(&rows->aRow[0].lpProps[entryIdColumn]);
            MapiEntryId entryId(reinterpret_cast<const char*>(entryIdProp->Value.bin.lpb), entryIdProp->Value.bin.cb);
            result = entryId;
        } else {
            // TODO error handling, invalid id
        }
        FreeProws(rows);
    } else {
        // TODO eror handling, invalid id
    }
    MAPIFreeBuffer(rows);

    return result;
}

QMessageFolderId MapiFolder::id()
{
    return QMessageFolderIdPrivate::from(_key, _parentStoreKey);
}

MapiStore::MapiStore()
    :_valid(false),
     _store(0)
{
}

MapiStore::MapiStore(LPMDB store, MapiRecordKey key, const QString &name)
    :_valid(true),
     _store(store),
     _key(key),
     _name(name)
{
}

MapiStore::~MapiStore()
{
    if (_valid)
        qDebug() << "Releasing MAPI Store"; // TODO remove
    if (_store)
        _store->Release();
    _store = 0;
    _valid = false;
};

bool MapiStore::isValid()
{
    return _valid;
}

MapiFolderPtr MapiStore::rootFolder()
{
    MapiFolderPtr result(MapiFolder::null());

    if (!_valid || !_store)
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

#if 0 // NO_MAPI_CACHING
    if (_store->OpenEntry(cbEntryId, lpEntryId, 0, (ULONG)0x00000200, &ulObjectType, reinterpret_cast<LPUNKNOWN FAR *>(&mapiFolder)) != S_OK)
#else
    if (_store->OpenEntry(cbEntryId, lpEntryId, 0, 0, &ulObjectType, reinterpret_cast<LPUNKNOWN FAR *>(&mapiFolder)) != S_OK)
#endif
        mapiFolder = 0;

    MAPIFreeBuffer(rgProps);
    if (mapiFolder)
        result = MapiFolderPtr(new MapiFolder(mapiFolder, QByteArray(), _key)); /// TODO Try to find a better record key for the root folder
    return result;
}

MapiFolderPtr MapiStore::findFolder(const MapiRecordKey &key)
{
    QList<MapiFolderPtr> folders;
    folders.append(rootFolder());

    while (!folders.isEmpty()) {
        MapiFolderPtr subFolder(folders.back()->nextSubFolder());
        if (subFolder->isValid()) {
            if (subFolder->recordKey() == key) {
                qDebug() << "found folder" << subFolder->name(); // TODO remove
                return subFolder;
            }
            folders.append(subFolder);
            /* Begin debug TODO remove */
            QStringList path;
            for (int i = 0; i < folders.count(); path.append(folders[i]->name()), ++i);
            qDebug() << "pushing" << path.join("/");
            /* End debug */
        } else {
            folders.pop_back();
        }
    }

    return MapiFolder::null();
}

QMessageFolderIdList MapiStore::folderIds()
{
    QMessageFolderIdList folderIds;
    QList<MapiFolderPtr> folders;
    folders.append(rootFolder());
    // No valid reason to list the root folder.

    while (!folders.isEmpty()) {
        MapiFolderPtr subFolder(folders.back()->nextSubFolder());
        if (subFolder->isValid()) {
            folderIds.append(subFolder->id());
            folders.append(subFolder);
            /* Begin debug TODO remove */
            QStringList path;
            for (int i = 0; i < folders.count(); path.append(folders[i]->name()), ++i);
            qDebug() << "pushing" << path.join("/");
            /* End debug */
        } else {
            folders.pop_back();
        }
    }

    return folderIds;
}

QMessageFolder MapiStore::folderFromId(const QMessageFolderId &folderId)
{
    QMessageFolder result;
    QList<MapiFolderPtr> folders;
    folders.append(rootFolder());

    while (!folders.isEmpty()) {
        MapiFolderPtr subFolder(folders.back()->nextSubFolder());
        if (subFolder->isValid()) {
            if (folderId == subFolder->id()) {
                QStringList path;
                for (int i = 0; i < folders.count(); ++i)
                    path.append(folders[i]->name());
                result = QMessageFolderPrivate::from(subFolder->id(), id(), folders.last()->id(), subFolder->name(), path.join("/"));
                qDebug() << "found" << path.join("/") << subFolder->name(); //TODO remove debug
                return result;
            }
            folders.append(subFolder);
        } else {
            folders.pop_back();
        }
    }

    return result;
}

QMessageAccountId MapiStore::id()
{
    return QMessageAccountIdPrivate::from(_key);
}

MapiSession::MapiSession()
    :_valid(false),
     _mapiSession(0)
{
}

MapiSession::MapiSession(bool mapiInitialized)
    :_valid(mapiInitialized),
     _mapiSession(0)
{
    if (!_valid)
        return;

    // Attempt to start a MAPI session on the default profile
    if (MAPILogonEx(0, (LPTSTR)0, 0, MAPI_EXTENDED | MAPI_USE_DEFAULT | MAPI_NEW_SESSION, &_mapiSession) != S_OK) {
        qWarning() << "Failed to login to MAPI session";
        _valid = false;
        _mapiSession = 0;
    }
}

MapiSession::~MapiSession()
{
    if (_valid)
        qDebug() << "Logging out of mapi session"; // TODO remove
    if (_mapiSession) {
        _mapiSession->Logoff(0, 0, 0);
        _mapiSession->Release();
    }
    _mapiSession = 0;
    _valid = false;
};

bool MapiSession::isValid()
{
    return _valid;
}

HRESULT MapiSession::openEntry(MapiEntryId entryId, LPMESSAGE *message)
{
    if (!_valid || !_mapiSession) {
        Q_ASSERT(_valid && _mapiSession);
        return 0;
    }

    ULONG FAR ulObjectType;
    return _mapiSession->OpenEntry(entryId.count(), reinterpret_cast<LPENTRYID>(entryId.data()), 0, MAPI_BEST_ACCESS, &ulObjectType, reinterpret_cast<LPUNKNOWN*>(message));
}

MapiStorePtr MapiSession::findStore(const QMessageAccountId &id)
{
    MapiStorePtr result(MapiStore::null());
    if (!_valid || !_mapiSession)
        return result;

    IMAPITable *mapiMessageStoresTable(0);
    const int nCols(4);
    enum { defaultStoreColumn = 0, nameColumn, entryIdColumn, recordKeyColumn };
    SizedSPropTagArray(nCols, columns) = {nCols, {PR_DEFAULT_STORE, PR_DISPLAY_NAME, PR_ENTRYID, PR_RECORD_KEY}};
    LPSRowSet rows(0);

    if (_mapiSession->GetMsgStoresTable(0, &mapiMessageStoresTable) != S_OK)
        return result;
    if (mapiMessageStoresTable->SetColumns(reinterpret_cast<LPSPropTagArray>(&columns), 0) != S_OK) {
        mapiMessageStoresTable->Release();
        return result;
    }

    // TODO: Consider handling MAPI_E_BUSY by calling IMAPITable::WaitForCompletion
    while (mapiMessageStoresTable->QueryRows(1, 0, &rows) == S_OK) {
        if (rows->cRows != 1) {
            FreeProws(rows);
            break;
        }
        LPSPropValue recordKeyProp(&rows->aRow[0].lpProps[recordKeyColumn]);
        MapiRecordKey storeKey(reinterpret_cast<const char*>(recordKeyProp->Value.bin.lpb), recordKeyProp->Value.bin.cb);
        if ((!id.isValid() && rows->aRow[0].lpProps[defaultStoreColumn].Value.b) ||  // default store found
            (id.isValid() && (id == QMessageAccountIdPrivate::from(storeKey)))) {    // specified store found
            LPMDB mapiStore;
            ULONG flags(MDB_NO_DIALOG | MAPI_BEST_ACCESS);
            ULONG cbEntryId(rows->aRow[0].lpProps[entryIdColumn].Value.bin.cb);
            LPENTRYID lpEntryId(reinterpret_cast<LPENTRYID>(rows->aRow[0].lpProps[entryIdColumn].Value.bin.lpb));
            QString name(stringFromLpctstr(rows->aRow[0].lpProps[nameColumn].Value.LPSZ));
            /**** Test copy begin, TODO remove ****/
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
            /**** Test copy end ****/
            if (_mapiSession->OpenMsgStore(0, cbEntryId, lpEntryId, 0, flags, &mapiStore) == S_OK)
                result = MapiStorePtr(new MapiStore(mapiStore, storeKey, name));
            FreeProws(rows);
            break;
        }
        FreeProws(rows);
    }
    MAPIFreeBuffer(rows);
    mapiMessageStoresTable->Release();
    return result;
}
