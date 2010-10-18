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
    // No second phase constructor
    return self;
}

// Basic first phase constructor
COrganizerItemRequestsServiceProvider::COrganizerItemRequestsServiceProvider(
        QOrganizerItemSymbianEngine& aOrganizerItemManagerEngine) : 
        CActive(EPriorityNormal), 
        iOrganizerItemManagerEngine(aOrganizerItemManagerEngine),
        iReq(NULL),
        iIndex(0)
{
    CActiveScheduler::Add(this);
}

// Destructor/Cleanup
COrganizerItemRequestsServiceProvider::~COrganizerItemRequestsServiceProvider()
{
    // Cancel ongoing request, if any
    Cancel();
}

// Start processing aReq request to be processed asynchronously
bool COrganizerItemRequestsServiceProvider::StartRequest(
        QOrganizerItemAbstractRequest* aReq)
{
    if (!IsActive()) {
        // Store the request to be processed
        iReq = aReq;
        // Initialize the member variables for the new requests
        Reset();

        switch (iReq->type()) {
        case QOrganizerItemAbstractRequest::DetailDefinitionRemoveRequest:
            {
                // Special case, removing detail definitions not supported
                QOrganizerItemManagerEngine::updateDefinitionRemoveRequest(
                    static_cast<QOrganizerItemDetailDefinitionRemoveRequest *>(iReq),
                    QOrganizerItemManager::NotSupportedError,
                    QMap<int, QOrganizerItemManager::Error>(),
                    QOrganizerItemAbstractRequest::FinishedState);
            }
            return false;
        case QOrganizerItemAbstractRequest::DetailDefinitionSaveRequest:
            {
                // Special case, saving detail definitions not supported
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
                // Special case, removing a collection not supported
                QOrganizerItemManagerEngine::updateCollectionRemoveRequest(
                    static_cast<QOrganizerCollectionRemoveRequest *>(iReq),
                    QOrganizerItemManager::NotSupportedError,
                    QMap<int, QOrganizerItemManager::Error>(),
                    QOrganizerItemAbstractRequest::FinishedState);
            }
            return false;                
        case QOrganizerItemAbstractRequest::CollectionSaveRequest :
            {
                // Special case, saving a collection not supported
                QOrganizerItemManagerEngine::updateCollectionSaveRequest(
                    static_cast<QOrganizerCollectionSaveRequest *>(iReq),
                    QList<QOrganizerCollection>(),
                    QOrganizerItemManager::NotSupportedError,
                    QMap<int, QOrganizerItemManager::Error>(),
                    QOrganizerItemAbstractRequest::FinishedState);
            }
            return false;
#else
        case QOrganizerItemAbstractRequest::CollectionRemoveRequest:       // Fallthrough
        case QOrganizerItemAbstractRequest::CollectionSaveRequest:         // .
#endif
        case QOrganizerItemAbstractRequest::ItemFetchRequest:              // .
        case QOrganizerItemAbstractRequest::ItemRemoveRequest:             // .
        case QOrganizerItemAbstractRequest::ItemSaveRequest:               // .
        case QOrganizerItemAbstractRequest::ItemInstanceFetchRequest:      // .
        case QOrganizerItemAbstractRequest::ItemLocalIdFetchRequest:       // .
        case QOrganizerItemAbstractRequest::DetailDefinitionFetchRequest:  // .
        case QOrganizerItemAbstractRequest::CollectionFetchRequest:        // .
        case QOrganizerItemAbstractRequest::CollectionLocalIdFetchRequest:
            {
                // QWeakPointer is aware if the request object (which is derived from QObject) is deleted.
                QWeakPointer<QOrganizerItemAbstractRequest> req = aReq;

                // Change the state of the request and emit signal
                QOrganizerItemManagerEngine::updateRequestState(aReq,
                        QOrganizerItemAbstractRequest::ActiveState);

                // Client may delete the request object when state is updated. And because by default
                // signals are synchronous we might not have a valid request anymore.
                if (!req.isNull())
                    SelfComplete(); // Process the request at RunL()

                return true;
            }
        default:
            // Unknown request
            return false;
        }
    } else {
        // Another asynchronous request is already going on so this request can 
        // not be taken
        return false;
    }
}

// Cancel an ongoing asynchronous request
bool COrganizerItemRequestsServiceProvider::CancelRequest()
{
    // Stop the request, Cancel() would call doCancel(), which updates the 
    // request status
    Cancel();
    return ETrue;
}

void COrganizerItemRequestsServiceProvider::RunL()
{
    switch (iReq->type()) {
    case QOrganizerItemAbstractRequest::ItemInstanceFetchRequest: 
        {
        FetchInstance();
        }
        break;
    case QOrganizerItemAbstractRequest::ItemFetchRequest: 
        {
        FetchItems();
        }
        break;
    case QOrganizerItemAbstractRequest::ItemLocalIdFetchRequest: 
        {
        FetchItemIds();
        }
        break;
    case QOrganizerItemAbstractRequest::ItemRemoveRequest: 
        {
        RemoveItem();
        }
        break;
    case QOrganizerItemAbstractRequest::ItemSaveRequest: 
        {
        SaveItem();
        }
        break;
    case QOrganizerItemAbstractRequest::DetailDefinitionFetchRequest: 
        {
        FetchDetailDefinition();
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
void COrganizerItemRequestsServiceProvider::FetchInstance()
{
    QOrganizerItemInstanceFetchRequest *req = static_cast<QOrganizerItemInstanceFetchRequest *>(iReq);
    // Fetch ItemInstancesList
    QOrganizerItemManager::Error error(QOrganizerItemManager::NoError);
    QList<QOrganizerItem> itemList = iOrganizerItemManagerEngine.itemInstances(
        req->filter(), req->sorting(), req->fetchHint(), &error);

    // Update the request status
    QOrganizerItemManagerEngine::updateItemInstanceFetchRequest(
        req, itemList, error, QOrganizerItemAbstractRequest::FinishedState);
}

// Delete item
void COrganizerItemRequestsServiceProvider::RemoveItem()
{
    Q_ASSERT(iReq->type() == QOrganizerItemAbstractRequest::ItemRemoveRequest);
    QOrganizerItemRemoveRequest *req = static_cast<QOrganizerItemRemoveRequest *>(iReq);
    QList<QOrganizerItemLocalId> itemIds = req->itemIds();
    Q_ASSERT(iIndex < itemIds.count());

    QOrganizerItemManager::Error error(QOrganizerItemManager::NoError);
    // Delete an item (emits "itemsRemoved")
    iOrganizerItemManagerEngine.removeItem(itemIds.at(iIndex), &error);
    if (error != QOrganizerItemManager::NoError)
        iErrorMap.insert(iIndex, error);

    iIndex++;
    if (iIndex < itemIds.count()) {
        // Continue until all itemIds handled
        SelfComplete();
    } else {
        // In case there were any errors, report the first available error code
        if (iErrorMap.count())
            error = iErrorMap.values().at(0);

        // Emit state change
        QOrganizerItemManagerEngine::updateItemRemoveRequest(
            req, error, iErrorMap, QOrganizerItemAbstractRequest::FinishedState);
    }
}

// Save item
void COrganizerItemRequestsServiceProvider::SaveItem()
{
    Q_ASSERT(iReq->type() == QOrganizerItemAbstractRequest::ItemSaveRequest);
    QOrganizerItemSaveRequest *req = static_cast<QOrganizerItemSaveRequest *>(iReq);
    QList<QOrganizerItem> items = req->items();
    Q_ASSERT(iIndex < items.count());

    QOrganizerItemManager::Error error(QOrganizerItemManager::NoError);
    // Validate item before saving
    if (iOrganizerItemManagerEngine.validateItem(items[iIndex], &error))
        {
        // Save item (emits "itemsAdded")
        iOrganizerItemManagerEngine.saveItem(&(items[iIndex]), req->collectionId(), &error);
        }

    if (error == QOrganizerItemManager::NoError) {
        // Update item in the list (does not emit resultsAvailable)
        req->setItems(items);
    } else {
        iErrorMap.insert(iIndex, error);
    }

    // Continue until all items handled
    iIndex++;
    if (iIndex < req->items().count()) {
        SelfComplete();
    } else {
        // In case there were any errors, report the first available error code
        if (iErrorMap.count())
            error = iErrorMap.values().at(0);

        // Remove erroneous items from the result
        QList<QOrganizerItem> result = req->items();
        foreach(int itemError, iErrorMap.keys()) {
            result.removeAt(itemError);
        }

        // Notify results (emits resultsAvailable and stateChanged)
        QOrganizerItemManagerEngine::updateItemSaveRequest(
            req, result, error, iErrorMap, QOrganizerItemAbstractRequest::FinishedState);
    }    
}

void COrganizerItemRequestsServiceProvider::FetchItems()
{
    QOrganizerItemFetchRequest *req = static_cast<QOrganizerItemFetchRequest *>(iReq);
    if (req->filter().type() == QOrganizerItemFilter::LocalIdFilter) {
        FetchItemsByLocalIds();
    } else {
        if (!iItemIds.count()) {
            // Fetch all item ids
            QOrganizerItemFilter filter;
            QList<QOrganizerItemSortOrder> sortOrder;
            QOrganizerItemManager::Error error(QOrganizerItemManager::NoError);
            iItemIds = iOrganizerItemManagerEngine.itemIds(filter, sortOrder, &error);
            if (error != QOrganizerItemManager::NoError || !iItemIds.count()) {
                // Complete with empty list
                QOrganizerItemManagerEngine::updateItemFetchRequest(
                    req, QList<QOrganizerItem>(), error, QOrganizerItemAbstractRequest::FinishedState);
            } else {
                FetchItemsandFilter();
            }
        } else {
            FetchItemsandFilter();
        }
    }
}

// Fetch item local ids
void COrganizerItemRequestsServiceProvider::FetchItemIds()
{
    Q_ASSERT(iReq->type() == QOrganizerItemAbstractRequest::ItemLocalIdFetchRequest);
    QOrganizerItemLocalIdFetchRequest *req = static_cast<QOrganizerItemLocalIdFetchRequest *>(iReq);
    QOrganizerItemManager::Error error(QOrganizerItemManager::NoError);
    QList<QOrganizerItemLocalId> itemIds = iOrganizerItemManagerEngine.itemIds(req->filter(), req->sorting(), &error);
    QOrganizerItemManagerEngine::updateItemLocalIdFetchRequest(
        req, itemIds, error, QOrganizerItemAbstractRequest::FinishedState);
}

// Fetch Entries by local Ids
void COrganizerItemRequestsServiceProvider::FetchItemsByLocalIds()
{
    Q_ASSERT(iReq->type() == QOrganizerItemAbstractRequest::ItemFetchRequest);
    QOrganizerItemFetchRequest *req = static_cast<QOrganizerItemFetchRequest *>(iReq);
    Q_ASSERT(req->filter().type() == QOrganizerItemFilter::LocalIdFilter);
    QOrganizerItemLocalIdFilter localIdFilter = req->filter();
    QList<QOrganizerItemLocalId> itemIds = localIdFilter.ids();
    Q_ASSERT(iIndex < itemIds.count());

    QOrganizerItemFetchHint fetchHint;
    QOrganizerItemManager::Error error(QOrganizerItemManager::NoError);
    // Fetch the item
    QOrganizerItem item =
        iOrganizerItemManagerEngine.item(localIdFilter.ids().at(iIndex), fetchHint, &error);

    // Append the fetched item to the result
    // TODO: we may need to batch signaling to improve performance,
    // for example "resultsAvailable" for every tenth iteration
    QList<QOrganizerItem> items = req->items();
    if (error == QOrganizerItemManager::NoError) {
        items << item;
    }

    // The first error will be reported
    if (req->error() != QOrganizerItemManager::NoError)
        error = req->error();

    iIndex++;
    if (iIndex < itemIds.count()) {
        // Continue until all items fetched; emit new result, no state change
        SelfComplete();
        QOrganizerItemManagerEngine::updateItemFetchRequest(req, items, error, req->state());
    } else {
        // Done, emit state change
        QOrganizerItemManagerEngine::updateItemFetchRequest(
            req, items, error, QOrganizerItemAbstractRequest::FinishedState);
    }
}

// Fetch items/entries by details
void COrganizerItemRequestsServiceProvider::FetchItemsandFilter()
{
    Q_ASSERT(iReq->type() == QOrganizerItemAbstractRequest::ItemFetchRequest);
    QOrganizerItemFetchRequest *req = static_cast<QOrganizerItemFetchRequest *>(iReq);
    Q_ASSERT(iIndex < iItemIds.count());

    // Fetch the item
    QOrganizerItemManager::Error error(QOrganizerItemManager::NoError);
    QOrganizerItem item = iOrganizerItemManagerEngine.item(iItemIds.at(iIndex), req->fetchHint(), &error);

    QList<QOrganizerItem> items;
    if (error == QOrganizerItemManager::NoError) {
        // Append the fetched item to the result if it passes the filter
        items.append(item);
        items = iOrganizerItemManagerEngine.slowFilter(items, req->filter(), req->sorting());
    }
    items.append(req->items());

    // The first error will be reported
    if (req->error() != QOrganizerItemManager::NoError)
        error = req->error();

    iIndex++;
    if (iIndex < iItemIds.count()) {
        // Continue until all items fetched; emit new result, no state change
        SelfComplete();
        QOrganizerItemManagerEngine::updateItemFetchRequest(req, items, error, req->state());
    } else {
        // Done, emit state change
        QOrganizerItemManagerEngine::updateItemFetchRequest(
            req, items, error, QOrganizerItemAbstractRequest::FinishedState);
    }
}

// Fetch detail definition
void COrganizerItemRequestsServiceProvider::FetchDetailDefinition()
{
    QOrganizerItemDetailDefinitionFetchRequest *req = static_cast<QOrganizerItemDetailDefinitionFetchRequest *>(iReq);

    QMap<QString, QOrganizerItemDetailDefinition> detailDefinitionMap;
    QMap<int, QOrganizerItemManager::Error> errorMap;
    QOrganizerItemManager::Error error(QOrganizerItemManager::NoError);
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
    QOrganizerItemManager::Error error(QOrganizerItemManager::NoError);
    iOrganizerItemManagerEngine.transformError(aError, &error);
    // Insert error map
    iErrorMap.insert(iIndex, error);
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
void COrganizerItemRequestsServiceProvider::Reset()
{
    iIndex = 0;
    iErrorMap.clear();
    iItemIds.clear();
}
