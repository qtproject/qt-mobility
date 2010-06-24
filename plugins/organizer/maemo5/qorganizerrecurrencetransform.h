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

#ifndef QORGANIZERRECURRENCETRANSFORM_H
#define QORGANIZERRECURRENCETRANSFORM_H

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

#include "qtorganizer.h"
#include <CMulticalendar.h>
#include <CRecurrenceRule.h>

QTM_USE_NAMESPACE

class OrganizerRecurrenceTransform
{
public:
    OrganizerRecurrenceTransform();
    ~OrganizerRecurrenceTransform();

public: // Conversion from QRecurrence to CRecurrence
    // init
    void beginTransformToCrecurrence();

    // set rules and dates
    void addQOrganizerItemRecurrenceRule(const QOrganizerItemRecurrenceRule &rule);
    void addQOrganizerItemExceptionRule(const QOrganizerItemRecurrenceRule &rule);

    // get recurrence
    CRecurrence* crecurrence(bool *success = 0) const;

public: // Conversion from CRecurrence to QRecurrence
    // init and set recurrence
    void transformToQrecurrence(CRecurrence *crecurrence);

    // get rules and dates
    QList<QOrganizerItemRecurrenceRule> recurrenceRules() const;
    QList<QOrganizerItemRecurrenceRule> exceptionRules() const;

private:
    // qrule -> crule conversions
    QString qrecurrenceRuleToIcalRecurrenceRule(const QOrganizerItemRecurrenceRule &rule) const;
    QString qfrequencyToIcalFrequency(QOrganizerItemRecurrenceRule::Frequency frequency) const;
    QString qcountToIcalCount(int count) const;
    QString qintervalToIcalInterval(int interval) const;
    QString qendDateToIcalUntil(QDate endDate) const;
    QString qdaysOfWeekToIcalByDay(const QList<Qt::DayOfWeek> &daysOfWeek) const;
    QString qweekStartToIcalWkst(Qt::DayOfWeek dayOfWeek) const;
    QString qweekdayToIcalWeekday(Qt::DayOfWeek dayOfWeek) const;
    QString qdaysOfMonthToIcalByMonthDay(const QList<int> &daysOfMonth) const;
    QString qdaysOfYearToIcalByYearDay(const QList<int> &daysOfYear) const;
    QString qmonthsToIcalByMonth(const QList<QOrganizerItemRecurrenceRule::Month> &months) const;
    QString qweeksOfYearToIcalByWeekNo(const QList<int> &weeksOfYear) const;
    QString qpositionsToIcalBySetPos(const QList<int> &positions) const;
    QString listOfNumbers(const QList<int> &list) const;
    int qfrequencyToRtype(QOrganizerItemRecurrenceRule::Frequency frequency) const;

    // crule -> qrule conversions
    QOrganizerItemRecurrenceRule icalRecurrenceRuleToQrecurrenceRule(CRecurrenceRule *rule) const;
    QOrganizerItemRecurrenceRule::Frequency icalFrequencyToQfrequency(FREQUENCY frequency) const;
    Qt::DayOfWeek icalWeekdayToQdayOfWeek(short weekday, bool *status = 0) const;
    Qt::DayOfWeek icalrecurrencetypeWeekdayToQdayOfWeek(const QString& weekday) const;

private:
    // QRecurrence -> CRecurrence
    int m_rtype; // Recursion type parameter for the native calendar backend
    std::vector< CRecurrenceRule* > m_vRRuleList; // recurrence and exception rules
    std::vector< std::string > m_vRecDateList; // recursion dates
    std::vector< std::string > m_vExceptionDateList; // exception dates

    // CRecurrence -> QRecurrence
    QList<QOrganizerItemRecurrenceRule> m_lRecurrenceRules;
    QList<QOrganizerItemRecurrenceRule> m_lExceptionRules;
};

#endif // QORGANIZERRECURRENCETRANSFORM_H
