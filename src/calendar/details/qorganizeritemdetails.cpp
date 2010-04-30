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

/*!
   \class QOrganizerItemAccessControl
   \brief The QOrganizerItemAccessControl class contains the access control information of the organizer item.
 */

/*!
   \class QOrganizerItemAttendee
   \brief The QOrganizerItemAttendee class contains the information of an attendee of the event to which the organizer item corresponds.
 */

/*!
   \class QOrganizerItemDateTime
   \brief The QOrganizerItemDateTime class contains the start and end date and time of the event, appointment or invitation which the organizer item describes.
 */

/*!
   \class QOrganizerItemDisplayLabel
   \brief The QOrganizerItemBirthday class contains the backend-synthesized display label of the organizer item.
 */

/*!
   \class QOrganizerItemGuid
   \brief The QOrganizerItemGuid class contains the globally unique identifier of the organizer item, which can be used for synchronization purposes.
 */

/*!
   \class QOrganizerItemLocation

   \brief The QOrganizerItemLocation class contains information about a location which is related to the organizer item in some manner.
 */

/*!
   \class QOrganizerItemNote
   \brief The QOrganizerItemNote class contains some arbitrary information which is relevant to the organizer item.
 */

/*!
   \class QOrganizerItemParticipationRole
   \brief The QOrganizerItemParticipationRole class contains information about the way in which a particular person or contact will participate in the event or appointment described by the organizer item.
 */

/*!
   \class QOrganizerItemParticipationStatus
   \brief The QOrganizerItemParticipationStatus class contains information regarding the participation of a particular person or contact at or in the event, todo, or appointment described by the organizer item.
 */

/*!
   \class QOrganizerItemPriority
   \brief The QOrganizerItemPriority class contains the priority of the organizer item, which may be used to resolve scheduling conflicts.
 */

/*!
   \class QOrganizerItemProvenance
   \brief The QOrganizerItemProvenance class contains information about when and from where or whom the organizer item originated.
 */

/*!
   \class QOrganizerItemRecurrenceRule
   \deprecated
   \brief The QOrganizerItemRecurrenceRule class contains the recurrence rule which determines when the event described in the organizer item will occur.
   NOTE: the QOrganizerItemRecurrenceRuleCriteria and QOrganizerItemRecurrenceRules classes may REPLACE this class -- ie, how the recurrence rules are stored is still under discussion.
 */

/*!
   \class QOrganizerItemRsvpInfo
   \brief The QOrganizerItemRsvpInfo class contains information about when, where and to whom a person should RSVP or has RSVP'ed in order to signify their intention to attend the event or appointment described by the organizer item.
 */

/*!
   \class QOrganizerItemTimestamp
   \brief The QOrganizerItemTimestamp class contains the creation and last-modified timestamp associated with the organizer item.  XXX TODO: what about last accessed?
 */

/*!
   \class QOrganizerItemType
   \brief The QOrganizerItemType class describes the type of the organizer item.  This detail may be automatically synthesized by the backend depending on other details in the organizer item.
 */

/*!
   \class QOrganizerItemVisibilityTag
   \brief The QOrganizerItemVisibilityTag class contains a hint which describes which sort of people should be able to view the information contained in the organizer item.  XXX TODO: should this be a part of every detail?  ie, allow everyone to see the date, time and location, but only some people to see the attendence list?
 */

/*!
   \variable QOrganizerItemAccessControl::DefinitionName
   The constant string which identifies the definition of details which contain access control information.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemAccessControl::DefinitionName, "AccessControl");

/*!
   \variable QOrganizerItemAttendee::DefinitionName
   The constant string which identifies the definition of details which contain the information about an attendee.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemAttendee::DefinitionName, "Attendee");

/*!
   \variable QOrganizerItemDateTime::DefinitionName
   The constant string which identifies the definition of details which contain the date and time of an event or appointment.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemDateTime::DefinitionName, "DateTime");

/*!
   \variable QOrganizerItemDisplayLabel::DefinitionName

   The constant string which identifies the definition of details
   which contain a display label of an organizer item.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemDisplayLabel::DefinitionName, "DisplayLabel");

/*!
   \variable QOrganizerItemGuid::DefinitionName
   The constant string which identifies the definition of details which are globally unique identifiers.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemGuid::DefinitionName, "Guid");

/*!
   \variable QOrganizerItemLocation::DefinitionName

   The constant string which identifies the definition of details
   which describe a location associated with an organizer item.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemLocation::DefinitionName, "Location");

/*!
   \variable QOrganizerItemNote::DefinitionName
   The constant string which identifies the definition of details which are notes.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemNote::DefinitionName, "Note");

/*!
   \variable QOrganizerItemParticipationRole::DefinitionName
   The constant string which identifies the definition of details which describe the participation role of a person or contact at the event or appointment described by the organizer item.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemParticipationRole::DefinitionName, "ParticipationRole");

/*!
   \variable QOrganizerItemParticipationStatus::DefinitionName
   The constant string which identifies the definition of details which describe the participation status of a person or contact at the event or appointment described by the organizer item.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemParticipationStatus::DefinitionName, "ParticipationStatus");

/*!
   \variable QOrganizerItemPriority::DefinitionName
   The constant string which identifies the definition of details which contain the priority of an organizer item.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemPriority::DefinitionName, "Priority");

/*!
   \variable QOrganizerItemProvenance::DefinitionName
   The constant string which identifies the definition of details which contain the provenance of an organizer item.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemProvenance::DefinitionName, "Provenance");

/*!
   \variable QOrganizerItemRecurrenceRule::DefinitionName
   The constant string which identifies the definition of details which contain the recurrence rule of an organizer item.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemRecurrenceRule::DefinitionName, "RecurrenceRule");

/*!
   \variable QOrganizerItemRsvpInfo::DefinitionName
   The constant string which identifies the definition of details which contain the RSVP information for an organizer item.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemRsvpInfo::DefinitionName, "RsvpInfo");

/*!
   \variable QOrganizerItemTimestamp::DefinitionName
   The constant string which identifies the definition of details which are contact synchronisation timestamps.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemTimestamp::DefinitionName, "Timestamp");

/*!
   \variable QOrganizerItemType::DefinitionName
   The constant string which identifies the definition of details which identify the type of the contact.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemType::DefinitionName, "Type");

/*!
   \variable QOrganizerItemVisibilityTag::DefinitionName
   The constant string which identifies the definition of details which contain the visibility tag of an organizer item.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemVisibilityTag::DefinitionName, "VisibilityTag");
















/*!
   \variable QOrganizerItemAccessControl::FieldReadAccess

   The constant key for which the read access value is stored in
   details of the QOrganizerItemAccessControl type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemAccessControl::FieldReadAccess, "ReadAccess");

/*!
   \variable QOrganizerItemAccessControl::FieldShareAccess

   The constant key for which the share access value is stored in
   details of the QOrganizerItemAccessControl type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemAccessControl::FieldShareAccess, "ShareAccess");

/*!
   \variable QOrganizerItemAccessControl::FieldWriteAccess

   The constant key for which the write access value is stored in
   details of the QOrganizerItemAccessControl type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemAccessControl::FieldWriteAccess, "WhiteAccess");

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

/*!
   \variable QOrganizerItemDateTime::FieldStartDateTime

   The constant key for which the start datetime value of the organizer item is stored in
   details of the QOrganizerItemDateTime type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemDateTime::FieldStartDateTime, "StartDateTime");

/*!
   \variable QOrganizerItemDisplayLabel::FieldLabel

   The constant key for which the display label value is stored in
   details of the QOrganizerItemDisplayLabel type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemDisplayLabel::FieldLabel, "Label");

/*!
   \variable QOrganizerItemGuid::FieldGuid

   The constant key for which the globally unique identifier value is
   stored in details of the QOrganizerItemGuid type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemGuid::FieldGuid, "Guid");

/*!
   \variable QOrganizerItemLocation::FieldGeoLocation

   The constant key for which the geolocation value is
   stored in details of the QOrganizerItemLocation type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemLocation::FieldGeoLocation, "GeoLocation");

/*!
   \variable QOrganizerItemLocation::FieldAddress

   The constant key for which the street address value is
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
   \variable QOrganizerItemNote::FieldNote

   The constant key for which the note value is stored in details of
   the QOrganizerItemNote type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemNote::FieldNote, "Note");

/*!
   \variable QOrganizerItemParticipationRole::FieldParticipationRole

   The constant key for which the participation role value is stored in details of
   the QOrganizerItemParticipationRole type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemParticipationRole::FieldParticipationRole, "ParticipationRole");

/*!
   \variable QOrganizerItemParticipationStatus::FieldParticipationStatus

   The constant key for which the participation status value is stored in details of
   the QOrganizerItemParticipationStatus type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemParticipationStatus::FieldParticipationStatus, "ParticipationStatus");

/*!
   \variable QOrganizerItemPriority::FieldPriority

   The constant key for which the priority value is stored in details of
   the QOrganizerItemPriority type.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemPriority::FieldPriority, "Priority");

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
   \variable QOrganizerItemType::TypeInvitation

   The constant attribute value which describes the organizer item as being an invitation.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemType::TypeInvitation, "Invitation");

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
   \variable QOrganizerItemVisibilityTag::FieldTag

   The constant key for the tag value which is stored in details of
   the QOrganizerItemVisibilityTag definition.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerItemVisibilityTag::FieldTag, "Tag");




















/*!
   \fn QOrganizerItemGuid::guid() const

   Returns the globally unique identifier which is stored in this detail.
 */

/*!
   \fn QOrganizerItemGuid::setGuid(const QString& guid)
   Sets the globally unique identifier which is stored in this detail to \a guid.
 */

/*!
   \fn QOrganizerItemNote::setNote(const QString& note)
   Sets a note associated with an organizer item to \a note.
 */

/*!
   \fn QOrganizerItemNote::note() const
   Returns a string for a note associated with an organizer item.
 */

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

/*!
   \fn QOrganizerItemType::type() const
   Returns the organizer item type value stored in this detail.
 */

/*!
   \fn QOrganizerItemType::setType(const QString& type)
   Sets the type of the organizer item to be the give \a type.
 */


/*!
   \fn QOrganizerItemDisplayLabel::label() const
   Returns the display label of the organizer item.
 */



QTM_END_NAMESPACE
