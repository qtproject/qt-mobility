/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QLANDMARKABSTRACTREQUEST_H
#define QLANDMARKABSTRACTREQUEST_H

#include "qlandmarkmanager.h"
#include <QObject>

QTM_BEGIN_NAMESPACE

class QLandmarkAbstractRequestPrivate;
class Q_LOCATION_EXPORT QLandmarkAbstractRequest : public QObject
{
    Q_OBJECT

public:
    enum State {
        InactiveState = 0,
        ActiveState,
        FinishedState
    };

    enum RequestType {
        InvalidRequest = 0,
        LandmarkIdFetchRequest,
        CategoryIdFetchRequest,
        LandmarkFetchRequest,
        LandmarkFetchByIdRequest,
        CategoryFetchRequest,
        CategoryFetchByIdRequest,
        LandmarkSaveRequest,
        LandmarkRemoveRequest,
        CategorySaveRequest,
        CategoryRemoveRequest,
        ImportRequest,
        ExportRequest
    };

    QLandmarkAbstractRequest(QLandmarkManager *manager, QObject *parent = 0);
    virtual ~QLandmarkAbstractRequest();

    RequestType type() const;

    State state();
    bool isInactive()const;
    bool isActive() const;
    bool isFinished() const;

    QLandmarkManager::Error error() const;
    QString errorString() const;

    QLandmarkManager * manager() const;
    void setManager(QLandmarkManager *manager);

public Q_SLOTS:
    bool start();
    bool cancel();
    bool waitForFinished(int msecs = 0);

Q_SIGNALS:
    void resultsAvailable();
    void stateChanged(QLandmarkAbstractRequest::State newState);
protected:
    QLandmarkAbstractRequest(QLandmarkAbstractRequestPrivate *otherd, QObject *parent = 0);
    QLandmarkAbstractRequestPrivate *d_ptr;
private:
    Q_DISABLE_COPY(QLandmarkAbstractRequest)
    friend class QLandmarkManagerEngine;
    friend class QLandmarkAbstractRequestPrivate;
};

QTM_END_NAMESPACE

#endif
