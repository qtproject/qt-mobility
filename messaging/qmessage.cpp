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
** contact Nokia at http://qt.nokia.com/contact.
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

    Message data that is less commonly accessed or relatively large should be lazily retrieved 
    on demand by the associated getter function.
    
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
    \value NoType   The message type is not defined.
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

    \value HighPriority    The message is high priority.
    \value NormalPriority  The message is normal priority.
    \value LowPriority     The message is low priority.
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
    \fn QMessage::QMessage()
    
    Constructs an empty message.
*/

/*!
    \fn QMessage::(const QMessageId& id)

    Constructs a message from data stored in the messaging store with identifier \a id.
    
    \sa QMessageStore
*/

/*!
    \fn QMessage::QMessage(const QMessage &other)

    Constructs a copy of \a other.
    
    \sa operator=()
*/

/*!
    \fn QMessage::operator=(const QMessage& other)
  
    Assign the value of the QMessage \a other to this.
*/

/*!
    \fn QMessage::~QMessage()
    
    Destroys the message.
*/

/*!
    \fn QMessage::fromTransmissionFormat(Type t, const QByteArray &ba)
    
    Constructs a message of type \a t from the data contained in \a ba.
    
    See the class description for a list of supported message encapsulations.
    
    \sa toTransmissionFormat()
*/
    
/*!
    \fn QMessage::fromTransmissionFormatFile(Type t, const QString& fileName)
    
    Constructs a message of type \a t from the data contained in \a fileName.

    See the class description for a list of supported message encapsulations.
    
    \sa toTransmissionFormat()
*/

/*!
    \fn QMessage::toTransmissionFormat() const
    
    Returns the message in a format suitable for transmission.
    
    See the class description for the encapsulations used for each
    message type.
    
    \sa fromTransmissionFormat()
*/

/*!
    \fn QMessage::toTransmissionFormat(QDataStream& out) const
  
    Writes the message to the output stream \a out.
    
    See the class description for the encapsulations used for each
    message type.

    \sa fromTransmissionFormat()
*/

/*!
    \fn QMessage::id() const
  
    Returns the identifier of the message.

    \sa QMessageFilter::byId()
*/
    
/*!
    \fn QMessage::type() const
    
    Returns the Type of the message.
    
    \sa setType(), QMessageFilter::byType()
*/
    
/*!
    \fn QMessage::setType(Type t)
    
    Sets the Type of the message to \a t.
    
    The type of a message may be set for non-empty messages.
    
    \sa type()
*/

/*!
    \fn QMessage::parentAccountId() const
    
    Returns the identifier of the parent account of the message if any; otherwise returns an 
    invalid identifier.
*/
    
/*!
    \fn QMessage::setParentAccountId(const QMessageAccountId &accountId)
    
    Sets the parent account of the message to the account with identifier \a accountId.
    
    This operation is only permitted on new messages that have not yet been inserted into
    the message store. Attempting to change the parent account of a message already
    in the message store will result in an error when attempting to update the message
    with QMessageStore::update().
*/
    
/*!
    \fn QMessage::parentFolderId() const
  
    Returns the identifier of the folder that contains the message if any; otherwise returns an 
    invalid identifier.
*/

/*!
    \fn QMessage::standardFolder() const
  
    Returns the standard folder of the message.
    
    Defaults to InboxFolder.
*/
    
/*!
    \fn QMessage::setStandardFolder(StandardFolder sf)
  
    Sets the standard folder of the message to \a sf.
*/
    
/*!
    \fn QMessage::from() const
  
    Returns the originating address of the message.

    \sa setFrom(), QMessageFilter::bySender()
*/

/*!
    \fn QMessage::setFrom(const QMessageAddress &address)
  
    Sets the from address, that is the originating address of the message to \a address.

    \sa from()
*/

/*!
    \fn QMessage::subject() const
  
    Returns the subject of the message, if present; otherwise returns a null string.

    \sa setSubject(), QMessageFilter::bySubject()
*/

/*!
    \fn QMessage::setSubject(const QString &s)
    
    Sets the subject of the message to \a s.
    
    \sa subject()
*/

/*!
    \fn QMessage::date() const
  
    Returns the timestamp contained in the origination date header field of the message, if present; 
    otherwise returns a null timestamp.
    
    \sa setDate(), QMessageFilter::byTimeStamp()
*/

/*!
    \fn QMessage::setDate(const QDateTime &d)
  
    Sets the origination date header field specifying the timestamp of the message to \a d.
    
    \sa date()
*/

/*!
    \fn QMessage::receivedDate() const
  
    Returns the timestamp placed in the message during reception by the device, if present;
    otherwise returns a null timestamp.
    
    \sa setReceivedDate(), QMessageFilter::byReceptionTimeStamp()
*/

/*!
    \fn QMessage::setReceivedDate(const QDateTime &d)
  
    Sets the timestamp indicating the time of message reception by the device to \a d.
    
    \sa receivedDate()
*/

/*! 
    \fn QMessage::to() const
  
    Returns the list of primary recipients for the message.

    \sa setTo(), QMessageFilter::byRecipients()
*/

/*! 
    \fn QMessage::setTo(const QMessageAddressList& toList)
  
    Sets the list of primary recipients for the message to \a toList.
    
    \sa to()
*/

/*! 
    \fn QMessage::setTo(const QMessageAddress& address)
  
    Sets the primary recipient for the message to \a address.
    
    \sa to()
*/

/*!
    \fn QMessage::cc() const
  
    Returns the list of all the cc (carbon copy) recipients specified for the message.

    \sa to(), bcc(), setCc(), QMessageFilter::byRecipients()
*/  

/*!
   \fn QMessage::setCc(const QMessageAddressList& ccList)
  
    Set the list of cc (carbon copy) recipients for the message to \a ccList.

    \sa cc(), setTo(), setBcc()
*/  

/*!
    \fn QMessage::bcc() const
  
    Returns the list of all the bcc (blind carbon copy) recipients specified for the message.

    \sa to(), cc(), setBcc()
*/  

/*!
    \fn QMessage::setBcc(const QMessageAddressList& bccList)
  
    Set the list of bcc (blind carbon copy) recipients for the message to \a bccList.

    \sa bcc(), setTo(), setCc()
*/  

/*!
    \fn QMessage::status() const
    
    Returns the status flags value for the message.

    \sa setStatus(), QMessageFilter::byStatus()
*/

/*!
    \fn QMessage::setStatus(QMessage::StatusFlags newStatus)
    
    Sets the status flags value for the message to \a newStatus.

    \sa status()
*/

/*!
    \fn QMessage::priority() const
    
    Returns the priority of the message.

    The default is NormalPriority.

    \sa setPriority(), QMessageFilter::byPriority()
*/

/*!
    \fn QMessage::setPriority(Priority newPriority)
    
    Sets the priority of the message to \a newPriority.

    \sa priority()
*/

/*!
    \fn QMessage::size() const
    
    Returns the complete size of the message as indicated on the originating server.
    
    \sa QMessageFilter::bySize()
*/

/*!
    \fn QMessage::body() const
  
    Returns the identifier for the body content contained by the Message if a body exists; 
    otherwise returns an invalid identifier.
    
    \sa QMessageContentContainer, setBody()
*/

/*!
    \fn QMessage::setBody(const QString &body)
  
    Sets the body text of the message to be the string \a body.
    
    The internet media (MIME) content type of the body will be "text", the subtype will be 
    "plain", the charset will be determined by preferredCharsets(),
    
    If the internet media (MIME) content type of the message is not multipart then the content of 
    the message will be set to \a body; otherwise a body part will be created if one does not 
    exist, and the body will be the first part of the message.
    
    \sa body(), setBodyFromFile(), preferredCharsets()
*/

/*!
    \fn QMessage::setBodyFromFile(const QString &fileName)
  
    Sets the body text of the message to be the contents of the file \a fileName.
    
    The internet media (MIME) content type of the body will be "text", the subtype will be 
    "plain", the charset will be determined by preferredCharsets(),
    
    If the internet media (MIME) content type of the message is not multipart then the content of 
    the message will be set to the contents of the file \a fileName; otherwise the body will be the 
    first part of the message.
        
    \sa body(), setBody(), preferredCharsets()
*/

/*!
    \fn QMessage::attachments() const
  
    Returns a list of attachments for the message.

    The body of the message will not be included in the list.
    
    \sa appendAttachments(), clearAttachments()
*/

/*!
    \fn QMessage::appendAttachments(const QStringList &fileNames)
  
    Append the contents of the files specified by \a fileNames to the end of the list of 
    attachments for the message. The internet media (MIME) type of the attachments will be 
    determined by examining the files or file names.

    \sa attachments(), clearAttachments()
*/

/*!
    \fn QMessage::clearAttachments()
  
    Clears the list of attachments for the message, leaving only the message body, if any.

    \sa attachments(), appendAttachments()
*/  

/*!
    \fn QMessage::setOriginatorPort(uint port)
  
    Sets the originating port of the message to \a port.

    Only relevant for SMS messages.
    
    \sa originatorPort(), setDestinationPort()
*/

/*!
    \fn QMessage::originatorPort()
  
    Returns the originating port of the message.

    Only relevant for SMS messages.

    Default is 0.
    
    \sa setOriginatorPort(), setDestinationPort()
*/

/*!
    \fn QMessage::setDestinationPort(uint port)
  
    Sets the destination port of the message to \a port.

    Only relevant for SMS messages.
    
    \sa setOriginatorPort(), destinationPort()
*/

/*!
    \fn QMessage::destinationPort()
  
    Returns the destination port of the message.

    Only relevant for SMS messages.

    Default is 0.
    
    \sa setDestinationPort(), setOriginatorPort()
*/

/*!
    \fn QMessage::customField(const QString &name) const
    
    Returns the value of the custom field with name \a name.

    \sa setCustomField(), customFields()
*/

/*!
    \fn QMessage::setCustomField(const QString &name, const QString &value)
    Sets the value of the custom field with name \a name to \a value.

    \sa customField(), customFields()
*/

/*!
    \fn QMessage::customFields() const
    
    Returns a list of names of custom fields.

    \sa customField(), setCustomField()
*/


/*!
    \fn QMessage::dataModified() const
    
    Returns true if the message has been modified since it was constructed; 
    otherwise returns false.
*/

/*!
    \fn QMessage::replyTo() const

    Creates a reply to sender response to the message
  
    \sa QMessageServiceAction::compose()
*/

/*!
    \fn QMessage::replyToAll() const
    
    Creates a reply to all response to the message
  
    \sa QMessageServiceAction::compose()
*/

/*!
    \fn QMessage::forward() const

    Creates a copy the message in a format suitable for forwarding.
  
    \sa QMessageServiceAction::compose()
*/
