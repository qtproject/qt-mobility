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

#ifndef QCONTACTRELATIONSHIPREMOVEREQUEST_H
#define QCONTACTRELATIONSHIPREMOVEREQUEST_H

#include "qtcontactsglobal.h"
#include "qcontactabstractrequest.h"

#include <QString>

QTM_BEGIN_NAMESPACE

class QContactRelationshipRemoveRequestPrivate;
class Q_CONTACTS_EXPORT QContactRelationshipRemoveRequest : public QContactAbstractRequest
{
    Q_OBJECT

public:
    QContactRelationshipRemoveRequest(QObject* parent = 0);
    ~QContactRelationshipRemoveRequest();

    /* Selection */
    void Q_DECL_DEPRECATED setFirst(const QContactId& firstId);                   // deprecated, replaced by explicitly defined relationship list
    QContactId Q_DECL_DEPRECATED first() const;                                   // deprecated, replaced by explicitly defined relationship list

    void Q_DECL_DEPRECATED setRelationshipType(const QString& relationshipType);  // deprecated, replaced by explicitly defined relationship list
    QString Q_DECL_DEPRECATED relationshipType() const;                           // deprecated, replaced by explicitly defined relationship list

    void Q_DECL_DEPRECATED setSecond(const QContactId& secondId);                 // deprecated, replaced by explicitly defined relationship list
    QContactId Q_DECL_DEPRECATED second() const;                                  // deprecated, replaced by explicitly defined relationship list

    void setRelationship(const QContactRelationship& relationship);
    void setRelationships(const QList<QContactRelationship>& relationships);
    QList<QContactRelationship> relationships() const;

    /* Results */
    QMap<int, QContactManager::Error> errorMap() const;

private:
    Q_DISABLE_COPY(QContactRelationshipRemoveRequest)
    friend class QContactManagerEngine;
    Q_DECLARE_PRIVATE_D(d_ptr, QContactRelationshipRemoveRequest)
};

QTM_END_NAMESPACE

#endif
