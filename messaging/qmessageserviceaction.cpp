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
#include "qmessageserviceaction.h"
#include "qmessageserviceaction_p.h"

/*!
    \class QMessageServiceAction

    \preliminary
    \ingroup messaging

    \brief The QMessageServiceAction class provides the interface for requesting 
    messaging services.

    QMessageServiceAction provides the mechanisms for messaging clients to request services, 
    and to receive information in response.  All actions present the same 
    interface for communicating status, and progress information.

    All actions communicate changes in their operational state by emitting the activityChanged()
    signal.

    Actions report progress information by emitting the progressChanged() signal.
    
    If an action operation fails after being initiated then the lastError() function will return 
    a value indicating the failure mode encountered.
    
    A client may attempt to cancel an operation after it has been initiated. The cancelOperation()
    slot is provided for this purpose.

    A QMessageServiceAction instance supports only a single request at any time. Attempting to
    initiate an operation on a QMessageServiceAction while another operation is already in
    progress will result in the later initiating function returning false. A client may, however,
    use multiple QMessageServiceAction instances to create a queue of requests that will be 
    performed sequentially.

    Each QMessageServiceAction instance will report only the changes pertaining to the request
    that instance delivers.
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
    \fn QMessageServiceAction::QMessageServiceAction(QObject *parent)
  
    Constructs a message service action object.
    
    The \a parent is passed to the QObject constructor.
*/

/*!
    \fn QMessageServiceAction::~QMessageServiceAction()
  
    Destroys the message service action.
*/

/*!
    \fn QMessageServiceAction::queryMessages(const QMessageFilterKey &key, const QMessageSortKey &sortKey, uint limit, uint offset) const
    
    Emits via messagesFound() signals \l{QMessageId}s of messages in the messaging 
    store. If \a key is not empty only identifiers for messages matching the parameters 
    set by \a key will be emitted, otherwise identifiers for all messages will be emitted.
    If \a sortKey is not empty, then the identifiers will be sorted by the parameters 
    set by \a sortKey.
    If \a limit is not zero, then \a limit places an upper bound on the number of 
    ids in the list returned.
    \a offset specifies how many ids to skip at the beginning of the list returned.
    
    Returns the number of progress steps required to perform the query if known;
    otherwise returns 0.
    
    Calling this function may result in the messagesFound() and progressChanged() 
    signals  being emitted multiple times. An emission of the progressChanged()
    signal with a total of 0 indicates that the number of progress steps is
    unknown.
    
    Returns true if the action can be initiated; otherwise returns false.
    
    \sa  QMessage, QMessageFilterKey, QMessageSortKey, messagesFound(), progressChanged()
*/

/*!
    \fn QMessageServiceAction::queryMessages(const QString &body, const QMessageFilterKey &key, const QMessageSortKey &sortKey, uint limit, uint offset) const
    
    Emits via the messagesFound() signal \l{QMessageId}s of messages in the messaging 
    store. If \a key is not empty only identifiers for messages matching the parameters 
    set by \a key and with a body containing the string \a body will be emitted, 
    otherwise identifiers for all messages with a body containing \a body will be emitted.
    If \a sortKey is not empty, then the identifiers will be sorted by the parameters 
    set by \a sortKey.
    If \a limit is not zero, then \a limit places an upper bound on the number of 
    ids in the list returned.
    \a offset specifies how many ids to skip at the beginning of the list returned.
     
    Returns the number of progress steps required to perform the query if known;
    otherwise returns 0.
    
    Calling this function may result in the messagesFound() and progressChanged() 
    signals being emitted multiple times. An emission of the progressChanged()
    signal with a total of 0 indicates that the number of progress steps is
    unknown.

    Returns true if the action can be initiated; otherwise returns false.
    
    \sa  QMessage, QMessageFilterKey, QMessageSortKey, messagesFound(), progressChanged()
*/

/*!
    \fn QMessageServiceAction::send(const QMessage &message, const QMessageAccountId &accountId)
  
    Transmit \a message using the account identified by \a accountId. 
    
    The message should be placed in the standard outbox folder for the account before sending, 
    and if the sending is successful moved to the sent folder for the account.
  
    Returns true if the action can be initiated; otherwise returns false.
    
    \sa QMessage, QMessageAccountId
*/

/*!
    \fn QMessageServiceAction::compose(const QMessage &message)
  
    Open a composer application using \a message as a prototype.
  
    The default application for handling the type of \a message should be used.
  
    Returns true if the action can be initiated; otherwise returns false.
    
    \sa QMessage::type()
*/

/*!
    \fn QMessageServiceAction::retrieveHeader(const QMessageId& id)
  
    Retrieve meta data of the message identified by \a id.  

    The meta data (including flags, from, to, subject, and date fields where applicable) of 
    the message identified by \a id should be retrieved.  If only the message envelope
    information is present, any other accessible header information should be retrieved.
    
    If the message can not be found on the originating server it will be marked as removed.

    Returns true if the action can be initiated; otherwise returns false.
    
    \sa QMessageId, QMessage::Removed
*/

/*!
    \fn QMessageServiceAction::retrieveBody(const QMessageId& id)
  
    Retrieve the body of the message identified by \a id.  

    If the message can not be found on the originating server it will be marked as removed.

    Returns true if the action can be initiated; otherwise returns false.
    
    \sa QMessageId, QMessage::Removed
*/

/*!
    \fn QMessageServiceAction::retrieve(const QMessageContentContainerId& id)
  
    Retrieve the container identified by \a id, the contents of the container should also be 
    retrieved.
    
    Returns true if the action can be initiated; otherwise returns false.
    
    \sa QMessageContentContainerId
*/

/*!
    \fn QMessageServiceAction::show(const QMessageId& id)
  
    Show the message identified by \a id.

    The default application for handling the type of message that \a id identifies should be used.

    Returns true if the action can be initiated; otherwise returns false.
    
    \sa QMessageId, QMessage::type()
*/
    
/*!
    \fn QMessageServiceAction::exportUpdates(const QMessageAccountId &id)
  
    Synchronize any changes that have been queued by message store operations with external servers.
    
    Pending changes to the account with identifier \a id should be synchronized.

    Returns true if the action can be initiated; otherwise returns false.
    
    \sa QMessageStore::addMessage(), QMessageStore::updateMessage(), QMessageStore::removeMessage(), QMessageStore::removeMessages()
*/
    
/*!
    \fn QMessageServiceAction::activity() const
  
    Returns the current activity state of the action.

    \sa activityChanged()
*/

/*!
    \fn QMessageServiceAction::cancelOperation()
  
    Attempts to cancel the last requested operation.
*/

/*!
    \fn QMessageServiceAction::activityChanged(QMessageServiceAction::Activity a)

    This signal is emitted when the activity status of the action changes,
    with the new state described by \a a.

    \sa activity()
*/

/*!
    \fn QMessageServiceAction::messagesFound(const QMessageIdList &ids);

    This signal is emitted when a queryMessages() operation has found
    messages.

    \a ids is the list of identifiers of messages found.

    \sa queryMessages()
*/

/*!
    \fn QMessageServiceAction::progressChanged(uint value, uint total)

    This signal is emitted when the action operation has progressed.

    \a total is the total number of progress steps to perform, or zero if
    the number of progress steps is unknown.

    \a value is the number of progress steps completed so far.
*/

/*!
    \fn QMessageServiceAction::lastError() const
  
    Returns a value indicating the last error condition encountered by the action.
*/

