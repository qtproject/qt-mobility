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
#include "qorganizersymbian_p.h"


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
        // Change the state of the request and emit signal
        QOrganizerItemManagerEngine::updateRequestState(aReq, 
                QOrganizerItemAbstractRequest::ActiveState);
        // Store the request to be processed
        iReq = aReq;
        // Initialize the member variables for the new requests
        Cleanup();
        QOrganizerItemAbstractRequest::RequestType requestType(iReq->type());
        switch (requestType)
            {
            case QOrganizerItemAbstractRequest::ItemRemoveRequest:
                {
                iItemIds.append(
                        ((QOrganizerItemRemoveRequest*)(iReq))->itemIds());
                iNoOfItems = iItemIds.count();
                }
                break;
            case QOrganizerItemAbstractRequest::ItemSaveRequest:
                {
                iItemList.append(((QOrganizerItemSaveRequest*)(iReq))->items());
                iNoOfItems = iItemList.count();
                }
                break;
            case QOrganizerItemAbstractRequest::ItemFetchRequest:
                {
                QOrganizerItemFilter filter = 
                        ((QOrganizerItemFetchRequest*)iReq)->filter();
                if (QOrganizerItemFilter::LocalIdFilter == filter.type())
                    {
                    // Get the local id filter
                    QOrganizerItemLocalIdFilter localIdFilter(filter);
                    // Get the local ids for the entries to be fetched
                    iItemIds.append(localIdFilter.ids());
                    iNoOfItems = iItemIds.count();
                    }
                
                }
                break;
#ifdef SYMBIAN_CALENDAR_V2
            case QOrganizerItemAbstractRequest::CollectionSaveRequest :
                {
                iNoOfItems = ((QOrganizerCollectionSaveRequest*)(iReq))
                    ->collections().count();
                }
                break;
            case QOrganizerItemAbstractRequest::CollectionRemoveRequest :
                {
                iNoOfItems = ((QOrganizerCollectionRemoveRequest*) iReq)
                    ->collectionIds().count();
                }
                break;
            case QOrganizerItemAbstractRequest::CollectionFetchRequest :
                {
                QList<QOrganizerCollectionId> collectionIds(
                    ((QOrganizerCollectionFetchRequest*) iReq)
                    ->collectionIds());
                TInt count(collectionIds.count());
                for (TInt index(0); index < count; index++)
                    {
                    iCollectionLocalIds.append(
                        collectionIds.at(index).localId());
                    }
                iNoOfItems = iOrganizerItemManagerEngine.sessionCount();
                }
                break;
#endif
           }

        SelfComplete();
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
            RemoveDetailDefinitionL();
            }
            break;
        case QOrganizerItemAbstractRequest::DetailDefinitionSaveRequest: 
            {
            SaveDetailDefinitionL();
            }
            break;
#ifdef SYMBIAN_CALENDAR_V2
        case QOrganizerItemAbstractRequest::CollectionFetchRequest : 
            {
            CollectionL();
            }
            break;
        case QOrganizerItemAbstractRequest::CollectionLocalIdFetchRequest:
            {
            CollectionIdL();
            }
            break;
        case QOrganizerItemAbstractRequest::CollectionRemoveRequest :
            {
            RemoveCollectionL();    
            }
            break;
        case QOrganizerItemAbstractRequest::CollectionSaveRequest :
            {
            SaveCollectionL();
            }
            break;
#endif
        default:
            {
            // Not implemented yet
            }
        }
    }

//Fetch item instances
void COrganizerItemRequestsServiceProvider::FetchInstanceL()
    {
    // Fetch ItemInstancesList
    iItemList = iOrganizerItemManagerEngine.itemInstances(
        ((QOrganizerItemFetchRequest*)iReq)->filter(), 
        ((QOrganizerItemFetchRequest*)iReq)->sorting(), 
        ((QOrganizerItemFetchRequest*)iReq)->fetchHint(),
        &iError);
    // Update the request status
    QOrganizerItemManagerEngine::updateItemInstanceFetchRequest(
        (QOrganizerItemInstanceFetchRequest*)(iReq), iItemList, 
        iError, QOrganizerItemAbstractRequest::FinishedState);
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
        // Calls itself recursively until all the items are deleted
        SelfComplete();
        }
    else
        {
        // Notify changeset
        iChangeSet.emitSignals(&iOrganizerItemManagerEngine);
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
        QOrganizerItem item(iItemList[iIndex-1]);
        // Validate item before saving
        TBool isItemSupported(
            iOrganizerItemManagerEngine.validateItem(item, &iError));
        if (isItemSupported)
            {
            // Save item
            // TODO: collection id needed!
            // without changeSet signaling does not work, and without collection id
            // the item is always stored to the default collection
            iOrganizerItemManagerEngine.saveItemL(&item, 0, &iChangeSet);
            iSuccessfullItems.append(item);
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
        // Notify changeset
        iChangeSet.emitSignals(&iOrganizerItemManagerEngine);
        // Notify results
        QOrganizerItemManagerEngine::updateItemSaveRequest(
                (QOrganizerItemSaveRequest*)(iReq), 
                iSuccessfullItems, iError, iErrorMap, 
                QOrganizerItemAbstractRequest::FinishedState);
        }    
    }

void COrganizerItemRequestsServiceProvider::FetchItemsL()
    {
    QOrganizerItemFilter filter = ((QOrganizerItemFetchRequest*)iReq)->filter();
    
    // Get the filter type
    QOrganizerItemFilter::FilterType filterType = filter.type();

    switch (filterType)
        {
        case QOrganizerItemFilter::LocalIdFilter :
            {
            FetchItemsByLocalIdsL();
            }
            break;
         default :
            {
            if (!iNoOfItems)
                {
                // Declare an invalid filter
                QOrganizerItemFilter filter;
                // Declare and empty sortOrder
                QList<QOrganizerItemSortOrder> sortOrder;
                // Fetch all the items Ids present in Data Base
                iItemIds.append(iOrganizerItemManagerEngine.itemIds(
                    filter, sortOrder, &iError));
                iNoOfItems = iItemIds.count();
                }
            QList<QOrganizerItemSortOrder> sortOrder = 
                    ((QOrganizerItemFetchRequest*)iReq)->sorting();
            // Fetch hint is not supported as of now
            QOrganizerItemFetchHint fetchHint = 
                    ((QOrganizerItemFetchRequest*)iReq)->fetchHint();

            FetchItemsandFilterL(filter, sortOrder, fetchHint);
            }
            break;
        }
    }

// Fetch item local ids
void COrganizerItemRequestsServiceProvider::FetchItemIdsL()
    {
    QOrganizerItemFilter filter(((QOrganizerItemFetchRequest*)iReq)->filter());
    QList<QOrganizerItemSortOrder> sortOrder(
            ((QOrganizerItemFetchRequest*)iReq)->sorting());
    
    iItemIds.append(iOrganizerItemManagerEngine.itemIds(
            filter, sortOrder, &iError));
    
    QOrganizerItemManagerEngine::updateItemLocalIdFetchRequest(
            (QOrganizerItemLocalIdFetchRequest*)iReq, 
            iItemIds, iError, QOrganizerItemAbstractRequest::FinishedState);
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
        QOrganizerItem item;
        iOrganizerItemManagerEngine.itemL(
                iItemIds.at(iIndex-1), &item, fetchHint);
        // Append the fetched item to iItemList
        iItemList << item;
        // Calls itself recursively until all the items are deleted
        SelfComplete();
        }
    else
        {
        // Notify results
        QOrganizerItemManagerEngine::updateItemFetchRequest(
                (QOrganizerItemFetchRequest*)(iReq), iItemList, iError, 
                QOrganizerItemAbstractRequest::FinishedState);
        }    
    }

// Fetch items/entries by details
void COrganizerItemRequestsServiceProvider::FetchItemsandFilterL(
        QOrganizerItemFilter& filter, QList<QOrganizerItemSortOrder>& sortOrder,
        QOrganizerItemFetchHint& fetchHint)
    {
    if (iIndex < iNoOfItems)
            {
            // update index beforehand in case itemL leaves, if so
            // RunError would call SelfComplete() for recursive operation
            iIndex++;
            // Fetch the item
            QOrganizerItem item;
            iOrganizerItemManagerEngine.itemL(
                    iItemIds.at(iIndex-1), &item, fetchHint);
            // Append the fetched item to iItemList
            iItemList << item;
            // Calls itself recursively until all the items are deleted
            SelfComplete();
            }
        else
            {
            // Filter & sort the items
            QList<QOrganizerItem> items(iOrganizerItemManagerEngine.slowFilter(
                iItemList, filter, sortOrder));
            // Notify results
            QOrganizerItemManagerEngine::updateItemFetchRequest(
                    (QOrganizerItemFetchRequest*)(iReq), items, iError, 
                    QOrganizerItemAbstractRequest::FinishedState);
            }    
    }

// Fetch detail definition
void COrganizerItemRequestsServiceProvider::FetchDetailDefinitionL()
    {
    QStringList stringList(((QOrganizerItemDetailDefinitionFetchRequest*)
            (iReq))->definitionNames());
    TInt count(stringList.count());

    QString itemType(((QOrganizerItemDetailDefinitionFetchRequest*)
            (iReq))->itemType());

    QMap<QString, QOrganizerItemDetailDefinition> detailDefinitionMap;

    // As there are no costly IPCs involved in this operation so
    // execute a loop to perform the operation as it's done in a short
    // time span
    for (TInt index(0); index < count; index++)
        {
        // Fetch detail definition
        QString stringItem(stringList.at(index));
        QOrganizerItemDetailDefinition detailDefinition( 
                (iOrganizerItemManagerEngine.detailDefinition(
                    stringItem, itemType, &iError)));
    
        if (QOrganizerItemManager::NoError == iError)
            {
            detailDefinitionMap.insert(stringItem, detailDefinition);
            }
        else
            {
            iErrorMap.insert(index, iError);
            }
        }
    // Notify results
    QOrganizerItemManagerEngine::updateDefinitionFetchRequest(
            (QOrganizerItemDetailDefinitionFetchRequest*)(iReq), 
            detailDefinitionMap, iError, iErrorMap, 
            QOrganizerItemAbstractRequest::FinishedState);
    }

// Remove detail definition
void COrganizerItemRequestsServiceProvider::RemoveDetailDefinitionL()
    {
    QStringList stringList(((QOrganizerItemDetailDefinitionRemoveRequest*)
            (iReq))->definitionNames());
    TInt count(stringList.count());

    QString itemType(((QOrganizerItemDetailDefinitionRemoveRequest*)
            (iReq))->itemType());

    // As there are no costly IPC is involved in this operation so
    // execute a loop to perform the operation as it's done in a short
    // time span
    for (TInt index(0); index < count; index++)
        {
		// Remove detail definition
        iOrganizerItemManagerEngine.removeDetailDefinition(
                stringList[index], itemType, &iError);
    
        if (iError != QOrganizerItemManager::NoError)
            {
            iErrorMap.insert(index, iError);
            }
        }
    // Notify results
    QOrganizerItemManagerEngine::updateDefinitionRemoveRequest(
            (QOrganizerItemDetailDefinitionRemoveRequest*)(iReq), 
            iError, iErrorMap, QOrganizerItemAbstractRequest::FinishedState);
    }

// Save detail definition
void COrganizerItemRequestsServiceProvider::SaveDetailDefinitionL()
    {
    QList<QOrganizerItemDetailDefinition> detailDefinitions(
            ((QOrganizerItemDetailDefinitionSaveRequest*)
                    (iReq))->definitions());
    TInt count(detailDefinitions.count());

    QString itemType(((QOrganizerItemDetailDefinitionSaveRequest*)
            (iReq))->itemType());

    // As there are no costly IPC is involved in this operation so
    // execute a loop to perform the operation as it's done in a short
    // time span
    for (TInt index(0); index < count; index++)
        {
		// Save detail definition
        iOrganizerItemManagerEngine.saveDetailDefinition(
                detailDefinitions[iIndex], itemType, &iError);
    
        if (iError != QOrganizerItemManager::NoError)
            {
            iErrorMap.insert(index, iError);
            }
        }
    // Clear all the definition as save definitions is not supported by
    // Symbian backend
    detailDefinitions.clear();
    // Notify results
    QOrganizerItemManagerEngine::updateDefinitionSaveRequest(
            (QOrganizerItemDetailDefinitionSaveRequest*)(iReq), 
            detailDefinitions, iError, iErrorMap, 
            QOrganizerItemAbstractRequest::FinishedState);
    }

#ifdef SYMBIAN_CALENDAR_V2
// Fetch collection local Id
void COrganizerItemRequestsServiceProvider::CollectionIdL()
    {
    QList<QOrganizerCollectionLocalId> collectionLocalIds = 
        iOrganizerItemManagerEngine.collectionIdsL();

    // Notify results
    QOrganizerItemManagerEngine::updateCollectionLocalIdFetchRequest( 
        (QOrganizerCollectionLocalIdFetchRequest*)(iReq), collectionLocalIds, 
        iError, QOrganizerItemAbstractRequest::FinishedState);
    }

// Fetch collection
void COrganizerItemRequestsServiceProvider::CollectionL()
    {
    if (iIndex < iNoOfItems)
        {
        // update index beforehand in case collectionL leaves, if so
        // RunError would call SelfComplete() for recursive operation
        iIndex++;
        QOrganizerCollection collection;
        TBool found(iOrganizerItemManagerEngine.collectionL(iIndex-1, 
            iCollectionLocalIds, collection));
        if (found) // Check if collection with 
            // iCollectionLocalIds[iIndex-1].LocalId() is present
            {
            // Append the fetched collection to iSuccessfullCollections
            iSuccessfullCollections.append(collection);
            }
        // Calls itself recursively until all the items are deleted
        SelfComplete();
        }
    else
        {
        // Notify results
        QOrganizerItemManagerEngine::updateCollectionFetchRequest(
            (QOrganizerCollectionFetchRequest*)(iReq), iSuccessfullCollections, 
            iError, QOrganizerItemAbstractRequest::FinishedState);
        }    
    }

// Save collection
void COrganizerItemRequestsServiceProvider::SaveCollectionL()
    {
    if (iIndex < iNoOfItems)
        {
        // update index beforehand in case saveCollectionL leaves, if so
        // RunError would call SelfComplete() for recursive operation
        iIndex++;
        QOrganizerCollection collection(((QOrganizerCollectionSaveRequest*)
            (iReq))->collections().at(iIndex-1));
        iOrganizerItemManagerEngine.saveCollectionL(&collection);
        // Append the successfully saved collection iSuccessfullCollections
        iSuccessfullCollections.append(collection);
        // Calls itself recursively until all the items are deleted
        SelfComplete();
        }
    else
        {
        // Notify results
        QOrganizerItemManagerEngine::updateCollectionSaveRequest(
            (QOrganizerCollectionSaveRequest*)(iReq), 
            iSuccessfullCollections, iError, iErrorMap, 
            QOrganizerItemAbstractRequest::FinishedState);
        }    
    }

// Remove collection
void COrganizerItemRequestsServiceProvider::RemoveCollectionL()
    {
    if (iIndex < iNoOfItems)
        {
        // update index beforehand in case removeCollectionL leaves, if so
        // RunError would call SelfComplete() for recursive operation
        iIndex++;
        QOrganizerCollectionId collectionId(
            ((QOrganizerCollectionRemoveRequest*) iReq)
            ->collectionIds().at(iIndex-1));

        iOrganizerItemManagerEngine.removeCollectionL(collectionId.localId());
        // Calls itself recursively until all the items are deleted
        SelfComplete();
        }
    else
        {
        // Notify results
        QOrganizerItemManagerEngine::updateCollectionRemoveRequest(
            (QOrganizerCollectionRemoveRequest*)(iReq), iError, 
            iErrorMap, QOrganizerItemAbstractRequest::FinishedState);
        }    
    }
#endif
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
    iItemList.clear();
    iError = QOrganizerItemManager::NoError;
    iSuccessfullItems.clear();
#ifdef SYMBIAN_CALENDAR_V2
    iSuccessfullCollections.clear();
    iCollectionLocalIds.clear();
#endif
    iChangeSet.clearAll();
    }
