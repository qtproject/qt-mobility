/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
//system includes
#include <calcommon.h>
#include <calinstance.h>
#include <calsession.h>
#include <calchangecallback.h>
#include <calentryview.h>
#include <calinstanceview.h>
#ifdef SYMBIAN_CALENDAR_V2
#include <QColor>
#include <calinstanceiterator.h>
#include <calcalendariterator.h>
#include <calcalendarinfo.h>
// This file (calenmulticaluids.hrh) no longer exists in S^4, so use a local copy for now
#include "local_calenmulticaluids.hrh"
#endif

// user includes
#include "qorganizersymbian_p.h"
#include "qtorganizer.h"
#include "organizeritemtypetransform.h"
#include "organizeritemguidtransform.h"
#include "qorganizeritemrequestqueue.h"
#include "organizersymbianutils.h"

using namespace OrganizerSymbianUtils;

// Special (internal) error code to be used when an item occurrence is not
// valid. The error code is not expected to clash with any symbian calendar
// API errors.
const TInt KErrInvalidOccurrence(-32768);

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
    m_defaultCollection(this),
    m_activeSchedulerWait(0)
{
    // Open the default collection
    m_defaultCollection.openL(KNullDesC);
    m_collections.insert(m_defaultCollection.localId(), m_defaultCollection); 
    m_defaultCollection.createEntryViewL();
    
#ifdef SYMBIAN_CALENDAR_V2
    // Start listening to calendar file changes
    m_defaultCollection.calSession()->StartFileChangeNotificationL(*this);
    
    // Load available calendars
    CCalCalendarIterator *iterator = CCalCalendarIterator::NewL(*m_defaultCollection.calSession());
    CleanupStack::PushL(iterator);
    for (CCalCalendarInfo *calInfo=iterator->FirstL(); calInfo != 0; calInfo=iterator->NextL()) {
        
        CleanupStack::PushL(calInfo);

        // Skip calendars which are marked for deletion
        TBool markAsDelete = EFalse;
        TRAP_IGNORE(markAsDelete = getCalInfoPropertyL<TBool>(*calInfo, EMarkAsDelete));
        if (markAsDelete) {
            CleanupStack::PopAndDestroy(calInfo);
            continue;
        }

        // Skip default calendar (already loaded)
        QString fileName = toQString(calInfo->FileNameL());
        if (fileName.compare(m_defaultCollection.fileName(), Qt::CaseInsensitive) == 0) {
            CleanupStack::PopAndDestroy(calInfo);
            continue;
        }
        
        // Open a new session to the calendar
        OrganizerSymbianCollection collection(this);
        collection.openL(calInfo->FileNameL());
        m_collections.insert(collection.localId(), collection);
        collection.createEntryViewL();
        
        CleanupStack::PopAndDestroy(calInfo);
    }
    CleanupStack::PopAndDestroy(iterator);
#endif
       
    m_instanceView = CCalInstanceView::NewL(*m_defaultCollection.calSession(), *this);
    // TODO: multical instance view(s)
    // TODO: The calendar session may take some time to initialize which would
    // make an UI app using symbian backend freeze. To be refactored.
    m_activeSchedulerWait = new CActiveSchedulerWait(); 
    m_activeSchedulerWait->Start();
    
    // Create request queue for asynch requests
    m_requestServiceProviderQueue = QOrganizerItemRequestQueue::instance(*this);
}

QOrganizerItemSymbianEngine::~QOrganizerItemSymbianEngine()
{
#ifdef SYMBIAN_CALENDAR_V2
    m_defaultCollection.calSession()->StopFileChangeNotification();
#endif
	delete m_requestServiceProviderQueue;
    delete m_activeSchedulerWait;
    delete m_instanceView;
}

QString QOrganizerItemSymbianEngine::managerName() const
{
    return QLatin1String("symbian");
}

QMap<QString, QString> QOrganizerItemSymbianEngine::managerParameters() const
{
    /* TODO - in case you have any actual parameters that are relevant that you saved in the factory method, return them here */
    return QMap<QString, QString>();
}

int QOrganizerItemSymbianEngine::managerVersion() const
{
    // This is strictly defined by the engine, so we can return whatever we like
    return 1;
}

QList<QOrganizerItem> QOrganizerItemSymbianEngine::itemInstances(const QOrganizerItem& generator, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount, QOrganizerItemManager::Error* error) const
{
    QList<QOrganizerItem> occurrenceList;
    
    // Parent item should be an Event or a Todo
    if (!((generator.type()== QOrganizerItemType::TypeEvent) ||(generator.type()== QOrganizerItemType::TypeTodo))) {
        *error = QOrganizerItemManager::InvalidItemTypeError;
        return occurrenceList;
    }

    //check for valid periodStart    
    if (periodStart.isValid()&&(periodEnd.isValid() || (maxCount > 0))) {
        
        // End period should be greater than start period.   
        if (periodEnd.isValid() && (periodEnd < periodStart)) {
            *error = QOrganizerItemManager::BadArgumentError;
            return occurrenceList;
        }
        RPointerArray<CCalInstance> instanceList;
        QDateTime endDateTime(periodEnd);
        CalCommon::TCalViewFilter filter(0);
        //use maximum end date if only count is present.
        if ((!periodEnd.isValid()) && (maxCount > 0)) {
              TCalTime endTime; 
              endTime.SetTimeUtcL(TCalTime::MaxTime());
              endDateTime = toQDateTimeL(endTime);  
        }
        
        if (generator.type() == QOrganizerItemType::TypeEvent) {
        	filter = CalCommon::EIncludeAppts; 
        } else if (generator.type() ==  QOrganizerItemType::TypeTodo) {
        	filter = (CalCommon::EIncludeCompletedTodos |
        	CalCommon::EIncludeIncompletedTodos);
        }
		#ifdef SYMBIAN_CALENDAR_V2
            CCalInstanceIterator *iterator(NULL);
            CCalFindInstanceSettings *findIntanceSettings = CCalFindInstanceSettings::NewL(filter,
                CalCommon::TCalTimeRange(toTCalTimeL(periodStart),
                                         toTCalTimeL(endDateTime)));
            CleanupStack::PushL(findIntanceSettings);
        
            TRAPD(err,iterator = m_instanceView->FindInstanceL(*findIntanceSettings));
            CleanupStack::PopAndDestroy(findIntanceSettings);
            CleanupStack::PushL(iterator);
        #else        
        
            TRAPD(err, m_instanceView->FindInstanceL(instanceList,filter,
                                                     CalCommon::TCalTimeRange(
                                       toTCalTimeL(periodStart),
                                       toTCalTimeL(endDateTime))
                                       ));
        #endif    
        transformError(err, error);
   
        if (*error == QOrganizerItemManager::NoError) {
            #ifdef SYMBIAN_CALENDAR_V2
                int count(iterator->Count()); 
            #else
                int count(instanceList.Count()); 
            #endif
            // Convert calninstance list to  QOrganizerEventOccurrence and add to QOrganizerItem list                 
            for( int index=0; index < count;index++ ) {
                 QOrganizerItem itemInstance;
                 CCalInstance* calInstance(NULL);
                 #ifdef SYMBIAN_CALENDAR_V2
                     calInstance = iterator->NextL(); 
                 #else
                     calInstance = (instanceList)[index];
                 #endif     
                     CleanupStack::PushL(calInstance);
                     
                 if (QOrganizerItemType::TypeEvent == generator.type())
                     itemInstance.setType(QOrganizerItemType::TypeEventOccurrence);
                 else if (QOrganizerItemType::TypeTodo == generator.type())
                     itemInstance.setType(QOrganizerItemType::TypeTodoOccurrence);
                 else
                     User::Leave(KErrNotSupported);

                 TRAPD(err, m_itemTransform.toItemInstanceL(*calInstance, &itemInstance));
                 transformError(err, error);
                 if ((*error == QOrganizerItemManager::NoError)&&(generator.guid() == itemInstance.guid())) {
                     if ((periodEnd.isValid()&& (maxCount < 0))||(maxCount > 0) && (index < maxCount)) {
                         QOrganizerItemId id;
                         id.setManagerUri(this->managerUri());
                         // The instance might be modified. Then it will not point to the parent entry.
                         // In this case local id must be set. Otherwise it should be zero.
                         QOrganizerItemLocalId instanceEntryId = calInstance->Entry().LocalUidL();
                         if (instanceEntryId != generator.localId())
                             id.setLocalId(instanceEntryId);
                         itemInstance.setId(id);
                         occurrenceList.append(itemInstance);
                     }
                 }    
                 CleanupStack::PopAndDestroy(calInstance);                  
            }           
        }
        #ifdef SYMBIAN_CALENDAR_V2
            CleanupStack::PopAndDestroy(iterator);
        #else
            instanceList.Close();
        #endif
        
    } else {
        *error = QOrganizerItemManager::BadArgumentError;
    }
    
    return occurrenceList;
}
QList<QOrganizerItem> QOrganizerItemSymbianEngine::itemInstances(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint,QOrganizerItemManager::Error* error) const
{
    Q_UNUSED(fetchHint);
    QList<QOrganizerItem> occurrenceList;
    TCalTime startTime;
    startTime.SetTimeUtcL(TCalTime::MinTime());

    TCalTime endTime;
    endTime.SetTimeUtcL(TCalTime::MaxTime());

    RPointerArray<CCalInstance> instanceList;
    TRAPD(err, m_instanceView->FindInstanceL(instanceList,CalCommon::EIncludeAll,
                                   CalCommon::TCalTimeRange(startTime,endTime)));
            
    transformError(err, error);
    //Convert instance list to list of QOrganizerItem    
    if (*error == QOrganizerItemManager::NoError) {  
        int count(instanceList.Count()); 
        // Convert calninstance list to  QOrganizerEventOccurrence and add to QOrganizerItem list                 
        for( int index=0; index < count;index++ ) {
             QOrganizerItem itemInstance;
             CCalInstance* calInstance = (instanceList)[index];
                                 
             TRAPD(err, m_itemTransform.toItemInstanceL(*calInstance, &itemInstance));
             transformError(err, error);
             if (*error == QOrganizerItemManager::NoError) {
                 QOrganizerItemId id;
                 id.setManagerUri(this->managerUri());
                 
                 // The instance might be modified. Then it will not point to the parent entry.
                 // In this case local id must be set. Otherwise it should be zero.                                                  
                 if (calInstance->Entry().RecurrenceIdL().TimeUtcL() != Time::NullTTime()) {
                     QOrganizerItemLocalId instanceEntryId = calInstance->Entry().LocalUidL();    
                     id.setLocalId(instanceEntryId);
                     itemInstance.setId(id);
                 }                                                                      
                 occurrenceList.append(itemInstance);
             }    
        }           
    } else {
        instanceList.ResetAndDestroy();
        return occurrenceList;
    }
    instanceList.ResetAndDestroy();
    
    //Check whether no filtering and sorting needed.Return complete list.
    if (filter == QOrganizerItemInvalidFilter() && sortOrders.count() == 0)
        return occurrenceList;    
            
    // Use the general implementation to filter and sort items
    QList<QOrganizerItem> filteredAndSortedItemList = slowFilter(occurrenceList, filter, sortOrders);
    occurrenceList.clear();
    occurrenceList.append(filteredAndSortedItemList);
                   
    return occurrenceList;
}

QList<QOrganizerItemLocalId> QOrganizerItemSymbianEngine::itemIds(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, QOrganizerItemManager::Error* error) const
{
    // Set minumum time for id fetch
    // TODO: get minumum time from filter
    TCalTime calTime;
    calTime.SetTimeUtcL(TCalTime::MinTime());
    
    // Get ids from all entry views
    // TODO: instead of looping through entry views, get the collection id from
    // local id? (not certain that is the correct way, because the Qt API for
    // collections is still under development)
    RArray<TCalLocalUid> ids;
    foreach(QOrganizerCollectionLocalId collectionId, m_collections.keys()) {
        TRAPD(err, entryViewL(collectionId)->GetIdsModifiedSinceDateL(calTime, ids));
        transformError(err, error);
        if (*error != QOrganizerItemManager::NoError) {
            ids.Close();
            return QList<QOrganizerItemLocalId>();
        }
    }

    // Convert to QOrganizerItemLocalId list
    QList<QOrganizerItemLocalId> itemIds;
    int count = ids.Count();
    for (int i=0; i<count; i++)
        itemIds << QOrganizerItemLocalId(ids[i]);
    ids.Close();
    
    // No filtering and sorting needed?
    if (filter.type() == QOrganizerItemFilter::InvalidFilter || filter.type() == QOrganizerItemFilter::DefaultFilter && sortOrders.count() == 0)
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
    // TODO: optimize if the filter rules out some of the collections
    foreach (QOrganizerCollectionLocalId collectionId, m_collections.keys()) {
        TRAPD(err, entryViewL(collectionId)->GetIdsModifiedSinceDateL(calTime, ids));
        transformError(err, error);
        if (*error != QOrganizerItemManager::NoError) {
            ids.Close();
            return QList<QOrganizerItem>();
        }
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
    if (filter.type() == QOrganizerItemFilter::InvalidFilter || filter.type() == QOrganizerItemFilter::DefaultFilter && sortOrders.count() == 0)
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
	Q_UNUSED(fetchHint)
    // TODO: use fetch hint to optimize performance and/or memory consumption?
        /* The fetch hint suggests how much of the item to fetch.
        You can ignore the fetch hint and fetch everything (but you must
        fetch at least what is mentioned in the fetch hint). */

    // Fetch item
    TCalLocalUid uid(itemId);
    CCalEntry *calEntry(0);
    QOrganizerCollectionLocalId collectionLocalId(0);
    foreach(QOrganizerCollectionLocalId id, m_collections.keys()) {
        // TODO: instead of looping through entry views, get the collection id from
        // local id? (not certain that is the correct way, because the Qt API for
        // collections is still under development)
        calEntry = entryViewL(id)->FetchL(uid);
        if (calEntry) {
            collectionLocalId = id;
            break;
        }
    }
    // The corresponding entry not found from the database
    if (!calEntry)
        User::Leave(KErrNotFound);
    CleanupStack::PushL(calEntry);

    // Transform CCalEntry -> QOrganizerItem
    m_itemTransform.toItemL(*calEntry, item);
    
    // Set item id
    QOrganizerItemId id;
    id.setLocalId(itemId);
    id.setManagerUri(managerUri());
    item->setId(id);

    // Set parent local id
    if (item->type() == QOrganizerItemType::TypeEventOccurrence) {
        HBufC8* globalUid = OrganizerItemGuidTransform::guidLC(*item);
        // We don't know the local id yet, let's search with globalUid
        CCalEntry *parentEntry = findParentEntryLC(collectionLocalId, item, *globalUid);
        QOrganizerEventOccurrence *eventOccurrence = (QOrganizerEventOccurrence *)item;
        eventOccurrence->setParentLocalId(parentEntry->LocalUidL());
        CleanupStack::PopAndDestroy(parentEntry);
        CleanupStack::PopAndDestroy(globalUid);
    }

    // Set collection id
    QOrganizerCollectionId cid;
    cid.setLocalId(collectionLocalId);
    cid.setManagerUri(managerUri());
    setItemCollectionId(item, cid);

    CleanupStack::PopAndDestroy(calEntry);
}

bool QOrganizerItemSymbianEngine::saveItems(QList<QOrganizerItem> *items, const QOrganizerCollectionLocalId& collectionId, QMap<int, QOrganizerItemManager::Error> *errorMap, QOrganizerItemManager::Error* error)
{
    // TODO: the performance would be probably better, if we had a separate
    // implementation for the case with a list of items that would save all
    // the items
    
    QOrganizerItemChangeSet changeSet;
    
    for (int i(0); i < items->count(); i++) {
        QOrganizerItem item = items->at(i);
        
        // Validate & save
        QOrganizerItemManager::Error saveError;
        if (validateItem(item, &saveError)) {
            TRAPD(err, saveItemL(&item, collectionId, &changeSet));
            transformError(err, &saveError);
        }
        
        // Check error
        if (saveError != QOrganizerItemManager::NoError) {
            *error = saveError;
            if (errorMap)
                errorMap->insert(i, *error);
        } else {
            // Update the item with the data that is available after save
            items->replace(i, item);
        }
    }
    
    // Emit changes
    changeSet.emitSignals(this);
    
    return *error == QOrganizerItemManager::NoError;
}

bool QOrganizerItemSymbianEngine::saveItem(QOrganizerItem* item, const QOrganizerCollectionLocalId& collectionId, QOrganizerItemManager::Error* error)
{
    // Validate & save
    if (validateItem(*item, error)) {
        QOrganizerItemChangeSet changeSet;
        TRAPD(err, saveItemL(item, collectionId, &changeSet));
        transformError(err, error);
        changeSet.emitSignals(this);
    }
    return *error == QOrganizerItemManager::NoError;
}

void QOrganizerItemSymbianEngine::saveItemL(QOrganizerItem *item, const QOrganizerCollectionLocalId& collectionId, QOrganizerItemChangeSet *changeSet)
{
    QOrganizerCollectionLocalId collectionLocalId = collectionLocalIdL(*item, collectionId);

    // Find the entry corresponding to the item or to the item occurrence.
    // Creates a new one, if the corresponding entry does not exist yet.
    CCalEntry *entry(0);
    bool isNewEntry(false);
    if(item->type()== QOrganizerItemType::TypeEventOccurrence
        || item->type()== QOrganizerItemType::TypeTodoOccurrence) {
        entry = entryForItemOccurrenceL(collectionId, item, isNewEntry);
    } else {
        entry = entryForItemL(collectionId, item, isNewEntry);
    }
    CleanupStack::PushL(entry);

    // Transform QOrganizerItem -> CCalEntry    
    m_itemTransform.toEntryL(*item, entry);

    // Save entry to the database
    RPointerArray<CCalEntry> entries;
    CleanupClosePushL(entries);
    entries.AppendL(entry);
    TInt count(0);
    entryViewL(collectionId)->StoreL(entries, count);
    if (count != entries.Count()) {
        // The documentation states about count "On return, this
        // contains the number of entries which were successfully stored".
        // So it is not clear which error caused storing the entry to fail
        // -> let's use the "one-error-fits-all" error code KErrGeneral.
        User::Leave(KErrGeneral);
    }

    // Transform details that are available/updated after saving    
    m_itemTransform.toItemPostSaveL(*entry, item, managerUri());

    // Set collection id
    QOrganizerCollectionId cid;
    cid.setLocalId(collectionLocalIdL(*item, collectionId));
    cid.setManagerUri(managerUri());
    setItemCollectionId(item, cid);

    // Cleanup
    CleanupStack::PopAndDestroy(&entries);
    CleanupStack::PopAndDestroy(entry);

    // Update change set for signal emissions
    if (changeSet) {
        if (isNewEntry)
            changeSet->insertAddedItem(item->localId());
        else
            changeSet->insertChangedItem(item->localId());
    }
}

/*!
 * Retrieves the entry view for the collection. Leaves with KErrArgument if
 * not found.
 */
CCalEntryView* QOrganizerItemSymbianEngine::entryViewL(const QOrganizerCollectionLocalId& collectionId) const
{
    QOrganizerCollectionLocalId tempCollectionId = collectionId;

    // 0 is interpreted as the default collection
    if (tempCollectionId == 0)
        tempCollectionId = m_defaultCollection.localId();

    if (!m_collections.contains(tempCollectionId))
        User::Leave(KErrArgument);

    return m_collections[tempCollectionId].calEntryView();
}

/*!
 * Returns item's collection id if it is valid. If not returns collectionId
 * given as a parameter if it is valid. Fallback is to return the default
 * session's collection id.
 */
QOrganizerCollectionLocalId QOrganizerItemSymbianEngine::collectionLocalIdL(QOrganizerItem item, const QOrganizerCollectionLocalId& collectionId) const
{
#ifdef SYMBIAN_CALENDAR_V2
    QOrganizerCollectionLocalId itemCollectionId = item.collectionId().localId();

    if (itemCollectionId && collectionId
        && collectionId != itemCollectionId)
            User::Leave(KErrArgument);
    else if (collectionId)
        return collectionId;
    else if (itemCollectionId)
        return itemCollectionId;

    // Default collection id is the default session's collection id
    return m_defaultCollection.localId();
#else
    Q_UNUSED(item);
    Q_UNUSED(collectionId);

    // Collections not supported, the default collection id is 1
    return 1;
#endif
}

CCalEntry* QOrganizerItemSymbianEngine::entryForItemOccurrenceL(const QOrganizerCollectionLocalId collectionId, QOrganizerItem *item, bool &isNewEntry) const
{
    CCalEntry * entry(NULL);

    // Find the child entry corresponding to the item occurrence
    if (item->localId()) {
        // The item has a local id, check the item is from this manager
        if (item->id().managerUri() != managerUri())
            User::Leave(KErrInvalidOccurrence);

        // Fetch the item (will return NULL if the localid is not found)
        entry = entryViewL(collectionLocalIdL(*item))->FetchL(item->localId());
        if (!entry)
            User::Leave(KErrInvalidOccurrence);
        return entry;
    }

    // Entry not found, find the parent entry and create a new child for it
    HBufC8* parentGlobalUid = OrganizerItemGuidTransform::guidLC(*item);
    CCalEntry *parentEntry = findParentEntryLC(collectionId, item, *parentGlobalUid);

    // Get the parameters for the new child entry
    QOrganizerItemInstanceOrigin origin = item->detail<QOrganizerItemInstanceOrigin>();
    if (!origin.originalDate().isValid()) {
        User::Leave(KErrInvalidOccurrence);
    }
    QDateTime parentStartTime = toQDateTimeL(parentEntry->StartTimeL());
    QDateTime recurrenceDateTime = QDateTime(origin.originalDate(), parentStartTime.time());
    TCalTime recurrenceId = toTCalTimeL(recurrenceDateTime);
    HBufC8* globalUid = HBufC8::NewLC(parentEntry->UidL().Length());
    globalUid->Des().Copy(parentEntry->UidL());

    // Create the new child entry
    entry = CCalEntry::NewL(parentEntry->EntryTypeL(),
                            globalUid,
                            parentEntry->MethodL(),
                            parentEntry->SequenceNumberL(),
                            recurrenceId,
                            CalCommon::EThisOnly);
    isNewEntry = true;
    CleanupStack::Pop(globalUid); // Ownership transferred
    CleanupStack::PopAndDestroy(parentEntry);
    CleanupStack::PopAndDestroy(parentGlobalUid);

    return entry; // Ownership transferred
}

CCalEntry* QOrganizerItemSymbianEngine::entryForItemL(const QOrganizerCollectionLocalId collectionId, QOrganizerItem *item, bool &isNewEntry) const
{
    // Try to find with local id
    CCalEntry *entry = findEntryL(collectionId, item->localId(), item->id().managerUri());

    // Not found. Try to find with globalUid
    if (!entry) {
        HBufC8* globalUid = OrganizerItemGuidTransform::guidLC(*item);

        entry = findEntryL(collectionId, *globalUid);
        // Not found? Create a new entry instance to be saved to the database
        if (!entry) {
            CCalEntry::TType type = OrganizerItemTypeTransform::entryTypeL(*item);
            entry = CCalEntry::NewL(type, globalUid, CCalEntry::EMethodAdd, 0);
            isNewEntry = true;
            CleanupStack::Pop(globalUid); // Ownership transferred to the new entry
            return entry;
        }
        CleanupStack::PopAndDestroy(globalUid);
    }
    return entry;
}

CCalEntry * QOrganizerItemSymbianEngine::findEntryL(const QOrganizerCollectionLocalId collectionId, QOrganizerItemLocalId localId, QString manageruri) const
{
    CCalEntry *entry(0);

    // There must be an existing entry if local id is provided
    if (localId) {
        // The item has a local id, check the item is from this manager
        if (manageruri == managerUri()) {
            // Fetch the item (will return NULL if the localid is not found)
            entry = entryViewL(collectionId)->FetchL(localId);
            if (!entry)
                User::Leave(KErrNotFound);
        } else {
            User::Leave(KErrArgument);
        }
    }

    // ownership transferred
    return entry;
}

CCalEntry * QOrganizerItemSymbianEngine::findEntryL(const QOrganizerCollectionLocalId collectionId, const TDesC8& globalUid) const
{
    CCalEntry *entry(0);

    if (globalUid.Length()) {
        // Search for an existing entry based on guid
        RPointerArray<CCalEntry> calEntryArray;
        entryViewL(collectionId)->FetchL(globalUid, calEntryArray);
        if (calEntryArray.Count()) {
            // take the first item in the array
            entry = calEntryArray[0];
            calEntryArray.Remove(0);
            calEntryArray.ResetAndDestroy();
        }
    }

    // ownership transferred
    return entry;
}

CCalEntry* QOrganizerItemSymbianEngine::findParentEntryLC(const QOrganizerCollectionLocalId collectionId, QOrganizerItem *item, const TDesC8& globalUid) const
{
    CCalEntry *parent(0);

    // Try to find with parent's local id
    QOrganizerItemInstanceOrigin origin = item->detail<QOrganizerItemInstanceOrigin>();
    if (origin.parentLocalId()) {
        // Fetch the item (will return NULL if the localid is not found)
        parent = entryViewL(collectionId)->FetchL(origin.parentLocalId()); // ownership transferred
        if (!parent)
            User::Leave(KErrInvalidOccurrence);
        CleanupStack::PushL(parent);
    // Try to find with globalUid
    } else if (globalUid.Length()) {
        parent = findEntryL(collectionId, globalUid);
        if (!parent)
            User::Leave(KErrInvalidOccurrence);
        CleanupStack::PushL(parent);

    } else {
        User::Leave(KErrInvalidOccurrence);
    }

    // Verify the item against parent
    if(parent->EntryTypeL() != OrganizerItemTypeTransform::entryTypeL(*item))
        User::Leave(KErrInvalidOccurrence);

    // Check for UID consistency for item with parentEntry
    if (!item->guid().isEmpty()
        && globalUid.Compare(parent->UidL())) {
        // Guid is not consistent with parentEntry UID
        User::Leave(KErrInvalidOccurrence);
    } else if(origin.parentLocalId()
        && (origin.parentLocalId() != parent->LocalUidL())) {
        // parentLocalId is not consistent with parentEntry localUID
        User::Leave(KErrInvalidOccurrence);
    }

    return parent;
}

bool QOrganizerItemSymbianEngine::removeItems(const QList<QOrganizerItemLocalId>& itemIds, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error)
{
    // Note: the performance would be probably better, if we had a separate
    // implementation for the case with a list of item ids that would
    // remove all the items

    QOrganizerItemChangeSet changeSet;

    for (int i(0); i < itemIds.count(); i++) {
        // Remove
        QOrganizerItemManager::Error removeError = QOrganizerItemManager::NoError;
        TRAPD(err, removeItemL(itemIds.at(i)));
        if (err != KErrNone) {
            transformError(err, &removeError);
            *error = removeError;
            if (errorMap)
                errorMap->insert(i, *error);
        } else {
            // Signals
            changeSet.insertRemovedItem(itemIds.at(i));
        }
    }

    // Emit changes
    changeSet.emitSignals(this);

    return *error == QOrganizerItemManager::NoError;
}

bool QOrganizerItemSymbianEngine::removeItem(const QOrganizerItemLocalId& organizeritemId, QOrganizerItemManager::Error* error)
{
    TRAPD(err, removeItemL(organizeritemId));
    if (err != KErrNone) {
        transformError(err, error);
    } else {
        // Signals
        QOrganizerItemChangeSet changeSet;
        changeSet.insertRemovedItem(organizeritemId);
        changeSet.emitSignals(this);
    }
    return *error == QOrganizerItemManager::NoError;
}

void QOrganizerItemSymbianEngine::removeItemL(const QOrganizerItemLocalId& organizeritemId)
{
    // TODO: How to remove item instances?

    // Fetch item
    // There is a bug in symbian calendar API. It will not report any error
    // when removing a nonexisting entry. So we need to fetch the item to see
    // if it really exists before trying to delete it.
    TCalLocalUid uid(organizeritemId);
    CCalEntry *calEntry(0);
    QOrganizerCollectionLocalId collectionLocalId(0);
    foreach(QOrganizerCollectionLocalId id, m_collections.keys()) {
        // TODO: instead of looping through entry views, get the collection id from
        // local id? (not certain that is the correct way, because the Qt API for
        // collections is still under development)
        calEntry = entryViewL(id)->FetchL(uid);
        if (calEntry) {
            collectionLocalId = id;
            break;
        }
    }
    if (!calEntry)
        User::Leave(KErrNotFound);
    CleanupStack::PushL(calEntry);
    entryViewL(collectionLocalId)->DeleteL(*calEntry);
    CleanupStack::PopAndDestroy(calEntry);
}

QList<QOrganizerItem> QOrganizerItemSymbianEngine::slowFilter(const QList<QOrganizerItem> &items, const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders) const
{
    QList<QOrganizerItem> filteredAndSorted;
    
    if (filter != QOrganizerItemInvalidFilter()) {
        foreach(const QOrganizerItem& item, items) {
            if (QOrganizerItemManagerEngine::testFilter(filter, item))
                QOrganizerItemManagerEngine::addSorted(&filteredAndSorted, item, sortOrders);
        }     
    } else {
	    // Only sort items.
        foreach(const QOrganizerItem& item, items) {
            QOrganizerItemManagerEngine::addSorted(&filteredAndSorted, item, sortOrders); 
        }
    }
    return filteredAndSorted;
}

#ifdef SYMBIAN_CALENDAR_V2
QOrganizerCollectionLocalId QOrganizerItemSymbianEngine::defaultCollectionId(QOrganizerItemManager::Error* error) const
{
    *error = QOrganizerItemManager::NoError;
    return m_defaultCollection.localId();
}

QList<QOrganizerCollectionLocalId> QOrganizerItemSymbianEngine::collectionIds(QOrganizerItemManager::Error* error) const
{
    QList<QOrganizerCollectionLocalId> ids;
    TRAPD(err, ids = collectionIdsL());
    transformError(err, error);
    return ids;    
}

QList<QOrganizerCollectionLocalId> QOrganizerItemSymbianEngine::collectionIdsL() const
{
    // TODO: refactor as function is no longer leaving
    QList<QOrganizerCollectionLocalId> ids;
    foreach (const OrganizerSymbianCollection &collection, m_collections)
        ids.append(collection.localId());
    return ids;
}

// Returns number of sessions currently open
int QOrganizerItemSymbianEngine::sessionCount() const
{
    return m_collections.count();
}

QList<QOrganizerCollection> QOrganizerItemSymbianEngine::collections(const QList<QOrganizerCollectionLocalId>& collectionIds, QOrganizerItemManager::Error* error) const
{
    QList<QOrganizerCollection> collections;
    TRAPD(err, collections = collectionsL(collectionIds));
    transformError(err, error);
    return collections;   
}

QList<QOrganizerCollection> QOrganizerItemSymbianEngine::collectionsL(const QList<QOrganizerCollectionLocalId>& collectionIds) const
    {
    QList<QOrganizerCollection> collections;

    int count(sessionCount());
    // Loop through open collections/sessions
    for (int i=0;  i < count; i++) {
        // Create a new collection to hold the data
        QOrganizerCollection collection;
        bool found(collectionL(i, collectionIds, collection));
        if (found) {
            collections.append(collection);
        }
    }

    // Nothing found?
    if (collections.isEmpty())
        User::Leave(KErrNotFound);

    return collections;
}

bool QOrganizerItemSymbianEngine::collectionL(const int 
    index, const QList<QOrganizerCollectionLocalId>& collectionIds, 
    QOrganizerCollection& collection) const
{
    CCalSession *session = m_collections.values()[index].calSession();
    
    // Get collection id
    QOrganizerCollectionLocalId localId(m_collections.values()[index].localId());
    
    // Find matching collection if id is provided
    if (!collectionIds.isEmpty()) {
        if (!collectionIds.contains(localId))
            return false;
    }
    
    // Read calendar info from session
    CCalCalendarInfo* calInfo = session->CalendarInfoL();
    CleanupStack::PushL(calInfo);
    collection.setMetaData(toMetaDataL(*calInfo));       
    CleanupStack::PopAndDestroy(calInfo);

    // Fetch successfull. Set collection id now
    QOrganizerCollectionId id;
    id.setManagerUri(managerUri());
    id.setLocalId(localId);
    collection.setId(id);

    return true;
}

bool QOrganizerItemSymbianEngine::saveCollection(QOrganizerCollection* collection, QOrganizerItemManager::Error* error)
{
    bool isNewCollection = true;
    if (collection->id().localId())
        isNewCollection = false;
    
    TRAPD(err, saveCollectionL(collection));
    transformError(err, error);
    
    if (*error == QOrganizerItemManager::NoError) {
        if (isNewCollection)
            emit collectionsAdded(QList<QOrganizerCollectionLocalId>() << collection->id().localId());
        // NOTE: collectionsChanged signal will be emitted from CalendarInfoChangeNotificationL
    }
    
    return (*error == QOrganizerItemManager::NoError);   
}

void QOrganizerItemSymbianEngine::saveCollectionL(QOrganizerCollection* collection)
{
    // Check manager uri if defined
    if (!collection->id().managerUri().isEmpty()) {
        if (collection->id().managerUri() != this->managerUri())
            User::Leave(KErrArgument); // uri does not match this manager
    }
    
    // Find existing collection
    QOrganizerCollectionLocalId localId = collection->id().localId();
    OrganizerSymbianCollection symbianCollection(this);
    if (localId) {
        if (m_collections.contains(localId))
            symbianCollection = m_collections[localId];
        else
            User::Leave(KErrArgument); // collection id was defined but was not found
    }
    
    // Convert metadata to cal info
    CCalCalendarInfo *calInfo = toCalInfoLC(collection->metaData());
    
    // Update modification time
    TTime currentTime;
    currentTime.HomeTime();
    setCalInfoPropertyL(calInfo, EModificationTime, currentTime);
    
    // Get filename from collection to be saved
    QString fileName = collection->metaData("FileName").toString();
    
    // Did we found an existing collection?
    if (!symbianCollection.isValid()) {

        // Set creation time
        setCalInfoPropertyL(calInfo, ECreationTime, currentTime);       
                
        // Create a new collection
        symbianCollection.openL(toPtrC16(fileName), calInfo);
        m_collections.insert(symbianCollection.localId(), symbianCollection);
        symbianCollection.createEntryViewL();
    }
    else {
        // Cannot allow changing the filename for an existing collection
        if (symbianCollection.fileName() != fileName)
            User::Leave(KErrArgument);

        // Update the existing collection
        symbianCollection.calSession()->SetCalendarInfoL(*calInfo);
    }
    
    CleanupStack::PopAndDestroy(calInfo);
    
    // Update id to the collection object
    QOrganizerCollectionId id;
    id.setManagerUri(managerUri());
    id.setLocalId(symbianCollection.localId());
    collection->setId(id);
}

bool QOrganizerItemSymbianEngine::removeCollection(const QOrganizerCollectionLocalId& collectionId, QOrganizerItemManager::Error* error)
{
    TRAPD(err, removeCollectionL(collectionId));
    transformError(err, error);
    if (*error == QOrganizerItemManager::NoError)
        emit collectionsRemoved(QList<QOrganizerCollectionLocalId>() << collectionId);
    return (*error == QOrganizerItemManager::NoError);
}

void QOrganizerItemSymbianEngine::removeCollectionL(const QOrganizerCollectionLocalId& collectionId)
{
    // Dont allow removing the default collection
    // TODO: Or should we allow this?
    if (collectionId == m_defaultCollection.localId())
        User::Leave(KErrAccessDenied);
    
    // Find collection
    foreach(OrganizerSymbianCollection collection, m_collections) {
        if (collection.localId() == collectionId) {
            
            // Get cal info
            CCalCalendarInfo *calInfo = collection.calSession()->CalendarInfoL();
            CleanupStack::PushL(calInfo);
                        
            // Remove the calendar file itself
            TRAPD(err, collection.calSession()->DeleteCalFileL(calInfo->FileNameL()));
            if( err == KErrInUse ) {
                
                // We cannot remove the calendar if we are not the only one
                // who has it open. So instead just disable it and mark it for
                // deletion. The native symbian calendar will remove it
                // during the next startup.
                // TODO: should we try to delete those during startup also?
                calInfo->SetEnabled( EFalse );
                setCalInfoPropertyL(calInfo, ESyncStatus, EFalse);
                setCalInfoPropertyL(calInfo, EMarkAsDelete, ETrue);
                
                // Update modification time
                TTime modificationTime;
                modificationTime.HomeTime();
                setCalInfoPropertyL(calInfo, EModificationTime, modificationTime);
                
                // TODO: Should we remove all entries also? 
                // Client might reopen the calendar before its really deleted.
                            
                // Update calendar info
                collection.calSession()->SetCalendarInfoL( *calInfo );
                }
            else {
                User::LeaveIfError(err);
            }
            CleanupStack::PopAndDestroy(calInfo);

            // Delete the collection
            m_collections.remove(collection.localId());
            return;
        }
    }
    User::Leave(KErrNotFound);
}
#endif // SYMBIAN_CALENDAR_V2

QMap<QString, QOrganizerItemDetailDefinition> QOrganizerItemSymbianEngine::detailDefinitions(const QString& itemType, QOrganizerItemManager::Error* error) const
{
    if (m_definition.isEmpty()) {
        // Get all the detail definitions from the base implementation
        m_definition = QOrganizerItemManagerEngine::schemaDefinitions();
        
        // Modify the base schema to match backend support
        m_itemTransform.modifyBaseSchemaDefinitions(m_definition);
    }
    
    // Check if we support the item type
    if (!m_definition.contains(itemType)) {
        *error = QOrganizerItemManager::NotSupportedError;
        return QMap<QString, QOrganizerItemDetailDefinition>();
    }
    
    *error = QOrganizerItemManager::NoError;
    return m_definition.value(itemType);
}

bool QOrganizerItemSymbianEngine::startRequest(QOrganizerItemAbstractRequest* req)
{
    /*
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
    return m_requestServiceProviderQueue->startRequest(req);
}

bool QOrganizerItemSymbianEngine::cancelRequest(QOrganizerItemAbstractRequest* req)
{
    /*
        Cancel an in progress async request.  If not possible, return false from here.
    */
    return m_requestServiceProviderQueue->cancelRequest(req);
}

bool QOrganizerItemSymbianEngine::waitForRequestFinished(QOrganizerItemAbstractRequest* req, int msecs)
{
    /*
        Wait for a request to complete (up to a max of msecs milliseconds).

        Return true if the request is finished (including if it was already).  False otherwise.

        You should really implement this function, if nothing else than as a delay, since clients
        may call this in a loop.

        It's best to avoid processing events, if you can, or at least only process non-UI events.
    */
    return m_requestServiceProviderQueue->waitForRequestFinished(req, msecs);
}

void QOrganizerItemSymbianEngine::requestDestroyed(QOrganizerItemAbstractRequest* req)
{
    /*
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
        m_requestServiceProviderQueue->requestDestroyed(req);
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
	Q_UNUSED(aError)
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

#ifdef SYMBIAN_CALENDAR_V2
void QOrganizerItemSymbianEngine::CalendarInfoChangeNotificationL(RPointerArray<CCalFileChangeInfo>& aCalendarInfoChangeEntries)
{
    // Loop through changes
    int changeCount = aCalendarInfoChangeEntries.Count();
    for (int i=0; i<changeCount; i++) {

        // Get changed calendar file name
        const TDesC& fileName = aCalendarInfoChangeEntries[i]->FileNameL();
        
        // Try to find matching collection
        OrganizerSymbianCollection collection(this);
        foreach (OrganizerSymbianCollection c, m_collections) {
            if (c.fileName() == toQString(fileName))
                collection = c; 
        }
        
        // Check change type
        QList<QOrganizerCollectionLocalId> ids;
        switch (aCalendarInfoChangeEntries[i]->ChangeType())
        {
        case ECalendarFileCreated:
            if (!collection.isValid()) {
                // A calendar file has been created but not by this manager instance.
                collection.openL(fileName);
                m_collections.insert(collection.localId(), collection);
                collection.createEntryViewL();
                emit collectionsAdded(ids << collection.localId());
            }
            break;
            
        case ECalendarFileDeleted:
            if (collection.isValid()) {
                // A calendar file has been removed but not by this manager instance.
                QOrganizerCollectionLocalId id = collection.localId();
                m_collections.remove(id);
                emit collectionsRemoved(ids << id);
            }
            break;
            
        case ECalendarInfoCreated:
            break;
            
        case ECalendarInfoUpdated:
            if (collection.isValid()) {
                if (collection.isMarkedForDeletionL()) {
                    // A calendar file has been marked for deletion but not by this manager instance
                    QOrganizerCollectionLocalId id = collection.localId();
                    m_collections.remove(id);
                    emit collectionsRemoved(ids << id);
                    // TODO: Try removing the calendar file?                    
                } else {
                    emit collectionsChanged(ids << collection.localId());
                }
            } else {
                // Calendar file has been modified but we do not have a session to it.
                collection.openL(fileName);             
                
                // Is it marked for deletion?
                if (collection.isMarkedForDeletionL()) {
                    // Something has modified a calendar which is marked for deletion.
                } else {
                    // A calendar file which was marked for deletion has been taken into use again.
                    m_collections.insert(collection.localId(), collection);
                    collection.createEntryViewL();
                    emit collectionsAdded(ids << collection.localId());
                }
            }
            break;
            
        case ECalendarInfoDeleted:
            break;
            
        default:
            break;
        }
    }
}
#endif

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
        // KErrInvalidOccurrence is a special error code defined for Qt
        // Organizer API implementation purpose only
        case KErrInvalidOccurrence:
        {
            *qtError = QOrganizerItemManager::InvalidOccurrenceError;
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
