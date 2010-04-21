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

#ifndef QORGANIZERITEMFILTER_H
#define QORGANIZERITEMFILTER_H

#include <QVariant>
#include <QList>
#include <QDateTime>
#include <QSharedData>

#include "qtorganizeritemsglobal.h"

QTM_BEGIN_NAMESPACE

/* Manual Q_DECLARE_CONTACTFILTER_PRIVATE macro */

#define Q_DECLARE_CONTACTFILTER_PRIVATE(Class) \
    inline Class##Private* d_func(); \
    inline const Class##Private* d_func() const; \
    friend class Class##Private;

class QOrganizerItemFilterPrivate;
class Q_CONTACTS_EXPORT QOrganizerItemFilter
{
public:
    QOrganizerItemFilter();
    ~QOrganizerItemFilter();
    QOrganizerItemFilter(const QOrganizerItemFilter& other);
    QOrganizerItemFilter& operator=(const QOrganizerItemFilter& other);

    enum FilterType {
        InvalidFilter,
        ContactDetailFilter,
        ContactDetailRangeFilter,
        ChangeLogFilter,
        ActionFilter,
        RelationshipFilter,
        IntersectionFilter,
        UnionFilter,
        LocalIdFilter,
        DefaultFilter
    };

    FilterType type() const;

    // Qt::MatchFlags don't quite match here
    enum MatchFlag {
        MatchExactly = Qt::MatchExactly,    // 0
        MatchContains = Qt::MatchContains,  // 1
        MatchStartsWith = Qt::MatchStartsWith,  // 2
        MatchEndsWith = Qt::MatchEndsWith, // 3
        MatchFixedString = Qt::MatchFixedString, // 8
        MatchCaseSensitive = Qt::MatchCaseSensitive, // 16
        MatchPhoneNumber = 1024,
        MatchKeypadCollation = 2048
    };
    Q_DECLARE_FLAGS(MatchFlags, MatchFlag)

    bool operator==(const QOrganizerItemFilter& other) const;
    bool operator!=(const QOrganizerItemFilter& other) const {return !operator==(other);}

protected:
    explicit QOrganizerItemFilter(QOrganizerItemFilterPrivate* d);

protected:
    friend class QOrganizerItemFilterPrivate;
    QSharedDataPointer<QOrganizerItemFilterPrivate> d_ptr;
};

const Q_CONTACTS_EXPORT QOrganizerItemFilter operator&(const QOrganizerItemFilter& left, const QOrganizerItemFilter& right);
const Q_CONTACTS_EXPORT QOrganizerItemFilter operator|(const QOrganizerItemFilter& left, const QOrganizerItemFilter& right);

QTM_END_NAMESPACE

Q_DECLARE_TYPEINFO(QTM_PREPEND_NAMESPACE(QOrganizerItemFilter), Q_MOVABLE_TYPE);

#endif
