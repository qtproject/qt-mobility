/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
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

OrganizerRequestTimeoutTimer::OrganizerRequestTimeoutTimer(QOrganizerAbstractRequest *req, int msecs)
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

QOrganizerAbstractRequest* OrganizerRequestTimeoutTimer::request() const
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

void OrganizerAsynchProcess::requestDestroyed(QOrganizerAbstractRequest *req)
{
    bool requestRemoved = false;

    m_mainMutex.lock();
    if (m_requestQueue.contains(req)) {
        m_requestQueue.removeOne(req);
        requestRemoved = true;
    }
    m_mainMutex.unlock();

    if (!requestRemoved && req->state() != QOrganizerAbstractRequest::FinishedState)
        waitForRequestFinished(req);
}

bool OrganizerAsynchProcess::addRequest(QOrganizerAbstractRequest *req)
{
    m_mainMutex.lock();
    m_requestQueue.enqueue(req);
    m_mainMutex.unlock();
    QOrganizerManagerEngine::updateRequestState(req, QOrganizerAbstractRequest::ActiveState);
    return true;
}

bool OrganizerAsynchProcess::cancelRequest(QOrganizerAbstractRequest *req)
{
    m_mainMutex.lock();

    if (m_requestQueue.contains(req)) {
        QOrganizerManagerEngine::updateRequestState(req, QOrganizerAbstractRequest::CanceledState);
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

bool OrganizerAsynchProcess::waitForRequestFinished(QOrganizerAbstractRequest *req, int msecs)
{
    if (req->state() == QOrganizerAbstractRequest::FinishedState) {
        return true;
    }
    else if (req->state() == QOrganizerAbstractRequest::CanceledState
             || req->state() == req->state() == QOrganizerAbstractRequest::InactiveState) {
        return false;
    }

    // Multiple timers are created to make this method thread safe.
    // There's a timer for each calling thread.
    OrganizerRequestTimeoutTimer* newTimer = new OrganizerRequestTimeoutTimer(req, msecs);
    connect(newTimer, SIGNAL(timeout(OrganizerRequestTimeoutTimer*)), this, SLOT(timeout(OrganizerRequestTimeoutTimer*)));
    m_timers << newTimer;
    return waitForRequestFinished(req);
}

bool OrganizerAsynchProcess::waitForRequestFinished(QOrganizerAbstractRequest *req)
{
    m_activeRequests.insert(req);

    do {
        yieldCurrentThread();
        // Process events to allow the timeout timers to work
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);
    } while(m_activeRequests.contains(req) && req->state() == QOrganizerAbstractRequest::ActiveState);

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

    QOrganizerAbstractRequest *req = m_requestQueue.dequeue();
    if (req->state() != QOrganizerAbstractRequest::ActiveState) {
        m_mainMutex.unlock();
        return;
    }

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

    m_mainMutex.unlock();
}

void OrganizerAsynchProcess::handleItemFetchRequest(QOrganizerItemFetchRequest *req)
{
    QOrganizerManager::Error err = QOrganizerManager::NoError;
    QList<QOrganizerItem> items = m_engine->items(req->startDate(), req->endDate(), req->filter(), req->sorting(), req->fetchHint(), &err);
    QOrganizerManagerEngine::updateItemFetchRequest(req, items, err, QOrganizerAbstractRequest::FinishedState);
}

void OrganizerAsynchProcess::handleItemFetchForExportRequest(QOrganizerItemFetchForExportRequest *req)
{
    QOrganizerManager::Error err = QOrganizerManager::NoError;
    QList<QOrganizerItem> items = m_engine->itemsForExport(req->startDate(), req->endDate(), req->filter(), req->sorting(), req->fetchHint(), &err);
    QOrganizerManagerEngine::updateItemFetchForExportRequest(req, items, err, QOrganizerAbstractRequest::FinishedState);
}

void OrganizerAsynchProcess::handleItemOccurrenceFetchRequest(QOrganizerItemOccurrenceFetchRequest *req)
{
    QOrganizerManager::Error err = QOrganizerManager::NoError;
    QList<QOrganizerItem> items = m_engine->itemOccurrences(req->parentItem(), req->startDate(), req->endDate(), req->maxOccurrences(), req->fetchHint(), &err);
    QOrganizerManagerEngine::updateItemOccurrenceFetchRequest(req, items, err, QOrganizerAbstractRequest::FinishedState);
}

void OrganizerAsynchProcess::handleIdFetchRequest(QOrganizerItemIdFetchRequest *req)
{
    QOrganizerManager::Error err = QOrganizerManager::NoError;
    QList<QOrganizerItemId> ids = m_engine->itemIds(req->startDate(), req->endDate(), req->filter(), req->sorting(), &err);
    QOrganizerManagerEngine::updateItemIdFetchRequest(req, ids, err, QOrganizerAbstractRequest::FinishedState);
}

void OrganizerAsynchProcess::handleItemRemoveRequest(QOrganizerItemRemoveRequest *req)
{
    QOrganizerManager::Error err = QOrganizerManager::NoError;
    QMap<int, QOrganizerManager::Error> errorMap;
    m_engine->removeItems(req->itemIds(), &errorMap, &err);
    QOrganizerManagerEngine::updateItemRemoveRequest(req, err, errorMap, QOrganizerAbstractRequest::FinishedState);
}

void OrganizerAsynchProcess::handleSaveRequest(QOrganizerItemSaveRequest *req)
{
    QOrganizerManager::Error err = QOrganizerManager::NoError;
    QMap<int, QOrganizerManager::Error> errorMap;
    QList<QOrganizerItem> items = req->items();
    m_engine->saveItems(&items, &errorMap, &err);
    QOrganizerManagerEngine::updateItemSaveRequest(req, items, err, errorMap, QOrganizerAbstractRequest::FinishedState);
}

void OrganizerAsynchProcess::handleDefinitionFetchRequest(QOrganizerItemDetailDefinitionFetchRequest *req)
{
    QOrganizerManager::Error err = QOrganizerManager::NoError;
    QMap<QString, QOrganizerItemDetailDefinition> definitions = m_engine->detailDefinitions(req->itemType(), &err);
    QMap<QString, QOrganizerItemDetailDefinition> retn;
    QMap<int, QOrganizerManager::Error> errorMap;
    QStringList keys = req->definitionNames();
    if (keys.isEmpty())
        keys = definitions.keys();
    int definitionsCount = keys.count();
    for (int i = 0; i < definitionsCount; ++i) {
        if (definitions.contains(keys.at(i)))
            retn.insert(keys.at(i), definitions[keys.at(i)]);
        else
            errorMap.insert(i, QOrganizerManager::DoesNotExistError);
    }
    QOrganizerManagerEngine::updateDefinitionFetchRequest(req, retn, err, errorMap, QOrganizerAbstractRequest::FinishedState);
}

void OrganizerAsynchProcess::handleDefinitionRemoveRequest(QOrganizerItemDetailDefinitionRemoveRequest *req)
{
    QOrganizerManager::Error err = QOrganizerManager::NoError;
    QMap<int, QOrganizerManager::Error> errorMap;
    QStringList definitionNames = req->definitionNames();
    int nameCount = definitionNames.count();
    for(int i = 0; i < nameCount; ++i) {
        m_engine->removeDetailDefinition(definitionNames.at(i), req->itemType(), &err);
        errorMap.insert(i, err);
    }
    QOrganizerManagerEngine::updateDefinitionRemoveRequest(req, err, errorMap, QOrganizerAbstractRequest::FinishedState);
}

void OrganizerAsynchProcess::handleDefinitionSaveRequest(QOrganizerItemDetailDefinitionSaveRequest *req)
{
    QOrganizerManager::Error err = QOrganizerManager::NoError;
    QMap<int, QOrganizerManager::Error> errorMap;
    QList<QOrganizerItemDetailDefinition> definitions = req->definitions();
    int definitionCount = definitions.count();
    for (int i = 0; i < definitionCount; ++i) {
        m_engine->saveDetailDefinition(definitions.at(i), req->itemType(), &err);
        errorMap.insert(i, err);
    }
    QOrganizerManagerEngine::updateDefinitionSaveRequest(req, definitions, err, errorMap, QOrganizerAbstractRequest::FinishedState);
}

void OrganizerAsynchProcess::handleCollectionFetchRequest(QOrganizerCollectionFetchRequest *req)
{
    QOrganizerManager::Error err = QOrganizerManager::NoError;
    QList<QOrganizerCollection> collections = m_engine->collections(&err);
    QOrganizerManagerEngine::updateCollectionFetchRequest(req, collections, err, QOrganizerAbstractRequest::FinishedState);
}

void OrganizerAsynchProcess::handleCollectionRemoveRequest(QOrganizerCollectionRemoveRequest *req)
{
    QOrganizerManager::Error err = QOrganizerManager::NoError;
    QMap<int, QOrganizerManager::Error> errorMap;
    int i = 0;
    foreach (QOrganizerCollectionId id, req->collectionIds()) {
        m_engine->removeCollection(id, &err);
        errorMap.insert(i, err);
        i++;
    }
    QOrganizerManagerEngine::updateCollectionRemoveRequest(req, err, errorMap, QOrganizerAbstractRequest::FinishedState);
}

void OrganizerAsynchProcess::handleCollectionSaveRequest(QOrganizerCollectionSaveRequest *req)
{
    QOrganizerManager::Error err = QOrganizerManager::NoError;
    QMap<int, QOrganizerManager::Error> errorMap;
    QList<QOrganizerCollection> collections = req->collections();
    QList<QOrganizerCollection> retn;
    int collectionsCount = collections.count();
    for (int i = 0; i < collectionsCount; ++i) {
        QOrganizerCollection collection = collections.at(i);
        m_engine->saveCollection(&collection, &err);
        retn << collection;
        errorMap.insert(i, err);
    }
    QOrganizerManagerEngine::updateCollectionSaveRequest(req, retn, err, errorMap, QOrganizerAbstractRequest::FinishedState);
}
