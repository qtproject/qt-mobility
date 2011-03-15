/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qorganizerasynchmanager.h"
#include "qtorganizer.h"

QTM_USE_NAMESPACE

class AsyncWorker: public QThread
{
public:
    AsyncWorker(OrganizerAsynchManager* manager): m_manager(manager), m_req(0), m_kill(false) {};
    ~AsyncWorker() {kill();}

    void assignRequest(QOrganizerAbstractRequest* r);
    void kill();
protected:
    void run();
private:
    OrganizerAsynchManager* m_manager;
    QOrganizerAbstractRequest* m_req;
    QMutex m_lock;
    QWaitCondition m_wait;
    bool m_kill;

    void processRequest(QOrganizerAbstractRequest *req);

    void handleItemFetchRequest(QOrganizerItemFetchRequest *req);
    void handleItemFetchForExportRequest(QOrganizerItemFetchForExportRequest *req);
    void handleItemOccurrenceFetchRequest(QOrganizerItemOccurrenceFetchRequest *req);
    void handleIdFetchRequest(QOrganizerItemIdFetchRequest *req);
    void handleItemRemoveRequest(QOrganizerItemRemoveRequest *req);
    void handleSaveRequest(QOrganizerItemSaveRequest *req);
    void handleDefinitionFetchRequest(QOrganizerItemDetailDefinitionFetchRequest *req);
    void handleDefinitionRemoveRequest(QOrganizerItemDetailDefinitionRemoveRequest *req);
    void handleDefinitionSaveRequest(QOrganizerItemDetailDefinitionSaveRequest *req);
    void handleCollectionFetchRequest(QOrganizerCollectionFetchRequest *req);
    void handleCollectionRemoveRequest(QOrganizerCollectionRemoveRequest *req);
    void handleCollectionSaveRequest(QOrganizerCollectionSaveRequest *req);
};

void AsyncWorker::assignRequest(QOrganizerAbstractRequest* r)
{
    QMutexLocker lock(&m_lock);
    Q_ASSERT_X(!m_req, "Worker", "Something is wrong with locking mechanism!");
    m_req = r;

    //either start the thread if is not running or wake up the sleeping thread
    if (!isRunning())
        start();
    else
        m_wait.wakeAll();
}

void AsyncWorker::kill()
{
    {
        QMutexLocker locker(&m_lock);

        //mark the exit flag and wake up the thread if is sleeping
        m_kill = true;
        m_wait.wakeAll();
    }

    //wait the thread to finish
    wait();
}

void AsyncWorker::run()
{
    QMutexLocker locker(&m_lock);

    //until manager destructor is called
    while (!m_kill) {
        //obtain a request
        QOrganizerAbstractRequest *r = m_req;
        m_req = 0;

        //if there are no requests sleep
        if (!r) {
            m_wait.wait(&m_lock);
        } else {
            locker.unlock();
            //process the request
            processRequest(r);
            //signal the manager
            if (!m_kill)
                m_manager->workerDone(this, r);
            locker.relock();
        }
    }
}

void AsyncWorker::processRequest(QOrganizerAbstractRequest* req)
{
    //if the request state is not active no need to do anything
    QOrganizerAbstractRequest::State state = req->state();

    //if the request is in inactive state, start the request (this could happen due to thread scheduling)
    if (state == QOrganizerAbstractRequest::InactiveState ||
        state == QOrganizerAbstractRequest::FinishedState) {
        QOrganizerManagerEngine::updateRequestState(req, QOrganizerAbstractRequest::ActiveState);
    } else if (state != QOrganizerAbstractRequest::ActiveState) {
        return;
    }

    //process the request
    switch(req->type()) {
    case QOrganizerAbstractRequest::ItemFetchRequest:
        handleItemFetchRequest(static_cast<QOrganizerItemFetchRequest *>(req));
        break;
    case QOrganizerAbstractRequest::ItemFetchForExportRequest:
        handleItemFetchForExportRequest(static_cast<QOrganizerItemFetchForExportRequest *>(req));
        break;
    case QOrganizerAbstractRequest::ItemOccurrenceFetchRequest:
        handleItemOccurrenceFetchRequest(static_cast<QOrganizerItemOccurrenceFetchRequest *>(req));
        break;
    case QOrganizerAbstractRequest::ItemIdFetchRequest:
        handleIdFetchRequest(static_cast<QOrganizerItemIdFetchRequest *>(req));
        break;
    case QOrganizerAbstractRequest::ItemRemoveRequest:
        handleItemRemoveRequest(static_cast<QOrganizerItemRemoveRequest *>(req));
        break;
    case QOrganizerAbstractRequest::ItemSaveRequest:
        handleSaveRequest(static_cast<QOrganizerItemSaveRequest *>(req));
        break;
    case QOrganizerAbstractRequest::DetailDefinitionFetchRequest:
        handleDefinitionFetchRequest(static_cast<QOrganizerItemDetailDefinitionFetchRequest *>(req));
        break;
    case QOrganizerAbstractRequest::DetailDefinitionRemoveRequest:
        handleDefinitionRemoveRequest(static_cast<QOrganizerItemDetailDefinitionRemoveRequest *>(req));
        break;
    case QOrganizerAbstractRequest::DetailDefinitionSaveRequest:
        handleDefinitionSaveRequest(static_cast<QOrganizerItemDetailDefinitionSaveRequest *>(req));
        break;
    case QOrganizerAbstractRequest::CollectionFetchRequest:
        handleCollectionFetchRequest(static_cast<QOrganizerCollectionFetchRequest *>(req));
        break;
    case QOrganizerAbstractRequest::CollectionRemoveRequest:
        handleCollectionRemoveRequest(static_cast<QOrganizerCollectionRemoveRequest *>(req));
        break;
    case QOrganizerAbstractRequest::CollectionSaveRequest:
        handleCollectionSaveRequest(static_cast<QOrganizerCollectionSaveRequest *>(req));
        break;

    default:
        // invalid request
        break;
    }
}

void AsyncWorker::handleItemFetchRequest(QOrganizerItemFetchRequest *req)
{
    QOrganizerManager::Error err = QOrganizerManager::NoError;
    QList<QOrganizerItem> items = m_manager->m_engine->items(req->startDate(), req->endDate(), req->filter(), req->sorting(), req->fetchHint(), &err);
    QOrganizerManagerEngine::updateItemFetchRequest(req, items, err, QOrganizerAbstractRequest::FinishedState);
}

void AsyncWorker::handleItemFetchForExportRequest(QOrganizerItemFetchForExportRequest *req)
{
    QOrganizerManager::Error err = QOrganizerManager::NoError;
    QList<QOrganizerItem> items = m_manager->m_engine->itemsForExport(req->startDate(), req->endDate(), req->filter(), req->sorting(), req->fetchHint(), &err);
    QOrganizerManagerEngine::updateItemFetchForExportRequest(req, items, err, QOrganizerAbstractRequest::FinishedState);
}

void AsyncWorker::handleItemOccurrenceFetchRequest(QOrganizerItemOccurrenceFetchRequest *req)
{
    QOrganizerManager::Error err = QOrganizerManager::NoError;
    QList<QOrganizerItem> items = m_manager->m_engine->itemOccurrences(req->parentItem(), req->startDate(), req->endDate(), req->maxOccurrences(), req->fetchHint(), &err);
    QOrganizerManagerEngine::updateItemOccurrenceFetchRequest(req, items, err, QOrganizerAbstractRequest::FinishedState);
}

void AsyncWorker::handleIdFetchRequest(QOrganizerItemIdFetchRequest *req)
{
    QOrganizerManager::Error err = QOrganizerManager::NoError;
    QList<QOrganizerItemId> ids = m_manager->m_engine->itemIds(req->startDate(), req->endDate(), req->filter(), req->sorting(), &err);
    QOrganizerManagerEngine::updateItemIdFetchRequest(req, ids, err, QOrganizerAbstractRequest::FinishedState);
}

void AsyncWorker::handleItemRemoveRequest(QOrganizerItemRemoveRequest *req)
{
    QOrganizerManager::Error err = QOrganizerManager::NoError;
    QMap<int, QOrganizerManager::Error> errorMap;
    m_manager->m_engine->removeItems(req->itemIds(), &errorMap, &err);
    QOrganizerManagerEngine::updateItemRemoveRequest(req, err, errorMap, QOrganizerAbstractRequest::FinishedState);
}

void AsyncWorker::handleSaveRequest(QOrganizerItemSaveRequest *req)
{
    QOrganizerManager::Error err = QOrganizerManager::NoError;
    QMap<int, QOrganizerManager::Error> errorMap;
    QList<QOrganizerItem> items = req->items();
    m_manager->m_engine->saveItems(&items, &errorMap, &err);
    QOrganizerManagerEngine::updateItemSaveRequest(req, items, err, errorMap, QOrganizerAbstractRequest::FinishedState);
}

void AsyncWorker::handleDefinitionFetchRequest(QOrganizerItemDetailDefinitionFetchRequest *req)
{
    QOrganizerManager::Error err = QOrganizerManager::NoError;
    QMap<QString, QOrganizerItemDetailDefinition> definitions = m_manager->m_engine->detailDefinitions(req->itemType(), &err);
    QMap<QString, QOrganizerItemDetailDefinition> retn;
    QMap<int, QOrganizerManager::Error> errorMap;
    QStringList keys = req->definitionNames();
    if (keys.isEmpty())
        retn = definitions;
    else {
        int definitionsCount = keys.count();
        for (int i = 0; i < definitionsCount; ++i) {
            if (definitions.contains(keys.at(i)))
                retn.insert(keys.at(i), definitions[keys.at(i)]);
            else
                errorMap.insert(i, QOrganizerManager::DoesNotExistError);
        }
    }
    QOrganizerManagerEngine::updateDefinitionFetchRequest(req, retn, err, errorMap, QOrganizerAbstractRequest::FinishedState);
}

void AsyncWorker::handleDefinitionRemoveRequest(QOrganizerItemDetailDefinitionRemoveRequest *req)
{
    QOrganizerManager::Error operationError = QOrganizerManager::NoError;
    QMap<int, QOrganizerManager::Error> errorMap;
    QStringList definitionNames = req->definitionNames();
    int nameCount = definitionNames.count();
    for(int i = 0; i < nameCount; ++i) {
        QOrganizerManager::Error tempError = QOrganizerManager::NoError;
        m_manager->m_engine->removeDetailDefinition(definitionNames.at(i), req->itemType(), &tempError);
        if (tempError != QOrganizerManager::NoError) {
            errorMap.insert(i, tempError);
            operationError = tempError;
        }
    }
    QOrganizerManagerEngine::updateDefinitionRemoveRequest(req, operationError, errorMap, QOrganizerAbstractRequest::FinishedState);
}

void AsyncWorker::handleDefinitionSaveRequest(QOrganizerItemDetailDefinitionSaveRequest *req)
{
    QOrganizerManager::Error operationError = QOrganizerManager::NoError;
    QMap<int, QOrganizerManager::Error> errorMap;
    QList<QOrganizerItemDetailDefinition> definitions = req->definitions();
    int definitionCount = definitions.count();
    for (int i = 0; i < definitionCount; ++i) {
        QOrganizerManager::Error tempError = QOrganizerManager::NoError;
        m_manager->m_engine->saveDetailDefinition(definitions.at(i), req->itemType(), &tempError);
        if (tempError != QOrganizerManager::NoError) {
            errorMap.insert(i, tempError);
            operationError = tempError;
        }
    }
    QOrganizerManagerEngine::updateDefinitionSaveRequest(req, definitions, operationError, errorMap, QOrganizerAbstractRequest::FinishedState);
}

void AsyncWorker::handleCollectionFetchRequest(QOrganizerCollectionFetchRequest *req)
{
    QOrganizerManager::Error err = QOrganizerManager::NoError;
    QList<QOrganizerCollection> collections = m_manager->m_engine->collections(&err);
    QOrganizerManagerEngine::updateCollectionFetchRequest(req, collections, err, QOrganizerAbstractRequest::FinishedState);
}

void AsyncWorker::handleCollectionRemoveRequest(QOrganizerCollectionRemoveRequest *req)
{
    QOrganizerManager::Error tempError = QOrganizerManager::NoError;
    QOrganizerManager::Error operationError = QOrganizerManager::NoError;
    QMap<int, QOrganizerManager::Error> errorMap;
    const QList<QOrganizerCollectionId>& colsToRemove(req->collectionIds());
    int collectionsCount = colsToRemove.count();
    for (int i = 0; i < collectionsCount; ++i) {
        const QOrganizerCollectionId& currentId(colsToRemove.at(i));
        m_manager->m_engine->removeCollection(currentId, &tempError);
        if (tempError != QOrganizerManager::NoError) {
            errorMap.insert(i, tempError);
            operationError = tempError;
        }
    }
    QOrganizerManagerEngine::updateCollectionRemoveRequest(req, operationError, errorMap, QOrganizerAbstractRequest::FinishedState);
}

void AsyncWorker::handleCollectionSaveRequest(QOrganizerCollectionSaveRequest *req)
{
    QOrganizerManager::Error tempError = QOrganizerManager::NoError;
    QOrganizerManager::Error operationError = QOrganizerManager::NoError;
    QMap<int, QOrganizerManager::Error> errorMap;
    QList<QOrganizerCollection> collections(req->collections());
    QList<QOrganizerCollection> retn;
    int collectionsCount = collections.count();
    for (int i = 0; i < collectionsCount; ++i) {
        QOrganizerCollection collection(collections.at(i));
        m_manager->m_engine->saveCollection(&collection, &tempError);
        retn << collection;
        if (tempError != QOrganizerManager::NoError) {
            errorMap.insert(i, tempError);
            operationError = tempError;
        }
    }
    QOrganizerManagerEngine::updateCollectionSaveRequest(req, retn, operationError, errorMap, QOrganizerAbstractRequest::FinishedState);
}

OrganizerAsynchManager::OrganizerAsynchManager(QOrganizerManagerEngine* engine, int maxWorkers)
    : m_engine(engine), m_maxWorkers(maxWorkers), m_destroying(false)
{
}

OrganizerAsynchManager::~OrganizerAsynchManager()
{
    QMutexLocker locker(&m_mutex);

    m_destroying = true;
    while (!m_activeWorkers.isEmpty())
        m_destroyWait.wait(&m_mutex);

    //delete all workers
    foreach(AsyncWorker* worker, m_idleWorkers) {
        delete worker;
    }
}

void OrganizerAsynchManager::requestDestroyed(QOrganizerAbstractRequest *req)
{
    //request was deleted somewhere
    m_mutex.lock();

    //if is in the queued requests it means it has not been processed yet, so remove it
    if (m_queuedRequests.contains(req)) {
        m_queuedRequests.removeOne(req);
        m_mutex.unlock();
    } else {
        //wait until the request is processed
        m_mutex.unlock();
        waitForRequestFinished(req);
    }
}

bool OrganizerAsynchManager::startRequest(QOrganizerAbstractRequest *req)
{
    QMutexLocker locker(&m_mutex);

    AsyncWorker *worker = 0;

    //first check if there is a lazy, idle worker around
    if (m_idleWorkers.size() > 0) {
        worker = m_idleWorkers.dequeue();
    } else
        //if the active workers number is less then maxWorkers create an additional worker
        if (m_activeWorkers.size() < m_maxWorkers) {
            worker = new AsyncWorker(this);
        }

    //if we found a worker assign him the request
    if (worker) {
        m_activeWorkers.enqueue(worker);
        m_activeRequests.insert(req);
        worker->assignRequest(req);
    } else {
        //no free worker is available, enqueue the request
        m_queuedRequests.enqueue(req);
    }

    locker.unlock();

    //update the request state to active
    QOrganizerManagerEngine::updateRequestState(req, QOrganizerAbstractRequest::ActiveState);

    return true;
}

bool OrganizerAsynchManager::cancelRequest(QOrganizerAbstractRequest *req)
{
    QMutexLocker locker(&m_mutex);

    //if the request is still in the queue cancel and remove it
    if (m_queuedRequests.contains(req)) {
        m_queuedRequests.removeOne(req);
        locker.unlock();
        QOrganizerManagerEngine::updateRequestState(req, QOrganizerAbstractRequest::CanceledState);

        return true;
    }

    // cannot cancel request when processing has already begun
    return false;
}

bool OrganizerAsynchManager::waitForRequestFinished(QOrganizerAbstractRequest *req, int msecs)
{
    QOrganizerAbstractRequest::State state = req->state();

    //check the request state, and if is not active return
    if (state == QOrganizerAbstractRequest::FinishedState) {
        return true;
    }
    else if (state == QOrganizerAbstractRequest::CanceledState
             || state == QOrganizerAbstractRequest::InactiveState) {
        return false;
    }

    {
        QMutexLocker locker(&m_mutex);

        //check if this request is known to the manager
        if (!m_activeRequests.contains(req) && !m_queuedRequests.contains(req)) {
            qWarning() << "Request was never queued: " << req;
            return false;
        }
    }

    QMutexLocker locker(&m_mutexMap);

    //create or fetch an existing wait condition for this requests from
    //the map to be able to block this thread until the condition is signaled
    QSharedPointer<QWaitCondition> cond(m_waitMap.value(req));
    if (!cond) {
        cond = QSharedPointer<QWaitCondition>(new QWaitCondition);
        m_waitMap.insert(req, cond);
    }

    //wait for the request to be finished or timeout
    bool res = cond->wait(&m_mutexMap, msecs <= 0 ? ULONG_MAX : msecs);

    m_waitMap.remove(req);

    return res;
}

void OrganizerAsynchManager::workerDone(AsyncWorker *worker, QOrganizerAbstractRequest *req)
{
    if (req) {
        {
            QMutexLocker locker(&m_mutexMap);

            //if somebody is waiting for this request wake him up
            QSharedPointer<QWaitCondition> cond(m_waitMap.value(req));
            if (cond) {
                cond->wakeAll();
            }
        }

        QMutexLocker locker(&m_mutex);

        m_activeRequests.remove(req);

        //check if there is more job for this worker
        if (!m_destroying && m_queuedRequests.count() > 0) {
            req = m_queuedRequests.dequeue();
            m_activeRequests.insert(req);
            worker->assignRequest(req);
        } else {
            //remove from the active workers list and add it to the idle one
            m_activeWorkers.removeOne(worker);
            m_idleWorkers.enqueue(worker);

            if (m_destroying)
                m_destroyWait.wakeAll();
        }
    }

}
