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

#include "cntsimdetaildefinitionfetchrequest.h"
#include "cntsymbiansimengine.h"
#include "cntsimstore.h"
#include <qcontactdetaildefinitionfetchrequest.h>
#include <QTimer>

CntSimDetailDefinitionFetchRequest::CntSimDetailDefinitionFetchRequest(CntSymbianSimEngine *engine, QContactDetailDefinitionFetchRequest *req)
    :CntAbstractSimRequest(engine),
     m_req(req)
{

}

CntSimDetailDefinitionFetchRequest::~CntSimDetailDefinitionFetchRequest()
{
    cancel();
}

bool CntSimDetailDefinitionFetchRequest::start()
{
    singleShotTimer(0, this, SLOT(readDetailDefinitions()));
    QContactManagerEngine::updateRequestState(m_req, QContactAbstractRequest::ActiveState);
    return true; 
}

bool CntSimDetailDefinitionFetchRequest::cancel()
{
    if (m_req->isActive()) {
        cancelTimer();
        QContactManagerEngine::updateRequestState(m_req, QContactAbstractRequest::CanceledState);
        return true;
    }
    return false;
}

void CntSimDetailDefinitionFetchRequest::readDetailDefinitions()
{
    if (!m_req->isActive())
        return;
    
    QContactManager::Error error = QContactManager::NoError;
    QMap<QString, QContactDetailDefinition> result;
    QMap<int, QContactManager::Error> errorMap;
        
    // Get all detail definitions
    QMap<QString, QContactDetailDefinition> allDefs = engine()->detailDefinitions(m_req->contactType(), error);
    
    QStringList defNames = m_req->definitionNames();
    
    // Check for error
    if (error != QContactManager::NoError) 
    {
        for (int i=0; i<defNames.count(); i++)
            errorMap.insert(i, error);
    
        // Complete the request
        QContactManagerEngine::updateRequestState(m_req, QContactAbstractRequest::FinishedState);
        QContactManagerEngine::updateDefinitionFetchRequest(m_req, result, error, errorMap);
        return;
    }

    // Filter results
    if (m_req->definitionNames().count() == 0) 
    {
        result = allDefs;
    }
    else 
    {
        for (int i=0; i<defNames.count(); i++) 
        {
            QString defName = defNames.at(i);
            if (allDefs.contains(defName))
                result.insert(defName, allDefs.value(defName));
            else
                errorMap.insert(i, QContactManager::DoesNotExistError);
        }
        
        // Set first error as the general error
        if (errorMap.count())
            error = errorMap.begin().value();
    }
    
    // Complete the request
    QContactManagerEngine::updateRequestState(m_req, QContactAbstractRequest::FinishedState);
    QContactManagerEngine::updateDefinitionFetchRequest(m_req, result, error, errorMap);
}


