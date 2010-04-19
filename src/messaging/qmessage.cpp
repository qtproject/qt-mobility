/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
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
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessage.h"
#ifdef Q_OS_SYMBIAN
#include "qmessage_symbian_p.h"
#else
#include "qmessage_p.h"
#endif

#include <QTextCodec>
#include <QDebug>

QTM_BEGIN_NAMESPACE

namespace {

QList<QByteArray> charsets;

}

/*!
    \class QMessage

    \preliminary
    \brief The QMessage class provides a convenient interface for working with messages.
    
    \ingroup messaging
   
    QMessage supports a number of types. Including internet email messages, 
    and the telephony types SMS and MMS.
     
    The QMessageId identifier for a message is returned by id(). Messages can be constructed by 
    retrieval from the messaging store via their identifier using QMessageManager::message(). A 
    QMessage can also be constructed piece by piece using functions such as 
    setType(), setFrom(), setTo(), setSubject(), setBody() and appendAttachments(). 
    
    If a message has been modified since it was last constructed isModified() returns true.

    A list of attachments identifiers will be returned by attachmentIds() and an identifier for the 
    message body will be returned by bodyId(). Attachments can be appended to the content of the 
    message using appendAttachments(), the body of the message can be set with setBody().
    
    The folder and account a message is associated with are returned by parentFolderId() and
    parentAccountId() respectively.

    Message data that is less commonly accessed or relatively large should be lazily retrieved 
    on demand by the associated getter function.
        
    Only phone numbers are valid destination addresses for SMS messages, only email addresses are valid
    destination addresses for Email messages, MMS messages may be addressed to either phone numbers
    or email addresses.
    
    \sa QMessageContentContainer, QMessageManager, QMessageId
*/

/*!
    \enum QMessage::Type

    This enum type is used to describe the type of a message.
    
    \value NoType             The message type is not defined.
    \value Mms                The message is an MMS, Multimedia Messaging Service object.
    \value Sms                The message is an SMS, Short Message Service object.
    \value Email              The message is an Email, Internet Message Format object.
    \value InstantMessage     The message is an instant message object, such as XMPP.
    \value AnyType            Bitflag value that matches any message type defined.
    
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
    \enum QMessage::ResponseType

    Defines the type of a response to an existing message.

    \value ReplyToSender    A response to the sender of the existing message.
    \value ReplyToAll       A response to the sender of the existing message, and any other recipients of that message.
    \value Forward          A response created to copy the content of the existing message to a new recipient.
*/

/*!
    \fn QMessage::QMessage()
    
    Constructs an empty message.
*/

/*!
    \fn QMessage::QMessage(const QMessageId& id)

    Constructs a message from data stored in the messaging store with identifier \a id.
    
    \sa QMessageManager::message()
*/

/*!
    \fn QMessage::QMessage(const QMessage &other)

    Constructs a copy of \a other.
*/

/*!
    \internal
    \fn QMessage::operator=(const QMessage& other)
*/

/*!
    \fn QMessage::~QMessage()
    
    Destroys the message.
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
    
    Defaults to DraftsFolder.
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
    \fn QMessage::setSubject(const QString &text)
    
    Sets the subject of the message to \a text.
    
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
    \fn QMessage::setStatus(QMessage::Status flag, bool set)
    
    Sets the status flag \a flag for the message to have the value \a set.

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
    \fn QMessage::bodyId() const
  
    Returns the identifier for the body content contained by the Message if a body exists; 
    otherwise returns an invalid identifier.
    
    \sa QMessageContentContainer, setBody()
*/

/*!
    \fn QMessage::setBody(const QString &body, const QByteArray &mimeType)
  
    Sets the body text of the message to be the string \a body.
    
    The internet media (MIME) content type of the body is set to \a mimeType, if provided.
    If the \a mimeType is not specified, the content type will default to "text/plain", and
    the encoding charset will be as determined by preferredCharsets().
    
    \sa bodyId(), preferredCharsets()
*/

/*!
    \fn QMessage::setBody(QTextStream &in, const QByteArray &mimeType)
  
    Sets the body text of the message to be the text read from the stream \a in.
    
    The internet media (MIME) content type of the body is set to \a mimeType, if provided.
    If the \a mimeType is not specified, the content type will default to "text/plain", and
    the encoding charset will be as determined by preferredCharsets().
    
    \sa bodyId(), preferredCharsets()
*/

/*!
    \fn QMessage::attachmentIds() const
  
    Returns a list of attachment identifiers for the message.

    The body of the message will not be included in the list.
    
    \sa appendAttachments(), clearAttachments()
*/

/*!
    \fn QMessage::appendAttachments(const QStringList &fileNames)
  
    Append the contents of the files specified by \a fileNames to the end of the list of 
    attachments for the message. The internet media (MIME) type of the attachments will be 
    determined by examining the files or file names.

    \sa attachmentIds(), clearAttachments()
*/

/*!
    \fn QMessage::clearAttachments()
  
    Clears the list of attachments for the message, leaving only the message body, if any.

    \sa attachmentIds(), appendAttachments()
*/  

/*!
    \fn QMessage::isModified() const
    
    Returns true if the message has been modified since it was constructed; 
    otherwise returns false.
*/

/*!
    \fn QMessage::createResponseMessage(ResponseType type) const

    Creates a new message as a response to this message, with properties predetermined according to \a type.
  
    \sa QMessageService::compose()
*/

/*!
    \fn QMessage::setPreferredCharsets(const QList<QByteArray> &charsetNames)
    
    Sets the ordered-by-preference list of names of charsets to use when encoding 
    unicode QString data to a serialized form to \a charsetNames.

    The set of valid charset names is returned by QTextCodec::availableCharsets().

    \sa preferredCharsets(), preferredCharsetFor(), setBody()
*/
void QMessage::setPreferredCharsets(const QList<QByteArray> &charsetNames)
{
    charsets = charsetNames;
}

/*!
    \fn QMessage::preferredCharsets()
    
    Returns an ordered-by-preference list of charset names to use when encoding 
    unicode QString data to a serialized form.

    \sa setPreferredCharsets(), preferredCharsetFor(), setBody()
*/
QList<QByteArray> QMessage::preferredCharsets()
{
    return charsets;
}

/*!
    Returns the first charset from the preferred list that is capable of encoding
    the content of \a text.

    \sa preferredCharsets(), setBody()
*/
QByteArray QMessage::preferredCharsetFor(const QString &text)
{
    QList<QTextCodec*> codecs;
    foreach (const QByteArray &name, charsets) {
        if (QTextCodec* codec = QTextCodec::codecForName(name)) {
            codecs.append(codec);
        } else {
            qWarning() << "No codec is available for:" << name;
        }
    }

    if (!codecs.isEmpty()) {
        // See if any of these codecs can encode the data
        QString::const_iterator sit = text.begin(), end = text.end();
        for ( ; sit != end; ++sit) {
            QList<QTextCodec*>::iterator cit = codecs.begin();
            if (!(*cit)->canEncode(*sit)) {
                // This codec is not acceptable
                cit = codecs.erase(cit);
                if (codecs.isEmpty()) {
                    break;
                }
            } else {
                ++cit;
            }
        }

        if (!codecs.isEmpty()) {
            // Return the first remaining codec
            return codecs.first()->name();
        }
    }

    return QByteArray();
}

QTM_END_NAMESPACE
