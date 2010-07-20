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
    : m_rtype( 0 )
{
}

OrganizerRecurrenceTransform::~OrganizerRecurrenceTransform()
{
    beginTransformToCrecurrence(); // frees the resources
}

void OrganizerRecurrenceTransform::beginTransformToCrecurrence()
{
    std::vector< CRecurrenceRule* >::iterator i;
    for( i = m_vRRuleList.begin(); i != m_vRRuleList.end(); ++i )
        delete *i;

    m_rtype = 0;
    m_vRRuleList.clear();
    m_vRecDateList.clear();
    m_vExceptionDateList.clear();
}

CRecurrence* OrganizerRecurrenceTransform::crecurrence( bool* success ) const
{
    if ( success )
        *success = false;

    if ( m_vRRuleList.empty() && m_vRecDateList.empty() && m_vExceptionDateList.empty() )
    {
        if ( success )
            *success = true;
        return 0; // success, but no recurrence information
    }

    CRecurrence* retn = new CRecurrence();

    // Set the recurrence type
    retn->setRtype( m_rtype );

    // Add the recursion and exception rules
    if( !m_vRRuleList.empty() ) {
        if( !retn->setRecurrenceRule( m_vRRuleList ) ) {
            delete retn;
            return 0; // failed
        }
    }

    // TODO: recurrence dates & exception dates

    if ( success )
        *success = true;

    return retn;
}

void OrganizerRecurrenceTransform::addQOrganizerItemRecurrenceRule( const QOrganizerItemRecurrenceRule& rule )
{
    QString icalRule = qrecurrenceRuleToIcalRecurrenceRule( rule );

    // Store the new rule to the rule vector
    CRecurrenceRule* crecrule = new CRecurrenceRule();
    crecrule->setRuleType( RECURRENCE_RULE );
    crecrule->setRrule( icalRule.toStdString() );
    m_vRRuleList.push_back( crecrule );

    // Update the recursion type, set according to the most frequent rule
    int ruleRtype = qfrequencyToRtype( rule.frequency() );
    m_rtype = m_rtype ? ( ruleRtype < m_rtype ? ruleRtype : m_rtype ) : ruleRtype;
}

void OrganizerRecurrenceTransform::addQOrganizerItemExceptionRule( const QOrganizerItemRecurrenceRule& rule )
{
    QString icalRule = qrecurrenceRuleToIcalRecurrenceRule( rule );

    // Store the new rule to the rule vector
    CRecurrenceRule* crecrule = new CRecurrenceRule();
    crecrule->setRuleType( EXCEPTION_RULE );
    crecrule->setRrule( icalRule.toStdString() );
    m_vRRuleList.push_back( crecrule );
}

QString OrganizerRecurrenceTransform::qrecurrenceRuleToIcalRecurrenceRule( const QOrganizerItemRecurrenceRule& rule ) const
{
    QStringList icalRule;

    icalRule << qfrequencyToIcalFrequency( rule.frequency() );

    if ( rule.count() > 0 )
        icalRule << qcountToIcalCount( rule.count() );
    else if ( rule.endDate() != QDate() )
        icalRule << qendDateToIcalUntil( rule.endDate() );

    icalRule << qintervalToIcalInterval( rule.interval() );

    QList<QOrganizerItemRecurrenceRule::Month> months( rule.months() );
    if( !months.isEmpty() )
        icalRule << qmonthsToIcalByMonth( months );

    QList<Qt::DayOfWeek> daysOfWeek( rule.daysOfWeek() );
    if( !daysOfWeek.isEmpty() )
        icalRule << qdaysOfWeekToIcalByDay( daysOfWeek );

    QList<int> daysOfMonth( rule.daysOfMonth() );
    if( !daysOfMonth.isEmpty() )
        icalRule << qdaysOfMonthToIcalByMonthDay( daysOfMonth );

    QList<int> daysOfYear( rule.daysOfYear() );
    if( !daysOfYear.isEmpty() )
        icalRule << qdaysOfYearToIcalByYearDay( daysOfYear );

    QList<int> positions( rule.positions() );
    if( !positions.isEmpty() )
        icalRule << qpositionsToIcalBySetPos( positions );

    icalRule << qweekStartToIcalWkst( rule.weekStart() );

    return icalRule.join(";");
}

QString OrganizerRecurrenceTransform::qfrequencyToIcalFrequency( QOrganizerItemRecurrenceRule::Frequency frequency ) const
{
    switch( frequency )
    {
    case QOrganizerItemRecurrenceRule::Daily: return QString("FREQ=DAILY");
    case QOrganizerItemRecurrenceRule::Weekly: return QString("FREQ=WEEKLY");
    case QOrganizerItemRecurrenceRule::Monthly: return QString("FREQ=MONTHLY");
    case QOrganizerItemRecurrenceRule::Yearly: return QString("FREQ=YEARLY");
    }
}

QString OrganizerRecurrenceTransform::qcountToIcalCount( int count ) const
{
    return QString("COUNT=") + QString::number(count);
}

QString OrganizerRecurrenceTransform::qintervalToIcalInterval( int interval ) const
{
    return QString("INTERVAL=") + QString::number( interval );
}

QString OrganizerRecurrenceTransform::qendDateToIcalUntil( QDate endDate ) const
{
    // TODO: Check the correctness of this conversion, is there any situation when this is not correct?
    return QString("UNTIL=") + endDate.toString("yyyyMMdd") + "T235959Z";
}

QString OrganizerRecurrenceTransform::qdaysOfWeekToIcalByDay( const QList<Qt::DayOfWeek>& daysOfWeek ) const
{
    QStringList slist;
    foreach( Qt::DayOfWeek day, daysOfWeek )
        slist << qweekdayToIcalWeekday( day );
    return QString("BYDAY=") + slist.join(",");
}

QString OrganizerRecurrenceTransform::qweekStartToIcalWkst( Qt::DayOfWeek dayOfWeek ) const
{
    QString dayOfWeekString( qweekdayToIcalWeekday( dayOfWeek ) );
    if ( dayOfWeekString.isNull() )
        dayOfWeekString = QString("MO"); // set Monday as default
    return QString("WKST=") + dayOfWeekString;
}

QString OrganizerRecurrenceTransform::qweekdayToIcalWeekday( Qt::DayOfWeek dayOfWeek ) const
{
    switch( dayOfWeek )
    {
    case Qt::Monday: return QString("MO");
    case Qt::Tuesday: return QString("TU");
    case Qt::Wednesday: return QString("WE");
    case Qt::Thursday: return QString("TH");
    case Qt::Friday: return QString("FR");
    case Qt::Saturday: return QString("SA");
    case Qt::Sunday: return QString("SU");
    default: return QString();
    }
}

QString OrganizerRecurrenceTransform::qdaysOfMonthToIcalByMonthDay( const QList<int>& daysOfMonth ) const
{
    return QString("BYMONTHDAY=") + listOfNumbers( daysOfMonth );
}

QString OrganizerRecurrenceTransform::qdaysOfYearToIcalByYearDay( const QList<int>& daysOfYear ) const
{
    return QString("BYYEARDAY=") + listOfNumbers( daysOfYear );
}

QString OrganizerRecurrenceTransform::qmonthsToIcalByMonth( const QList<QOrganizerItemRecurrenceRule::Month>& months ) const
{
    QList<int> monthList;
    foreach( QOrganizerItemRecurrenceRule::Month month, months )
        monthList << static_cast<int>( month );
    return QString("BYMONTH=") + listOfNumbers( monthList );
}

QString OrganizerRecurrenceTransform::qweeksOfYearToIcalByWeekNo( const QList<int>& weeksOfYear ) const
{
    return QString("BYWEEKNO=") + listOfNumbers( weeksOfYear );
}

QString OrganizerRecurrenceTransform::qpositionsToIcalBySetPos( const QList<int>& positions ) const
{
    return QString("BYSETPOS=") + listOfNumbers( positions );
}

QString OrganizerRecurrenceTransform::listOfNumbers( const QList<int>& list ) const
{
    // The calendar backend wants the lists to be sorted
    QList<int> sortedList = list;
    qSort(sortedList.begin(), sortedList.end());

    // Return a list of numbers, separated by comma
    QStringList slist;
    foreach( int number, sortedList )
        slist << QString::number( number );
    return slist.join(",");
}

int OrganizerRecurrenceTransform::qfrequencyToRtype( QOrganizerItemRecurrenceRule::Frequency frequency ) const
{
    // The recursion types the native calendar application
    // uses are found by experiment:
    const int NATIVE_CAL_APP_DAILY = 1;
    const int NATIVE_CAL_APP_WEEKLY = 3;
    const int NATIVE_CAL_APP_MONTHLY = 5;
    const int NATIVE_CAL_APP_YEARLY = 6;

    switch( frequency )
    {
    case QOrganizerItemRecurrenceRule::Daily: return NATIVE_CAL_APP_DAILY;
    case QOrganizerItemRecurrenceRule::Weekly: return NATIVE_CAL_APP_WEEKLY;
    case QOrganizerItemRecurrenceRule::Monthly: return NATIVE_CAL_APP_MONTHLY;
    case QOrganizerItemRecurrenceRule::Yearly: return NATIVE_CAL_APP_YEARLY;
    }
}
