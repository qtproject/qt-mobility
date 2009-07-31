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

#define Q_DECLARE_CONTACTREQUEST_PRIVATE(Class) \
    inline Class##Private* d_func(); \
    inline const Class##Private* d_func() const;

class QContactManagerEngine;
class QContactAbstractRequestPrivate;
class QTCONTACTS_EXPORT QContactAbstractRequest : public QObject
{
    Q_OBJECT

public:
    QContactAbstractRequest() {}
    virtual ~QContactAbstractRequest();

    enum Status {
        Inactive = 0,   // operation not yet started
        Active,         // operation started, not yet finished
        Cancelling,     // operation started then cancelled, not yet finished
        Cancelled,      // operation is finished due to cancellation
        Finished        // operation successfully completed
    };

    Status status() const;
    bool isActive() const;
    bool isFinished() const;
    QList<QContactManager::Error> errors();
    QContactManager::Error error() const;

    enum Type {
        Invalid = 0,
        ContactFetch,
        ContactIdFetch,
        ContactRemove,
        ContactSave,
        DetailDefinitionFetch,
        DetailDefinitionRemove,
        DetailDefinitionSave,
        GroupFetch,
        GroupRemove,
        GroupSave
    };

    virtual Type type() const = 0;

    /* Which manager we want to perform the asynchronous request */
    QContactManager* manager() const;
    void setManager(QContactManager* manager);

public slots:
    /* Verbs */
    bool start();
    bool cancel();

    /* waiting for stuff */
    bool waitForFinished(int msecs = 0);
    bool waitForProgress(int msecs = 0);

protected:
    QContactAbstractRequest(QContactAbstractRequestPrivate* otherd);
    QContactAbstractRequestPrivate* d_ptr;

private:
    Q_DISABLE_COPY(QContactAbstractRequest);
    friend class QContactManagerEngine;
};

#endif
