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
        QOrganizerAbstractRequest* aReq)
{
    if (!IsActive()) {
        // Store the request to be processed
        iReq = aReq;
        // Initialize the member variables for the new requests
        Reset();

        switch (iReq->type()) {
        case QOrganizerAbstractRequest::DetailDefinitionRemoveRequest:
            {
                // Special case, removing detail definitions not supported
                QOrganizerManagerEngine::updateDefinitionRemoveRequest(
                    static_cast<QOrganizerItemDetailDefinitionRemoveRequest *>(iReq),
                    QOrganizerManager::NotSupportedError,
                    QMap<int, QOrganizerManager::Error>(),
                    QOrganizerAbstractRequest::FinishedState);
            }
            return false;
        case QOrganizerAbstractRequest::DetailDefinitionSaveRequest:
            {
                // Special case, saving detail definitions not supported
                QOrganizerManagerEngine::updateDefinitionSaveRequest(
                    static_cast<QOrganizerItemDetailDefinitionSaveRequest *>(iReq),
                    QList<QOrganizerItemDetailDefinition>(),
                    QOrganizerManager::NotSupportedError,
                    QMap<int, QOrganizerManager::Error>(),
                    QOrganizerAbstractRequest::FinishedState);
            }
            return false;
#ifndef SYMBIAN_CALENDAR_V2
        case QOrganizerAbstractRequest::CollectionRemoveRequest :
            {
                // Special case, removing a collection not supported
                QOrganizerManagerEngine::updateCollectionRemoveRequest(
                    static_cast<QOrganizerCollectionRemoveRequest *>(iReq),
                    QOrganizerManager::NotSupportedError,
                    QMap<int, QOrganizerManager::Error>(),
                    QOrganizerAbstractRequest::FinishedState);
            }
            return false;                
        case QOrganizerAbstractRequest::CollectionSaveRequest :
            {
                // Special case, saving a collection not supported
                QOrganizerManagerEngine::updateCollectionSaveRequest(
                    static_cast<QOrganizerCollectionSaveRequest *>(iReq),
                    QList<QOrganizerCollection>(),
                    QOrganizerManager::NotSupportedError,
                    QMap<int, QOrganizerManager::Error>(),
                    QOrganizerAbstractRequest::FinishedState);
            }
            return false;
#else
        case QOrganizerAbstractRequest::CollectionRemoveRequest:       // Fallthrough
        case QOrganizerAbstractRequest::CollectionSaveRequest:         // .
#endif
        case QOrganizerAbstractRequest::ItemFetchForExportRequest:     // .
        case QOrganizerAbstractRequest::ItemRemoveRequest:             // .
        case QOrganizerAbstractRequest::ItemSaveRequest:               // .
        case QOrganizerAbstractRequest::ItemFetchRequest:              // .
        case QOrganizerAbstractRequest::ItemIdFetchRequest:       // .
        case QOrganizerAbstractRequest::DetailDefinitionFetchRequest:  // .
        case QOrganizerAbstractRequest::CollectionFetchRequest:
            {
                // QWeakPointer is aware if the request object (which is derived from QObject) is deleted.
                QWeakPointer<QOrganizerAbstractRequest> req = aReq;

                // Change the state of the request and emit signal
                QOrganizerManagerEngine::updateRequestState(aReq,
                        QOrganizerAbstractRequest::ActiveState);

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
    case QOrganizerAbstractRequest::ItemFetchRequest: 
        {
        FetchItems();
        }
        break;
    case QOrganizerAbstractRequest::ItemFetchForExportRequest: 
        {
        FetchItemsForExport();
        }
        break;
    case QOrganizerAbstractRequest::ItemIdFetchRequest: 
        {
        FetchItemIds();
        }
        break;
    case QOrganizerAbstractRequest::ItemRemoveRequest: 
        {
        RemoveItem();
        }
        break;
    case QOrganizerAbstractRequest::ItemSaveRequest: 
        {
        SaveItem();
        }
        break;
    case QOrganizerAbstractRequest::DetailDefinitionFetchRequest: 
        {
        FetchDetailDefinition();
        }
        break;
    case QOrganizerAbstractRequest::DetailDefinitionRemoveRequest: 
        {
        // Not supported. Already handled at StartRequest()
        }
        break;
    case QOrganizerAbstractRequest::DetailDefinitionSaveRequest: 
        {
        // Not supported. Already handled at StartRequest()
        }
        break;
    case QOrganizerAbstractRequest::CollectionFetchRequest : 
        {
        FetchCollections();
        }
        break;
    case QOrganizerAbstractRequest::CollectionRemoveRequest :
        {
        RemoveCollections();
        }
        break;
    case QOrganizerAbstractRequest::CollectionSaveRequest :
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
void COrganizerItemRequestsServiceProvider::FetchItems()
{
    QOrganizerItemFetchRequest *req = static_cast<QOrganizerItemFetchRequest *>(iReq);
    // Fetch ItemOccurrencesList
    QOrganizerManager::Error error(QOrganizerManager::NoError);
    QList<QOrganizerItem> itemList = iOrganizerItemManagerEngine.items(
        req->startDate(), req->endDate(), req->filter(), req->sorting(), req->fetchHint(), &error);

    // Update the request status
    QOrganizerManagerEngine::updateItemFetchRequest(
        req, itemList, error, QOrganizerAbstractRequest::FinishedState);
}

// Delete item
void COrganizerItemRequestsServiceProvider::RemoveItem()
{
    Q_ASSERT(iReq->type() == QOrganizerAbstractRequest::ItemRemoveRequest);
    QOrganizerItemRemoveRequest *req = static_cast<QOrganizerItemRemoveRequest *>(iReq);
    QList<QOrganizerItemId> itemIds = req->itemIds();
    Q_ASSERT(iIndex < itemIds.count());

    QOrganizerManager::Error error(QOrganizerManager::NoError);
    // Delete an item (emits "itemsRemoved")
    iOrganizerItemManagerEngine.removeItem(itemIds.at(iIndex), &error);
    if (error != QOrganizerManager::NoError)
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
        QOrganizerManagerEngine::updateItemRemoveRequest(
            req, error, iErrorMap, QOrganizerAbstractRequest::FinishedState);
    }
}

// Save item
void COrganizerItemRequestsServiceProvider::SaveItem()
{
    Q_ASSERT(iReq->type() == QOrganizerAbstractRequest::ItemSaveRequest);
    QOrganizerItemSaveRequest *req = static_cast<QOrganizerItemSaveRequest *>(iReq);
    QList<QOrganizerItem> items = req->items();
    Q_ASSERT(iIndex < items.count());

    QOrganizerManager::Error error(QOrganizerManager::NoError);
    // Validate item before saving
    if (iOrganizerItemManagerEngine.validateItem(items[iIndex], &error))
        {
        // Save item (emits "itemsAdded")
        iOrganizerItemManagerEngine.saveItem(&(items[iIndex]), &error);
        }

    if (error == QOrganizerManager::NoError) {
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
        QOrganizerManagerEngine::updateItemSaveRequest(
            req, result, error, iErrorMap, QOrganizerAbstractRequest::FinishedState);
    }    
}

void COrganizerItemRequestsServiceProvider::FetchItemsForExport()
{
    QOrganizerItemFetchForExportRequest *req = static_cast<QOrganizerItemFetchForExportRequest *>(iReq);
    if (req->filter().type() == QOrganizerItemFilter::IdFilter) {
        FetchItemsByIds();
    } else {
        if (!iItemIds.count()) {
            // Fetch all item ids
            QOrganizerItemFilter filter;
            QList<QOrganizerItemSortOrder> sortOrder;
            QOrganizerManager::Error error(QOrganizerManager::NoError);
            iItemIds = iOrganizerItemManagerEngine.itemIds(req->startDate(), req->endDate(), filter, sortOrder, &error);
            if (error != QOrganizerManager::NoError || !iItemIds.count()) {
                // Complete with empty list
                QOrganizerManagerEngine::updateItemFetchForExportRequest(
                    req, QList<QOrganizerItem>(), error, QOrganizerAbstractRequest::FinishedState);
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
    Q_ASSERT(iReq->type() == QOrganizerAbstractRequest::ItemIdFetchRequest);
    QOrganizerItemIdFetchRequest *req = static_cast<QOrganizerItemIdFetchRequest *>(iReq);
    QOrganizerManager::Error error(QOrganizerManager::NoError);
    QList<QOrganizerItemId> itemIds = iOrganizerItemManagerEngine.itemIds(req->startDate(), req->endDate(), req->filter(), req->sorting(), &error);
    QOrganizerManagerEngine::updateItemIdFetchRequest(
        req, itemIds, error, QOrganizerAbstractRequest::FinishedState);
}

// Fetch Entries by local Ids
void COrganizerItemRequestsServiceProvider::FetchItemsByIds()
{
    Q_ASSERT(iReq->type() == QOrganizerAbstractRequest::ItemFetchForExportRequest);
    QOrganizerItemFetchForExportRequest *req = static_cast<QOrganizerItemFetchForExportRequest *>(iReq);
    Q_ASSERT(req->filter().type() == QOrganizerItemFilter::IdFilter);
    QOrganizerItemIdFilter localIdFilter = req->filter();
    QList<QOrganizerItemId> itemIds = localIdFilter.ids();
    Q_ASSERT(iIndex < itemIds.count());

    QOrganizerItemFetchHint fetchHint;
    QOrganizerManager::Error error(QOrganizerManager::NoError);
    // Fetch the item
    QOrganizerItem item =
        iOrganizerItemManagerEngine.item(localIdFilter.ids().at(iIndex), fetchHint, &error);

    // Append the fetched item to the result
    // TODO: we may need to batch signaling to improve performance,
    // for example "resultsAvailable" for every tenth iteration
    QList<QOrganizerItem> items = req->items();
    if (error == QOrganizerManager::NoError) {
        items << item;
    }

    // The first error will be reported
    if (req->error() != QOrganizerManager::NoError)
        error = req->error();

    iIndex++;
    if (iIndex < itemIds.count()) {
        // Continue until all items fetched; emit new result, no state change
        SelfComplete();
        QOrganizerManagerEngine::updateItemFetchForExportRequest(req, items, error, req->state());
    } else {
        // Done, emit state change
        QOrganizerManagerEngine::updateItemFetchForExportRequest(
            req, items, error, QOrganizerAbstractRequest::FinishedState);
    }
}

// Fetch items/entries by details
void COrganizerItemRequestsServiceProvider::FetchItemsandFilter()
{
    Q_ASSERT(iReq->type() == QOrganizerAbstractRequest::ItemFetchForExportRequest);
    QOrganizerItemFetchForExportRequest *req = static_cast<QOrganizerItemFetchForExportRequest *>(iReq);
    Q_ASSERT(iIndex < iItemIds.count());

    // Fetch the item
    QOrganizerManager::Error error(QOrganizerManager::NoError);
    QOrganizerItem item = iOrganizerItemManagerEngine.item(iItemIds.at(iIndex), req->fetchHint(), &error);

    QList<QOrganizerItem> items;
    if (error == QOrganizerManager::NoError) {
        // Append the fetched item to the result if it passes the filter
        items.append(item);
        items = iOrganizerItemManagerEngine.slowFilterItems(items, req->filter(), req->sorting());
    }
    items.append(req->items());

    // The first error will be reported
    if (req->error() != QOrganizerManager::NoError)
        error = req->error();

    iIndex++;
    if (iIndex < iItemIds.count()) {
        // Continue until all items fetched; emit new result, no state change
        SelfComplete();
        QOrganizerManagerEngine::updateItemFetchForExportRequest(req, items, error, req->state());
    } else {
        // Done, emit state change
        QOrganizerManagerEngine::updateItemFetchForExportRequest(
            req, items, error, QOrganizerAbstractRequest::FinishedState);
    }
}

// Fetch detail definition
void COrganizerItemRequestsServiceProvider::FetchDetailDefinition()
{
    QOrganizerItemDetailDefinitionFetchRequest *req = static_cast<QOrganizerItemDetailDefinitionFetchRequest *>(iReq);

    QMap<QString, QOrganizerItemDetailDefinition> detailDefinitionMap;
    QMap<int, QOrganizerManager::Error> errorMap;
    QOrganizerManager::Error error(QOrganizerManager::NoError);
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

            if (QOrganizerManager::NoError == error) {
                detailDefinitionMap.insert(stringItem, detailDefinition);
            } else {
                errorMap.insert(index, error);
            }
        }
    }
    
    // Notify results
    QOrganizerManagerEngine::updateDefinitionFetchRequest(
        req, detailDefinitionMap, error, errorMap, QOrganizerAbstractRequest::FinishedState);
}

void COrganizerItemRequestsServiceProvider::FetchCollections()
{
    Q_ASSERT(iReq->type() == QOrganizerAbstractRequest::CollectionFetchRequest);
    QOrganizerCollectionFetchRequest *fetchReq = static_cast<QOrganizerCollectionFetchRequest *>(iReq);

    QOrganizerManager::Error error(QOrganizerManager::NoError);
    QList<QOrganizerCollection> result = iOrganizerItemManagerEngine.collections(&error);
    QOrganizerManagerEngine::updateCollectionFetchRequest(
        fetchReq,
        result,
        error,
        QOrganizerAbstractRequest::FinishedState);
}

void COrganizerItemRequestsServiceProvider::SaveCollections()
{
    Q_ASSERT(iReq->type() == QOrganizerAbstractRequest::CollectionSaveRequest);

    QOrganizerCollectionSaveRequest *saveReq = static_cast<QOrganizerCollectionSaveRequest *>(iReq);
    QList<QOrganizerCollection> collections = saveReq->collections();
    QOrganizerManager::Error error(QOrganizerManager::NoError);
    QMap<int, QOrganizerManager::Error> errorMap;

    // Save all collections
    for (int i(0); i < collections.count(); i++) {
        // The following also emits the necessary signals
        iOrganizerItemManagerEngine.saveCollection(&(collections[i]), &error);
        if (error != QOrganizerManager::NoError) {
            errorMap.insert(i, error);
        }
    }

    // Notify completion
    QOrganizerManagerEngine::updateCollectionSaveRequest(
        saveReq,
        collections,
        error,
        errorMap,
        QOrganizerAbstractRequest::FinishedState);
}

void COrganizerItemRequestsServiceProvider::RemoveCollections()
{
    Q_ASSERT(iReq->type() == QOrganizerAbstractRequest::CollectionRemoveRequest);

    QOrganizerCollectionRemoveRequest *removeReq = static_cast<QOrganizerCollectionRemoveRequest *>(iReq);
    QList<QOrganizerCollectionId> collectionIds = removeReq->collectionIds();
    QOrganizerManager::Error error(QOrganizerManager::NoError);
    QMap<int, QOrganizerManager::Error> errorMap;

    // Remove all collections
    for (int i(0); i < collectionIds.count(); i++) {
        // The following also emits the necessary signals
        iOrganizerItemManagerEngine.removeCollection(collectionIds.at(i), &error);
        if (error != QOrganizerManager::NoError) {
            errorMap.insert(i, error);
        }
    }

    // Notify completion
    QOrganizerManagerEngine::updateCollectionRemoveRequest(
        removeReq,
        error,
        errorMap,
        QOrganizerAbstractRequest::FinishedState);
}

// Called by Cancel()
void COrganizerItemRequestsServiceProvider::DoCancel()
{
    // Update the request status
    QOrganizerManagerEngine::updateRequestState(iReq, 
            QOrganizerAbstractRequest::CanceledState);
}

// Call if RunL leaves
TInt COrganizerItemRequestsServiceProvider::RunError(TInt aError)
{
    // Operation not successfull
    // Generate error map
    QOrganizerManager::Error error(QOrganizerManager::NoError);
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
