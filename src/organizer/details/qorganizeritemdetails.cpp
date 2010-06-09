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

QTM_BEGIN_NAMESPACE








/* ==================== QOrganizerItemAttendee ======================= */
// XXX TODO: attendees can also have participation status / role.
/*!
   \class QOrganizerItemAttendee
   \brief The QOrganizerItemAttendee class contains the information of an attendee of the event to which the organizer item corresponds.
 */

/*!
   \variable QOrganizerItemAttendee::DefinitionName
   The constant string which identifies the definition of details which contain the information about an attendee.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemAttendee::DefinitionName, "Attendee");

/*!
   \variable QOrganizerItemAttendee::FieldAttendeeDisplayLabel

   The constant key for which the display label value of the attendee is stored in
   details of the QOrganizerItemAttendee type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemAttendee::FieldAttendeeDisplayLabel, "AttendeeDisplayLabel");

/*!
   \variable QOrganizerItemAttendee::FieldAttendeeContactUri

   The constant key for which the contact URI value of the attendee is stored in
   details of the QOrganizerItemAttendee type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemAttendee::FieldAttendeeContactUri, "AttendeeContactUri");

/*!
   \variable QOrganizerItemAttendee::FieldAttendeeParticipationStatus

   The constant key for which the participation status value of the attendee is stored in
   details of the QOrganizerItemAttendee type.  XXX TODO: detail linking instead?
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemAttendee::FieldAttendeeParticipationStatus, "AttendeeParticipationStatus");








/* ==================== QOrganizerItemDescription ======================= */
/*!
   \class QOrganizerItemDescription
   \brief The QOrganizerItemDescription class contains some arbitrary information which is relevant to the organizer item.
 */

/*!
   \variable QOrganizerItemDescription::DefinitionName
   The constant string which identifies the definition of details which are notes.
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
   Sets a note associated with an organizer item to \a description.
 */

/*!
   \fn QOrganizerItemDescription::description() const
   Returns a string for a description associated with an organizer item.
 */







/* ==================== QOrganizerItemDisplayLabel ======================= */
/*!
   \class QOrganizerItemDisplayLabel
   \brief The QOrganizerItemDisplayLabel class contains the backend-synthesized display label of the organizer item.
*/

/*!
   \fn QOrganizerItemDisplayLabel::label() const
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







/* ==================== QOrganizerItemEventTimeRange ======================= */
/*!
   \class QOrganizerItemEventTimeRange
   \brief The QOrganizerItemEventTimeRange class contains the start and end dates and times of a recurring event series,
          or occurrence of an event.
*/

/*!
   \variable QOrganizerItemEventTimeRange::DefinitionName

   The constant string which identifies the definition of details
   which contain a start and end timestamps of an event series or event occurrence.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemEventTimeRange::DefinitionName, "EventTimeRange");

/*!
   \variable QOrganizerItemEventTimeRange::FieldEndDateTime

   The constant key for which the end date and time value is stored in
   details of the QOrganizerItemEventTimeRange type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemEventTimeRange::FieldEndDateTime, "EndDateTime");

/*!
   \variable QOrganizerItemEventTimeRange::FieldStartDateTime

   The constant key for which the start date and time value is stored in
   details of the QOrganizerItemEventTimeRange type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemEventTimeRange::FieldStartDateTime, "StartDateTime");








/* ==================== QOrganizerItemGuid ======================= */

/*!
   \class QOrganizerItemGuid
   \brief The QOrganizerItemGuid class contains the globally unique identifier of the organizer item, which can be used for synchronization purposes.
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







/* ==================== QOrganizerItemJournalTimeRange ======================= */
// XXX TODO: time range or just single instant in time?
/*!
   \class QOrganizerItemJournalTimeRange

   \brief The QOrganizerItemJournalTimeRange class contains information about
   the date and time for which a journal entry has been created.
 */

/*!
\variable QOrganizerItemJournalTimeRange::DefinitionName

The constant string which identifies the definition of details
which describe the date and time associated with a journal item.
*/
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemJournalTimeRange::DefinitionName, "JournalTimeRange");

/*!
   \variable QOrganizerItemJournalTimeRange::FieldEntryDateTime

   The constant key for which the date and time of value for a journal entry is
   stored in details of the QOrganizerItemJournalTimeRange type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemJournalTimeRange::FieldEntryDateTime, "EntryDateTime");


/* ==================== QOrganizerItemLocation ======================= */
// XXX TODO: do we include all three pieces of data into a single detail
// ie, street address + geolocation + location string
// Or, do we have three separate detail types?
/*!
   \class QOrganizerItemLocation

   \brief The QOrganizerItemLocation class contains information about a location which is related to the organizer item in some manner.
 */

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



/* ==================== QOrganizerItemNote ======================= */
/*!
   \class QOrganizerItemNote
   \brief The QOrganizerItemNote class contains some arbitrary information which is relevant to the organizer item.
 */

/*!
   \variable QOrganizerItemNote::DefinitionName
   The constant string which identifies the definition of details which are notes.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemNote::DefinitionName, "Note");

/*!
   \variable QOrganizerItemNote::FieldNote

   The constant key for which the note value is stored in details of
   the QOrganizerItemNote type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemNote::FieldNote, "Note");

/*!
   \fn QOrganizerItemNote::setNote(const QString& note)
   Sets a note associated with an organizer item to \a note.
 */

/*!
   \fn QOrganizerItemNote::note() const
   Returns a string for a note associated with an organizer item.
 */





/* ==================== QOrganizerItemParticipationRole ======================= */
// XXX TODO: is participation a detail?  or status/role?  Attendees can have a participation role/status
// so how do we implement that?
// my personal opinion is that this participation detail should be for the "self" person
// ie, the owner of the calendar.  other attendee participation should be listed in the attendee detail.
/*!
   \class QOrganizerItemParticipation
   \brief The QOrganizerItemParticipation class contains information about the way in which the owner of the
        calendar will participate in the event or appointment described by the organizer item.
 */

/*!
   \variable QOrganizerItemParticipation::DefinitionName
   The constant string which identifies the definition of details which describe the participation role of a person or contact at the event or appointment described by the organizer item.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemParticipation::DefinitionName, "Participation");

/*!
   \variable QOrganizerItemParticipation::FieldParticipationRole

   The constant key for which the participation role value is stored in details of
   the QOrganizerItemParticipation type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemParticipation::FieldParticipationRole, "ParticipationRole");

/*!
   \variable QOrganizerItemParticipation::FieldParticipationStatus

   The constant key for which the participation status value is stored in details of
   the QOrganizerItemParticipation type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemParticipation::FieldParticipationStatus, "ParticipationStatus");

/* ==================== QOrganizerItemPriority ======================= */
/*!
   \class QOrganizerItemPriority
   \brief The QOrganizerItemPriority class contains the priority of the organizer item, which may be used to resolve scheduling conflicts.
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








/* ==================== QOrganizerItemProvenance ======================= */
/*!
   \class QOrganizerItemProvenance
   \brief The QOrganizerItemProvenance class contains information about when and from where or whom the organizer item originated.
 */

/*!
   \variable QOrganizerItemProvenance::DefinitionName
   The constant string which identifies the definition of details which contain the provenance of an organizer item.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemProvenance::DefinitionName, "Provenance");

/*!
   \variable QOrganizerItemProvenance::FieldReceivedFrom

   The constant key for which the contact URI from whom the item was received from is stored in details of
   the QOrganizerItemProvenance type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemProvenance::FieldReceivedFrom, "ReceivedFrom");

/*!
   \variable QOrganizerItemProvenance::FieldTimestamp

   The constant key for which the receipt timestamp is stored in details of
   the QOrganizerItemProvenance type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemProvenance::FieldTimestamp, "Timestamp");








/* ==================== QOrganizerItemRecurrence ======================= */
/*!
   \class QOrganizerItemRecurrence
   \brief The QOrganizerItemRecurrence class contains a list of rules and dates on which the recurrent item occurs,
          and a list of rules and dates on which exceptions occur.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemRecurrence::DefinitionName, "Recurrence");
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemRecurrence::FieldRecurrenceRules, "RecurrenceRules");
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemRecurrence::FieldRecurrenceDates, "RecurrenceDates");
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemRecurrence::FieldExceptionRules, "ExceptionRules");
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemRecurrence::FieldExceptionDates, "ExceptionDates");











/* ==================== QOrganizerItemRsvpInfo ======================= */
/*!
   \class QOrganizerItemRsvpInfo
   \brief The QOrganizerItemRsvpInfo class contains information about when, where and to whom a person should RSVP or has RSVP'ed in order to signify their intention to attend the event or appointment described by the organizer item.
 */

/*!
   \variable QOrganizerItemRsvpInfo::DefinitionName
   The constant string which identifies the definition of details which contain the RSVP information for an organizer item.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemRsvpInfo::DefinitionName, "RsvpInfo");

/*!
   \variable QOrganizerItemRsvpInfo::FieldDeadline

   The constant key for which the deadline timestamp is stored in details of
   the QOrganizerItemRsvpInfo type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemRsvpInfo::FieldDeadline, "Deadline");

/*!
   \variable QOrganizerItemRsvpInfo::FieldResponseDate

   The constant key for which the response date timestamp is stored in details of
   the QOrganizerItemRsvpInfo type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemRsvpInfo::FieldResponseDate, "ResponseDate");

/*!
   \variable QOrganizerItemRsvpInfo::FieldContactUri

   The constant key for which the contact URI to whom the RSVP must be sent is stored in details of
   the QOrganizerItemRsvpInfo type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemRsvpInfo::FieldContactUri, "ContactUri");











/* ==================== QOrganizerItemTimestamp ======================= */
/*!
   \class QOrganizerItemTimestamp
   \brief The QOrganizerItemTimestamp class contains the creation and last-modified timestamp associated with the organizer item.  XXX TODO: what about last accessed?
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





/* ==================== QOrganizerItemTodoProgress ======================= */
/*!
   \class QOrganizerItemTodoProgress
   \brief The QOrganizerItemTodoProgress class contains information about the progress of a todo item.
 */

/*!
   \variable QOrganizerItemTodoProgress::DefinitionName
   The constant string which identifies the definition of details which contain progress information about a todo item.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemTodoProgress::DefinitionName, "TodoProgress");

/*!
   \variable QOrganizerItemTodoProgress::FieldStatus

   The constant key of the value which describes the current completion status of the
   todo item.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemTodoProgress::FieldStatus, "Status");

/*!
   \variable QOrganizerItemTodoProgress::FieldPercentageComplete

   The constant key of the value which contains the current completion percentage of the
   todo item.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemTodoProgress::FieldPercentageComplete, "PercentageComplete");

/*!
   \variable QOrganizerItemTodoProgress::FieldStartedDateTime

   The constant key of the date time value which contains the date and time at which the
   todo item was started.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemTodoProgress::FieldStartedDateTime, "StartedDateTime");

/*!
   \variable QOrganizerItemTodoProgress::FieldFinishedDateTime

   The constant key of the date time value which contains the date and time at which the
   todo item was completed.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemTodoProgress::FieldFinishedDateTime, "FinishedDateTime");





/* ==================== QOrganizerItemTodoTimeRange ======================= */
/*!
   \class QOrganizerItemTodoTimeRange
   \brief The QOrganizerItemTodoTimeRange class contains information about the time range of a todo item.
 */

/*!
   \variable QOrganizerItemTodoTimeRange::DefinitionName
   The constant string which identifies the definition of details which contain time-range information about a todo item.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemTodoTimeRange::DefinitionName, "TodoTimeRange");

/*!
   \variable QOrganizerItemTodoTimeRange::FieldNotBeforeDateTime

   The constant key of the date time value which describes the earliest date at which
   the todo should be completed.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemTodoTimeRange::FieldNotBeforeDateTime, "NotBeforeDateTime");

/*!
   \variable QOrganizerItemTodoTimeRange::FieldDueDateTime

   The constant key of the date time value which describes the latest date at which
   the todo should be completed.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemTodoTimeRange::FieldDueDateTime, "DueDateTime");


/* ==================== QOrganizerItemType ======================= */
/*!
   \class QOrganizerItemType
   \brief The QOrganizerItemType class describes the type of the organizer item.  This detail may be automatically synthesized by the backend depending on other details in the organizer item.
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
