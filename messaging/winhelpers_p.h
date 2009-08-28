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
#ifndef QMESSAGEWINHELPERPRIVATE_H
#define QMESSAGEWINHELPERPRIVATE_H
#include <QPair>
#include <QSharedPointer>
#include <QString>
#include <QByteArray>
#include <MAPIUtil.h>
#include <qmessagestore.h>

typedef QByteArray MapiRecordKey;

class MapiEntryId : public QByteArray
{
public:
    MapiEntryId():QByteArray(){}
    MapiEntryId(LPENTRYID mapiEntryId, ULONG entryIdLength)
    :
    QByteArray(reinterpret_cast<const char*>(mapiEntryId),entryIdLength){}

    MapiEntryId(LPBYTE mapiEntryId, ULONG entryIdLength)
    :
    QByteArray(reinterpret_cast<const char*>(mapiEntryId),entryIdLength){}
};

class MapiFolder;
class MapiStore;
class MapiSession;
typedef QSharedPointer<MapiFolder> MapiFolderPtr;
typedef QSharedPointer<MapiStore> MapiStorePtr;
typedef QSharedPointer<MapiSession> MapiSessionPtr;

namespace WinHelpers {

typedef QPair<QMessageId, ULONG> AttachmentLocator;
QMessageContentContainer fromLocator(const WinHelpers::AttachmentLocator &l);

}

QString QStringFromLpctstr(LPCTSTR lpszValue);

class MapiFolder {
public:

    enum CommonFolder { Drafts, Tasks, Notes, Journal, Contacts, Appointments };


    MapiFolder();
    MapiFolder(LPMAPIFOLDER folder, MapiRecordKey key, MapiRecordKey parentStoreKey, const QString &name, const MapiEntryId &entryId, bool hasSubFolders, uint messageCount);
    ~MapiFolder();
    MapiFolderPtr nextSubFolder(QMessageStore::ErrorCode *lastError);
    QMessageIdList queryMessages(QMessageStore::ErrorCode *lastError, const QMessageFilterKey &key = QMessageFilterKey(), const QMessageSortKey &sortKey = QMessageSortKey(), uint limit = 0, uint offset = 0) const;
    MapiEntryId messageEntryId(QMessageStore::ErrorCode *lastError, const MapiRecordKey &messagekey);
#ifdef QMESSAGING_OPTIONAL_FOLDER
    QMessageFolderId id();
#endif
    bool isValid() { return _valid; }
    LPMAPIFOLDER folder() { return _folder; }
    MapiRecordKey recordKey() { return _key; }
    MapiRecordKey storeKey() { return _parentStoreKey; }
    QString name() const { return _name; }
    MapiEntryId entryId() { return _entryId; }
    bool hasSubFolders() { return _hasSubFolders; }
    uint messageCount() { return _messageCount; }
    MapiFolderPtr subFolder(CommonFolder commonFolder, QMessageStore::ErrorCode *lastError);
    LPMAPITABLE subFolders(QMessageStore::ErrorCode *lastError) { if (!_init) findSubFolders(lastError); return _subFolders; }
    IMessage *createMessage(QMessageStore::ErrorCode* lastError);

    IMessage *openMessage(QMessageStore::ErrorCode *lastError, const MapiEntryId &entryId);

    static MapiFolderPtr null() { return MapiFolderPtr(new MapiFolder()); }

    static MapiFolder* create(LPMAPIFOLDER mapifolder, MapiEntryId entryId, MapiRecordKey storeKey);

private:
    void findSubFolders(QMessageStore::ErrorCode *lastError);

    bool _valid;
    LPMAPIFOLDER _folder;
    MapiRecordKey _key;
    MapiRecordKey _parentStoreKey;
    QString _name;
    LPMAPITABLE _subFolders;
    uint _itemCount;
    MapiEntryId _entryId;
    bool _hasSubFolders;
    uint _messageCount;
    bool _init;

    enum columnOrder { entryIdColumn = 0, nameColumn, recordKeyColumn, countColumn, subFoldersColumn };
};

class MapiStore {
public:
    MapiStore();
    MapiStore(LPMDB store, MapiRecordKey key, const QString &name);
    ~MapiStore();

    bool isValid();
    MapiFolderPtr rootFolder(QMessageStore::ErrorCode *lastError);
    MapiFolderPtr receiveFolder(QMessageStore::ErrorCode *lastError);
    MapiFolderPtr findFolder(QMessageStore::ErrorCode *lastError, const MapiRecordKey &key);
#ifdef QMESSAGING_OPTIONAL_FOLDER
    QMessageFolderIdList folderIds(QMessageStore::ErrorCode *lastError);
    QMessageFolder folderFromId(QMessageStore::ErrorCode *lastError, const QMessageFolderId &folderId);
#endif
    QMessageAccountId id();
    QString name() { return _name; }

    IMessage *openMessage(QMessageStore::ErrorCode *lastError, const MapiEntryId &entryId);
    IMAPIFolder *openFolder(QMessageStore::ErrorCode *lastError, const MapiEntryId &entryId);

    LPMDB store() { return _store; }

    static MapiStorePtr null() { return MapiStorePtr(new MapiStore()); }

private:
    bool _valid;
    LPMDB _store;
    MapiRecordKey _key;
    QString _name;
};

class MapiSession {
public:
    MapiSession();
    MapiSession(QMessageStore::ErrorCode *lastError, bool mapiInitialized);
    ~MapiSession();

    bool isValid();

    HRESULT openEntry(QMessageStore::ErrorCode *lastError, MapiEntryId entryId, LPMESSAGE *message) const;
    HRESULT openEntry(QMessageStore::ErrorCode *lastError, MapiEntryId entryId, LPMAPIFOLDER *folder) const;
    HRESULT openEntry(QMessageStore::ErrorCode *lastError, MapiEntryId entryId, LPUNKNOWN *unknown) const;
    MapiStorePtr findStore(QMessageStore::ErrorCode *lastError, const QMessageAccountId &id = QMessageAccountId()) const;
    MapiStorePtr defaultStore(QMessageStore::ErrorCode *lastError) { return findStore(lastError); }
    QList<MapiStorePtr> allStores(QMessageStore::ErrorCode *lastError) const;
    QMessage message(QMessageStore::ErrorCode *lastError, const QMessageId& id) const;
    QByteArray attachmentData(QMessageStore::ErrorCode *lastError, const QMessageId& id, ULONG number) const;
    bool showForm(LPMESSAGE message, LPMAPIFOLDER folder, LPMDB store);

    IMAPISession* session() { return _mapiSession; }
    static MapiSessionPtr null() { return MapiSessionPtr(new MapiSession()); }

private:
    bool _valid;
    IMAPISession* _mapiSession;
};
#endif
