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

#ifndef QORGANIZERITEMREQUESTQUEUE_H_
#define QORGANIZERITEMREQUESTQUEUE_H_

// System includes
#include <QObject>
#include <QMap>
#include <QTimer>
#include <QEventLoop>

// User includes
#include "organizeritemrequestserviceprovider.h"

// Forward declarations
//class QOrganizerAbstractRequest;
class QOrganizerItemSymbianEngine;

class QOrganizerItemRequestQueue : public QObject
{
    Q_OBJECT
public:
    // Returns a heap instantiated instance of the class
    static QOrganizerItemRequestQueue* instance(
            QOrganizerItemSymbianEngine& aOrganizerItemManagerEngine);
    // Destructor
    ~QOrganizerItemRequestQueue();
    // To start req request
    bool startRequest(QOrganizerAbstractRequest* req);
    // To cancel req request
    bool cancelRequest(QOrganizerAbstractRequest* req);
    // Wait for request to complete 
    bool waitForRequestFinished(QOrganizerAbstractRequest* req, int msecs);
    // Request is no more a valid request, removes req from the queue
    void requestDestroyed(QOrganizerAbstractRequest* req);

private:
    QOrganizerItemRequestQueue(
            QOrganizerItemSymbianEngine& aOrganizerItemManagerEngine);

private:
    QOrganizerItemSymbianEngine&                m_organizerItemManagerEngine;
    QMap<QOrganizerAbstractRequest*, COrganizerItemRequestsServiceProvider*>m_abstractRequestMap;
};

#endif /* QORGANIZERITEMREQUESTQUEUE_H_ */
