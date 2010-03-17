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

#include "cntsimcontactsaverequest.h"
#include "cntsymbiansimengine.h"
#include "cntsimstore.h"
#include <qcontactsaverequest.h>

CntSimContactSaveRequest::CntSimContactSaveRequest(CntSymbianSimEngine *engine, QContactSaveRequest *req)
    :CntAbstractSimRequest(engine, req)
{
    connect( simStore(), SIGNAL(writeComplete(QContact, QContactManager::Error)),
        this, SLOT(writeComplete(QContact, QContactManager::Error)), Qt::QueuedConnection );
}

CntSimContactSaveRequest::~CntSimContactSaveRequest()
{
    cancel();
}

void CntSimContactSaveRequest::run()
{
    QContactSaveRequest *r = req<QContactSaveRequest>();
    
    if (!r->isActive())
        return;
    
    m_contacts = r->contacts();
    m_errorMap.clear();
    m_index = 0;
    
    writeNext();
}

void CntSimContactSaveRequest::writeComplete(QContact contact, QContactManager::Error error)
{
    if (!req()->isActive())
        return;
    
    if (error)
        m_errorMap.insert(m_index, error);
    engine()->updateDisplayLabel(contact);
    m_contacts[m_index] = contact;
    m_index++;
    singleShotTimer(KRequestDelay, this, SLOT(writeNext()));
}

void CntSimContactSaveRequest::writeNext()
{
    QContactSaveRequest *r = req<QContactSaveRequest>();
    
    if (!r->isActive())
        return;
    
    if (r->contacts().count() == 0) {
        QContactManagerEngine::updateContactSaveRequest(r, QList<QContact>(), QContactManager::BadArgumentError, m_errorMap, QContactAbstractRequest::FinishedState);
        return;
    }    
    
    // All contacts written?
    if (m_index >= m_contacts.count())
    {
        // Take first error from errormap (if any)
        QContactManager::Error error = QContactManager::NoError;
        if (m_errorMap.count())
            error = m_errorMap.begin().value();
        
        QContactManagerEngine::updateContactSaveRequest(r, m_contacts, error, m_errorMap, QContactAbstractRequest::FinishedState);
        return;
    }

    // Get next contact
    QContact contact = m_contacts.at(m_index);
    
    // Validate & write contact 
    QContactManager::Error error = QContactManager::NoError;
    if (engine()->validateContact(contact, error))
        simStore()->write(contact, error);

    if (error) {
        m_errorMap.insert(m_index, error);
        m_index++;
        singleShotTimer(KRequestDelay, this, SLOT(writeNext()));
    }
}
