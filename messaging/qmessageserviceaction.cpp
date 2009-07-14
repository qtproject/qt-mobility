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
    interface for communicating status, connectivity and progress information.

    All actions communicate changes in their operational state by emitting the activityChanged()
    signal.
    
    If an action operation fails, the lastErrorString() function will return a string
    indicating the failure mode encountered.  A successful operation will set the 
    lastError() result to a null string.
    
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
    \fn QMessageServiceAction::QMessageServiceAction(QObject *parent)
  
    Constructs a message service action object.
    
    The \a parent is passed to the QObject constructor.
*/

/*!
    \fn QMessageServiceAction::~QMessageServiceAction()
  
    Destroys the message service action.
*/

/*!
    \fn QMessageServiceAction::send(const QMessage &message, const QMessageAccountId &accountId)
  
    Transmit \a message using the account identified by \a accountId.
  
    \sa QMessage, QMessageAccountId
*/

/*!
    \fn QMessageServiceAction::compose(const QMessage &message)
  
    Open a composer application using \a message as a prototype.
  
    The default application for handling the type of \a message should be used.
  
    \sa QMessage::type()
*/

/*!
    \fn QMessageServiceAction::retrieve(const QMessageId& id)
  
    Retrieve meta data of the message identified by \a id.  

    The meta data (including flags, from, to, subject, and date fields where applicable) of 
    the message identified by \a id should be retrieved.
    
    If the message can not be found on the originating server it will be marked as removed.

    \sa QMessageId, QMessage::Removed

*/

/*!
    \fn QMessageServiceAction::retrieveBody(const QMessageId& id)
  
    Retrieve the body of the message identified by \a id.  

    If the message can not be found on the originating server it will be marked as removed.

    \sa QMessageId, QMessage::Removed

*/

/*!
    \fn QMessageServiceAction::retrieve(const QMessageContentContainerId& id)
  
    Retrieve the container identified by \a id, the contents of the container should also be 
    retrieved.
    
    \sa QMessageContentContainerId
*/

/*!
    \fn QMessageServiceAction::show(const QMessageId& id)
  
    Show the message identified by \a id.

    The default application for handling the type of message that \a id identifies should be used.

    \sa QMessageId, QMessage::type()
*/
    
/*!
    \fn QMessageServiceAction::exportUpdates()
  
    Synchronize any changes that have been queued by message store operations with external servers.

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
    \fn QMessageServiceAction::lastErrorString() const
  
    Returns a string indicating the last error condition reported by the action if any; 
    otherwise returns a null string.
*/

