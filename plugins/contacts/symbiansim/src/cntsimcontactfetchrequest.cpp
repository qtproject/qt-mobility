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

#include "cntsimcontactfetchrequest.h"
#include "cntsymbiansimengine.h"
#include "cntsimstore.h"
#include <qcontactfetchrequest.h>
#include <qcontactlocalidfilter.h>

CntSimContactFetchRequest::CntSimContactFetchRequest(CntSymbianSimEngine *engine, QContactFetchRequest *req)
    :CntAbstractSimRequest(engine),
     m_req(req)
{
    connect( simStore(), SIGNAL(readComplete(QList<QContact>, QContactManager::Error)),
        this, SLOT(readComplete(QList<QContact>, QContactManager::Error)), Qt::QueuedConnection );
}

CntSimContactFetchRequest::~CntSimContactFetchRequest()
{
    cancel();
}

bool CntSimContactFetchRequest::start()
{
    if (m_req->isActive())
        return false;

    clearRetryCount();
    
    QContactManager::Error error = QContactManager::NoError;
    if (execute(error))
        QContactManagerEngine::updateRequestState(m_req, QContactAbstractRequest::ActiveState);
    return (error == QContactManager::NoError);
}

bool CntSimContactFetchRequest::cancel()
{
    if (m_req->isActive()) {
        cancelTimer();
        simStore()->cancel();
        QContactManagerEngine::updateRequestState(m_req, QContactAbstractRequest::CanceledState);
        return true;
    }
    return false;
}

void CntSimContactFetchRequest::retry()
{
    QContactManager::Error error = QContactManager::NoError;
    if (!execute(error)) {
        QContactManagerEngine::updateRequestState(m_req, QContactAbstractRequest::FinishedState);
        QContactManagerEngine::updateContactFetchRequest(m_req, QList<QContact>(), error);
    }
}

void CntSimContactFetchRequest::readComplete(QList<QContact> contacts, QContactManager::Error error)    
{
    if (!m_req->isActive())
        return;
    
    // Sometimes the sim store will return server busy error. All we can do is
    // wait and try again. The error seems to occur if we try to read from the
    // store right after writing some contacts to it.  
    // This was observed with S60 5.0 HW (Tube).
    if (simStore()->lastAsyncError() == KErrServerBusy) {
        if (waitAndRetry())
            return;
    }
    
    // Filter & sort results
    QList<QContact> filteredAndSorted;
    for (int i=0; i<contacts.count(); i++) {
        if (QContactManagerEngine::testFilter(m_req->filter(), contacts.at(i)))
            QContactManagerEngine::addSorted(&filteredAndSorted, contacts.at(i), m_req->sorting());
    }

    // Complete the request
    QContactManagerEngine::updateRequestState(m_req, QContactAbstractRequest::FinishedState);    
    QContactManagerEngine::updateContactFetchRequest(m_req, filteredAndSorted, error);
}

bool CntSimContactFetchRequest::execute(QContactManager::Error &error)
{
    // Get filter
    QContactLocalIdFilter lidFilter;
    if (m_req->filter().type() == QContactFilter::LocalIdFilter) {
        lidFilter = static_cast<QContactLocalIdFilter>(m_req->filter());
    }        

    // Fetch all contacts and filter the results.
    // Contacts are fetched starting from index 1, all slots are read
    // since slots may be not filled in a sequence.
    int index = 1;
    int numSlots = simStore()->storeInfo().iTotalEntries;
    
    if (lidFilter.ids().count() == 1) {
        // Optimization for performance. Fetch a single contact from store.
        // This is mainly for CntSymbianSimEngine::contact().
        index = lidFilter.ids().at(0);
        numSlots = 1;
    } 

    return simStore()->read(index, numSlots, error);
}
