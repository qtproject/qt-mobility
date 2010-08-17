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


#ifndef QCONTACTREQUESTWORKER_H
#define QCONTACTREQUESTWORKER_H

#include <QThread>
#include <QSharedDataPointer>

#include "qtcontactsglobal.h"

QTM_BEGIN_NAMESPACE

class QContactAbstractRequest;
class QContactFetchRequest;
class QContactLocalIdFetchRequest;
class QContactSaveRequest;
class QContactRemoveRequest;
class QContactRelationshipFetchRequest;
class QContactRelationshipSaveRequest;
class QContactRelationshipRemoveRequest;
class QContactDetailDefinitionFetchRequest;
class QContactDetailDefinitionSaveRequest;
class QContactDetailDefinitionRemoveRequest;
QTM_END_NAMESPACE

class QContactRequestWorkerData;

QTM_USE_NAMESPACE

class QContactRequestWorker : public QThread
{
    Q_OBJECT

public:
    QContactRequestWorker();
    QContactRequestWorker(const QContactRequestWorker& other);
    QContactRequestWorker& operator=(const QContactRequestWorker& other);
    virtual ~QContactRequestWorker();
    
    /* Stop the worker thread*/
    void stop();
    
    /* Main loop of the worker thread*/
    virtual void run();

    /* Add a asynchronous request to the worker queue*/
    bool addRequest(QContactAbstractRequest* req);

    /* Remove a asynchronous request from the worker queue*/
    bool removeRequest(QContactAbstractRequest* req);
    
    /* Cancel a asynchronous request*/
    bool cancelRequest(QContactAbstractRequest* req);
    
    /* waiting for a request*/
    bool waitRequest(QContactAbstractRequest* req, int msecs);

protected:
    /* Actual asynchronous requests process functions*/
    virtual void processContactFetchRequest(QContactFetchRequest* req);
    virtual void processContactLocalIdFetchRequest(QContactLocalIdFetchRequest* req);
    virtual void processContactSaveRequest(QContactSaveRequest* req);
    virtual void processContactRemoveRequest(QContactRemoveRequest* req );
    virtual void processContactRelationshipFetchRequest(QContactRelationshipFetchRequest* req);
    virtual void processContactRelationshipSaveRequest(QContactRelationshipSaveRequest* req);
    virtual void processContactRelationshipRemoveRequest(QContactRelationshipRemoveRequest* req);
    virtual void processContactDetailDefinitionFetchRequest(QContactDetailDefinitionFetchRequest* req);
    virtual void processContactDetailDefinitionSaveRequest(QContactDetailDefinitionSaveRequest* req);
    virtual void processContactDetailDefinitionRemoveRequest(QContactDetailDefinitionRemoveRequest* req);

private:
    QSharedDataPointer<QContactRequestWorkerData> d;
};



#endif

