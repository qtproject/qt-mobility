/* License goes here */
#include "draft-api.h"

/*!
    \class QMessageId
    \ingroup messaging

    \preliminary
    \brief The QMessageId class provides a unique identifier for message entities stored by a QMessageStore.

    A QMessageId can be constructed from a string, or converted to a string with toString().

    A QMessageId instance can be tested for validity with isValid(), and compared to other instances
    for equality.
    
    \sa QMessageStore, QMessage, QMessageContent
*/

/*!
    \fn QMessageId::QMessageId()

    Creates an invalid identifier, toString() will return an empty string;
*/
QMessageId::QMessageId()
{
}

/*!
    \fn QMessageId::QMessageId(const QMessageId& other)

    Constructs a copy of \a other.
*/
QMessageId::QMessageId()
{
}

/*!
    \fn QMessageId::QMessageId(const QString& id)

    Constructs an identifier from \a id.
    
    \sa toString()
*/
QMessageId::QMessageId(const QString& id)
{
}

/*!
    \fn bool QMessageId::operator==(const QMessageId &other) const

    Returns true if this identifier and the \a other identifier are identical;
    otherwise returns false.
*/
bool QMessageId::operator==(const QMessageId& other) const
{
    return false; // stub
}

/*!
    \fn bool QMessageId::operator=(const QMessageId &other) const

    Assigns \a other to this identifier and returns a reference to this identifier.
*/
QMessageId& operator=(const QMessageId& other)
{
    return *this; // stub
}

/*!
    \fn bool QMessageId::toString() const

    Returns the string representation of this identifier.
    
    An empty string should be returned if and only if the identifier is invalid.
*/
QString QMessageId::toString() const
{
    return QString::null; // stub
}

/*!
    \fn bool QMessageId::isValid() const

    Returns true if this identifier is valid; otherwise returns false.
*/
bool QMessageId::isValid() const
{
    return false; // stub
}

/*! \typedef QMessageIdList

    Qt-style synonym for QList<QMessageId>
*/

/*!
    \class QMessageContentId
    \ingroup messaging

    \preliminary
    \brief The QMessageContentId class provides a unique identifier for message entities stored by a QMessageStore.

    A QMessageContentId can be constructed from a string, or converted to a string with toString().

    A QMessageContentId instance can be tested for validity with isValid(), and compared to other instances
    for equality.
    
    \sa QMessageStore, QMessage, QMessageContent
*/

/*!
    \fn QMessageContentId::QMessageContentId()

    Creates an invalid identifier, toString() will return an empty string;
*/
QMessageContentId::QMessageContentId()
{
}

/*!
    \fn QMessageContentId::QMessageContentId(const QMessageContentId& other)

    Constructs a copy of \a other.
*/
QMessageContentId::QMessageContentId()
{
}

/*!
    \fn QMessageContentId::QMessageContentId(const QString& id)

    Constructs an identifier from \a id.
    
    \sa toString()
*/
QMessageContentId::QMessageContentId(const QString& id)
{
}

/*!
    \fn bool QMessageContentId::operator==(const QMessageContentId &other) const

    Returns true if this identifier and the \a other identifier are identical;
    otherwise returns false.
*/
bool QMessageContentId::operator==(const QMessageContentId& other) const
{
    return false; // stub
}

/*!
    \fn bool QMessageContentId::operator=(const QMessageContentId &other) const

    Assigns \a other to this identifier and returns a reference to this identifier.
*/
QMessageContentId& operator=(const QMessageContentId& other)
{
    return *this; // stub
}

/*!
    \fn bool QMessageContentId::toString() const

    Returns the string representation of this identifier.
    
    An empty string should be returned if and only if the identifier is invalid.
*/
QString QMessageContentId::toString() const
{
    return QString::null; // stub
}

/*!
    \fn bool QMessageContentId::isValid() const

    Returns true if this identifier is valid; otherwise returns false.
*/
bool QMessageContentId::isValid() const
{
    return false; // stub
}

/*! \typedef QMessageContentIdList

    Qt-style synonym for QList<QMessageContentId>
*/

/*!
    \class QMessageContent
    \preliminary

    \brief The QMessageContent class provides an interface for accessing
    message content.

    \ingroup messaging
    
    QMessageContent provides an interface for accessing message content, including
    content that has only been partially retrieved.

    QMessageContent objects can be retrieved from the QMessageStore via their QMessageContentId 
    identifier.
    
    For textual content using a recognized encoding decodedTextContent() will return the content 
    as a unicode string.
    
    decodedContent() will return the content after decoding any transfer encoding used
    to represent binary data using 7-bit ASCII characters, such as quoted-printable and base64.
    
    For attachments fileName() will return the suggested filename for the attachment, when defined.

    When an internet media type (mime type) for the content is defined contentType() will return 
    the type, and contentSubType() the subtype.
    
    An indication of the size of the message on the originating server is given by indicativeSize(). If the
    content is available on the device contentAvailable() will return true.

    decodedContentFileName() will return the name of a local file containing the content, when available.

    \sa QMessage, QMessageContentId
*/

/*!
    Constructs an empty message content object.
*/
QMessageContent::QMessageContent()
{
}

/*!
    Constructs a message content object from data stored in the message store with QMessageContentId \a id.
*/
QMessageContent::QMessageContent(const QMessageContentId& id)
{
}

/*!
    Destroys the message content object.
*/
QMessageContent::~QMessageContent();

/*!
      Returns the identifier of the message content object.
*/
QMessageContentId QMessageContent::QMessageContentId id() const
{
    return QMessageContentId();
}

/*!
      Sets the identifier of the message content object to \a id.
*/
void QMessageContent::setId(const QMessageContentId &id)
{
}

/*!
    Returns true if all the content is available on the device; otherwise returns false.
*/
bool QMessageContent::contentAvailable() const
{
}

/*!
    Returns an indication of the size of the message.
*/
uint QMessageContent::indicativeSize() const
{
}

/*!
    Returns the internet media type of the content, when defined;
    otherwise an empty string is returned.
*/
QString QMessageContent::contentType() const
{
}

/*!
    Returns the internet media subtype of the content, when defined;
    otherwise an empty string is returned.
*/
QString QMessageContent::contentSubType() const
{
}

/*!
    For an attachment return the suggested filename of the attachment, when defined;
    otherwise an empty string is returned.
*/
QString QMessageContent::fileName() const
{
}

/*!
    For textual content using a recognized encoding decodedTextContent() will return 
    the content as a unicode string; otherwise an empty string is returned.
*/
QString QMessageContent::decodedTextContent() const
{
}

/*!
  Return the content after decoding any transfer encoding used to represent binary data 
  using 7-bit ASCII characters, such as quoted-printable and base64.
  
  For textual content any text encoding such as Shift-JIS, ISO 2022-JP, KOI8-R, 
  Windows-1251 etc will not be decoded.
*/
QByteArray QMessageContent::decodedContent() const
{
}

/*!
  Returns the name of a local file containing the content, when available.
*/
QString QMessageContent::decodedContentFileName() const
{
}

/*!
  Writes the content as would be returned by decodedContent() into the stream \a out
*/
void QMessageContent::serialize(QDataStream& out) const
{
}

/*!
    \class QMessage

    \preliminary
    \brief The QMessage class provides a convenient interface for working with messages.
    
    \ingroup messaging
   
    QMessage supports a number of types. These include telephony types 
    such as SMS and MMS, and internet email messages.
    
    A QMessage can be constructed piece by piece using functions such as 
    setMessageType(), setFrom(), setTo(), setSubject(), setBody() and setAttachments().

    Alternatively a message can be initialized from raw data using fromTransmissionFormat().
    
    Messages can be added to the QMessageStore, or retrieved from the store via their QMessageId 
    identifier.
    
    A message may be serialized to a QDataStream, or returned as a QByteArray using 
    toTransmissionFormat().

    A list of attachments identifiers will be return by attachments() and a body identifier will
    be returned by body().
    
    The following transmission formats are used both when contructing and encapsulating messages:
    QMessage::SMS \l{http://www.3gpp.org/ftp/Specs/html-info/23040.htm} 
    {3GPP TS 23.040} (Technical realization of the Short Message Service), 
    QMessage::MMS \l{http://www.openmobilealliance.org/technical/release_program/docs/MMS/V1_3-20050927-C/OMA-TS-MMS-ENC-V1_3-20050927-C.pdf} 
    {OMA TS MMS ENC} (Multimedia Messaging Service Encapsulation Protocol),
    QMessage::Email 
    \l{http://www.ietf.org/rfc/rfc2822.txt} {RFC 2822} (Internet Message Format), and 
    \l{http://www.ietf.org/rfc/rfc2045.txt} {RFC 2045} (Format of Internet Message Bodies) through 
    \l{http://www.ietf.org/rfc/rfc2049.txt} {RFC 2049} (Conformance Criteria and Examples).
    
    Only phone numbers are valid destination addresses for SMS messages, only email addresses are valid
    destination addresses for Email messages, MMS messages may be addressed to either phone numbers
    or email addresses. Addresses containing an '@' character are considered email addresses, other
    addresses are considered phone numbers or invalid.
    
    \sa QMessageContent, QMessageStore, QMessageId
*/

/* 
   TODO MessageType, MessageStatus enum doc
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
        Read = 0x1
    };
*/

/*!
    Constructs an empty message.
*/
QMessage::QMessage()
{
}

/*!
    Constructs a message from data stored in the message store with identifier \a id.
    
    \sa QMessageStore
*/
QMessage::QMessage(const QMessageId& id)
{
}

/*!
    Destroys the message.
*/
QMessage::~QMessage()
{
}

/*!
    Constructs a message of type \a t from the data contained in \a ba.
    
    See the class description for a list of supported message encapsulations.
    
    \sa QMessage, toTransmissionFormat()
*/
QMessage QMessage::fromTransmissionFormat(MessageType t, const QByteArray &ba)
{
    return QMessage(); // stub
}
    
/*!
    Constructs a message of type \a t from the data contained in \a fileName.

    See the class description for a list of supported message encapsulations.
    
    \sa QMessage, toTransmissionFormat()
*/
QMessage QMessage::fromTransmissionFormatFile(MessageType t, const QString& fileName)
{
    return QMessage(); // stub
}

/*!
    Returns the message in a format suitable for transmission.
    
    See the class description for the encapsulations used for each
    message type.
    
    \sa QMessage, fromTransmissionFormat()
*/
QByteArray QMessage::toTransmissionFormat() const
{
    return QByteArray(); // stub
}

/*!
    Writes the message to the output stream \a out.
    
    See the class description for the encapsulations used for each
    message type.

    \sa QMessage, fromTransmissionFormat()
*/
void QMessage::toTransmissionFormat(QDataStream& out) const
{
}

/*!
    Returns the identifier of the message.

    \sa setId()
*/
QMessageId QMessage::id() const
{
    return QMessageId(); // stub
}
    
/*!
    Sets the identifier of the message to \a id.
    
    \sa id()
*/
void QMessage::setId(const QMessageId &id)
{
}

/*!
    Returns the MessageType of the message.
    
    \sa setMessageType()
*/
MessageType QMessage::messageType() const
{
    return None; // stub
}
    
/*!
    Sets the MessageType of the message to \a t.
    
    \sa messageType()
*/
void QMessage::setMessageType(MessageType t)
{
}

/*!
    Returns the originating address of the message.

    \sa setFrom()
*/
QString QMessage::from() const
{
    return QString(); // stub
}

/*!
    Sets the from address, that is the originating address of the message to \a from.

    \sa from()
*/
void QMessage::setFrom(const QString &s)
{
}

/*!
    Returns the subject of the message, if present; otherwise returns an empty string.

    \sa setSubject()
*/
QString QMessage::subject() const
{
    return QString(); //stub
}

/*!
    Sets the subject of the message to \a s.
    
    \sa subject()
*/
void QMessage::setSubject(const QString &s)
{
}

/*!
    Returns the timestamp contained in the origination date header field of the message, if present; 
    otherwise returns an empty timestamp.
    
    \sa setDate()
*/
QDateTime QMessage::date() const
{
    return date(); // stub
}

/*!
    Sets the origination date header field specifying the timestamp of the message to \a timeStamp.
    
    \sa date()
*/
void QMessage::setDate(const QDateTime &s)
{
}

/*!
    Returns the timestamp placed in the message during reception by the device, if present;
    otherwise returns an empty timestamp.
    
    \sa setRecievedDate()
*/
QDateTime QMessage::receivedDate() const
{
    return QDateTime(); // stub
}

/*!
    Sets the timestamp indicating the time of message reception by the device to \a s.
    
    \sa receivedDate()
*/
void QMessage::setReceivedDate(const QDateTime &d)
{
}

/*! 
    Returns the list of primary recipients for the message.

    \sa setTo()
*/
QList<QString> QMessage::to() const
{
    return QList<QString>(); // stub
}

/*! 
    Sets the list of primary recipients for the message to \a s.
    
    \sa to()
*/
void QMessage::setTo(const QList<QString>& toList)
{
}

/*! 
    Sets the primary recipient for the message to \a s.
    
    \sa to()
*/
void QMessage::setTo(const QString& s)
{
}

/*!
    Returns the list of all the cc (carbon copy) recipients specified for the message.

    \sa to(), bcc(), setCc()
*/  
QList<QString> QMessage::cc() const
{
    return QList<QString>(); // stub
}

/*!
    Set the list of cc (carbon copy) recipients for the message to \a ccList.

    \sa cc(), setTo(), setBcc()
*/  
void QMessage::setCc(const QList<QString>& ccList)
{
}

/*!
    Returns the list of all the bcc (blind carbon copy) recipients specified for the message.

    \sa to(), cc(), setBcc()
*/  
QList<QString> QMessage::bcc() const
{
    return QList<QString>(); // stub
}

/*!
    Set the list of bcc (blind carbon copy) recipients for the message to \a bccList.

    \sa bcc(), setTo(), setCc()
*/  
void QMessage::setBcc(const QList<QString>& s)
{
}

/*!
    Returns the status value for the message.

    \sa setStatus()
*/  
MessageStatus QMessage::status() const
{
    return 0; // stub
}

/*!
    Sets the status value for the message to \a newStatus.

    \sa status()
*/
void QMessage::setStatus(MessageStatus newStatus)
{
}

/*!
    Returns the complete size of the message as indicated on the originating server.
    
    \sa setSize()
*/
uint QMessage::size() const
{
    return 0; // stub
}

/*!
    Sets the complete size of the message as found on the originating server to \a size.
    
    \sa size()
*/
void QMessage::setSize(uint size)
{
}

/*!
    Returns the identifier for the body content contained by the Message if a body exists; 
    otherwise returns an invalid identifier.
    
    TODO: Example body finding algorithm.
    
    \sa QMessageContent, setBody()
*/
QMessageContentId QMessage::body() const
{
    return QMessageContentId(); // stub
}

/*!
    Sets the body text of the message to be the string \a body.
    
    If \a html is true then the internet media type of the body wil be "plain\html";
    otherwise it will be "plain\text".
    
    \sa body()
*/
void QMessage::setBody(const QString &body, bool html)
{
}

/*!
    Returns a list of attachments for the message.

    The body of the message will not be included in the list.
    
    TODO: Example attachment list generation algorithm.
    
    \sa setAttachments()
*/
QMessageContentIdList QMessage::attachments() const
{
}

/*!
    Set the list of attachments for the message to be the contents of the files 
    specified by \a fileNames. The internet media types of each attachment
    will be determined by examining the contents of the file.
    
    \sa attachments()
*/  
virtual QMessage::setAttachments(QStringList fileNames)
{
}

/*!
    Returns true if the message has been modified since it was constructed; 
    otherwise returns false.
*/
bool QMessage::dataModified() const
{
}

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

enum PresenceComparator
{
    Present,
    Absent
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

    static QMessageFilterKey id(const QMessageId &id, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey id(const QMessageIdList &ids, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
    static QMessageFilterKey id(const QMessageFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

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
    void showNew(const QMessageId& id);

public slots:
    void cancelOperation();

signals:
    void activityChanged(QMessageServiceAction::Activity a);

private:
    // ...
};
#endif
