/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/


#ifndef QCONTACTABSTRACTREQUEST_H
#define QCONTACTABSTRACTREQUEST_H

#include "qcontactmanager.h"
#include <QObject>

class QTCONTACTS_EXPORT QContactAbstractRequest : public QObject
{
public:
    QContactAbstractRequest() {}
    ~QContactAbstractRequest() {}

    enum Status {
        Inactive = 0,   // operation not yet started
        Pending,        // operation started, not yet finished
        Cancelling,     // operation started then cancelled, not yet finished
        Cancelled,      // operation is finished due to cancellation
        Finished        // operation successfully completed
    };

    virtual bool isFinished() const = 0;

    virtual QContactManager::Error error() const = 0;
    virtual Status status() const = 0;

    enum Type {
        Invalid = 0,
        Contact,
        DetailDefinition,
        Group
    };

    virtual Type type() const = 0;

    enum Operation {
        SaveOperation = 0,
        RemoveOperation,
        RetrieveOperation
    };

public slots:
    /* waiting for stuff */
    virtual bool waitForFinished(int msecs = 0) = 0;
    virtual bool waitForProgress(int msecs = 0) = 0; /* also waits for progress emitted by finished */

    /* Verbs */
    virtual void start(QContactManager *manager, Operation operation = QContactAbstractRequest::RetrieveOperation) = 0;
    virtual void cancel() = 0;

private:
    Q_DISABLE_COPY(QContactAbstractRequest);
};

#endif
