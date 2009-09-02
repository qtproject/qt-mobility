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
#include "winhelpers_p.h"
#include "qmessageid_p.h"
#include "qmessagefolderid_p.h"
#include "qmessageaccountid_p.h"
#include "qmessage_p.h"
#include "qmessagefolder_p.h"
#include "qmessageaccount_p.h"
#include "qmessagesortkey_p.h"
#include "qmessagefilterkey_p.h"
#include <qdebug.h>
#include <shlwapi.h>
#include <shlguid.h>

// TODO Retrieve message count, and hasSubfolders flag for message folders.
// TODO determine if it is neccessary to use MAPI_MODIFY when opening folder in order to modify a message in a folder?
// TODO review error handling make sure lastError is being updated appropriately
// TODO Consider wrapping LPMAPITABLE and LPSRowSet
// TODO proper iterators for folders (for sub folders, messages, their entry ids and their record ids)

namespace {

GUID GuidPublicStrings = { 0x00020329, 0x0000, 0x0000, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 };

}

namespace WinHelpers {

// Note: UNICODE is always defined
QString QStringFromLpctstr(LPCTSTR lpszValue)
{
    if (!lpszValue || ::IsBadStringPtr(lpszValue, (UINT_PTR)-1)) // Don't crash when MAPI returns a bad string (and it does).
        return QString();

    return QString::fromUtf16(reinterpret_cast<const quint16*>(lpszValue));
}

void LptstrFromQString(const QString &src, LPTSTR* dst)
{
    uint length(src.length());
    *dst = new TCHAR[length + 1];

    const quint16 *data = src.utf16();
    const quint16 *it = data, *end = data + length;
    TCHAR *oit = *dst;
    for ( ; it != end; ++it, ++oit) {
        *oit = static_cast<TCHAR>(*it);
    }
    *oit = TCHAR('\0');
}

ULONG createNamedProperty(IMAPIProp *object, const QString &name)
{
    ULONG result = 0;

    if (!name.isEmpty()) {
        LPTSTR nameBuffer(0);
        LptstrFromQString(name, &nameBuffer);

        MAPINAMEID propName = { 0 };
        propName.lpguid = &GuidPublicStrings;
        propName.ulKind = MNID_STRING;
        propName.Kind.lpwstrName = nameBuffer;

        LPMAPINAMEID propNames = &propName;

        SPropTagArray *props;
        HRESULT rv = object->GetIDsFromNames(1, &propNames, MAPI_CREATE, &props);
        if (HR_SUCCEEDED(rv)) {
            result = props->aulPropTag[0] | PT_UNICODE;

            MAPIFreeBuffer(props);
        } else {
            qWarning() << "createNamedProperty: GetIDsFromNames failed";
        }

        delete [] nameBuffer;
    }

    return result;
}

ULONG getNamedPropertyTag(IMAPIProp *object, const QString &name)
{
    ULONG result = 0;

    if (!name.isEmpty()) {
        LPTSTR nameBuffer(0);
        LptstrFromQString(name, &nameBuffer);

        MAPINAMEID propName = { 0 };
        propName.lpguid = &GuidPublicStrings;
        propName.ulKind = MNID_STRING;
        propName.Kind.lpwstrName = nameBuffer;

        LPMAPINAMEID propNames = &propName;

        SPropTagArray *props;
        HRESULT rv = object->GetIDsFromNames(1, &propNames, 0, &props);
        if (HR_SUCCEEDED(rv)) {
            if (props->aulPropTag[0] != PT_ERROR) {
                result = props->aulPropTag[0] | PT_UNICODE;
            }

            MAPIFreeBuffer(props);
        } else {
            qWarning() << "getNamedPropertyTag: GetIDsFromNames failed";
        }

        delete [] nameBuffer;
    }

    return result;
}

bool setNamedProperty(IMAPIProp *object, ULONG tag, const QString &value)
{
    if (object && tag && !value.isEmpty()) {
        SPropValue prop = { 0 };
        prop.ulPropTag = tag;
        prop.Value.LPSZ = reinterpret_cast<LPTSTR>(const_cast<quint16*>(value.utf16()));

        HRESULT rv = object->SetProps(1, &prop, 0);
        if (HR_SUCCEEDED(rv)) {
            return true;
        } else {
            qWarning() << "setNamedProperty: SetProps failed";
        }
    }

    return false;
}

QString getNamedProperty(IMAPIProp *object, ULONG tag)
{
    QString result;

    if (object && tag) {
        SPropValue *prop(0);
        HRESULT rv = HrGetOneProp(object, tag, &prop);
        if (HR_SUCCEEDED(rv)) {
            result = QStringFromLpctstr(prop->Value.LPSZ);

            MAPIFreeBuffer(prop);
        } else if (rv != MAPI_E_NOT_FOUND) {
            qWarning() << "getNamedProperty: HrGetOneProp failed";
        }
    }

    return result;
}

}

using namespace WinHelpers;

MapiFolder::MapiFolder()
    :_valid(false),
     _folder(0),
     _subFolders(0),
     _hasSubFolders(false),
     _messageCount(0),
     _init(true)
{
}

#ifndef PR_IS_NEWSGROUP
#define PR_IS_NEWSGROUP PROP_TAG( PT_BOOLEAN, 0x6697 )
#endif
#ifndef PR_IS_NEWSGROUP_ANCHOR
#define PR_IS_NEWSGROUP_ANCHOR PROP_TAG( PT_BOOLEAN, 0x6696 )
#endif

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

    SizedSPropTagArray(3, columns) = {3, {PR_ENTRYID, PR_IS_NEWSGROUP, PR_IS_NEWSGROUP_ANCHOR}};
    if (subFolders->SetColumns(reinterpret_cast<LPSPropTagArray>(&columns), 0) == S_OK)
        _subFolders = subFolders;
}

MapiFolderPtr MapiFolder::createFolder(QMessageStore::ErrorCode *lastError, const MapiStorePtr &store, IMAPIFolder *folder, const MapiRecordKey &recordKey, const QString &name, const MapiEntryId &entryId, bool hasSubFolders, uint messageCount)
{
    return MapiFolderPtr(new MapiFolder(store, folder, recordKey, name, entryId, hasSubFolders, messageCount));
}

MapiFolder::MapiFolder(const MapiStorePtr &store, IMAPIFolder *folder, const MapiRecordKey &recordKey, const QString &name, const MapiEntryId &entryId, bool hasSubFolders, uint messageCount)
    :_store(store),
     _valid(true),
     _folder(folder),
     _key(recordKey),
     _name(name),
     _subFolders(0),
     _entryId(entryId),
     _hasSubFolders(hasSubFolders),
     _messageCount(messageCount),
     _init(false)
{
    if (!folder)
        _valid = false;
    //qDebug() << "MapiFolder::MapiFolder, name" << name << "hasSubfolders" << _hasSubFolders << "messageCount" << messageCount; //TODO remove debug
}

MapiFolder::~MapiFolder()
{
    //if (_valid)
        //qDebug() << "Releasing" << _name; // TODO remove
    if (_subFolders)
        _subFolders->Release();
    _subFolders = 0;
    if (_folder)
        _folder->Release();
    _folder = 0;
    _valid = false;
}

MapiFolderPtr MapiFolder::nextSubFolder(QMessageStore::ErrorCode *lastError, const MapiStore &store)
{
    MapiFolderPtr result;

    if (!_init)
        findSubFolders(lastError);

    if (!_hasSubFolders)
        return result;

    if (!_valid || !_folder || !_subFolders) {
        Q_ASSERT(_valid && _folder && _subFolders);
        *lastError = QMessageStore::FrameworkFault;
        return result;
    }

    while (true) {
        LPSRowSet rows(0);
        if (_subFolders->QueryRows(1, 0, &rows) == S_OK) {
            if (rows->cRows == 1) {
                SRow *row(&rows->aRow[0]);
                MapiEntryId entryId(row->lpProps[0].Value.bin.lpb, row->lpProps[0].Value.bin.cb);
                bool isNewsGroup = (row->lpProps[1].ulPropTag == PR_IS_NEWSGROUP && row->lpProps[1].Value.b);
                bool isNewsGroupAnchor = (row->lpProps[2].ulPropTag == PR_IS_NEWSGROUP_ANCHOR && row->lpProps[2].Value.b);

                FreeProws(rows);

                if (isNewsGroup || isNewsGroupAnchor) {
                    // Doesn't contain messages...
                }  else {
                    result = store.openFolder(lastError, entryId);
                    break;
                }
            } else {
                // We have retrieved all rows
                FreeProws(rows);
                break;
            }
        } else {
            *lastError = QMessageStore::ContentInaccessible;
            break;
        }
    }

    return result;
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

    QMessageFilterKeyPrivate::filterTable(lastError, key, messagesTable);
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
            LPSPropValue recordKeyProp(&rows->aRow[0].lpProps[recordKeyColumn]);
            MapiRecordKey recordKey(reinterpret_cast<const char*>(recordKeyProp->Value.bin.lpb), recordKeyProp->Value.bin.cb);
            MapiEntryId entryId(entryIdProp->Value.bin.lpb, entryIdProp->Value.bin.cb);
            result.append(QMessageIdPrivate::from(recordKey, _key, _store->recordKey(), entryId));
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
            MapiEntryId entryId(entryIdProp->Value.bin.lpb, entryIdProp->Value.bin.cb);
            result = entryId;
        } else {
            *lastError = QMessageStore::InvalidId;
        }
        FreeProws(rows);
    } else {
        *lastError = QMessageStore::InvalidId;
    }

    return result;
}

IMessage *MapiFolder::openMessage(QMessageStore::ErrorCode *lastError, const MapiEntryId &entryId)
{
    IMessage *message(0);

    LPENTRYID entryIdPtr(reinterpret_cast<LPENTRYID>(const_cast<char*>(entryId.data())));
    ULONG objectType(0);
    HRESULT rv = _folder->OpenEntry(entryId.count(), entryIdPtr, 0, MAPI_BEST_ACCESS, &objectType, reinterpret_cast<LPUNKNOWN*>(&message));
    if (rv != S_OK) {
        *lastError = QMessageStore::InvalidId;
    }

    return message;
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
QMessageFolderId MapiFolder::id() const
{
    return QMessageFolderIdPrivate::from(_key, _store->recordKey(), _entryId);
}
#endif

MapiRecordKey MapiFolder::storeKey() const
{
    return _store->recordKey();
}

static unsigned long commonFolderMap(MapiFolder::CommonFolder folder)
{
    static bool init = false;
    static QMap<MapiFolder::CommonFolder,unsigned long> propertyMap;

    if(!init)
    {
        propertyMap.insert(MapiFolder::Drafts,PROP_TAG(PT_BINARY, 0x36D7));
        propertyMap.insert(MapiFolder::Tasks,PROP_TAG(PT_BINARY, 0x36D4));
        propertyMap.insert(MapiFolder::Notes,PROP_TAG(PT_BINARY, 0x36D3));
        propertyMap.insert(MapiFolder::Appointments,PROP_TAG(PT_BINARY, 0x36D0));
        propertyMap.insert(MapiFolder::Journal,PROP_TAG(PT_BINARY, 0x36D2));
        propertyMap.insert(MapiFolder::Contacts,PROP_TAG(PT_BINARY, 0x36D1));
        init = true;
    }

    return propertyMap.value(folder);

}

MapiFolderPtr MapiFolder::subFolder(QMessageStore::ErrorCode *lastError, CommonFolder commonFolder, const MapiStore &store)
{
    MapiFolderPtr result;

    LPSPropValue props= 0;
    ULONG cValues=0;
    ULONG rgTags[]={ 1, commonFolderMap(commonFolder)};
    LPMAPIFOLDER targetFolder = 0;
    HRESULT hr = _folder->GetProps((LPSPropTagArray) rgTags, 0, &cValues, &props);
    if (hr != S_OK)
    {
        *lastError = QMessageStore::ContentInaccessible;
        return result;
    }

    MapiEntryId entryId(props[0].Value.bin.lpb, props[0].Value.bin.cb);
    MAPIFreeBuffer(props);

    return store.openFolder(lastError, entryId);
}

IMessage *MapiFolder::createMessage(QMessageStore::ErrorCode* lastError)
{
    IMessage *message = 0;

    if(HRESULT hr = _folder->CreateMessage(NULL, 0, &message)!=S_OK)
    {
        *lastError = QMessageStore::FrameworkFault;
        if(message) message->Release(); message =0;
    }
    return message;
}

MapiStore::MapiStore()
    :_valid(false),
     _store(0)
{
}

QHash<MapiEntryId, QWeakPointer<MapiFolder> > MapiStore::_folderMap;

MapiStorePtr MapiStore::createStore(QMessageStore::ErrorCode *lastError, const MapiSessionPtr &session, LPMDB store, const MapiRecordKey &key, const MapiEntryId &entryId, const QString &name)
{
    return MapiStorePtr(new MapiStore(session, store, key, entryId, name));
}

MapiStore::MapiStore(const MapiSessionPtr &session, LPMDB store, const MapiRecordKey &key, const MapiEntryId &entryId, const QString &name)
    :_session(session),
     _valid(true),
     _store(store),
     _key(key),
     _entryId(entryId),
     _name(name)
{
    //qDebug() << "MapiStore ctor - store:" << _store;
}

MapiStore::~MapiStore()
{
    //if (_valid)
        //qDebug() << "Releasing MAPI Store"; // TODO remove
    if (_store)
        _store->Release();
    _store = 0;
    _valid = false;
};

MapiFolderPtr MapiStore::rootFolder(QMessageStore::ErrorCode *lastError)
{
    MapiFolderPtr result;

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

    MapiEntryId entryId(rgProps[0].Value.bin.lpb, rgProps[0].Value.bin.cb);
    MAPIFreeBuffer(rgProps);

    return openFolder(lastError, entryId);
}

MapiFolderPtr MapiStore::receiveFolder(QMessageStore::ErrorCode *lastError)
{
    MapiFolderPtr result;

    ULONG entryIdSize = 0;
    LPENTRYID entryIdPtr = 0;

    HRESULT hr = _store->GetReceiveFolder(NULL, 0, &entryIdSize, &entryIdPtr, NULL);
    if (hr != S_OK)
    {
       *lastError = QMessageStore::FrameworkFault;
       return result;
    }

    MapiEntryId entryId(entryIdPtr, entryIdSize);
    MAPIFreeBuffer(entryIdPtr);

    return openFolder(lastError, entryId);
}

MapiFolderPtr MapiStore::findFolder(QMessageStore::ErrorCode *lastError, const MapiRecordKey &key)
{
    QList<MapiFolderPtr> folders;
    folders.append(rootFolder(lastError));

    while (!folders.isEmpty()) {
        MapiFolderPtr subFolder(folders.back()->nextSubFolder(lastError, *this));
        if (!subFolder.isNull() && subFolder->isValid()) {
            if (subFolder->recordKey() == key) {
                return subFolder;
            }
            folders.append(subFolder);
        } else {
            folders.pop_back();
        }
    }

    return MapiFolderPtr();
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
QMessageFolderIdList MapiStore::folderIds(QMessageStore::ErrorCode *lastError)
{
    QMessageFolderIdList folderIds;
    QList<MapiFolderPtr> folders;
    folders.append(rootFolder(lastError));
    // No valid reason to list the root folder.

    while (!folders.isEmpty()) {
        MapiFolderPtr subFolder(folders.back()->nextSubFolder(lastError, *this));
        if (!subFolder.isNull() && subFolder->isValid()) {
            folderIds.append(subFolder->id());
            folders.append(subFolder);
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
        MapiFolderPtr subFolder(folders.back()->nextSubFolder(lastError, *this));
        if (!subFolder.isNull() && subFolder->isValid()) {
            if (folderId == subFolder->id()) {
                QStringList path;
                for (int i = 0; i < folders.count(); ++i)
                    if (!folders[i]->name().isEmpty())
                        path.append(folders[i]->name());
                path.append(subFolder->name());
                result = QMessageFolderPrivate::from(subFolder->id(), id(), folders.last()->id(), subFolder->name(), path.join("/"));
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

IMessage *MapiStore::openMessage(QMessageStore::ErrorCode *lastError, const MapiEntryId &entryId)
{
    IMessage *message(0);

    LPENTRYID entryIdPtr(reinterpret_cast<LPENTRYID>(const_cast<char*>(entryId.data())));
    ULONG objectType(0);
    HRESULT rv = _store->OpenEntry(entryId.count(), entryIdPtr, 0, MAPI_BEST_ACCESS, &objectType, reinterpret_cast<LPUNKNOWN*>(&message));
    if (HR_SUCCEEDED(rv)) {
        if (objectType != MAPI_MESSAGE) {
            qWarning() << "Not a message - wrong object type:" << objectType;
            message->Release();
            message = 0;
            *lastError = QMessageStore::InvalidId;
        }
    } else {
        *lastError = QMessageStore::InvalidId;
        qDebug() << "Invalid message entryId:" << entryId.toBase64();
    }

    return message;
}

IMAPIFolder *MapiStore::openMapiFolder(QMessageStore::ErrorCode *lastError, const MapiEntryId &entryId) const
{
    IMAPIFolder *folder(0);

    // TODO:See MS KB article 312013, OpenEntry is not re-entrant, also true of MAPI functions in general?
    // TODO:See ms859378, GetPropsExample for alternative memory allocation technique
    LPENTRYID entryIdPtr(reinterpret_cast<LPENTRYID>(const_cast<char*>(entryId.data())));
    ULONG objectType(0);
    HRESULT rv = _store->OpenEntry(entryId.count(), entryIdPtr, 0, MAPI_BEST_ACCESS, &objectType, reinterpret_cast<LPUNKNOWN*>(&folder));
    if (HR_SUCCEEDED(rv)) {
        if (objectType != MAPI_FOLDER) {
            qWarning() << "Not a folder - wrong object type:" << objectType;
            folder->Release();
            folder = 0;
            *lastError = QMessageStore::InvalidId;
        }
    } else {
        *lastError = QMessageStore::InvalidId;
        qDebug() << "Invalid folder entryId:" << entryId.toBase64();
    }

    return folder;
}

QMessageAccountId MapiStore::id() const
{
    return QMessageAccountIdPrivate::from(_key);
}

QMessage::TypeFlags MapiStore::types() const
{
    QMessage::TypeFlags flags(QMessage::Email);

#ifdef _WIN32_WCE
    if (name().toUpper() == "SMS") {
        // On Windows Mobile SMS store is named "SMS"
        flags = QMessage::Sms;
    }
#endif

    return flags;
}

QMessageAddress MapiStore::address() const
{
    QMessageAddress result;


    return result;
}

MapiFolderPtr MapiStore::openFolder(QMessageStore::ErrorCode *lastError, const MapiEntryId& entryId) const
{
    MapiFolderPtr result(0);

    // See if we can create a new pointer to an existing folder
    QWeakPointer<MapiFolder> &existing = _folderMap[entryId];
    if (!existing.isNull()) {
        // Get a pointer to the existing folder
        result = existing.toStrongRef();
        if (!result.isNull()) {
            return result;
        }
        else qDebug() << "expired weak pointer";
    }

    // We need to create a new instance
    LPENTRYID entryIdPtr(reinterpret_cast<LPENTRYID>(const_cast<char*>(entryId.data())));
    IMAPIFolder *folder = openMapiFolder(lastError, entryId);
    if (folder && (*lastError == QMessageStore::NoError)) {
        SizedSPropTagArray(4, columns) = {4, {PR_DISPLAY_NAME, PR_RECORD_KEY, PR_CONTENT_COUNT, PR_SUBFOLDERS}};
        SPropValue *properties(0);
        ULONG count;
        HRESULT rv = folder->GetProps(reinterpret_cast<LPSPropTagArray>(&columns), MAPI_UNICODE, &count, &properties);
        if (HR_SUCCEEDED(rv)) {
            QString name(QStringFromLpctstr(properties[0].Value.LPSZ));
            MapiRecordKey recordKey(reinterpret_cast<const char*>(properties[1].Value.bin.lpb), properties[1].Value.bin.cb);
            uint messageCount = properties[2].Value.ul;
            bool hasSubFolders = properties[3].Value.b;

            MapiStorePtr self(_session->openStore(lastError, _entryId));
            MapiFolderPtr folderPtr = MapiFolder::createFolder(lastError, self, folder, recordKey, name, entryId, hasSubFolders, messageCount);
            if (*lastError == QMessageStore::NoError) {
                result = folderPtr;

                // Add to the map
                _folderMap.insert(entryId, result);
            } else {
                qWarning() << "Error creating folder object";
            }

            MAPIFreeBuffer(properties);
        } else {
            qWarning() << "Unable to access folder properties";
            folder->Release();
        }
    }

    return result;
}


QWeakPointer<MapiSession> MapiSession::_session;

QHash<MapiEntryId, QWeakPointer<MapiStore> > MapiSession::_storeMap;

MapiSessionPtr MapiSession::createSession(QMessageStore::ErrorCode *lastError, bool mapiInitialized)
{
    // See if we can create a new pointer to an existing session
    MapiSessionPtr ptr(_session.toStrongRef());
    if (ptr.isNull()) {
        // We need to create a new session
        ptr = MapiSessionPtr(new MapiSession(lastError, mapiInitialized));
        if (!ptr.isNull()) {
            // Keep a reference to the existing session
            _session = ptr;
        }
    }

    return ptr;
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
    //if (_valid)
        //qDebug() << "Logging out of mapi session"; // TODO remove
    if (_mapiSession) {
        _mapiSession->Logoff(0, 0, 0);
        _mapiSession->Release();
    }
    _mapiSession = 0;
    _valid = false;
}

MapiStorePtr MapiSession::findStore(QMessageStore::ErrorCode *lastError, const QMessageAccountId &id) const
{
    MapiStorePtr result(0);
    if (!_valid || !_mapiSession) {
        Q_ASSERT(_valid && _mapiSession);
        *lastError = QMessageStore::FrameworkFault;
        return result;
    }

    IMAPITable *mapiMessageStoresTable(0);
    const int nCols(3);
    enum { defaultStoreColumn = 0, entryIdColumn, recordKeyColumn };
    SizedSPropTagArray(nCols, columns) = {nCols, {PR_DEFAULT_STORE, PR_ENTRYID, PR_RECORD_KEY}};
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
            MapiEntryId entryId(rows->aRow[0].lpProps[entryIdColumn].Value.bin.lpb, rows->aRow[0].lpProps[entryIdColumn].Value.bin.cb);
            result = openStore(lastError, entryId);

            FreeProws(rows);
            break;
        }
        FreeProws(rows);
    }
    if (res != S_OK) {
        *lastError = QMessageStore::ContentInaccessible;
    }
    mapiMessageStoresTable->Release();
    return result;
}

QList<MapiStorePtr> MapiSession::allStores(QMessageStore::ErrorCode *lastError) const
{
    QList<MapiStorePtr> result;
    if (!_valid || !_mapiSession) {
        Q_ASSERT(_valid && _mapiSession);
        *lastError = QMessageStore::FrameworkFault;
        return result;
    }

    IMAPITable *mapiMessageStoresTable(0);
    if (_mapiSession->GetMsgStoresTable(0, &mapiMessageStoresTable) != S_OK) {
        *lastError = QMessageStore::ContentInaccessible;
    } else {
        SizedSPropTagArray(1, columns) = {1, {PR_ENTRYID}};
        LPSRowSet rows(0);
        HRESULT rv = HrQueryAllRows(mapiMessageStoresTable, reinterpret_cast<LPSPropTagArray>(&columns), 0, 0, 0, &rows);
        if (HR_SUCCEEDED(rv)) {
            for (uint n = 0; n < rows->cRows; ++n) {
                MapiEntryId entryId(rows->aRow[n].lpProps[0].Value.bin.lpb, rows->aRow[n].lpProps[0].Value.bin.cb);
                result.append(openStore(lastError, entryId));
                if (result.last().isNull()) qDebug() << "openStore failed - allStores";
            }

            FreeProws(rows);
        } else {
            *lastError = QMessageStore::ContentInaccessible;
        }

        mapiMessageStoresTable->Release();
    }

    return result;
}

IMsgStore *MapiSession::openMapiStore(QMessageStore::ErrorCode *lastError, const MapiEntryId &entryId) const
{
    IMsgStore *store(0);

    LPENTRYID entryIdPtr(reinterpret_cast<LPENTRYID>(const_cast<char*>(entryId.data())));
    HRESULT rv = _mapiSession->OpenMsgStore(0, entryId.count(), entryIdPtr, 0, MAPI_BEST_ACCESS | MDB_WRITE, reinterpret_cast<LPMDB*>(&store));
    if (HR_FAILED(rv)) {
        *lastError = QMessageStore::InvalidId;
        qDebug() << "Invalid store entryId:" << entryId.toBase64();
    }

    return store;
}

MapiStorePtr MapiSession::openStore(QMessageStore::ErrorCode *lastError, const MapiEntryId& entryId) const
{
    MapiStorePtr result(0);

    // See if we can create a new pointer to an existing store
    QWeakPointer<MapiStore> &existing = _storeMap[entryId];
    if (!existing.isNull()) {
        // Get a pointer to the existing store
        result = existing.toStrongRef();
        if (!result.isNull()) {
            return result;
        }
        else qDebug() << "expired weak pointer";
    }

    // We need to create a new instance
    IMsgStore *store = openMapiStore(lastError, entryId);
    if (store && (*lastError == QMessageStore::NoError)) {
        // Find the other properties of this store
        SizedSPropTagArray(2, columns) = {2, {PR_DISPLAY_NAME, PR_RECORD_KEY}};
        SPropValue *properties(0);
        ULONG count;
        HRESULT rv = store->GetProps(reinterpret_cast<LPSPropTagArray>(&columns), MAPI_UNICODE, &count, &properties);
        if (HR_SUCCEEDED(rv)) {
            QString name(QStringFromLpctstr(properties[0].Value.LPSZ));
            MapiRecordKey recordKey(reinterpret_cast<const char*>(properties[1].Value.bin.lpb), properties[1].Value.bin.cb);

            MapiSessionPtr self(MapiSession::createSession(lastError, true));
            MapiStorePtr storePtr = MapiStore::createStore(lastError, self, store, recordKey, entryId, name);
            if (*lastError == QMessageStore::NoError) {
                result = storePtr;

                // Add to the map
                _storeMap.insert(entryId, result);
            } else {
                qWarning() << "Error creating store object";
            }

            MAPIFreeBuffer(properties);
        } else {
            qWarning() << "Unable to access store properties";
            store->Release();
        }
    }

    return result;
}

namespace {

QDateTime fromFileTime(const FILETIME &ft)
{
    SYSTEMTIME st = {0};
    FileTimeToSystemTime(&ft, &st);
    QString dateStr(QString("yyyy%1M%2d%3h%4m%5s%6z%7").arg(st.wYear).arg(st.wMonth).arg(st.wDay).arg(st.wHour).arg(st.wMinute).arg(st.wSecond).arg(st.wMilliseconds)); 
    QDateTime dt(QDateTime::fromString(dateStr, "'yyyy'yyyy'M'M'd'd'h'h'm'm's's'z'z"));
    dt.setTimeSpec(Qt::UTC);
    return dt;
}

typedef QPair<QString, QString> StringPair;

QList<StringPair> decomposeHeaders(const QString &headers)
{
    QList<StringPair> result;

    if (!headers.isEmpty()) {
        int lastIndex = 0;
        int index = -2;

        do {
            index += 2;
            lastIndex = index;

            // Find CRLF not followed by whitespace
            QRegExp lineSeparator("\r\n(?!\\s)");
            index = headers.indexOf(lineSeparator, lastIndex);

            QString line = headers.mid(lastIndex, (index == -1 ? -1 : (index - lastIndex)));

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

QByteArray readStream(QMessageStore::ErrorCode *lastError, IStream *is)
{
    QByteArray result;
    STATSTG stg = { 0 };
    HRESULT rv = is->Stat(&stg, STATFLAG_NONAME);
    if (HR_SUCCEEDED(rv)) {
        char *data = new char[stg.cbSize.LowPart];
        ULONG bytes = 0;
        rv = is->Read(data, stg.cbSize.LowPart, &bytes);
        if (HR_SUCCEEDED(rv)) {
            result = QByteArray(data, bytes);
        } else {
            *lastError = QMessageStore::ContentInaccessible;
        }

        delete [] data;
    } else {
        *lastError = QMessageStore::ContentInaccessible;
    }

    return result;
}

QByteArray contentTypeFromExtension(const QString &extension)
{
    QByteArray result("application/octet-stream");

    if (!extension.isEmpty()) {
        IQueryAssociations *associations(0);
        HRESULT rv = AssocCreate(CLSID_QueryAssociations, IID_PPV_ARGS(&associations));
        if (HR_SUCCEEDED(rv)) {
            // Create the extension string to search for
            QString dotExtension(extension);
            if (!dotExtension.startsWith('.')) {
                dotExtension.prepend('.');
            }

            LPTSTR ext(0);
            LptstrFromQString(dotExtension, &ext);

            rv = associations->Init(0, ext, 0, 0);
            if (HR_SUCCEEDED(rv)) {
                // Find the length of the content-type string
                DWORD length = 0;
                rv = associations->GetString(0, ASSOCSTR_CONTENTTYPE, 0, 0, &length);
                if ((rv == S_FALSE) && length) {
                    // Retrieve the string
                    wchar_t *buffer = new wchar_t[length + 1];
                    buffer[length] = '\0';
                    rv = associations->GetString(0, ASSOCSTR_CONTENTTYPE, 0, buffer, &length);
                    if (rv == S_OK) {
                        QString output(QString::fromUtf16(reinterpret_cast<quint16*>(buffer)));
                        result = output.toLatin1();
                    }

                    delete [] buffer;
                }
            }

            delete [] ext;
            associations->Release();
        }
    }

    return result;
}

}

QMessageFolder MapiSession::folder(QMessageStore::ErrorCode *lastError, const QMessageFolderId& id) const
{
    QMessageFolder result;

    MapiRecordKey storeRecordKey(QMessageFolderIdPrivate::storeRecordKey(id));
    MapiStorePtr mapiStore(findStore(lastError, QMessageAccountIdPrivate::from(storeRecordKey)));
    if (*lastError != QMessageStore::NoError)
        return result;

    // Find the root folder for this store
    MapiFolderPtr storeRoot(mapiStore->rootFolder(lastError));
    if (*lastError != QMessageStore::NoError)
        return result;

    MapiEntryId entryId(QMessageFolderIdPrivate::entryId(id));
    MapiFolderPtr folder = mapiStore->openFolder(lastError, entryId);
    if (folder && (*lastError == QMessageStore::NoError)) {
        SizedSPropTagArray(3, columns) = {3, {PR_DISPLAY_NAME, PR_RECORD_KEY, PR_PARENT_ENTRYID}};
        SPropValue *properties(0);
        ULONG count;
        HRESULT rv = folder->folder()->GetProps(reinterpret_cast<LPSPropTagArray>(&columns), MAPI_UNICODE, &count, &properties);
        if (HR_SUCCEEDED(rv)) {
            QString displayName(QStringFromLpctstr(properties[0].Value.LPSZ));
            MapiRecordKey folderKey(reinterpret_cast<const char*>(properties[1].Value.bin.lpb), properties[1].Value.bin.cb);
            MapiEntryId parentEntryId(properties[2].Value.bin.lpb, properties[2].Value.bin.cb);

            MAPIFreeBuffer(properties);

            QMessageFolderId folderId(QMessageFolderIdPrivate::from(folderKey, storeRecordKey, entryId));

            QStringList path;
            path.append(displayName);

            QMessageFolderId parentId;
            MapiEntryId ancestorEntryId(parentEntryId);
            MapiFolderPtr ancestorFolder;

            // Iterate through ancestors towards the root
            while ((ancestorFolder = mapiStore->openFolder(lastError, ancestorEntryId)) &&
                   (ancestorFolder && (*lastError == QMessageStore::NoError))) {
                SPropValue *ancestorProperties(0);
                if (ancestorFolder->folder()->GetProps(reinterpret_cast<LPSPropTagArray>(&columns), MAPI_UNICODE, &count, &ancestorProperties) == S_OK) {
                    SPropValue &ancestorRecordKeyProp(ancestorProperties[1]);
                    MapiRecordKey ancestorRecordKey(reinterpret_cast<const char*>(ancestorRecordKeyProp.Value.bin.lpb), ancestorRecordKeyProp.Value.bin.cb);

                    if (ancestorEntryId == parentEntryId) {
                        // This ancestor is the parent of the folder being retrieved, create a QMessageFolderId for the parent
                        parentId = QMessageFolderIdPrivate::from(ancestorRecordKey, storeRecordKey, parentEntryId);
                    }

                    SPropValue &entryIdProp(ancestorProperties[2]);
                    ancestorEntryId = MapiEntryId(entryIdProp.Value.bin.lpb, entryIdProp.Value.bin.cb);

                    QString ancestorName(QStringFromLpctstr(ancestorProperties[0].Value.LPSZ));

                    MAPIFreeBuffer(ancestorProperties);

                    if (ancestorRecordKey == storeRoot->recordKey()) {
                        // Reached the root and have a complete path for the folder being retrieved
                        QMessageAccountId accountId(QMessageAccountIdPrivate::from(storeRecordKey));
                        return QMessageFolderPrivate::from(folderId, accountId, parentId, displayName, path.join("/"));
                    }

                    // Prepare to consider next ancestor
                    if (!ancestorName.isEmpty())
                        path.prepend(ancestorName);
                } else {
                    break;
                }
            }
        }
    }

    // Failed to quickly retrieve the folder, fallback to an exhaustive search of all folders
    result = mapiStore->folderFromId(lastError, id);
    return result;
}

QMessage MapiSession::message(QMessageStore::ErrorCode *lastError, const QMessageId& id) const
{
    QMessage result;

    MapiRecordKey storeRecordKey(QMessageIdPrivate::storeRecordKey(id));
    MapiStorePtr mapiStore(findStore(lastError, QMessageAccountIdPrivate::from(storeRecordKey)));
    if (*lastError != QMessageStore::NoError)
        return result;

    MapiEntryId entryId(QMessageIdPrivate::entryId(id));
    IMessage *message = mapiStore->openMessage(lastError, entryId);
    if (*lastError != QMessageStore::NoError)
        return result;

    result = QMessagePrivate::from(id);

    QByteArray messageBody;
    QByteArray bodySubType;
    bool hasAttachments = false;

    SizedSPropTagArray(10, msgCols) = {10, { PR_RECORD_KEY, 
                                             PR_MESSAGE_FLAGS, 
                                             PR_SENDER_NAME, 
                                             PR_SENDER_EMAIL_ADDRESS, 
                                             PR_CLIENT_SUBMIT_TIME, 
                                             PR_MESSAGE_DELIVERY_TIME, 
                                             PR_TRANSPORT_MESSAGE_HEADERS, 
                                             PR_HASATTACH,
                                             PR_SUBJECT,
#ifdef _WIN32_WCE
                                             PR_CONTENT_LENGTH
#else
                                             PR_MESSAGE_SIZE
#endif
                                             }};
    ULONG count = 0;
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
            case PR_HASATTACH:
                hasAttachments = (properties[n].Value.b != FALSE);
                break;
#ifdef _WIN32_WCE
            case PR_CONTENT_LENGTH:
#else
            case PR_MESSAGE_SIZE:
#endif
                QMessagePrivate::setSize(result, properties[n].Value.ul);
                break;
            default:
                break;
            }
        }

        if (!senderName.isEmpty() || !senderAddress.isEmpty()) {
            result.setFrom(createAddress(senderName, senderAddress));
            QMessagePrivate::setSenderName(result, senderName);
        }
    } else {
        *lastError = QMessageStore::ContentInaccessible;
    }

    if (*lastError != QMessageStore::NoError) {
        message->Release();
        return result;
    }

    // Extract the recipients for the message
    IMAPITable *recipientsTable(0);
    rv = message->GetRecipientTable(0, &recipientsTable);
    if (HR_SUCCEEDED(rv)) {
        SizedSPropTagArray(3, rcpCols) = {3, { PR_DISPLAY_NAME, PR_EMAIL_ADDRESS, PR_RECIPIENT_TYPE}};
        LPSRowSet rows(0);

        rv = HrQueryAllRows(recipientsTable, reinterpret_cast<LPSPropTagArray>(&rcpCols), NULL, NULL, 0, &rows);
        if (HR_SUCCEEDED(rv)) {
            QMessageAddressList to;
            QMessageAddressList cc;
            QMessageAddressList bcc;

            for (uint n = 0; n < rows->cRows; ++n) {
                QMessageAddressList *list = 0;

                SPropValue *props = rows->aRow[n].lpProps;
                switch (props[2].Value.l) {
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
                    QString name;
                    QString address;

                    if (props[0].ulPropTag == PR_DISPLAY_NAME)
                        name = QStringFromLpctstr(rows->aRow[n].lpProps[0].Value.LPSZ);
                    if (props[1].ulPropTag == PR_EMAIL_ADDRESS)
                        address = QStringFromLpctstr(rows->aRow[n].lpProps[1].Value.LPSZ);

                    if (!name.isEmpty() || ! address.isEmpty()) {
                        list->append(createAddress(name, address));
                    }
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

    if (*lastError != QMessageStore::NoError) {
        message->Release();
        return result;
    }

    // See if this message has any custom field data
    ULONG tag = getNamedPropertyTag(message, "customFieldData");
    if (tag) {
        QString customFieldData = getNamedProperty(message, tag);
        foreach (const QString &field, customFieldData.split("\n\n")) {
            int index = field.indexOf("\n");
            if (index) {
                result.setCustomField(field.left(index), field.mid(index + 1));
            }
        }
    }

    // See if this message has HTML body (encoded in RTF)
    IStream *is(0);
    rv = message->OpenProperty(PR_RTF_COMPRESSED, &IID_IStream, STGM_READ, 0, (IUnknown**)&is);
    if (HR_SUCCEEDED(rv)) {
        IStream *decompressor(0);
        if (WrapCompressedRTFStream(is, 0, &decompressor) == S_OK) {
            ULONG bytes = 0;
            char buffer[BUFSIZ] = { 0 };
            do {
                decompressor->Read(buffer, BUFSIZ, &bytes);
                messageBody.append(buffer, bytes);
            } while (bytes == BUFSIZ);

            decompressor->Release();

            // The RTF may contain HTML - parse that here?
            bodySubType = "rtf";
        } else {
            *lastError = QMessageStore::ContentInaccessible;
        }
    } else {
        // Fall back to a text body
        rv = message->OpenProperty(PR_BODY, &IID_IStream, STGM_READ, 0, (IUnknown**)&is);
        if (HR_SUCCEEDED(rv)) {
            messageBody = readStream(lastError, is);
            bodySubType = "plain";
        }
    }

    if (is)
        is->Release();

    if (*lastError != QMessageStore::NoError) {
        message->Release();
        return result;
    }

#ifndef PR_ATTACH_CONTENT_ID
// This is not available in my SDK version...
#define PR_ATTACH_CONTENT_ID PROP_TAG( PT_UNICODE, 0x3712 )
#endif

    if (!hasAttachments) {
        // Make the body the entire content of the message
        result.setContentType("text");
        result.setContentSubType(bodySubType);
        result.setContent(messageBody);
    } else {
        // Add the message body data as the first part
        QMessageContentContainer bodyPart;
        bodyPart.setContentType("text");
        bodyPart.setContentSubType(bodySubType);
        bodyPart.setContent(messageBody);

        result.setContentType("multipart");
        result.setContentSubType("related");
        result.appendContent(bodyPart);

        // Find any attachments for this message
        IMAPITable *attachmentsTable(0);
        rv = message->GetAttachmentTable(0, &attachmentsTable);
        if (HR_SUCCEEDED(rv)) {
            // Find the properties of these attachments
            SizedSPropTagArray(7, attCols) = {7, { PR_ATTACH_NUM, 
                                                   PR_ATTACH_EXTENSION, 
                                                   PR_ATTACH_LONG_FILENAME, 
                                                   PR_ATTACH_FILENAME, 
                                                   PR_ATTACH_CONTENT_ID,
                                                   PR_ATTACH_SIZE,
                                                   PR_RENDERING_POSITION }};
            LPSRowSet rows(0);

            rv = HrQueryAllRows(attachmentsTable, reinterpret_cast<LPSPropTagArray>(&attCols), NULL, NULL, 0, &rows);
            if (HR_SUCCEEDED(rv)) {
                for (uint n = 0; n < rows->cRows; ++n) {
                    LONG number(0);
                    QString extension;
                    QString filename;
                    QString contentId;
                    LONG size(0);
                    LONG renderingPosition(0);

                    // If not available, the output tag will not match our requested content tag
                    if (rows->aRow[n].lpProps[0].ulPropTag == PR_ATTACH_NUM) {
                        number = rows->aRow[n].lpProps[0].Value.l;
                    } else {
                        // We can't access this part...
                        continue;
                    }

                    if (rows->aRow[n].lpProps[1].ulPropTag == PR_ATTACH_EXTENSION) {
                        extension = QStringFromLpctstr(rows->aRow[n].lpProps[1].Value.LPSZ);
                    }
                    if (rows->aRow[n].lpProps[2].ulPropTag == PR_ATTACH_LONG_FILENAME) {
                        filename = QStringFromLpctstr(rows->aRow[n].lpProps[2].Value.LPSZ);
                    } else if (rows->aRow[n].lpProps[3].ulPropTag == PR_ATTACH_FILENAME) {
                        filename = QStringFromLpctstr(rows->aRow[n].lpProps[3].Value.LPSZ);
                    }
                    if (rows->aRow[n].lpProps[4].ulPropTag == PR_ATTACH_CONTENT_ID) {
                        contentId = QStringFromLpctstr(rows->aRow[n].lpProps[4].Value.LPSZ);
                    }
                    if (rows->aRow[n].lpProps[5].ulPropTag == PR_ATTACH_SIZE) {
                        size = rows->aRow[n].lpProps[5].Value.l;
                    }
                    if (rows->aRow[n].lpProps[6].ulPropTag == PR_RENDERING_POSITION) {
                        renderingPosition = rows->aRow[n].lpProps[6].Value.l;
                    }

                    WinHelpers::AttachmentLocator locator(id, number);

                    QMessageContentContainer container(WinHelpers::fromLocator(locator));
                    container.setContentFileName(filename.toAscii());
                    // TODO: No setSize() ?

                    if (!extension.isEmpty()) {
                        QByteArray contentType(contentTypeFromExtension(extension));
                        if (!contentType.isEmpty()) {
                            int index = contentType.indexOf('/');
                            if (index != -1) {
                                container.setContentType(contentType.left(index));
                                container.setContentSubType(contentType.mid(index + 1));
                            } else {
                                container.setContentType(contentType);
                            }
                        }
                    }
                    if (!contentId.isEmpty()) {
                        container.setHeaderField("Content-ID", contentId.toAscii());
                    }
                    if (renderingPosition == -1) {
                        QByteArray value("attachment");
                        if (!filename.isEmpty()) {
                            value.append("; filename=" + filename.toAscii());
                        }
                        container.setHeaderField("Content-Disposition", value);
                    }

                    result.appendContent(container);
                }

                FreeProws(rows);
            }
        } else {
            *lastError = QMessageStore::ContentInaccessible;
        }
    }

    message->Release();
    return result;
}

QByteArray MapiSession::attachmentData(QMessageStore::ErrorCode *lastError, const QMessageId& id, ULONG number) const
{
    QByteArray result;

    MapiRecordKey storeRecordKey(QMessageIdPrivate::storeRecordKey(id));
    MapiStorePtr mapiStore(findStore(lastError, QMessageAccountIdPrivate::from(storeRecordKey)));
    if (*lastError != QMessageStore::NoError)
        return result;

    MapiEntryId entryId(QMessageIdPrivate::entryId(id));
    IMessage *message = mapiStore->openMessage(lastError, entryId);
    if (*lastError != QMessageStore::NoError)
        return result;

    LPATTACH attachment(0);
    HRESULT rv = message->OpenAttach(number, 0, 0, &attachment);
    if (HR_SUCCEEDED(rv)) {
        IStream *is(0);
        rv = attachment->OpenProperty(PR_ATTACH_DATA_BIN, &IID_IStream, STGM_READ, 0, (IUnknown**)&is);
        if (HR_SUCCEEDED(rv)) {
            result = readStream(lastError, is);
            is->Release();
        }
    }

    message->Release();
    return result;
}

bool MapiSession::showForm(LPMESSAGE message, LPMAPIFOLDER folder, LPMDB store)
{
    ULONG messageToken;

    if(_mapiSession->PrepareForm(NULL,message, &messageToken )== S_OK)
    {
        ULONG messageStatus = 0;
        LPSPropValue pProp = 0;
        ULONG propertyCount = 0;
        ULONG p[2]={ 1,PR_MSG_STATUS};

        if(message->GetProps((LPSPropTagArray)p, MAPI_UNICODE, &propertyCount, &pProp) == S_OK)
        {
            messageStatus = pProp->Value.l;
            MAPIFreeBuffer(pProp);
        }

        ULONG messageFlags = 0;
        p[1] = PR_MESSAGE_FLAGS;
        if(message->GetProps((LPSPropTagArray)p, MAPI_UNICODE, &propertyCount, &pProp) == S_OK)
        {
            messageFlags = pProp->Value.l;
            MAPIFreeBuffer(pProp);
        }

        ULONG messageAccess = 0;
        p[1] = PR_ACCESS;
        if(message->GetProps((LPSPropTagArray)p, MAPI_UNICODE, &propertyCount, &pProp) == S_OK)
        {
            messageAccess = pProp->Value.l;
            MAPIFreeBuffer(pProp);
        }

        propertyCount = 0;
        p[1] = PR_MESSAGE_CLASS;
        if(message->GetProps((LPSPropTagArray)p, MAPI_UNICODE, &propertyCount, &pProp) == S_OK)
        {
#ifdef UNICODE
            char szMessageClass[256];
            WideCharToMultiByte(CP_ACP, 0, pProp->Value.LPSZ,-1, szMessageClass,255, NULL, NULL);
#else
            char* szMessageClass=pProp->Value.LPSZ;
#endif
            HRESULT hr=_mapiSession->ShowForm(NULL, store, folder, NULL,messageToken, NULL, 0,messageStatus,messageFlags,messageAccess, szMessageClass);
            MAPIFreeBuffer(pProp);
            if(hr!=S_OK)
            {
                qWarning() << "Show form failed";
                return false;
            }
            else if(hr==MAPI_E_USER_CANCEL)
            {
                qWarning() << "Show form cancelled";
                return false;
            }
        }
        else
        {
            qWarning() << "Failed to show form";
            return false;
        }
    }
    return true;
}

