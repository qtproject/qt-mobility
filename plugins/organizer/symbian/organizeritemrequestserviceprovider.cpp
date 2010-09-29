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

#ifdef _DEBUG
// To enable asserts in debug builds
#undef QT_NO_DEBUG
#endif

// User includes
#include "organizeritemrequestserviceprovider.h"
#include "qorganizersymbian_p.h"
#include "qorganizeritemrequestqueue.h"


// Static two phase construction
COrganizerItemRequestsServiceProvider* 
COrganizerItemRequestsServiceProvider::NewL(
        QOrganizerItemSymbianEngine& aOrganizerItemManagerEngine)
    {
    COrganizerItemRequestsServiceProvider* self(new (ELeave) 
            COrganizerItemRequestsServiceProvider(aOrganizerItemManagerEngine));
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Basic first phase constructor
COrganizerItemRequestsServiceProvider::COrganizerItemRequestsServiceProvider(
        QOrganizerItemSymbianEngine& aOrganizerItemManagerEngine) : 
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
TBool COrganizerItemRequestsServiceProvider::StartRequest(
        QOrganizerItemAbstractRequest* aReq)
    {
    if (!IsActive())
        {
        // Store the request to be processed
        iReq = aReq;
        // Initialize the member variables for the new requests
        Cleanup();
        QOrganizerItemAbstractRequest::RequestType requestType(iReq->type());
        switch (requestType)
            {
            case QOrganizerItemAbstractRequest::ItemFetchRequest:
                {
                    QOrganizerItemFetchRequest *req = static_cast<QOrganizerItemFetchRequest *>(iReq);
                    if (QOrganizerItemFilter::LocalIdFilter == req->filter().type()) {
                        // Get the local id filter
                        QOrganizerItemLocalIdFilter localIdFilter(req->filter());
                        // Get the local ids for the entries to be fetched
                        iItemIds.append(localIdFilter.ids());
                        iNoOfItems = iItemIds.count();
                    }
                }
                break;
            case QOrganizerItemAbstractRequest::ItemRemoveRequest:
                {
                    QOrganizerItemRemoveRequest *req = static_cast<QOrganizerItemRemoveRequest *>(iReq);
                    iItemIds.append(req->itemIds());
                    iNoOfItems = iItemIds.count();
                }
                break;                
            case QOrganizerItemAbstractRequest::ItemSaveRequest:
                {
                    QOrganizerItemSaveRequest * req = static_cast<QOrganizerItemSaveRequest *>(iReq);
                    iItemList.append(req->items());
                    iNoOfItems = iItemList.count();
                    iCollectionLocalId = req->collectionId();
                }
                break;
            case QOrganizerItemAbstractRequest::DetailDefinitionRemoveRequest:
                {
                    // Removing detail definitions not supported
                    QOrganizerItemManagerEngine::updateDefinitionRemoveRequest(
                        static_cast<QOrganizerItemDetailDefinitionRemoveRequest *>(iReq),
                        QOrganizerItemManager::NotSupportedError,
                        QMap<int, QOrganizerItemManager::Error>(),
                        QOrganizerItemAbstractRequest::FinishedState);
                }
                return false;
            case QOrganizerItemAbstractRequest::DetailDefinitionSaveRequest:
                {
                    // Saving detail definitions not supported
                    QOrganizerItemManagerEngine::updateDefinitionSaveRequest(
                        static_cast<QOrganizerItemDetailDefinitionSaveRequest *>(iReq),
                        QList<QOrganizerItemDetailDefinition>(),
                        QOrganizerItemManager::NotSupportedError,
                        QMap<int, QOrganizerItemManager::Error>(),
                        QOrganizerItemAbstractRequest::FinishedState);
                }
                return false;
    #ifndef SYMBIAN_CALENDAR_V2
                case QOrganizerItemAbstractRequest::CollectionRemoveRequest :
                {
                    // Removing a collection not supported
                    QOrganizerItemManagerEngine::updateCollectionRemoveRequest(
                        static_cast<QOrganizerCollectionRemoveRequest *>(iReq),
                        QOrganizerItemManager::NotSupportedError,
                        QMap<int, QOrganizerItemManager::Error>(),
                        QOrganizerItemAbstractRequest::FinishedState);
                }
                return false;                
            case QOrganizerItemAbstractRequest::CollectionSaveRequest :
                {
                    // Saving a collection not supported
                    QOrganizerItemManagerEngine::updateCollectionSaveRequest(
                        static_cast<QOrganizerCollectionSaveRequest *>(iReq),
                        QList<QOrganizerCollection>(),
                        QOrganizerItemManager::NotSupportedError,
                        QMap<int, QOrganizerItemManager::Error>(),
                        QOrganizerItemAbstractRequest::FinishedState);
                }
                return false;
    #else
            case QOrganizerItemAbstractRequest::CollectionRemoveRequest: 
                // Fallthrough
            case QOrganizerItemAbstractRequest::CollectionSaveRequest:
                // Fallthrough
    #endif
            case QOrganizerItemAbstractRequest::ItemInstanceFetchRequest:
                // Fallthrough
            case QOrganizerItemAbstractRequest::ItemLocalIdFetchRequest:
                // Fallthrough
            case QOrganizerItemAbstractRequest::DetailDefinitionFetchRequest:
                // Fallthrough
            case QOrganizerItemAbstractRequest::CollectionFetchRequest:
                // Fallthrough
            case QOrganizerItemAbstractRequest::CollectionLocalIdFetchRequest:
                // Do nothing, request is not handled iteratively, so no temporary data is needed
                break;
            default:
                // Unknown request
                return false;
           }
        
        // Process the request at RunL()
        SelfComplete(); 
        
        // Change the state of the request and emit signal
        QOrganizerItemManagerEngine::updateRequestState(aReq, 
                QOrganizerItemAbstractRequest::ActiveState);
        
        return ETrue;
        }
    else
        {
        // Another asynchronous request is already going on so this request can 
        // not be taken
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

TBool COrganizerItemRequestsServiceProvider::waitForRequestFinished(
        TTimeIntervalMicroSeconds32 /*aInterval*/)
    {
    // We do not support this feature
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
            FetchInstanceL();
            }
            break;
        case QOrganizerItemAbstractRequest::ItemFetchRequest: 
            {
            FetchItemsL();
            }
            break;
        case QOrganizerItemAbstractRequest::ItemLocalIdFetchRequest: 
            {
            FetchItemIdsL();
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
            FetchDetailDefinitionL();
            }
            break;
        case QOrganizerItemAbstractRequest::DetailDefinitionRemoveRequest: 
            {
            // Not supported. Already handled at StartRequest()
            }
            break;
        case QOrganizerItemAbstractRequest::DetailDefinitionSaveRequest: 
            {
            // Not supported. Already handled at StartRequest()
            }
            break;
        case QOrganizerItemAbstractRequest::CollectionFetchRequest : 
            {
            FetchCollections();
            }
            break;
        case QOrganizerItemAbstractRequest::CollectionLocalIdFetchRequest:
            {
            CollectionIds();
            }
            break;
        case QOrganizerItemAbstractRequest::CollectionRemoveRequest :
            {
            RemoveCollections();
            }
            break;
        case QOrganizerItemAbstractRequest::CollectionSaveRequest :
            {
            SaveCollections();
            }
            break;
        default:
            {
            // Unknown request. Already handled at StartRequest()
            }
            break;
        }
    }

//Fetch item instances
void COrganizerItemRequestsServiceProvider::FetchInstanceL()
    {
    QOrganizerItemInstanceFetchRequest *req = static_cast<QOrganizerItemInstanceFetchRequest *>(iReq);
    // Fetch ItemInstancesList
    QOrganizerItemManager::Error error(QOrganizerItemManager::NoError);
    iItemList = iOrganizerItemManagerEngine.itemInstances(
        req->filter(), req->sorting(), req->fetchHint(), &error);

    // Update the request status
    QOrganizerItemManagerEngine::updateItemInstanceFetchRequest(
        req, iItemList, error, QOrganizerItemAbstractRequest::FinishedState);
    }

// Delete item
void COrganizerItemRequestsServiceProvider::RemoveItemL()
    {
    if (iIndex < iNoOfItems)
        {
        // update index beforehand in case deleteItemL leaves, if so
        // RunError would call SelfComplete() for recursive operation
        iIndex++;
        // Delete an item
        QOrganizerItemLocalId itemLocalId(iItemIds.at(iIndex-1));
        iOrganizerItemManagerEngine.removeItemL(itemLocalId);
        iChangeSet.insertRemovedItem(itemLocalId);
        // Notify changeset
        iChangeSet.emitSignals(&iOrganizerItemManagerEngine);
        // Clear changeset so that it can be used for next item
        iChangeSet.clearAll();
        // Calls itself recursively until all the items are deleted
        SelfComplete();
        }
    else
        {
        // Notify results
        QOrganizerItemManagerEngine::updateItemRemoveRequest(
                static_cast<QOrganizerItemRemoveRequest *>(iReq), 
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
        QOrganizerItem item(iItemList[iIndex-1]);
        // Validate item before saving
        TBool isItemSupported(
            iOrganizerItemManagerEngine.validateItem(item, &iError));
        if (isItemSupported)
            {
            // Save item
            iOrganizerItemManagerEngine.saveItemL(&item, iCollectionLocalId, &iChangeSet);
            iSuccessfullItems.append(item);
            // Notify changeset
            iChangeSet.emitSignals(&iOrganizerItemManagerEngine);
            // Clear changeset so that it can be used for next item
            iChangeSet.clearAll();
            }
        else
            {
            iErrorMap.insert(iIndex-1, iError);
            }
        // Calls itself recursively until all the items are deleted
        SelfComplete();
        }
    else
        {
        // Notify results
        QOrganizerItemManagerEngine::updateItemSaveRequest(
                static_cast<QOrganizerItemSaveRequest *>(iReq), 
                iSuccessfullItems, iError, iErrorMap, 
                QOrganizerItemAbstractRequest::FinishedState);
        }    
    }

void COrganizerItemRequestsServiceProvider::FetchItemsL()
{
    QOrganizerItemFetchRequest *req = static_cast<QOrganizerItemFetchRequest *>(iReq);
    if (req->filter().type() == QOrganizerItemFilter::LocalIdFilter) {
        FetchItemsByLocalIdsL();
    }
    else {
        if (!iNoOfItems) {
            // Declare an invalid filter
            QOrganizerItemFilter filter;
            // Declare and empty sortOrder
            QList<QOrganizerItemSortOrder> sortOrder;
            // Fetch all the items Ids present in Data Base
            iItemIds.append(iOrganizerItemManagerEngine.itemIds(
                filter, sortOrder, &iError));
            iNoOfItems = iItemIds.count();
        }
        FetchItemsandFilterL();
    }
}

// Fetch item local ids
void COrganizerItemRequestsServiceProvider::FetchItemIdsL()
    {
    QOrganizerItemLocalIdFetchRequest *req = static_cast<QOrganizerItemLocalIdFetchRequest *>(iReq);
    iItemIds.append(iOrganizerItemManagerEngine.itemIds(
            req->filter(), req->sorting(), &iError));
    QOrganizerItemManagerEngine::updateItemLocalIdFetchRequest(
            req, iItemIds, iError, QOrganizerItemAbstractRequest::FinishedState);
    }

// Fetch Entries by local Ids
void COrganizerItemRequestsServiceProvider::FetchItemsByLocalIdsL()
    {
    QOrganizerItemFetchHint fetchHint;
    if (iIndex < iNoOfItems)
        {
        // update index beforehand in case itemL leaves, if so
        // RunError would call SelfComplete() for recursive operation
        iIndex++;
        // Fetch the item
        QOrganizerItem item =
            iOrganizerItemManagerEngine.itemL(
                iItemIds.at(iIndex-1), fetchHint);
        // Append the fetched item to iItemList
        iItemList << item;
        // Calls itself recursively until all the items are deleted
        SelfComplete();
        }
    else
        {
        // Notify results
        QOrganizerItemManagerEngine::updateItemFetchRequest(
                static_cast<QOrganizerItemFetchRequest *>(iReq), iItemList, iError, 
                QOrganizerItemAbstractRequest::FinishedState);
        }    
    }

// Fetch items/entries by details
void COrganizerItemRequestsServiceProvider::FetchItemsandFilterL()
    {
    QOrganizerItemFetchRequest *req = static_cast<QOrganizerItemFetchRequest *>(iReq);
    if (iIndex < iNoOfItems)
            {
            // update index beforehand in case itemL leaves, if so
            // RunError would call SelfComplete() for recursive operation
            iIndex++;
            // Fetch the item
            QOrganizerItem item = 
                iOrganizerItemManagerEngine.itemL(
                    iItemIds.at(iIndex-1), req->fetchHint());
            // Append the fetched item to iItemList
            iItemList << item;
            // Calls itself recursively until all the items are deleted
            SelfComplete();
            }
        else
            {
            // Filter & sort the items
            QList<QOrganizerItem> items(iOrganizerItemManagerEngine.slowFilter(
                iItemList, req->filter(), req->sorting()));
            // Notify results
            QOrganizerItemManagerEngine::updateItemFetchRequest(
                req, items, iError, QOrganizerItemAbstractRequest::FinishedState);
            }    
    }

// Fetch detail definition
void COrganizerItemRequestsServiceProvider::FetchDetailDefinitionL()
{
    QOrganizerItemDetailDefinitionFetchRequest *req = static_cast<QOrganizerItemDetailDefinitionFetchRequest *>(iReq);

    QMap<QString, QOrganizerItemDetailDefinition> detailDefinitionMap;
    QMap<int, QOrganizerItemManager::Error> errorMap;
    QOrganizerItemManager::Error error;
    int count = req->definitionNames().count();
    
    // As there are no costly IPCs involved in this operation so
    // execute a loop to perform the operation as it's done in a short
    // time span
    if (count == 0) {
        detailDefinitionMap = iOrganizerItemManagerEngine.detailDefinitions(req->itemType(), &error);
    } else {
        for (TInt index(0); index < count; index++) {
            // Fetch detail definition
            QString stringItem(req->definitionNames().at(index));
            QOrganizerItemDetailDefinition detailDefinition( 
                (iOrganizerItemManagerEngine.detailDefinition(
                    stringItem, req->itemType(), &error)));

            if (QOrganizerItemManager::NoError == error) {
                detailDefinitionMap.insert(stringItem, detailDefinition);
            } else {
                errorMap.insert(index, error);
            }
        }
    }
    
    // Notify results
    QOrganizerItemManagerEngine::updateDefinitionFetchRequest(
        req, detailDefinitionMap, error, errorMap, QOrganizerItemAbstractRequest::FinishedState);
}

void COrganizerItemRequestsServiceProvider::CollectionIds()
{
    Q_ASSERT(iReq->type() == QOrganizerItemAbstractRequest::CollectionLocalIdFetchRequest);
    QOrganizerCollectionLocalIdFetchRequest *fetchReq = static_cast<QOrganizerCollectionLocalIdFetchRequest *>(iReq);

    // Notify results
    QOrganizerItemManager::Error error(QOrganizerItemManager::NoError);
    QOrganizerItemManagerEngine::updateCollectionLocalIdFetchRequest(
        fetchReq,
        iOrganizerItemManagerEngine.collectionIds(&error),
        error,
        QOrganizerItemAbstractRequest::FinishedState);
}

void COrganizerItemRequestsServiceProvider::FetchCollections()
{
    Q_ASSERT(iReq->type() == QOrganizerItemAbstractRequest::CollectionFetchRequest);
    QOrganizerCollectionFetchRequest *fetchReq = static_cast<QOrganizerCollectionFetchRequest *>(iReq);

    QMap<int, QOrganizerItemManager::Error> errorMap;
    QOrganizerItemManager::Error error(QOrganizerItemManager::NoError);
    QList<QOrganizerCollection> result = iOrganizerItemManagerEngine.collections(fetchReq->collectionIds(), &errorMap, &error);
    QOrganizerItemManagerEngine::updateCollectionFetchRequest(
        fetchReq,
        result,
        error,
        errorMap,
        QOrganizerItemAbstractRequest::FinishedState);
}

void COrganizerItemRequestsServiceProvider::SaveCollections()
{
    Q_ASSERT(iReq->type() == QOrganizerItemAbstractRequest::CollectionSaveRequest);

    QOrganizerCollectionSaveRequest *saveReq = static_cast<QOrganizerCollectionSaveRequest *>(iReq);
    QList<QOrganizerCollection> collections = saveReq->collections();
    QOrganizerItemManager::Error error(QOrganizerItemManager::NoError);
    QMap<int, QOrganizerItemManager::Error> errorMap;

    // Save all collections
    for (int i(0); i < collections.count(); i++) {
        // The following also emits the necessary signals
        iOrganizerItemManagerEngine.saveCollection(&(collections[i]), &error);
        if (error != QOrganizerItemManager::NoError) {
            errorMap.insert(i, error);
        }
    }

    // Notify completion
    QOrganizerItemManagerEngine::updateCollectionSaveRequest(
        saveReq,
        collections,
        error,
        errorMap,
        QOrganizerItemAbstractRequest::FinishedState);
}

void COrganizerItemRequestsServiceProvider::RemoveCollections()
{
    Q_ASSERT(iReq->type() == QOrganizerItemAbstractRequest::CollectionRemoveRequest);

    QOrganizerCollectionRemoveRequest *removeReq = static_cast<QOrganizerCollectionRemoveRequest *>(iReq);
    QList<QOrganizerCollectionLocalId> collectionIds = removeReq->collectionIds();
    QOrganizerItemManager::Error error(QOrganizerItemManager::NoError);
    QMap<int, QOrganizerItemManager::Error> errorMap;

    // Remove all collections
    for (int i(0); i < collectionIds.count(); i++) {
        // The following also emits the necessary signals
        iOrganizerItemManagerEngine.removeCollection(collectionIds.at(i), &error);
        if (error != QOrganizerItemManager::NoError) {
            errorMap.insert(i, error);
        }
    }

    // Notify completion
    QOrganizerItemManagerEngine::updateCollectionRemoveRequest(
        removeReq,
        error,
        errorMap,
        QOrganizerItemAbstractRequest::FinishedState);
}

// Called by Cancel()
void COrganizerItemRequestsServiceProvider::DoCancel()
    {
    // Update the request status
    QOrganizerItemManagerEngine::updateRequestState(iReq, 
            QOrganizerItemAbstractRequest::CanceledState);
    }

// Call if RunL leaves
TInt COrganizerItemRequestsServiceProvider::RunError(TInt aError)
    {
    // Operation not successfull
    // Generate error map
    iOrganizerItemManagerEngine.transformError(aError, &iError);
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

// Initialize member variables
void COrganizerItemRequestsServiceProvider::Cleanup()
    {
    iIndex = 0;
    iErrorMap.clear();
    iItemIds.clear();
    iNoOfItems = 0;
    iItemList.clear();
    iError = QOrganizerItemManager::NoError;
    iSuccessfullItems.clear();
    iChangeSet.clearAll();
    }
