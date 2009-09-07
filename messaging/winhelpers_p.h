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
#include <QHash>
#include <QPair>
#include <QSharedPointer>
#include <QString>
#include <QByteArray>
#include <MAPIUtil.h>
#include <qmessagestore.h>

// As per http://support.microsoft.com/kb/816477
#ifndef PR_MSG_EDITOR_FORMAT
#define PR_MSG_EDITOR_FORMAT PROP_TAG( PT_LONG, 0x5909 )
#define EDITOR_FORMAT_DONTKNOW ((ULONG)0)
#define EDITOR_FORMAT_PLAINTEXT ((ULONG)1)
#define EDITOR_FORMAT_HTML ((ULONG)2)
#define EDITOR_FORMAT_RTF ((ULONG)3)
#endif

#ifndef PR_ATTACH_CONTENT_ID
// This is not available in my SDK version...
#define PR_ATTACH_CONTENT_ID PROP_TAG( PT_UNICODE, 0x3712 )
#endif

#ifndef PR_BODY_HTML
#define PR_BODY_HTML PROP_TAG( PT_TSTRING, 0x1013 )
#endif

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

QString QStringFromLpctstr(LPCTSTR lpszValue);
void LptstrFromQString(const QString &value, LPTSTR *lpsz);

typedef QPair<QMessageId, ULONG> AttachmentLocator;
QMessageContentContainer fromLocator(const WinHelpers::AttachmentLocator &l);

ULONG createNamedProperty(IMAPIProp *object, const QString &name);
ULONG getNamedPropertyTag(IMAPIProp *object, const QString &name);

bool setNamedProperty(IMAPIProp *object, ULONG tag, const QString &value);
QString getNamedProperty(IMAPIProp *object, ULONG tag);

}

class MapiFolder {
public:

    enum CommonFolder { Drafts, Tasks, Notes, Journal, Contacts, Appointments };

    static MapiFolderPtr createFolder(QMessageStore::ErrorCode *lastError, const MapiStorePtr &store, IMAPIFolder *folder, const MapiRecordKey &recordKey, const QString &name, const MapiEntryId &entryId, bool hasSubFolders, uint messageCount);

    ~MapiFolder();

    MapiFolderPtr nextSubFolder(QMessageStore::ErrorCode *lastError, const MapiStore &store);
    QMessageIdList queryMessages(QMessageStore::ErrorCode *lastError, const QMessageFilter &filter = QMessageFilter(), const QMessageOrdering &ordering = QMessageOrdering(), uint limit = 0, uint offset = 0) const;
    MapiEntryId messageEntryId(QMessageStore::ErrorCode *lastError, const MapiRecordKey &messagekey);
#ifdef QMESSAGING_OPTIONAL_FOLDER
    QMessageFolderId id() const;
#endif

    bool isValid() const { return _valid; }
    LPMAPIFOLDER folder() const { return _folder; }
    MapiRecordKey recordKey() const { return _key; }
    MapiRecordKey storeKey() const;
    QString name() const { return _name; }
    MapiEntryId entryId() const { return _entryId; }
    bool hasSubFolders() const { return _hasSubFolders; }
    uint messageCount() const { return _messageCount; }

    MapiFolderPtr subFolder(QMessageStore::ErrorCode *lastError, CommonFolder commonFolder, const MapiStore &store);

    LPMAPITABLE subFolders(QMessageStore::ErrorCode *lastError) { if (!_init) findSubFolders(lastError); return _subFolders; }
    IMessage *createMessage(QMessageStore::ErrorCode* lastError);

    IMessage *openMessage(QMessageStore::ErrorCode *lastError, const MapiEntryId &entryId);

private:
    MapiFolder();
    MapiFolder(const MapiStorePtr &store, IMAPIFolder *folder, const MapiRecordKey &recordKey, const QString &name, const MapiEntryId &entryId, bool hasSubFolders, uint messageCount);

    void findSubFolders(QMessageStore::ErrorCode *lastError);

    friend class MapiStore;

    MapiStorePtr _store;
    bool _valid;
    LPMAPIFOLDER _folder;
    MapiRecordKey _key;
    QString _name;
    LPMAPITABLE _subFolders;
    uint _itemCount;
    MapiEntryId _entryId;
    bool _hasSubFolders;
    uint _messageCount;
    bool _init;
};

class MapiStore {
public:
    static MapiStorePtr createStore(QMessageStore::ErrorCode *lastError, const MapiSessionPtr &session, LPMDB store, const MapiRecordKey &key, const MapiEntryId &entryId, const QString &name);

    ~MapiStore();

    MapiFolderPtr rootFolder(QMessageStore::ErrorCode *lastError);
    MapiFolderPtr receiveFolder(QMessageStore::ErrorCode *lastError);
    MapiFolderPtr findFolder(QMessageStore::ErrorCode *lastError, const MapiRecordKey &key);
#ifdef QMESSAGING_OPTIONAL_FOLDER
    QMessageFolderIdList folderIds(QMessageStore::ErrorCode *lastError);
    QMessageFolder folderFromId(QMessageStore::ErrorCode *lastError, const QMessageFolderId &folderId);
#endif

    bool isValid() const { return _valid; }
    QMessageAccountId id() const;
    MapiEntryId entryId() const { return _entryId; }
    QString name() const { return _name; }
    LPMDB store() const { return _store; }
    MapiRecordKey recordKey() const { return _key; }
    QMessage::TypeFlags types() const;
    QMessageAddress address() const;

    IMessage *openMessage(QMessageStore::ErrorCode *lastError, const MapiEntryId &entryId);

    MapiFolderPtr openFolder(QMessageStore::ErrorCode *lastError, const MapiEntryId& id) const;

private:
    MapiStore();
    MapiStore(const MapiSessionPtr &session, LPMDB store, const MapiRecordKey &key, const MapiEntryId &entryId, const QString &name);

    IMAPIFolder *openMapiFolder(QMessageStore::ErrorCode *lastError, const MapiEntryId &entryId) const;

    MapiSessionPtr _session;
    bool _valid;
    LPMDB _store;
    MapiRecordKey _key;
    MapiEntryId _entryId;
    QString _name;

    static QHash<MapiEntryId, QWeakPointer<MapiFolder> > _folderMap;
};

class MapiSession {
public:
    static MapiSessionPtr createSession(QMessageStore::ErrorCode *lastError, bool mapiInitialized);

    ~MapiSession();

    bool isValid() const { return _valid; }

    MapiStorePtr findStore(QMessageStore::ErrorCode *lastError, const QMessageAccountId &id = QMessageAccountId()) const;
    MapiStorePtr defaultStore(QMessageStore::ErrorCode *lastError) const { return findStore(lastError); }
    QList<MapiStorePtr> allStores(QMessageStore::ErrorCode *lastError) const;

    QMessageFolder folder(QMessageStore::ErrorCode *lastError, const QMessageFolderId& id) const;

    QMessage message(QMessageStore::ErrorCode *lastError, const QMessageId& id) const;
    QByteArray attachmentData(QMessageStore::ErrorCode *lastError, const QMessageId& id, ULONG number) const;
    bool showForm(LPMESSAGE message, LPMAPIFOLDER folder, LPMDB store);

    MapiStorePtr openStore(QMessageStore::ErrorCode *lastError, const MapiEntryId& id) const;

    IMAPISession* session() const { return _mapiSession; }

private:
    MapiSession();
    MapiSession(QMessageStore::ErrorCode *lastError, bool mapiInitialized);

    IMsgStore *openMapiStore(QMessageStore::ErrorCode *lastError, const MapiEntryId &entryId) const;

    bool _valid;
    IMAPISession* _mapiSession;

    static QWeakPointer<MapiSession> _session;

    static QHash<MapiEntryId, QWeakPointer<MapiStore> > _storeMap;
};

#endif
