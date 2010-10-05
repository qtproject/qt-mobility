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

#include "qorganizerasynchprocess.h"
#include "qtorganizer.h"
#include "qorganizermaemo5_p.h"
#include "qorganizermaemo5ids_p.h"
#include <QtCore/qtimer.h>
#include <QtCore/qcoreapplication.h>

QTM_USE_NAMESPACE

OrganizerRequestTimeoutTimer::OrganizerRequestTimeoutTimer(QOrganizerItemAbstractRequest *req, int msecs)
    : m_req(req)
{
    m_timeoutTimer = new QTimer();
    connect(m_timeoutTimer, SIGNAL(timeout()), this, SLOT(internalTimeout()));
    m_timeoutTimer->setSingleShot(true);
    m_timeoutTimer->start(msecs);
}

OrganizerRequestTimeoutTimer::~OrganizerRequestTimeoutTimer()
{
    delete m_timeoutTimer;
}

QOrganizerItemAbstractRequest* OrganizerRequestTimeoutTimer::request() const
{
    return m_req;
}

void OrganizerRequestTimeoutTimer::internalTimeout()
{
    emit timeout(this);
}

OrganizerAsynchProcess::OrganizerAsynchProcess(QOrganizerItemMaemo5Engine* engine)
    : m_engine(engine), m_quitNow(false)
{
    start();
    QObject::moveToThread(this);
}

void OrganizerAsynchProcess::run()
{
    while(!m_quitNow) {
        yieldCurrentThread();
        processRequest();
    }

    // Inform that the thread quits now
    m_quitNow = false;
}

OrganizerAsynchProcess::~OrganizerAsynchProcess()
{
    // Tell thread to quit
    m_quitNow = true;

    // Wait for run() to exit
    while(m_quitNow)
        yieldCurrentThread();
}

void OrganizerAsynchProcess::requestDestroyed(QOrganizerItemAbstractRequest *req)
{
    bool requestRemoved = false;

    m_mainMutex.lock();
    if (m_requestQueue.contains(req)) {
        m_requestQueue.removeOne(req);
        requestRemoved = true;
    }
    m_mainMutex.unlock();

    if (!requestRemoved && req->state() != QOrganizerItemAbstractRequest::FinishedState)
        waitForRequestFinished(req);
}

bool OrganizerAsynchProcess::addRequest(QOrganizerItemAbstractRequest *req)
{
    m_mainMutex.lock();
    m_requestQueue.enqueue(req);
    m_mainMutex.unlock();
    QOrganizerItemManagerEngine::updateRequestState(req, QOrganizerItemAbstractRequest::ActiveState);
    return true;
}

bool OrganizerAsynchProcess::cancelRequest(QOrganizerItemAbstractRequest *req)
{
    m_mainMutex.lock();

    if (m_requestQueue.contains(req)) {
        QOrganizerItemManagerEngine::updateRequestState(req, QOrganizerItemAbstractRequest::CanceledState);
        m_requestQueue.removeOne(req);
        m_mainMutex.unlock();
        return true;
    }
    else {
        // cannot cancel request when processing has already begun
        m_mainMutex.unlock();
        return false;
    }
}

bool OrganizerAsynchProcess::waitForRequestFinished(QOrganizerItemAbstractRequest *req, int msecs)
{
    if (req->state() == QOrganizerItemAbstractRequest::FinishedState) {
        return true;
    }
    else if (req->state() == QOrganizerItemAbstractRequest::CanceledState
             || req->state() == req->state() == QOrganizerItemAbstractRequest::InactiveState) {
        return false;
    }

    // Multiple timers are created to make this method thread safe.
    // There's a timer for each calling thread.
    OrganizerRequestTimeoutTimer* newTimer = new OrganizerRequestTimeoutTimer(req, msecs);
    connect(newTimer, SIGNAL(timeout(OrganizerRequestTimeoutTimer*)), this, SLOT(timeout(OrganizerRequestTimeoutTimer*)));
    m_timers << newTimer;
    return waitForRequestFinished(req);
}

bool OrganizerAsynchProcess::waitForRequestFinished(QOrganizerItemAbstractRequest *req)
{
    m_activeRequests.insert(req);

    do {
        yieldCurrentThread();
        // Process events to allow the timeout timers to work
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);
    } while(m_activeRequests.contains(req) && req->state() == QOrganizerItemAbstractRequest::ActiveState);

    m_timeoutMutex.lock();
    if (!m_activeRequests.contains(req)) {
        // timeout occured
        m_timeoutMutex.unlock();
        return false;
    }
    else {
        // timeout not occured
        m_activeRequests.remove(req);

        // cancel and remove timer
        QList<OrganizerRequestTimeoutTimer *>::iterator timer;
        for (timer = m_timers.begin(); timer != m_timers.end(); ++timer)
            if ((*timer)->request() == req)
                break;
        if (timer != m_timers.end()) {
            m_timers.removeOne(*timer);
            delete *timer;
        }
        m_timeoutMutex.unlock();
        return true;
    }
}

void OrganizerAsynchProcess::timeout(OrganizerRequestTimeoutTimer *timer)
{
    m_timeoutMutex.lock();
    if (m_activeRequests.contains(timer->request())) {
        m_activeRequests.remove(timer->request());
        m_timers.removeOne(timer);
        delete timer;
    }
    m_timeoutMutex.unlock();
}

void OrganizerAsynchProcess::processRequest()
{
    m_mainMutex.lock();

    if (m_requestQueue.isEmpty()) {
        m_mainMutex.unlock();
        return;
    }

    QOrganizerItemAbstractRequest *req = m_requestQueue.dequeue();
    if (req->state() != QOrganizerItemAbstractRequest::ActiveState) {
        m_mainMutex.unlock();
        return;
    }

    switch(req->type()) {
    case QOrganizerItemAbstractRequest::ItemInstanceFetchRequest:
        handleItemInstanceFetchRequest(static_cast<QOrganizerItemInstanceFetchRequest *>(req));
        break;
    case QOrganizerItemAbstractRequest::ItemFetchRequest:
        handleItemFetchRequest(static_cast<QOrganizerItemFetchRequest *>(req));
        break;
    case QOrganizerItemAbstractRequest::ItemLocalIdFetchRequest:
        handleLocalIdFetchRequest(static_cast<QOrganizerItemLocalIdFetchRequest *>(req));
        break;
    case QOrganizerItemAbstractRequest::ItemRemoveRequest:
        handleItemRemoveRequest(static_cast<QOrganizerItemRemoveRequest *>(req));
        break;
    case QOrganizerItemAbstractRequest::ItemSaveRequest:
        handleSaveRequest(static_cast<QOrganizerItemSaveRequest *>(req));
        break;
    case QOrganizerItemAbstractRequest::DetailDefinitionFetchRequest:
        handleDefinitionFetchRequest(static_cast<QOrganizerItemDetailDefinitionFetchRequest *>(req));
        break;
    case QOrganizerItemAbstractRequest::DetailDefinitionRemoveRequest:
        handleDefinitionRemoveRequest(static_cast<QOrganizerItemDetailDefinitionRemoveRequest *>(req));
        break;
    case QOrganizerItemAbstractRequest::DetailDefinitionSaveRequest:
        handleDefinitionSaveRequest(static_cast<QOrganizerItemDetailDefinitionSaveRequest *>(req));
        break;
    case QOrganizerItemAbstractRequest::CollectionFetchRequest:
        handleCollectionFetchRequest(static_cast<QOrganizerCollectionFetchRequest *>(req));
        break;
    case QOrganizerItemAbstractRequest::CollectionLocalIdFetchRequest:
        handleCollectionLocalIdFetchRequest(static_cast<QOrganizerCollectionLocalIdFetchRequest *>(req));
        break;
    case QOrganizerItemAbstractRequest::CollectionRemoveRequest:
        handleCollectionRemoveRequest(static_cast<QOrganizerCollectionRemoveRequest *>(req));
        break;
    case QOrganizerItemAbstractRequest::CollectionSaveRequest:
        handleCollectionSaveRequest(static_cast<QOrganizerCollectionSaveRequest *>(req));
        break;

    default:
        // invalid request
        break;
    }

    m_mainMutex.unlock();
}

void OrganizerAsynchProcess::handleItemInstanceFetchRequest(QOrganizerItemInstanceFetchRequest *req)
{
    QOrganizerItemManager::Error err = QOrganizerItemManager::NoError;
    QList<QOrganizerItem> items = m_engine->itemInstances(req->filter(), req->sorting(), req->fetchHint(), &err);
    QOrganizerItemManagerEngine::updateItemInstanceFetchRequest(req, items, err, QOrganizerItemAbstractRequest::FinishedState);
}

void OrganizerAsynchProcess::handleItemFetchRequest(QOrganizerItemFetchRequest *req)
{
    QOrganizerItemManager::Error err = QOrganizerItemManager::NoError;
    QList<QOrganizerItem> items = m_engine->items(req->filter(), req->sorting(), req->fetchHint(), &err);
    QOrganizerItemManagerEngine::updateItemFetchRequest(req, items, err, QOrganizerItemAbstractRequest::FinishedState);
}

void OrganizerAsynchProcess::handleLocalIdFetchRequest(QOrganizerItemLocalIdFetchRequest *req)
{
    QOrganizerItemManager::Error err = QOrganizerItemManager::NoError;
    QList<QOrganizerItemLocalId> ids = m_engine->itemIds(req->filter(), req->sorting(), &err);
    QOrganizerItemManagerEngine::updateItemLocalIdFetchRequest(req, ids, err, QOrganizerItemAbstractRequest::FinishedState);
}

void OrganizerAsynchProcess::handleItemRemoveRequest(QOrganizerItemRemoveRequest *req)
{
    QOrganizerItemManager::Error err = QOrganizerItemManager::NoError;
    QMap<int, QOrganizerItemManager::Error> errorMap;
    m_engine->removeItems(req->itemIds(), &errorMap, &err);
    QOrganizerItemManagerEngine::updateItemRemoveRequest(req, err, errorMap, QOrganizerItemAbstractRequest::FinishedState);
}

void OrganizerAsynchProcess::handleSaveRequest(QOrganizerItemSaveRequest *req)
{
    QOrganizerItemManager::Error err = QOrganizerItemManager::NoError;
    QMap<int, QOrganizerItemManager::Error> errorMap;
    QList<QOrganizerItem> items = req->items();
    m_engine->saveItems(&items, req->collectionId(), &errorMap, &err);
    QOrganizerItemManagerEngine::updateItemSaveRequest(req, items, err, errorMap, QOrganizerItemAbstractRequest::FinishedState);
}

void OrganizerAsynchProcess::handleDefinitionFetchRequest(QOrganizerItemDetailDefinitionFetchRequest *req)
{
    QOrganizerItemManager::Error err = QOrganizerItemManager::NoError;
    QMap<QString, QOrganizerItemDetailDefinition> definitions = m_engine->detailDefinitions(req->itemType(), &err);
    QMap<int, QOrganizerItemManager::Error> errorMap;
    QStringList keys = req->definitionNames();
    int definitionsCount = keys.count();
    for (int i = 0; i < definitionsCount; ++i) {
        QOrganizerItemDetailDefinition definition = m_engine->detailDefinition(keys.at(i), req->itemType(), &err);
        definitions.insert(keys.at(i), definition);
        if (err != QOrganizerItemManager::NoError)
            errorMap.insert(i, err);
    }
    QOrganizerItemManagerEngine::updateDefinitionFetchRequest(req, definitions, err, errorMap, QOrganizerItemAbstractRequest::FinishedState);
}

void OrganizerAsynchProcess::handleDefinitionRemoveRequest(QOrganizerItemDetailDefinitionRemoveRequest *req)
{
    QOrganizerItemManager::Error err = QOrganizerItemManager::NoError;
    QMap<int, QOrganizerItemManager::Error> errorMap;
    QStringList definitionNames = req->definitionNames();
    int nameCount = definitionNames.count();
    for(int i = 0; i < nameCount; ++i) {
        m_engine->removeDetailDefinition(definitionNames.at(i), req->itemType(), &err);
        errorMap.insert(i, err);
    }
    QOrganizerItemManagerEngine::updateDefinitionRemoveRequest(req, err, errorMap, QOrganizerItemAbstractRequest::FinishedState);
}

void OrganizerAsynchProcess::handleDefinitionSaveRequest(QOrganizerItemDetailDefinitionSaveRequest *req)
{
    QOrganizerItemManager::Error err = QOrganizerItemManager::NoError;
    QMap<int, QOrganizerItemManager::Error> errorMap;
    QList<QOrganizerItemDetailDefinition> definitions = req->definitions();
    int definitionCount = definitions.count();
    for (int i = 0; i < definitionCount; ++i) {
        m_engine->saveDetailDefinition(definitions.at(i), req->itemType(), &err);
        errorMap.insert(i, err);
    }
    QOrganizerItemManagerEngine::updateDefinitionSaveRequest(req, definitions, err, errorMap, QOrganizerItemAbstractRequest::FinishedState);
}

void OrganizerAsynchProcess::handleCollectionFetchRequest(QOrganizerCollectionFetchRequest *req)
{
    QOrganizerItemManager::Error err = QOrganizerItemManager::NoError;
    QMap<int, QOrganizerItemManager::Error> errorMap;
    QList<QOrganizerCollection> collections = m_engine->collections(req->collectionIds(), &errorMap, &err);
    QOrganizerItemManagerEngine::updateCollectionFetchRequest(req, collections, err, errorMap, QOrganizerItemAbstractRequest::FinishedState);
}

void OrganizerAsynchProcess::handleCollectionLocalIdFetchRequest(QOrganizerCollectionLocalIdFetchRequest *req)
{
    QOrganizerItemManager::Error err = QOrganizerItemManager::NoError;
    QList<QOrganizerCollectionLocalId> collectionIds = m_engine->collectionIds(&err);
    QOrganizerItemManagerEngine::updateCollectionLocalIdFetchRequest(req, collectionIds, err, QOrganizerItemAbstractRequest::FinishedState);
}

void OrganizerAsynchProcess::handleCollectionRemoveRequest(QOrganizerCollectionRemoveRequest *req)
{
    QOrganizerItemManager::Error err = QOrganizerItemManager::NoError;
    QMap<int, QOrganizerItemManager::Error> errorMap;
    int i = 0;
    foreach (QOrganizerCollectionLocalId id, req->collectionIds()) {
        m_engine->removeCollection(id, &err);
        errorMap.insert(i, err);
        i++;
    }
    QOrganizerItemManagerEngine::updateCollectionRemoveRequest(req, err, errorMap, QOrganizerItemAbstractRequest::FinishedState);
}

void OrganizerAsynchProcess::handleCollectionSaveRequest(QOrganizerCollectionSaveRequest *req)
{
    QOrganizerItemManager::Error err = QOrganizerItemManager::NoError;
    QMap<int, QOrganizerItemManager::Error> errorMap;
    QList<QOrganizerCollection> collections = req->collections();
    QList<QOrganizerCollection> retn;
    int collectionsCount = collections.count();
    for (int i = 0; i < collectionsCount; ++i) {
        QOrganizerCollection collection = collections.at(i);
        m_engine->saveCollection(&collection, &err);
        retn << collection;
        errorMap.insert(i, err);
    }
    QOrganizerItemManagerEngine::updateCollectionSaveRequest(req, retn, err, errorMap, QOrganizerItemAbstractRequest::FinishedState);
}
