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
#include <qcontactdetaildefinitionfetchrequest.h>

CntSimDetailDefinitionFetchRequest::CntSimDetailDefinitionFetchRequest(CntSymbianSimEngine *engine, QContactDetailDefinitionFetchRequest *req)
    :CntAbstractSimRequest(engine, req)
{

}

CntSimDetailDefinitionFetchRequest::~CntSimDetailDefinitionFetchRequest()
{
    cancel();
}

void CntSimDetailDefinitionFetchRequest::run()
{
    QContactDetailDefinitionFetchRequest *r = req<QContactDetailDefinitionFetchRequest>();
    
    if (!r->isActive())
        return;
    
    QContactManager::Error error = QContactManager::NoError;
    QMap<QString, QContactDetailDefinition> result;
    QMap<int, QContactManager::Error> errorMap;
        
    // Get all detail definitions
    QMap<QString, QContactDetailDefinition> allDefs = engine()->detailDefinitions(r->contactType(), error);
    
    QStringList defNames = r->definitionNames();
    
    // Check for error
    if (error != QContactManager::NoError) 
    {
        for (int i=0; i<defNames.count(); i++)
            errorMap.insert(i, error);
    
        // Complete the request
        QContactManagerEngine::updateDefinitionFetchRequest(r, result, error, errorMap, QContactAbstractRequest::FinishedState);
        return;
    }

    // Filter results
    if (r->definitionNames().count() == 0) 
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
    QContactManagerEngine::updateDefinitionFetchRequest(r, result, error, errorMap, QContactAbstractRequest::FinishedState);
}
