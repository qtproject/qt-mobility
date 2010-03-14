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

#include "cntsimcontactremoverequest.h"
#include "cntsymbiansimengine.h"
#include "cntsimstore.h"
#include <qcontactremoverequest.h>
#include <QTimer>

CntSimContactRemoveRequest::CntSimContactRemoveRequest(CntSymbianSimEngine *engine, QContactRemoveRequest *req)
    :CntAbstractSimRequest(engine),
     m_req(req)
{
    connect( simStore(), SIGNAL(removeComplete(QContactManager::Error)),
        this, SLOT(removeComplete(QContactManager::Error)), Qt::QueuedConnection );
}

CntSimContactRemoveRequest::~CntSimContactRemoveRequest()
{
    cancel();
}

bool CntSimContactRemoveRequest::start()
{    
    if (m_req->isActive())
        return false;
    
    if (simStore()->isBusy())
        return false;
    
    m_contactIds = m_req->contactIds();
    m_errorMap.clear();
    m_index = 0;
    singleShotTimer(0, this, SLOT(removeNext()));
    
    QContactManagerEngine::updateRequestState(m_req, QContactAbstractRequest::ActiveState);
    return true; 
}

bool CntSimContactRemoveRequest::cancel()
{
    if (m_req->isActive()) {
        cancelTimer();
        simStore()->cancel();
        QContactManagerEngine::updateRequestState(m_req, QContactAbstractRequest::CanceledState);
        return true;
    }
    return false;
}

void CntSimContactRemoveRequest::removeComplete(QContactManager::Error error)
{
    if (error)
        m_errorMap.insert(m_index, error);
    m_index++;
    removeNext();
}

void CntSimContactRemoveRequest::removeNext()
{
    if (m_req->isCanceled())
        return;
    
    // All contacts removed?
    if (m_index >= m_contactIds.count())
    {
        // Take first error from errormap (if any)
        QContactManager::Error error = QContactManager::NoError;
        if (m_errorMap.count())
            error = m_errorMap.begin().value();

        QContactManagerEngine::updateRequestState(m_req, QContactAbstractRequest::FinishedState);
        QContactManagerEngine::updateContactRemoveRequest(m_req, error, m_errorMap);
        return;
    }

    // Remove next contact
    QContactLocalId contactId = m_contactIds.at(m_index);
    QContactManager::Error error = QContactManager::NoError;
    if (!simStore()->remove(contactId, error)) {
        m_errorMap.insert(m_index, error);
        m_index++;
        singleShotTimer(0, this, SLOT(removeNext()));
    }
}
