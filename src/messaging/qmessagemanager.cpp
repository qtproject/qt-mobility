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
#include "qmessagemanager.h"
#include "qmessagestore.h"
#include <QDebug>


QTM_BEGIN_NAMESPACE

/*!
    \class QMessageManager

    \preliminary
    \brief The QMessageManager class represents the main interface for storage and retrieval
    of messages, folders and accounts in the system message store.

    \ingroup messaging

    The system contains a single store for messages, which is accessed via the handle
    class QMessageManager.  QMessageManager provides the interface for adding, updating 
    and deleting messages in the system's message store. 
    
    QMessageManager provides the countFolders() and queryFolders() functions for
    counting and listing the folders contained by the messaging store, and the 
    countAccounts() and queryAccounts() functions for counting and listing the
    accounts contained by the store.  These functions use the QMessageFolderFilter
    and QMessageFolderSortOrder classes, and the QMessageAccountFilter and 
    QMessageAccountSortOrder classes to constrain their searches.

    QMessageManager also implements functionality allowing the messages contained by the 
    store to be counted or listed, using various filtering and sortOrder constraints.
    Clients can access this functionality via the \l{QMessageService::countMessages()}{countMessages}
    and \l{QMessageService::queryMessages()}{queryMessages} functions of the
    QMessageService class.
    
    With the exception of Windows mobile and desktop platforms, QMessageManager functions 
    should not initiate network activity. Instead functions are restricted to operating 
    on data already on the device. See QMessageService for functions related to 
    initiating network activity.

    If a QMessageManager operation fails, the error() function will return an error code
    value indicating the failure mode encountered.  A successful operation will set the 
    error() result to QMessageManager::NoError.

    Messages in the messaging store are identified by QMessageId objects. The data associated 
    with a message is retrieved in the form of a QMessage object using message(). Likewise 
    a folder is retrieved in the form of a QMessageFolder object using folder(), and an 
    account is retrieved in the form of a QMessageAccount object using account().

    Messages can be inserted into the store using the addMessage() function, messages in the 
    store can be manipulated via the updateMessage() function, and removed by the 
    removeMessage() functions.
    
    Messaging store manipulations involving messages are reported via the messagesAdded(), 
    messagesUpdated() and messagesRemoved() signals.

    \sa QMessage, QMessageId, QMessageContentContainerId, QMessageService
*/

/*!
    \enum QMessageManager::RemovalOption

    Defines whether or not a message will be removed from the originating server.

    \value RemoveLocalCopyOnly        Do not remove the message from the originating server.
    \value RemoveOnOriginatingServer  Remove the message both from the local store and from the originating server if any.
*/

/*!
    \typedef QMessageManager::NotificationFilterId

    This type contains a value identifying a registered message filter.
*/

/*!
    \typedef QMessageManager::NotificationFilterIdSet

    This type contains a set of values identifying registered message filters.
*/

/*!
    \enum QMessageManager::Error

    Defines the result of attempting to perform a messaging store operation.

    \value NoError                The operation was successfully performed.
    \value InvalidId              The operation failed due to the specification of an invalid identifier.
    \value ConstraintFailure      The operation failed due to a constraint violation.
    \value ContentInaccessible    The operation failed because the content data cannot be accessed by the messaging store.
    \value NotYetImplemented      The operation failed because the messaging store does not yet implement the operation.
    \value FrameworkFault         The operation failed because the messaging store encountered an error in performing the operation.
    \value WorkingMemoryOverflow  The operation failed because the messaging store exhausted all memory available for evaluating queries.
    \value Busy                   The operation failed because the messaging store is being used by another thread.
    \value RequestIncomplete      The operation failed to report successful completion, although no specific error was reported.
*/

/*!
    \fn QMessageManager::QMessageManager(QObject *parent)
    
    Constructs a handle to the message store.
    
    If \a parent is not 0 then the handle will be deleted when \a parent is deleted.
*/
QMessageManager::QMessageManager(QObject *parent)
    : QObject(parent),
      store(QMessageStore::instance())
{
    connect(store, SIGNAL(messageAdded(QMessageId, QMessageManager::NotificationFilterIdSet)), this, SIGNAL(messageAdded(QMessageId, QMessageManager::NotificationFilterIdSet)));
    connect(store, SIGNAL(messageRemoved(QMessageId, QMessageManager::NotificationFilterIdSet)), this, SIGNAL(messageRemoved(QMessageId, QMessageManager::NotificationFilterIdSet)));
    connect(store, SIGNAL(messageUpdated(QMessageId, QMessageManager::NotificationFilterIdSet)), this, SIGNAL(messageUpdated(QMessageId, QMessageManager::NotificationFilterIdSet)));
}

/*!
    \fn QMessageManager::~QMessageManager()
    
    Destroys the handle to the message store.
*/
QMessageManager::~QMessageManager()
{
}

/*!
    \fn QMessageManager::error() const
    
    Returns the code of the last error condition reported by the messaging store.
*/
QMessageManager::Error QMessageManager::error() const
{
    return store->error();
}

/*!
    \fn QMessageManager::queryMessages(const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
    
    Returns the \l{QMessageId}s of messages in the messaging store. If \a filter is not empty 
    only messages matching the parameters set by \a filter will be returned, otherwise 
    identifiers for all messages will be returned.
    If \a sortOrder is not empty, then the identifiers will be sorted by the parameters 
    set by \a sortOrder.
    If \a limit is not zero, then \a limit places an upper bound on the number of 
    ids in the list returned.
    \a offset specifies how many ids to skip at the beginning of the list returned.
    
    The performance of querying messages is currently significantly less than 
    optimal for some querying criteria on some platforms.
    
    \sa error(), countMessages()
*/
QMessageIdList QMessageManager::queryMessages(const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    return store->queryMessages(filter, sortOrder, limit, offset);
}

/*!
    \fn QMessageManager::queryMessages(const QMessageFilter &filter, const QList<QMessageSortOrder> &sortOrders, uint limit, uint offset) const
    
    Returns the \l{QMessageId}s of messages in the messaging store. If \a filter is not empty 
    only messages matching the parameters set by \a filter will be returned, otherwise 
    identifiers for all messages will be returned.
    If \a sortOrders is not empty, then the identifiers will be sorted by applying each
    sort order element in sequence.
    If \a limit is not zero, then \a limit places an upper bound on the number of 
    ids in the list returned.
    \a offset specifies how many ids to skip at the beginning of the list returned.
    
    The performance of querying messages is currently significantly less than 
    optimal for some querying criteria on some platforms.
    
    \sa error(), countMessages()
*/
QMessageIdList QMessageManager::queryMessages(const QMessageFilter &filter, const QList<QMessageSortOrder> &sortOrders, uint limit, uint offset) const
{
    // Collapse the list of sort orders down to single sort object
    QMessageSortOrder cumulativeOrder;
    if (!sortOrders.isEmpty()) {
        foreach (const QMessageSortOrder &sort, sortOrders) {
            cumulativeOrder += sort;
        }
    }

    return store->queryMessages(filter, cumulativeOrder, limit, offset);
}

/*!
    \fn QMessageManager::queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
    
    Returns the \l{QMessageId}s of messages in the messaging store. If \a filter is not empty 
    only messages matching the parameters set by \a filter and with a body containing the 
    string \a body will be returned, otherwise identifiers for all messages with 
    a body containing \a body will be returned.
    If \a sortOrder is not empty, then the identifiers will be sorted by the parameters 
    set by \a sortOrder.
    If \a limit is not zero, then \a limit places an upper bound on the number of 
    ids in the list returned.
    \a offset specifies how many ids to skip at the beginning of the list returned.
    \a matchFlags specifies the matching method to use.
    
    The performance of querying messages is currently significantly less than 
    optimal for some querying criteria on some platforms.
    
    \sa error(), countMessages()
*/
QMessageIdList QMessageManager::queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    return store->queryMessages(filter, body, matchFlags, sortOrder, limit, offset);
}

/*!
    \fn QMessageManager::queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QList<QMessageSortOrder> &sortOrders, uint limit, uint offset) const
    
    Returns the \l{QMessageId}s of messages in the messaging store. If \a filter is not empty 
    only messages matching the parameters set by \a filter and with a body containing the 
    string \a body will be returned, otherwise identifiers for all messages with 
    a body containing \a body will be returned.
    If \a sortOrders is not empty, then the identifiers will be sorted by applying each
    sort order element in sequence.
    If \a limit is not zero, then \a limit places an upper bound on the number of 
    ids in the list returned.
    \a offset specifies how many ids to skip at the beginning of the list returned.
    \a matchFlags specifies the matching method to use.
    
    The performance of querying messages is currently significantly less than 
    optimal for some querying criteria on some platforms.
    
    \sa error(), countMessages()
*/
QMessageIdList QMessageManager::queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QList<QMessageSortOrder> &sortOrders, uint limit, uint offset) const
{
    // Collapse the list of sort orders down to single sort object
    QMessageSortOrder cumulativeOrder;
    if (!sortOrders.isEmpty()) {
        foreach (const QMessageSortOrder &sort, sortOrders) {
            cumulativeOrder += sort;
        }
    }

    return store->queryMessages(filter, body, matchFlags, cumulativeOrder, limit, offset);
}

/*!
    \fn QMessageManager::queryFolders(const QMessageFolderFilter &filter, const QMessageFolderSortOrder &sortOrder, uint limit, uint offset) const
    
    Returns the \l{QMessageFolderId}s of folders in the messaging store. If \a filter 
    is not empty only folders matching the parameters set by \a filter will be returned,
    otherwise identifiers for all folders will be returned.
    If \a sortOrder is not empty, then the identifiers will be sorted by the parameters 
    set by \a sortOrder.
    If \a limit is not zero, then \a limit places an upper bound on the number of 
    ids in the list returned.
    \a offset specifies how many ids to skip at the beginning of the list returned.
    
    \sa error(), countFolders()
*/
QMessageFolderIdList QMessageManager::queryFolders(const QMessageFolderFilter &filter, const QMessageFolderSortOrder &sortOrder, uint limit, uint offset) const
{
    return store->queryFolders(filter, sortOrder, limit, offset);
}

/*!
    \fn QMessageManager::queryFolders(const QMessageFolderFilter &filter, const QList<QMessageFolderSortOrder> &sortOrders, uint limit, uint offset) const
    
    Returns the \l{QMessageFolderId}s of folders in the messaging store. If \a filter 
    is not empty only folders matching the parameters set by \a filter will be returned,
    otherwise identifiers for all folders will be returned.
    If \a sortOrders is not empty, then the identifiers will be sorted by applying each
    sort order element in sequence.
    If \a limit is not zero, then \a limit places an upper bound on the number of 
    ids in the list returned.
    \a offset specifies how many ids to skip at the beginning of the list returned.
    
    \sa error(), countFolders()
*/
QMessageFolderIdList QMessageManager::queryFolders(const QMessageFolderFilter &filter, const QList<QMessageFolderSortOrder> &sortOrders, uint limit, uint offset) const
{
    // Collapse the list of sort orders down to single sort object
    QMessageFolderSortOrder cumulativeOrder;
    if (!sortOrders.isEmpty()) {
        foreach (const QMessageFolderSortOrder &sort, sortOrders) {
            cumulativeOrder += sort;
        }
    }

    return store->queryFolders(filter, cumulativeOrder, limit, offset);
}

/*!
    \fn QMessageManager::queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountSortOrder &sortOrder, uint limit, uint offset) const
    
    Returns the \l{QMessageAccountId}s of accounts in the messaging store. If \a filter 
    is not empty only accounts matching the parameters set by \a filter will be returned, 
    otherwise identifiers for all accounts will be returned.
    If \a sortOrder is not empty, then the identifiers will be sorted by the parameters 
    set by \a sortOrder.
    If \a limit is not zero, then \a limit places an upper bound on the number of 
    ids in the list returned.
    \a offset specifies how many ids to skip at the beginning of the list returned.
    
    \sa error(), countAccounts()
*/
QMessageAccountIdList QMessageManager::queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountSortOrder &sortOrder, uint limit, uint offset) const
{
    return store->queryAccounts(filter, sortOrder, limit, offset);
}

/*!
    \fn QMessageManager::queryAccounts(const QMessageAccountFilter &filter, const QList<QMessageAccountSortOrder> &sortOrders, uint limit, uint offset) const
    
    Returns the \l{QMessageAccountId}s of accounts in the messaging store. If \a filter 
    is not empty only accounts matching the parameters set by \a filter will be returned, 
    otherwise identifiers for all accounts will be returned.
    If \a sortOrders is not empty, then the identifiers will be sorted by applying each
    sort order element in sequence.
    If \a limit is not zero, then \a limit places an upper bound on the number of 
    ids in the list returned.
    \a offset specifies how many ids to skip at the beginning of the list returned.
    
    \sa error(), countAccounts()
*/
QMessageAccountIdList QMessageManager::queryAccounts(const QMessageAccountFilter &filter, const QList<QMessageAccountSortOrder> &sortOrders, uint limit, uint offset) const
{
    // Currently, we do not support multiple account sort orderings
    QMessageAccountSortOrder cumulativeOrder;
    if (!sortOrders.isEmpty()) {
        cumulativeOrder = sortOrders.first();
    }

    return store->queryAccounts(filter, cumulativeOrder, limit, offset);
}

/*!
    \fn QMessageManager::countMessages(const QMessageFilter& filter) const
    
    Returns the number of messages which match the filtering criteria defined 
    in QMessageFilter \a filter. If \a filter is empty the count of all 
    available messages is returned.
    
    The performance of counting messages is currently significantly less than optimal 
    for some filters on some platforms.
    
    \sa error(), queryMessages()
*/
int QMessageManager::countMessages(const QMessageFilter& filter) const
{
    return store->countMessages(filter);
}

/*!
    \fn QMessageManager::countFolders(const QMessageFolderFilter& filter) const
    
    Returns the number of folders which match the filtering criteria defined 
    in QMessageFolderFilter \a filter. If \a filter is empty the count of all 
    available folders is returned.
    
    \sa error(), queryFolders()
*/
int QMessageManager::countFolders(const QMessageFolderFilter& filter) const
{
    return store->countFolders(filter);
}

/*!
    \fn QMessageManager::countAccounts(const QMessageAccountFilter& filter) const
    
    Returns the number of messages which match the filtering criteria defined 
    in QMessageAccountFilter \a filter. If \a filter is empty the count of all 
    available accounts is returned.
    
    \sa error(), queryAccounts()
*/
int QMessageManager::countAccounts(const QMessageAccountFilter& filter) const
{
    return store->countAccounts(filter);
}

/*!
    \fn QMessageManager::removeMessage(const QMessageId& id, RemovalOption option)
    
    Removes the message with QMessageId \a id from the messaging store. If \a option is 
    QMessageManager::RemoveOnOriginatingServer then when synchronization is performed the 
    message should be removed from both the local message store and the originating server  
    if any.

    Returns \c true if the operation successfully updates the store; otherwise returns \c false.
    
    To ensure the change is propagated to any affected external server
    QMessageService::exportUpdates() should be subsequently called.
    
    \a option is ignored on Windows mobile and desktop platforms.

    \sa removeMessages(), addMessage(), updateMessage(), QMessageService::exportUpdates()
*/
bool QMessageManager::removeMessage(const QMessageId& id, QMessageManager::RemovalOption option)
{
    return store->removeMessage(id, option);
}

/*!
    \fn QMessageManager::removeMessages(const QMessageFilter& filter, QMessageManager::RemovalOption option)
    
    Removes all messages identified by the filter \a filter from the messaging store.
    If \a option is QMessageManager::RemoveOnOriginatingServer then when synchronization is performed the 
    messages should be removed from both the local message store and the originating server if any.

    Returns \c true if the operation successfully updates the store; otherwise returns \c false. 
    
    To ensure the change is propagated to any affected external server
    QMessageService::exportUpdates() should be subsequently called.

    \a option is ignored on Windows mobile and desktop platforms.

    
    For example:

    To implement a function to remove a list messages identified by QMessageIds
    from the messaging store.
    \code
    bool removeMessages(const QMessageIdList& ids)
    {
        QMessageFilter idsFilter(ids);
        return QMessageManager().removeMessages(idsFilter);
    }
    \endcode

    \sa removeMessage(), addMessage(), updateMessage(), QMessageService::exportUpdates()
*/
bool QMessageManager::removeMessages(const QMessageFilter& filter, QMessageManager::RemovalOption option)
{
    return store->removeMessages(filter, option);
}

/*!
    \fn QMessageManager::addMessage(QMessage *message)
    
    Adds a copy of the message indicated by \a message to the messaging store 
    and modifies the message indicated by \a message to contain the identifier 
    of the created message.

    Returns \c true if the operation successfully updates the store; otherwise returns \c false. 
    
    To ensure the change is propagated to any affected external server
    QMessageService::exportUpdates() should be subsequently called.
    
    On the Maemo 5 (Fremantle) platform for SMS type messages this function is not yet 
    supported.
    
    Using this function to explicitly set a size or date is not currently supported on some platforms.
    
    \sa message(), updateMessage(), removeMessage(), QMessageService::exportUpdates()
*/
bool QMessageManager::addMessage(QMessage *m)
{
    return store->addMessage(m);
}

/*!
    \fn QMessageManager::updateMessage(QMessage *message)
    
    Updates the messaging store so that the message whose identifier is contained 
    by the message at \a message contains the content at \a message. If \a message
    does not contain a valid identifier, no changes will result.  Internal data
    of the QMessage object at \a message can be modified by this operation.
    
    Returns \c true if the operation successfully updates the store; otherwise returns \c false. 

    To ensure the change is propagated to any affected external server 
    QMessageService::exportUpdates() should be subsequently called.

    Using this function to explicitly set a size or date is not currently supported on some platforms.
    
    On the Maemo 5 (Fremantle) platform this function is not yet supported.
    
    \sa addMessage(), removeMessage(), QMessageService::exportUpdates()
*/
bool QMessageManager::updateMessage(QMessage *m)
{
    return store->updateMessage(m);
}

/*!
    \fn QMessageManager::message(const QMessageId& id) const
    
    Returns the QMessage identified by \a id from the store.
*/
QMessage QMessageManager::message(const QMessageId& id) const
{
    return store->message(id);
}

/*!
    \fn QMessageManager::folder(const QMessageFolderId& id) const
    
    Returns the QMessageFolder identified by \a id from the store.
*/
QMessageFolder QMessageManager::folder(const QMessageFolderId& id) const
{
    return store->folder(id);
}

/*!
    \fn QMessageManager::account(const QMessageAccountId& id) const
    
    Returns the QMessageAccount identified by \a id from the store.
*/
QMessageAccount QMessageManager::account(const QMessageAccountId& id) const
{
    return store->account(id);
}

/*!
    \fn NotificationFilterId QMessageManager::registerNotificationFilter(const QMessageFilter &filter);

    Registers a message filter that will be used to generate messageAdded(), messageRemoved()
    and messageUpdated() signals.  Returns an identifier value that can be used to identify the 
    reason that a signal was emitted, and to unregister the filter at a later time.

    The \a filter is applied to the state of the data after the occurrence of the event for which 
    a notification may be emitted.

    \sa unregisterNotificationFilter(), messageAdded(), messageRemoved(), messageUpdated()
*/
QMessageManager::NotificationFilterId QMessageManager::registerNotificationFilter(const QMessageFilter &filter)
{
    return store->registerNotificationFilter(filter);
}

/*!
    \fn void QMessageManager::unregisterNotificationFilter(NotificationFilterId notificationFilterId);

    Removes the message filter associated with \a notificationFilterId from the set used 
    to generate message event signals.

    \sa registerNotificationFilter(), messageAdded(), messageRemoved(), messageUpdated()
*/
void QMessageManager::unregisterNotificationFilter(NotificationFilterId notificationFilterId)
{
    store->unregisterNotificationFilter(notificationFilterId);
}

/*!
    \fn void QMessageManager::messageAdded(const QMessageId &id, const QMessageManager::NotificationFilterIdSet &matchingFilterIds);

    Signal that is emitted when the message identified by \a id is added to the message store.
    \a matchingFilterIds contains a set of values identifiying registered notification filters 
    that matched the message.
    
    Currently on the Maemo 5 (Fremantle) platform this signal is not reported for messages in the 
    inbox when the inbox is open in modest.

    \sa messageRemoved(), messageUpdated(), registerNotificationFilter()
*/

/*!
    \fn void QMessageManager::messageRemoved(const QMessageId &id, const QMessageManager::NotificationFilterIdSet &matchingFilterIds);

    Signal that is emitted when the message identified by \a id is removed from the message store.
    \a matchingFilterIds contains a set of values identifiying registered notification filters 
    that matched the message.

    Since the filters apply to the state of the data after the message removal, the only 
    data item that may be subject to filtration is the identifier of the removed message.

    \sa messageAdded(), messageUpdated(), registerNotificationFilter()
*/

/*!
    \fn void QMessageManager::messageUpdated(const QMessageId &id, const QMessageManager::NotificationFilterIdSet &matchingFilterIds);

    Signal that is emitted when the message identified by \a id is updated in the message store.
    \a matchingFilterIds contains a set of values identifiying registered notification filters 
    that matched the message.

    Since the filters apply to the state of the data after the message modification, updates 
    to messages which matched a given filter prior to modification but not afterwards will not 
    result in the emission of the messageUpdated signal.

    \sa messageAdded(), messageRemoved(), registerNotificationFilter()
*/

static const int registrationId1 = qRegisterMetaType<QMessageManager::NotificationFilterId>();
static const int registrationId2 = qRegisterMetaType<QMessageManager::NotificationFilterIdSet>();

#include "moc_qmessagemanager.cpp"

QTM_END_NAMESPACE
