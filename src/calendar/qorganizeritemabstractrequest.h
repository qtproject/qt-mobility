/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Item: Nokia Corporation (qt-info@nokia.com)
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

#ifndef QORGANIZERITEMABSTRACTREQUEST_H
#define QORGANIZERITEMABSTRACTREQUEST_H

#include "qorganizeritemmanager.h"
#include <QObject>

QTM_BEGIN_NAMESPACE

class QOrganizerItemManagerEngine;
class QOrganizerItemAbstractRequestPrivate;
class Q_CALENDAR_EXPORT QOrganizerItemAbstractRequest : public QObject
{
    Q_OBJECT

public:
    ~QOrganizerItemAbstractRequest();

    enum State {
        InactiveState = 0,   // operation not yet started
        ActiveState,         // operation started, not yet finished
        CanceledState,       // operation is finished due to cancellation
        FinishedState        // operation either completed successfully or failed.  No further results will become available.
    };

    State state() const; // replaces status()
    bool isInactive() const;
    bool isActive() const;
    bool isFinished() const;
    bool isCanceled() const;
    QOrganizerItemManager::Error error() const;

    enum RequestType {
        InvalidRequest = 0,
        ItemInstanceFetchRequest,
        ItemFetchRequest,
        ItemLocalIdFetchRequest,
        ItemRemoveRequest,
        ItemSaveRequest,
        DetailDefinitionFetchRequest,
        DetailDefinitionRemoveRequest,
        DetailDefinitionSaveRequest
    };

    RequestType type() const;

    /* Which manager we want to perform the asynchronous request */
    QOrganizerItemManager* manager() const;
    void setManager(QOrganizerItemManager* manager);

public Q_SLOTS:
    /* Verbs */
    bool start();
    bool cancel();

    /* waiting for stuff */
    bool waitForFinished(int msecs = 0);

Q_SIGNALS:
    void stateChanged(QOrganizerItemAbstractRequest::State newState);
    void resultsAvailable();

protected:
    QOrganizerItemAbstractRequest(QOrganizerItemAbstractRequestPrivate* otherd, QObject* parent = 0);
    QOrganizerItemAbstractRequestPrivate* d_ptr;

private:
    QOrganizerItemAbstractRequest(QObject* parent = 0) : QObject(parent), d_ptr(0) {}
    Q_DISABLE_COPY(QOrganizerItemAbstractRequest)
    friend class QOrganizerItemManagerEngine;
};

QTM_END_NAMESPACE

#endif
