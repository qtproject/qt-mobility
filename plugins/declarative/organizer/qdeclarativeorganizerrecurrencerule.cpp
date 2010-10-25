/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "qdeclarativeorganizerrecurrencerule_p.h"

/*!
    \qmlclass RecurrenceRule QDeclarativeOrganizerRecurrenceRule

    \brief The RecurrenceRule element represents a rule by which a organizer item repeats.

    \ingroup qml-organizer


  \sa QOrganizerRecurrenceRule

  The RecurrenceRule element is part of the \bold{QtMobility.organizer 1.1} module.
*/

/*!
  \qmlproperty enumeration RecurrenceRule::frequency

  This property holds the frequency with which the item recurs, the value can be one of:
  \list
  \o RecurrenceRule.Invalid - (default).
  \o RecurrenceRule.Daily
  \o RecurrenceRule.Weekly
  \o RecurrenceRule.Monthly
  \o RecurrenceRule.Yearly
 \endlist
  */

/*!
  \qmlproperty variant RecurrenceRule::limit

  This property holds the limit condition of the recurrence rule, the value can be a limit date and time or a limit count.
  The default is no limit.
  */
/*!
  \qmlproperty int RecurrenceRule::interval

  This property holds the interval of recurrence.  The default interval is 1.
  */
/*!
  \qmlproperty list<variant> RecurrenceRule::daysOfWeek

  This property holds a list of the days of week that the item should recur on.
  */
/*!
  \qmlproperty list<variant> RecurrenceRule::daysOfMonth

  This property holds a list of the days of the month that the item should recur on.
  */
/*!
  \qmlproperty list<variant> RecurrenceRule::daysOfYear

  This property holds a list of the days of the year that the item should recur on.
  */
/*!
  \qmlproperty list<int> RecurrenceRule::monthsOfYear

  This property holds a list of the months that the item should recur on, the list element value can be one of:
  \list
  \o RecurrenceRule.January
  \o RecurrenceRule.February
  \o RecurrenceRule.March
  \o RecurrenceRule.April
  \o RecurrenceRule.May
  \o RecurrenceRule.June
  \o RecurrenceRule.July
  \o RecurrenceRule.August
  \o RecurrenceRule.September
  \o RecurrenceRule.October
  \o RecurrenceRule.November
  \o RecurrenceRule.December
  \endlist
  */

/*!
  \qmlproperty list<int> RecurrenceRule::weeksOfYear

  This property holds a list of the weeks of the year that the item should recur on.
  */
/*!
  \qmlproperty list<int> RecurrenceRule::positions

  This property holds the position-list of the recurrence rule.
  */
/*!
  \qmlproperty enumeration RecurrenceRule::firstDayOfWeek

  This property holds the day that the week starts on.  If not set, this is Monday. The value can be one of:
  \list
  \o Qt.Monday
  \o Qt.Tuesday
  \o Qt.Wednesday
  \o Qt.Thursday
  \o Qt.Friday
  \o Qt.Saturday
  \o Qt.Sunday
  \endlist
  */
