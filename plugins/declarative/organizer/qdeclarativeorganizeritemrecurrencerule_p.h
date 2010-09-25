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


#ifndef QDECLARATIVEORGANIZERITEMRECURRENCERULE_H
#define QDECLARATIVEORGANIZERITEMRECURRENCERULE_H

#include <QtDeclarative>
#include "qorganizeritemrecurrencerule.h"


QTM_USE_NAMESPACE

class QDeclarativeOrganizerItemRecurrenceRule : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Frequency frequency READ frequency WRITE setFrequency NOTIFY valueChanged)
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY valueChanged)
    Q_PROPERTY(QDate endDate READ endDate WRITE setEndDate NOTIFY valueChanged)
    Q_PROPERTY(int interval READ interval WRITE setInterval NOTIFY valueChanged)
    Q_PROPERTY(QVariantList daysOfWeek READ daysOfWeek WRITE setDaysOfWeek NOTIFY valueChanged)
    Q_PROPERTY(QVariantList daysOfMonth READ daysOfMonth WRITE setDaysOfMonth NOTIFY valueChanged)
    Q_PROPERTY(QVariantList daysOfYear READ daysOfYear WRITE setDaysOfYear NOTIFY valueChanged)
    Q_PROPERTY(QVariantList months READ months WRITE setMonths NOTIFY valueChanged)
    Q_PROPERTY(QVariantList positions READ positions WRITE setPositions NOTIFY valueChanged)
    Q_PROPERTY(Qt::DayOfWeek weekStart READ weekStart WRITE setWeekStart NOTIFY valueChanged)

    Q_ENUMS(Frequency)
    Q_ENUMS(Month)
public:

    QDeclarativeOrganizerItemRecurrenceRule(QObject* parent = 0)
        :QObject(parent)
    {
    }

    enum Frequency {
        Invalid = QOrganizerItemRecurrenceRule::Invalid,
        Daily = QOrganizerItemRecurrenceRule::Daily,
        Weekly = QOrganizerItemRecurrenceRule::Weekly,
        Monthly = QOrganizerItemRecurrenceRule::Monthly,
        Yearly = QOrganizerItemRecurrenceRule::Yearly
    };

    enum Month {
        January = QOrganizerItemRecurrenceRule::January,
        February = QOrganizerItemRecurrenceRule::February,
        March = QOrganizerItemRecurrenceRule::March,
        April = QOrganizerItemRecurrenceRule::April,
        May = QOrganizerItemRecurrenceRule::May,
        June = QOrganizerItemRecurrenceRule::June,
        July = QOrganizerItemRecurrenceRule::July,
        August = QOrganizerItemRecurrenceRule::August,
        September = QOrganizerItemRecurrenceRule::September,
        October = QOrganizerItemRecurrenceRule::October,
        November = QOrganizerItemRecurrenceRule::November,
        December = QOrganizerItemRecurrenceRule::December
    };

    void setFrequency(Frequency freq)
    {
        m_rule.setFrequency(static_cast<QOrganizerItemRecurrenceRule::Frequency>(freq));
    }

    Frequency frequency() const
    {
        return static_cast<Frequency>(m_rule.frequency());
    }

    void setCount(int count)
    {
        m_rule.setCount(count);
    }

    int count() const
    {
        return m_rule.count();
    }

    void setEndDate(const QDate& endDate)
    {
        m_rule.setEndDate(endDate);
    }

    QDate endDate() const
    {
        return m_rule.endDate();
    }

    void setInterval(int interval)
    {
        m_rule.setInterval(interval);
    }

    int interval() const
    {
        return m_rule.interval();
    }

    void setDaysOfWeek(const QVariantList& days)
    {
        QList<Qt::DayOfWeek> saved;
        foreach(QVariant day, days) {
            saved << static_cast<Qt::DayOfWeek>(day.value<int>());
        }
        m_rule.setDaysOfWeek(saved);
    }

    QVariantList daysOfWeek() const
    {
        QVariantList days;
        foreach (Qt::DayOfWeek day, m_rule.daysOfWeek())
            days << day;
        return days;
    }

    void setDaysOfMonth(const QVariantList& days)
    {
        QList<Qt::DayOfWeek> saved;
        foreach(QVariant day, days) {
            saved << static_cast<Qt::DayOfWeek>(day.value<int>());
        }
        m_rule.setDaysOfWeek(saved);
    }

    QVariantList daysOfMonth() const
    {
        QVariantList days;
        foreach (int day, m_rule.daysOfMonth())
            days << day;
        return days;
    }

    void setDaysOfYear(const QVariantList& days)
    {
        QList<int> saved;
        foreach(QVariant day, days) {
            saved << day.value<int>();
        }
        m_rule.setDaysOfYear(saved);
    }

    QVariantList daysOfYear() const
    {
        QVariantList days;
        foreach (int day, m_rule.daysOfYear())
            days << day;
        return days;
    }

    void setMonths(const QVariantList& months)
    {
        QList<QOrganizerItemRecurrenceRule::Month> saved;
        foreach(QVariant day, months) {
            saved << static_cast<QOrganizerItemRecurrenceRule::Month>(day.value<int>());
        }
        m_rule.setMonths(saved);
    }

    QVariantList months() const
    {
        QVariantList ms;
        foreach (int m, m_rule.months())
            ms << m;
        return ms;
    }

    void setWeeksOfYear(const QVariantList& weeks)
    {
        QList<int> saved;
        foreach(QVariant week, weeks) {
            saved << week.value<int>();
        }
        m_rule.setDaysOfYear(saved);
    }

    QVariantList weeksOfYear() const
    {
        QVariantList weeks;
        foreach (int week, m_rule.weeksOfYear())
            weeks << week;
        return weeks;
    }

    void setPositions(const QVariantList& pos)
    {
        QList<int> saved;
        foreach(QVariant p, pos) {
            saved << p.value<int>();
        }
        m_rule.setPositions(saved);
    }

    QVariantList positions() const
    {
        QVariantList pos;
        foreach (int p, m_rule.positions())
            pos << p;
        return pos;
    }

    void setWeekStart(Qt::DayOfWeek day)
    {
        m_rule.setWeekStart(day);
    }

    Qt::DayOfWeek weekStart() const
    {
        return m_rule.weekStart();
    }
    QOrganizerItemRecurrenceRule rule() const
    {
        return m_rule;
    }
signals:
    void valueChanged();
private:
    QOrganizerItemRecurrenceRule m_rule;
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemRecurrenceRule)

#endif
