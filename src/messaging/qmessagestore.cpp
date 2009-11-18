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
#include "qmessagestore.h"
#include "qmessagestore_p.h"


QTM_BEGIN_NAMESPACE

/*!
    \class QMessageStore

    \preliminary
    \brief The QMessageStore class represents the main interface for storage and retrieval
    of messages, folders and accounts in the messaging store.

    \ingroup messaging

    The QMessageStore class is accessed through a singleton interface and provides functions 
    for adding, updating and deleting messages in the messaging store. 
    
    QMessageStore provides the countFolders() and queryFolders() functions for
    counting and listing the folders contained by the messaging store, and the 
    countAccounts() and queryAccounts() functions for counting and listing the
    accounts contained by the store.  These functions use the QMessageFolderFilter
    and QMessageFolderOrdering classes, and the QMessageAccountFilter and 
    QMessageAccountOrdering classes to constrain their searches.

    QMessageStore also implements functionality allowing the messages contained by the 
    store to be counted or listed, using various filtering and ordering constraints.
    Clients can access this functionality via the \l{QMessageServiceAction::countMessages()}{countMessages}
    and \l{QMessageServiceAction::queryMessages()}{queryMessages} functions of the
    QMessageServiceAction class.
    
    With the exception of MAPI based platforms, QMessageStore functions should not initiate 
    network activity. Instead they are restricted to operating on data already on the device.
    See QMessageServiceAction for functions related to initiating network activity.

    If a QMessageStore operation fails, the lastError() function will return an error code
    value indicating the failure mode encountered.  A successful operation will set the 
    lastError() result to QMessageStore::NoError.

    Messages in the messaging store are identified by QMessageId objects. The data associated 
    with a message is retrieved in the form of a QMessage object using message(). Likewise 
    a folder is retrieved in the form of a QMessageFolder object using folder(), and an 
    account is retrieved in the form of a QMessageAccount object using account().

    Messages can be inserted into the store using the addMessage() function, messages in the 
    store can be manipulated via the updateMessage() function ,and removed by the 
    removeMessage() functions.
    
    Messaging store manipulations involving messages are reported via the messagesAdded(), 
    messagesUpdated() and messagesRemoved() signals.

    \sa QMessage, QMessageId, QMessageContentContainerId, QMessageServiceAction
*/

/*!
    \enum QMessageStore::RemovalOption

    Defines whether or not a message will be removed from the originating server.

    \value RemoveLocalCopyOnly        Do not remove the message from the originating server.
    \value RemoveOnOriginatingServer  Remove the message both from the local store and from the originating server if any.
*/

/*!
    \typedef QMessageStore::NotificationFilterId

    This type contains a value identifying a registered message filter.
*/

/*!
    \typedef QMessageStore::NotificationFilterIdSet

    This type contains a set of values identifying registered message filters.
*/

/*
    TODO capabilities AtomicBooleanSearch AtomicSlices Sms Mms Email Xmpp Presence AtomicExtendedSearching AtomicBodySearching
      ExtendedServices AtomicCustomSearching CaseInsensitiveSearching FullWordSearching
      
    Maybe capabilities are not necessary.
      Sms, Mms, Email, Xmpp are only enabled if a valid default account exists for that type.
    All other capabilities are searching related, lack of support for them can be detected by a QMailStore::lastError of NotYetImplemented.

    Activate/deactive on only adds, deletes or updates? Requires enum,
    enum NotificationType 
    {
        Add = 1,
        Removed,
        Changed
    }
    
    MAPI has move and copy signals, QMF does not.
*/

/*!
    \enum QMessageStore::ErrorCode

    Defines the result of attempting to perform a messaging store operation.

    \value NoError                The operation was successfully performed.
    \value InvalidId              The operation failed due to the specification of an invalid identifier.
    \value ConstraintFailure      The operation failed due to a constraint violation.
    \value ContentInaccessible    The operation failed because the content data cannot be accessed by the messaging store.
    \value NotYetImplemented      The operation failed because the messaging store does not yet implement the operation.
    \value FrameworkFault         The operation failed because the messaging store encountered an error in performing the operation.
    \value WorkingMemoryOverflow  The operation failed because the messaging store exhausted all memory available for evaluating queries.
*/

/*!
    \fn QMessageStore::QMessageStore(QObject *parent)
    
    Constructs the messaging store.
    
    If \a parent is not 0 then the messaging store will be deleted when \a parent is deleted.
    
    \sa instance()
*/

/*!
    \fn QMessageStore::~QMessageStore()
    
    Destroys the messaging store.
*/

/*!
    \fn QMessageStore::lastError() const
    
    Returns the code of the last error condition reported by the messaging store.
*/

/*!
    \fn QMessageStore::queryMessages(const QMessageFilter &filter, const QMessageOrdering &ordering, uint limit, uint offset) const
    
    Returns the \l{QMessageId}s of messages in the messaging store. If \a filter is not empty 
    only messages matching the parameters set by \a filter will be returned, otherwise 
    identifiers for all messages will be returned.
    If \a ordering is not empty, then the identifiers will be sorted by the parameters 
    set by \a ordering.
    If \a limit is not zero, then \a limit places an upper bound on the number of 
    ids in the list returned.
    \a offset specifies how many ids to skip at the beginning of the list returned.
    
    \sa lastError(), countMessages()
*/

/*!
    \fn QMessageStore::queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::Options options, const QMessageOrdering &ordering, uint limit, uint offset) const
    
    Returns the \l{QMessageId}s of messages in the messaging store. If \a filter is not empty 
    only messages matching the parameters set by \a filter and with a body containing the 
    string \a body will be returned, otherwise identifiers for all messages with 
    a body containing \a body will be returned.
    If \a ordering is not empty, then the identifiers will be sorted by the parameters 
    set by \a ordering.
    If \a limit is not zero, then \a limit places an upper bound on the number of 
    ids in the list returned.
    \a offset specifies how many ids to skip at the beginning of the list returned.
    \a options specifies the search optons to use.
    
    \sa lastError(), countMessages()
*/

/*!
    \fn QMessageStore::queryFolders(const QMessageFolderFilter &filter, const QMessageFolderOrdering &ordering, uint limit, uint offset) const
    
    Returns the \l{QMessageFolderId}s of folders in the messaging store. If \a filter 
    is not empty only folders matching the parameters set by \a filter will be returned,
    otherwise identifiers for all folders will be returned.
    If \a ordering is not empty, then the identifiers will be sorted by the parameters 
    set by \a ordering.
    If \a limit is not zero, then \a limit places an upper bound on the number of 
    ids in the list returned.
    \a offset specifies how many ids to skip at the beginning of the list returned.
    
    \sa lastError(), countFolders()
*/

/*!
    \fn QMessageStore::queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountOrdering &ordering, uint limit, uint offset) const
    
    Returns the \l{QMessageAccountId}s of accounts in the messaging store. If \a filter 
    is not empty only accounts matching the parameters set by \a filter will be returned, 
    otherwise identifiers for all accounts will be returned.
    If \a ordering is not empty, then the identifiers will be sorted by the parameters 
    set by \a ordering.
    If \a limit is not zero, then \a limit places an upper bound on the number of 
    ids in the list returned.
    \a offset specifies how many ids to skip at the beginning of the list returned.
    
    \sa lastError(), countAccounts()
*/

/*!
    \fn QMessageStore::countMessages(const QMessageFilter& filter) const
    
    Returns the number of messages which match the 
    filtering criteria defined in QMessageFilter \a filter. If 
    filter is empty a count of all messages is returned.
    
    \sa lastError(), queryMessages()
*/

/*!
    \fn QMessageStore::countFolders(const QMessageFolderFilter& filter) const
    
    Returns the number of messages which match the 
    filtering criteria defined in QMessageFolderFilter \a filter. If 
    filter is empty a count of all messages is returned.
    
    \sa lastError(), queryFolders()
*/

/*!
    \fn QMessageStore::countAccounts(const QMessageAccountFilter& filter) const
    
    Returns the number of messages which match the 
    filtering criteria defined in QMessageAccountFilter \a filter. If 
    filter is empty a count of all messages is returned.
    
    \sa lastError(), queryAccounts()
*/

/*!
    \fn QMessageStore::removeMessage(const QMessageId& id, RemovalOption option)
    
    Removes the message with QMessageId \a id from the messaging store. If \a option is 
    QMessageStore::RemoveOnOriginatingServer then when synchronization is performed the 
    message should be removed from both the local message store and the originating server  
    if any.

    Returns \c true if the operation successfully updates the store; otherwise returns \c false.
    
    To ensure the change is propagated to any affected external server
    QMessageServiceAction::exportUpdates() should be subsequently called.

    \sa removeMessages(), addMessage(), updateMessage(), QMessageServiceAction::exportUpdates()
*/

/*!
    \fn QMessageStore::removeMessages(const QMessageFilter& filter, QMessageStore::RemovalOption option)
    
    Removes all messages identified by the filter \a filter from the messaging store.
    If \a option is QMessageStore::RemoveOnOriginatingServer then when synchronization is performed the 
    messages should be removed from both the local message store and the originating server if any.

    Returns \c true if the operation successfully updates the store; otherwise returns \c false. 
    
    To ensure the change is propagated to any affected external server
    QMessageServiceAction::exportUpdates() should be subsequently called.


    For example:

    To implement a function to remove a list messages identified by QMessageIds
    from the messaging store.
    \code
    bool removeMessages(const QMessageIdList& ids)
    {
        QMessageFilter idsFilter(ids);
        return QMessageStore::instance()->removeMessages(idsFilter);
    }
    \endcode

    \sa removeMessage(), addMessage(), updateMessage(), QMessageServiceAction::exportUpdates()
*/

/*!
    \fn QMessageStore::addMessage(QMessage *m)
    
    Updates the existing QMessage \a m on the messaging store.
    Returns \c true if the operation successfully updates the store; otherwise returns \c false. 
    
    To ensure the change is propagated to any affected external server
    QMessageServiceAction::exportUpdates() should be subsequently called.

    \sa message(), updateMessage(), removeMessage(), QMessageServiceAction::exportUpdates()
*/

/*!
    \fn QMessageStore::updateMessage(QMessage *m)
    
    Updates the existing QMessage \a m on the messaging store.
    Returns \c true if the operation successfully updates the store; otherwise returns \c false. 

    To ensure the change is propagated to any affected external server 
    QMessageServiceAction::exportUpdates() should be subsequently called.
    
    \sa addMessage(), removeMessage(), QMessageServiceAction::exportUpdates()
*/

/*!
    \fn QMessageStore::message(const QMessageId& id) const
    
   Returns the QMessage identified by \a id from the store.
*/

/*!
    \fn QMessageStore::folder(const QMessageFolderId& id) const
    
   Returns the QMessageFolder identified by \a id from the store.
*/

/*!
    \fn QMessageStore::account(const QMessageAccountId& id) const
    
   Returns the QMessageAccount identified by \a id from the store.
*/

/*!
    \fn NotificationFilterId QMessageStore::registerNotificationFilter(const QMessageFilter &filter);

    Registers a message filter that will be used to generate messageAdded(), messageRemoved()
    and messageUpdated() signals.  Returns an identifier value that can be used to identify the 
    reason that a signal was emitted, and to unregister the filter at a later time.

    The \a filter is applied to the state of the data after the occurrence of the event for which 
    a notification may be emitted.

    \sa unregisterNotificationFilter(), messageAdded(), messageRemoved(), messageUpdated()
*/

/*!
    \fn void QMessageStore::unregisterNotificationFilter(NotificationFilterId notificationFilterId);

    Removes the message filter associated with \a notificationFilterId from the set used 
    to generate message event signals.

    \sa registerNotificationFilter(), messageAdded(), messageRemoved(), messageUpdated()
*/

/*!
    \fn QMessageStore::instance()
    
    Returns the single instance of the QMessageStore class.

    If necessary, the store will be instantiated and initialized.
*/

/*!
    \fn void QMessageStore::messageAdded(const QMessageId &id, const QMessageStore::NotificationFilterIdSet &matchingFilterIds);

    Signal that is emitted when the message identified by \a id is added to the message store.
    \a matchingFilterIds contains a set of values identifiying registered notification filters 
    that matched the message.

    \sa messageRemoved(), messageUpdated(), registerNotificationFilter()
*/

/*!
    \fn void QMessageStore::messageRemoved(const QMessageId &id, const QMessageStore::NotificationFilterIdSet &matchingFilterIds);

    Signal that is emitted when the message identified by \a id is removed from the message store.
    \a matchingFilterIds contains a set of values identifiying registered notification filters 
    that matched the message.

    Since the filters apply to the state of the data after the message removal, the only 
    data item that may be subject to filtration is the identifier of the removed message.

    \sa messageAdded(), messageUpdated(), registerNotificationFilter()
*/

/*!
    \fn void QMessageStore::messageUpdated(const QMessageId &id, const QMessageStore::NotificationFilterIdSet &matchingFilterIds);

    Signal that is emitted when the message identified by \a id is updated in the message store.
    \a matchingFilterIds contains a set of values identifiying registered notification filters 
    that matched the message.

    Since the filters apply to the state of the data after the message modification, updates 
    to messages which matched a given filter prior to modification but not afterwards will not 
    result in the emission of the messageUpdated signal.

    \sa messageAdded(), messageRemoved(), registerNotificationFilter()
*/

static const int registrationId1 = qRegisterMetaType<QMessageStore::NotificationFilterId>();
static const int registrationId2 = qRegisterMetaType<QMessageStore::NotificationFilterIdSet>();

QTM_END_NAMESPACE
