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

class QMessageId 
{
public:
    QMessageId();
    QMessageId(const QMessageId& other);
    QMessageId(const QString& id);
    bool operator==(const QMessageId& other) const;
    QMessageId& operator=(const QMessageId& other);

    QString toString() const;
    bool isValid() const;

private:
    // ...
};

class QMessageContentId
{
public:
    QMessageContentId();
    QMessageContentId(const QMessageContentId& other);
    QMessageContentId(const QString& id);
    bool operator==(const QMessageContentId& other) const;
    QMessageContentId& operator=(const QMessageContentId& other);

    QString toString() const;
    bool isValid() const;

private:
    // ...
}

typedef QList<QMessageId> QMessageIdList;
typedef QList<QMessageContentId> QMessageContentIdList;

class QMessageContent {
public:
    QMessageContent();
    QMessageContent(const QMessageContentId& id);
    virtual ~QMessageContent();

    virtual QMessageContentId id() const;
    virtual void setId(const QMessageContentId &id);

    virtual bool contentAvailable() const;
    virtual uint indicativeSize() const;
    virtual QString contentType() const;
    virtual QString contentSubType() const;
    virtual QString fileName() const;

    virtual QString decodedTextContent() const;
    virtual QByteArray decodedContent() const;
    virtual QString decodedContentFileName() const;
    virtual void serialize(QDataStream& out) const;        

private:
    // ...
};

class QMessage {
public:
    enum MessageType
    {
        Mms     = 0x1,
        Sms     = 0x2,
        Email   = 0x4,
        None    = 0,
        AnyType = Mms | Sms | Email
    };

    enum MessageStatus
    {
        Read = 0x1,
        HasAttachments = 0x2
    };

    QMessage();
    QMessage(const QMessageId& id);
    virtual ~QMessage();

    static QMessage fromTransmissionFormat(MessageType t, const QByteArray &ba);
    static QMessage fromTransmissionFormatFile(MessageType t, const QString& fileName);

    virtual QByteArray toTransmissionFormat() const;
    virtual void toTransmissionFormat(QDataStream& out) const;

    virtual QMessageId id() const;
    virtual void setId(const QMessageId &id);

    virtual MessageType messageType() const;
    virtual void setMessageType(MessageType t);

    virtual QString from() const;
    virtual void setFrom(const QString &s);

    virtual QString subject() const;
    virtual void setSubject(const QString &s);

    virtual QDateTime date() const;
    virtual void setDate(const QDateTime &s);

    virtual QDateTime receivedDate() const;
    virtual void setReceivedDate(const QDateTime &d);

    virtual QList<QString> to() const;
    virtual void setTo(const QList<QString>& toList);
    virtual void setTo(const QString& s);
    virtual QList<QString> cc() const;
    virtual void setCc(const QList<QString>& ccList);
    virtual QList<QString> bcc() const;
    virtual void setBcc(const QList<QString>& bccList);

    virtual MessageStatus status() const;
    virtual void setStatus(MessageStatus newStatus);

    virtual uint size() const;
    virtual void setSize(uint size);

    virtual QMessageContentId body() const;
    virtual void setBody(const QString &body, bool html = false);

    virtual QMessageContentIdList attachments() const;
    virtual void setAttachments(QStringList fileNames);

    virtual bool dataModified() const;

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

}

class QMessageFilterKey
{
public:
    QMessageFilterKey();
    bool isEmpty() const;

    QMessageFilterKey operator~() const;
    QMessageFilterKey operator&(const QMessageFilterKey& other) const;
    QMessageFilterKey operator|(const QMessageFilterKey& other) const;
    const QMessageFilterKey& operator&=(const QMessageFilterKey& other);
    const QMessageFilterKey& operator|=(const QMessageFilterKey& other);

    bool operator==(const QMessageFilterKey& other) const;
    const QMessageFilterKey& operator=(const QMessageFilterKey& other);

    static QMessageFilterKey id(const QMessageId &id, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey id(const QMessageIdList &ids, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
    static QMessageFilterKey id(const QMessageFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFilterKey messageType(QMessage::MessageType type, QMailDataComparator::EqualityComparator cmp);
    static QMessageFilterKey messageType(QMessage::MessageType type, QMessageDataComparator::InclusionComparator cmp);

    static QMessageFilterKey sender(const QString &value, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey sender(const QString &value, QMessageDataComparator::InclusionComparator cmp);
    static QMessageFilterKey sender(const QStringList &values, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFilterKey recipients(const QString &value, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey recipients(const QString &value, QMessageDataComparator::InclusionComparator cmp);

    static QMessageFilterKey subject(const QString &value, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey subject(const QString &value, QMessageDataComparator::InclusionComparator cmp);
    static QMessageFilterKey subject(const QStringList &values, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFilterKey timeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey timeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp);

    static QMessageFilterKey receptionTimeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey receptionTimeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp);

    static QMessageFilterKey status(quint64 mask, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFilterKey status(quint64 mask, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFilterKey size(int value, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey size(int value, QMessageDataComparator::RelationComparator cmp);

private:
    // ...
};

class QMessageSortKey {
public:
    QMessageSortKey();
    bool isEmpty() const;

    QMessageSortKey operator&(const QMessageSortKey& other) const;
    QMessageSortKey& operator&=(const QMessageSortKey& other);

    bool operator==(const QMessageSortKey& other) const;
    const QMessageSortKey& operator=(const QMessageSortKey& other);

    static QMessageSortKey id(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageSortKey messageType(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageSortKey sender(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageSortKey recipients(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageSortKey subject(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageSortKey timeStamp(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageSortKey receptionTimeStamp(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageSortKey status(Qt::SortOrder order = Qt::AscendingOrder);
    static QMessageSortKey size(Qt::SortOrder order = Qt::AscendingOrder);

private:
    // ...
};

class QMessageStore : public QObject
{
    Q_OBJECT

public:
    enum MessageRemovalOption
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
        FrameworkFault
    };

    QMessageStore::ErrorCode lastError() const;
    QMessageIdList queryMessages(const QMessageFilterKey &key, const QMessageSortKey &sortKey) const;
    int countMessages(const QMessageFilterKey& key) const;
    bool removeMessage(const QMessageId& id, MessageRemovalOption option = NoRemovalRecord);
    bool removeMessages(const QMessageFilterKey& key, MessageRemovalOption option = NoRemovalRecord);
    bool updateMessage(QMessage *m);
    QMessage message(const QMessageId& id) const;
    QMessageContent messageContent(const QMessageContentId& id) const;

    static QMessageStore* instance();
    
signals:
    void messagesAdded(const QMessageIdList &ids);
    void messagesRemoved(const QMessageIdList &ids);
    void messagesUpdated(const QMessageIdList &ids);

slots:
    startNotifications(const QMessageFilterKey &key);
    stopNotifications(const QMessageFilterKey &key);

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
    void compose(const QMessage &message);
    void send(const QMessage &message);
    void retrieve(const QMessageId& id);
    void retrieve(const QMessageContentId& id);
    void showNew(const QMessageId& id);
    Activity activity() const;

public slots:
    void cancelOperation();

signals:
    void activityChanged(QMessageServiceAction::Activity a);

private:
    // ...
};
#endif
