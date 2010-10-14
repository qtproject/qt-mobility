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

#include "qorganizerrecurrencetransform.h"
#include "qtorganizer.h"

#include <CRecurrence.h>
#include <CRecurrenceRule.h>

QTM_USE_NAMESPACE

OrganizerRecurrenceTransform::OrganizerRecurrenceTransform()
    : m_rtype(0)
{
}

OrganizerRecurrenceTransform::~OrganizerRecurrenceTransform()
{
    beginTransformToCrecurrence(); // frees the resources
}

void OrganizerRecurrenceTransform::beginTransformToCrecurrence()
{
    std::vector< CRecurrenceRule* >::iterator i;
    for (i = m_vRRuleList.begin(); i != m_vRRuleList.end(); ++i)
        delete *i;

    m_rtype = 0;
    m_vRRuleList.clear();
    m_vRecDateList.clear();
    m_vExceptionDateList.clear();
}

CRecurrence* OrganizerRecurrenceTransform::crecurrence(bool *success) const
{
    if (success)
        *success = false;

    if (m_vRRuleList.empty() && m_vRecDateList.empty() && m_vExceptionDateList.empty())
    {
        if (success)
            *success = true;
        return 0; // success, but no recurrence information
    }

    CRecurrence *retn = new CRecurrence();

    // Set the recurrence type
    retn->setRtype(m_rtype);

    // Add the recursion and exception rules
    if (!m_vRRuleList.empty()) {
        // Make a copy of the rules
        std::vector< CRecurrenceRule* > copyOfRules;
        std::vector< CRecurrenceRule* >::const_iterator i;
        for (i = m_vRRuleList.begin(); i != m_vRRuleList.end(); ++i) {
            CRecurrenceRule *curr = *i;
            CRecurrenceRule *copyOfRule = new CRecurrenceRule(*curr);
            copyOfRules.push_back(copyOfRule);
        }

        // Setting the rules transfers ownership of all the rules
        if (!retn->setRecurrenceRule(copyOfRules)) {
            // If failed, the ownership was not transferred
            std::vector< CRecurrenceRule* >::iterator i;
            for (i = copyOfRules.begin(); i != copyOfRules.end(); ++i)
                delete *i;

            delete retn;
            return 0; // failed
        }
    }

    // Set the recursion dates
    if (!m_vRecDateList.empty()) {
        if (!retn->setRDays(m_vRecDateList)) {
            delete retn;
            return 0; // failed
        }
    }

    // Set the exception dates
    if (!m_vExceptionDateList.empty()) {
        if (!retn->setEDays(m_vExceptionDateList)) {
            delete retn;
            return 0; // failed
        }
    }

    if (success)
        *success = true;

    return retn;
}

void OrganizerRecurrenceTransform::addQOrganizerRecurrenceRule(const QOrganizerRecurrenceRule& rule)
{
    QString icalRule = qrecurrenceRuleToIcalRecurrenceRule(rule);

    // Store the new rule to the rule vector
    CRecurrenceRule *crecrule = new CRecurrenceRule();
    crecrule->setRuleType(RECURRENCE_RULE);
    crecrule->setRrule(icalRule.toStdString());
    m_vRRuleList.push_back(crecrule);

    // Update the recursion type, set according to the most frequent rule
    int ruleRtype = qfrequencyToRtype(rule.frequency());
    m_rtype = m_rtype ? ( ruleRtype < m_rtype ? ruleRtype : m_rtype ) : ruleRtype;
}

void OrganizerRecurrenceTransform::addQOrganizerItemExceptionRule(const QOrganizerRecurrenceRule& rule)
{
    QString icalRule = qrecurrenceRuleToIcalRecurrenceRule(rule);

    // Store the new rule to the rule vector
    CRecurrenceRule *crecrule = new CRecurrenceRule();
    crecrule->setRuleType(EXCEPTION_RULE);
    crecrule->setRrule(icalRule.toStdString());
    m_vRRuleList.push_back(crecrule);
}

void OrganizerRecurrenceTransform::addQOrganizerItemRecurrenceDate(const QDate &date)
{
    // Store the new date to the recurrence date vector
    m_vRecDateList.push_back(QString::number(QDateTime(date, QTime(0, 0, 0)).toTime_t()).toStdString());

    if (!m_rtype)
        m_rtype = qfrequencyToRtype(QOrganizerRecurrenceRule::Yearly);
}

void OrganizerRecurrenceTransform::addQOrganizerItemExceptionDate(const QDate &date)
{
    // Store the new date to the exception date vector
    m_vExceptionDateList.push_back(QString::number(QDateTime(date, QTime(0, 0, 0)).toTime_t()).toStdString());
}

QString OrganizerRecurrenceTransform::qrecurrenceRuleToIcalRecurrenceRule(const QOrganizerRecurrenceRule& rule) const
{
    QStringList icalRule;

    icalRule << qfrequencyToIcalFrequency(rule.frequency());

    if (rule.limitCount() > 0)
        icalRule << qcountToIcalCount(rule.limitCount());
    else if (rule.limitDate() != QDate())
        icalRule << qendDateToIcalUntil(rule.limitDate().addDays(-1)); // in Maemo5 the end date is inclusive

    icalRule << qintervalToIcalInterval(rule.interval());

    QSet<QOrganizerRecurrenceRule::Month> months(rule.monthsOfYear());
    if (!months.isEmpty())
        icalRule << qmonthsToIcalByMonth(months);

    QSet<Qt::DayOfWeek> daysOfWeek(rule.daysOfWeek());
    if (!daysOfWeek.isEmpty())
        icalRule << qdaysOfWeekToIcalByDay(daysOfWeek);

    QSet<int> daysOfMonth(rule.daysOfMonth());
    if (!daysOfMonth.isEmpty())
        icalRule << qdaysOfMonthToIcalByMonthDay(daysOfMonth);

    QSet<int> daysOfYear(rule.daysOfYear());
    if (!daysOfYear.isEmpty())
        icalRule << qdaysOfYearToIcalByYearDay(daysOfYear);

    QSet<int> weeksOfYear;
    foreach(int w, rule.weeksOfYear())
        weeksOfYear.insert(w-1);// in Maemo5 the week numbers start from zero

    if (!weeksOfYear.isEmpty())
        icalRule << qweeksOfYearToIcalByWeekNo(weeksOfYear);

    QSet<int> positions(rule.positions());
    if (!positions.isEmpty())
        icalRule << qpositionsToIcalBySetPos(positions);

    icalRule << qweekStartToIcalWkst(rule.firstDayOfWeek());

    return icalRule.join(";");
}

QString OrganizerRecurrenceTransform::qfrequencyToIcalFrequency(QOrganizerRecurrenceRule::Frequency frequency) const
{
    switch( frequency )
    {
    case QOrganizerRecurrenceRule::Daily: return QString("FREQ=DAILY");
    case QOrganizerRecurrenceRule::Weekly: return QString("FREQ=WEEKLY");
    case QOrganizerRecurrenceRule::Monthly: return QString("FREQ=MONTHLY");
    case QOrganizerRecurrenceRule::Yearly: return QString("FREQ=YEARLY");
    default: return QString("FREQ=WEEKLY"); // invalid frequency, assume weekly
    }
}

QString OrganizerRecurrenceTransform::qcountToIcalCount(int count) const
{
    return QString("COUNT=") + QString::number(count);
}

QString OrganizerRecurrenceTransform::qintervalToIcalInterval(int interval) const
{
    return QString("INTERVAL=") + QString::number(interval);
}

QString OrganizerRecurrenceTransform::qendDateToIcalUntil(QDate endDate) const
{
    return QString("UNTIL=") + endDate.toString("yyyyMMdd") + "T235959Z";
}

QString OrganizerRecurrenceTransform::qdaysOfWeekToIcalByDay(const QSet<Qt::DayOfWeek> &daysOfWeek) const
{
    QStringList slist;
    foreach (Qt::DayOfWeek day, daysOfWeek)
        slist << qweekdayToIcalWeekday( day );
    return QString("BYDAY=") + slist.join(",");
}

QString OrganizerRecurrenceTransform::qweekStartToIcalWkst(Qt::DayOfWeek dayOfWeek) const
{
    QString dayOfWeekString(qweekdayToIcalWeekday(dayOfWeek));
    if (dayOfWeekString.isNull())
        dayOfWeekString = QString("MO"); // set Monday as default
    return QString("WKST=") + dayOfWeekString;
}

QString OrganizerRecurrenceTransform::qweekdayToIcalWeekday(Qt::DayOfWeek dayOfWeek) const
{
    QMap<QString, Qt::DayOfWeek> mapping = icalRecurrenceWeekDayQdayOfWeekMapping();
    return mapping.key(dayOfWeek);
}

QString OrganizerRecurrenceTransform::qdaysOfMonthToIcalByMonthDay(const QSet<int> &daysOfMonth) const
{
    return QString("BYMONTHDAY=") + listOfNumbers(daysOfMonth);
}

QString OrganizerRecurrenceTransform::qdaysOfYearToIcalByYearDay(const QSet<int> &daysOfYear) const
{
    return QString("BYYEARDAY=") + listOfNumbers(daysOfYear);
}

QString OrganizerRecurrenceTransform::qmonthsToIcalByMonth(const QSet<QOrganizerRecurrenceRule::Month> &months) const
{
    QSet<int> monthList;
    foreach (QOrganizerRecurrenceRule::Month month, months)
        monthList << static_cast<int>(month);
    return QString("BYMONTH=") + listOfNumbers(monthList);
}

QString OrganizerRecurrenceTransform::qweeksOfYearToIcalByWeekNo(const QSet<int>& weeksOfYear) const
{
    return QString("BYWEEKNO=") + listOfNumbers(weeksOfYear);
}

QString OrganizerRecurrenceTransform::qpositionsToIcalBySetPos(const QSet<int>& positions) const
{
    return QString("BYSETPOS=") + listOfNumbers(positions);
}

QString OrganizerRecurrenceTransform::listOfNumbers(const QSet<int> &list) const
{
    // The calendar backend wants the lists to be sorted
    QList<int> sortedList = list.toList();
    qSort(sortedList.begin(), sortedList.end());

    // Return a list of numbers, separated by comma
    QStringList slist;
    foreach (int number, sortedList)
        slist << QString::number( number );
    return slist.join(",");
}

int OrganizerRecurrenceTransform::qfrequencyToRtype(QOrganizerRecurrenceRule::Frequency frequency) const
{
    // The recursion types the native calendar application
    // uses are found by experiment:
    const int NATIVE_CAL_APP_DAILY = 1;
    const int NATIVE_CAL_APP_WEEKLY = 3;
    const int NATIVE_CAL_APP_MONTHLY = 5;
    const int NATIVE_CAL_APP_YEARLY = 6;

    switch( frequency )
    {
    case QOrganizerRecurrenceRule::Daily: return NATIVE_CAL_APP_DAILY;
    case QOrganizerRecurrenceRule::Weekly: return NATIVE_CAL_APP_WEEKLY;
    case QOrganizerRecurrenceRule::Monthly: return NATIVE_CAL_APP_MONTHLY;
    case QOrganizerRecurrenceRule::Yearly: return NATIVE_CAL_APP_YEARLY;
    default: return NATIVE_CAL_APP_YEARLY;
    }
}

void OrganizerRecurrenceTransform::transformToQrecurrence(CRecurrence *crecurrence)
{
    m_lRecurrenceRules.clear();
    m_lExceptionRules.clear();
    m_lRecurrenceDates.clear();
    m_lExceptionDates.clear();

    if (!crecurrence)
        return;

    // Recurrence and exception rules
    std::vector< CRecurrenceRule* > rules = crecurrence->getRecurrenceRule();
    std::vector< CRecurrenceRule* >::const_iterator i;
    for (i = rules.begin(); i != rules.end(); ++i)
    {
        CRecurrenceRule *rule = *i;
        if (rule)
        {
            QOrganizerRecurrenceRule qrule(icalRecurrenceRuleToQrecurrenceRule(rule));
            if (rule->getRuleType() == RECURRENCE_RULE)
                m_lRecurrenceRules << qrule;
            else if(rule->getRuleType() == EXCEPTION_RULE)
                m_lExceptionRules << qrule;
        }
    }

    // Recurrence dates
    std::vector< std::string > recurrenceDates = crecurrence->getRDays();
    std::vector< std::string >::const_iterator recurrenceDate;
    for (recurrenceDate = recurrenceDates.begin(); recurrenceDate != recurrenceDates.end(); ++recurrenceDate)
        m_lRecurrenceDates << QDateTime::fromTime_t(QString::fromStdString(*recurrenceDate).toInt()).date();

    // Exception dates
    std::vector< std::string > exceptionDates = crecurrence->getEDays();
    std::vector< std::string >::const_iterator exceptionDate;
    for (exceptionDate = exceptionDates.begin(); exceptionDate != exceptionDates.end(); ++exceptionDate)
        m_lExceptionDates << QDateTime::fromTime_t(QString::fromStdString(*exceptionDate).toInt()).date();
}

QSet<QOrganizerRecurrenceRule> OrganizerRecurrenceTransform::recurrenceRules() const
{
    return m_lRecurrenceRules;
}

QSet<QOrganizerRecurrenceRule> OrganizerRecurrenceTransform::exceptionRules() const
{
    return m_lExceptionRules;
}

QSet<QDate> OrganizerRecurrenceTransform::recurrenceDates() const
{
    return m_lRecurrenceDates;
}

QSet<QDate> OrganizerRecurrenceTransform::exceptionDates() const
{
    return m_lExceptionDates;
}

QOrganizerRecurrenceRule OrganizerRecurrenceTransform::icalRecurrenceRuleToQrecurrenceRule(CRecurrenceRule *rule ) const
{
    QOrganizerRecurrenceRule retn;
    // Parse rrule
    if (!rule->rruleParser(rule->getRrule()))
        return retn; // parsing failed

    // Now it's enough to just set the fields
    retn.setFrequency(icalFrequencyToQfrequency(rule->getFrequency()));

    time_t until = rule->getUntil();
    if (until > 0) {
        QDateTime endDate(QDateTime::fromTime_t(until));
        retn.setLimit(endDate.date().addDays(1)); // in Maemo5 the end date is inclusive
    }

    int count = rule->getCount();
    if (count > 0)
        retn.setLimit(count);

    retn.setInterval(rule->getInterval());

    QSet<QOrganizerRecurrenceRule::Month> qMonths;
    std::vector< short > months = rule->getMonth();
    std::vector< short >::const_iterator month;
    for (month = months.begin(); month != months.end(); ++month)
        qMonths << static_cast<QOrganizerRecurrenceRule::Month>(*month);
    retn.setMonthsOfYear(qMonths);

    QSet<Qt::DayOfWeek> qDaysOfWeek;
    std::vector< short > daysOfWeek = rule->getWeekDay();
    std::vector< short >::const_iterator dayOfWeek;
    for (dayOfWeek = daysOfWeek.begin(); dayOfWeek != daysOfWeek.end(); ++dayOfWeek)
        qDaysOfWeek << icalWeekdayToQdayOfWeek(*dayOfWeek);
    retn.setDaysOfWeek(qDaysOfWeek);

    QSet<int> qDaysOfMonth;
    std::vector< short > daysOfMonth = rule->getMonthDay();
    std::vector< short >::const_iterator dayOfMonth;
    for (dayOfMonth = daysOfMonth.begin(); dayOfMonth != daysOfMonth.end(); ++dayOfMonth)
        qDaysOfMonth << static_cast<int>(*dayOfMonth);
    retn.setDaysOfMonth(qDaysOfMonth);

    QSet<int> qDaysOfYear;
    std::vector< short > daysOfYear = rule->getYearDay();
    std::vector< short >::const_iterator dayOfYear;
    for (dayOfYear = daysOfYear.begin(); dayOfYear != daysOfYear.end(); ++dayOfYear)
        qDaysOfYear << static_cast<int>(*dayOfYear);
    retn.setDaysOfYear(qDaysOfYear);

    QSet<int> qWeeksOfYear;
    std::vector< short > weekNumbers = rule->getWeekNumber();
    std::vector< short >::const_iterator weekNumber;
    for (weekNumber = weekNumbers.begin(); weekNumber != weekNumbers.end(); ++weekNumber)
        qWeeksOfYear << static_cast<int>(*weekNumber + 1); // in Maemo5 the week numbers start from zero
    retn.setWeeksOfYear(qWeeksOfYear);

    QSet<int> qPositions;
    std::vector< short > positions = rule->getPos();
    std::vector< short >::const_iterator position;
    for (position = positions.begin(); position != positions.end(); ++position)
        qPositions << static_cast<int>(*position);
    retn.setPositions(qPositions);

    // Parse the start of week here as it does not seem to become
    // parsed correctly with the rruleParser
    QString qrule(QString::fromStdString(rule->getRrule()));
    const QString weekstartField("WKST=");
    int weekstartAt = qrule.indexOf(weekstartField);
    QString weekday;
    if (weekstartAt>=0)
        weekday = qrule.mid(weekstartAt + weekstartField.length(), 2);
    retn.setFirstDayOfWeek(icalRecurrenceTypeWeekdayToQdayOfWeek(weekday));

    return retn;
}

QOrganizerRecurrenceRule::Frequency OrganizerRecurrenceTransform::icalFrequencyToQfrequency(FREQUENCY frequency) const
{
    switch( frequency )
    {
    case DAILY_RECURRENCE: return QOrganizerRecurrenceRule::Daily;
    case WEEKLY_RECURRENCE: return QOrganizerRecurrenceRule::Weekly;
    case MONTHLY_RECURRENCE: return QOrganizerRecurrenceRule::Monthly;
    case YEARLY_RECURRENCE: return QOrganizerRecurrenceRule::Yearly;
    default:
        // No corresponding frequencies defined for the rest of cases
        return QOrganizerRecurrenceRule::Daily;
    }
}

Qt::DayOfWeek OrganizerRecurrenceTransform::icalWeekdayToQdayOfWeek(short weekday, bool *status) const
{
    // The Fremantle calendar representation of weekdays is 1=Sunday, 2=Monday,..., 7=Saturday
    QList<Qt::DayOfWeek> week;
    week << Qt::Sunday << Qt::Monday << Qt::Tuesday << Qt::Wednesday << Qt::Thursday << Qt::Friday << Qt::Saturday;
    if (weekday >= 1 && weekday <= 7) {
        if (status)
            *status = true;
        return week[weekday-1];
    }
    else {
        if (status)
            *status = false;
        return Qt::Monday; // This should never happen and should be interpreted as an error
    }
}

Qt::DayOfWeek OrganizerRecurrenceTransform::icalRecurrenceTypeWeekdayToQdayOfWeek(const QString &weekday) const
{
    QMap<QString, Qt::DayOfWeek> mapping = icalRecurrenceWeekDayQdayOfWeekMapping();
    if (mapping.contains(weekday))
        return mapping.value(weekday);
    else
        return Qt::Monday; // Return Monday as a default start of week
}

QMap<QString, Qt::DayOfWeek> OrganizerRecurrenceTransform::icalRecurrenceWeekDayQdayOfWeekMapping() const
{
    QMap<QString,Qt::DayOfWeek> mapping;
    mapping["MO"] = Qt::Monday;
    mapping["TU"] = Qt::Tuesday;
    mapping["WE"] = Qt::Wednesday;
    mapping["TH"] = Qt::Thursday;
    mapping["FR"] = Qt::Friday;
    mapping["SA"] = Qt::Saturday;
    mapping["SU"] = Qt::Sunday;
    return mapping;
}
