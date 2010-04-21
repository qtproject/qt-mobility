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

#ifndef QORGANIZERITEMRECURRENCERULES_H
#define QORGANIZERITEMRECURRENCERULES_H

#include <QString>

#include "qtorganizeritemsglobal.h"
#include "qorganizeritemdetail.h"
#include "qorganizeritem.h"
#include "qorganizeritemrecurrencerulecriteria.h"

QTM_BEGIN_NAMESPACE

class Q_CALENDAR_EXPORT QOrganizerItemRecurrenceRules
{
public:
    QOrganizerItemRecurrenceRules();
    ~QOrganizerItemRecurrenceRules();


    // calculate and return all of the occurrence dates
    QSet<QDateTime> occurrences() const;

    // QOrganizerItemRecurrenceRules is either:
    // 1) a single QOrganizerItemRecurrenceRuleCriteria (via setCriteria())
    // 2) a pair of QOrganizerItemRecurrenceRules joined by a set operator, created via the public "operator" functions.

    bool isCriteria() const;
    inline bool isExpression() const { return !isCriteria(); }

    bool setCriteria(const QOrganizerItemRecurrenceRuleCriteria& criteria);
    QOrganizerItemRecurrenceRuleCriteria criteria() const;

    enum Expression {
        Intersection,               // both left and right must be satisfied
        Union,                      // either left or right must be satisfied
        LeftRelativeComplement,     // the occurrences in the left which are not occurrences of the right
        RightRelativeComplement     // the occurrences in the right which are not occurrences of the left
    };

    QOrganizerItemRecurrenceRules left() const;
    QOrganizerItemRecurrenceRules right() const;
    Expression joiningExpression() const;
};


const Q_CALENDAR_EXPORT QOrganizerItemRecurrenceRules notOf(const QOrganizerItemRecurrenceRules& input);

const Q_CALENDAR_EXPORT QOrganizerItemRecurrenceRules intersectionOf(const QOrganizerItemRecurrenceRules& left, const QOrganizerItemRecurrenceRules& right);
const Q_CALENDAR_EXPORT QOrganizerItemRecurrenceRules unionOf(const QOrganizerItemRecurrenceRules& left, const QOrganizerItemRecurrenceRules& right);
const Q_CALENDAR_EXPORT QOrganizerItemRecurrenceRules leftRelativeComplementOf(const QOrganizerItemRecurrenceRules& left, const QOrganizerItemRecurrenceRules& right);
const Q_CALENDAR_EXPORT QOrganizerItemRecurrenceRules rightRelativeComplementOf(const QOrganizerItemRecurrenceRules& left, const QOrganizerItemRecurrenceRules& right);

const Q_CALENDAR_EXPORT QOrganizerItemRecurrenceRules completeRelativeComplementOf(const QOrganizerItemRecurrenceRules& left, const QOrganizerItemRecurrenceRules& right); // union(lrc(left, right), rrc(left, right))
const Q_CALENDAR_EXPORT QOrganizerItemRecurrenceRules inverseUnionOf(const QOrganizerItemRecurrenceRules& left, const QOrganizerItemRecurrenceRules& right); // not(union(left, right))

QTM_END_NAMESPACE

#endif

