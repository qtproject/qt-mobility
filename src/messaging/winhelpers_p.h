/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
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
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QMESSAGEWINHELPERPRIVATE_H
#define QMESSAGEWINHELPERPRIVATE_H

#include <QHash>
#include <QMap>
#include <QPair>
#include <QSharedPointer>
#include <QString>
#include <QByteArray>
#include <MAPIUtil.h>
#include <qmessagemanager.h>
#include <QVector>
#include <QQueue>
#include <QEvent>

#ifndef _WIN32_WCE

#define USES_IID_IMAPIForm
#define USES_IID_IMAPIMessageSite
#define USES_IID_IPersistMessage

#include <initguid.h>
#include <mapiguid.h>
#include <mapiform.h>

#endif


// As per http://support.microsoft.com/kb/816477
#ifndef PR_MSG_EDITOR_FORMAT
#define PR_MSG_EDITOR_FORMAT PROP_TAG( PT_LONG, 0x5909 )
#define EDITOR_FORMAT_DONTKNOW ((ULONG)0)
#define EDITOR_FORMAT_PLAINTEXT ((ULONG)1)
#define EDITOR_FORMAT_HTML ((ULONG)2)
#ifndef _WIN32_WCE
#define EDITOR_FORMAT_RTF ((ULONG)3)
#else
// Not a real definition, but useful for us:
#define EDITOR_FORMAT_MIME ((ULONG)3)
#endif
#endif

#ifndef PR_ATTACH_CONTENT_ID
// This is not available in my SDK version...
#define PR_ATTACH_CONTENT_ID PROP_TAG( PT_UNICODE, 0x3712 )
#endif

#ifndef PR_BODY_HTML
#define PR_BODY_HTML PROP_TAG( PT_TSTRING, 0x1013 )
#endif

#define mapiRelease(a) if(a){a->Release();a=0;}


QTM_BEGIN_NAMESPACE

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

class MapiRecordKey : public QByteArray
{
public:
    MapiRecordKey():QByteArray(){}
    MapiRecordKey(LPENTRYID mapiRecordKey, ULONG entryIdLength)
    :
    QByteArray(reinterpret_cast<const char*>(mapiRecordKey),entryIdLength){}

    MapiRecordKey(LPBYTE mapiRecordKey, ULONG entryIdLength)
    :
    QByteArray(reinterpret_cast<const char*>(mapiRecordKey),entryIdLength){}
};

class MapiFolder;
class MapiStore;
class MapiSession;
typedef QSharedPointer<MapiFolder> MapiFolderPtr;
typedef QSharedPointer<MapiStore> MapiStorePtr;
typedef QSharedPointer<MapiSession> MapiSessionPtr;

namespace WinHelpers {

enum SavePropertyOption { DontSavePropertyChanges = 0, SavePropertyChanges };

QString QStringFromLpctstr(LPCTSTR lpszValue);
void LptstrFromQString(const QString &value, LPTSTR *lpsz);

typedef QPair<QMessageId, ULONG> AttachmentLocator;
QMessageContentContainer fromLocator(const WinHelpers::AttachmentLocator &l);

ULONG createNamedProperty(IMAPIProp *object, const QString &name);
ULONG getNamedPropertyTag(IMAPIProp *object, const QString &name);

bool setNamedProperty(IMAPIProp *object, ULONG tag, const QString &value);
QString getNamedProperty(IMAPIProp *object, ULONG tag);

QByteArray contentTypeFromExtension(const QString &extension);

class Lptstr : public QVector<TCHAR>
{
public:
    Lptstr():QVector<TCHAR>(){}
    Lptstr(int length) : QVector<TCHAR>(length){}
    operator TCHAR* (){ return QVector<TCHAR>::data(); }
};

Lptstr LptstrFromQString(const QString &src);

class MapiInitializer;

typedef QSharedPointer<MapiInitializer> MapiInitializationToken;

MapiInitializationToken initializeMapi();

class MapiInitializer
{
    friend MapiInitializationToken WinHelpers::initializeMapi();

private:
    MapiInitializer();
    MapiInitializer &operator=(const MapiInitializer &);

public:
    ~MapiInitializer();

    bool _initialized;
};

bool getMapiProperty(IMAPIProp *object, ULONG tag, ULONG *value);
bool getMapiProperty(IMAPIProp *object, ULONG tag, LONG *value);
bool getMapiProperty(IMAPIProp *object, ULONG tag, QByteArray *value);
bool getMapiProperty(IMAPIProp *object, ULONG tag, QString value);

bool setMapiProperty(IMAPIProp *object, ULONG tag, const QString &value);
bool setMapiProperty(IMAPIProp *object, ULONG tag, LONG value);
bool setMapiProperty(IMAPIProp *object, ULONG tag, ULONG value);
bool setMapiProperty(IMAPIProp *object, ULONG tag, bool value);
bool setMapiProperty(IMAPIProp *object, ULONG tag, FILETIME value);
bool setMapiProperty(IMAPIProp *object, ULONG tag, MapiEntryId value);

}

/* Note on links:
    - Session must close at exit for correct cleanup
        Session must be a singleton
        Stores must have weak ref to session
    - Stores must remain open after opening to enable notifications
        Session holds strong ref to stores
    - Folders should not remain open
        Store holds weak ref to folders
        Folder can have strong ref to store

    * Session keeps stores open
    * Folders keep store open
*/

class MapiFolder {

public:
    static MapiFolderPtr createFolder(QMessageManager::Error *error, const MapiStorePtr &store, IMAPIFolder *folder, const MapiRecordKey &recordKey, const QString &name, const MapiEntryId &entryId, bool hasSubFolders, uint messageCount);

    ~MapiFolder();

    MapiFolderPtr nextSubFolder(QMessageManager::Error *error);

    LPMAPITABLE queryBegin(QMessageManager::Error *error, const QMessageFilter &filter, const QMessageSortOrder &sortOrder);
    QMessageIdList queryNext(QMessageManager::Error *error, LPMAPITABLE messagesTable, const QMessageFilter &filter);
    void queryEnd(LPMAPITABLE messagesTable);

    uint countMessages(QMessageManager::Error *error, const QMessageFilter &filter = QMessageFilter()) const;

    void removeMessages(QMessageManager::Error *error, const QMessageIdList &ids);

    MapiEntryId messageEntryId(QMessageManager::Error *error, const MapiRecordKey &messagekey);

    QMessageFolderId id() const;

    QMessageAccountId accountId() const;
    QMessageFolderId parentId() const;
    QList<QMessageFolderId> ancestorIds() const;

    bool isValid() const { return _valid; }
    IMAPIFolder* folder() const { return _folder; }
    MapiRecordKey recordKey() const { return _key; }
    MapiRecordKey storeKey() const;
#ifdef _WIN32_WCE
    MapiEntryId storeEntryId() const;
#endif
    QString name() const { return _name; }
    MapiEntryId entryId() const { return _entryId; }
    bool hasSubFolders() const { return _hasSubFolders; }
    uint messageCount() const { return _messageCount; }

    IMessage *createMessage(QMessageManager::Error* error);
    IMessage *createMessage(QMessageManager::Error* error, const QMessage& source, const MapiSessionPtr &session, WinHelpers::SavePropertyOption saveOption = WinHelpers::SavePropertyChanges );

    IMessage *openMessage(QMessageManager::Error *error, const MapiEntryId &entryId);

    QMessageFolder folder(QMessageManager::Error *error, const QMessageFolderId& id) const;
    QMessage message(QMessageManager::Error *error, const QMessageId& id) const;

    QMessage::StandardFolder standardFolder() const;

private:
    MapiFolder();
    MapiFolder(const MapiStorePtr &store, IMAPIFolder *folder, const MapiRecordKey &recordKey, const QString &name, const MapiEntryId &entryId, bool hasSubFolders, uint messageCount);

    void findSubFolders(QMessageManager::Error *error);

    friend class MapiStore;

    QWeakPointer<MapiFolder> _self;
    MapiStorePtr _store;
    bool _valid;
    IMAPIFolder* _folder;
    MapiRecordKey _key;
    QString _name;
    MapiEntryId _entryId;
    bool _hasSubFolders;
    uint _messageCount;
    bool _init;
    QList<MapiEntryId> _subFolders;
};

class MapiStore {
public:
    static MapiStorePtr createStore(QMessageManager::Error *error, const MapiSessionPtr &session, IMsgStore *store, const MapiRecordKey &key, const MapiEntryId &entryId, const QString &name, bool cachedMode);

    ~MapiStore();

    MapiFolderPtr findFolder(QMessageManager::Error *error, QMessage::StandardFolder sf);

    QMessageFolderIdList folderIds(QMessageManager::Error *error) const;
    QMessageFolder folderFromId(QMessageManager::Error *error, const QMessageFolderId &folderId);

    QList<MapiFolderPtr> filterFolders(QMessageManager::Error *error, const QMessageFolderFilter &filter) const;

    MapiEntryId messageEntryId(QMessageManager::Error *error, const MapiRecordKey &folderKey, const MapiRecordKey &messageKey);

    MapiFolderPtr openFolder(QMessageManager::Error *error, const MapiEntryId& id) const;
    MapiFolderPtr openFolderWithKey(QMessageManager::Error *error, const MapiRecordKey& key) const;

    bool supports(ULONG featureFlag) const;

    bool isValid() const { return _valid; }
    QMessageAccountId id() const;
    MapiEntryId entryId() const { return _entryId; }
    QString name() const { return _name; }
    IMsgStore* store() const { return _store; }
    MapiRecordKey recordKey() const { return _key; }
    QMessage::TypeFlags types() const;
    QMessageAddress address() const;

    MapiSessionPtr session() const;

    MapiFolderPtr rootFolder(QMessageManager::Error *error) const;
    MapiFolderPtr receiveFolder(QMessageManager::Error *error) const;

    IMessage *openMessage(QMessageManager::Error *error, const MapiEntryId &entryId);

    QMessageFolder folder(QMessageManager::Error *error, const QMessageFolderId& id) const;
    QMessage message(QMessageManager::Error *error, const QMessageId& id) const;

    QMessage::StandardFolder standardFolder(const MapiEntryId &entryId) const;

    void notifyEvents(ULONG mask);

#ifdef _WIN32_WCE
    QString transportName() const;
#endif

private:
    MapiStore();
    MapiStore(const MapiSessionPtr &session, IMsgStore *store, const MapiRecordKey &key, const MapiEntryId &entryId, const QString &name, bool cachedMode);

    MapiEntryId standardFolderId(QMessageManager::Error *error, QMessage::StandardFolder sf) const;
    MapiEntryId rootFolderId(QMessageManager::Error *error) const;
    MapiEntryId receiveFolderId(QMessageManager::Error *error) const;

    IMAPIFolder *openMapiFolder(QMessageManager::Error *error, const MapiEntryId &entryId) const;

    bool setAdviseSink(ULONG mask, IMAPIAdviseSink *sink);

    class AdviseSink : public IMAPIAdviseSink
    {
        MapiStore *_store;
        LONG _refCount;

    public:
        AdviseSink(MapiStore *store) : _store(store), _refCount(0) {}

        STDMETHOD(QueryInterface)(REFIID riid, LPVOID FAR* ppvObj);
        STDMETHOD_(ULONG, AddRef)();
        STDMETHOD_(ULONG, Release)();

        STDMETHOD_(ULONG, OnNotify)(ULONG cNotification, LPNOTIFICATION lpNotifications);
    };

    QWeakPointer<MapiStore> _self;
    QWeakPointer<MapiSession> _session;
    bool _valid;
    IMsgStore* _store;
    MapiRecordKey _key;
    MapiEntryId _entryId;
    QString _name;
    bool _cachedMode;
    ULONG _adviseConnection;
    QMap<QMessage::StandardFolder, MapiEntryId> _standardFolderId;

    mutable QHash<MapiEntryId, QWeakPointer<MapiFolder> > _folderMap;
};

class MapiSession : public QObject
{
    Q_OBJECT

public:
    enum NotifyType { Added = 1, Removed, Updated };

    class NotifyEvent : public QEvent
    {
    public:
        static QEvent::Type eventType();

        NotifyEvent(MapiStore *store, const QMessageId &id, MapiSession::NotifyType type);

        virtual Type type();

        MapiStore *_store;
        QMessageId _id;
        NotifyType _notifyType;
    };

    static MapiSessionPtr createSession(QMessageManager::Error *error);

    ~MapiSession();

    bool isValid() const { return (_mapiSession != 0); }

    MapiStorePtr findStore(QMessageManager::Error *error, const QMessageAccountId &id = QMessageAccountId(), bool cachedMode = true) const;
    MapiStorePtr defaultStore(QMessageManager::Error *error, bool cachedMode = true) const { return findStore(error,QMessageAccountId(),cachedMode); }

    QList<MapiStorePtr> filterStores(QMessageManager::Error *error, const QMessageAccountFilter &filter, const QMessageAccountSortOrder &sortOrder = QMessageAccountSortOrder(), uint limit = 0, uint offset = 0, bool cachedMode = true) const;

    QList<MapiStorePtr> allStores(QMessageManager::Error *error, bool cachedMode = true) const;

    QList<MapiFolderPtr> filterFolders(QMessageManager::Error *error, const QMessageFolderFilter &filter, const QMessageFolderSortOrder &sortOrder = QMessageFolderSortOrder(), uint limit = 0, uint offset = 0, bool cachedMode = true) const;

    MapiStorePtr openStore(QMessageManager::Error *error, const MapiEntryId& id, bool cachedMode = true) const;
    MapiStorePtr openStoreWithKey(QMessageManager::Error *error, const MapiRecordKey& key, bool cachedMode = true) const;

    QMessageAccountId defaultAccountId(QMessageManager::Error *error, QMessage::Type type) const;

    MapiEntryId messageEntryId(QMessageManager::Error *error, const MapiRecordKey &storeKey, const MapiRecordKey &folderKey, const MapiRecordKey &messageKey);

    MapiRecordKey messageRecordKey(QMessageManager::Error *error, const QMessageId &id);
    MapiRecordKey folderRecordKey(QMessageManager::Error *error, const QMessageId &id);

#ifdef _WIN32_WCE
    MapiEntryId folderEntryId(QMessageManager::Error *error, const QMessageId &id);
#endif

    bool equal(const MapiEntryId &lhs, const MapiEntryId &rhs) const;

    QMessageFolder folder(QMessageManager::Error *error, const QMessageFolderId& id) const;
    QMessage message(QMessageManager::Error *error, const QMessageId& id) const;

    bool updateMessageProperties(QMessageManager::Error *error, QMessage *msg) const;
    bool updateMessageRecipients(QMessageManager::Error *error, QMessage *msg) const;
    bool updateMessageBody(QMessageManager::Error *error, QMessage *msg) const;
    bool updateMessageAttachments(QMessageManager::Error *error, QMessage *msg) const;

    bool haveAttachmentData(QMessageManager::Error* error, const QMessageId& id, ULONG number) const;
    QByteArray attachmentData(QMessageManager::Error *error, const QMessageId& id, ULONG number) const;

    QMessageIdList queryMessages(QMessageManager::Error *error, const QMessageFilter &filter, const QMessageSortOrder &sortOrder = QMessageSortOrder(), uint limit = 0, uint offset = 0, const QString &body = QString(), QMessageDataComparator::MatchFlags matchFlags = 0) const;

    void updateMessage(QMessageManager::Error* error, const QMessage& source);

    void removeMessages(QMessageManager::Error *error, const QMessageIdList &ids);

    IMAPISession* session() const { return _mapiSession; }

    QMessageManager::NotificationFilterId registerNotificationFilter(QMessageManager::Error *error, const QMessageFilter &filter);
    void unregisterNotificationFilter(QMessageManager::Error *error, QMessageManager::NotificationFilterId filterId);

    static QMessagePrivate *messageImpl(const QMessage &message);
    static QMessageContentContainerPrivate *containerImpl(const QMessageContentContainer &);

    void addToNotifyQueue(const NotifyEvent& e);
    void flushNotifyQueue();

signals:
    void messageAdded(const QMessageId &id, const QMessageManager::NotificationFilterIdSet &matchingFilterIds);
    void messageRemoved(const QMessageId &id, const QMessageManager::NotificationFilterIdSet &matchingFilterIds);
    void messageUpdated(const QMessageId &id, const QMessageManager::NotificationFilterIdSet &matchingFilterIds);

public slots:
    void dispatchNotifications();
    void processNotifyQueue();

private:
    MapiSession();
    MapiSession(QMessageManager::Error *error);

    IMsgStore *openMapiStore(QMessageManager::Error *error, const MapiEntryId &entryId, bool cachedMode = true) const;
    IMessage *openMapiMessage(QMessageManager::Error *error, const QMessageId &id, MapiStorePtr *storePtr = 0) const;

    void addRecipients(LPMESSAGE message, const QMessageAddressList& addressList, unsigned long mapiAddressType);
    void addAttachment(LPMESSAGE message, const QMessageContentContainer& attachmentContainer);

    bool event(QEvent *e);

    void notify(MapiStore *store, const QMessageId &id, NotifyType notifyType);

    template<typename Predicate, typename Ordering>
    QList<MapiStorePtr> filterStores(QMessageManager::Error *error, Predicate predicate, Ordering sortOrder, uint limit, uint offset, bool cachedMode) const;

private:
    friend class SessionManager;

    QWeakPointer<MapiSession> _self;
    WinHelpers::MapiInitializationToken _token;
    IMAPISession* _mapiSession;
    QMessageManager::NotificationFilterId _filterId;
    QMap<QMessageManager::NotificationFilterId, QMessageFilter> _filters;
    bool _registered;
    QQueue<NotifyEvent> _notifyEventQueue;

    mutable QHash<MapiEntryId, MapiStorePtr> _storeMap;
};

#ifndef _WIN32_WCE

class MapiForm : public IMAPIMessageSite
{
public:
    MapiForm(IMsgStore* mapiStore,
             IMAPISession* mapiSession,
             IMAPIFolder* mapiFolder,
             IMessage* mapiMessage);

    virtual ~MapiForm();

    // IUnknown interface
    STDMETHODIMP QueryInterface (REFIID riid, void** ppvObj);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();
    STDMETHODIMP GetLastError(HRESULT hResult, ulong flags, LPMAPIERROR FAR * lppMAPIError);

    // IMAPIMessageSite interface
    STDMETHODIMP GetSession(IMAPISession* FAR * mapiSession);
    STDMETHODIMP GetStore(IMsgStore* FAR * mapiStore);
    STDMETHODIMP GetFolder(IMAPIFolder* FAR * mapiFolder);
    STDMETHODIMP GetMessage(IMessage* FAR * mapiMessage);
    STDMETHODIMP GetFormManager(IMAPIFormMgr* FAR * ppFormMgr);
    STDMETHODIMP NewMessage(ULONG fComposeInFolder,
                            IMAPIFolder* mapiFolder,
                            IPersistMessage* pPersistMessage,
                            IMessage* FAR * mapiMessage,
                            IMAPIMessageSite* FAR * ppMessageSite,
                            LPMAPIVIEWCONTEXT FAR * ppViewContext);
    STDMETHODIMP CopyMessage(IMAPIFolder* pFolderDestination);
    STDMETHODIMP MoveMessage(IMAPIFolder* pFolderDestination,
                             LPMAPIVIEWCONTEXT pViewContext,
                             LPCRECT prcPosRect);
    STDMETHODIMP DeleteMessage(LPMAPIVIEWCONTEXT pViewContext, LPCRECT prcPosRect);
    STDMETHODIMP SaveMessage();
    STDMETHODIMP SubmitMessage(ulong flags);
    STDMETHODIMP GetSiteStatus(ulong* status);

    bool show();

private:
    HRESULT setPersistMessage(LPMAPIFORM lpForm, IPersistMessage* mapiPersistMessage);
    void releasePersistMessage();
    void releaseAll();

private:
    long m_referenceCount;
    IMAPIFolder* m_mapiFolder;
    IMessage* m_mapiMessage;
    IMsgStore* m_mapiStore;
    IMAPISession* m_mapiSession;
    IPersistMessage* m_mapiPersistMessage;
};

#endif

QTM_END_NAMESPACE

#endif
