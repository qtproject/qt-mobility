/* License goes here */
#ifndef MessagingAPI
#define MessagingAPI
#include <QString>
#include <QByteArray>
#include <QList>
#include <QDataStream>
#include <QStringList>
#include <QDateTime>
#include <QObject>
#include <QFlags>

class QMessageId 
{
public:
    QMessageId();
    QMessageId(const QMessageId &other);
    QMessageId(const QString &id);
    bool operator==(const QMessageId &other) const;
    QMessageId& operator=(const QMessageId &other);

    QString toString() const;
    bool isValid() const;

private:
    // ...
};

typedef QList<QMessageId> QMessageIdList;


class QMessageContentContainerId
{
public:
    QMessageContentContainerId();
    QMessageContentContainerId(const QMessageContentContainerId &other);
    QMessageContentContainerId(const QString &id);
    bool operator==(const QMessageContentContainerId &other) const;
    QMessageContentContainerId& operator=(const QMessageContentContainerId &other);

    QString toString() const;
    bool isValid() const;

private:
    // ...
};

typedef QList<QMessageContentContainerId> QMessageContentContainerIdList;


class QMessageFolderId 
{
public:
    QMessageFolderId();
    QMessageFolderId(const QMessageFolderId &other);
    QMessageFolderId(const QString &id);
    bool operator==(const QMessageFolderId &other) const;
    QMessageFolderId& operator=(const QMessageFolderId &other);

    QString toString() const;
    bool isValid() const;

private:
    // ...
};

typedef QList<QMessageFolderId> QMessageFolderIdList;


class QMessageAccountId 
{
public:
    QMessageAccountId();
    QMessageAccountId(const QMessageAccountId &other);
    QMessageAccountId(const QString &id);
    bool operator==(const QMessageAccountId &other) const;
    QMessageAccountId& operator=(const QMessageAccountId &other);

    QString toString() const;
    bool isValid() const;

private:
    // ...
};

typedef QList<QMessageAccountId> QMessageAccountIdList;


class QMessageContentContainer {
public:
    QMessageContentContainer();
    virtual ~QMessageContentContainer();

    virtual QMessageContentContainerId containerId() const;
    virtual QMessageId messageId() const;

    virtual void setContentType(const QByteArray &data);
    virtual QByteArray contentType() const;
    virtual void setContentSubType(const QByteArray &data);
    virtual QByteArray contentSubType() const;
    virtual void setContentCharset(const QByteArray &data);
    virtual QByteArray contentCharset() const;
    virtual void setContentFileName(const QByteArray &data);
    virtual QByteArray contentFileName() const;

    virtual bool contentAvailable() const;
    virtual uint indicativeSize() const;

    virtual QString decodedTextContent() const;
    virtual QByteArray decodedContent() const;
    virtual QString decodedContentFileName() const;
    virtual void writeContentTo(QDataStream &out) const;

    virtual void setContent(const QString &text);
    virtual void setContent(const QByteArray &data);
    virtual void setContentFromFile(const QString &fileName);
    virtual void readContentFrom(QDataStream &in);

    virtual QMessageContentContainerId appendContent(const QMessageContentContainer &content);
    virtual void replaceContent(const QMessageContentContainerId &id, const QMessageContentContainer &content);
    virtual void clearContents();
    virtual QMessageContentContainerIdList contentIds() const;

    virtual const QMessageContentContainer container(const QMessageContentContainerId) const;
    virtual QMessageContentContainer container(const QMessageContentContainerId);

    virtual void appendHeaderField(const QByteArray &name, const QString &value);
    virtual void setHeaderField(const QByteArray &name, const QString &value);
    virtual QString headerField(const QByteArray &name) const;
    virtual QList<QString> headerFieldValues(const QByteArray &name) const;
    virtual QList<QByteArray> headerFields() const;
    virtual void appendHeaderField(const QByteArray &name, const QByteArray &value);
    virtual void setHeaderField(const QByteArray &name, const QByteArray &value);

    static void setPreferredCharsets(const QList<QByteArray> &charsetNames);
    static QList<QByteArray> preferredCharsets();
    virtual bool containerDataModified() const;

protected:
    virtual QMessageContentContainerId prependContent(const QMessageContentContainer &content);

private:
    friend class QMessageStore;

    virtual void setContainerId(const QMessageContentContainerId &id);
    virtual void setContainerDataModified(bool modified);
    virtual void setContentAvailable(bool available);
    virtual void setIndicativeSize(uint size);

    // ...
};

class QMessageAddress {
public:
    enum Type {
        System = 1,
        Phone,
        Email,
        Xmpp
        // Extensible
    };

    QMessageAddress();
    QMessageAddress(const QString &recipient, Type type);
    virtual ~QMessageAddress();
    QString recipient() const;
    void setRecipient(const QString &recipient);
    Type type() const;
    void setType(Type type);

private:
    // ...
};

typedef QList<QMessageAddress> QMessageAddressList;


class QMessage : public QMessageContentContainer {
public:
    enum Type
    {
        Mms     = 0x1,
        Sms     = 0x2,
        Email   = 0x4,
        Xmpp    = 0x8,
        // Extensible
        None    = 0,
        AnyType = 0xFFFFFFFF
    };
    Q_DECLARE_FLAGS(TypeFlags, Type)

    enum Status
    {
        Read = 0x1,
        HasAttachments = 0x2,
        Incoming = 0x4,
        Removed = 0x8
    };
    Q_DECLARE_FLAGS(StatusFlags, Status)

    enum Priority
    {
        High = 1,
        Normal,
        Low
    };

    enum StandardFolder
    {
        InboxFolder = 1,
        OutboxFolder,
        DraftsFolder,
        SentFolder,
        TrashFolder
    };

    QMessage();
    QMessage(const QMessageId &id);
    virtual ~QMessage();

    static QMessage fromTransmissionFormat(Type t, const QByteArray &ba);
    static QMessage fromTransmissionFormatFile(Type t, const QString &fileName);

    virtual QByteArray toTransmissionFormat() const;
    virtual void toTransmissionFormat(QDataStream &out) const;

    virtual QMessageId id() const;

    virtual Type type() const;
    virtual void setType(Type t);

    virtual QMessageAccountId parentAccountId() const;
    virtual QMessageFolderId parentFolderId() const;

    virtual StandardFolder standardFolder() const;
    virtual void setStandardFolder(StandardFolder sf);

    virtual QMessageAddress from() const;
    virtual void setFrom(const QMessageAddress &address);

    virtual QString subject() const;
    virtual void setSubject(const QString &s);

    virtual QDateTime date() const;
    virtual void setDate(const QDateTime &d);

    virtual QDateTime receivedDate() const;
    virtual void setReceivedDate(const QDateTime &d);

    virtual QMessageAddressList to() const;
    virtual void setTo(const QMessageAddressList &toList);
    virtual void setTo(const QMessageAddress &address);
    virtual QMessageAddressList cc() const;
    virtual void setCc(const QMessageAddressList &ccList);
    virtual QMessageAddressList bcc() const;
    virtual void setBcc(const QMessageAddressList &bccList);

    virtual StatusFlags status() const;
    virtual void setStatus(StatusFlags newStatus);

    virtual Priority priority() const;
    virtual void setPriority(Priority newPriority);

    virtual uint size() const;

    virtual QMessageContentContainerId body() const;
    virtual void setBody(const QString &body);
    virtual void setBodyFromFile(const QString &fileName);

    virtual QMessageContentContainerIdList attachments() const;
    virtual void appendAttachments(const QStringList &fileNames);
    virtual void clearAttachments();

    virtual void setOriginatorPort(uint port);
    virtual uint originatorPort();
    virtual void setDestinationPort(uint port);
    virtual uint destinationPort();

    virtual QString customField(const QString &name) const;
    virtual void setCustomField(const QString &name, const QString &value);
    virtual QList<QString> customFields() const;

    virtual bool dataModified() const;

    virtual QMessage replyTo() const;
    virtual QMessage replyToAll() const;
    virtual QMessage forward() const;

private:
    friend class QMessageStore;

    void setId(const QMessageId &id);
    void setDataModified(bool modified);
    void setParentAccountId(const QMessageAccountId &accountId);
    void setParentFolderId(const QMessageFolderId &folderId);
    void setSize(uint size);
    // ...
};
Q_DECLARE_OPERATORS_FOR_FLAGS(QMessage::TypeFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(QMessage::StatusFlags)


class QMessageFolder {
public:
    QMessageFolder();
    QMessageFolder(const QMessageFolderId & id);
    virtual ~QMessageFolder();

    QMessageFolderId id() const;
    QMessageAccountId parentAccountId() const;
    QMessageFolderId parentFolderId() const;

    QString displayName() const;
    QString path() const;

private:
    // ...
};


class QMessageAccount {
public:
    QMessageAccount();
    QMessageAccount(const QMessageAccountId &id);
    virtual ~QMessageAccount();

    QMessageAccountId id() const;
    QString name() const;
    QMessageAddress fromAddress() const;
    QMessage::TypeFlags types() const;
    QString signature() const;

    static QMessageAccountId defaultAccount(QMessage::Type type);

private:
    // ...
};


namespace QMessageDataComparator {

    enum EqualityComparator
    {
        Equal,
        NotEqual
    };

    enum InclusionComparator 
    {
        Includes,
        Excludes
    };

    enum RelationComparator
    {
        LessThan,
        LessThanEqual,
        GreaterThan,
        GreaterThanEqual
    };

    enum Option
    {
        FullWord        = 0x1,
        CaseSensitive   = 0x2
    };
    Q_DECLARE_FLAGS(Options, Option)

}
Q_DECLARE_OPERATORS_FOR_FLAGS(QMessageDataComparator::Options)


class QMessageAccountFilterKey
{
public:

    void setOptions(QMessageDataComparator::Options options);
    QMessageDataComparator::Options options();

    QMessageAccountFilterKey();
    bool isEmpty() const;

    QMessageAccountFilterKey operator~() const;
    QMessageAccountFilterKey operator&(const QMessageAccountFilterKey &other) const;
    QMessageAccountFilterKey operator|(const QMessageAccountFilterKey &other) const;
    const QMessageAccountFilterKey& operator&=(const QMessageAccountFilterKey &other);
    const QMessageAccountFilterKey& operator|=(const QMessageAccountFilterKey &other);

    bool operator==(const QMessageAccountFilterKey &other) const;
    const QMessageAccountFilterKey& operator=(const QMessageAccountFilterKey &other);

    static QMessageAccountFilterKey id(const QMessageId &id, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageAccountFilterKey id(const QMessageIdList &ids, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
    static QMessageAccountFilterKey id(const QMessageAccountFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageAccountFilterKey fromAddress(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageAccountFilterKey fromAddress(const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageAccountFilterKey name(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageAccountFilterKey name(const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

private:
    // ...
};


class QMessageAccountSortKey {
public:
    QMessageAccountSortKey();
    bool isEmpty() const;

    bool operator==(const QMessageAccountSortKey &other) const;
    const QMessageAccountSortKey& operator=(const QMessageAccountSortKey &other);

    static QMessageAccountSortKey name(Qt::SortOrder order = Qt::AscendingOrder);

private:
    // ...
};


class QMessageFolderFilterKey
{
public:

    void setOptions(QMessageDataComparator::Options options);
    QMessageDataComparator::Options options();

    QMessageFolderFilterKey();
    bool isEmpty() const;

    QMessageFolderFilterKey operator~() const;
    QMessageFolderFilterKey operator&(const QMessageFolderFilterKey &other) const;
    QMessageFolderFilterKey operator|(const QMessageFolderFilterKey &other) const;
    const QMessageFolderFilterKey& operator&=(const QMessageFolderFilterKey &other);
    const QMessageFolderFilterKey& operator|=(const QMessageFolderFilterKey &other);

    bool operator==(const QMessageFolderFilterKey &other) const;
    const QMessageFolderFilterKey& operator=(const QMessageFolderFilterKey &other);

    static QMessageFolderFilterKey id(const QMessageId &id, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFolderFilterKey id(const QMessageIdList &ids, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
    static QMessageFolderFilterKey id(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFolderFilterKey displayName(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFolderFilterKey displayName(const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFolderFilterKey path(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFolderFilterKey path(const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFolderFilterKey parentAccountId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFolderFilterKey parentAccountId(const QMessageAccountFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFolderFilterKey parentFolderId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFolderFilterKey parentFolderId(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
    static QMessageFolderFilterKey ancestorFolderIds(const QMessageFolderId &id, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
    static QMessageFolderFilterKey ancestorFolderIds(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

private:
    // ...
};


class QMessageFolderSortKey {
public:
    QMessageFolderSortKey();
    bool isEmpty() const;

    QMessageFolderSortKey operator+(const QMessageFolderSortKey &other) const;
    QMessageFolderSortKey& operator+=(const QMessageFolderSortKey &other);

    bool operator==(const QMessageFolderSortKey &other) const;
    const QMessageFolderSortKey& operator=(const QMessageFolderSortKey &other);

    static QMessageFolderSortKey displayName(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageFolderSortKey path(Qt::SortOrder order = Qt::AscendingOrder);

private:
    // ...
};


class QMessageFilterKey
{
public:

    void setOptions(QMessageDataComparator::Options options);
    QMessageDataComparator::Options options();

    QMessageFilterKey();
    bool isEmpty() const;

    QMessageFilterKey operator~() const;
    QMessageFilterKey operator&(const QMessageFilterKey &other) const;
    QMessageFilterKey operator|(const QMessageFilterKey &other) const;
    const QMessageFilterKey& operator&=(const QMessageFilterKey &other);
    const QMessageFilterKey& operator|=(const QMessageFilterKey &other);

    bool operator==(const QMessageFilterKey &other) const;
    const QMessageFilterKey& operator=(const QMessageFilterKey &other);

    static QMessageFilterKey id(const QMessageId &id, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey id(const QMessageIdList &ids, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
    static QMessageFilterKey id(const QMessageFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFilterKey type(QMessage::Type type, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFilterKey type(QMessage::TypeFlags type, QMessageDataComparator::InclusionComparator cmp);

    static QMessageFilterKey sender(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFilterKey sender(const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFilterKey recipients(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFilterKey recipients(const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFilterKey subject(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFilterKey subject(const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFilterKey body(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFilterKey body(const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFilterKey timeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey timeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp);

    static QMessageFilterKey receptionTimeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey receptionTimeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp);

    static QMessageFilterKey status(QMessage::Status value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFilterKey status(QMessage::StatusFlags mask, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFilterKey priority(QMessage::Priority priority, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);

    static QMessageFilterKey size(int value, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey size(int value, QMessageDataComparator::RelationComparator cmp);

    static QMessageFilterKey customField(const QString &name, const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFilterKey customField(const QString &name, const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFilterKey parentAccountId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey parentAccountId(const QMessageAccountFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
    static QMessageFilterKey parentFolderId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey parentFolderId(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
    static QMessageFilterKey ancestorFolderIds(const QMessageFolderId &id, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
    static QMessageFilterKey ancestorFolderIds(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

private:
    // ...
};


class QMessageSortKey {
public:
    QMessageSortKey();
    bool isEmpty() const;

    QMessageSortKey operator+(const QMessageSortKey &other) const;
    QMessageSortKey& operator+=(const QMessageSortKey &other);

    bool operator==(const QMessageSortKey &other) const;
    const QMessageSortKey& operator=(const QMessageSortKey &other);

    static QMessageSortKey type(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageSortKey sender(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageSortKey recipients(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageSortKey subject(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageSortKey timeStamp(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageSortKey receptionTimeStamp(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageSortKey status(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageSortKey priority(Qt::SortOrder order = Qt::AscendingOrder);

    static QMessageSortKey size(Qt::SortOrder order = Qt::AscendingOrder);

private:
    // ...
};


class QMessageStore : public QObject
{
    Q_OBJECT

public:
    enum RemovalOption
    {
        NoRemovalRecord = 1,
        CreateRemovalRecord
    };

    enum ErrorCode
    {
        NoError = 0,
        InvalidId, 
        ConstraintFailure,
        ContentInaccessible,
        NotYetImplemented,
        FrameworkFault,
        WorkingIdsOverflow
    };

    QMessageStore::ErrorCode lastError() const;

    QMessageIdList queryMessages(const QMessageFilterKey &key, const QMessageSortKey &sortKey, uint limit = 0, uint offset = 0) const;
    QMessageFolderIdList queryFolders(const QMessageFolderFilterKey &key, const QMessageFolderSortKey &sortKey, uint limit = 0, uint offset = 0) const;
    QMessageAccountIdList queryAccounts(const QMessageAccountFilterKey &key, const QMessageAccountSortKey &sortKey, uint limit = 0, uint offset = 0) const;
    int countMessages(const QMessageFilterKey &key) const;
    int countFolders(const QMessageFolderFilterKey &key) const;
    int countAccounts(const QMessageAccountFilterKey &key) const;

    bool addMessage(QMessage *m);
    bool updateMessage(QMessage *m);
    bool removeMessage(const QMessageId &id, RemovalOption option = NoRemovalRecord);
    bool removeMessages(const QMessageFilterKey &key, RemovalOption option = NoRemovalRecord);

    QMessage message(const QMessageId &id) const;
    QMessageFolder folder(const QMessageFolderId &id) const;
    QMessageAccount account(const QMessageAccountId &id) const;

    void setMaximumWorkingIds(uint maximumIds);
    uint maximumWorkingIds();
    
    static QMessageStore* instance();
    
signals:
    void messagesAdded(const QMessageIdList &ids);
    void messagesRemoved(const QMessageIdList &ids);
    void messagesUpdated(const QMessageIdList &ids);

public slots:
    void startNotifications(const QMessageFilterKey &key);
    void stopNotifications(const QMessageFilterKey &key);

private:
    QMessageStore();
    // ...
};


class QMessageServiceAction : public QObject
{
    Q_OBJECT

public:
    enum Activity {
        Pending = 0,
        InProgress,
        Successful,
        Failed
    };

    void send(const QMessage &message, const QMessageAccountId &accountId);
    void compose(const QMessage &message);
    void retrieve(const QMessageId &id);
    void retrieve(const QMessageContentContainerId &id);
    void show(const QMessageId &id);
    Activity activity() const;
    QString lastErrorString() const;

public slots:
    void cancelOperation();

signals:
    void activityChanged(QMessageServiceAction::Activity a);

private:
    // ...
};
#endif
