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
#include "qmessagesortkey_p.h"
#include <qdebug.h>

// TODO Retrieve message count, and hasSubfolders flag for message folders.
// TODO determine if it is neccessary to use MAPI_MODIFY when opening folder in order to modify a message in a folder?
// TODO review error handling make sure lastError is being updated appropriately
// TODO Consider wrapping LPMAPITABLE and LPSRowSet
// TODO proper iterators for folders (for sub folders, messages, their entry ids and their record ids)

QString QStringFromLpctstr(LPCTSTR lpszValue)
{
    if (::IsBadStringPtr(lpszValue, (UINT_PTR)-1)) // Don't crash when MAPI returns a bad string (and it does).
        return QString::null;
    if (lpszValue)
        return QString::fromUtf16(reinterpret_cast<const ushort*>(lpszValue));
    return QString::null;
}

MapiFolder::MapiFolder()
    :_valid(false),
     _folder(0),
     _subFolders(0),
     _hasSubFolders(false),
     _messageCount(0),
     _init(true)
{
}

void MapiFolder::findSubFolders(QMessageStore::ErrorCode *lastError)
{
    if (_init)
        return;
    _init = true;

    if (!_hasSubFolders)
        return;

    LPMAPITABLE subFolders(0);
    if (!_folder || (_folder->GetHierarchyTable(0, &subFolders) != S_OK)) {
        Q_ASSERT(_folder);
        *lastError = QMessageStore::ContentInaccessible;
        return;
    }

    // Order of properties must be consistent with columnOrder enum.
    const int nCols(5);
    SizedSPropTagArray(nCols, columns) = {nCols, {PR_ENTRYID, PR_DISPLAY_NAME, PR_RECORD_KEY, PR_CONTENT_COUNT, PR_SUBFOLDERS}};
    if (subFolders->SetColumns(reinterpret_cast<LPSPropTagArray>(&columns), 0) == S_OK)
        _subFolders = subFolders;
}

MapiFolder::MapiFolder(LPMAPIFOLDER folder, MapiRecordKey key, MapiRecordKey parentStoreKey, const QString &name, const MapiEntryId &entryId, bool hasSubFolders, uint messageCount)
    :_valid(true),
     _folder(folder),
     _key(key),
     _parentStoreKey(parentStoreKey),
     _name(name),
     _subFolders(0),
     _entryId(entryId),
     _hasSubFolders(hasSubFolders),
     _messageCount(messageCount),
     _init(false)
{
    if (!folder)
        _valid = false;
    qDebug() << "MapiFolder::MapiFolder, name" << name << "hasSubfolders" << _hasSubFolders << "messageCount" << messageCount; //TODO remove debug
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

MapiFolderPtr MapiFolder::nextSubFolder(QMessageStore::ErrorCode *lastError)
{
    if (!_init)
        findSubFolders(lastError);

    if (!_hasSubFolders)
        return MapiFolder::null();

    if (!_valid || !_folder || !_subFolders) {
        Q_ASSERT(_valid && _folder && _subFolders);
        *lastError = QMessageStore::FrameworkFault;
        return MapiFolder::null();
    }

    DWORD objectType;
    LPSRowSet rows(0);
    LPMAPIFOLDER subFolder(0);
    MapiRecordKey folderKey;
    QString name;
    MapiEntryId entryId;
    bool subHasSubFolders;
    uint subMessageCount;

    if (_subFolders->QueryRows(1, 0, &rows) == S_OK) {
        if (rows->cRows == 1) {
            LPSPropValue entryIdProp(&rows->aRow[0].lpProps[entryIdColumn]);
            ULONG cbEntryId(entryIdProp->Value.bin.cb);
            LPENTRYID lpEntryId(reinterpret_cast<LPENTRYID>(entryIdProp->Value.bin.lpb));
            LPSPropValue recordKeyProp(&rows->aRow[0].lpProps[recordKeyColumn]);
            folderKey = MapiRecordKey(reinterpret_cast<const char*>(recordKeyProp->Value.bin.lpb), recordKeyProp->Value.bin.cb);
            entryId = MapiEntryId(reinterpret_cast<const char*>(entryIdProp->Value.bin.lpb), entryIdProp->Value.bin.cb);
            if (_folder->OpenEntry(cbEntryId, lpEntryId, 0, 0, &objectType, reinterpret_cast<LPUNKNOWN*>(&subFolder)) == S_OK) {
                name = QStringFromLpctstr(rows->aRow[0].lpProps[nameColumn].Value.LPSZ);
                subHasSubFolders = rows->aRow[0].lpProps[subFoldersColumn].Value.b;
                subMessageCount = rows->aRow[0].lpProps[countColumn].Value.ul;
            } else {
                // When offline opening a subfolder may simply fail, so we ignore any error.
                // TODO: Discriminate between differnt types of errors, e.g. critical and non-critical
                subFolder = 0;
            }
        }
        FreeProws(rows);
    } else {
        *lastError = QMessageStore::ContentInaccessible;
    }
    MAPIFreeBuffer(rows);
    if (!subFolder)
        return MapiFolder::null();
    return MapiFolderPtr(new MapiFolder(subFolder, folderKey, _parentStoreKey, name, entryId, subHasSubFolders, subMessageCount));
}

QMessageIdList MapiFolder::queryMessages(QMessageStore::ErrorCode *lastError, const QMessageFilterKey &key, const QMessageSortKey &sortKey, uint limit, uint offset) const
{
    Q_UNUSED(key)
    Q_UNUSED(sortKey)
    Q_UNUSED(offset)

    if (!_valid || !_folder) {
        Q_ASSERT(_valid && _folder);
        *lastError = QMessageStore::FrameworkFault;
        return QMessageIdList();
    }

    QMessageIdList result;
    LPMAPITABLE messagesTable;
    if (_folder->GetContentsTable(MAPI_UNICODE, &messagesTable) != S_OK) {
        *lastError = QMessageStore::ContentInaccessible;
        return QMessageIdList(); // TODO set last error to content inaccessible, and review all != S_OK result handling
    }

    // TODO remove flags, sender, subject, they are just used for debugging
    const int nCols(5);
    enum { entryIdColumn = 0, recordKeyColumn, flagsColumn, senderColumn, subjectColumn };
    SizedSPropTagArray(nCols, columns) = {nCols, {PR_ENTRYID, PR_RECORD_KEY, PR_MESSAGE_FLAGS, PR_SENDER_NAME, PR_SUBJECT}};
    if (messagesTable->SetColumns(reinterpret_cast<LPSPropTagArray>(&columns), 0) != S_OK) {
        *lastError = QMessageStore::ContentInaccessible;
        return QMessageIdList();
    }

    QMessageSortKeyPrivate::sortTable(lastError, sortKey, messagesTable);
    if (*lastError != QMessageStore::NoError)
        return QMessageIdList();

    LPSRowSet rows(0);
    uint workingLimit(limit);
    HRESULT hres;
    LONG ignored;
    if (messagesTable->SeekRow(BOOKMARK_BEGINNING, offset, &ignored) != S_OK) {
        *lastError = QMessageStore::ContentInaccessible;
        return QMessageIdList();
    }

    while ((hres = messagesTable->QueryRows(1, 0, &rows)) == S_OK) {
        ULONG cRows(rows->cRows);
        if (cRows == 1) {
            if (limit)
                --workingLimit;
            LPSPropValue entryIdProp(&rows->aRow[0].lpProps[entryIdColumn]);
            /* Begin test code TODO remove, and remove fetching flags, sender and subject properties */
            bool read(rows->aRow[0].lpProps[flagsColumn].Value.ul & MSGFLAG_READ);
            QString sender = QStringFromLpctstr(rows->aRow[0].lpProps[senderColumn].Value.LPSZ);
            QString subject = QStringFromLpctstr(rows->aRow[0].lpProps[subjectColumn].Value.LPSZ);
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
    if (hres != S_OK) {
        *lastError = QMessageStore::ContentInaccessible;
    }
    MAPIFreeBuffer(rows);

    return result;
}

MapiEntryId MapiFolder::messageEntryId(QMessageStore::ErrorCode *lastError, const MapiRecordKey &messageKey)
{
    MapiEntryId result;
    MapiRecordKey key(messageKey);

    LPMAPITABLE messagesTable;
    if (_folder->GetContentsTable(MAPI_UNICODE, &messagesTable) != S_OK) {
        *lastError = QMessageStore::ContentInaccessible;
        return result; // TODO set last error to content inaccessible, and review all != S_OK result handling
    }

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
    if (messagesTable->Restrict(&restriction, flags) != S_OK) {
        *lastError = QMessageStore::ContentInaccessible;
        return result; // TODO error handling, framework fault
    }

    const int nCols(2);
    enum { entryIdColumn = 0, recordKeyColumn };
    SizedSPropTagArray(nCols, columns) = {nCols, {PR_ENTRYID, PR_RECORD_KEY}};
    if (messagesTable->SetColumns(reinterpret_cast<LPSPropTagArray>(&columns), 0) != S_OK) {
        *lastError = QMessageStore::ContentInaccessible;
        return result; // TODO error handling, framework fault
    }

    LPSRowSet rows(0);
    if (messagesTable->QueryRows(1, 0, &rows) == S_OK) {
        if (rows->cRows == 1) {
            LPSPropValue entryIdProp(&rows->aRow[0].lpProps[entryIdColumn]);
            MapiEntryId entryId(reinterpret_cast<const char*>(entryIdProp->Value.bin.lpb), entryIdProp->Value.bin.cb);
            result = entryId;
        } else {
            *lastError = QMessageStore::InvalidId;
        }
        FreeProws(rows);
    } else {
        *lastError = QMessageStore::InvalidId;
    }
    MAPIFreeBuffer(rows);

    return result;
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
QMessageFolderId MapiFolder::id()
{
    return QMessageFolderIdPrivate::from(_key, _parentStoreKey, _entryId);
}
#endif

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

MapiFolderPtr MapiStore::rootFolder(QMessageStore::ErrorCode *lastError)
{
    MapiFolderPtr result(MapiFolder::null());

    if (!_valid || !_store) {
        Q_ASSERT(_valid && _store);
        *lastError = QMessageStore::FrameworkFault;
        return result;
    }

    SPropValue *rgProps(0);
    ULONG rgTags[] = {1, PR_IPM_SUBTREE_ENTRYID};
    ULONG cCount;

    if (_store->GetProps(reinterpret_cast<LPSPropTagArray>(rgTags), MAPI_UNICODE, &cCount, &rgProps) != S_OK) {
        *lastError = QMessageStore::ContentInaccessible;
        return result;
    }

    // TODO:See MS KB article 312013, OpenEntry is not re-entrant, also true of MAPI functions in general?
    // TODO:See ms859378, GetPropsExample for alternative memory allocation technique
    LPMAPIFOLDER mapiFolder;
    ULONG ulObjectType;
    ULONG cbEntryId(rgProps[0].Value.bin.cb);
    LPENTRYID lpEntryId(reinterpret_cast<LPENTRYID>(rgProps[0].Value.bin.lpb));
    MapiEntryId entryId(reinterpret_cast<const char*>(rgProps[0].Value.bin.lpb), rgProps[0].Value.bin.cb);
#if 0 // NO_MAPI_CACHING
    if (_store->OpenEntry(cbEntryId, lpEntryId, 0, (ULONG)0x00000200, &ulObjectType, reinterpret_cast<LPUNKNOWN FAR *>(&mapiFolder)) != S_OK) {
#else
    if (_store->OpenEntry(cbEntryId, lpEntryId, 0, 0, &ulObjectType, reinterpret_cast<LPUNKNOWN FAR *>(&mapiFolder)) != S_OK) {
#endif
        *lastError = QMessageStore::ContentInaccessible;
        mapiFolder = 0;
    }

    MAPIFreeBuffer(rgProps);
    if (mapiFolder) {
        const int nCols(4);
        enum { displayNameColumn = 0, recordKeyColumn, countColumn, subFoldersColumn };
        SizedSPropTagArray(nCols, columns) = {nCols, {PR_DISPLAY_NAME, PR_RECORD_KEY, PR_CONTENT_COUNT, PR_SUBFOLDERS}};
        SPropValue *properties(0);
        ULONG count;

        if (mapiFolder->GetProps(reinterpret_cast<LPSPropTagArray>(&columns), MAPI_UNICODE, &count, &properties) == S_OK) {
            LPSPropValue recordKeyProp(&properties[recordKeyColumn]);
            MapiRecordKey recordKey(reinterpret_cast<const char*>(recordKeyProp->Value.bin.lpb), recordKeyProp->Value.bin.cb);
            QString name(QStringFromLpctstr(properties[displayNameColumn].Value.LPSZ));
            bool hasSubFolders = properties[subFoldersColumn].Value.b;
            uint messageCount = properties[countColumn].Value.ul;
            result = MapiFolderPtr(new MapiFolder(mapiFolder, recordKey, _key, QString(), entryId, hasSubFolders, messageCount));
        }
    }
    return result;
}

MapiFolderPtr MapiStore::findFolder(QMessageStore::ErrorCode *lastError, const MapiRecordKey &key)
{
    QList<MapiFolderPtr> folders;
    folders.append(rootFolder(lastError));

    while (!folders.isEmpty()) {
        MapiFolderPtr subFolder(folders.back()->nextSubFolder(lastError));
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

#ifdef QMESSAGING_OPTIONAL_FOLDER
QMessageFolderIdList MapiStore::folderIds(QMessageStore::ErrorCode *lastError)
{
    QMessageFolderIdList folderIds;
    QList<MapiFolderPtr> folders;
    folders.append(rootFolder(lastError));
    // No valid reason to list the root folder.

    while (!folders.isEmpty()) {
        MapiFolderPtr subFolder(folders.back()->nextSubFolder(lastError));
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

QMessageFolder MapiStore::folderFromId(QMessageStore::ErrorCode *lastError, const QMessageFolderId &folderId)
{
    QMessageFolder result;
    QList<MapiFolderPtr> folders;
    folders.append(rootFolder(lastError));

    while (!folders.isEmpty()) {
        MapiFolderPtr subFolder(folders.back()->nextSubFolder(lastError));
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
#endif

QMessageAccountId MapiStore::id()
{
    return QMessageAccountIdPrivate::from(_key);
}

MapiSession::MapiSession()
    :_valid(false),
     _mapiSession(0)
{
}

MapiSession::MapiSession(QMessageStore::ErrorCode *lastError, bool mapiInitialized)
    :_valid(mapiInitialized),
     _mapiSession(0)
{
    if (!_valid) {
        *lastError = QMessageStore::ContentInaccessible;
        return;
    }

    // Attempt to start a MAPI session on the default profile
    if (MAPILogonEx(0, (LPTSTR)0, 0, MAPI_EXTENDED | MAPI_USE_DEFAULT | MAPI_NEW_SESSION, &_mapiSession) != S_OK) {
        qWarning() << "Failed to login to MAPI session";
        *lastError = QMessageStore::ContentInaccessible;
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

HRESULT MapiSession::openEntry(QMessageStore::ErrorCode *lastError, MapiEntryId entryId, LPMESSAGE *message) const
{
    return openEntry(lastError, entryId, reinterpret_cast<LPUNKNOWN*>(message));
}

HRESULT MapiSession::openEntry(QMessageStore::ErrorCode *lastError, MapiEntryId entryId, LPMAPIFOLDER *folder) const
{
    return openEntry(lastError, entryId, reinterpret_cast<LPUNKNOWN*>(folder));
}

HRESULT MapiSession::openEntry(QMessageStore::ErrorCode *lastError, MapiEntryId entryId, LPUNKNOWN *unknown) const
{
    if (!_valid || !_mapiSession) {
        Q_ASSERT(_valid && _mapiSession);
        *lastError = QMessageStore::FrameworkFault;
        return 0;
    }

    ULONG FAR ulObjectType;
    return _mapiSession->OpenEntry(entryId.count(), reinterpret_cast<LPENTRYID>(entryId.data()), 0, MAPI_BEST_ACCESS, &ulObjectType, unknown);
}

MapiStorePtr MapiSession::findStore(QMessageStore::ErrorCode *lastError, const QMessageAccountId &id) const
{
    MapiStorePtr result(MapiStore::null());
    if (!_valid || !_mapiSession) {
        Q_ASSERT(_valid && _mapiSession);
        *lastError = QMessageStore::FrameworkFault;
        return result;
    }

    IMAPITable *mapiMessageStoresTable(0);
    const int nCols(4);
    enum { defaultStoreColumn = 0, nameColumn, entryIdColumn, recordKeyColumn };
    SizedSPropTagArray(nCols, columns) = {nCols, {PR_DEFAULT_STORE, PR_DISPLAY_NAME, PR_ENTRYID, PR_RECORD_KEY}};
    LPSRowSet rows(0);

    if (_mapiSession->GetMsgStoresTable(0, &mapiMessageStoresTable) != S_OK) {
        *lastError = QMessageStore::ContentInaccessible;
        return result;
    }
    if (mapiMessageStoresTable->SetColumns(reinterpret_cast<LPSPropTagArray>(&columns), 0) != S_OK) {
        *lastError = QMessageStore::ContentInaccessible;
        mapiMessageStoresTable->Release();
        return result;
    }

    // TODO: Consider handling MAPI_E_BUSY by calling IMAPITable::WaitForCompletion
    HRESULT res;
    while ((res = mapiMessageStoresTable->QueryRows(1, 0, &rows)) == S_OK) {
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
            QString name(QStringFromLpctstr(rows->aRow[0].lpProps[nameColumn].Value.LPSZ));
            /**** Test copy begin, TODO remove ****/
            SPropValue storeRecordKey;
            SPropValue storeEntryKey;
            PropCopyMore(&storeEntryKey, &(rows->aRow[0].lpProps[entryIdColumn]), MAPIAllocateMore, 0);
            PropCopyMore(&storeRecordKey, &(rows->aRow[0].lpProps[recordKeyColumn]), MAPIAllocateMore, 0);
            QByteArray storeRecordKeyA((const char*)rows->aRow[0].lpProps[recordKeyColumn].Value.bin.lpb, rows->aRow[0].lpProps[recordKeyColumn].Value.bin.cb);
            QByteArray storeRecordKeyB((const char*)storeRecordKey.Value.bin.lpb, storeRecordKey.Value.bin.cb);
            /**** Test copy end ****/
            if (_mapiSession->OpenMsgStore(0, cbEntryId, lpEntryId, 0, flags, &mapiStore) == S_OK)
                result = MapiStorePtr(new MapiStore(mapiStore, storeKey, name));
            FreeProws(rows);
            break;
        }
        FreeProws(rows);
    }
    if (res != S_OK) {
        *lastError = QMessageStore::ContentInaccessible;
    }
    MAPIFreeBuffer(rows);
    mapiMessageStoresTable->Release();
    return result;
}

namespace {

QDateTime fromFileTime(const FILETIME &ft)
{
    SYSTEMTIME st = {0};
    FileTimeToSystemTime(&ft, &st);
    QString dateStr(QString("yyyy%1M%2d%3h%4m%5s%6z%7").arg(st.wYear).arg(st.wMonth).arg(st.wDay).arg(st.wHour).arg(st.wMinute).arg(st.wSecond).arg(st.wMilliseconds)); 
    return QDateTime::fromString(dateStr, "'yyyy'yyyy'M'M'd'd'h'h'm'm's's'z'z");
}

typedef QPair<QString, QString> StringPair;

QList<StringPair> decomposeHeaders(const QString &headers)
{
    QList<StringPair> result;

    if (!headers.isEmpty()) {
        int lastIndex = 0;
        int index = 0;

        do {
            lastIndex = index;

            // Find CRLF not followed by whitespace
            QRegExp lineSeparator("\r\n(?!\\s)");
            index = headers.indexOf(lineSeparator, lastIndex);

            QString line = headers.mid(lastIndex, (index == -1 ? -1 : (index - lastIndex)));
            index += 2;

            // Split the current line
            QRegExp headerIdentifier("\\s*(\\w+)\\s*:");
            if (line.indexOf(headerIdentifier) == 0) {
                result.append(qMakePair(headerIdentifier.cap(1), line.mid(headerIdentifier.cap(0).length()).trimmed()));
            } else {
                // Assume the whole line is an identifier
                result.append(qMakePair(line, QString()));
            }
        } while (index != -1);
    }

    return result;
}

QMessageAddress createAddress(const QString &name, const QString &address)
{
    QMessageAddress result;

    if (!name.isEmpty() || !address.isEmpty()) {
        QString from;
        if (!name.isEmpty() && !address.isEmpty()) {
            from = name + " <" + address + ">";
        } else {
            from = (!name.isEmpty() ? name : address);
        }
            
        result = QMessageAddress(from, QMessageAddress::Email);
    }

    return result;
}

}

QMessage MapiSession::message(QMessageStore::ErrorCode *lastError, const QMessageId& id) const
{
    QMessage result;
    QMessageId newId(id);
    MapiEntryId entryId(QMessageIdPrivate::entryId(id));
    MapiRecordKey messageRecordKey(QMessageIdPrivate::messageRecordKey(id));
    MapiRecordKey folderRecordKey(QMessageIdPrivate::folderRecordKey(id));
    MapiRecordKey storeRecordKey(QMessageIdPrivate::storeRecordKey(id));
    MapiStorePtr mapiStore(findStore(lastError, QMessageAccountIdPrivate::from(storeRecordKey)));
    if (*lastError != QMessageStore::NoError)
        return result;

    LPMESSAGE message;
    QMessageStore::ErrorCode ignoredError;
#if 0 // force lookup using record keys, TODO remove
    if (true) {
#else
    if (openEntry(&ignoredError, entryId, &message) != S_OK) {
#endif
        MapiFolderPtr parentFolder(mapiStore->findFolder(lastError, folderRecordKey));
        if (*lastError != QMessageStore::NoError)
            return result;

        qDebug() << "parentFolder" << parentFolder->name(); // TODO remove this
        entryId = parentFolder->messageEntryId(lastError, messageRecordKey);
        if (!entryId.count() || (openEntry(lastError, entryId, &message) != S_OK)) {
            *lastError = QMessageStore::InvalidId;
            return result;
        }

        newId = QMessageIdPrivate::from(messageRecordKey, folderRecordKey, storeRecordKey, entryId);
    }

    result = QMessagePrivate::from(newId);

    SizedSPropTagArray( 8, msgCols) = { 8, { PR_RECORD_KEY, 
                                             PR_MESSAGE_FLAGS, 
                                             PR_SENDER_NAME, 
                                             PR_SENDER_EMAIL_ADDRESS, 
                                             PR_CLIENT_SUBMIT_TIME, 
                                             PR_MESSAGE_DELIVERY_TIME, 
                                             PR_TRANSPORT_MESSAGE_HEADERS, 
                                             PR_SUBJECT }};
    ULONG count;
    LPSPropValue properties;
    HRESULT rv = message->GetProps(reinterpret_cast<LPSPropTagArray>(&msgCols), MAPI_UNICODE, &count, &properties);
    if (HR_SUCCEEDED(rv)) {
        QString senderName;
        QString senderAddress;
        QMessage::StatusFlags flags(0);

        for (ULONG n = 0; n < count; ++n) {
            switch (properties[n].ulPropTag) {
            case PR_MESSAGE_FLAGS:
                if (properties[n].Value.ul & MSGFLAG_READ) {
                    flags |= QMessage::Read;
                }
                result.setStatus(flags);
                break;
            case PR_SENDER_NAME:
                senderName = QStringFromLpctstr(properties[n].Value.LPSZ);
                break;
            case PR_SENDER_EMAIL_ADDRESS:
                senderAddress = QStringFromLpctstr(properties[n].Value.LPSZ);
                break;
            case PR_CLIENT_SUBMIT_TIME:
                result.setDate(fromFileTime(properties[n].Value.ft));
                break;
            case PR_MESSAGE_DELIVERY_TIME:
                result.setReceivedDate(fromFileTime(properties[n].Value.ft));
                break;
            case PR_TRANSPORT_MESSAGE_HEADERS:
                foreach (const StringPair &pair, decomposeHeaders(QStringFromLpctstr(properties[n].Value.LPSZ))) {
                    result.appendHeaderField(pair.first.toAscii(), pair.second);
                }
                break;
            case PR_SUBJECT:
                result.setSubject(QStringFromLpctstr(properties[n].Value.LPSZ));
                break;
            default:
                break;
            }
        }

        if (!senderName.isEmpty() || !senderAddress.isEmpty()) {
            result.setFrom(createAddress(senderName, senderAddress));
        }
    } else {
        *lastError = QMessageStore::ContentInaccessible;
    }

    // Extract the recipients for the message
    IMAPITable *recipientsTable(0);
    rv = message->GetRecipientTable(0, &recipientsTable);
    if (HR_SUCCEEDED(rv)) {
        SizedSPropTagArray(3, rcpCols) = {3, { PR_DISPLAY_NAME, PR_EMAIL_ADDRESS, PR_RECIPIENT_TYPE}};
        LPSRowSet rows(0);

        rv = HrQueryAllRows(recipientsTable, (SPropTagArray*)&rcpCols, NULL, NULL, 0, &rows);
        if (HR_SUCCEEDED(rv)) {
            QMessageAddressList to;
            QMessageAddressList cc;
            QMessageAddressList bcc;

            for (uint n = 0; n < rows->cRows; ++n) {
                QMessageAddressList *list = 0;
                switch (rows->aRow[n].lpProps[2].Value.l) {
                case MAPI_TO:
                    list = &to;
                    break;
                case MAPI_CC:
                    list = &cc;
                    break;
                case MAPI_BCC:
                    list = &bcc;
                    break;
                default:
                    break;
                }

                if (list) {
                    QString name(QStringFromLpctstr(rows->aRow[n].lpProps[0].Value.LPSZ));
                    QString address(QStringFromLpctstr(rows->aRow[n].lpProps[1].Value.LPSZ));

                    list->append(createAddress(name, address));
                }
            }

            if (!to.isEmpty()) {
                result.setTo(to);
            }
            if (!cc.isEmpty()) {
                result.setCc(cc);
            }
            if (!bcc.isEmpty()) {
                result.setBcc(bcc);
            }

            FreeProws(rows);
        } else {
            *lastError = QMessageStore::ContentInaccessible;
        }

        recipientsTable->Release();
    } else {
        *lastError = QMessageStore::ContentInaccessible;
    }

    message->Release();
    return result;
}
