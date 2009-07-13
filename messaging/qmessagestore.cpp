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
#include "qmessagestore.h"
#include "qmessagestore_p.h"

/*!
    \class QMessageStore

    \preliminary
    \brief The QMessageStore class represents the main interface for storage and retrieval
    of messages, folders and accounts in the messaging store.

    \ingroup messaging

    The QMessageStore class is accessed through a singleton interface and provides functions 
    for adding, updating and deleting messages in the messaging store. 
    
    QMessageStore also provides queryMessages() and countMessages() functions for querying 
    and counting of QMessage, QMessageFolder and QMessageAccount objects by using the
    QMessageFilterKey, QMessageFolderFilterKey, QMessageAccountFilterKey classes 
    respectively.
    
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

    \sa QMessage, QMessageId, QMessageContentContainerId, QMessageFilterKey, QMessageSortKey
*/

/*!
    \enum QMessageStore::RemovalOption

    Defines whether or not a message will be removed from the originating server.

    \value RemoveLocalCopyOnly        Do not remove the message from the originating server.
    \value RemoveOnOriginatingServer  Remove the message from the originating server.
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
    \value WorkingMemoryOverflow  The operation failed because the messaging store could not perform the operation within the constraint specified by setMaximumWorkingMemory().
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
    \fn QMessageStore::queryMessages(const QMessageFilterKey &key, const QMessageSortKey &sortKey, uint limit, uint offset) const
    
    Returns the \l{QMessageId}s of messages in the messaging store. If \a key is not empty 
    only messages matching the parameters set by \a key will be returned, otherwise 
    all message identifiers will be returned.
    If \a sortKey is not empty, then the identifiers will be sorted by the parameters 
    set by \a sortKey.
    If \a limit is not zero, then \a limit places an upper bound on the number of 
    ids in the list returned.
    \a offset specifies how many ids to skip at the beginning of the list returned.
    
    \sa lastError(), countMessages(), setMaximumWorkingMemory()
*/

/*!
    \fn QMessageStore::queryFolders(const QMessageFolderFilterKey &key, const QMessageFolderSortKey &sortKey, uint limit, uint offset) const
    
    Returns the \l{QMessageId}s of messages in the messaging store. If \a key is not empty 
    only messages matching the parameters set by \a key will be returned, otherwise 
    all message identifiers will be returned.
    If \a sortKey is not empty, then the identifiers will be sorted by the parameters 
    set by \a sortKey.
    If \a limit is not zero, then \a limit places an upper bound on the number of 
    ids in the list returned.
    \a offset specifies how many ids to skip at the beginning of the list returned.
    
    \sa lastError(), countMessages(), setMaximumWorkingMemory()
*/

/*!
    \fn QMessageStore::queryAccounts(const QMessageAccountFilterKey &key, const QMessageAccountSortKey &sortKey, uint limit, uint offset) const
    
    Returns the \l{QMessageId}s of messages in the messaging store. If \a key is not empty 
    only messages matching the parameters set by \a key will be returned, otherwise 
    all message identifiers will be returned.
    If \a sortKey is not empty, then the identifiers will be sorted by the parameters 
    set by \a sortKey.
    If \a limit is not zero, then \a limit places an upper bound on the number of 
    ids in the list returned.
    \a offset specifies how many ids to skip at the beginning of the list returned.
    
    \sa lastError(), countMessages(), setMaximumWorkingMemory()
*/

/*!
    \fn QMessageStore::countMessages(const QMessageFilterKey& key) const
    
    Returns the count of the number of messages which match the 
    filtering criteria defined in QMessageFilterKey \a key. If 
    key is empty a count of all messages is returned.
    
    \sa lastError(), queryMessages(), setMaximumWorkingMemory()
*/

/*!
    \fn QMessageStore::countFolders(const QMessageFolderFilterKey& key) const
    
    Returns the count of the number of messages which match the 
    filtering criteria defined in QMessageFolderFilterKey \a key. If 
    key is empty a count of all messages is returned.
    
    \sa lastError(), queryMessages(), setMaximumWorkingMemory()
*/

/*!
    \fn QMessageStore::countAccounts(const QMessageAccountFilterKey& key) const
    
    Returns the count of the number of messages which match the 
    filtering criteria defined in QMessageAccountFilterKey \a key. If 
    key is empty a count of all messages is returned.
    
    \sa lastError(), queryMessages(), setMaximumWorkingMemory()
*/

/*!
    \fn QMessageStore::removeMessage(const QMessageId& id, RemovalOption option)
    
    Removes the message with QMessageId \a id from the messaging store. If \a option is 
    QMessageStore::RemoveOnOriginatingServer then the message should be removed from the 
    originating server when synchronization is performed with that server.

    Returns \c true if the operation successfully updates the store; otherwise returns \c false.
    
    To ensure the change is propagated to any affected external server
    QMessageServiceAction::exportUpdates() should be subsequently called.

    \sa removeMessages(), addMessage(), updateMessage(), QMessageServiceAction::exportUpdates()
*/

/*!
    \fn QMessageStore::removeMessages(const QMessageFilterKey& key, QMessageStore::RemovalOption option)
    
    Removes all messages identified by the key \a key from the messaging store.
    If \a option is QMessageStore::RemoveOnOriginatingServer then removal records will be 
    created for each removed message.

    Returns \c true if the operation successfully updates the store; otherwise returns \c false. 
    
    To ensure the change is propagated to any affected external server
    QMessageServiceAction::exportUpdates() should be subsequently called.


    For example:

    To implement a function to remove a list messages identified by QMessageIds
    from the messaging store.
    \code
    bool removeMessages(const QMessageIdList& ids)
    {
        QMessageFilterKey idsFilter(ids);
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
    \fn QMessageStore::setMaximumWorkingMemory(uint maximumBytes)
    
   If \a maximumBytes is 0, removes any constraint on the maximum memory 
   that can be allocated directly by the store when evaluating 
   countMessages() and queryMessages().
   
   Otherwise sets the maximum number of bytes than can be directly allocated 
   by the store when evaluating countMessages() and queryMessages() to 
   \a maximumBytes. A failure  to satisfy the working memory constraint is 
   reported by lastError() returning WorkingMemoryOverflow.
   
   The store only directly allocates working memory on platforms where 
   evaluating a boolean QMessageFilterKey based query requires a combination 
   of calls to the underlying platform query function.
   
   \sa maximumWorkingMemory(), ErrorCode, countMessages(), queryMessages(), lastError()
*/

/*!
    \fn QMessageStore::maximumWorkingMemory()
    
   Returns 0 if no constraint has been set by setMaximumWorkingMemory().
   
   Otherwise returns the maximum working memory as set by setMaximumWorkingMemory().
   
   The default maximum working memory is platform specific.
   
   \sa setMaximumWorkingMemory(), ErrorCode, countMessages(), queryMessages()
*/

/*!
    \fn QMessageStore::instance()
    
    Returns the single instance of the QMessageStore class.

    If necessary, the store will be instantiated and initialized.
*/

/*!
    \fn void QMessageStore::messagesAdded(const QMessageIdList& ids)

    Signal that is emitted when the messages identified by the list \a ids are
    added to the messaging store.

    \sa messagesRemoved(), messagesUpdated(), startNotifications()
*/

/*!
    \fn void QMessageStore::messagesRemoved(const QMessageIdList& ids)

    Signal that is emitted when the messages in the list \a ids are
    removed from the messaging store.

    \sa messagesAdded(), messagesUpdated(), startNotifications()
*/

/*!
    \fn void QMessageStore::messagesUpdated(const QMessageIdList& ids)

    Signal that is emitted when the messages in the list \a ids are
    updated within the messaging store.

    \sa messagesAdded(), messagesRemoved(), startNotifications()
*/

/*!
    \fn QMessageStore::startNotifications(const QMessageFilterKey &key)
    
    Starts emission of messagesAdded(), messagesRemoved() and messagesUpdated() signals
    for messages that match \a key.
    
    \sa messagesAdded(), messagesRemoved(), messagesUpdated(), stopNotifications()
*/

/*!
    \fn QMessageStore::stopNotifications()
    
    Stops emission of messagesAdded(), messagesRemoved() and messagesUpdated() signals.
    
    \sa messagesAdded(), messagesRemoved(), messagesUpdated(), startNotifications()
*/
