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
#include <QDebug>

QTM_USE_NAMESPACE

OrganizerRecurrenceTransform::OrganizerRecurrenceTransform()
{
}

OrganizerRecurrenceTransform::~OrganizerRecurrenceTransform()
{
    beginTransformToCrecurrence(); // clears the lists
}

void OrganizerRecurrenceTransform::beginTransformToCrecurrence()
{
    //qDebug() << "OrganizerRecurrenceTransform::beginTransformToCrecurrence";

    std::vector< CRecurrenceRule* >::iterator i;
    for( i = m_vRRuleList.begin(); i != m_vRRuleList.end(); ++i )
        delete *i;
    for( i = m_vERuleList.begin(); i != m_vERuleList.end(); ++i )
        delete *i;

    m_vRRuleList.clear();
    m_vERuleList.clear();
    m_vRecDateList.clear();
    m_vExceptionDateList.clear();
}

CRecurrence* OrganizerRecurrenceTransform::crecurrence( bool* success ) const
{
    // TODO: Not completed yet

    if ( success )
        *success = false;

    if ( m_vRRuleList.empty() && m_vERuleList.empty() && m_vRecDateList.empty() && m_vExceptionDateList.empty() )
    {
        if ( success )
            *success = true;
        return 0; // success, but no recurrence information
    }

    CRecurrence* retn = new CRecurrence();
    // Add recursion rules
    if( !m_vRRuleList.empty() ) {
        if( !retn->setRecurrenceRule( m_vRRuleList ) ) {
            qDebug() << "setRRule failed";
            delete retn;
            return 0; // failed
        }

        std::vector< CRecurrenceRule* > tmp = retn->getRecurrenceRule();
        qDebug() << "Recurrence rules count: " << tmp.size();
        if( tmp.size() > 0 )
            qDebug() << QString::fromStdString(tmp[0]->toString());
    }

    if ( success )
        *success = true;

    return retn;
}

void OrganizerRecurrenceTransform::addQOrganizerItemRecurrenceRule( const QOrganizerItemRecurrenceRule& rule )
{
    qDebug() << "OrganizerRecurrenceTransform::addQOrganizerItemRecurrenceRule";

    CRecurrenceRule* crecrule = new CRecurrenceRule();

    crecrule->setFrequency( qfrequencyToCfrequency( rule.frequency() ) );

    if ( rule.count() > 0 )
        crecrule->setCount( rule.count() );
    else if ( rule.endDate() != QDate() )
        crecrule->setUntil( QDateTime( rule.endDate(), QTime( 23, 59, 59 ) ).toTime_t() );

    crecrule->setInterval( rule.interval() );

    crecrule->setCount( 10 ); // TEST, remove
    crecrule->setInterval( 1 ); // TEST, remove
    crecrule->setUntil( QDateTime( QDate( 2010, 8, 1), QTime( 23, 59, 59) ).toTime_t() ); // TEST, remove

    qDebug() << "Recurrence rule " << QString::fromStdString( crecrule->toString() );

    m_vRRuleList.push_back( crecrule );
}

FREQUENCY OrganizerRecurrenceTransform::qfrequencyToCfrequency( QOrganizerItemRecurrenceRule::Frequency frequency ) const
{
    switch( frequency )
    {
    case QOrganizerItemRecurrenceRule::Daily: return DAILY_RECURRENCE;
    case QOrganizerItemRecurrenceRule::Weekly: return WEEKLY_RECURRENCE;
    case QOrganizerItemRecurrenceRule::Monthly: return MONTHLY_RECURRENCE;
    case QOrganizerItemRecurrenceRule::Yearly: return YEARLY_RECURRENCE;
    }
}
