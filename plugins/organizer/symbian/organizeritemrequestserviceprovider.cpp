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

// System includes
#include <calentry.h> // CCalEntry
#include <calentryview.h> // CCalEntryView

// User includes
#include "organizeritemrequestserviceprovider.h"
#include "organizerItemTransform.h"
#include "organizerItemGuidTransform.h"
#include "organizerItemTypeTransform.h"
#include "qorganizersymbian_p.h"


// Static two phase construction
COrganizerItemRequestsServiceProvider* COrganizerItemRequestsServiceProvider::NewL(QOrganizerItemSymbianEngine& aOrganizerItemManagerEngine)
    {
    COrganizerItemRequestsServiceProvider* self(new (ELeave) COrganizerItemRequestsServiceProvider(aOrganizerItemManagerEngine));
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Basic first phase constructor
COrganizerItemRequestsServiceProvider::COrganizerItemRequestsServiceProvider(QOrganizerItemSymbianEngine& aOrganizerItemManagerEngine) : 
        CActive(EPriorityNormal), 
        iOrganizerItemManagerEngine(aOrganizerItemManagerEngine),
        iReq(NULL)
    {
    CActiveScheduler::Add(this);
    }

// Second phase constructor
void COrganizerItemRequestsServiceProvider::ConstructL()
    {
    iOrganizerItemTransform = new (ELeave) OrganizerItemTransform;
    }

// Destructor/Cleanup
COrganizerItemRequestsServiceProvider::~COrganizerItemRequestsServiceProvider()
    {
    // Cancel ongoing request, if any
    Cancel();
    // Cleanup
    iCalEntryList.Close();
    delete iOrganizerItemTransform;
    }

// Start processing aReq request to be processed asynchronously
TBool COrganizerItemRequestsServiceProvider::StartRequest(QOrganizerItemAbstractRequest* aReq)
    {
    // Change the state of the request and emit signal
    QOrganizerItemManagerEngine::updateRequestState(aReq, QOrganizerItemAbstractRequest::ActiveState);
    TRequestStatus* pStat = &iStatus;
    if (!IsActive())
		{
        // Save asychronous request, start the timer and then return
        iReq = aReq;
		SetActive();
        User::RequestComplete(pStat, KErrNone);
		return ETrue;
		}
	else
        {
        // Another asynchronous request is already going on so this can not be taken
        // until we implement a queue
        return EFalse;
		}
    
    }
    
// Cancel an ongoing asynchronous request
TBool COrganizerItemRequestsServiceProvider::CancelRequest()
    {
    // Stop the request
    Cancel();
    // Change the state of the request and emit signal
    QOrganizerItemManagerEngine::updateRequestState(iReq, QOrganizerItemAbstractRequest::CanceledState);
    
    return ETrue;
    }

TBool COrganizerItemRequestsServiceProvider::waitForRequestFinished(TTimeIntervalMicroSeconds32 aInterval)
    {
    if (aInterval.Int() > 0)
        {
        // Block for aInterval microseconds
        }
    else
        {
        // Block forever
        }
    // Return false as its yet to be fully implemented
    return false;
    }

void COrganizerItemRequestsServiceProvider::RunL()
    {
    // Request type
    QOrganizerItemAbstractRequest::RequestType requestType(iReq->type());
    switch (requestType) 
        {
        case QOrganizerItemAbstractRequest::ItemInstanceFetchRequest: 
            {
            break;
            }
        case QOrganizerItemAbstractRequest::ItemFetchRequest: 
            {
            FetchItemsL();
            break;
            }
        case QOrganizerItemAbstractRequest::ItemLocalIdFetchRequest: 
            {
            break;
            }
        case QOrganizerItemAbstractRequest::ItemRemoveRequest: 
            {
            break;
            }
        case QOrganizerItemAbstractRequest::ItemSaveRequest: 
            {
            SaveItemsL();
            break;
            }
        case QOrganizerItemAbstractRequest::DetailDefinitionFetchRequest: 
            {
            break;
            }
        case QOrganizerItemAbstractRequest::DetailDefinitionRemoveRequest: 
            {
            break;
            }
        case QOrganizerItemAbstractRequest::DetailDefinitionSaveRequest: 
            {
            break;
            }
        default: 
            {
            // Not implemented yet
            }
        }
    }

// Save item/items
void COrganizerItemRequestsServiceProvider::SaveItemsL()
    {
    QList<QOrganizerItem> itemList;
    itemList.append(((QOrganizerItemSaveRequest*)(iReq))->items());
    
    RPointerArray<CCalEntry> entryPointerArray;
    CleanupClosePushL(entryPointerArray);

    TBool isNewEntry(ETrue);
    
    TInt count(itemList.count());
    
    for(TInt index(0); index < count; index++) 
        {
        QOrganizerItem item = itemList[index];
        
        // Get guid from item. New guid is generated if empty.
        HBufC8* globalUid = OrganizerItemGuidTransform::guidLC(item);
        
        // If guid was defined in item check if it matches to something
        if (!item.guid().isEmpty()) 
            {
            RPointerArray<CCalEntry> calEntryArray;
            CleanupClosePushL(calEntryArray);
            iOrganizerItemManagerEngine.entryView()->FetchL(*globalUid, calEntryArray);
            if (calEntryArray.Count())
                {
                isNewEntry = EFalse; // Found at least one existing entry with this guid
                }
            CleanupStack::PopAndDestroy(&calEntryArray);
            }
        
        // Create entry
        CCalEntry::TType type = OrganizerItemTypeTransform::entryTypeL(item);
        CCalEntry::TMethod method = CCalEntry::EMethodAdd;
        TInt seqNum = 0;
    
        CCalEntry *entry = CCalEntry::NewL(type, globalUid, method, seqNum);
        CleanupStack::Pop(globalUid);
        CleanupStack::PushL(entry);
       // Use old local id if we are updating an entry
        if (!isNewEntry)
            {
            entry->SetLocalUidL(TCalLocalUid(item.localId()));
            }

        // Transform QOrganizerItem -> CCalEntry    
        iOrganizerItemTransform->toEntryL(item, entry);
        
        // Save entry
        entryPointerArray.AppendL(entry);
        CleanupStack::Pop(entry);
        }   
     
    TInt noOfEntriesToBeSaved(entryPointerArray.Count());
    TInt entryCount(0);
    
    // Store entry in agenda server db
    iOrganizerItemManagerEngine.entryView()->StoreL(entryPointerArray, entryCount);
    
    QOrganizerItemManager::Error error;
    error = QOrganizerItemManager::NoError;
    QMap<int, QOrganizerItemManager::Error> errorMap;
    if (entryCount != noOfEntriesToBeSaved) 
        {
        // Some of the entries are not save, but the error is not know
        // So set the error to General Error and transform this error 
        // to corresponding Qt error
        iOrganizerItemManagerEngine.transformError(KErrGeneral, &error);
        // Regenerate the entryPointerArray to contain only the emtries saved to agenda server
        }
    
    // Transform details that are available/updated after saving 
    for (TInt index(0); index < entryCount; index++)
        {
        iOrganizerItemTransform->toItemPostSaveL(*(entryPointerArray[index]), &(itemList[index]));
        }
    // Clear iItemList for any obsolete data
    iItemList.clear();
    iItemList.append(itemList);
    
    CleanupStack::PopAndDestroy(&entryPointerArray);
 
    QOrganizerItemManagerEngine::updateItemSaveRequest((QOrganizerItemSaveRequest*)(iReq), iItemList, error, errorMap, QOrganizerItemAbstractRequest::FinishedState);
    }

void COrganizerItemRequestsServiceProvider::FetchItemsL()
    {
    QOrganizerItemFilter filter = ((QOrganizerItemFetchRequest*)iReq)->filter();
    QList<QOrganizerItemSortOrder> sortOrder= ((QOrganizerItemFetchRequest*)iReq)->sorting();
    // Fetch hint is not supported as of now
    QOrganizerItemFetchHint fetchHint = ((QOrganizerItemFetchRequest*)iReq)->fetchHint();
    
    // Get the filter type
    QOrganizerItemFilter::FilterType filterType = filter.type();

    switch (filterType)
        {
        case QOrganizerItemFilter::LocalIdFilter :
            {
            FetchItemsByLocalIdsL(filter, fetchHint);
            }
            break;
         default :
            {
            FetchItemsandFilterL(filter, sortOrder, fetchHint);
            }
            break;
        }
    }

// Fetch Entries by local Ids
void COrganizerItemRequestsServiceProvider::FetchItemsByLocalIdsL(QOrganizerItemFilter& filter, QOrganizerItemFetchHint& fetchHint)
    {
    // Get the local id filter
    QOrganizerItemLocalIdFilter localIdFilter(filter);
    // Get the local ids for the entries to be fetched
    QList<QOrganizerItemLocalId> ids = localIdFilter.ids();
    // Fetch all the entries for the ids in loop
    TInt count(ids.count());
    // Clear the items list before appending items in it
    iItemList.clear();
    
    QOrganizerItemManager::Error error;
    
    for (int index(0); index < count; index++) 
        {
        QOrganizerItem item = iOrganizerItemManagerEngine.item(QOrganizerItemLocalId(ids[index]), fetchHint, &error);
        if (error != QOrganizerItemManager::NoError)
            {
            // Change state to finish and update the results whatsoever availabel
            // If index < count, fetch rest of the items and keep on appending to 
            // iItemList
            // Exit out of the loop
            break;
            }
        iItemList << item;
        }
    QOrganizerItemManagerEngine::updateItemFetchRequest((QOrganizerItemFetchRequest*)(iReq), iItemList, error, QOrganizerItemAbstractRequest::FinishedState);
    }

// Fetch items/entries by details
void COrganizerItemRequestsServiceProvider::FetchItemsandFilterL(QOrganizerItemFilter& filter, QList<QOrganizerItemSortOrder>& sortOrder, QOrganizerItemFetchHint& fetchHint)
    {
    QOrganizerItemManager::Error error;
    iItemList.clear();
    iItemList = iOrganizerItemManagerEngine.items(filter, sortOrder, fetchHint, &error);
    QOrganizerItemManagerEngine::updateItemFetchRequest((QOrganizerItemFetchRequest*)(iReq), iItemList, error, QOrganizerItemAbstractRequest::FinishedState);
    }

// Called by Cancel()
void COrganizerItemRequestsServiceProvider::DoCancel()
    {
    // Clean up
    }

// Call if RunL leaves
TInt COrganizerItemRequestsServiceProvider::RunError(TInt /*aError*/)
    {
    // State change, RunL is left while completing an asynchronous request.
    QOrganizerItemManagerEngine::updateRequestState(iReq, QOrganizerItemAbstractRequest::FinishedState);

    // Generate error maps
    
    // Handle the error here
    return KErrNone;
    }

