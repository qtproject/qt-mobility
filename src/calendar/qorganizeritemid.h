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

#ifndef QORGANIZERITEMID_H
#define QORGANIZERITEMID_H

#include <QString>
#include <QSharedDataPointer>

#include "qtorganizeritemsglobal.h"

QTM_BEGIN_NAMESPACE

typedef quint32 QOrganizerItemLocalId;

class QOrganizerItemIdPrivate;
class Q_CALENDAR_EXPORT QOrganizerItemId
{
public:
    QOrganizerItemId();
    ~QOrganizerItemId();

    QOrganizerItemId(const QOrganizerItemId& other);
    QOrganizerItemId& operator=(const QOrganizerItemId& other);

    bool operator==(const QOrganizerItemId& other) const;
    bool operator!=(const QOrganizerItemId& other) const;
    bool operator<(const QOrganizerItemId& other) const;

    QString managerUri() const;
    QOrganizerItemLocalId localId() const;

    void setManagerUri(const QString& uri);
    void setLocalId(const QOrganizerItemLocalId& id);

private:
    QSharedDataPointer<QOrganizerItemIdPrivate> d;
};

Q_CALENDAR_EXPORT uint qHash(const QOrganizerItemId& key);
#ifndef QT_NO_DEBUG_STREAM
Q_CALENDAR_EXPORT QDebug operator<<(QDebug dbg, const QOrganizerItemId& id);
#endif

QTM_END_NAMESPACE

Q_DECLARE_TYPEINFO(QTM_PREPEND_NAMESPACE(QOrganizerItemId), Q_MOVABLE_TYPE);


#endif

