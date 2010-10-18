/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QORGANIZERASYNCHPROCESS_H
#define QORGANIZERASYNCHPROCESS_H

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
#include <QtCore/qtimer.h>

QTM_USE_NAMESPACE

class QOrganizerItemMaemo5Engine;

class OrganizerRequestTimeoutTimer : public QObject
{
    Q_OBJECT

public:
    OrganizerRequestTimeoutTimer(QOrganizerAbstractRequest *req, int msecs);
    ~OrganizerRequestTimeoutTimer();

    QOrganizerAbstractRequest *request() const;

private Q_SLOTS:
    void internalTimeout();

Q_SIGNALS:
    void timeout(OrganizerRequestTimeoutTimer* timer);

private:
    QTimer* m_timeoutTimer;
    QOrganizerAbstractRequest *m_req;
};


class OrganizerAsynchProcess : public QThread
{
    Q_OBJECT

public:
    OrganizerAsynchProcess(QOrganizerItemMaemo5Engine *engine);
    ~OrganizerAsynchProcess();

    void requestDestroyed(QOrganizerAbstractRequest *req);
    bool addRequest(QOrganizerAbstractRequest *req);
    bool cancelRequest(QOrganizerAbstractRequest *req);
    bool waitForRequestFinished(QOrganizerAbstractRequest *req, int msecs);

protected:
    void processRequest();
    void run();

private:
    bool waitForRequestFinished(QOrganizerAbstractRequest *req);

    void handleItemFetchRequest(QOrganizerItemFetchRequest *req);
    void handleItemFetchForExportRequest(QOrganizerItemFetchForExportRequest *req);
    void handleItemOccurrenceFetchRequest(QOrganizerItemOccurrenceFetchRequest *req);
    void handleIdFetchRequest(QOrganizerItemIdFetchRequest *req);
    void handleItemRemoveRequest(QOrganizerItemRemoveRequest *req);
    void handleSaveRequest(QOrganizerItemSaveRequest *req);
    void handleDefinitionFetchRequest(QOrganizerItemDetailDefinitionFetchRequest *req);
    void handleDefinitionRemoveRequest(QOrganizerItemDetailDefinitionRemoveRequest *req);
    void handleDefinitionSaveRequest(QOrganizerItemDetailDefinitionSaveRequest *req);
    void handleCollectionFetchRequest(QOrganizerCollectionFetchRequest *req);
    void handleCollectionRemoveRequest(QOrganizerCollectionRemoveRequest *req);
    void handleCollectionSaveRequest(QOrganizerCollectionSaveRequest *req);

private Q_SLOTS:
    void timeout(OrganizerRequestTimeoutTimer* timer);

private:
    QOrganizerItemMaemo5Engine* m_engine;
    QQueue<QOrganizerAbstractRequest *> m_requestQueue;
    QList<OrganizerRequestTimeoutTimer *> m_timers;
    QSet<QOrganizerAbstractRequest *> m_activeRequests;
    QMutex m_mainMutex;
    QMutex m_timeoutMutex;
    bool m_quitNow;
};

#endif // QORGANIZERASYNCHPROCESS_H
