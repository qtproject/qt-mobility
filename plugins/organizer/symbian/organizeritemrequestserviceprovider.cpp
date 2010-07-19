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
#include <calentryview.h>
#include <calentry.h>

// User includes
#include "organizeritemrequestserviceprovider.h"
#include "organizerItemTransform.h"
#include "organizerItemGuidTransform.h"
#include "organizerItemTypeTransform.h"
#include "qorganizersymbian_p.h"

// Static two phase construction
COrganizerItemRequestsServiceProvider* COrganizerItemRequestsServiceProvider::NewL(QOrganizerItemManagerEngine& aOrganizerItemManagerEngine)
    {
    COrganizerItemRequestsServiceProvider* self(new (ELeave) COrganizerItemRequestsServiceProvider(aOrganizerItemManagerEngine));
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Basic first phase constructor
COrganizerItemRequestsServiceProvider::COrganizerItemRequestsServiceProvider(QOrganizerItemManagerEngine& aOrganizerItemManagerEngine) : 
        CActive(EPriorityNormal), 
        iOrganizerItemManagerEngine(aOrganizerItemManagerEngine)
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
void COrganizerItemRequestsServiceProvider::StartRequest(QOrganizerItemAbstractRequest* aReq)
    {
    // change the state of the request and emit signal
    QOrganizerItemManagerEngine::updateRequestState(aReq, QOrganizerItemAbstractRequest::ActiveState);
    // Save asych request, start the timer and then return
    iReq = aReq;
    TRequestStatus* pStat = &iStatus;
    if (!IsActive())
        {
        SetActive();
        User::RequestComplete(pStat, KErrNone);
        }
    }
    
// Stop asynchronous request
void COrganizerItemRequestsServiceProvider::StopRequest()
    {
    // Stop asynchronous ongoing request
    Cancel();
    // Set the request status to InactiveState
    QOrganizerItemManagerEngine::updateRequestState(iReq, QOrganizerItemAbstractRequest::InactiveState);
    }

// Cancel an ongoing asynchronous request
void COrganizerItemRequestsServiceProvider::CancelRequest()
    {
    // Stop the request
    Cancel();
    // Change the state of the request and smit signal
    QOrganizerItemManagerEngine::updateRequestState(iReq, QOrganizerItemAbstractRequest::CanceledState);
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
            ((QOrganizerItemSymbianEngine&)(iOrganizerItemManagerEngine)).entryView()->FetchL(*globalUid, calEntryArray);
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
    ((QOrganizerItemSymbianEngine&)(iOrganizerItemManagerEngine)).entryView()->StoreL(entryPointerArray, entryCount);
    
    QOrganizerItemManager::Error error;
    error = QOrganizerItemManager::NoError;
    QMap<int, QOrganizerItemManager::Error> errorMap;
    if (entryCount != noOfEntriesToBeSaved) 
        {
        // Update the Error Map
        }
    
    // Transform details that are available/updated after saving 
    for (TInt index(0); index < entryCount; index++)
        {
        // QOrganizerItem item;
        // iOrganizerItemTransform->toItemPostSaveL(entryPointerArray[index], item);
        }
    
    CleanupStack::PopAndDestroy(&entryPointerArray);
 
    QOrganizerItemManagerEngine::updateItemSaveRequest((QOrganizerItemSaveRequest*)(iReq), itemList, error, errorMap, QOrganizerItemAbstractRequest::FinishedState);
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

    // generate error maps
    
    // Handle the error here
    return KErrNone;
    }

