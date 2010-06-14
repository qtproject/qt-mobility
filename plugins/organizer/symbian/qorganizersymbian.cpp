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

#include "qorganizersymbian_p.h"
#include "qtorganizer.h"

#include <calsession.h>
#include <calentryview.h>

//QTM_USE_NAMESPACE

QOrganizerItemManagerEngine* QOrganizerItemSymbianFactory::engine(const QMap<QString, QString>& parameters, QOrganizerItemManager::Error* error)
{
    Q_UNUSED(parameters);
    Q_UNUSED(error);

    /* TODO - if you understand any specific parameters. save them in the engine so that engine::managerParameters can return them */

    QOrganizerItemSymbianEngine* ret = new QOrganizerItemSymbianEngine(); // manager takes ownership and will clean up.
    return ret;
}

QString QOrganizerItemSymbianFactory::managerName() const
{
    /* TODO - put your engine name here */
    return QLatin1String("symbian");
}
Q_EXPORT_PLUGIN2(qtorganizer_symbian, QOrganizerItemSymbianFactory);

QOrganizerItemSymbianEngine::QOrganizerItemSymbianEngine() :
    QOrganizerItemManagerEngine(),
    m_calSession(0),
    m_entryView(0),
    m_activeSchedulerWait(0),
    m_entrycount(0)
{
    // TODO: using CCal api stuff might be more readable if it was refactored into a separate class

    // Open calendar session and open default file
    m_calSession = CCalSession::NewL();
    m_calSession->OpenL(KNullDesC);

    // Create entry view (creation is synchronized with CActiveSchedulerWait)
    m_entryView = CCalEntryView::NewL(*m_calSession, *this);
    m_activeSchedulerWait = new CActiveSchedulerWait();
    m_activeSchedulerWait->Start();
}

QOrganizerItemSymbianEngine::~QOrganizerItemSymbianEngine()
{
    /* TODO clean up your stuff.  Perhaps a QScopedPointer or QSharedDataPointer would be in order */
    delete m_activeSchedulerWait;
    delete m_entryView;
    delete m_calSession;
}

QString QOrganizerItemSymbianEngine::managerName() const
{
    /* TODO - put your engine name here */
    return QLatin1String("symbian");
}

QMap<QString, QString> QOrganizerItemSymbianEngine::managerParameters() const
{
    /* TODO - in case you have any actual parameters that are relevant that you saved in the factory method, return them here */
    return QMap<QString, QString>();
}

int QOrganizerItemSymbianEngine::managerVersion() const
{
    /* TODO - implement this appropriately.  This is strictly defined by the engine, so return whatever you like */
    return 1;
}

QList<QOrganizerItem> QOrganizerItemSymbianEngine::itemInstances(const QOrganizerItem& generator, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount, QOrganizerItemManager::Error* error) const
{
    /*
        TODO

        This function should create a list of instances that occur in the time period from the supplied item.
        The periodStart should always be valid, and either the periodEnd or the maxCount will be valid (if periodEnd is
        valid, use that.  Otherwise use the count).  It's permissible to limit the number of items returned...

        Basically, if the generator item is an Event, a list of EventOccurrences should be returned.  Similarly for
        Todo/TodoOccurrence.

        If there are no instances, return an empty list.

        The returned items should have a QOrganizerItemInstanceOrigin detail that points to the generator and the
        original instance that the event would have occurred on (e.g. with an exception).

        They should not have recurrence information details in them.

        We might change the signature to split up the periodStart + periodEnd / periodStart + maxCount cases.
    */

    return QOrganizerItemManagerEngine::itemInstances(generator, periodStart, periodEnd, maxCount, error);
}

QList<QOrganizerItemLocalId> QOrganizerItemSymbianEngine::itemIds(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, QOrganizerItemManager::Error* error) const
{
    // Set minumum time for id fetch
    // TODO: get minumum time from filter
    TCalTime calTime;
    calTime.SetTimeUtcL(TCalTime::MinTime());
    
    // Get ids
    RArray<TCalLocalUid> ids;
    TRAPD(err, m_entryView->GetIdsModifiedSinceDateL(calTime, ids));
    transformError(err, error);
    if (*error != QOrganizerItemManager::NoError) {
        ids.Close();
        return QList<QOrganizerItemLocalId>();
    }
    
    // Convert to QOrganizerItemLocalId list
    QList<QOrganizerItemLocalId> itemIds;
    int count = ids.Count();
    for (int i=0; i<count; i++)
        itemIds << QOrganizerItemLocalId(ids[i]);
    ids.Close();
    
    // No filtering and sorting needed?
    if (filter == QOrganizerItemInvalidFilter() && sortOrders.count() == 0)
        return itemIds;    
        
    // Get items for slow filter
    QOrganizerItemFetchHint fetchHint;
    QList<QOrganizerItem> items;
    foreach(const QOrganizerItemLocalId &id, itemIds) {
        QOrganizerItem item = this->item(id, fetchHint, error);
        if (*error != QOrganizerItemManager::NoError)
            return QList<QOrganizerItemLocalId>();
        items << item;
    }
    
    // Use the general implementation to filter and sort items
    QList<QOrganizerItem> filteredAndSorted = slowFilter(items, filter, sortOrders);
    
    // Convert to QOrganizerItemLocalId list
    QList<QOrganizerItemLocalId> filteredAndSortedIds;
    foreach (const QOrganizerItem &item, filteredAndSorted)
        filteredAndSortedIds << item.localId();
    
    return filteredAndSortedIds;
}

QList<QOrganizerItem> QOrganizerItemSymbianEngine::items(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const
{
    // Set minumum time for id fetch
    // TODO: get minumum time from filter
    TCalTime calTime;
    calTime.SetTimeUtcL(TCalTime::MinTime());
    
    // Get ids
    RArray<TCalLocalUid> ids;
    TRAPD(err, m_entryView->GetIdsModifiedSinceDateL(calTime, ids));
    transformError(err, error);
    if (*error != QOrganizerItemManager::NoError) {
        ids.Close();
        return QList<QOrganizerItem>();
    }
        
    // Get items
    QList<QOrganizerItem> items;
    int count = ids.Count();
    for (int i=0; i<count; i++) {
        QOrganizerItem item = this->item(QOrganizerItemLocalId(ids[i]), fetchHint, error);
        if (*error != QOrganizerItemManager::NoError)
            return QList<QOrganizerItem>();
        items << item;
    }
    ids.Close();
    
    // No filtering and sorting needed?
    if (filter == QOrganizerItemInvalidFilter() && sortOrders.count() == 0)
        return items;
    
    // Use the general implementation to filter and sort items
    return slowFilter(items, filter, sortOrders);
}

QOrganizerItem QOrganizerItemSymbianEngine::item(const QOrganizerItemLocalId& itemId, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const
{
    QOrganizerItem item;
    TRAPD(err, itemL(itemId, &item, fetchHint));
    transformError(err, error);
    return item;
}

void QOrganizerItemSymbianEngine::itemL(const QOrganizerItemLocalId& itemId, QOrganizerItem *item, const QOrganizerItemFetchHint& fetchHint) const
{
    // TODO: use fetch hint to optimize performance and/or memory consumption?
        /* The fetch hint suggests how much of the item to fetch.
        You can ignore the fetch hint and fetch everything (but you must
        fetch at least what is mentioned in the fetch hint). */

    // Fetch item
    TCalLocalUid uid(itemId);
    CCalEntry *calEntry = m_entryView->FetchL(uid);
    CleanupStack::PushL(calEntry);
    
    // Transform CCalEntry -> QOrganizerItem
    m_itemTransform.toItemL(*calEntry, item);
    
    // Set item id
    QOrganizerItemId id;
    id.setLocalId(itemId);
    id.setManagerUri(managerUri());
    item->setId(id);

    CleanupStack::PopAndDestroy(calEntry);
}

bool QOrganizerItemSymbianEngine::saveItems(QList<QOrganizerItem> *items, QMap<int, QOrganizerItemManager::Error> *errorMap, QOrganizerItemManager::Error* error)
{
    // TODO: the performance would be probably better, if we had a separate
    // implementation for the case with a list of items that would save all
    // the items
    for (int i(0); i < items->count(); i++) {
        QOrganizerItem item = items->at(i);
        saveItem(&item, error);
        if (*error != QOrganizerItemManager::NoError) {
            errorMap->insert(i, *error);
        } else {
            // Update the item with the data that is available after save
            items->replace(i, item);
        }
    }

    return *error == QOrganizerItemManager::NoError;
}

bool QOrganizerItemSymbianEngine::saveItem(QOrganizerItem *item, QOrganizerItemManager::Error* error)
{
    // TODO: Validate item according to the schema

    TRAPD(err, saveItemL(item));
    return transformError(err, error);
}

void QOrganizerItemSymbianEngine::saveItemL(QOrganizerItem *item)
{
    // Transform QOrganizerItem -> CCalEntry
    CCalEntry *entry = m_itemTransform.toEntryLC(*item);
    
    // update last modified date
    entry->SetLastModifiedDateL();
    // TODO: update timestamp

    // Save entry
    RPointerArray<CCalEntry> entries;
    CleanupClosePushL(entries);
    entries.AppendL(entry);
    TInt count(0);
    m_entryView->StoreL(entries, count);
    const TInt expectedCount(1);
    if (count != expectedCount) {
        // The documentation states about count "On return, this
        // contains the number of entries which were successfully stored".
        // So it is not clear which error caused storing the entry to fail
        // -> let's use the "one-error-fits-all" error code KErrGeneral.
        User::Leave(KErrGeneral);
    }
    
    // Update guid
    item->setGuid(OrganizerItemTransform::toQString(entry->UidL()));
    
    // Update local id
    QOrganizerItemId itemId;
    TCalLocalUid localUid = entry->LocalUidL();
    itemId.setLocalId(localUid);
    itemId.setManagerUri(managerUri());
    item->setId(itemId);

    // Cleanup
    CleanupStack::PopAndDestroy(&entries);
    CleanupStack::PopAndDestroy(entry);
}

bool QOrganizerItemSymbianEngine::removeItems(const QList<QOrganizerItemLocalId>& itemIds, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error)
{
    // TODO: the performance would be probably better, if we had a separate
    // implementation for the case with a list of item ids that would
    // remove all the items
    for (int i(0); i < itemIds.count(); i++) {
        QOrganizerItemLocalId localId = itemIds.at(i);
        removeItem(localId, error);
        if (*error != QOrganizerItemManager::NoError) {
            errorMap->insert(i, *error);
        }
    }
    return *error == QOrganizerItemManager::NoError;
}

bool QOrganizerItemSymbianEngine::removeItem(const QOrganizerItemLocalId& organizeritemId, QOrganizerItemManager::Error* error)
{
    TRAPD(err, removeItemL(organizeritemId));
    return transformError(err, error);
}

void QOrganizerItemSymbianEngine::removeItemL(const QOrganizerItemLocalId& organizeritemId)
{
    // TODO: DoesNotExistError should be used if the id refers to a non existent item.
    // TODO: How to remove item instances?

    RArray<TCalLocalUid> ids;
    CleanupClosePushL(ids);
    ids.AppendL(TCalLocalUid(organizeritemId));
    TInt count(0);
    m_entryView->DeleteL(ids, count);
    CleanupStack::PopAndDestroy(&ids);
}

QList<QOrganizerItem> QOrganizerItemSymbianEngine::slowFilter(const QList<QOrganizerItem> &items, const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders) const
{
    QList<QOrganizerItem> filteredAndSorted;
    foreach(const QOrganizerItem& item, items) {
        if (QOrganizerItemManagerEngine::testFilter(filter, item))
            QOrganizerItemManagerEngine::addSorted(&filteredAndSorted, item, sortOrders);
    }
    return filteredAndSorted;
}

QMap<QString, QOrganizerItemDetailDefinition> QOrganizerItemSymbianEngine::detailDefinitions(const QString& itemType, QOrganizerItemManager::Error* error) const
{
    /* TODO - once you know what your engine will support, implement this properly.  One way is to call the base version, and add/remove things as needed */
    return detailDefinitions(itemType, error);
}

QOrganizerItemDetailDefinition QOrganizerItemSymbianEngine::detailDefinition(const QString& definitionId, const QString& itemType, QOrganizerItemManager::Error* error) const
{
    /* TODO - the default implementation just calls the base detailDefinitions function.  If that's inefficent, implement this */
    return QOrganizerItemManagerEngine::detailDefinition(definitionId, itemType, error);
}

bool QOrganizerItemSymbianEngine::saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& itemType, QOrganizerItemManager::Error* error)
{
    /* TODO - if you support adding custom fields, do that here.  Otherwise call the base functionality. */
    return QOrganizerItemManagerEngine::saveDetailDefinition(def, itemType, error);
}

bool QOrganizerItemSymbianEngine::removeDetailDefinition(const QString& definitionId, const QString& itemType, QOrganizerItemManager::Error* error)
{
    /* TODO - if you support removing custom fields, do that here.  Otherwise call the base functionality. */
    return QOrganizerItemManagerEngine::removeDetailDefinition(definitionId, itemType, error);
}

bool QOrganizerItemSymbianEngine::startRequest(QOrganizerItemAbstractRequest* req)
{
    /*
        TODO

        This is the entry point to the async API.  The request object describes the
        type of request (switch on req->type()).  Req will not be null when called
        by the framework.

        Generally, you can queue the request and process them at some later time
        (probably in another thread).

        Once you start a request, call the updateRequestState and/or the
        specific updateXXXXXRequest functions to mark it in the active state.

        If your engine is particularly fast, or the operation involves only in
        memory data, you can process and complete the request here.  That is
        probably not the case, though.

        Note that when the client is threaded, and the request might live on a
        different thread, you might need to be careful with locking.  In particular,
        the request might be deleted while you are still working on it.  In this case,
        your requestDestroyed function will be called while the request is still valid,
        and you should block in that function until your worker thread (etc) has been
        notified not to touch that request any more.

        We plan to provide some boiler plate code that will allow you to:

        1) implement the sync functions, and have the async versions call the sync
           in another thread

        2) or implement the async versions of the function, and have the sync versions
           call the async versions.

        It's not ready yet, though.

        Return true if the request can be started, false otherwise.  You can set an error
        in the request if you like.
    */
    return QOrganizerItemManagerEngine::startRequest(req);
}

bool QOrganizerItemSymbianEngine::cancelRequest(QOrganizerItemAbstractRequest* req)
{
    /*
        TODO

        Cancel an in progress async request.  If not possible, return false from here.
    */
    return QOrganizerItemManagerEngine::cancelRequest(req);
}

bool QOrganizerItemSymbianEngine::waitForRequestFinished(QOrganizerItemAbstractRequest* req, int msecs)
{
    /*
        TODO

        Wait for a request to complete (up to a max of msecs milliseconds).

        Return true if the request is finished (including if it was already).  False otherwise.

        You should really implement this function, if nothing else than as a delay, since clients
        may call this in a loop.

        It's best to avoid processing events, if you can, or at least only process non-UI events.
    */
    return QOrganizerItemManagerEngine::waitForRequestFinished(req, msecs);
}

void QOrganizerItemSymbianEngine::requestDestroyed(QOrganizerItemAbstractRequest* req)
{
    /*
        TODO

        This is called when a request is being deleted.  It lets you know:

        1) the client doesn't care about the request any more.  You can still complete it if
           you feel like it.
        2) you can't reliably access any properties of the request pointer any more.  The pointer will
           be invalid once this function returns.

        This means that if you have a worker thread, you need to let that thread know that the
        request object is not valid and block until that thread acknowledges it.  One way to do this
        is to have a QSet<QOIAR*> (or QMap<QOIAR, MyCustomRequestState>) that tracks active requests, and
        insert into that set in startRequest, and remove in requestDestroyed (or when it finishes or is
        cancelled).  Protect that set/map with a mutex, and make sure you take the mutex in the worker
        thread before calling any of the QOIAR::updateXXXXXXRequest functions.  And be careful of lock
        ordering problems :D

    */
    return QOrganizerItemManagerEngine::requestDestroyed(req);
}

bool QOrganizerItemSymbianEngine::hasFeature(QOrganizerItemManager::ManagerFeature feature, const QString& itemType) const
{
    // TODO - the answer to the question may depend on the type
    Q_UNUSED(itemType);
    switch(feature) {
        case QOrganizerItemManager::MutableDefinitions:
            // TODO If you support save/remove detail definition, return true
            return false;

        case QOrganizerItemManager::Anonymous:
            // TODO if this engine is anonymous (e.g. no other engine can share the data) return true
            // (mostly for an in memory engine)
            return false;
        case QOrganizerItemManager::ChangeLogs:
            // TODO if this engine supports filtering by last modified/created/removed timestamps, return true
            return false;
    }
    return false;
}

bool QOrganizerItemSymbianEngine::isFilterSupported(const QOrganizerItemFilter& filter) const
{
    // TODO if you engine can natively support the filter, return true.  Otherwise you should emulate support in the item{Ids} functions.
    Q_UNUSED(filter);
    return false;
}

QList<QVariant::Type> QOrganizerItemSymbianEngine::supportedDataTypes() const
{
    QList<QVariant::Type> ret;
    // TODO - tweak which data types this engine understands
    ret << QVariant::String;
    ret << QVariant::Date;
    ret << QVariant::DateTime;
    ret << QVariant::Time;

    return ret;
}

QStringList QOrganizerItemSymbianEngine::supportedItemTypes() const
{
    // TODO - return which [predefined] types this engine supports
    QStringList ret;

    ret << QOrganizerItemType::TypeEvent;
    ret << QOrganizerItemType::TypeEventOccurrence;
    ret << QOrganizerItemType::TypeJournal;
    ret << QOrganizerItemType::TypeNote;
    ret << QOrganizerItemType::TypeTodo;
    ret << QOrganizerItemType::TypeTodoOccurrence;

    return ret;
}

/*!
 * From MCalProgressCallBack
 */
void QOrganizerItemSymbianEngine::Progress(TInt /*aPercentageCompleted*/)
{
}

/*!
 * From MCalProgressCallBack
 */
void QOrganizerItemSymbianEngine::Completed(TInt aError)
{
    // TODO: How to handle aError? The client should be informed that the
    // initialization failed

    // Let's continue the operation that started the calendar operation
    m_activeSchedulerWait->AsyncStop();
}

/*!
 * From MCalProgressCallBack
 */
TBool QOrganizerItemSymbianEngine::NotifyProgress()
{
    // No 
    return EFalse;
}

/*! Transform a Symbian error id to QOrganizerItemManager::Error.
 *
 * \param symbianError Symbian error.
 * \param QtError Qt error.
 * \return true if there was no error
 *         false if there was an error
*/
bool QOrganizerItemSymbianEngine::transformError(TInt symbianError, QOrganizerItemManager::Error* qtError)
{
    switch(symbianError)
    {
        case KErrNone:
        {
            *qtError = QOrganizerItemManager::NoError;
            break;
        }
        case KErrNotFound:
        {
            *qtError = QOrganizerItemManager::DoesNotExistError;
            break;
        }
        case KErrAlreadyExists:
        {
            *qtError = QOrganizerItemManager::AlreadyExistsError;
            break;
        }
        case KErrLocked:
        {
            *qtError = QOrganizerItemManager::LockedError;
            break;
        }
        case KErrAccessDenied:
        case KErrPermissionDenied:
        {
            *qtError = QOrganizerItemManager::PermissionsError;
            break;
        }
        case KErrNoMemory:
        {
            *qtError = QOrganizerItemManager::OutOfMemoryError;
            break;
        }
        case KErrNotSupported:
        {
            *qtError = QOrganizerItemManager::NotSupportedError;
            break;
        }
        case KErrArgument:
        {
            *qtError = QOrganizerItemManager::BadArgumentError;
            break;
        }
        default:
        {
            *qtError = QOrganizerItemManager::UnspecifiedError;
            break;
        }
    }
    return *qtError == QOrganizerItemManager::NoError;
}
