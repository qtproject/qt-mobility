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

OrganizerAsynchProcess::OrganizerAsynchProcess()
{
}

OrganizerAsynchProcess::~OrganizerAsynchProcess()
{
}

void OrganizerAsynchProcess::requestDestroyed(QOrganizerItemAbstractRequest *req)
{
    bool requestRemoved = false;
    m_mainMutex.lock();
    if (req->state() != QOrganizerItemAbstractRequest::ActiveState) {
        m_requestQueue.removeOne(req);
        requestRemoved = true;
    }
    m_mainMutex.unlock();

    if (!requestRemoved)
        waitForRequestFinished(req);
}

bool OrganizerAsynchProcess::startRequest(QOrganizerItemAbstractRequest *req)
{
    QOrganizerItemManagerEngine::updateRequestState(req, QOrganizerItemAbstractRequest::InactiveState);
    m_requestQueue.enqueue(req);
    processRequest();
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
    if (req->state() != QOrganizerItemAbstractRequest::ActiveState)
        return true;

    OrganizerRequestTimeoutTimer* newTimer = new OrganizerRequestTimeoutTimer(req, msecs);
    connect(newTimer, SIGNAL(timeout(OrganizerRequestTimeoutTimer*)), this, SLOT(proceed(OrganizerRequestTimeoutTimer*)));
    m_timers << newTimer;
    return waitForRequestFinished(req);
}

bool OrganizerAsynchProcess::waitForRequestFinished(QOrganizerItemAbstractRequest *req)
{
    m_activeRequests.insert(req);
    connect(req, SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)),
            this, SLOT(proceed(QOrganizerItemAbstractRequest::State)));

    do {
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);
    } while(m_activeRequests.contains(req) && req->state() == QOrganizerItemAbstractRequest::ActiveState);

    disconnect(req, SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)),
            this, SLOT(proceed(QOrganizerItemAbstractRequest::State)));

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

void OrganizerAsynchProcess::proceed(OrganizerRequestTimeoutTimer *timer)
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
        handleItemInstanceFetchRequest(req);
        break;
    case QOrganizerItemAbstractRequest::ItemFetchRequest:
        handleItemFetchRequest(req);
        break;
    case QOrganizerItemAbstractRequest::ItemLocalIdFetchRequest:
        handleLocalIdFetchRequest(req);
        break;
    case QOrganizerItemAbstractRequest::ItemRemoveRequest:
        handleItemRemoveRequest(req);
        break;
    case QOrganizerItemAbstractRequest::ItemSaveRequest:
        handleSaveRequest(req);
        break;
    case QOrganizerItemAbstractRequest::DetailDefinitionFetchRequest:
        handleDefinitionFetchRequest(req);
        break;
    case QOrganizerItemAbstractRequest::DetailDefinitionRemoveRequest:
        handleDefinitionRemoveRequest(req);
        break;
    case QOrganizerItemAbstractRequest::DetailDefinitionSaveRequest:
        handleDefinitionSaveRequest(req);
        break;
    default:
        // invalid request
        break;
    }

    m_mainMutex.unlock();
}

void OrganizerAsynchProcess::handleItemInstanceFetchRequest(QOrganizerItemAbstractRequest *req)
{
    Q_UNUSED(req);
}

void OrganizerAsynchProcess::handleItemFetchRequest(QOrganizerItemAbstractRequest *req)
{
    Q_UNUSED(req);
}

void OrganizerAsynchProcess::handleLocalIdFetchRequest(QOrganizerItemAbstractRequest *req)
{
    Q_UNUSED(req);
}

void OrganizerAsynchProcess::handleItemRemoveRequest(QOrganizerItemAbstractRequest *req)
{
    Q_UNUSED(req);
}

void OrganizerAsynchProcess::handleSaveRequest(QOrganizerItemAbstractRequest *req)
{
    Q_UNUSED(req);
}

void OrganizerAsynchProcess::handleDefinitionFetchRequest(QOrganizerItemAbstractRequest *req)
{
    Q_UNUSED(req);
}

void OrganizerAsynchProcess::handleDefinitionRemoveRequest(QOrganizerItemAbstractRequest *req)
{
    Q_UNUSED(req);
}

void OrganizerAsynchProcess::handleDefinitionSaveRequest(QOrganizerItemAbstractRequest *req)
{
    Q_UNUSED(req);
}
