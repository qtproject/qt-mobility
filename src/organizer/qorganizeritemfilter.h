/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** OrganizerItem: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
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
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QORGANIZERITEMFILTER_H
#define QORGANIZERITEMFILTER_H

#include <QVariant>
#include <QList>
#include <QDateTime>
#include <QSharedData>

#include "qtorganizerglobal.h"

class QDataStream;

QTM_BEGIN_NAMESPACE

/* Manual Q_DECLARE_ORGANIZERITEMFILTER_PRIVATE macro */

#define Q_DECLARE_ORGANIZERITEMFILTER_PRIVATE(Class) \
    inline Class##Private* d_func(); \
    inline const Class##Private* d_func() const; \
    friend class Class##Private;

class QOrganizerItemFilterPrivate;

// MSVC needs the function declared before the friend declaration
class QOrganizerItemFilter;
#ifndef QT_NO_DATASTREAM
Q_ORGANIZER_EXPORT QDataStream& operator<<(QDataStream& out, const QOrganizerItemFilter& filter);
Q_ORGANIZER_EXPORT QDataStream& operator>>(QDataStream& in, QOrganizerItemFilter& filter);
#endif

class Q_ORGANIZER_EXPORT QOrganizerItemFilter
{
public:
    QOrganizerItemFilter();
    ~QOrganizerItemFilter();
    QOrganizerItemFilter(const QOrganizerItemFilter& other);
    QOrganizerItemFilter& operator=(const QOrganizerItemFilter& other);

    enum FilterType {
        InvalidFilter,
        OrganizerItemDetailFilter,
        OrganizerItemDetailRangeFilter,
        ChangeLogFilter,
        ActionFilter,
        IntersectionFilter,
        UnionFilter,
        IdFilter,
        CollectionFilter,
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
        MatchCaseSensitive = Qt::MatchCaseSensitive // 16
    };
    Q_DECLARE_FLAGS(MatchFlags, MatchFlag)

    bool operator==(const QOrganizerItemFilter& other) const;
    bool operator!=(const QOrganizerItemFilter& other) const {return !operator==(other);}

protected:
    explicit QOrganizerItemFilter(QOrganizerItemFilterPrivate* d);

protected:
    friend class QOrganizerItemFilterPrivate;
#ifndef QT_NO_DATASTREAM
    Q_ORGANIZER_EXPORT friend QDataStream& operator<<(QDataStream& out, const QOrganizerItemFilter& filter);
    Q_ORGANIZER_EXPORT friend QDataStream& operator>>(QDataStream& in, QOrganizerItemFilter& filter);
#endif
    QSharedDataPointer<QOrganizerItemFilterPrivate> d_ptr;
};

const Q_ORGANIZER_EXPORT QOrganizerItemFilter operator&(const QOrganizerItemFilter& left, const QOrganizerItemFilter& right);
const Q_ORGANIZER_EXPORT QOrganizerItemFilter operator|(const QOrganizerItemFilter& left, const QOrganizerItemFilter& right);

QTM_END_NAMESPACE

Q_DECLARE_TYPEINFO(QTM_PREPEND_NAMESPACE(QOrganizerItemFilter), Q_MOVABLE_TYPE);
Q_DECLARE_OPERATORS_FOR_FLAGS(QTM_PREPEND_NAMESPACE(QOrganizerItemFilter::MatchFlags));

#endif
