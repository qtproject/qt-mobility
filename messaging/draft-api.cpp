/* License goes here */
#include "draft-api.h"

/*!
    \class QMessageId
    \ingroup messaging

    \preliminary
    \brief The QMessageId class provides a unique identifier for a QMessage message.

    A QMessageId can be constructed from a string, or converted to a string with toString().

    A QMessageId instance can be tested for validity with isValid(), and compared to other instances
    for equality.
    
    If the message a QMessageId identifies is removed from the message store then the identifier will not be 
    reused.
    
    The QMessageId implementation should be small, ideally less than or equal to 16 bytes.
    
    \sa QMessageStore, QMessage, QMessageContentContainer
*/

/*!
    \fn QMessageId::QMessageId()

    Creates an invalid identifier, toString() will return a null string.
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
    
    An null string should be returned if and only if the identifier is invalid.
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
    \class QMessageContentContainerId
    \ingroup messaging

    \preliminary
    \brief The QMessageContentContainerId class provides a unique identifier for 
    QMessageContentContainer internet media (MIME) type entities.

    A QMessageContentContainerId can be constructed from a string, or converted to a string 
    with toString().

    The identifier of the containing (parent) message is return by messageId()

    A QMessageContentContainerId instance can be tested for validity with isValid(), and compared 
    to other instances for equality.
    
    If the object a QMessageContentContainerId identifies is removed then the identifier will not 
    be reused.
    
    \sa QMessageStore, QMessage, QMessageContentContainer
*/

/*!
    \fn QMessageContentContainerId::QMessageContentContainerId()

    Creates an invalid identifier, toString() will return a null string;
*/
QMessageContentContainerId::QMessageContentContainerId()
{
}

/*!
    \fn QMessageContentContainerId::QMessageContentContainerId(const QMessageContentContainerId& other)

    Constructs a copy of \a other.
*/
QMessageContentContainerId::QMessageContentContainerId(const QMessageContentContainerId& other)
{
    Q_UNUSED(other)
}

/*!
    \fn QMessageContentContainerId::QMessageContentContainerId(const QString& id)

    Constructs an identifier from \a id.
    
    \sa toString()
*/
QMessageContentContainerId::QMessageContentContainerId(const QString& id)
{
    Q_UNUSED(id)
}

/*!
    \fn bool QMessageContentContainerId::operator==(const QMessageContentContainerId &other) const

    Returns true if this identifier and the \a other identifier are identical;
    otherwise returns false.
*/
bool QMessageContentContainerId::operator==(const QMessageContentContainerId& other) const
{
    Q_UNUSED(other)
    return false; // stub
}

/*!
    \fn QMessageContentContainerId& QMessageContentContainerId::operator=(const QMessageContentContainerId &other)

    Assigns \a other to this identifier and returns a reference to this identifier.
*/
QMessageContentContainerId& QMessageContentContainerId::operator=(const QMessageContentContainerId& other)
{
    Q_UNUSED(other)
    return *this; // stub
}

/*!
    \fn bool QMessageContentContainerId::toString() const

    Returns the string representation of this identifier.
    
    A null string should be returned if and only if the identifier is invalid.
*/
QString QMessageContentContainerId::toString() const
{
    return QString::null; // stub
}

/*!
    \fn bool QMessageContentContainerId::isValid() const

    Returns true if this identifier is valid; otherwise returns false.
*/
bool QMessageContentContainerId::isValid() const
{
    return false; // stub
}

/*!
    \fn QMessageId QMessageContentContainerId::messageId() const

    Returns the identifier of the containing (parent) message if any; otherwise returns an invalid 
    message identifier.
*/
QMessageId QMessageContentContainerId::messageId() const
{
    return QMessageId(); // stub
}

/*! \typedef QMessageContentContainerIdList

    Qt-style synonym for QList<QMessageContentContainerId>
*/

/*!
    \class QMessageContentContainer
    \preliminary

    \brief The QMessageContentContainer class provides an interface for internet media (MIME) 
    type content that is possibly only partially retrieved.

    \ingroup messaging

    A QMessageContentContainer object can either directly contain media such as text, 
    image, audio, video, application or message data, or contain multiple parts of content, 
    but can not contain both media and multiple parts directly.
    
    Container objects can be constructed via their QMessageContentContainerId 
    identifier using the container() function of the parent QMessage, or constructed piece by 
    piece using setContentType(), setContent(), setHeaderField() and related functions.
    
    For textual content using a recognized charset encoding decodedTextContent() will 
    return the content as a unicode string.
    
    For non multipart content decodedContent() will return the content after decoding any 
    transfer encoding used to represent binary data using 7-bit ASCII characters, such as 
    quoted-printable and base64.
    
    contentFileName() will return the suggested filename for an attachment, when 
    defined.

    The internet media (MIME) type of the container is returned by contentType(),
    the content subtype is returned by contentSubType(), the content type charset parameter 
    is returned by contentCharset(), and the content suggested filename by contentFileName(). The 
    type of a container that contains multiple parts of content must be "multipart" (case 
    insensitive).
    
    An indication of the size of the container and its contents on the originating server is 
    given by indicativeSize(). If the content is completely available on the device 
    contentAvailable() will return true.

    decodedContentFileName() will return the name of a local file containing the content, when 
    available. Non multipart content can be serialized to a QDataStream using 
    writeContentTo(), and set using setContent() or setContentFromFile().
  
    A part of content can be appended to the existing content of a container using 
    appendContent(). Existing content may be replaced using replaceContent(). A list of 
    identifiers for directly contained parts of content is returned by contentIds().

    The default container contains an empty string, with type "text", subtype "plain", 
    and charset "US-ASCII".

    clearContents() will remove any existing parts, and set the content of the container 
    to be the default content.

    A container stores name value pairs known as header fields. Names are 
    ASCII strings, while values are charset encoded unicode strings. A header field may be 
    appended using appendHeaderField(), an existing header field may be 
    replaced using setHeaderField(). A list of header fields is returned by headerFields(). The 
    unicode string value of a header field is returned by headerField().

    A list of charset names is returned by preferredCharsets(). When encoding unicode QString text 
    using setContent(), appendHeaderField() or setHeaderField() the first charset in the list 
    returned by preferredCharsets() that is capable of encoding  all characters in the given 
    unicode QString text will be used, or if none is capable UTF-8 will be used.
    
    If the container has been modified since it was last constructed containerDataModified() 
    returns true.

    \sa QMessage, QMessageContentContainerId
*/

/*!
    Constructs an empty container object.
*/
QMessageContentContainer::QMessageContentContainer()
{
}

/*!
    Destroys the container object.
*/
QMessageContentContainer::~QMessageContentContainer()
{
}

/*!
      Returns the identifier of the container object.
*/
QMessageContentContainerId QMessageContentContainer::containerId() const
{
    return QMessageContentContainerId();
}

/*!
      Sets the identifier of the container object to \a id.
*/
void QMessageContentContainer::setContainerId(const QMessageContentContainerId &id)
{
    Q_UNUSED(id)
}

/*!
    Clears all existing content, including all parts, using clearContents() and sets the content 
    type of the container to \a data.

    The content subtype, content charset and suggested content file name of the container 
    will be set to the default values for type \a data.

    \sa clearContents(), contentType(), contentSubType(), contentCharset(), contentFileName(), setHeaderField()
*/
void QMessageContentContainer::setContentType(const QByteArray &data)
{
    Q_UNUSED(data)
}

/*!
    Returns the content type of the container. Common types are "text", "image", "audio", 
    "video", "application", "message" and "multipart".

    The internet media (MIME) type of the container is "multipart" iff the container directly 
    contains multiple parts rather than directly contains media.

    The default is "text".

    \sa setContentType()
*/
QByteArray QMessageContentContainer::contentType() const
{
    return QByteArray(); // stub
}

/*!
    Sets the internet media (MIME) content subtype of the content to \a data.

    \sa contentSubType()
*/
void QMessageContentContainer::setContentSubType(const QByteArray &data)
{
    Q_UNUSED(data);
}

/*!
    Returns the internet media (MIME) subtype of the content.

    The default is "plain" for "text" type media content, "mixed" for "multipart" type content, 
    and an empty array for other types of content.

    \sa setContentSubType()
*/
QByteArray QMessageContentContainer::contentSubType() const
{
    return QByteArray(); // stub
}

/*!
    Sets the internet media (MIME) content charset to \a data.

    \sa contentCharset()
*/
void QMessageContentContainer::setContentCharset(const QByteArray &data)
{
    Q_UNUSED(data)
}

/*!
    Returns the internet media (MIME) content charset, when defined; otherwise an empty array is 
    returned.

    The default is "US-ASCII" for "text" type content; and an empty array for other types.

    \sa setContentCharset()
*/
QByteArray QMessageContentContainer::contentCharset() const
{
    return QByteArray(); // stub
}

/*!
    Sets the suggested filename of the content to \a data.

    \sa contentFileName()
*/
void QMessageContentContainer::setContentFileName(const QByteArray &data)
{
    Q_UNUSED(data)
}

/*!
    Returns the suggested filename of the attachment, when defined;
    otherwise an empty array is returned.

    The default is an empty array.

    \sa setContentFileName()
*/
QByteArray QMessageContentContainer::contentFileName() const
{
    return QByteArray(); // stub
}

/*!
    Sets the content availability of the content contained to \a available.

    \sa contentAvailable()
*/
void QMessageContentContainer::setContentAvailable(bool available)
{
    Q_UNUSED(available)
}

/*!
    Returns true if the entirety of the content contained is available on the device; 
    otherwise returns false.
*/
bool QMessageContentContainer::contentAvailable() const
{
    return false;
}

/*!
    Set the size indication of the container including contents to \a size.

    \sa indicativeSize()
*/
void QMessageContentContainer::setIndicativeSize(uint size)
{
    Q_UNUSED(size)
}

/*!
    Returns an indication of the size of the container including contents.
*/
uint QMessageContentContainer::indicativeSize() const
{
    return 0;
}

/*!
    For textual content encoded with a recognized charset decodedTextContent() will return 
    the content as a unicode string; otherwise a null string is returned.
    
    \sa preferredCharsets()
*/
QString QMessageContentContainer::decodedTextContent() const
{
    return QString::null;
}

/*!
    Return the content after decoding any transfer encoding used to represent binary data 
    using 7-bit ASCII characters, such as quoted-printable and base64.
  
    For textual content any text charset encoding such as Shift-JIS, ISO 2022-JP, KOI8-R, 
    Windows-1251 etc will not be decoded.

    \sa decodedTextContent()
*/
QByteArray QMessageContentContainer::decodedContent() const
{
    return QByteArray(); // stub
}

/*!
    Returns the name of a local file containing the content, when available.
*/
QString QMessageContentContainer::decodedContentFileName() const
{
    return QString(); // stub
}

/*!
    For a non multipart container writes the content as would be returned by decodedContent() 
    into the stream \a out; otherwise for a multipart container does nothing.

    \sa readContentFrom()
*/
void QMessageContentContainer::writeContentTo(QDataStream& out) const
{
    Q_UNUSED(out)
}

/*!
    Clears existing content either media or parts, and resets the content type, subtype and 
    charset to default values for text content. 

    Does not modify header fields other than "Content-Type".

    \sa contentType(), contentSubType(), contentCharset()
*/
void QMessageContentContainer::clearContents()
{
}

/*!
    Sets the content to \a text, content type to "text", and charset to the first charset 
    returned by preferredCharsets() that can encode \a text if any; otherwise sets the charset 
    to "UTF-8".

    Does not modify the content subtype, the subtype should be set separately.

    \sa preferredCharsets(), setContentSubType()
*/
void QMessageContentContainer::setContent(const QString &text)
{
    Q_UNUSED(text)
}

/*!
    Sets the content to \a data.

    Does not modify the content type, subtype or charset, they should be set separately.

    \sa setContentType(), setContentSubType(), setContentCharset()

*/
void QMessageContentContainer::setContent(const QByteArray &data)
{
    Q_UNUSED(data)
}

/*!
    For a multipart container does nothing; otherwise sets the content of the container to be the 
    content of the file \a fileName.

    Does not modify the content type, subtype or charset, they should be set separately.

    \sa setContentType(), setContentSubType(), setContentCharset()
*/
void QMessageContentContainer::setContentFromFile(const QString &fileName)
{
    Q_UNUSED(fileName)
}

/*!
    For a multipart container does nothing; otherwise sets the content of the container by 
    reading from the stream \a in.

    Does not modify the content type, subtype or charset, they should be set separately.

    \sa writeContentTo(), setContentType(), setContentSubType(), setContentCharset()
*/
void QMessageContentContainer::readContentFrom(QDataStream &in)
{
    Q_UNUSED(in)
}

/*!
    Appends \a content to the end of the list of content contained.

    For a non multipart container, before a part is appended the content type of the 
    container is set to "multipart" and the contents of the container cleared with clearContents().

    Returns an identifier for the appended content.

    \sa container(), clearContents(), replaceContent(), contentIds()
*/
QMessageContentContainerId QMessageContentContainer::appendContent(const QMessageContentContainer & content)
{
    Q_UNUSED(content)
    return QMessageContentContainerId(); // stub
}

/*!
    If the container contains content with the identifier \a id, either directly or recursively 
    then replaces that content with \a content; otherwise does nothing.

    \sa container(), clearContents(), appendContent(), contentIds()
*/
void QMessageContentContainer::replaceContent(const QMessageContentContainerId &id, const QMessageContentContainer & content)
{
    Q_UNUSED(id)
    Q_UNUSED(content)
}

/*!
    For a multipart container returns a list of identifiers for all content directly contained by 
    the container; otherwise returns an empty list.

    \sa container(), clearContents(), appendContent(), replaceContent()
*/
QMessageContentContainerIdList QMessageContentContainer::contentIds() const
{
    return QMessageContentContainerIdList(); // stub
}

/*!
    If the container contains another container with identifier \a id either directly or 
    recursively then returns that other container; otherwise returns an empty container 
    constructed with the default constructor.

    \sa contentIds(), clearContents(), appendContent(), replaceContent()
*/
const QMessageContentContainer QMessageContentContainer::container(const QMessageContentContainerId id) const
{
    Q_UNUSED(id)
    return QMessageContentContainer(); // stub
}

/*!
    If the container contains another container with identifier \a id either directly or 
    recursively then returns that other container; otherwise returns an empty container 
    constructed with the default constructor.

    \sa contentIds(), clearContents(), appendContent(), replaceContent()
*/
QMessageContentContainer QMessageContentContainer::container(const QMessageContentContainerId id)
{
    Q_UNUSED(id)
    return QMessageContentContainer(); // stub
}

/*!
    Append a header field with name \a name and value \a value to the end of the list of 
    header fields for the container. Any existing header field with the same name is not 
    modified.

    If \a value is not ASCII text then it will be encoded by the first charset returned by 
    preferredCharsets() that can encode \a value if any; otherwise \a value will be encoded using 
    "UTF-8".

    \sa setHeaderField(), headerField(), headerFields()
*/
void QMessageContentContainer::appendHeaderField(const QByteArray &name, const QString &value)
{
    Q_UNUSED(name)
    Q_UNUSED(value)    
}

/*!
    Sets the value of the first header field of the container with name \a name to \a value if it 
    already exists; otherwise appends a header with the supplied name and value.

    If \a value is not ASCII text then it will be encoded by the first charset returned by 
    preferredCharsets() that can encode \a value if any; otherwise \a value will be encoded using 
    "UTF-8".

    The type, subtype and charset of the container are stored in the 'Content-Type' header
    field, the transfer encoding, such as quoted-printable, is stored in the 
    'Content-Transfer-Encoding' header field, the filename of the container is stored in
    the 'Content-Disposition' header field. These header fields should not be modified using
    the setHeaderField() function and doing so may result in undefined behavior.

    \sa appendHeaderField(), headerField(), headerFields(), preferredCharsets()
*/
void QMessageContentContainer::setHeaderField(const QByteArray &name, const QString &value)
{
    Q_UNUSED(name)
    Q_UNUSED(value)    
}

/*!
    Returns the value of the first header field of the container with the name \a name, if any;
    otherwise returns a null string.

    \sa headerFieldValues(), appendHeaderField(), setHeaderField(), headerFields(), preferredCharsets()
*/
QString QMessageContentContainer::headerField(const QByteArray &name) const
{
    Q_UNUSED(name)
    return QString(); // stub
}

/*!
    Returns a list of values of header fields with the name \a name, if any;
    otherwise returns an empty list.

    \sa appendHeaderField(), setHeaderField(), headerField(), headerFields(), preferredCharsets()
*/
QList<QString> QMessageContentContainer::headerFieldValues(const QByteArray &name) const
{
    Q_UNUSED(name)
    return QList<QString>(); // stub
}

/*!
    Returns a list of names of header fields of the container.

    \sa appendHeaderField(), setHeaderField(), headerField(), preferredCharsets()
*/
QList<QByteArray> QMessageContentContainer::headerFields() const
{
    return QList<QByteArray>(); // stub
}

/*!
    Append a header field with name \a name and value \a value to the end of the list of 
    header fields for the container. Any existing header field with the same name is not 
    modified.

    \sa setHeaderField(), headerField(), headerFields(), preferredCharsets()
*/
void QMessageContentContainer::appendHeaderField(const QByteArray &name, const QByteArray &value)
{
    Q_UNUSED(name)
    Q_UNUSED(value)    
}

/*!
    Sets the value of the first header field of the container with name \a name to \a value if it 
    already exists; otherwise appends a header with the supplied name and value.

    \sa appendHeaderField(), headerField(), headerFields(), preferredCharsets()
*/
void QMessageContentContainer::setHeaderField(const QByteArray &name, const QByteArray &value)
{
    Q_UNUSED(name)
    Q_UNUSED(value)    
}

/*!
    Sets the ordered by preference list of names of charsets to use when encoding unicode QString 
    text with setContent(), appendHeaderField() or setHeaderField() to \a charsetNames.

    \sa preferredCharsets()
*/
void QMessageContentContainer::setPreferredCharsets(const QList<QByteArray> &charsetNames)
{
    Q_UNUSED(charsetNames);
}

/*!
    Returns an ordered by preference list of charset names to use when encoding unicode QString 
    text with setContent(), appendHeaderField() or setHeaderField().

    \sa setPreferredCharsets()
*/
QList<QByteArray> QMessageContentContainer::preferredCharsets()
{
    return QList<QByteArray>(); // stub;
}

/*!
    Returns true if the container has been modified since it was constructed; 
    otherwise returns false.
*/
bool QMessageContentContainer::containerDataModified() const
{
    return false; // stub
}

/*!
    Sets the modified data state of the container to \a modified.
*/
void QMessageContentContainer::setContainerDataModified(bool modified)
{
    Q_UNUSED(modified);
}

/*!
    Prepend \a content to the start of the list of content contained.

    For a non multipart container, before a part is prepended the content type of the 
    container is set to "multipart" and the contents of the container cleared with clearContents().

    Returns an identifier for the prepended content.

    \sa QMessage::setBody()
*/
QMessageContentContainerId QMessageContentContainer::prependContent(const QMessageContentContainer & content)
{
    Q_UNUSED(content);
    return QMessageContentContainerId(); // stub
}


/*!
    \class QMessageAddress

    \preliminary
    \brief The QMessageAddress class provides an interface for a message address.

    \ingroup messaging
   
    A message address consists of a recipient string and a type.
*/    

/*!
    \enum QMessageAddress::Type

    This enum type is used to describe the type of a message address.
    
    \value System   A system address.
    \value Phone    A telephony address.
    \value Email    An Email, Internet Message Format address.
    \value Xmpp     An XMPP, Extensible Messaging and Presence Protocol address.
    
    \sa type(), setType()
*/

/*!
    Constructs an empty message address.
*/
QMessageAddress::QMessageAddress()
{
}

/*!
    Constructs a message address with the given recipient \a recipient and type \a type.
*/
QMessageAddress::QMessageAddress(const QString &recipient, Type type)
{
    Q_UNUSED(recipient);
    Q_UNUSED(type);
}

/*!
    Destroys the message address.
*/
QMessageAddress::~QMessageAddress()
{
}

/*!
    Returns the recipient.

    \sa setRecipient()
*/
QString QMessageAddress::recipient() const
{
    return QString(); // stub
}

/*!
    Sets the recipient to \a recipient.

    \sa recipient()
*/
void QMessageAddress::setRecipient(const QString &recipient)
{
    Q_UNUSED(recipient);
}

/*!
    Returns the type of the message address.

    \sa setType()
*/
QMessageAddress::Type QMessageAddress::type() const
{
    return System; // stub
}

/*!
    Sets the type of the message address to \a type.

    \sa type()
*/
void QMessageAddress::setType(Type type)
{
    Q_UNUSED(type);
}

/*! \typedef QMessageAddressList

    Qt-style synonym for QList<QMessageAddress>
*/


/*!
    \class QMessage

    \preliminary
    \brief The QMessage class provides a convenient interface for working with messages.
    
    \ingroup messaging
   
    QMessage supports a number of types. Including internet email messages, 
    the telephony types SMS and MMS, and also XMPP messages.
     
    The QMessageId identifier for a message is returned by id(). Messages can be constructed by 
    retrieval from the message store via their identifier using QMessageStore::message(). A 
    QMessage can also be constructed piece by piece using functions such as 
    setType(), setFrom(), setTo(), setSubject(), setBody() and appendAttachments(). 
    
    Alternatively a message can be initialized from raw data using fromTransmissionFormat().
    A message may be serialized to a QDataStream, or returned as a QByteArray using 
    toTransmissionFormat().
    
    If a message has been modified since it was last constructed dataModified() returns true.

    A list of attachments identifiers will be returned by attachments() and an identifier for the 
    message body will be returned by body(). Attachments can be appended to the content of the 
    message using appendAttachments(), the body of the message can be set with setBody() or
    setBodyFromFile().
    
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
    
    In addition to and separate from the message content information related to a message may be
    stored in name value pairs known as custom fields, both names and values are unicode strings. 
    A custom field may be set or updated with setCustomField(), the value of existing custom field 
    can be retrieved with customField(), and a list of existing custom fields is returned by
    customFields().
    
    \sa QMessageContentContainer, QMessageStore, QMessageId
*/

/*!
    \enum QMessage::Type

    This enum type is used to describe the type of a message.
    
    \value Mms      The message is an MMS, Multimedia Messaging Service object.
    \value Sms      The message is an SMS, Short Message Service object.
    \value Email    The message is an Email, Internet Message Format object.
    \value Xmpp     The message is an XMPP, Extensible Messaging and Presence Protocol object.
    \value None     The message type is not defined.
    \value AnyType  Bitflag value that matches any message type defined.
    
    \sa type(), setType()
*/
    
/*!
    \enum QMessage::Status

    This enum type is used to describe the status of a message.

    \value Read            This flag indicates that the content of this message has been displayed to the user.
    \value HasAttachments  This flag indicates that the message contains at least one sub-part with 'Attachment' disposition.
    \value Incoming        This flag indicates that the message has been sent from an external source.
    \value Removed         This flag indicates that the message has been deleted from or moved on the originating server.
    
    \sa status(), setStatus()
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
    
    \sa toTransmissionFormat()
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
    
    \sa toTransmissionFormat()
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
    
    \sa fromTransmissionFormat()
*/
QByteArray QMessage::toTransmissionFormat() const
{
    return QByteArray(); // stub
}

/*!
    Writes the message to the output stream \a out.
    
    See the class description for the encapsulations used for each
    message type.

    \sa fromTransmissionFormat()
*/
void QMessage::toTransmissionFormat(QDataStream& out) const
{
    Q_UNUSED(out)
}

/*!
    Returns the identifier of the message.

    \sa QMessageFilterKey::id()
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
QMessageAddress QMessage::from() const
{
    return QMessageAddress(); // stub
}

/*!
    Sets the from address, that is the originating address of the message to \a address.

    \sa from()
*/
void QMessage::setFrom(const QMessageAddress &address)
{
    Q_UNUSED(address)
}

/*!
    Returns the subject of the message, if present; otherwise returns a null string.

    \sa setSubject(), QMessageFilterKey::subject()
*/
QString QMessage::subject() const
{
    return QString::null; //stub
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
    otherwise returns a null timestamp.
    
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
    otherwise returns a null timestamp.
    
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
QMessageAddressList QMessage::to() const
{
    return QMessageAddressList(); // stub
}

/*! 
    Sets the list of primary recipients for the message to \a toList.
    
    \sa to()
*/
void QMessage::setTo(const QMessageAddressList& toList)
{
    Q_UNUSED(toList)
}

/*! 
    Sets the primary recipient for the message to \a address.
    
    \sa to()
*/
void QMessage::setTo(const QMessageAddress& address)
{
    Q_UNUSED(address)
}

/*!
    Returns the list of all the cc (carbon copy) recipients specified for the message.

    \sa to(), bcc(), setCc(), QMessageFilterKey::recipients()
*/  
QMessageAddressList QMessage::cc() const
{
    return QMessageAddressList(); // stub
}

/*!
    Set the list of cc (carbon copy) recipients for the message to \a ccList.

    \sa cc(), setTo(), setBcc()
*/  
void QMessage::setCc(const QMessageAddressList& ccList)
{
    Q_UNUSED(ccList)
}

/*!
    Returns the list of all the bcc (blind carbon copy) recipients specified for the message.

    \sa to(), cc(), setBcc()
*/  
QMessageAddressList QMessage::bcc() const
{
    return QMessageAddressList(); // stub
}

/*!
    Set the list of bcc (blind carbon copy) recipients for the message to \a bccList.

    \sa bcc(), setTo(), setCc()
*/  
void QMessage::setBcc(const QMessageAddressList& bccList)
{
    Q_UNUSED(bccList)
}

/*!
    Returns the status flags value for the message.

    \sa setStatus(), QMessageFilterKey::status()
*/
QMessage::StatusFlags QMessage::status() const
{
    return StatusFlags(None); // stub
}

/*!
    Sets the status flags value for the message to \a newStatus.

    \sa status()
*/
void QMessage::setStatus(QMessage::StatusFlags newStatus)
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
    
    \sa QMessageContentContainer, setBody()
*/
QMessageContentContainerId QMessage::body() const
{
    return QMessageContentContainerId(); // stub
}

/*!
    Sets the body text of the message to be the string \a body.
    
    The internet media (MIME) content type of the body will be "text", the subtype will be 
    "plain", the charset will be determined by preferredCharsets(),
    
    If the internet media (MIME) content type of the message is not multipart then the content of 
    the message will be set to \a body; otherwise the body will be the first part of the message.
    
    \sa body(), setBodyFromFile(), preferredCharsets()
*/
void QMessage::setBody(const QString &body)
{
    // Implementation note, this should be platform independent. Will require a member variable 
    // for the body id, maybe should add protected setBodyId() and bodyId() methods to the API.
    Q_UNUSED(body)
}

/*!
    Sets the body text of the message to be the contents of the file \a fileName.
    
    The internet media (MIME) content type of the body will be "text", the subtype will be 
    "plain", the charset will be determined by preferredCharsets(),
    
    If the internet media (MIME) content type of the message is not multipart then the content of 
    the message will be set to the contents of the file \a fileName; otherwise the body will be the 
    first part of the message.
        
    \sa body(), setBody(), preferredCharsets()
*/
void QMessage::setBodyFromFile(const QString &fileName)
{
    // Implementation note, this should be platform independent. Will require a member variable 
    // for the body id. Will need to use prepend for multipart messages.
    Q_UNUSED(fileName)
}

/*!
    Returns a list of attachments for the message.

    The body of the message will not be included in the list.
    
    TODO: Example attachment list generation algorithm, message parts are the main issue, have to 
    recurse into them.
    
    \sa appendAttachments(), clearAttachments()
*/
QMessageContentContainerIdList QMessage::attachments() const
{
    // Implementation note, this should be platform independent.
    return QMessageContentContainerIdList(); // stub
}

/*!
    Append the contents of the files specified by \a fileNames to the end of the list of 
    attachments for the message. The internet media (MIME) type of the attachments will be 
    determined by examining the contents the files.

    \sa attachments(), clearAttachments()
*/
void QMessage::appendAttachments(const QStringList &fileNames)
{
    // Implementation note, this should be platform independent.
    Q_UNUSED(fileNames)
}

/*!
    Clears the list of attachments for the message, leaving only the message body, if any.

    \sa attachments(), appendAttachments()
*/  
void QMessage::clearAttachments()
{
    // Implementation note, this should be platform independent.
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
    Returns the value of the custom field with name \a name.

    \sa setCustomField(), customFields()
*/
QString QMessage::customField(const QString &name) const
{
    Q_UNUSED(name);
    return QString(); // stub
}

/*!
    Sets the value of the custom field with name \a name to \a value.

    \sa customField(), customFields()
*/
void QMessage::setCustomField(const QString &name, const QString &value)
{
    Q_UNUSED(name);
    Q_UNUSED(value);
}

/*!
    Returns a list of names of custom fields.

    \sa customField(), setCustomField()
*/
QList<QString> QMessage::customFields() const
{
    return QList<QString>(); // stub
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
    Sets the modified data state of the message to \a modified.
*/
void QMessage::setDataModified(bool modified)
{
    Q_UNUSED(modified);
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
    \enum QMessageFilterKey::Option

    This enum describes additional matching criteria when performing a search.
    
    \value FullWord        The key should only match a complete word
    \value CaseInsensitive The key should not be case sensitive.
    
    \sa options(), setOptions()
*/

/*!
    Set the options for the search key to \a options.

    \sa options()
*/
void QMessageFilterKey::setOptions(Options options)
{
    Q_UNUSED(options)
}

/*!
    Return the options for the search key.
    
    Default is no options set.

    \sa setOptions()
*/
QMessageFilterKey::Options QMessageFilterKey::options()
{
    return 0;
}

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
QMessageFilterKey QMessageFilterKey::type(QMessage::TypeFlags type, QMessageDataComparator::InclusionComparator cmp)
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
QMessageFilterKey QMessageFilterKey::status(QMessage::Status value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages whose status is a bitwise match to \a mask, according to \a cmp.

    \sa QMessage::status()
*/
QMessageFilterKey QMessageFilterKey::status(QMessage::StatusFlags mask, QMessageDataComparator::InclusionComparator cmp)
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
    Returns a key matching messages having a custom field with the name \a name, whose 
    value matches \a value, according to \a cmp.

    \sa QMessage::customField()
*/
QMessageFilterKey QMessageFilterKey::customField(const QString &name, const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(name)
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

/*!
    Returns a key matching messages having a custom field with the name \a name, whose value 
    matches the substring \a value, according to \a cmp.

    \sa QMessage::customField()
*/
QMessageFilterKey QMessageFilterKey::customField(const QString &name, const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(name)
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

    The QMessageStore class is accessed through a singleton interface and provides a functions
    for adding, and updating, deleting and querying in the message store.

    QMessageStore also provides functions for querying and counting of QMessages
    when used in conjunction with QMessageFilterKey class, namely queryMessages() and 
    countMessages().

    If a QMessageStore operation fails, the lastError() function will return an error code
    value indicating the failure mode encountered.  A successful operation will set the 
    lastError() result to QMessageStore::NoError.

    Messages in the message store are identified by QMessageId objects. The data associated
    with a message is retrieved in the form of a QMessage object using message().

    Messages can be inserted into the store using the addMessage() function, messages in the 
    store can be manipulated via the updateMessage() function ,and removed by the 
    removeMessage() functions.
    
    Message store manipulations involving messages are reported via the messagesAdded(), 
    messagesUpdated() and messagesRemoved() signals.

    \sa QMessage, QMessageId, QMessageContentContainerId, QMessageFilterKey, QMessageSortKey
*/

/*!
    \enum QMessageStore::RemovalOption

    Defines whether or not a message will be removed from the originating server.

    \value NoRemovalRecord     Do not remove the message from the originating server.
    \value CreateRemovalRecord Remove the message from the originating server.
*/

/*
    TODO capabilities AtomicBooleanSarch AtomicSlices Sms Mms Email Xmpp Presence AtomicExtendedSearching AtomicBodySearching
      ExtendedServices, AtomicCustomSearching

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

    \sa removeMessages(), addMessage(), updateMessage()
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

    \sa removeMessage(), addMessage(), updateMessage()
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

    \sa message(), updateMessage(), removeMessage()
*/
bool QMessageStore::addMessage(QMessage *m)
{
    Q_UNUSED(m)
    return true; // stub
}

/*!
    Updates the existing QMessage \a m on the message store.
    Returns \c true if the operation completed successfully, or \c false otherwise. 
    
    \sa addMessage(), removeMessage()
*/
bool QMessageStore::updateMessage(QMessage *m)
{
    Q_UNUSED(m)
    return true; // stub
}

/*!
   Returns the QMessage defined by the QMessageContentContainerId \a id from the store.
*/
QMessage QMessageStore::message(const QMessageId& id) const
{
    Q_UNUSED(id)
    return QMessage(); // stub
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

    \sa QMessageContentContainerId
*/
void QMessageServiceAction::retrieve(const QMessageContentContainerId& id)
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

//#include "draft-api.moc"
