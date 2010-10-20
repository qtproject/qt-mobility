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
