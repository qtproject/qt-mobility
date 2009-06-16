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

    QMessageId messageId() const;

private:
    // ...
};

typedef QList<QMessageContentContainerId> QMessageContentContainerIdList;


class QMessageContentContainer {
public:
    QMessageContentContainer();
    virtual ~QMessageContentContainer();

    virtual QMessageContentContainerId containerId() const;

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
    virtual void setSize(uint size);

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

    QString customField(const QString &name) const;
    void setCustomField(const QString &name, const QString &value);
    QList<QString> customFields() const;

    virtual bool dataModified() const;

private:
    friend class QMessageStore;

    virtual void setId(const QMessageId &id);
    virtual void setDataModified(bool modified);

    // ...
};
Q_DECLARE_OPERATORS_FOR_FLAGS(QMessage::TypeFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(QMessage::StatusFlags)


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

}


class QMessageFilterKey
{
public:
    enum Option
    {
        FullWord        = 0x1,
        CaseInsensitive = 0x2
    };
    Q_DECLARE_FLAGS(Options, Option)

    void setOptions(Options options);
    Options options();

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
    static QMessageFilterKey customField(const QString &name, const QString &value, QMessageDataComparator::InclusionComparator cmp);

private:
    // ...
};
Q_DECLARE_OPERATORS_FOR_FLAGS(QMessageFilterKey::Options)


class QMessageSortKey {
public:
    QMessageSortKey();
    bool isEmpty() const;

    QMessageSortKey operator+(const QMessageSortKey &other) const;
    QMessageSortKey& operator+=(const QMessageSortKey &other);

    bool operator==(const QMessageSortKey &other) const;
    const QMessageSortKey& operator=(const QMessageSortKey &other);

    static QMessageSortKey id(Qt::SortOrder order = Qt::AscendingOrder);
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
    int countMessages(const QMessageFilterKey &key) const;
    bool addMessage(QMessage *m);
    bool updateMessage(QMessage *m);
    bool removeMessage(const QMessageId &id, RemovalOption option = NoRemovalRecord);
    bool removeMessages(const QMessageFilterKey &key, RemovalOption option = NoRemovalRecord);
    QMessage message(const QMessageId &id) const;
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

    enum Activity {
        Pending = 0,
        InProgress,
        Successful,
        Failed
    };

public:
    void send(const QMessage &message);
    void compose(const QMessage &message);
    void reply(const QMessageId &id);
    void forward(const QMessageId &id);
    void retrieve(const QMessageId &id);
    void retrieve(const QMessageContentContainerId &id);
    void show(const QMessageId &id);
    Activity activity() const;

public slots:
    void cancelOperation();

signals:
    void activityChanged(QMessageServiceAction::Activity a);

private:
    // ...
};
#endif
