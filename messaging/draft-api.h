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

class MessageId 
{
public:
    MessageId();
    MessageId(const MessageId& other);
    MessageId(const QString& id);
    bool operator==(const MessageId& other) const;
    MessageId& operator=(const MessageId& other);

    QString toString() const;
    bool isValid() const;

private:
    // ...
};

class MessageContentId
{
public:
    MessageContentId();
    MessageContentId(const MessageContentId& other);
    MessageContentId(const QString& id);
    bool operator==(const MessageContentId& other) const;
    MessageConetentId& operator=(const MessageContentId& other);

    QString toString() const;
    bool isValid() const;

private:
    // ...
}

typedef QList<MessageId> MessageIdList;

class MessageContent {
public:
    MessageContent();
    MessageContent(const MessageContentId& id);
    virtual ~MessageContent();

    virtual MessageContentId id() const;
    virtual void setId(const MessageContentId &id);

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

class Message {
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
        Read = 0x1
    };

    Message();
    Message(const MessageId& id);
    virtual ~Message();

    static Message fromTransmissionFormat(MessageType t, const QByteArray &ba);
    static Message fromTransmissionFormatFile(MessageType t, const QString& fileName);

    virtual QByteArray toTransmissionFormat() const;
    virtual void toTransmissionFormat(QDataStream& out) const;

    virtual MessageId id() const;
    virtual void setId(const MessageId &id);

    virtual MessageType messageType() const;
    virtual void setMessageType(MessageType t);

    virtual QString from() const;
    virtual void setFrom(const QString &s);

    virtual QString subject() const;
    virtual void setSubject(const QString &s);

    virtual QDateTime date() const;
    virtual void setDate(const QDateTime &s);

    virtual QDateTime receivedDate() const;
    virtual void setReceivedDate(const QDateTime &s);

    virtual QList<QString> to() const;
    virtual void setTo(const QList<QString>& s);
    virtual void setTo(const QString& s);
    virtual QList<QString> cc() const;
    virtual void setCc(const QList<QString>& s);
    virtual QList<QString> bcc() const;
    virtual void setBcc(const QList<QString>& s);

    virtual MessageStatus status() const;
    virtual void setStatus(MessageStatus newStatus);

    virtual uint size() const;
    virtual void setSize(uint i);

    virtual MessageContentId body() const;
    virtual void setBody(const QString &b);

    virtual MessageContentIdList attachments() const;
    virtual void setAttachments(QStringList fileNames);

    virtual bool dataModified() const;

private:
    // ...
};

namespace MessageComparator {

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

enum PresenceComparator
{
    Present,
    Absent
};

}

class MessageFilterKey
{
public:
    MessageFilterKey();
    bool isEmpty() const;

    MessageFilterKey operator~() const;
    MessageFilterKey operator&(const MessageFilterKey& other) const;
    MessageFilterKey operator|(const MessageFilterKey& other) const;
    const MessageFilterKey& operator&=(const MessageFilterKey& other);
    const MessageFilterKey& operator|=(const MessageFilterKey& other);

    static MessageFilterKey id(const MessageId &id, MessageDataComparator::EqualityComparator cmp = MessageDataComparator::Equal);
    static MessageFilterKey id(const MessageIdList &ids, MessageDataComparator::InclusionComparator cmp = MessageComparator::Includes);
    static MessageFilterKey id(const MessageFilterKey &key, MessageDataComparator::InclusionComparator cmp = MessageDataComparator::Includes);

    static MessageFilterKey messageType(Message::MessageType type, MessageDataComparator::InclusionComparator cmp);

    static MessageFilterKey sender(const QString &value, MessageDataComparator::EqualityComparator cmp = MessageDataComparator::Equal);
    static MessageFilterKey sender(const QString &value, MessageDataComparator::InclusionComparator cmp);
    static MessageFilterKey sender(const QStringList &values, MessageDataComparator::InclusionComparator cmp = MessageDataComparator::Includes);

    static MessageFilterKey recipients(const QString &value, MessageDataComparator::EqualityComparator cmp = MessageDataComparator::Equal);
    static MessageFilterKey recipients(const QString &value, MessageDataComparator::InclusionComparator cmp);

    static MessageFilterKey subject(const QString &value, MessageDataComparator::EqualityComparator cmp = MessageDataComparator::Equal);
    static MessageFilterKey subject(const QString &value, MessageDataComparator::InclusionComparator cmp);
    static MessageFilterKey subject(const QStringList &values, MessageDataComparator::InclusionComparator cmp = MessageDataComparator::Includes);

    static MessageFilterKey timeStamp(const QDateTime &value, MessageDataComparator::EqualityComparator cmp = MessageDataComparator::Equal);
    static MessageFilterKey timeStamp(const QDateTime &value, MessageDataComparator::RelationComparator cmp);

    static MessageFilterKey receptionTimeStamp(const QDateTime &value, MessageDataComparator::EqualityComparator cmp = MessageDataComparator::Equal);
    static MessageFilterKey receptionTimeStamp(const QDateTime &value, MessageDataComparator::RelationComparator cmp);

    static MessageFilterKey status(quint64 mask, MessageDataComparator::EqualityComparator cmp);
    static MessageFilterKey status(quint64 mask, MessageDataComparator::InclusionComparator cmp = MessageDataComparator::Includes);

    static MessageFilterKey size(int value, MessageDataComparator::EqualityComparator cmp = MessageDataComparator::Equal);
    static MessageFilterKey size(int value, MessageDataComparator::RelationComparator cmp);

private:
    // ...
};

class MessageSortKey {
public:
    MessageSortKey();
    bool isEmpty() const;

    MessageSortKey operator&(const MessageSortKey& other) const;
    MessageSortKey& operator&=(const MessageSortKey& other);

    static MessageSortKey id(Qt::SortOrder order = Qt::AscendingOrder);
    static MessageSortKey messageType(Qt::SortOrder order = Qt::AscendingOrder);
    static MessageSortKey sender(Qt::SortOrder order = Qt::AscendingOrder);
    static MessageSortKey recipients(Qt::SortOrder order = Qt::AscendingOrder);
    static MessageSortKey subject(Qt::SortOrder order = Qt::AscendingOrder);
    static MessageSortKey timeStamp(Qt::SortOrder order = Qt::AscendingOrder);
    static MessageSortKey receptionTimeStamp(Qt::SortOrder order = Qt::AscendingOrder);
    static MessageSortKey status(Qt::SortOrder order = Qt::AscendingOrder);
    static MessageSortKey size(Qt::SortOrder order = Qt::AscendingOrder);

private:
    // ...
};

class MessageStore : public QObject
{
    Q_OBJECT

public:
    enum MessageRemovalOption
    {
        NoRemovalRecord = 1,
        CreateRemovalRecord
    };

    MessageIdList queryMessages(const MessageFilterKey &key, const MessageSortKey &sortKey) const;
    int countMessages(const MessageFilterKey& key) const;
    bool removeMessage(const MessageId& id, MessageRemovalOption option = NoRemovalRecord);
    bool removeMessages(const MessageFilterKey& key, MessageRemovalOption option = NoRemovalRecord);
    bool updateMessage(Message *m);
    Message message(const MessageId& id) const;
    MessageContent messageContent(const MessageContentId& id) const;

    static MessageStore* instance();
    
signals:
    void messagesAdded(const MessageIdList &ids);

slots:
    startNotifications(const MessageFilterKey &key);
    stopNotifications(const MessageFilterKey &key);

private:
    MessageStore();
    // ...
};

class MessageServiceAction : public QObject
{
    Q_OBJECT

    enum Activity {
        Pending = 0,
        InProgress,
        Successful,
        Failed
    };

public:
    void compose(const Message &message);
    void send(const Message &message);
    void retrieve(const MessageId& id);
    void showNew(const MessageId& id);

public slots:
    void cancelOperation();

signals:
    void activityChanged(MessageServiceAction::Activity a);

private:
    // ...
};
#endif
