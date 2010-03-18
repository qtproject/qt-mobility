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
#include <QDebug>

CntSimContactRemoveRequest::CntSimContactRemoveRequest(CntSymbianSimEngine *engine, QContactRemoveRequest *req)
    :CntAbstractSimRequest(engine, req)
{
    connect( simStore(), SIGNAL(removeComplete(QContactManager::Error)),
        this, SLOT(removeComplete(QContactManager::Error)), Qt::QueuedConnection );
    
    connect( simStore(), SIGNAL(getReservedSlotsComplete(QList<int>, QContactManager::Error)),
        this, SLOT(getReservedSlotsComplete(QList<int>, QContactManager::Error)), Qt::QueuedConnection );
}

CntSimContactRemoveRequest::~CntSimContactRemoveRequest()
{
    cancel();
}

void CntSimContactRemoveRequest::run()
{
    QContactRemoveRequest *r = req<QContactRemoveRequest>();
    
    if (!r->isActive())
        return;
       
    m_contactIds = r->contactIds();
    m_errorMap.clear();
    m_index = 0;
#ifdef SYMBIANSIM_BACKEND_CHECK_BEFORE_REMOVE
    m_reservedSlots.clear();
    getReservedSlots();    
#else
    removeNext();
#endif
}

void CntSimContactRemoveRequest::removeComplete(QContactManager::Error error)
{
    if (!req()->isActive())
        return;
    
    if (error)
        m_errorMap.insert(m_index, error);
    
    m_index++;
    singleShotTimer(KRequestDelay, this, SLOT(removeNext()));
}

void CntSimContactRemoveRequest::removeNext()
{
    QContactRemoveRequest *r = req<QContactRemoveRequest>();
    
    if (!r->isActive())
        return;
    
    if (r->contactIds().count() == 0) {
        QContactManagerEngine::updateContactRemoveRequest(r, QContactManager::BadArgumentError, m_errorMap, QContactAbstractRequest::FinishedState);
        return;
    }        
    
    // All contacts removed?
    if (m_index >= m_contactIds.count())
    {
        // Take first error from errormap (if any)
        QContactManager::Error error = QContactManager::NoError;
        if (m_errorMap.count())
            error = m_errorMap.begin().value();

        QContactManagerEngine::updateContactRemoveRequest(r, error, m_errorMap, QContactAbstractRequest::FinishedState);
        return;
    }

    // Remove next contact
    QContactLocalId contactId = m_contactIds.at(m_index);
    QContactManager::Error error = QContactManager::NoError;
    
#ifdef SYMBIANSIM_BACKEND_CHECK_BEFORE_REMOVE
    if (m_reservedSlots.contains(contactId))
        simStore()->remove(contactId, error);
    else
        error = QContactManager::DoesNotExistError;
#else
    simStore()->remove(contactId, error);
#endif

    if (error) {
        m_errorMap.insert(m_index, error);
        m_index++;
        singleShotTimer(KRequestDelay, this, SLOT(removeNext()));
    }
}

void CntSimContactRemoveRequest::getReservedSlotsComplete(QList<int> reservedSlots, QContactManager::Error error)
{
    QContactRemoveRequest *r = req<QContactRemoveRequest>();
    
    if (!r->isActive())
        return;
    
    if (error != QContactManager::NoError && error != QContactManager::DoesNotExistError) {
        QContactManagerEngine::updateContactRemoveRequest(r, error, m_errorMap, QContactAbstractRequest::FinishedState);
        return;
    }

    m_reservedSlots = reservedSlots;
    singleShotTimer(KRequestDelay, this, SLOT(removeNext()));
}

void CntSimContactRemoveRequest::getReservedSlots()
{
    QContactRemoveRequest *r = req<QContactRemoveRequest>();
    
    if (!r->isActive())
        return;
    
    QContactManager::Error error = QContactManager::NoError;
    if (!simStore()->getReservedSlots(error)) {
        QContactManagerEngine::updateContactRemoveRequest(r, error, m_errorMap, QContactAbstractRequest::FinishedState);
    }
}
