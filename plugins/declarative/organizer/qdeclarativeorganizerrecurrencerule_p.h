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
#include "qorganizerrecurrencerule.h"


QTM_USE_NAMESPACE

class QDeclarativeOrganizerRecurrenceRule : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Frequency frequency READ frequency WRITE setFrequency NOTIFY recurrenceRuleChanged)
    Q_PROPERTY(QVariant limit READ limit WRITE setLimit NOTIFY recurrenceRuleChanged)
    Q_PROPERTY(int interval READ interval WRITE setInterval NOTIFY recurrenceRuleChanged)
    Q_PROPERTY(QVariantList daysOfWeek READ daysOfWeek WRITE setDaysOfWeek NOTIFY recurrenceRuleChanged)
    Q_PROPERTY(QVariantList daysOfMonth READ daysOfMonth WRITE setDaysOfMonth NOTIFY recurrenceRuleChanged)
    Q_PROPERTY(QVariantList daysOfYear READ daysOfYear WRITE setDaysOfYear NOTIFY recurrenceRuleChanged)
    Q_PROPERTY(QVariantList monthsOfYear READ monthsOfYear WRITE setMonthsOfYear NOTIFY recurrenceRuleChanged)
    Q_PROPERTY(QVariantList positions READ positions WRITE setPositions NOTIFY recurrenceRuleChanged)
    Q_PROPERTY(Qt::DayOfWeek firstDayOfWeek READ firstDayOfWeek WRITE setFirstDayOfWeek NOTIFY recurrenceRuleChanged)

    Q_ENUMS(Frequency)
    Q_ENUMS(Month)
public:

    QDeclarativeOrganizerRecurrenceRule(QObject* parent = 0)
        :QObject(parent)
    {
    }

    enum Frequency {
        Invalid = QOrganizerRecurrenceRule::Invalid,
        Daily = QOrganizerRecurrenceRule::Daily,
        Weekly = QOrganizerRecurrenceRule::Weekly,
        Monthly = QOrganizerRecurrenceRule::Monthly,
        Yearly = QOrganizerRecurrenceRule::Yearly
    };

    enum Month {
        January = QOrganizerRecurrenceRule::January,
        February = QOrganizerRecurrenceRule::February,
        March = QOrganizerRecurrenceRule::March,
        April = QOrganizerRecurrenceRule::April,
        May = QOrganizerRecurrenceRule::May,
        June = QOrganizerRecurrenceRule::June,
        July = QOrganizerRecurrenceRule::July,
        August = QOrganizerRecurrenceRule::August,
        September = QOrganizerRecurrenceRule::September,
        October = QOrganizerRecurrenceRule::October,
        November = QOrganizerRecurrenceRule::November,
        December = QOrganizerRecurrenceRule::December
    };

    void setFrequency(Frequency freq)
    {
        if (freq != frequency()) {
            m_rule.setFrequency(static_cast<QOrganizerRecurrenceRule::Frequency>(freq));
            emit recurrenceRuleChanged();
        }
    }

    Frequency frequency() const
    {
        return static_cast<Frequency>(m_rule.frequency());
    }

    void setLimit(const QVariant& value)
    {
        if (value.userType() == QVariant::Date) {
            QDate v = value.value<QDate>();
            if (v != m_rule.limitDate()) {
                m_rule.setLimit(v);
                emit recurrenceRuleChanged();
            }
        }  else if (value.type() == QVariant::Int) {
            int v = value.value<int>();
            if (v != m_rule.limitCount()) {
                m_rule.setLimit(v);
                emit recurrenceRuleChanged();
            }
        } else {
            qWarning() << "Invalid recurrence rule limit value:" << value;
        }
    }

    QVariant limit() const
    {
        if (m_rule.limitType() == QOrganizerRecurrenceRule::CountLimit) {
            return QVariant::fromValue(m_rule.limitCount());
        } else if (m_rule.limitType() == QOrganizerRecurrenceRule::DateLimit) {
            return QVariant::fromValue(m_rule.limitDate());
        }

        //NoLimit
        return QVariant();
    }

    void setInterval(int interval)
    {
        if (interval != m_rule.interval()) {
            m_rule.setInterval(interval);
            emit recurrenceRuleChanged();
        }
    }

    int interval() const
    {
        return m_rule.interval();
    }

    void setDaysOfWeek(const QVariantList& days)
    {
        QSet<Qt::DayOfWeek> saved;
        foreach(QVariant day, days) {
            saved << static_cast<Qt::DayOfWeek>(day.value<int>());
        }
        if (saved != m_rule.daysOfWeek()) {
            m_rule.setDaysOfWeek(saved);
            emit recurrenceRuleChanged();
        }
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
        QSet<int> saved;
        foreach(QVariant day, days) {
            saved << day.value<int>();
        }
        if (saved != m_rule.daysOfMonth()) {
            m_rule.setDaysOfMonth(saved);
            emit recurrenceRuleChanged();
        }
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
        QSet<int> saved;
        foreach(QVariant day, days) {
            saved << day.value<int>();
        }
        if (saved != m_rule.daysOfYear()) {
            m_rule.setDaysOfYear(saved);
            emit recurrenceRuleChanged();
        }
    }

    QVariantList daysOfYear() const
    {
        QVariantList days;
        foreach (int day, m_rule.daysOfYear())
            days << day;
        return days;
    }

    void setMonthsOfYear(const QVariantList& months)
    {
        QSet<QOrganizerRecurrenceRule::Month> saved;
        foreach(QVariant day, months) {
            saved << static_cast<QOrganizerRecurrenceRule::Month>(day.value<int>());
        }
        if (saved != m_rule.monthsOfYear()) {
            m_rule.setMonthsOfYear(saved);
            emit recurrenceRuleChanged();
        }
    }

    QVariantList monthsOfYear() const
    {
        QVariantList ms;
        foreach (int m, m_rule.monthsOfYear())
            ms << m;
        return ms;
    }

    void setWeeksOfYear(const QVariantList& weeks)
    {
        QSet<int> saved;
        foreach(QVariant week, weeks) {
            saved << week.value<int>();
        }
        if (saved != m_rule.weeksOfYear()) {
            m_rule.setWeeksOfYear(saved);
            emit recurrenceRuleChanged();
        }
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
        if (pos != positions()) {
            QSet<int> saved;
            foreach(QVariant p, pos) {
                saved << p.value<int>();
            }
            m_rule.setPositions(saved);
            emit recurrenceRuleChanged();
        }
    }

    QVariantList positions() const
    {
        QVariantList pos;
        foreach (int p, m_rule.positions())
            pos << p;
        return pos;
    }

    void setFirstDayOfWeek(Qt::DayOfWeek day)
    {
        if (day != firstDayOfWeek()) {
            m_rule.setFirstDayOfWeek(day);
            emit recurrenceRuleChanged();
        }
    }

    Qt::DayOfWeek firstDayOfWeek() const
    {
        return m_rule.firstDayOfWeek();
    }
    QOrganizerRecurrenceRule rule() const
    {
        return m_rule;
    }

    void setRule(const QOrganizerRecurrenceRule& rule)
    {
        m_rule = rule;
    }
signals:
    void recurrenceRuleChanged();
private:
    QOrganizerRecurrenceRule m_rule;
};
QML_DECLARE_TYPE(QDeclarativeOrganizerRecurrenceRule)

#endif
