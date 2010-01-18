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


#ifndef QCONTACTDETAIL_P_H
#define QCONTACTDETAIL_P_H

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

#include "qcontactdetail.h"

#include <QSharedData>
#include <QMap>
#include <QString>

QTM_BEGIN_NAMESPACE

class QContactDetailPrivate : public QSharedData
{
public:
    QContactDetailPrivate()
        : QSharedData(),
        m_id(lastDetailKey.fetchAndAddOrdered(1))
        m_access(QContactDetail::NoConstraint)
    {
    }

    QContactDetailPrivate(const QContactDetailPrivate& other)
        : QSharedData(other),
        m_id(other.m_id),
        m_definitionName(other.m_definitionName),
        m_values(other.m_values),
        m_preferredActions(other.m_preferredActions),
        m_access(other.m_access)
    {
    }

    ~QContactDetailPrivate() {}

    int m_id; // internal, unique id.
    QString m_definitionName;
    QVariantMap m_values; // the value(s) stored in this field.
    QList<QContactActionDescriptor> m_preferredActions;

    static QAtomicInt lastDetailKey;
    QContactDetail::AccessConstraints m_access;

    static void setAccessConstraints(QContactDetail *d, QContactDetail::AccessConstraints constraint)
    {
        d->d->m_access = constraint;
    }
};

QTM_END_NAMESPACE

#endif
