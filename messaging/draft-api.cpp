/* License goes here */
#include "draft-api.h"

/*!
    \class MessageId
    \ingroup mobility-messaging

    \preliminary
    \brief The MessageId class provides a unique identifier for message entities stored by a MessageStore.

    A MessageId can be constructed from a string, or converted to a string with toString().

    A MessageId instance can be tested for validity with isValid(), and compared to other instances
    for equality.
    
    \sa MessageStore, Message, MessageContent
*/

/*!
    \fn MessageId::MessageId()

    Creates an invalid identifier, toString() will return an empty string;
*/
MessageId::MessageId()
{
}

/*!
    \fn MessageId::MessageId(const MessageId& other)

    Constructs a copy of \a other.
*/
MessageId::MessageId()
{
}

/*!
    \fn MessageId::MessageId(const QString& id)

    Constructs an identifier from \a id.
    
    \sa toString()
*/
MessageId::MessageId(const QString& id)
{
}

/*!
    \fn bool MessageId::operator==(const MessageId &other) const

    Returns true if this identifier and the \a other identifier are identical;
    otherwise returns false.
*/
bool MessageId::operator==(const MessageId& other) const
{
    return false; // stub
}

/*!
    \fn bool MessageId::operator=(const MessageId &other) const

    Assigns \a other to this identifier and returns a reference to this identifier.
*/
MessageId& operator=(const MessageId& other)
{
    return *this; // stub
}

/*!
    \fn bool MessageId::toString() const

    Returns the string representation of this identifier.
    
    An empty string should be returned if and only if the identifier is invalid.
*/
QString MessageId::toString() const
{
    return QString::null; // stub
}

/*!
    \fn bool MessageId::isValid() const

    Returns true if this identifier is valid; otherwise returns false.
*/
bool MessageId::isValid() const
{
    return false; // stub
}

/*! \typedef MessageIdList

    Qt-style synonym for QList<MessageId>
*/

/*!
    \class MessageContent
    \preliminary

    \brief The MessageContent class provides an interface for working 
    with message attachments and the body of a message.

    \ingroup mobility-messaging
    
    MessageContent provides an interface for accessing message content, including
    content that has only been partially retrieved.
    
    For textual content using a recognized encoding decodedTextContent() will return the content 
    as a unicode string.
    
    decodedContent() will return the content after decoding any transfer encoding used
    to represent binary data using 7-bit ASCII characters, such as quoted-printable and base64.
    
    For attachments fileName() will return the suggested filename for the attachment, when defined.

    Where an internet media type of the content is defined contentType() will return the type, and 
    contentSubType() the subtype.
    
    An indication of the size of the message on the source is given by indicativeSize(). If the
    content is available contentAvailable() will return true.

    decodedContentFileName() will return the name of a local file containing the content when available.

    \sa Message, MessageId
*/

/*!
    Constructs an empty message content object.
*/
MessageContent::MessageContent()
{
}

/*!
    Constructs a message content object from data stored in the message store with MessageId \a id.
*/
MessageContent::MessageContent(const MessageId& id)
{
}

/*!
    Destroys the message content object.
*/
MessageContent::~MessageContent();

/*!
    Returns true if all the content is available on the device; otherwise returns false.
*/
bool MessageContent::contentAvailable() const
{
}

/*!
    Returns an indication of the size of the message.
*/
uint MessageContent::indicativeSize() const
{
}

/*!
    Returns the internet media type of the content, when defined;
    otherwise an empty string is returned.
*/
QString MessageContent::contentType() const
{
}

/*!
    Returns the internet media subtype of the content, when defined;
    otherwise an empty string is returned.
*/
QString MessageContent::contentSubType() const
{
}

/*!
    For an attachment return the suggested filename of the attachment, when defined;
    otherwise an empty string is returned.
*/
QString MessageContent::fileName() const
{
}

/*!
    For textual content using a recognized encoding decodedTextContent() will return 
    the content as a unicode string; otherwise an empty string is returned.
*/
QString MessageContent::decodedTextContent() const
{
}

/*!
  Return the content after decoding any transfer encoding used to represent binary data 
  using 7-bit ASCII characters, such as quoted-printable and base64.
  
  For textual content any text encoding such as Shift-JIS, ISO 2022-JP, KOI8-R, 
  Windows-1251 etc will not be decoded.
*/
QByteArray MessageContent::decodedContent() const
{
}

/*!
  Returns the name of a local file containing the content, when available.
*/
QString MessageContent::decodedContentFileName() const
{
}

/*!
  Writes the content as would be returned by decodedContent() into the stream \a out
*/
void MessageContent::serialize(QDataStream& out) const
{
}

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

    virtual MessageId body() const;
    virtual void setBody(const QString &b);

    virtual MessageIdList attachments() const;
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
