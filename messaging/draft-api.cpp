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
    
    If the message a QMessageId identifies is removed from the message store then the identifier will not be 
    reused.
    
    The QMessageId implementation should be small, ideally less than or equal to 16 bytes.
    
    \sa QMessageStore, QMessage, QMessageContent
*/

/*!
    \fn QMessageId::QMessageId()

    Creates an invalid identifier, toString() will return an empty string.
*/
QMessageId::QMessageId()
{
}

/*!
    \fn QMessageId::QMessageId(const QMessageId& other)

    Constructs a copy of \a other.
*/
QMessageId::QMessageId(const QMessageId& other)
{
    Q_UNUSED(other)
}

/*!
    \fn QMessageId::QMessageId(const QString& id)

    Constructs an identifier from \a id.
    
    \sa toString()
*/
QMessageId::QMessageId(const QString& id)
{
    Q_UNUSED(id)
}

/*!
    \fn bool QMessageId::operator==(const QMessageId &other) const

    Returns true if this identifier and the \a other identifier are identical;
    otherwise returns false.
*/
bool QMessageId::operator==(const QMessageId& other) const
{
    Q_UNUSED(other)
    return false; // stub
}

/*!
    \fn QMessageId& QMessageId::operator=(const QMessageId &other)

    Assigns \a other to this identifier and returns a reference to this identifier.
*/
QMessageId& QMessageId::operator=(const QMessageId& other)
{
    Q_UNUSED(other)
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
    
    If the object a QMessageContentId identifies is removed then the identifier will not be reused.
    
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
QMessageContentId::QMessageContentId(const QMessageContentId& other)
{
    Q_UNUSED(other)
}

/*!
    \fn QMessageContentId::QMessageContentId(const QString& id)

    Constructs an identifier from \a id.
    
    \sa toString()
*/
QMessageContentId::QMessageContentId(const QString& id)
{
    Q_UNUSED(id)
}

/*!
    \fn bool QMessageContentId::operator==(const QMessageContentId &other) const

    Returns true if this identifier and the \a other identifier are identical;
    otherwise returns false.
*/
bool QMessageContentId::operator==(const QMessageContentId& other) const
{
    Q_UNUSED(other)
    return false; // stub
}

/*!
    \fn QMessageContentId& QMessageContentId::operator=(const QMessageContentId &other)

    Assigns \a other to this identifier and returns a reference to this identifier.
*/
QMessageContentId& QMessageContentId::operator=(const QMessageContentId& other)
{
    Q_UNUSED(other)
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
    message content, including content that has only been partially retrieved.

    \ingroup messaging
    
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
    content is completely available on the device contentAvailable() will return true.

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
    Q_UNUSED(id)
}

/*!
    Destroys the message content object.
*/
QMessageContent::~QMessageContent()
{
}

/*!
      Returns the identifier of the message content object.
*/
QMessageContentId QMessageContent::id() const
{
    return QMessageContentId();
}

/*!
      Sets the identifier of the message content object to \a id.
*/
void QMessageContent::setId(const QMessageContentId &id)
{
    Q_UNUSED(id)
}

/*!
    Returns true if all the content is available on the device; otherwise returns false.
*/
bool QMessageContent::contentAvailable() const
{
    return false;
}

/*!
    Returns an indication of the size of the message.
*/
uint QMessageContent::indicativeSize() const
{
    return 0;
}

/*!
    Returns the internet media type of the content, when defined;
    otherwise an empty string is returned.
*/
QString QMessageContent::contentType() const
{
    return QString(); // stub
}

/*!
    Returns the internet media subtype of the content, when defined;
    otherwise an empty string is returned.
*/
QString QMessageContent::contentSubType() const
{
    return QString(); // stub
}

/*!
    For an attachment return the suggested filename of the attachment, when defined;
    otherwise an empty string is returned.
*/
QString QMessageContent::fileName() const
{
    return QString(); // stub
}

/*!
    For textual content using a recognized encoding decodedTextContent() will return 
    the content as a unicode string; otherwise an empty string is returned.
*/
QString QMessageContent::decodedTextContent() const
{
    return QString();
}

/*!
  Return the content after decoding any transfer encoding used to represent binary data 
  using 7-bit ASCII characters, such as quoted-printable and base64.
  
  For textual content any text encoding such as Shift-JIS, ISO 2022-JP, KOI8-R, 
  Windows-1251 etc will not be decoded.
*/
QByteArray QMessageContent::decodedContent() const
{
    return QByteArray(); // stub
}

/*!
  Returns the name of a local file containing the content, when available.
*/
QString QMessageContent::decodedContentFileName() const
{
    return QString(); // stub
}

/*!
  Writes the content as would be returned by decodedContent() into the stream \a out.
*/
void QMessageContent::serialize(QDataStream& out) const
{
    Q_UNUSED(out)
}

/*!
    \class QMessage

    \preliminary
    \brief The QMessage class provides a convenient interface for working with messages.
    
    \ingroup messaging
   
    QMessage supports a number of types. Including internet email messages, 
    the telephony types SMS and MMS, and also XMPP messages.
     
    A QMessage can be constructed piece by piece using functions such as 
    setType(), setFrom(), setTo(), setSubject(), setBody() and setAttachments().

    Alternatively a message can be initialized from raw data using fromTransmissionFormat().
    
    Messages can be added to the QMessageStore, or retrieved from the store via their QMessageId 
    identifier.
    
    A message may be serialized to a QDataStream, or returned as a QByteArray using 
    toTransmissionFormat().

    A list of attachments identifiers will be returned by attachments() and a body identifier will
    be returned by body().
    
    The following transmission formats are used both when constructing and encapsulating messages:
    \list
    
    \o QMessage::Sms
    \l{http://www.3gpp.org/ftp/Specs/html-info/23040.htm} 
    {3GPP TS 23.040} (Technical realization of the Short Message Service).
    
    \o QMessage::Mms \l{http://www.openmobilealliance.org/technical/release_program/docs/MMS/V1_3-20050927-C/OMA-TS-MMS-ENC-V1_3-20050927-C.pdf} 
    {OMA TS MMS ENC} (Multimedia Messaging Service Encapsulation Protocol).
    
    \o QMessage::Email 
    \l{http://www.ietf.org/rfc/rfc2822.txt} {RFC 2822} (Internet Message Format), and 
    \l{http://www.ietf.org/rfc/rfc2045.txt} {RFC 2045} (Format of Internet Message Bodies) through 
    \l{http://www.ietf.org/rfc/rfc2049.txt} {RFC 2049} (Conformance Criteria and Examples).

    \o QMessage::Xmpp 
    \l{http://www.ietf.org/rfc/rfc3921.txt} {RFC 3921} (Extensible Messaging and Presence Protocol).

    \endlist
    
    Only phone numbers are valid destination addresses for SMS messages, only email addresses are valid
    destination addresses for Email messages, MMS messages may be addressed to either phone numbers
    or email addresses. Only XMPP addresses are valid destination addresses for XMPP messages.
    
    \sa QMessageContent, QMessageStore, QMessageId
*/

/*!
    \enum QMessage::Type

    Defines the type of a message.
    
    \value Mms      The message is an MMS, Multimedia Messaging Service object.
    \value Sms      The message is an SMS, Short Message Service object.
    \value Email    The message is an Email, Internet Message Format object.
    \value Xmpp     The message is an XMPP, Extensible Messaging and Presence Protocol object.
    \value None     The message type is not defined.
    \value AnyType  Bitflag value that matches Mms, Sms, Email or XMPP. Subject to future redefinition as new types are declared.
*/
    
/*!
    \enum QMessage::Status

    Defines properties of a message.

    \value Read            This flag indicates that the content of this message has been displayed to the user.
    \value HasAttachments  This flag indicates that the message contains at least one sub-part with 'Attachment' disposition.
    \value Incoming        This flag indicates that the message has been sent from an external source.
    \value Removed         This flag indicates that the message has been deleted from or moved on the originating server.
*/

/*!
    \enum QMessage::Priority

    Defines the priority of a message.

    \value High    The message is high priority.
    \value Normal  The message is normal priority.
    \value Low     The message is low priority.
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
    Q_UNUSED(id)
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
QMessage QMessage::fromTransmissionFormat(Type t, const QByteArray &ba)
{
    Q_UNUSED(t)
    Q_UNUSED(ba)
    return QMessage(); // stub
}
    
/*!
    Constructs a message of type \a t from the data contained in \a fileName.

    See the class description for a list of supported message encapsulations.
    
    \sa QMessage, toTransmissionFormat()
*/
QMessage QMessage::fromTransmissionFormatFile(Type t, const QString& fileName)
{
    Q_UNUSED(t)
    Q_UNUSED(fileName)
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
    Q_UNUSED(out)
}

/*!
    Returns the identifier of the message.

    \sa setId(), QMessageFilterKey::id()
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
    Q_UNUSED(id)
}

/*!
    Returns the Type of the message.
    
    \sa setType(), QMessageFilterKey::type()
*/
QMessage::Type QMessage::type() const
{
    return None; // stub
}
    
/*!
    Sets the Type of the message to \a t.
    
    The type of a message may be set for non-empty messages.
    
    \sa type()
*/
void QMessage::setType(Type t)
{
    Q_UNUSED(t)
}

/*!
    Returns the originating address of the message.

    \sa setFrom(), QMessageFilterKey::sender()
*/
QString QMessage::from() const
{
    return QString(); // stub
}

/*!
    Sets the from address, that is the originating address of the message to \a s.

    \sa from()
*/
void QMessage::setFrom(const QString &s)
{
    Q_UNUSED(s)
}

/*!
    Returns the subject of the message, if present; otherwise returns an empty string.

    \sa setSubject(), QMessageFilterKey::subject()
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
    Q_UNUSED(s)
}

/*!
    Returns the timestamp contained in the origination date header field of the message, if present; 
    otherwise returns an empty timestamp.
    
    \sa setDate(), QMessageFilterKey::timeStamp()
*/
QDateTime QMessage::date() const
{
    return date(); // stub
}

/*!
    Sets the origination date header field specifying the timestamp of the message to \a d.
    
    \sa date()
*/
void QMessage::setDate(const QDateTime &d)
{
    Q_UNUSED(d)
}

/*!
    Returns the timestamp placed in the message during reception by the device, if present;
    otherwise returns an empty timestamp.
    
    \sa setReceivedDate(), QMessageFilterKey::receptionTimeStamp()
*/
QDateTime QMessage::receivedDate() const
{
    return QDateTime(); // stub
}

/*!
    Sets the timestamp indicating the time of message reception by the device to \a d.
    
    \sa receivedDate()
*/
void QMessage::setReceivedDate(const QDateTime &d)
{
    Q_UNUSED(d)
}

/*! 
    Returns the list of primary recipients for the message.

    \sa setTo(), QMessageFilterKey::recipients()
*/
QList<QString> QMessage::to() const
{
    return QList<QString>(); // stub
}

/*! 
    Sets the list of primary recipients for the message to \a toList.
    
    \sa to()
*/
void QMessage::setTo(const QList<QString>& toList)
{
    Q_UNUSED(toList)
}

/*! 
    Sets the primary recipient for the message to \a s.
    
    \sa to()
*/
void QMessage::setTo(const QString& s)
{
    Q_UNUSED(s)
}

/*!
    Returns the list of all the cc (carbon copy) recipients specified for the message.

    \sa to(), bcc(), setCc(), QMessageFilterKey::recipients()
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
    Q_UNUSED(ccList)
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
    Set the list of bcc (blind carbon copy) recipients for the message to \a s.

    \sa bcc(), setTo(), setCc()
*/  
void QMessage::setBcc(const QList<QString>& s)
{
    Q_UNUSED(s)
}

/*!
    Returns the status value for the message.

    \sa setStatus(), QMessageFilterKey::status()
*/
quint64 QMessage::status() const
{
    return None; // stub
}

/*!
    Sets the status value for the message to \a newStatus.

    \sa status()
*/
void QMessage::setStatus(quint64 newStatus)
{
    Q_UNUSED(newStatus)
}

/*!
    Returns the priority of the message.

    The default is Normal.

    \sa setPriority(), QMessageFilterKey::priority()
*/
QMessage::Priority QMessage::priority() const
{
    return QMessage::Normal; // stub
}

/*!
    Sets the priority of the message to \a newPriority.

    \sa priority()
*/
void QMessage::setPriority(Priority newPriority)
{
    Q_UNUSED(newPriority)
}

/*!
    Returns the complete size of the message as indicated on the originating server.
    
    \sa setSize(), QMessageFilterKey::size()
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
    Q_UNUSED(size)
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
    
    If \a html is true then the internet media type of the body wil be "plain\\html";
    otherwise it will be "plain\\text".
    
    \sa body(), setBodyFromFile()
*/
void QMessage::setBody(const QString &body, bool html)
{
    Q_UNUSED(body)
    Q_UNUSED(html)
}

/*!
    Sets the body text of the message to be the contents of the file \a fileName.
    
    If \a html is true then the internet media type of the body wil be "plain\\html";
    otherwise it will be "plain\\text".
    
    \sa body(), setBody()
*/
void QMessage::setBodyFromFile(const QString &fileName, bool html)
{
    Q_UNUSED(fileName)
    Q_UNUSED(html)
}

/*!
    Returns a list of attachments for the message.

    The body of the message will not be included in the list.
    
    TODO: Example attachment list generation algorithm.
    
    \sa setAttachments()
*/
QMessageContentIdList QMessage::attachments() const
{
    return QMessageContentIdList(); // stub
}

/*!
    Set the list of attachments for the message to be the contents of the files 
    specified by \a fileNames. The internet media types of each attachment
    will be determined by examining the contents of the file.
    
    \sa attachments()
*/  
void QMessage::setAttachments(const QStringList &fileNames)
{
    Q_UNUSED(fileNames)
}

/*!
    Sets the originating port of the message to \a port.

    Only relevant for SMS messages.
    
    \sa originatorPort(), setDestinationPort()
*/
void QMessage::setOriginatorPort(uint port)
{
    Q_UNUSED(port)
}

/*!
    Returns the originating port of the message.

    Only relevant for SMS messages.

    Default is 0.
    
    \sa setOriginatorPort(), setDestinationPort()
*/
uint QMessage::originatorPort()
{
    return 0; // stub
}

/*!
    Sets the destination port of the message to \a port.

    Only relevant for SMS messages.
    
    \sa setOriginatorPort(), destinationPort()
*/
void QMessage::setDestinationPort(uint port)
{
    Q_UNUSED(port)
}

/*!
    Returns the destination port of the message.

    Only relevant for SMS messages.

    Default is 0.
    
    \sa setDestinationPort(), setOriginatorPort()
*/
uint QMessage::destinationPort()
{
    return 0; // stub
}

/*!
    Returns true if the message has been modified since it was constructed; 
    otherwise returns false.
*/
bool QMessage::dataModified() const
{
    return false; // stub
}

/*!
    \namespace QMessageDataComparator
    \ingroup messaging

    \brief The QMessageDataComparator namespace contains types used in specifying the comparison
    of MessageStore objects with user-defined values.
*/

/*!
    \enum QMessageDataComparator::EqualityComparator

    Defines the comparison operations that can be used to compare data elements 
    of QMessageStore objects for equality and inequality.

    \value Equal Represents the '==' operator.
    \value NotEqual Represents the '!=' operator.
*/

/*!
    \enum QMessageDataComparator::InclusionComparator

    Defines the comparison operations that can be used to compare data elements 
    of QMessageStore objects for inclusion or exclusion.

    \value Includes Represents an operation in which an associated property is tested to 
                    determine whether it includes a value.
    \value Excludes Represents an operation in which an associated property is tested to 
                    determine whether it is equal to the supplied value.
*/

/*!
    \enum QMessageDataComparator::RelationComparator

    Defines the comparison operations that can be used to compare data elements 
    of QMessageStore objects, according to a specific relation.

    \value LessThan Represents the '<' operator.
    \value LessThanEqual Represents the '<=' operator.
    \value GreaterThan Represents the '>' operator.
    \value GreaterThanEqual Represents the '>= operator'.
*/

/*!
    \class QMessageFilterKey

    \preliminary
    \brief The QMessageFilterKey class defines the parameters used for querying a subset of
    all available messages from the message store.
    \ingroup messaging

    A QMessageFilterKey is composed of a message property, an optional comparison operator
    and a comparison value. The QMessageFilterKey class is used in conjunction with the 
    QMessageStore::queryMessages() and QMessageStore::countMessages() functions to filter results 
    which meet the criteria defined by the key.

    QMessageFilterKeys can be combined using the logical operators (&), (|) and (~) to
    create more refined queries.

    For example:

    To create a query for all messages sent from "joe@user.com" with subject "meeting":
    \code
    QMessageFilterKey subjectKey(QMessageFilterKey::subject("meeting"));
    QMessageFilterKey senderKey(QMessageFilterKey::sender("joe@user.com"));
    QMessageIdList results = QMessageStore::instance()->queryMessages(subjectKey & senderKey);
    \endcode

    \sa QMessageStore, QMessage
*/

/*!
    Creates a QMessageFilterKey without specifying matching parameters.

    A default-constructed key (one for which isEmpty() returns true) matches all messages. 

    \sa isEmpty()
*/
QMessageFilterKey::QMessageFilterKey()
{
}

/*!
    Returns true if the key remains empty after default construction; otherwise returns false. 

    An empty key matches all messages.

    The result of combining an empty key with a non-empty key is the original non-empty key. 
    This is true regardless of whether the combination is formed by an AND or an OR operation.

    The result of combining two empty keys is an empty key.
*/
bool QMessageFilterKey::isEmpty() const
{
    return false; // stub
}

/*!
    Returns a key that is the logical NOT of the value of this key.

    If this key is empty, the result will be a non-matching key; if this key is 
    non-matching, the result will be an empty key.

    \sa isEmpty()
*/
QMessageFilterKey QMessageFilterKey::operator~() const
{
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key that is the logical AND of this key and the value of key \a other.
*/
QMessageFilterKey QMessageFilterKey::operator&(const QMessageFilterKey& other) const
{
    Q_UNUSED(other)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key that is the logical OR of this key and the value of key \a other.
*/
QMessageFilterKey QMessageFilterKey::operator|(const QMessageFilterKey& other) const
{
    Q_UNUSED(other)
    return QMessageFilterKey(); // stub
}

/*!
    Performs a logical AND with this key and the key \a other and assigns the result
    to this key.
*/
const QMessageFilterKey& QMessageFilterKey::operator&=(const QMessageFilterKey& other)
{
    Q_UNUSED(other)
    return *this;
}

/*!
    Performs a logical OR with this key and the key \a other and assigns the result
    to this key.
*/
const QMessageFilterKey& QMessageFilterKey::operator|=(const QMessageFilterKey& other)
{
    Q_UNUSED(other)
    return *this;
}

/*!
    Returns \c true if the value of this key is the same as the key \a other. Returns 
    \c false otherwise.
*/
bool QMessageFilterKey::operator==(const QMessageFilterKey& other) const
{
    Q_UNUSED(other)
    return false; // stub
}

/*!
    Assign the value of the QMessageFilterKey \a other to this.
*/
const QMessageFilterKey& QMessageFilterKey::operator=(const QMessageFilterKey& other)
{
    Q_UNUSED(other)
    return *this; // stub
}

/*!
    Returns a key matching messages whose identifier matches \a id, according to \a cmp.

    \sa QMessage::id()
*/
QMessageFilterKey QMessageFilterKey::id(const QMessageId &id, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(id)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose identifier is a member of \a ids, according to \a cmp.

    \sa QMessage::id()
*/
QMessageFilterKey QMessageFilterKey::id(const QMessageIdList &ids, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(ids)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose identifier is a member of the set yielded by \a key, according to \a cmp.

    \sa QMessage::id()
*/
QMessageFilterKey QMessageFilterKey::id(const QMessageFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(key)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose type matches \a type, according to \a cmp.

    \sa QMessage::type()
*/
QMessageFilterKey QMessageFilterKey::type(QMessage::Type type, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(type)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching accounts whose type is a bitwise match to \a type, according to \a cmp.

    \sa QMessage::type()
*/
QMessageFilterKey QMessageFilterKey::type(quint64 type, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(type)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose sender matches \a value, according to \a cmp.

    \sa QMessage::from()
*/
QMessageFilterKey QMessageFilterKey::sender(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose sender matches the substring \a value, according to \a cmp.

    \sa QMessage::from()
*/
QMessageFilterKey QMessageFilterKey::sender(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose recipients include \a value, according to \a cmp.

    \sa QMessage::to(), QMessage::cc(), QMessage::bcc()
*/
QMessageFilterKey QMessageFilterKey::recipients(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose recipients include the substring \a value, 
    according to \a cmp.

    \sa QMessage::to(), QMessage::cc(), QMessage::bcc()
*/
QMessageFilterKey QMessageFilterKey::recipients(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose subject matches \a value, according 
    to \a cmp.

    \sa QMessage::subject()
*/
QMessageFilterKey QMessageFilterKey::subject(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose subject matches the 
    substring \a value, according to \a cmp.

    \sa QMessage::subject()
*/
QMessageFilterKey QMessageFilterKey::subject(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose body is textual and matches \a value, according 
    to \a cmp.

    \sa QMessage::body()
*/
QMessageFilterKey QMessageFilterKey::body(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose body is textual and matches the 
    substring \a value, according to \a cmp.

    \sa QMessage::body()
*/
QMessageFilterKey QMessageFilterKey::body(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose timestamp matches \a value, according to \a cmp.

    \sa QMessage::date()
*/
QMessageFilterKey QMessageFilterKey::timeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose timestamp has the relation to \a value that is specified by \a cmp.

    \sa QMessage::date()
*/
QMessageFilterKey QMessageFilterKey::timeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose reception timestamp matches \a value, according to \a cmp.

    \sa QMessage::receivedDate()
*/
QMessageFilterKey QMessageFilterKey::receptionTimeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose reception timestamp has the relation to \a value that is specified by \a cmp.

    \sa QMessage::receivedDate()
*/
QMessageFilterKey QMessageFilterKey::receptionTimeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose status matches \a value, according to \a cmp.

    \sa QMessage::status()
*/
QMessageFilterKey QMessageFilterKey::status(quint64 value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose status is a bitwise match to \a mask, according to \a cmp.

    \sa QMessage::status()
*/
QMessageFilterKey QMessageFilterKey::status(quint64 mask, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(mask)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose priority matches \a value, according to \a cmp.

    \sa QMessage::priority()
*/
QMessageFilterKey QMessageFilterKey::priority(QMessage::Priority value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose size matches \a value, according to \a cmp.

    \sa QMessage::size()
*/
QMessageFilterKey QMessageFilterKey::size(int value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose size matches \a value, according to \a cmp.

    \sa QMessage::size()
*/
QMessageFilterKey QMessageFilterKey::size(int value, QMessageDataComparator::RelationComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    \class QMessageSortKey

    \preliminary
    \brief The QMessageSortKey class defines the parameters used for sorting a subset of 
    queried messages from the message store.
    \ingroup messaging

    A QMessageSortKey is composed of a message property to sort and a sort order. 
    The QMessageSortKey class is used in conjunction with the QMessageStore::queryMessages() 
    function to sort message results according to the criteria defined by the sort key.

    For example:
    To create a query for all messages sorted by their timestamp in decending order:
    \code
    QMessageSortKey sortKey(QMessageSortKey::timeStamp(Qt::DescendingOrder));
    QMessageIdList results = QMessageStore::instance()->queryMessages(QMessageFilterKey(), sortKey);
    \endcode
    
    \sa QMessageStore, QMessageFilterKey
*/

/*!
    Create a QMessageSortKey with specifying matching parameters.

    A default-constructed key (one for which isEmpty() returns true) sorts no messages. 

    The result of combining an empty key with a non-empty key is the same as the original 
    non-empty key.

    The result of combining two empty keys is an empty key.
*/
QMessageSortKey::QMessageSortKey()
{
}

/*!
    Returns true if the key remains empty after default construction; otherwise returns false.
*/
bool QMessageSortKey::isEmpty() const
{
    return false; // stub
}

/*!
    Returns a key that which is the result of concatenating the vaue of this key and the value 
    of key \a other.
*/
QMessageSortKey QMessageSortKey::operator+(const QMessageSortKey& other) const
{
    Q_UNUSED(other)
    return QMessageSortKey(); // stub
}

/*!
    Appends the value of the key \a other with the value of this key and assigns the result
    to this key.
*/
QMessageSortKey& QMessageSortKey::operator+=(const QMessageSortKey& other)
{
    Q_UNUSED(other)
    return *this; // stub
}

/*!
    Returns \c true if the value of this key is the same as the key \a other. Returns 
    \c false otherwise.
*/
bool QMessageSortKey::operator==(const QMessageSortKey& other) const
{
    Q_UNUSED(other)
    return false; // stub
}

/*!
    Assign the value of the QMessageSortKey \a other to this.
*/
const QMessageSortKey& QMessageSortKey::operator=(const QMessageSortKey& other)
{
    Q_UNUSED(other)
    return *this; // stub
}
    
/*!
    Returns a key that sorts messages by their identifiers, according to \a order.

    \sa QMessage::id()
*/
QMessageSortKey QMessageSortKey::id(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageSortKey(); // stub
}

/*!
    Returns a key that sorts messages by their message type, according to \a order.

    \sa QMessage::type()
*/
QMessageSortKey QMessageSortKey::type(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageSortKey(); // stub
}

/*!
    Returns a key that sorts messages by the address from which they were sent, according to \a order.

    \sa QMessage::from()
*/
QMessageSortKey QMessageSortKey::sender(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageSortKey(); // stub
}

/*!
    Returns a key that sorts messages by the addresses to which they were sent, according to \a order.

    \sa QMessage::to()
*/
QMessageSortKey QMessageSortKey::recipients(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageSortKey(); // stub
}

/*!
    Returns a key that sorts messages by their subject, according to \a order.

    \sa QMessage::subject()
*/
QMessageSortKey QMessageSortKey::subject(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageSortKey(); // stub
}

/*!
    Returns a key that sorts messages by their origination timestamp, according to \a order.

    \sa QMessage::date()
*/
QMessageSortKey QMessageSortKey::timeStamp(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageSortKey(); // stub
}

/*!
    Returns a key that sorts messages by their reception timestamp, according to \a order.

    \sa QMessage::receivedDate()
*/
QMessageSortKey QMessageSortKey::receptionTimeStamp(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageSortKey(); // stub
}

/*!
    Returns a key that sorts messages by their status values, according to \a order.

    \sa QMessage::status()
*/
QMessageSortKey QMessageSortKey::status(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageSortKey(); // stub
}

/*!
    Returns a key that sorts messages by their priority, according to \a order.

    \sa QMessage::priority()
*/
QMessageSortKey QMessageSortKey::priority(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageSortKey(); // stub
}

/*!
    Returns a key that sorts messages by their size, according to \a order.

    \sa QMessage::size()
*/
QMessageSortKey QMessageSortKey::size(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageSortKey(); // stub
}

/*!
    \class QMessageStore

    \preliminary
    \brief The QMessageStore class represents the main interface for storage and retrieval
    of messages in the message store.

    \ingroup messaging

    The QMessageStore class is accessed through a singleton interface and provides functions 
    for adding messages to the message store, and updating and deleting messages in the message store.

    QMessageStore also provides functions for querying and counting of QMessages
    when used in conjunction with QMessageFilerKey class.

    If a QMessageStore operation fails, the lastError() function will return an error code
    value indicating the failure mode encountered.  A successful operation will set the 
    lastError() result to QMessageStore::NoError.

    Messages in the message store are identified by QMessageId objects. The data associated
    with a message is retrieved in the form of a QMessage object. Message content
    such as the message body and message parts are identified by QMessageContentId objects.
    The data associated with message content is retrieved in the form of a QMessageContent
    object.

    Message objects are accessed via the message() and queryMessages() functions. Messages 
    can be inserted into the store using the addMessage() function, and messages in the 
    store can be manipulated via the updateMessage() function and removed by the 
    removeMessage() functions.
    
    Message store manipulations involving messages are reported via the messagesAdded(), 
    messagesUpdated() and messagesRemoved() signals.

    \sa QMessage, QMessageId, QMessageContentId, QMessageFilterKey, QMessageSortKey
*/

/*!
    \enum QMessageStore::RemovalOption

    Defines whether or not a message will be removed from the originating server.

    \value NoRemovalRecord     Do not remove the message from the originating server.
    \value CreateRemovalRecord Remove the message from the originating server.
*/

/*
    TODO capabilities NativeBooleanSarch NativeSlices Sms Mms Email Xmpp Presence NativeExentendedSearching NativeBodySearching
      ExtendedServices

    Activate/deactive on only adds, deletes or updates? Requires enum,
    enum NotificationType 
    {
        Add = 1,
        Removed,
        Changed
    }
    constraint(), setConstraint()
*/

/*!
    \enum QMessageStore::ErrorCode

    Defines the result of attempting to perform a message store operation.

    \value NoError              The operation was successfully performed.
    \value InvalidId            The operation failed due to the specification of an invalid identifier.
    \value ConstraintFailure    The operation failed due to a constraint violation.
    \value ContentInaccessible  The operation failed because the content data cannot be accessed by the message store.
    \value NotYetImplemented    The operation failed because the message store does not yet implement the operation.
    \value FrameworkFault       The operation failed because the message store encountered an error in performing the operation.
    \value WorkingIdsOverflow   The operation failed because the message store could not perform the operation within the constraint specified by setMaximumWorkingIds().
*/

/*!
    Returns the code of the last error condition reported by the message store.
*/
QMessageStore::ErrorCode QMessageStore::lastError() const
{
    return NotYetImplemented;
}

/*!
    Returns the \l{QMessageId}s of messages in the message store. If \a key is not empty 
    only messages matching the parameters set by \a key will be returned, otherwise 
    all message identifiers will be returned.
    If \a sortKey is not empty, then the identifiers will be sorted by the parameters 
    set by \a sortKey.
    If \a limit is not zero, then \a limit places an upper bound on the number of 
    ids in the list returned.
    \a offset specifies how many ids to skip at the beginning of the list returned.
    
    \sa lastError(), countMessages(), setMaximumWorkingIds()
*/
QMessageIdList QMessageStore::queryMessages(const QMessageFilterKey &key, const QMessageSortKey &sortKey, uint limit, uint offset) const
{
    Q_UNUSED(key)
    Q_UNUSED(sortKey)
    Q_UNUSED(limit)
    Q_UNUSED(offset)
    return QMessageIdList(); // stub
}

/*!
    Returns the count of the number of messages which match the 
    filtering criteria defined in QMessageFilterKey \a key. If 
    key is empty a count of all messages is returned.
    
    \sa lastError(), queryMessages(), setMaximumWorkingIds()
*/
int QMessageStore::countMessages(const QMessageFilterKey& key) const
{
    Q_UNUSED(key)
    return 0; // stub
}

/*!
    Removes the message with QMessageId \a id from the message store. If \a option is 
    QMessageStore::CreateRemovalRecord then a removal record will be created for the
    removed message.
    Returns \c true if the operation completed successfully, \c false otherwise. 
*/
bool QMessageStore::removeMessage(const QMessageId& id, RemovalOption option)
{
    Q_UNUSED(id)
    Q_UNUSED(option)
    return false; // stub
}

/*!
    Removes all messages identified by the key \a key from the message store.
    If \a option is QMessageStore::CreateRemovalRecord then removal records will be 
    created for each removed message.
    Returns \c true if the operation completed successfully, \c false otherwise. 

    For example:

    To implement a function to remove a list messages identified by QMessageIds
    from the message store.
    \code
    bool removeMessages(const QMessageIdList& ids)
    {
        QMessageFilterKey idsFilter(ids);
        return QMessageStore::instance()->removeMessages(idsFilter);
    }
    \endcode
*/
bool QMessageStore::removeMessages(const QMessageFilterKey& key, QMessageStore::RemovalOption option)
{
    Q_UNUSED(key)
    Q_UNUSED(option)
    return true; // stub
}

/*!
    Updates the existing QMessage \a m on the message store.
    Returns \c true if the operation completed successfully, or \c false otherwise. 
*/
bool QMessageStore::updateMessage(QMessage *m)
{
    Q_UNUSED(m)
    return true; // stub
}

/*!
   Returns the QMessage defined by the QMessageContentId \a id from the store.
*/
QMessage QMessageStore::message(const QMessageId& id) const
{
    Q_UNUSED(id)
    return QMessage(); // stub
}

/*!
   Returns the QMessageContent defined by the QMessageContentId \a id from the store.
*/
QMessageContent QMessageStore::messageContent(const QMessageContentId& id) const
{
    Q_UNUSED(id)
    return QMessageContent(); // stub
}

/*!
   If \a maximumIds is 0, removes any constraint on the maximum number of ids
   that can be kept in the working list when evaluating countMessages() 
   and queryMessages().
   
   Otherwise sets the maximum number of ids than can be kept in the working
   list when evaluating countMessages() and queryMessages(). A failure to 
   satisfy the working list size constraint is reported by lastError()
   returning WorkingIdsOverflow.
   
   The working list is used only when an atomic evaluation of a QMessageFilterKey
   based query is not supported.
   
   \sa maximumWorkingIds(), ErrorCode, countMessages(), queryMessages(), lastError()
*/
void QMessageStore::setMaximumWorkingIds(uint maximumIds)
{
    Q_UNUSED(maximumIds)
}

/*!
   Returns 0 if no constraint has been set by setMaximumWorkingIds().
   
   Otherwise returns the maximum working list size as set by setMaximumWorkingIds().
   
   The default working list size is platform specific.
   
   \sa setMaximumWorkingIds(), ErrorCode, countMessages(), queryMessages()
*/
uint QMessageStore::maximumWorkingIds()
{
    return 0; // stub
}

/*!
    Returns the single instance of the QMessageStore class.

    If necessary, the store will be instantiated and initialized.
*/
QMessageStore* QMessageStore::instance()
{
    return 0;
}
    
/*!
    \fn void QMessageStore::messagesAdded(const QMessageIdList& ids)

    Signal that is emitted when the messages identified by the list \a ids are
    added to the message store.

    \sa messagesRemoved(), messagesUpdated(), startNotifications()
*/

/*!
    \fn void QMessageStore::messagesRemoved(const QMessageIdList& ids)

    Signal that is emitted when the messages in the list \a ids are
    removed from the message store.

    \sa messagesAdded(), messagesUpdated(), startNotifications()
*/

/*!
    \fn void QMessageStore::messagesUpdated(const QMessageIdList& ids)

    Signal that is emitted when the messages in the list \a ids are
    updated within the message store.

    \sa messagesAdded(), messagesRemoved(), startNotifications()
*/

/*!
    Starts emission of messagesAdded(), messagesRemoved() and messagesUpdated() signals
    for messages that match \a key.
    
    \sa messagesAdded(), messagesRemoved(), messagesUpdated(), stopNotifications()
*/
void QMessageStore::startNotifications(const QMessageFilterKey &key)
{
    Q_UNUSED(key)    
}

/*!
    Stops emission of messagesAdded(), messagesRemoved() and messagesUpdated() signals
    for messages that match \a key.
    
    \sa messagesAdded(), messagesRemoved(), messagesUpdated(), startNotifications()
*/
void QMessageStore::stopNotifications(const QMessageFilterKey &key)
{
    Q_UNUSED(key)    
}

/*!
    \class QMessageServiceAction

    \preliminary
    \ingroup messaging

    \brief The QMessageServiceAction class provides the interface for requesting 
    external message services.

    QMessageServiceAction provides the mechanisms for messaging clients to request actions from 
    external services, and to receive information in response.  All actions present the same 
    interface for communicating status, connectivity and progress information.

    All actions communicate changes in their operational state by emitting the activityChanged()
    signal.
    
    A user may attempt to cancel an operation after it has been initiated.  The cancelOperation()
    slot is provided for this purpose.

    A QMessageServiceAction instance supports only a single request at any time.  A client
    may, however, use multiple QMessageServiceAction instances to send independent requests concurrently.
    Each QMessageServiceAction instance will report only the changes pertaining to the request
    that instance delivered.
*/

/*!
    \enum QMessageServiceAction::Activity

    This enum type is used to describe the activity state of the requested action.

    \value Pending          The action has not yet begun execution.
    \value InProgress       The action is currently executing.
    \value Successful       The action has completed successfully.
    \value Failed           The action could not be completed successfully, and has finished execution.
*/

/*!
  Transmit \a message using the default account for the type of \a message.
  
  \sa QMessage::type()
*/
void QMessageServiceAction::send(const QMessage &message)
{
    Q_UNUSED(message)
}

/*!
  Open a composer window using \a message as a prototype.

  The default application for handling the type \a message should be used.
  
  \sa QMessage::type()
*/
void QMessageServiceAction::compose(const QMessage &message)
{
    Q_UNUSED(message)
}

/*!
  Open a composer window replying to the message identified by \a id.

  The default application for handling the type of message that \a id identifies should be used.
  
  \sa QMessageId, QMessage::type()
*/
void QMessageServiceAction::reply(const QMessageId &id)
{
    Q_UNUSED(id)
}

/*!
  Open a composer window forwarding the message identified by \a id.
  
  The default application for handling the type of message that \a id identifies should be used.

  \sa QMessageId, QMessage::type()
*/
void QMessageServiceAction::forward(const QMessageId &id)
{
    Q_UNUSED(id)
}

/*!
    Requests that the message server retrieve data regarding the message identified by \a id.  

    The meta data (including flags, from, to, subject, and date fields where applicable) of 
    the message identified by \a id should be retrieved.
    
    If the message can not be found on the originating server it will be marked as removed.

    \sa QMessageId, QMessage::Removed

*/
void QMessageServiceAction::retrieve(const QMessageId& id)
{
    Q_UNUSED(id)
}

/*!
    Requests that the message server retrieve data regarding the message identified by \a id.  

    The entirety of the message content identified by \a id should be retrieved.

    \sa QMessageContentId
*/
void QMessageServiceAction::retrieve(const QMessageContentId& id)
{
    Q_UNUSED(id)
}

/*!
    Show the message identified by \a id.

    The default application for handling the type of message that \a id identifies should be used.

    \sa QMessageId, QMessage::type()
*/
void QMessageServiceAction::show(const QMessageId& id)
{
    Q_UNUSED(id)
}
    
/*!
    Returns the current activity state of the action.

    \sa activityChanged()
*/
QMessageServiceAction::Activity QMessageServiceAction::activity() const
{
    return Pending; // stub
}

/*!
    Attempts to cancel the last requested operation.
*/
void QMessageServiceAction::cancelOperation()
{
}

/*!
    \fn QMessageServiceAction::activityChanged(QMessageServiceAction::Activity a)

    This signal is emitted when the activity status of the action changes,
    with the new state described by \a a.

    \sa activity()
*/
void QMessageServiceAction::activityChanged(QMessageServiceAction::Activity a)
{
    Q_UNUSED(a)
}
/* 
   TODO which methods are essentially private
   QMessage::setId QMessage:setSize, ?, others??
   use friends for these.
*/

//#include "draft-api.moc"
