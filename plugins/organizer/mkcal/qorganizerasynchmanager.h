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

#ifndef QORGANIZERASYNCHMANAGER_H
#define QORGANIZERASYNCHMANAGER_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qtorganizer.h"
#include <QtCore/qthread.h>
#include <QtCore/qqueue.h>
#include <QtCore/qmutex.h>
#include <QtCore/qmap.h>
#include <QSharedPointer>
#include <QWaitCondition>

QTM_USE_NAMESPACE

class QOrganizerItemMKCalEngine;
class AsyncWorker;

class OrganizerAsynchManager
{
public:
    OrganizerAsynchManager(QOrganizerManagerEngine *engine, int maxWorkers = 1);
    ~OrganizerAsynchManager();

    void requestDestroyed(QOrganizerAbstractRequest *req);
    bool startRequest(QOrganizerAbstractRequest *req);
    bool cancelRequest(QOrganizerAbstractRequest *req);
    bool waitForRequestFinished(QOrganizerAbstractRequest *req, int msecs = -1);

private:
    QOrganizerManagerEngine* m_engine;

    int m_maxWorkers;
    QQueue<AsyncWorker *> m_idleWorkers;
    QQueue<AsyncWorker *> m_activeWorkers;

    QQueue<QOrganizerAbstractRequest *> m_queuedRequests;
    QSet<QOrganizerAbstractRequest *> m_activeRequests;

    QMap<QOrganizerAbstractRequest*, QSharedPointer<QWaitCondition> > m_waitMap;
    QMutex m_mutexMap;

    QMutex m_mutex;

    volatile bool m_destroying;
    QWaitCondition m_destroyWait;

    void workerDone(AsyncWorker *worker, QOrganizerAbstractRequest *req);

    friend class AsyncWorker;
};

#endif // QORGANIZERASYNCHMANAGER_H
