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
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qorganizerasynchprocess.h"
#include "qtorganizer.h"
#include "qorganizermaemo5_p.h"
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
    : m_engine(engine)
{
    QTimer::singleShot(0, this, SLOT(doStart()));
}

void OrganizerAsynchProcess::doStart()
{
    start();
    QObject::moveToThread(this);
}

OrganizerAsynchProcess::~OrganizerAsynchProcess()
{
    quit();
    wait();
}

void OrganizerAsynchProcess::requestDestroyed(QOrganizerItemAbstractRequest *req)
{
    bool requestRemoved = false;

    if (req->state() != QOrganizerItemAbstractRequest::ActiveState) {
        m_mainMutex.lock();
        m_requestQueue.removeOne(req);
        m_mainMutex.unlock();
        requestRemoved = true;
    }

    if (!requestRemoved)
        waitForRequestFinished(req);
}

bool OrganizerAsynchProcess::addRequest(QOrganizerItemAbstractRequest *req)
{
    QOrganizerItemManagerEngine::updateRequestState(req, QOrganizerItemAbstractRequest::InactiveState);
    m_requestQueue.enqueue(req);
    return true; // TODO: Is this ok?
}

bool OrganizerAsynchProcess::cancelRequest(QOrganizerItemAbstractRequest *req)
{
    m_mainMutex.lock();
    if (req->state() != QOrganizerItemAbstractRequest::ActiveState) {
        QOrganizerItemManagerEngine::updateRequestState(req, QOrganizerItemAbstractRequest::CanceledState);
        m_requestQueue.removeOne(req);
        m_mainMutex.unlock();
        return true;
    }
    else {
        // cannot cancel active requests
        m_mainMutex.unlock();
        return false;
    }
}

bool OrganizerAsynchProcess::waitForRequestFinished(QOrganizerItemAbstractRequest *req, int msecs)
{
    if (req->state() == QOrganizerItemAbstractRequest::FinishedState)
        return true;
    else if (req->state() == QOrganizerItemAbstractRequest::CanceledState)
        return false;

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
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);
    } while(m_activeRequests.contains(req)
            && (req->state() == QOrganizerItemAbstractRequest::InactiveState
             || req->state() == QOrganizerItemAbstractRequest::ActiveState));

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
        return false;
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
    if (!req->state() == QOrganizerItemAbstractRequest::InactiveState) {
        m_mainMutex.unlock();
        return;
    }

    QOrganizerItemManagerEngine::updateRequestState(req, QOrganizerItemAbstractRequest::ActiveState);

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
    QOrganizerCollectionLocalId collectionLocalId; // TODO: Make support for this
    m_engine->saveItems(&items, collectionLocalId, &errorMap, &err);
    QOrganizerItemManagerEngine::updateItemSaveRequest(req, items, err, errorMap, QOrganizerItemAbstractRequest::FinishedState);
}

void OrganizerAsynchProcess::handleDefinitionFetchRequest(QOrganizerItemDetailDefinitionFetchRequest *req)
{
    Q_UNUSED(req);
    // TODO
}

void OrganizerAsynchProcess::handleDefinitionRemoveRequest(QOrganizerItemDetailDefinitionRemoveRequest *req)
{
    Q_UNUSED(req);
    // TODO
}

void OrganizerAsynchProcess::handleDefinitionSaveRequest(QOrganizerItemDetailDefinitionSaveRequest *req)
{
    Q_UNUSED(req);
    // TODO
}
