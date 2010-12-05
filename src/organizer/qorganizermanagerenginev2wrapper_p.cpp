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

#include "qorganizermanagerenginev2wrapper_p.h"
#include "qorganizerabstractrequest_p.h"
#include "qorganizeritemsaverequest.h"
#include "qorganizeritemidfilter.h"
#include "qorganizeritem_p.h"

QOrganizerManagerEngineV2Wrapper::QOrganizerManagerEngineV2Wrapper(QOrganizerManagerEngine *wrappee)
    : m_engine(wrappee)
{
    Q_ASSERT(wrappee);
    // Connect all the signals
    connect(wrappee, SIGNAL(dataChanged()), this, SIGNAL(dataChanged()));
    connect(wrappee, SIGNAL(itemsAdded(QList<QOrganizerItemId>)),
            this, SIGNAL(itemsAdded(QList<QOrganizerItemId>)));
    connect(wrappee, SIGNAL(itemsChanged(QList<QOrganizerItemId>)),
            this, SIGNAL(itemsChanged(QList<QOrganizerItemId>)));
    connect(wrappee, SIGNAL(itemsRemoved(QList<QOrganizerItemId>)),
            this, SIGNAL(itemsRemoved(QList<QOrganizerItemId>)));
    connect(wrappee, SIGNAL(collectionsAdded(QList<QOrganizerCollectionId>)),
            this, SIGNAL(collectionsAdded(QList<QOrganizerCollectionId>)));
    connect(wrappee, SIGNAL(collectionsChanged(QList<QOrganizerCollectionId>)),
            this, SIGNAL(collectionsChanged(QList<QOrganizerCollectionId>)));
    connect(wrappee, SIGNAL(collectionsRemoved(QList<QOrganizerCollectionId>)),
            this, SIGNAL(collectionsRemoved(QList<QOrganizerCollectionId>)));

    connect(wrappee, SIGNAL(itemsChanged(QList<QOrganizerItemId>)),
            this, SLOT(itemsUpdated(QList<QOrganizerItemId>)));
    connect(wrappee, SIGNAL(itemsRemoved(QList<QOrganizerItemId>)),
            this, SLOT(itemsDeleted(QList<QOrganizerItemId>)));
}

QOrganizerManagerEngineV2Wrapper::~QOrganizerManagerEngineV2Wrapper()
{
    delete m_engine;
}

/*! \reimp
 * This function calls the wrapped engine's items function and ensures the results are sorted by
 * date (if no sort order is given).
 */
QList<QOrganizerItem> QOrganizerManagerEngineV2Wrapper::items(
        const QDateTime& startDate,
        const QDateTime& endDate,
        const QOrganizerItemFilter& filter,
        const QList<QOrganizerItemSortOrder>& sortOrders,
        const QOrganizerItemFetchHint& fetchHint,
        QOrganizerManager::Error* error) const
{
    QList<QOrganizerItem> list = m_engine->items(
            startDate, endDate, filter, sortOrders, fetchHint, error);
    if (sortOrders.isEmpty())
        qSort(list.begin(), list.end(), QOrganizerManagerEngine::itemLessThan);
    return list;
}

/*! \reimp
 * This function calls the wrapped engine's items function and ensures the results are sorted by
 * date and truncates the list after maxCount items (if maxCount is not negative)
 */
QList<QOrganizerItem> QOrganizerManagerEngineV2Wrapper::items(
        const QDateTime& startDate,
        const QDateTime& endDate,
        int maxCount,
        const QOrganizerItemFilter& filter,
        const QOrganizerItemFetchHint& fetchHint,
        QOrganizerManager::Error* error) const
{
    QList<QOrganizerItem> list = m_engine->items(
            startDate, endDate, filter, QList<QOrganizerItemSortOrder>(), fetchHint, error);
    // TODO: optimize this by using the quick select algorithm
    qSort(list.begin(), list.end(), itemLessThan);
    return list.mid(0, maxCount);
}

/*! \reimp */
void QOrganizerManagerEngineV2Wrapper::requestDestroyed(QOrganizerAbstractRequest* req)
{
    RequestController* controller = m_controllerForRequest.value(req);

    if (controller) {
        // If we own it, just delete the controller (and ignore any subrequests' signals from now on)
        delete controller;
        m_controllerForRequest.insert(req, 0);
    } else {
        m_engine->requestDestroyed(req);
    }
}

/*! \reimp */
bool QOrganizerManagerEngineV2Wrapper::startRequest(QOrganizerAbstractRequest* req)
{
    if (req->type() == QOrganizerAbstractRequest::ItemFetchRequest
            && (!static_cast<QOrganizerItemFetchRequest*>(req)->maxCount() > 0
             || !static_cast<QOrganizerItemFetchRequest*>(req)->sorting().isEmpty())) {
        RequestController* controller = new ItemFetchRequestController(m_engine);
        controller->setRequest(req);
        connect(controller, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)),
                this, SLOT(requestStateChanged(QOrganizerAbstractRequest::State)),
                Qt::QueuedConnection);
        m_controllerForRequest.insert(req, controller);
        if (controller->start()) {
            updateRequestState(req, QOrganizerAbstractRequest::ActiveState);
            return true;
        } else {
            return false;
        }
    }

    // Otherwise, pass it on
    return m_engine->startRequest(req);
}

/* A slot connected to the stateChanged signal of a controller object.  It is signaled by the
   controller to indicate that one of the client's requests has an updated state. */
void QOrganizerManagerEngineV2Wrapper::requestStateChanged(QOrganizerAbstractRequest::State state)
{
    RequestController* controller = qobject_cast<RequestController*>(sender());
    Q_ASSERT(controller);
    QOrganizerAbstractRequest* request = controller->request();

    if (state == QOrganizerAbstractRequest::FinishedState) {
        delete controller;
        if (request) { // It's possible the request was deleted by the sender.
            // Keep the key in m_controllerForRequest but point it to null to indicate a defunct
            // controller
            m_controllerForRequest.insert(request, 0);
        }
    } else {
        updateRequestState(request, state);
    }

}

/* \reimp */
bool QOrganizerManagerEngineV2Wrapper::cancelRequest(QOrganizerAbstractRequest* req)
{
    if (m_controllerForRequest.contains(req)) {
        RequestController* controller = m_controllerForRequest.value(req);
        if (controller) {
            // If we own it and it hasn't already been deleted,
            // just delete the controller (and ignore any subrequests' signals from now on)
            delete controller;
            m_controllerForRequest.insert(req, 0);
        }
        return true;
    } else {
        return m_engine->cancelRequest(req);
    }
}

/* \reimp */
bool QOrganizerManagerEngineV2Wrapper::waitForRequestFinished(QOrganizerAbstractRequest* req, int msecs)
{
    if (m_controllerForRequest.contains(req)) {
        RequestController* controller = m_controllerForRequest.value(req);
        if (controller) {
            if (controller->waitForFinished(msecs)) {
                updateRequestState(req, QOrganizerAbstractRequest::FinishedState);
                return true;
            } else {
                return false;
            }
        } else {
            // A request with a null controller means it has already finished
            return true;
        }
    } else {
        return m_engine->waitForRequestFinished(req, msecs);
    }

}

QSharedPointer<QOrganizerItemObserver> QOrganizerManagerEngineV2Wrapper::observeItem(
        const QOrganizerItemId& itemId)
{
    QOrganizerItemObserver* observer = createOrganizerItemObserver(this);
    connect(observer, SIGNAL(destroyed(QObject*)),
            this, SLOT(observerDestroyed(QOrganizerItemObserver*)));
    m_observerForItem.insert(itemId, observer);
    return QSharedPointer<QOrganizerItemObserver>(observer);
}

void QOrganizerManagerEngineV2Wrapper::observerDestroyed(QOrganizerItemObserver* observer)
{
    QOrganizerItemId key = m_observerForItem.key(observer);
    if (!key.isNull()) {
        m_observerForItem.remove(key, observer);
    }
}

void QOrganizerManagerEngineV2Wrapper::itemsUpdated(const QList<QOrganizerItemId>& ids)
{
    foreach (QOrganizerItemId id, ids) {
        QHash<QOrganizerItemId, QOrganizerItemObserver*>::iterator it = m_observerForItem.find(id);
        while (it != m_observerForItem.end()) {
            (*it)->emitItemChanged();
            it++;
        }
    }
}

void QOrganizerManagerEngineV2Wrapper::itemsDeleted(const QList<QOrganizerItemId>& ids)
{
    foreach (QOrganizerItemId id, ids) {
        QHash<QOrganizerItemId, QOrganizerItemObserver*>::iterator it = m_observerForItem.find(id);
        while (it != m_observerForItem.end()) {
            (*it)->emitItemRemoved();
            it++;
        }
    }
}

/* A static helper to twiddle with \a request's privates, setting its engine to \a engine. */
void QOrganizerManagerEngineV2Wrapper::setEngineOfRequest(QOrganizerAbstractRequest* request,
                                                          QOrganizerManagerEngine* engine)
{
    request->d_ptr->m_engine = engine;
}

// General RequestController stuff
/* A single RequestController is associated with a single client QOrganizerAbstractRequest.  It
   manages the sub-requests that need to be sent to the real engine and controls the asynchronous
   flow between start() and one or more handleUpdatedSubRequest() calls that might follow.
   waitForFinished() can also be called on a controller, which synchronously performs the rest of
   the necessary sub-requests.
 */

/* A slot connected the stateChanged signal of a sub request */
void RequestController::handleUpdatedSubRequest(QOrganizerAbstractRequest::State state)
{
    QObject* caller = sender();
    QOrganizerAbstractRequest* subRequest = qobject_cast<QOrganizerAbstractRequest*>(caller);
    if (subRequest) {
        if (state == QOrganizerAbstractRequest::FinishedState) {
            // It's possibly already finished if waitForFinished has previously been called
            if (!isFinished())
                handleFinishedSubRequest(subRequest);
        } else {
            // XXX maybe Canceled should be handled
        }
    }
}

/* This function handles the rest of the program flow in a synchronous way. */
bool RequestController::waitForFinished(int msecs)
{
    // If the current request is active, it must be an OrganizerFetchRequest.  We just need to
    // wait for it to finish, then finalize the post-processing.
    if (m_currentSubRequest.isNull()) {
        return false;
    }
    while (!isFinished()) {
        if (!m_currentSubRequest->waitForFinished(msecs))
            return false;
        handleFinishedSubRequest(m_currentSubRequest.data());
    }
    return true;
}



// ItemFetchRequestController stuff
/* An ItemFetchRequestController is a RequestController for QOrganizerItemFetchRequests */
bool ItemFetchRequestController::start()
{
    Q_ASSERT(m_request);
    QOrganizerItemFetchRequest* originalRequest =
        static_cast<QOrganizerItemFetchRequest*>(m_request.data());

    // Take a copy of the item fetch request
    QOrganizerItemFetchRequest* subRequest = new QOrganizerItemFetchRequest;
    subRequest->setFilter(originalRequest->filter());
    subRequest->setSorting(originalRequest->sorting());
    subRequest->setFetchHint(originalRequest->fetchHint());
    subRequest->setStartDate(originalRequest->startDate());
    subRequest->setEndDate(originalRequest->endDate());
    m_currentSubRequest.reset(subRequest);

    // normally, you'd set the manager, but in this case, we only have a bare engine:
    QOrganizerManagerEngineV2Wrapper::setEngineOfRequest(subRequest, m_engine);
    m_currentSubRequest.reset(subRequest);
    connect(subRequest, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)),
            this, SLOT(handleUpdatedSubRequest(QOrganizerAbstractRequest::State)),
            Qt::QueuedConnection);
    return subRequest->start();
}

/* One of our subrequests has finished.  Go to the next step. */
void ItemFetchRequestController::handleFinishedSubRequest(QOrganizerAbstractRequest* subReq)
{
    // For an ItemFetchRequest, we know that the only subrequest is an ItemFetchRequest.
    // The next step is simply to take the results and count-limit them
    // Take the results:
    QOrganizerItemFetchRequest* subRequest = qobject_cast<QOrganizerItemFetchRequest*>(subReq);
    QList<QOrganizerItem> items = subRequest->items();
    QOrganizerManager::Error error = subRequest->error();

    QOrganizerItemFetchRequest* request(static_cast<QOrganizerItemFetchRequest*>(m_request.data()));

    // Sort and limit the number of items
    // TODO: optimize this with the quick select algorithm if maxCount is set
    if (subRequest->sorting().isEmpty()) // sort by date if no sort order is given
        qSort(items.begin(), items.end(), QOrganizerManagerEngine::itemLessThan);
    int maxCount = request->maxCount();
    if (maxCount >= 0)
        items = items.mid(0, maxCount);

    // Update the request object
    QOrganizerManagerEngine::updateItemFetchRequest(
            request,
            items,
            error,
            QOrganizerAbstractRequest::FinishedState);
    finish();
}


// FetchByIdRequestController stuff
/* A FetchByIdRequestController is a RequestController for QOrganizerItemFetchByIdRequests */
bool FetchByIdRequestController::start()
{
    // Our strategy is to translate it to a OrganizerItemFetchRequest.  Later when it finishes, we can
    // fiddle with the results to get it in the right format.
    Q_ASSERT(m_request);
    QOrganizerItemFetchByIdRequest* originalRequest = static_cast<QOrganizerItemFetchByIdRequest*>(m_request.data());
    QOrganizerItemFetchRequest* qoifr = new QOrganizerItemFetchRequest;
    QOrganizerItemIdFilter filter;
    filter.setIds(originalRequest->ids());
    qoifr->setFilter(filter);
    qoifr->setFetchHint(originalRequest->fetchHint());
    // normally, you'd set the manager, but in this case, we only have a bare engine:
    QOrganizerManagerEngineV2Wrapper::setEngineOfRequest(qoifr, m_engine);
    m_currentSubRequest.reset(qoifr);
    connect(qoifr, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)),
            this, SLOT(handleUpdatedSubRequest(QOrganizerAbstractRequest::State)),
            Qt::QueuedConnection);
    return qoifr->start();
}

/* One of our subrequests has finished.  Go to the next step. */
void FetchByIdRequestController::handleFinishedSubRequest(QOrganizerAbstractRequest* subReq)
{
    // For a FetchByIdRequest, we know that the only subrequest is a
    // QOrganizerItemFetchForExportRequest.
    // The next step is simply to take the results and reformat it.
    // Take the results:
    QOrganizerItemFetchForExportRequest* qoiffer = qobject_cast<QOrganizerItemFetchForExportRequest*>(subReq);
    QList<QOrganizerItem> items = qoiffer->items();
    QOrganizerManager::Error error = qoiffer->error();

    // Build an index into the results
    QHash<QOrganizerItemId, int> idMap; // value is index into unsorted
    if (error == QOrganizerManager::NoError) {
        for (int i = 0; i < items.size(); i++) {
            idMap.insert(items[i].id(), i);
        }
    }

    // Find the order in which the results should be presented
    QOrganizerItemFetchByIdRequest* request = static_cast<QOrganizerItemFetchByIdRequest*>(m_request.data());
    QList<QOrganizerItemId> ids(request->ids());

    // Build up the results and errors
    QList<QOrganizerItem> results;
    QMap<int, QOrganizerManager::Error> errorMap;
    for (int i = 0; i < ids.count(); i++) {
        QOrganizerItemId id(ids[i]);
        if (!idMap.contains(id)) {
            errorMap.insert(i, QOrganizerManager::DoesNotExistError);
            if (error == QOrganizerManager::NoError)
                error = QOrganizerManager::DoesNotExistError;
            results.append(QOrganizerItem());
        } else {
            results.append(items[idMap[id]]);
        }
    }

    // Update the request object
    QOrganizerManagerEngineV2Wrapper::updateItemFetchByIdRequest(
            request,
            results,
            error,
            errorMap,
            QOrganizerAbstractRequest::FinishedState);
    finish();
}


// PartialSaveRequestController stuff
/* A PartialSaveRequestController is a RequestController for QOrganizerItemSaveRequests with definition mask */
bool PartialSaveRequestController::start()
{
    // The first step is to fetch the existing items.
    QList<QOrganizerItemId> existingItemIds;

    // First, remove the items that aren't from this manager
    QList<QOrganizerItem> items(request()->items());
    // Try to figure out which of our arguments are new items
    for(int i = 0; i < items.count(); i++) {
        // See if there's a itemId that's not from this manager
        const QOrganizerItem item = items.at(i);
        if (item.id().managerUri() == m_engine->managerUri()) {
            if (!item.id().isNull()) {
                m_existingIdMap.insert(i, existingItemIds.count());
                existingItemIds.append(item.id());
            } else {
                // Strange. it's just a new item
            }
        } else if (!item.id().managerUri().isEmpty() || !item.id().isNull()) {
            // Hmm, error (wrong manager)
            m_errorMap.insert(i, QOrganizerManager::DoesNotExistError);
        } // else new item
    }

    // Now do the fetch and wait for the result in handleFinishedSubRequest
    QOrganizerItemFetchByIdRequest* oifbir = new QOrganizerItemFetchByIdRequest;
    oifbir->setIds(existingItemIds);
    // normally, you'd set the manager, but in this case, we only have a bare engine:
    QOrganizerManagerEngineV2Wrapper::setEngineOfRequest(oifbir, m_v2wrapper);
    m_currentSubRequest.reset(oifbir);
    connect(oifbir, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)),
            this, SLOT(handleUpdatedSubRequest(QOrganizerAbstractRequest::State)),
            Qt::QueuedConnection);
    return oifbir->start();
}

/* One of our subrequests has finished.  Go to the next step. */
void PartialSaveRequestController::handleFinishedSubRequest(QOrganizerAbstractRequest* subReq)
{
    if (subReq->type() == QOrganizerAbstractRequest::ItemFetchByIdRequest) {
        QOrganizerItemFetchByIdRequest* oifbir = qobject_cast<QOrganizerItemFetchByIdRequest*>(subReq);
        QList<QOrganizerItem> itemsToSave;
        QMap<int, QOrganizerManager::Error> fetchErrors(oifbir->errorMap());
        QList<QOrganizerItem> existingItems(oifbir->items());
        QList<QOrganizerItem> items(request()->items());
        QSet<QString> mask(request()->definitionMask().toSet());
        for (int i = 0; i < items.count(); i++) {
            // See if this is an existing item or a new one
            const int fetchedIdx = m_existingIdMap.value(i, -1);
            QOrganizerItem itemToSave;
            if (fetchedIdx >= 0) {
                // See if we had an error
                if (fetchErrors[fetchedIdx] != QOrganizerManager::NoError) {
                    m_errorMap.insert(i, fetchErrors[fetchedIdx]);
                    continue;
                }

                // Existing item we should have fetched
                itemToSave = existingItems.at(fetchedIdx);

                QSharedDataPointer<QOrganizerItemData>& data = QOrganizerItemData::itemData(itemToSave);
                data->removeOnly(mask);
            } else if (m_errorMap.contains(i)) {
                // A bad argument.  Leave it out of the itemsToSave list
                continue;
            } // else new item

            // Now copy in the details from the arguments
            const QOrganizerItem& item = items.at(i);

            // Perhaps this could do this directly rather than through saveDetail
            // but that would duplicate the checks for display label etc
            foreach (const QString& name, mask) {
                QList<QOrganizerItemDetail> details = item.details(name);
                foreach(QOrganizerItemDetail detail, details) {
                    itemToSave.saveDetail(&detail);
                }
            }

            m_savedToOriginalMap.append(i);
            itemsToSave.append(itemToSave);
        }

        // Now do the fetch and wait for the result
        QOrganizerItemSaveRequest* oisr = new QOrganizerItemSaveRequest;
        oisr->setItems(itemsToSave);
        // normally, you'd set the manager, but in this case, we only have a bare engine:
        QOrganizerManagerEngineV2Wrapper::setEngineOfRequest(oisr, m_engine);
        m_currentSubRequest.reset(oisr);
        connect(oisr, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)),
                this, SLOT(handleUpdatedSubRequest(QOrganizerAbstractRequest::State)),
                Qt::QueuedConnection);
        oisr->start(); // TODO what if this fails?
    } else if (subReq->type() == QOrganizerAbstractRequest::ItemSaveRequest) {
        QOrganizerItemSaveRequest* oisr = qobject_cast<QOrganizerItemSaveRequest*>(subReq);
        QList<QOrganizerItem> savedItems(oisr->items());
        QMap<int, QOrganizerManager::Error> saveErrors(oisr->errorMap());
        QList<QOrganizerItem> items(request()->items());
        for (int i = 0; i < savedItems.count(); i++) {
            items[m_savedToOriginalMap[i]].setId(savedItems[i].id());
        }
        // Populate the m_errorMap with the m_errorMap of the attempted save
        QMap<int, QOrganizerManager::Error>::iterator it(saveErrors.begin());
        QOrganizerManager::Error error = QOrganizerManager::NoError;
        while (it != saveErrors.end()) {
            error = it.value();
            m_errorMap.insert(m_savedToOriginalMap[it.key()], it.value());
            it++;
        }

        // Update the request object
        QOrganizerManagerEngine::updateItemSaveRequest(
                request(),
                items,
                error,
                m_errorMap,
                QOrganizerAbstractRequest::FinishedState);
        finish();
    } else {
        Q_ASSERT(false);
    }
}
