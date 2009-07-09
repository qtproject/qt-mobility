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
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessage.h"
#include "qmessage_p.h"

/*!
    \class QMessage

    \preliminary
    \brief The QMessage class provides a convenient interface for working with messages.
    
    \ingroup messaging
   
    QMessage supports a number of types. Including internet email messages, 
    the telephony types SMS and MMS, and also XMPP messages.
     
    The QMessageId identifier for a message is returned by id(). Messages can be constructed by 
    retrieval from the messaging store via their identifier using QMessageStore::message(). A 
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
    
    The folder and account a message is associated with are returned by parentFolderId() and
    parentAccountId() respectively.
    
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
    \enum QMessage::StandardFolder

    Defines the standard folders.

    \value InboxFolder   Represents the standard inbox folder.
    \value DraftsFolder  Represents the standard drafts folder.
    \value OutboxFolder  Represents the standard outbox folder.
    \value SentFolder    Represents the standard sent folder.
    \value TrashFolder   Represents the standard trash folder.
*/

/*!
    Constructs an empty message.
*/
QMessage::QMessage()
{
}

/*!
    Constructs a message from data stored in the messaging store with identifier \a id.
    
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
    Returns the identifier of the parent account of the message if any; otherwise returns an 
    invalid identifier.
*/
QMessageAccountId QMessage::parentAccountId() const
{
    return QMessageAccountId(); // stub
}
    
/*!
    Sets the identifier of the parent account of the message to \a accountId.
*/
void QMessage::setParentAccountId(const QMessageAccountId &accountId)
{
    Q_UNUSED(accountId)
}

/*!
    Returns the identifier of the folder that contains the message if any; otherwise returns an 
    invalid identifier.
*/
QMessageFolderId QMessage::parentFolderId() const
{
    return QMessageFolderId(); // stub
}
    
/*!
    Sets the identifier of the folder that contains the message to \a folderId.
*/
void QMessage::setParentFolderId(const QMessageFolderId &folderId)
{
    Q_UNUSED(folderId)
}

/*!
    Returns the standard folder of the message.
    
    Defaults to InboxFolder.
*/
QMessage::StandardFolder QMessage::standardFolder() const
{
    return QMessage::InboxFolder; // stub
}
    
/*!
    Sets the standard folder of the message to \a sf.
*/
void QMessage::setStandardFolder(StandardFolder sf)
{
    Q_UNUSED(sf)
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
    
    \sa QMessageFilterKey::size()
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
    
    \sa QMessageContentContainer, setBody()
*/
QMessageContentContainerId QMessage::body() const
{
    // TODO: Example body finding algorithm.
    // If the content type of the message is text, then that is the body
    // otherwise if the first part of the body is text then that is the body.
    
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
    
    \sa appendAttachments(), clearAttachments()
*/
QMessageContentContainerIdList QMessage::attachments() const
{
    //    TODO: Example attachment list generation algorithm, message parts are the main issue, maybe 
    //    have to recurse into them, somewhat ambiguous.
    //    Don't recurse, just ignore any body part, see body() for body finding algorithm.
    
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
  Creates a reply to sender response to the message
  
  \sa QMessageServiceAction::compose()
*/
QMessage QMessage::replyTo() const
{
    return QMessage(); // stub
}

/*!
  Creates a reply to all response to the message
  
  \sa QMessageServiceAction::compose()
*/
QMessage QMessage::replyToAll() const
{
    return QMessage(); // stub
}

/*!
  Creates a copy the message in a format suitable for forwarding.
  
  \sa QMessageServiceAction::compose()
*/
QMessage QMessage::forward() const
{
    return QMessage(); // stub
}
