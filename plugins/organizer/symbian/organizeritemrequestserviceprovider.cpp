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
        iReq(NULL), iNoOfItems(0), iIndex(0)
    {
    CActiveScheduler::Add(this);
    }

// Second phase constructor
void COrganizerItemRequestsServiceProvider::ConstructL()
    {
    }

// Destructor/Cleanup
COrganizerItemRequestsServiceProvider::~COrganizerItemRequestsServiceProvider()
    {
    // Cancel ongoing request, if any
    Cancel();
    // Cleanup
    }

// Start processing aReq request to be processed asynchronously
TBool COrganizerItemRequestsServiceProvider::StartRequest(QOrganizerItemAbstractRequest* aReq)
    {
    if (!IsActive())
        {
        // Change the state of the request and emit signal
        QOrganizerItemManagerEngine::updateRequestState(aReq, QOrganizerItemAbstractRequest::ActiveState);

        // Save asynchronous request, start the timer and then return
        iReq = aReq;
        iIndex = 0; // Reset the index as new request is being processed
        QOrganizerItemAbstractRequest::RequestType requestType(iReq->type());
        switch (requestType)
            {
            case QOrganizerItemAbstractRequest::ItemRemoveRequest:
                {
                iItemIds.clear();
                iItemIds.append(((QOrganizerItemRemoveRequest*)(iReq))->itemIds());
                iNoOfItems = iItemIds.count();
                }
                break;
            case QOrganizerItemAbstractRequest::ItemSaveRequest:
                {
                iItemList.clear();
                iItemList.append(((QOrganizerItemSaveRequest*)(iReq))->items());
                iNoOfItems = iItemList.count();
                }
                break;
            }

        SelfComplete();
        return ETrue;
        }
    else
        {
        // Another asynchronous request is already going on so this request can not be taken
        return EFalse;
        }
    
    }
    
// Cancel an ongoing asynchronous request
TBool COrganizerItemRequestsServiceProvider::CancelRequest()
    {
    // Stop the request, Cancel() would call doCancel(), which updates the 
    // request status
    Cancel();
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
            
            }
            break;
        case QOrganizerItemAbstractRequest::ItemFetchRequest: 
            {
            FetchItemsL();
            }
            break;
        case QOrganizerItemAbstractRequest::ItemLocalIdFetchRequest: 
            {
            
            }
            break;
        case QOrganizerItemAbstractRequest::ItemRemoveRequest: 
            {
            RemoveItemL();
            }
            break;
        case QOrganizerItemAbstractRequest::ItemSaveRequest: 
            {
            SaveItemL();
            }
            break;
        case QOrganizerItemAbstractRequest::DetailDefinitionFetchRequest: 
            {
            
            }
            break;
        case QOrganizerItemAbstractRequest::DetailDefinitionRemoveRequest: 
            {
            
            }
            break;
        case QOrganizerItemAbstractRequest::DetailDefinitionSaveRequest: 
            {
            
            }
            break;
        default:
            {
            // Not implemented yet
            }
        }
    }

// Delete item
void COrganizerItemRequestsServiceProvider::RemoveItemL()
    {
    TInt sucessCount(0);

    if (iIndex < iNoOfItems)
        {
        // update index beforehand in case deleteItemL leaves, if so
        // RunError would call SelfComplete() for recursive operation
        iIndex++;
        // Delete an item
        iOrganizerItemManagerEngine.deleteItemL(iItemIds.at(iIndex-1), sucessCount);
        // Calls itself recursively until all the items are deleted
        SelfComplete();

        if (!sucessCount) // Entry not deleted successfully
            {
            // Entry is not deleted, but the error is not known.
            // So set the error to General Error and transform this error 
            // to corresponding Qt error
            iOrganizerItemManagerEngine.transformError(KErrGeneral, &iError);
            // Insert error map
            iErrorMap.insert(iIndex, iError);
            }
        }
    else
        {
        // Notify results
        QOrganizerItemManagerEngine::updateItemRemoveRequest(
                (QOrganizerItemRemoveRequest*)(iReq), 
                iError, iErrorMap, 
                QOrganizerItemAbstractRequest::FinishedState);
        }
    }

// Save item
void COrganizerItemRequestsServiceProvider::SaveItemL()
    {
    if (iIndex < iNoOfItems)
        {
        // update index beforehand in case saveItemL leaves, if so
        // RunError would call SelfComplete() for recursive operation
        iIndex++;
        // Save item
        iOrganizerItemManagerEngine.saveItemL(&iItemList[iIndex-1]);
        // Calls itself recursively until all the items are deleted
        SelfComplete();
        }
    else
        {
        // Notify results
        QOrganizerItemManagerEngine::updateItemSaveRequest(
                (QOrganizerItemSaveRequest*)(iReq), 
                iItemList, iError, iErrorMap, 
                QOrganizerItemAbstractRequest::FinishedState);
        }    
    }

void COrganizerItemRequestsServiceProvider::FetchItemsL()
    {
    QOrganizerItemFilter filter = ((QOrganizerItemFetchRequest*)iReq)->filter();
    QList<QOrganizerItemSortOrder> sortOrder = ((QOrganizerItemFetchRequest*)iReq)->sorting();
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
            // Change state to finish and update the results whatsoever available
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
    // Update the request status
    QOrganizerItemManagerEngine::updateRequestState(iReq, 
            QOrganizerItemAbstractRequest::CanceledState);
    // Clean up
    }

// Call if RunL leaves
TInt COrganizerItemRequestsServiceProvider::RunError(TInt /*aError*/)
    {
    // Operation not successfull
    // Generate error map
    iOrganizerItemManagerEngine.transformError(KErrGeneral, &iError);
    // Insert error map
    iErrorMap.insert(iIndex, iError);
    // Self complete the request to process more items one by one
    SelfComplete();
    // Handle the error here
    return KErrNone;
    }

void COrganizerItemRequestsServiceProvider::SelfComplete()
    {
    SetActive();
    TRequestStatus* pStat = &iStatus;
    User::RequestComplete(pStat, KErrNone);
    }
