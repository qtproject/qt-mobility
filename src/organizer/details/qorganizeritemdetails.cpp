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

#include "qorganizeritemdetails.h"
#include "qorganizeritemdetailfilter.h"

QTM_BEGIN_NAMESPACE








/* ==================== QOrganizerItemDescription ======================= */
/*!
   \class QOrganizerItemDescription
   \brief The QOrganizerItemDescription class contains some arbitrary information which is relevant to the organizer item.
   \inmodule QtOrganizer
   \ingroup organizer-details
 */

/*!
   \variable QOrganizerItemDescription::DefinitionName
   The constant string which identifies the definition of details which are long descriptions.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemDescription::DefinitionName, "Description");

/*!
   \variable QOrganizerItemDescription::FieldDescription

   The constant key for which the description value is stored in details of
   the QOrganizerItemDescription type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemDescription::FieldDescription, "Description");

/*!
   \fn QOrganizerItemDescription::setDescription(const QString& description)
   Sets a description associated with an organizer item to \a description.
 */

/*!
   \fn QOrganizerItemDescription::description() const
   Returns a string for a description associated with an organizer item.
 */







/* ==================== QOrganizerItemDisplayLabel ======================= */
/*!
   \class QOrganizerItemDisplayLabel
   \brief The QOrganizerItemDisplayLabel class contains the backend-synthesized display label of the organizer item.
   \inmodule QtOrganizer
   \ingroup organizer-details
*/

/*!
    Returns a filter suitable for finding items with a display label containing the specified
    \a substring.
*/
QOrganizerItemFilter QOrganizerItemDisplayLabel::match(const QString &substring)
{
    QOrganizerItemDetailFilter f;
    f.setDetailDefinitionName(QOrganizerItemDisplayLabel::DefinitionName,
                              QOrganizerItemDisplayLabel::FieldLabel);
    f.setValue(substring);
    f.setMatchFlags(QOrganizerItemFilter::MatchContains);

    return f;
}

/*!
   \fn void QOrganizerItemDisplayLabel::setLabel(const QString& label)
   Sets the display label of the organizer item to \a label.
 */

/*!
   \fn QString QOrganizerItemDisplayLabel::label() const
   Returns the display label of the organizer item.
 */

/*!
   \variable QOrganizerItemDisplayLabel::DefinitionName

   The constant string which identifies the definition of details
   which contain a display label of an organizer item.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemDisplayLabel::DefinitionName, "DisplayLabel");

/*!
   \variable QOrganizerItemDisplayLabel::FieldLabel

   The constant key for which the display label value is stored in
   details of the QOrganizerItemDisplayLabel type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemDisplayLabel::FieldLabel, "Label");







/* ==================== QOrganizerEventTimeRange ======================= */
/*!
   \class QOrganizerEventTimeRange
   \brief The QOrganizerEventTimeRange class contains the start and end dates and times of a recurring event series,
          or occurrence of an event.
   \inmodule QtOrganizer
   \ingroup organizer-details
*/

/*!
   \variable QOrganizerEventTimeRange::DefinitionName

   The constant string which identifies the definition of details
   which contain a start and end timestamps of an event series or event occurrence.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerEventTimeRange::DefinitionName, "EventTimeRange");

/*!
   \variable QOrganizerEventTimeRange::FieldEndDateTime

   The constant key for which the end date and time value is stored in
   details of the QOrganizerEventTimeRange type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerEventTimeRange::FieldEndDateTime, "EndDateTime");

/*!
   \variable QOrganizerEventTimeRange::FieldStartDateTime

   The constant key for which the start date and time value is stored in
   details of the QOrganizerEventTimeRange type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerEventTimeRange::FieldStartDateTime, "StartDateTime");

/*!
   \variable QOrganizerEventTimeRange::FieldTimeSpecified

   The constant key for the specification of whether the time is significant in the
   start datetime of the QOrganizerEventTimeRange type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerEventTimeRange::FieldTimeSpecified, "TimeSpecified");


/*!
    \fn QOrganizerEventTimeRange::startDateTime() const
    Returns the event timerange's start date and time as QDateTime.
 */

/*!
    \fn QOrganizerEventTimeRange::setStartDateTime(const QDateTime& startDateTime)
    Sets the event timerange's start date and time to \a startDateTime.
 */

/*!
    \fn QOrganizerEventTimeRange::endDateTime() const
    Returns the event timerange's due date and time as QDateTime.
 */

/*!
    \fn QOrganizerEventTimeRange::setEndDateTime(const QDateTime& dueDateTime)
    Sets the event timerange's due date and time to \a dueDateTime.
 */

/*!
    \fn QOrganizerEventTimeRange::setTimeSpecified(bool isTimeSpecified)
    Sets the event should be specified through \a isTimeSpecified.
 */

/*!
    \fn QOrganizerEventTimeRange::isTimeSpecified()
    return true if the time is specified.
 */
/* ==================== QOrganizerItemGuid ======================= */

/*!
   \class QOrganizerItemGuid
   \brief The QOrganizerItemGuid class contains the globally unique identifier of the organizer item, which can be used for synchronization purposes.
   \inmodule QtOrganizer
   \ingroup organizer-details
 */

/*!
   \variable QOrganizerItemGuid::DefinitionName
   The constant string which identifies the definition of details which are globally unique identifiers.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemGuid::DefinitionName, "Guid");

/*!
   \variable QOrganizerItemGuid::FieldGuid

   The constant key for which the globally unique identifier value is
   stored in details of the QOrganizerItemGuid type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemGuid::FieldGuid, "Guid");

/*!
   \fn QOrganizerItemGuid::guid() const

   Returns the globally unique identifier which is stored in this detail.
 */

/*!
   \fn QOrganizerItemGuid::setGuid(const QString& guid)
   Sets the globally unique identifier which is stored in this detail to \a guid.
 */






/* ==================== QOrganizerItemInstanceOrigin.h ======================= */
/*!
   \class QOrganizerItemInstanceOrigin
   \brief The QOrganizerItemInstanceOrigin class contains information about when and from where or whom the organizer item originated.
   \inmodule QtOrganizer
   \ingroup organizer-details
 */

/*!
   \variable QOrganizerItemInstanceOrigin::DefinitionName
   The constant string which identifies the definition of details which contain information which identifies
   the recurrent event of which the organizer item is a particular instance or exception of.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemInstanceOrigin::DefinitionName, "InstanceOrigin");

/*!
   \variable QOrganizerItemInstanceOrigin::FieldParentLocalId

   The constant key for the field in which the local id of the parent recurrent event
   is stored in details of the QOrganizerItemInstanceOrigin type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemInstanceOrigin::FieldParentLocalId, "ParentLocalId");


/*!
   \variable QOrganizerItemInstanceOrigin::FieldOriginalDate

   The constant key for the field in which the original date of this instance
   or exception according to the recurrent series of the parent item is stored.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemInstanceOrigin::FieldOriginalDate, "OriginalDate");

/*!
   \fn QOrganizerItemInstanceOrigin::parentLocalId() const
    Returns the local id of the item instance origin's parent item.
 */

/*!
   \fn QOrganizerItemInstanceOrigin::setParentLocalId(const QOrganizerItemLocalId& parentId)
    Sets the parent id of this instance origin item to \a parentId.
 */

/*!
   \fn QOrganizerItemInstanceOrigin::originalDate() const
    Returns the original date of this instance origin item.
 */

/*!
   \fn QOrganizerItemInstanceOrigin::setOriginalDate(const QDate& date)
    Sets the origin date to \a date.
 */


/* ==================== QOrganizerJournalTimeRange ======================= */
// XXX TODO: time range or just single instant in time?
/*!
   \class QOrganizerJournalTimeRange

   \brief The QOrganizerJournalTimeRange class contains information about
   the date and time for which a journal entry has been created.
   \inmodule QtOrganizer
   \ingroup organizer-details
 */

/*!
\variable QOrganizerJournalTimeRange::DefinitionName

The constant string which identifies the definition of details
which describe the date and time associated with a journal item.
*/
Q_DEFINE_LATIN1_CONSTANT(QOrganizerJournalTimeRange::DefinitionName, "JournalTimeRange");

/*!
   \variable QOrganizerJournalTimeRange::FieldEntryDateTime

   The constant key for which the date and time of value for a journal entry is
   stored in details of the QOrganizerJournalTimeRange type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerJournalTimeRange::FieldEntryDateTime, "EntryDateTime");

/*!
   \fn QOrganizerJournalTimeRange::entryDateTime() const
   Returns the journal entry date and time as QDateTime.
 */

/*!
   \fn QOrganizerJournalTimeRange::setEntryDateTime(const QDateTime& entryDateTime)
   Sets the journal entry date and time to \a entryDateTime.
 */

/* ==================== QOrganizerItemLocation ======================= */
// XXX TODO: do we include all three pieces of data into a single detail
// ie, street address + geolocation + location string
// Or, do we have three separate detail types?
/*!
   \class QOrganizerItemLocation

   \brief The QOrganizerItemLocation class contains information about a location which is related to the organizer item in some manner.
   \inmodule QtOrganizer
   \ingroup organizer-details
 */


/*!
    Returns a filter suitable for finding items whose location matches the specified \a substring.
    This filter matches location names only.  If you wish to match against address or geolocation
    co-ordinates, use a QContactDetailFilter instead.
*/
QOrganizerItemFilter QOrganizerItemLocation::match(const QString &substring)
{
    QOrganizerItemDetailFilter f;
    f.setDetailDefinitionName(QOrganizerItemLocation::DefinitionName,
                              QOrganizerItemLocation::FieldLocationName);
    f.setValue(substring);
    f.setMatchFlags(QOrganizerItemFilter::MatchContains);

    return f;
}

/*!
\variable QOrganizerItemLocation::DefinitionName

The constant string which identifies the definition of details
which describe a location associated with an organizer item.
*/
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemLocation::DefinitionName, "Location");

/*!
   \variable QOrganizerItemLocation::FieldGeoLocation

   The constant key for which the geolocation value is
   stored in details of the QOrganizerItemLocation type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemLocation::FieldGeoLocation, "GeoLocation");

/*!
   \variable QOrganizerItemLocation::FieldAddress

   The constant key for which the location address value is
   stored in details of the QOrganizerItemLocation type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemLocation::FieldAddress, "Address");

/*!
   \variable QOrganizerItemLocation::FieldLocationName

   The constant key for which the location label value is
   stored in details of the QOrganizerItemLocation type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemLocation::FieldLocationName, "LocationName");

/*!
    \fn QOrganizerItemLocation::geoLocation() const
    Returns a string of geolication value.
 */

/*!
    \fn QOrganizerItemLocation::setGeoLocation(const QString& stringCoords)
    Sets geolication value to \a stringCoords.
 */

/*!
    \fn QOrganizerItemLocation::locationName() const
    Returns a string of location name.
 */

/*!
   \fn QOrganizerItemLocation::setLocationName(const QString& locationName)
   Sets location name to \a locationName.
 */

/*!
    \fn QOrganizerItemLocation::address() const
    Returns a string of address name.
 */

/*!
   \fn QOrganizerItemLocation::setAddress(const QString& address)
   Sets address name to \a address.
 */

/* ==================== QOrganizerItemComment ======================= */
/*!
   \class QOrganizerItemComment
   \brief The QOrganizerItemComment class contains some arbitrary information which is relevant to the organizer item.
   \inmodule QtOrganizer
   \ingroup organizer-details
 */

/*!
    Returns a filter suitable for finding items with a comment containing the specified
    \a substring.
*/
QOrganizerItemFilter QOrganizerItemComment::match(const QString &substring)
{
    QOrganizerItemDetailFilter f;
    f.setDetailDefinitionName(QOrganizerItemComment::DefinitionName,
                              QOrganizerItemComment::FieldComment);
    f.setValue(substring);
    f.setMatchFlags(QOrganizerItemFilter::MatchContains);

    return f;
}

/*!
   \variable QOrganizerItemComment::DefinitionName
   The constant string which identifies the definition of details which are comments.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemComment::DefinitionName, "Comment");

/*!
   \variable QOrganizerItemComment::FieldComment

   The constant key for which the comment value is stored in details of
   the QOrganizerItemComment type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemComment::FieldComment, "Comment");

/*!
   \fn QOrganizerItemComment::setComment(const QString& comment)
   Sets a comment associated with an organizer item to \a comment.
 */

/*!
   \fn QOrganizerItemComment::comment() const
   Returns a string for a comment associated with an organizer item.
 */



/* ==================== QOrganizerItemPriority ======================= */
/*!
   \class QOrganizerItemPriority
   \brief The QOrganizerItemPriority class contains the priority of the organizer item, which may be used to resolve scheduling conflicts.
   \inmodule QtOrganizer
   \ingroup organizer-details
 */

/*!
    Returns a filter suitable for finding items of the specified \a priority.
*/
QOrganizerItemFilter QOrganizerItemPriority::match(QOrganizerItemPriority::Priority priority)
{
    QOrganizerItemDetailFilter f;
    f.setDetailDefinitionName(QOrganizerItemPriority::DefinitionName,
                              QOrganizerItemPriority::FieldPriority);
    f.setValue(priority);
    f.setMatchFlags(QOrganizerItemFilter::MatchExactly);

    return f;
}

/*!
   \enum QOrganizerItemPriority::Priority
   \value UnknownPriority
   \value HighestPriority
   \value ExtremelyHighPriority
   \value VeryHighPriority
   \value HighPriority
   \value MediumPriority
   \value LowPriority
   \value VeryLowPriority
   \value ExtremelyLowPriority
   \value LowestPriority
*/

/*!
   \variable QOrganizerItemPriority::DefinitionName
   The constant string which identifies the definition of details which contain the priority of an organizer item.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemPriority::DefinitionName, "Priority");

/*!
   \variable QOrganizerItemPriority::FieldPriority

   The constant key for which the priority value is stored in details of
   the QOrganizerItemPriority type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemPriority::FieldPriority, "Priority");

/*!
   \fn QOrganizerItemPriority::setPriority(Priority priority)

   Sets the priority associated with an organizer item to \a priority.
 */

/*!
   \fn Priority QOrganizerItemPriority::priority() const

   Returns the priority associated with an organizer item.
 */





/* ==================== QOrganizerItemRecurrence ======================= */
/*!
   \class QOrganizerItemRecurrence
   \brief The QOrganizerItemRecurrence class contains a list of rules and dates on which the recurrent item occurs,
          and a list of rules and dates on which exceptions occur.
   \inmodule QtOrganizer
   \ingroup organizer-details
 */

/*!
   \variable QOrganizerItemRecurrence::DefinitionName
   The constant string which identifies the definition of details which are organizer item recurrence specifications.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemRecurrence::DefinitionName, "Recurrence");

/*!
   \variable QOrganizerItemRecurrence::FieldRecurrenceRules

   The constant key for the value which is stored in details of the
   QOrganizerItemRecurrence type which describes the rules for when an
   item should recur.

   When multiple rules are specified, the list of recurrence dates are
   calculated separately for each rule and the results are unioned.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemRecurrence::FieldRecurrenceRules, "RecurrenceRules");

/*!
   \variable QOrganizerItemRecurrence::FieldRecurrenceDates

   The constant key for the value which is stored in details of the
   QOrganizerItemRecurrence type which describes the dates on which an
   item should recur.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemRecurrence::FieldRecurrenceDates, "RecurrenceDates");

/*!
   \variable QOrganizerItemRecurrence::FieldExceptionRules

   The constant key for the value which is stored in details of the
   QOrganizerItemRecurrence type which describes the rules for when an
   item should not recur.

   If a recurrence rule or the recurrence dates list specifies that an item should occur on a
   particular date and any of the exception rules include that date, the item should not occur on
   that date.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemRecurrence::FieldExceptionRules, "ExceptionRules");

/*!
   \variable QOrganizerItemRecurrence::FieldExceptionDates

   The constant key for the value which is stored in details of the
   QOrganizerItemRecurrence type which describes the dates on which an
   item should not recur.

   If a recurrence rule or the recurrence dates list specifies that an item should occur on a
   particular date and that date appears in the exception dates list, the item should not occur on
   that date.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemRecurrence::FieldExceptionDates, "ExceptionDates");

/*!
   \fn QOrganizerItemRecurrence::recurrenceDates() const
   Returns a list of recurrence dates.
 */

/*!
   \fn QOrganizerItemRecurrence::setRecurrenceDates(const QList<QDate>& rdates)
   Sets a list of recurrence dates to \a rdates.
 */

/*!
   \fn QOrganizerItemRecurrence::exceptionRules() const
   Returns a list of exception rules.
 */

/*!
   \fn QOrganizerItemRecurrence::setExceptionRules(const QList<QOrganizerItemRecurrenceRule>& xrules)
   Sets a list of exception rules to \a xrules.
 */

/*!
   \fn QOrganizerItemRecurrence::recurrenceRules() const
   Returns a list of recurrence rules.
 */

/*!
   \fn QOrganizerItemRecurrence::setRecurrenceRules(const QList<QOrganizerItemRecurrenceRule>& rrules)
   Sets a list of recurrence rules to \a rrules.
 */

/*!
   \fn QOrganizerItemRecurrence::exceptionDates() const
   Returns a list of exception dates.
 */

/*!
   \fn QOrganizerItemRecurrence::setExceptionDates(const QList<QDate>& exdates)
   Sets a list of exception dates to \a exdates.
 */



/* ==================== QOrganizerItemReminder ======================= */
/*!
   \class QOrganizerItemReminder
   \brief The QOrganizerItemReminder class contains information about when and how the user wants to reminded of the item
   \inmodule QtOrganizer
   \ingroup organizer-details

   Note that the Organizer API does not enforce that the user is reminded of the item;
   rather, it simply allows clients to store and manipulate data which might be used
   by the platform to implement alarms and reminders.
 */

/*!
   \enum QOrganizerItemReminder::ReminderType
   \value NoReminder This reminder is entirely unobtrusive
   \value AudibleReminder This reminder has an audible element
   \value VisualReminder This reminder has a visual element
   \value EmailReminder This reminder has a email element
*/

/*! 
   \fn QOrganizerItemReminder::QOrganizerItemReminder(const char* definitionName)

    Constructor of a QOrganizerItemReminder object by defining the \a definitionName. 
*/

/*! 
    \fn QOrganizerItemReminder::QOrganizerItemReminder(const QOrganizerItemDetail& detail, const char* definitionName)

    Constructor of a QOrganizerItemReminder object by defining the \a detail, and \a definitionName. 
*/

/*!
   \variable QOrganizerItemReminder::DefinitionName
   The constant string which identifies the definition of details which contain reminder information of an organizer item.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemReminder::DefinitionName, "Reminder");

/*!
   \variable QOrganizerItemReminder::FieldReminderType

   The constant key for which the reminder type value is stored in details of
   the QOrganizerItemReminder type (and its subclasses).
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemReminder::FieldReminderType, "ReminderType");

/*!
   \variable QOrganizerItemReminder::FieldTimeDelta

   The constant key for which time delta (in seconds prior to the item activation time)
   at which the user should be reminded of the item is stored in details of the QOrganizerItemReminder type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemReminder::FieldTimeDelta, "TimeDelta");

/*!
   \variable QOrganizerItemReminder::FieldDateTime

   The constant key for which the date time at which the user should be reminded of the item
   is stored in details of the QOrganizerItemReminder type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemReminder::FieldDateTime, "DateTime");

/*!
   \variable QOrganizerItemReminder::FieldRepetitionCount

   The constant key for which the number of repetitions of the reminder
   is stored in details of the QOrganizerItemReminder type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemReminder::FieldRepetitionCount, "RepetitionCount");

/*!
   \variable QOrganizerItemReminder::FieldRepetitionDelay

   The constant key for which the delay (in seconds) between repetitions of the reminder
   is stored in details of the QOrganizerItemReminder type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemReminder::FieldRepetitionDelay, "RepetitionDelay");

/*!
   \fn ReminderTypes QOrganizerItemReminder::reminderType() const

   Returns the reminder type of this reminder for an organizer item.
 */

/*!
   \fn void QOrganizerItemReminder::setRepetition(int count, int delaySeconds)

   Sets the repetition by using \a count and, \a delaySeconds.
 */

/*!
   \fn QOrganizerItemReminder::setTimeDelta(int secondsBefore)

   Sets the number of seconds prior to the activation of the item
   at which the user wants to be reminded of the item to \a secondsBefore.
   The exact datetime of activation of the item depends on the type of
   item: for a QOrganizerTodo or QOrganizerTodoOccurrence it is the
   due date time; for a QOrganizerEvent or QOrganizerEventOccurrence
   it is the start date time.

   Calling this function will clear the absolute date time, if set.

   \sa setDateTime()
 */

/*!
   \fn int QOrganizerItemReminder::timeDelta() const

   Returns the number of seconds prior to the activation of the item
   at which the user wants to be reminded of the item.
   The exact datetime of activation of the item depends on the type of
   item: for a QOrganizerTodo or QOrganizerTodoOccurrence it is the
   due date time; for a QOrganizerEvent or QOrganizerEventOccurrence
   it is the start date time.
 */

/*!
   \fn QOrganizerItemReminder::setDateTime(const QDateTime& dateTime)

   Sets the date time at which the user should be reminded of the item
   to \a dateTime.

   Calling this function will clear the time delta, if set.

   \sa setTimeDelta()
*/

/*!
   \fn QDateTime QOrganizerItemReminder::dateTime() const

   Returns the date time at which the user should be reminded of the item.
*/

/*!
   \fn int QOrganizerItemReminder::repetitionCount() const

   Returns the number of times the user should be reminded of the item.

   \sa repetitionDelay()
*/

/*!
   \fn int QOrganizerItemReminder::repetitionDelay() const

   Returns the delay (in seconds) between each repetition of the reminder.

   \sa repetitionCount()
*/

/*!
  \macro Q_DECLARE_CUSTOM_ORGANIZER_REMINDER_DETAIL
  \relates QOrganizerItemReminder

  Macro for simplifying declaring custom (leaf) reminder detail classes.

  The first argument is the name of the class, and the second argument
  is a Latin-1 string literal naming the detail type, and the third argument
  is the reminder type of the leaf reminder detail class.

  If you are creating a convenience class for a type of QOrganizerItemReminder,
  you should use this macro when declaring your class to ensure that
  it interoperates with other organizer item functionality.
 */


/* ==================== QOrganizerItemAudibleReminder ======================= */

/*!
   \class QOrganizerItemAudibleReminder
   \brief The QOrganizerItemAudibleReminder class contains information about an audible reminder of an item.
   \inmodule QtOrganizer
   \ingroup organizer-details

   An audible reminder is a reminder which alerts the user about the item, with sound.
   Note that the Organizer API does not enforce that the sound data is played,
   or that any other sort of reminder occurs; rather, it simply allows clients
   to store and manipulate data which might be used by the platform to
   implement alarms and reminders.
 */

/*!
   \variable QOrganizerItemAudibleReminder::DefinitionName
   The constant string which identifies the definition of details which contain audible reminder information of an organizer item.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemAudibleReminder::DefinitionName, "AudibleReminder");

/*!
   \variable QOrganizerItemAudibleReminder::FieldDataUrl
   The constant key for which the value of the sound data url is stored.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemAudibleReminder::FieldDataUrl, "DataUrl");

/*!
   \fn QOrganizerItemAudibleReminder::setDataUrl(const QUrl& dataUrl)

   Sets the url of the audible data which should be played to \a dataUrl.
*/

/*!
   \fn QUrl QOrganizerItemAudibleReminder::dataUrl() const

   Returns the url of the audible data which should be played.
*/


/* ==================== QOrganizerItemEmailReminder ======================= */

/*!
   \class QOrganizerItemEmailReminder
   \brief The QOrganizerItemEmailReminder class contains information about an email reminder of an item.
   \inmodule QtOrganizer
   \ingroup organizer-details

   An email reminder is a reminder which alerts the user (or other users) about the item,
   by sending an email.
   Note that the Organizer API does not enforce that the email is sent,
   or that any other sort of reminder occurs; rather, it simply allows clients
   to store and manipulate data which might be used by the platform to
   implement alarms and reminders.
 */

/*!
   \variable QOrganizerItemEmailReminder::DefinitionName
   The constant string which identifies the definition of details which contain email reminder information of an organizer item.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemEmailReminder::DefinitionName, "EmailReminder");

/*!
   \variable QOrganizerItemEmailReminder::FieldSubject
   The constant key for which the subject field of the email which the user wishes to be sent as a reminder, is stored.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemEmailReminder::FieldSubject, "Subject");

/*!
   \variable QOrganizerItemEmailReminder::FieldBody
   The constant key for which the body field of the email which the user wishes to be sent as a reminder, is stored.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemEmailReminder::FieldBody, "Body");

/*!
   \variable QOrganizerItemEmailReminder::FieldAttachments
   The constant key for which the attachments of the email which the user wishes to be sent as a reminder, is stored.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemEmailReminder::FieldAttachments, "Attachments");

/*!
   \variable QOrganizerItemEmailReminder::FieldRecipients
   The constant key for which the recipients field of the email which the user wishes to be sent as a reminder, is stored.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemEmailReminder::FieldRecipients, "Recipients");

/*!
   \fn QOrganizerItemEmailReminder::setContents(const QString& subject, const QString& body, const QVariantList& attachments)

   Sets the contents of the email reminder to be the given \a subject, \a body and \a attachments.
*/

/*!
   \fn QString QOrganizerItemEmailReminder::subject() const

   Returns the subject of the email.
*/

/*!
   \fn QString QOrganizerItemEmailReminder::body() const

   Returns the body of the email.
*/

/*!
   \fn QOrganizerItemEmailReminder::attachments()

   Returns a list of attachments.
*/

/*!
   \fn QOrganizerItemEmailReminder::setRecipients(const QStringList& recipients)

   Sets the list of recipients that the user wishes to be sent an email as part of the reminder
   to \a recipients.
*/

/*!
   \fn QStringList QOrganizerItemEmailReminder::recipients() const

   Returns the list of recipients that the user wishes to be sent an email as part of the reminder.
*/

/* ==================== QOrganizerItemVisualReminder ======================= */

/*!
   \class QOrganizerItemVisualReminder
   \brief The QOrganizerItemVisualReminder class contains information about a visual reminder of an item.
   \inmodule QtOrganizer
   \ingroup organizer-details

   A visual reminder is a reminder which alerts the user about the item, with a message, image or video.
   Note that the Organizer API does not enforce that the visual data is displayed,
   or that any other sort of reminder occurs; rather, it simply allows clients
   to store and manipulate data which might be used by the platform to
   implement alarms and reminders.
 */

/*!
   \variable QOrganizerItemVisualReminder::DefinitionName
   The constant string which identifies the definition of details which contain visual reminder information of an organizer item.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemVisualReminder::DefinitionName, "VisualReminder");

/*!
   \variable QOrganizerItemVisualReminder::FieldMessage
   The constant key for which the value of the message to be displayed is stored in details of the QOrganizerItemVisualReminder type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemVisualReminder::FieldMessage, "Message");

/*!
   \variable QOrganizerItemVisualReminder::FieldDataUrl
   The constant key for which the value of the visual data url is stored.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemVisualReminder::FieldDataUrl, "DataUrl");

/*!
   \fn QOrganizerItemVisualReminder::setMessage(const QString& message)

   Sets the message which the user wishes to be displayed as part of the reminder to \a message.
*/

/*!
   \fn QString QOrganizerItemVisualReminder::message() const

   Returns the message which the user wishes to be displayed as part of the reminder.
*/

/*!
   \fn QOrganizerItemVisualReminder::setDataUrl(const QUrl& dataUrl)

   Sets the url of the visual data which the user wishes to be displayed as part of the reminder to \a dataUrl.
*/

/*!
   \fn QUrl QOrganizerItemVisualReminder::dataUrl() const

   Returns the url of the visual data which the user wishes to be displayed as part of the reminder.
*/


/* ==================== QOrganizerItemTimestamp ======================= */
/*!
   \class QOrganizerItemTimestamp
   \brief The QOrganizerItemTimestamp class contains the creation and last-modified timestamp associated with the organizer item.  XXX TODO: what about last accessed?
   \inmodule QtOrganizer
   \ingroup organizer-details
 */

/*!
   \variable QOrganizerItemTimestamp::DefinitionName
   The constant string which identifies the definition of details which are organizer synchronisation timestamps.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemTimestamp::DefinitionName, "Timestamp");

/*!
   \variable QOrganizerItemTimestamp::FieldModificationTimestamp

   The constant key for the value which is stored in details of the
   QOrganizerItemTimestamp type which describes the last modification date
   and time of an organizer item.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemTimestamp::FieldModificationTimestamp, "ModificationTimestamp");

/*!
   \variable QOrganizerItemTimestamp::FieldCreationTimestamp

   The constant key for the value which is stored in details of the
   QOrganizerItemTimestamp type which describes the creation date and time
   of an organizer item.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemTimestamp::FieldCreationTimestamp, "CreationTimestamp");

/*!
   \fn QOrganizerItemTimestamp::created() const
   Returns the creation timestamp saved in this detail.
 */

/*!
   \fn QOrganizerItemTimestamp::lastModified() const
   Returns the last-modified timestamp saved in this detail.
 */

/*!
   \fn QOrganizerItemTimestamp::setCreated(const QDateTime& dateTime)
   Sets the creation timestamp saved in this detail to \a dateTime.
 */

/*!
   \fn QOrganizerItemTimestamp::setLastModified(const QDateTime& dateTime)
   Sets the last-modified timestamp saved in this detail to \a dateTime.
 */





/* ==================== QOrganizerTodoProgress ======================= */
/*!
   \class QOrganizerTodoProgress
   \brief The QOrganizerTodoProgress class contains information about the progress of a todo item.
   \inmodule QtOrganizer
   \ingroup organizer-details
 */

/*!
   \variable QOrganizerTodoProgress::DefinitionName
   The constant string which identifies the definition of details which contain progress information about a todo item.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerTodoProgress::DefinitionName, "TodoProgress");

/*!
   \variable QOrganizerTodoProgress::FieldStatus

   The constant key of the value which describes the current completion status of the
   todo item.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerTodoProgress::FieldStatus, "Status");

/*!
   \variable QOrganizerTodoProgress::FieldPercentageComplete

   The constant key of the value which contains the current completion percentage of the
   todo item.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerTodoProgress::FieldPercentageComplete, "PercentageComplete");

/*!
   \variable QOrganizerTodoProgress::FieldFinishedDateTime

   The constant key of the date time value which contains the date and time at which the
   todo item was completed.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerTodoProgress::FieldFinishedDateTime, "FinishedDateTime");

/*!
  \enum QOrganizerTodoProgress::Status
  Enumerates the various possible types of todo item status
  \value StatusNotStarted The todo item hasn't been started yet
  \value StatusInProgress The todo item is current in progress
  \value StatusComplete The todo item has finished
 */

/*!
  \fn QOrganizerTodoProgress::status() const
  Returns the todo progress item's current status as QOrganizerTodoProgress::Status.
 */

/*!
  \fn QOrganizerTodoProgress::setStatus(Status status)
  Sets the todo progress item's current status to \a status.
 */

/*!
  \fn QOrganizerTodoProgress::finishedDateTime() const
  Returns the todo progress item's finished date and timeas QDateTime.
 */

/*!
  \fn QOrganizerTodoProgress::setFinishedDateTime(const QDateTime& finishedDateTime)
  Sets the todo progress item's finished date and time to \a finishedDateTime.
 */

/*!
  \fn QOrganizerTodoProgress::percentageComplete() const
  Returns the todo progress item's completion percentage.
 */

/*!
  \fn QOrganizerTodoProgress::setPercentageComplete(int percentage)
  Sets the todo progress item's completion percentage to \a percentage.
 */
/* ==================== QOrganizerTodoTimeRange ======================= */
/*!
   \class QOrganizerTodoTimeRange
   \brief The QOrganizerTodoTimeRange class contains information about the time range of a todo item.
   \inmodule QtOrganizer
   \ingroup organizer-details
 */

/*!
   \variable QOrganizerTodoTimeRange::DefinitionName
   The constant string which identifies the definition of details which contain time-range information about a todo item.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerTodoTimeRange::DefinitionName, "TodoTimeRange");

/*!
   \variable QOrganizerTodoTimeRange::FieldStartDateTime

   The constant key of the date time value which describes the date at which
   the todo should be started.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerTodoTimeRange::FieldStartDateTime, "StartDateTime");

/*!
   \variable QOrganizerTodoTimeRange::FieldDueDateTime

   The constant key of the date time value which describes the latest date at which
   the todo should be completed.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerTodoTimeRange::FieldDueDateTime, "DueDateTime");

/*!
   \variable QOrganizerTodoTimeRange::FieldTimeSpecified

   The constant key for the specification of whether the time is significant in the
   start datetime of the QOrganizerTodoTimeRange type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerTodoTimeRange::FieldTimeSpecified, "TimeSpecified");

/*!
    \fn QOrganizerTodoTimeRange::startDateTime() const
    Returns the todo timerange's start date and time as QDateTime.
 */

/*!
    \fn QOrganizerTodoTimeRange::setStartDateTime(const QDateTime& startDateTime)
    Sets the todo timerange's start date and time to \a startDateTime.
 */

/*!
    \fn QOrganizerTodoTimeRange::dueDateTime() const
    Returns the todo timerange's due date and time as QDateTime.
 */

/*!
    \fn QOrganizerTodoTimeRange::setDueDateTime(const QDateTime& dueDateTime)
    Sets the todo timerange's due date and time to \a dueDateTime.
 */


/*!
    \fn QOrganizerTodoTimeRange::setTimeSpecified(bool isTimeSpecified)
    Sets the time in specification mode through \a isTimeSpecified.
 */

/*!
    \fn QOrganizerTodoTimeRange::isTimeSpecified()
    Returns true if the time is specified.
 */

/* ==================== QOrganizerItemType ======================= */
/*!
   \class QOrganizerItemType
   \brief The QOrganizerItemType class describes the type of the organizer item.  This detail may be automatically synthesized by the backend depending on other details in the organizer item.
   \inmodule QtOrganizer
   \ingroup organizer-details
 */

/*!
   \variable QOrganizerItemType::DefinitionName
   The constant string which identifies the definition of details which identify the type of the organizer item.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemType::DefinitionName, "Type");

/*!
   \variable QOrganizerItemType::FieldType

   The constant key for the type value which is stored in details of
   the QOrganizerItemType definition.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemType::FieldType, "Type");

/*!
   \variable QOrganizerItemType::TypeEvent

   The constant attribute value which describes the organizer item as being an event.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemType::TypeEvent, "Event");

/*!
   \variable QOrganizerItemType::TypeEventOccurrence

   The constant attribute value which describes the organizer item as being an occurrence of an event.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemType::TypeEventOccurrence, "EventOccurrence");

/*!
   \variable QOrganizerItemType::TypeJournal

   The constant attribute value which describes the organizer item as being a journal.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemType::TypeJournal, "Journal");

/*!
   \variable QOrganizerItemType::TypeNote

   The constant attribute value which describes the organizer item as being a note.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemType::TypeNote, "Note");

/*!
   \variable QOrganizerItemType::TypeTodo

   The constant attribute value which describes the organizer item as being a todo.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemType::TypeTodo, "Todo");

/*!
   \variable QOrganizerItemType::TypeTodoOccurrence

   The constant attribute value which describes the organizer item as being an occurrence of a todo.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemType::TypeTodoOccurrence, "TodoOccurrence");

/*!
   \fn QOrganizerItemType::type() const
   Returns the organizer item type value stored in this detail.
 */

/*!
   \fn QOrganizerItemType::setType(const QString& type)
   Sets the type of the organizer item to be the give \a type.
 */


















QTM_END_NAMESPACE
