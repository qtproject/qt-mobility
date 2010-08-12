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

// user includes
#include "qorganizersymbian_p.h"
#include "qtorganizer.h"
#include "organizeritemdetailtransform.h"
#include "organizeritemtypetransform.h"
#include "organizeritemguidtransform.h"
#include "qorganizeritemrequestqueue.h"

//QTM_USE_NAMESPACE

// Constants
const int KSingleCount = 1;

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
        
    m_activeSchedulerWait = new CActiveSchedulerWait();    

    m_instanceView = CCalInstanceView::NewL(*m_calSession, *this);
    
    // TODO: The calendar session may take some time to initialize which would
    // make an UI app using symbian backend freeze. To be refactored.
    m_activeSchedulerWait->Start();
    
    // Create entry view (creation is synchronized with CActiveSchedulerWait)
    m_entryView = CCalEntryView::NewL(*m_calSession, *this);   

    m_activeSchedulerWait->Start();

    m_requestServiceProviderQueue = QOrganizerItemRequestQueue::instance(*this);
    // Create change notification filter
    TCalTime minTime;
    minTime.SetTimeUtcL(TCalTime::MinTime());
    TCalTime maxTime;
    maxTime.SetTimeUtcL(TCalTime::MaxTime());
    CalCommon::TCalTimeRange calTimeRange(minTime, maxTime);
    CCalChangeNotificationFilter *filter = CCalChangeNotificationFilter::NewL(MCalChangeCallBack2::EChangeEntryAll, true, calTimeRange);
    
    // Start listening to calendar events
    m_calSession->StartChangeNotification(*this, *filter);
    
    // Cleanup
    delete filter;
}

QOrganizerItemSymbianEngine::~QOrganizerItemSymbianEngine()
{
    /* TODO clean up your stuff.  Perhaps a QScopedPointer or QSharedDataPointer would be in order */
    m_calSession->StopChangeNotification();

	delete m_requestServiceProviderQueue;
    delete m_activeSchedulerWait;
    delete m_entryView;
    delete m_instanceView;
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
    QList<QOrganizerItem> occurrenceList;
    
    // Parent item should be an Event or a Todo.
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
        CalCommon::TCalViewFilter filter;
        //use maximum end date if only count is present.
        if ((!periodEnd.isValid()) && (maxCount > 0)) {
              TCalTime endTime; 
              endTime.SetTimeUtcL(TCalTime::MaxTime());
              endDateTime = OrganizerItemDetailTransform::toQDateTimeL(endTime);  
        }
        
        if (generator.type()== QOrganizerItemType::TypeEvent) {
            filter = CalCommon::EIncludeAppts; 
        }
        
        TRAPD(err, m_instanceView->FindInstanceL(instanceList,filter,
                                   CalCommon::TCalTimeRange(OrganizerItemDetailTransform::toTCalTimeL(periodStart),
                                   OrganizerItemDetailTransform::toTCalTimeL(endDateTime))
                                   ));
            
        transformError(err, error);
   
        if (*error == QOrganizerItemManager::NoError) {  
            int count(instanceList.Count()); 
            // Convert calninstance list to  QOrganizerEventOccurrence and add to QOrganizerItem list                 
            for( int index=0; index < count;index++ ) {
                 QOrganizerItem itemInstance;
                 CCalInstance* calInstance = (instanceList)[index];
                 
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
            }           
        }
        instanceList.ResetAndDestroy();
        
    } else {
        *error = QOrganizerItemManager::BadArgumentError;
    }
    
    return occurrenceList;
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
    CCalEntry *calEntry = m_entryView->FetchL(uid);
    if (!calEntry) {
        User::Leave(KErrNotFound); // Leave with KErrNotFound as to indicate that the entry 
        // is not present in the database
    }
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
    
    QOrganizerItemChangeSet changeSet;
    
    for (int i(0); i < items->count(); i++) {
        QOrganizerItem item = items->at(i);
        
        // Validate & save
        QOrganizerItemManager::Error saveError;
        if (validateItem(item, &saveError)) {
            TRAPD(err, saveItemL(&item, &changeSet));
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


bool QOrganizerItemSymbianEngine::saveItem(QOrganizerItem *item, QOrganizerItemManager::Error* error)
{
    // Validate & save
    if (validateItem(*item, error)) {
        QOrganizerItemChangeSet changeSet;
        TRAPD(err, saveItemL(item, &changeSet));
        if ((item->type()== QOrganizerItemType::TypeEventOccurrence || 
            item->type()== QOrganizerItemType::TypeTodoOccurrence)&& 
            err == KErrArgument) {
            *error = QOrganizerItemManager::InvalidOccurrenceError; 
        } else {
            transformError(err, error);
	    }	
        changeSet.emitSignals(this);
    }
    return *error == QOrganizerItemManager::NoError;
}

void QOrganizerItemSymbianEngine::saveItemL(QOrganizerItem *item, QOrganizerItemChangeSet *changeSet)
{
    // Check local id
    bool isNewEntry = true;    
    CCalEntry *entry(NULL);
    
    // Search if the entry is existing based on local id   
    if (item->localId()) {
        // Don't allow saving with local id defined unless the item is from this manager.
        if (item->id().managerUri() != managerUri())
            User::Leave(KErrArgument);        
        entry = m_entryView->FetchL(item->localId());   
        isNewEntry = false;
    }
    
    if (isNewEntry) {
        if ((item->type()== QOrganizerItemType::TypeEventOccurrence || 
             item->type()== QOrganizerItemType::TypeTodoOccurrence)) {
             entry = createEntryToSaveItemInstanceL(item);
        } else {
            RPointerArray<CCalEntry> calEntryArray;
            // Get guid from item. New guid is generated if empty.
            HBufC8* globalUid = OrganizerItemGuidTransform::guidLC(*item);

            // Search for an existing entry based on guid
            if (!item->guid().isEmpty()) {
                m_entryView->FetchL(*globalUid, calEntryArray);
                if (calEntryArray.Count()== KSingleCount) {
                    //existing entry found for updating.
                    entry = calEntryArray[0];
                    calEntryArray.Close();
                    isNewEntry = false;
                } else if(calEntryArray.Count() > KSingleCount) {
                    //invalid item arguments.
                    CleanupStack::PopAndDestroy(globalUid);  
                    calEntryArray.ResetAndDestroy();
                    User::Leave(KErrArgument);
                }     
            } 
            if (isNewEntry) {       
                // Create a new entry to save.                      
                CCalEntry::TType type = OrganizerItemTypeTransform::entryTypeL(*item);
                CCalEntry::TMethod method = CCalEntry::EMethodAdd;
                TInt seqNum = 0; 
                entry = CCalEntry::NewL(type, globalUid, method, seqNum);
                // ownership passed?
            }
            CleanupStack::Pop(globalUid);  
        }
    }
    CleanupStack::PushL(entry);

    // Transform QOrganizerItem -> CCalEntry    
    m_itemTransform.toEntryL(*item, entry);
    
    // Save entry
    RPointerArray<CCalEntry> entries;
    CleanupClosePushL(entries);
    entries.AppendL(entry);
    TInt count(0);
    if (!isNewEntry) {
        entry->SetLastModifiedDateL();
    }
    m_entryView->StoreL(entries, count);

    if (count != KSingleCount) {
        // The documentation states about count "On return, this
        // contains the number of entries which were successfully stored".
        // So it is not clear which error caused storing the entry to fail
        // -> let's use the "one-error-fits-all" error code KErrGeneral.
        User::Leave(KErrGeneral);
    }
    
    // Transform details that are available/updated after saving    
    m_itemTransform.toItemPostSaveL(*entry, item);
    
    // Update local id
    QOrganizerItemId itemId;
    TCalLocalUid localUid = entry->LocalUidL();
    itemId.setLocalId(localUid);
    itemId.setManagerUri(managerUri());
    item->setId(itemId);

    // Cleanup
    CleanupStack::PopAndDestroy(&entries);
    CleanupStack::PopAndDestroy(entry);   
    // Update change set
    if (changeSet) {
        if (isNewEntry)
            changeSet->insertAddedItem(item->localId());
        else
            changeSet->insertChangedItem(item->localId());
    }
}

bool QOrganizerItemSymbianEngine::removeItems(const QList<QOrganizerItemLocalId>& itemIds, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error)
{
    // TODO: the performance would be probably better, if we had a separate
    // implementation for the case with a list of item ids that would
    // remove all the items
    
    QOrganizerItemChangeSet changeSet;
    
    for (int i(0); i < itemIds.count(); i++) {
        
        // Remove
        QOrganizerItemManager::Error removeError;
        TRAPD(err, removeItemL(itemIds.at(i), &changeSet));
        transformError(err, &removeError);
        
        // Check error
        if (removeError != QOrganizerItemManager::NoError) {
            *error = removeError;
            if (errorMap)
                errorMap->insert(i, *error);
        }
    }
    
    // Emit changes
    changeSet.emitSignals(this);
    
    return *error == QOrganizerItemManager::NoError;
}

bool QOrganizerItemSymbianEngine::removeItem(const QOrganizerItemLocalId& organizeritemId, QOrganizerItemManager::Error* error)
{
    QOrganizerItemChangeSet changeSet;
    TRAPD(err, removeItemL(organizeritemId, &changeSet));
    transformError(err, error);
    changeSet.emitSignals(this);
    return *error == QOrganizerItemManager::NoError;
}

void QOrganizerItemSymbianEngine::removeItemL(const QOrganizerItemLocalId& organizeritemId, QOrganizerItemChangeSet *changeSet)
{
    // TODO: DoesNotExistError should be used if the id refers to a non existent item.
    // TODO: How to remove item instances?
    int sucessCount(0);
    deleteItemL(organizeritemId, sucessCount);
    // Update change set
    changeSet->insertRemovedItem(organizeritemId);
}

void QOrganizerItemSymbianEngine::deleteItemL( 
        const QOrganizerItemLocalId& organizeritemId, 
        int& sucessCount)
{
    // Remove
    RArray<TCalLocalUid> ids;
    CleanupClosePushL(ids);
    ids.AppendL(TCalLocalUid(organizeritemId));
    m_entryView->DeleteL(ids, sucessCount);
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

void QOrganizerItemSymbianEngine::modifyDetailDefinitionsForEvent() const
{
    // Remove all the details for an event not supported on Symbian
    m_definition[QOrganizerItemType::TypeEvent].remove(QOrganizerItemComment::DefinitionName);
    m_definition[QOrganizerItemType::TypeEvent].remove(QOrganizerItemInstanceOrigin::DefinitionName);
    m_definition[QOrganizerItemType::TypeEvent].remove(QOrganizerTodoProgress::DefinitionName);
    m_definition[QOrganizerItemType::TypeEvent].remove(QOrganizerTodoTimeRange::DefinitionName);
    m_definition[QOrganizerItemType::TypeEvent].remove(QOrganizerJournalTimeRange::DefinitionName);
}

void QOrganizerItemSymbianEngine::modifyDetailDefinitionsForEventOccurrence() const
{
    // Remove all the details for an event occurrence not supported on Symbian
    m_definition[QOrganizerItemType::TypeEventOccurrence].remove(QOrganizerItemComment::DefinitionName);
    m_definition[QOrganizerItemType::TypeEventOccurrence].remove(QOrganizerItemRecurrence::DefinitionName);
    m_definition[QOrganizerItemType::TypeEventOccurrence].remove(QOrganizerTodoProgress::DefinitionName);
    m_definition[QOrganizerItemType::TypeEventOccurrence].remove(QOrganizerTodoTimeRange::DefinitionName);
    m_definition[QOrganizerItemType::TypeEventOccurrence].remove(QOrganizerJournalTimeRange::DefinitionName);
}

void QOrganizerItemSymbianEngine::modifyDetailDefinitionsForTodo() const
{
    // Remove all the details for a to-do not supported on Symbian
    m_definition[QOrganizerItemType::TypeTodo].remove(QOrganizerItemComment::DefinitionName);
    m_definition[QOrganizerItemType::TypeTodo].remove(QOrganizerEventTimeRange::DefinitionName);
    m_definition[QOrganizerItemType::TypeTodo].remove(QOrganizerItemInstanceOrigin::DefinitionName);
    m_definition[QOrganizerItemType::TypeTodo].remove(QOrganizerJournalTimeRange::DefinitionName);
    m_definition[QOrganizerItemType::TypeTodo].remove(QOrganizerItemLocation::DefinitionName);
}

void QOrganizerItemSymbianEngine::modifyDetailDefinitionsForTodoOccurrence() const
{
    // Remove all the details for a to-do occurrence not supported on Symbian
    m_definition[QOrganizerItemType::TypeTodoOccurrence].remove(QOrganizerItemComment::DefinitionName);
    m_definition[QOrganizerItemType::TypeTodoOccurrence].remove(QOrganizerItemRecurrence::DefinitionName);
    m_definition[QOrganizerItemType::TypeTodoOccurrence].remove(QOrganizerEventTimeRange::DefinitionName);
    m_definition[QOrganizerItemType::TypeTodoOccurrence].remove(QOrganizerJournalTimeRange::DefinitionName);
    m_definition[QOrganizerItemType::TypeTodoOccurrence].remove(QOrganizerItemLocation::DefinitionName);
}

void QOrganizerItemSymbianEngine::modifyDetailDefinitionsForNote() const
{
    // Remove all the details for a not not supported on Symbian
    m_definition[QOrganizerItemType::TypeNote].remove(QOrganizerItemDisplayLabel::DefinitionName);
    m_definition[QOrganizerItemType::TypeNote].remove(QOrganizerItemComment::DefinitionName);
    m_definition[QOrganizerItemType::TypeNote].remove(QOrganizerItemRecurrence::DefinitionName);
    m_definition[QOrganizerItemType::TypeNote].remove(QOrganizerEventTimeRange::DefinitionName);
    m_definition[QOrganizerItemType::TypeNote].remove(QOrganizerItemPriority::DefinitionName);
    m_definition[QOrganizerItemType::TypeNote].remove(QOrganizerItemLocation::DefinitionName);
    m_definition[QOrganizerItemType::TypeNote].remove(QOrganizerItemInstanceOrigin::DefinitionName);
    m_definition[QOrganizerItemType::TypeNote].remove(QOrganizerTodoProgress::DefinitionName);
    m_definition[QOrganizerItemType::TypeNote].remove(QOrganizerTodoTimeRange::DefinitionName);
    m_definition[QOrganizerItemType::TypeNote].remove(QOrganizerJournalTimeRange::DefinitionName);
}

void QOrganizerItemSymbianEngine::modifyDetailDefinitionsForJournal() const
{
    // Journal is not supported on Symbian. Remove the type itself
    m_definition.remove(QOrganizerItemType::TypeJournal);
}

CCalEntry* QOrganizerItemSymbianEngine::createEntryToSaveItemInstanceL(QOrganizerItem *item)
{  
    CCalEntry * entry(NULL);
    CCalEntry *parentEntry(NULL);
    HBufC8* globalUid(NULL);
    
    QOrganizerItemInstanceOrigin origin = item->detail<QOrganizerItemInstanceOrigin>();
   
    //Search parent based on guid
    globalUid = OrganizerItemGuidTransform::guidLC(*item);    
    if (!item->guid().isEmpty()) {
        RPointerArray<CCalEntry> calEntryArray;
        m_entryView->FetchL(*globalUid, calEntryArray);
    
        if (calEntryArray.Count()== KSingleCount) {
           //single count hence set it to the parent. 
           parentEntry = calEntryArray[0];
           calEntryArray.Close();
        } else {
		    calEntryArray.ResetAndDestroy();
		}
    }
    // Search on the basis of parentLocalId
    if (!parentEntry && origin.parentLocalId()) {        
        //another instance of same entry is modified.
        //so search parent entry by parentlocalId. 
        parentEntry = m_entryView->FetchL(origin.parentLocalId());    
    } 
       
    if (parentEntry && checkForValidParentEntryL(item,parentEntry)) {
        CleanupStack::PushL(parentEntry);
        //create an exceptional entry to save                
        TCalTime recurrenceId;
        recurrenceId = OrganizerItemDetailTransform::toTCalTimeL(origin.originalDate());
         
        // create the new child entry now
        entry = CCalEntry::NewL( parentEntry->EntryTypeL(),globalUid ,
                                 parentEntry->MethodL(),
                                 parentEntry->SequenceNumberL(),
                                 recurrenceId,CalCommon::EThisOnly );
       
        entry->ClearRepeatingPropertiesL();
        entry->SetLocalUidL( TCalLocalUid( 0 ) );
        CleanupStack::PopAndDestroy(parentEntry);
    } else {
        CleanupStack::PopAndDestroy(globalUid);
        delete parentEntry;
        User::Leave(KErrArgument);
    }
          
    CleanupStack::Pop(globalUid);
    return entry;    
}

bool QOrganizerItemSymbianEngine::checkForValidParentEntryL(QOrganizerItem *item , CCalEntry *parentEntry)
{    
    QOrganizerItemInstanceOrigin origin;
        bool validParentEntry = true;
    origin = item->detail<QOrganizerItemInstanceOrigin>();
    HBufC8* globalUid  = OrganizerItemGuidTransform::guidLC(*item);
    
    //Check if item type is consistent with parentEntry type
    switch(parentEntry->EntryTypeL()) 
    {
        case CCalEntry::EAppt:
        {    
            if (item->type()!= QOrganizerItemType::TypeEventOccurrence) {            
                // For an eventOccurrence the parentEntry type should be EAppt
                validParentEntry = false;
            }    
            break;
        }
        case CCalEntry::ETodo:
        {    
            if (item->type()!= QOrganizerItemType::TypeTodoOccurrence) {
                // For an todoOccurrence the parentEntry type should be ETodo
                validParentEntry = false;
            }    
            break;
        }    
    }
    //Check for UID  consistency for item with parentEntry.
    if (!item->guid().isEmpty() && 
       (globalUid->Compare(parentEntry->UidL()))) {
        //Guid is not consistent with parentEntry UID.
        validParentEntry = false;
    } else if(origin.parentLocalId() && 
        (origin.parentLocalId()!= parentEntry->LocalUidL())) {
        // parentLocalId is not consistent with parentEntry localUID.
        validParentEntry = false;
    }
    CleanupStack::PopAndDestroy(globalUid);
    return validParentEntry;        
}

QMap<QString, QOrganizerItemDetailDefinition> QOrganizerItemSymbianEngine::detailDefinitions(const QString& itemType, QOrganizerItemManager::Error* error) const
{
    // Get all the detail definitions from the base implementation
    if (m_definition.isEmpty()) {
        m_definition = QOrganizerItemManagerEngine::schemaDefinitions();
        // Add or remove definitions based on the Symbian offering
        modifyDetailDefinitionsForEvent();
        modifyDetailDefinitionsForEventOccurrence();
        modifyDetailDefinitionsForTodo();
        modifyDetailDefinitionsForTodoOccurrence();
        modifyDetailDefinitionsForNote();
        modifyDetailDefinitionsForJournal();
    }
    
    *error = QOrganizerItemManager::NoError;
    return m_definition.value(itemType);
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

/*!
 * From MCalChangeCallBack2
 */
void QOrganizerItemSymbianEngine::CalChangeNotification(RArray<TCalChangeEntry>& aChangeItems)
{
    // NOTE: We will not be notified of a change if we are the source. So these events are
    // caused by something else than our manager instance.
    
    QOrganizerItemChangeSet changeSet;
    
    int count = aChangeItems.Count();
    for (int i=0; i<count; i++) 
    {
        QOrganizerItemLocalId entryId = QOrganizerItemLocalId(aChangeItems[i].iEntryId);
        switch(aChangeItems[i].iChangeType)
        {
            case MCalChangeCallBack2::EChangeAdd:       
                changeSet.insertAddedItem(entryId);
                break;
                
            case MCalChangeCallBack2::EChangeDelete:
                changeSet.insertRemovedItem(entryId);
                break;
                
            case MCalChangeCallBack2::EChangeModify:
                changeSet.insertChangedItem(entryId);
                break;
                
            case MCalChangeCallBack2::EChangeUndefined:
                // fallthrough
            default: 
                changeSet.setDataChanged(true);
                break;
        }
    }
    
    changeSet.emitSignals(this);
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

CCalEntryView* QOrganizerItemSymbianEngine::entryView()
{
    return m_entryView;
}
