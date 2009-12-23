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
   \brief The QOrganizerItemRecurrenceRule class contains the recurrence rule which determines when the event described in the organizer item will occur.
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
Q_DEFINE_LATIN1_LITERAL(QOrganizerItemAccessControl::DefinitionName, "AccessControl");

/*!
   \variable QOrganizerItemAttendee::DefinitionName
   The constant string which identifies the definition of details which contain the information about an attendee.
 */
Q_DEFINE_LATIN1_LITERAL(QOrganizerItemAttendee::DefinitionName, "Attendee");

/*!
   \variable QOrganizerItemDateTime::DefinitionName
   The constant string which identifies the definition of details which contain the date and time of an event or appointment.
 */
Q_DEFINE_LATIN1_LITERAL(QOrganizerItemDateTime::DefinitionName, "DateTime");

/*!
   \variable QOrganizerItemDisplayLabel::DefinitionName

   The constant string which identifies the definition of details
   which contain a display label of an organizer item.
 */
Q_DEFINE_LATIN1_LITERAL(QOrganizerItemDisplayLabel::DefinitionName, "DisplayLabel");

/*!
   \variable QOrganizerItemGuid::DefinitionName
   The constant string which identifies the definition of details which are globally unique identifiers.
 */
Q_DEFINE_LATIN1_LITERAL(QOrganizerItemGuid::DefinitionName, "Guid");

/*!
   \variable QOrganizerItemLocation::DefinitionName

   The constant string which identifies the definition of details
   which describe a location associated with an organizer item.
 */
Q_DEFINE_LATIN1_LITERAL(QOrganizerItemLocation::DefinitionName, "Location");

/*!
   \variable QOrganizerItemNote::DefinitionName
   The constant string which identifies the definition of details which are notes.
 */
Q_DEFINE_LATIN1_LITERAL(QOrganizerItemNote::DefinitionName, "Note");

/*!
   \variable QOrganizerItemParticipationRole::DefinitionName
   The constant string which identifies the definition of details which describe the participation role of a person or contact at the event or appointment described by the organizer item.
 */
Q_DEFINE_LATIN1_LITERAL(QOrganizerItemParticipationRole::DefinitionName, "ParticipationRole");

/*!
   \variable QOrganizerItemParticipationStatus::DefinitionName
   The constant string which identifies the definition of details which describe the participation status of a person or contact at the event or appointment described by the organizer item.
 */
Q_DEFINE_LATIN1_LITERAL(QOrganizerItemParticipationStatus::DefinitionName, "ParticipationStatus");

/*!
   \variable QOrganizerItemPriority::DefinitionName
   The constant string which identifies the definition of details which contain the priority of an organizer item.
 */
Q_DEFINE_LATIN1_LITERAL(QOrganizerItemPriority::DefinitionName, "Priority");

/*!
   \variable QOrganizerItemProvenance::DefinitionName
   The constant string which identifies the definition of details which contain the provenance of an organizer item.
 */
Q_DEFINE_LATIN1_LITERAL(QOrganizerItemProvenance::DefinitionName, "Provenance");

/*!
   \variable QOrganizerItemRecurrenceRule::DefinitionName
   The constant string which identifies the definition of details which contain the recurrence rule of an organizer item.
 */
Q_DEFINE_LATIN1_LITERAL(QOrganizerItemRecurrenceRule::DefinitionName, "RecurrenceRule");

/*!
   \variable QOrganizerItemRsvpInfo::DefinitionName
   The constant string which identifies the definition of details which contain the RSVP information for an organizer item.
 */
Q_DEFINE_LATIN1_LITERAL(QOrganizerItemRsvpInfo::DefinitionName, "RsvpInfo");

/*!
   \variable QOrganizerItemTimestamp::DefinitionName
   The constant string which identifies the definition of details which are contact synchronisation timestamps.
 */
Q_DEFINE_LATIN1_LITERAL(QOrganizerItemTimestamp::DefinitionName, "Timestamp");

/*!
   \variable QOrganizerItemType::DefinitionName
   The constant string which identifies the definition of details which identify the type of the contact.
 */
Q_DEFINE_LATIN1_LITERAL(QOrganizerItemType::DefinitionName, "Type");

/*!
   \variable QOrganizerItemVisibilityTag::DefinitionName
   The constant string which identifies the definition of details which contain the visibility tag of an organizer item.
 */
Q_DEFINE_LATIN1_LITERAL(QOrganizerItemVisibilityTag::DefinitionName, "VisibilityTag");





/*!
   \variable QOrganizerItemDisplayLabel::FieldLabel

   The constant key for which the display label value is stored in
   details of the QOrganizerItemDisplayLabel type.
 */
Q_DEFINE_LATIN1_LITERAL(QOrganizerItemDisplayLabel::FieldLabel, "Label");

/*!
   \variable QOrganizerItemGuid::FieldGuid

   The constant key for which the globally unique identifier value is
   stored in details of the QOrganizerItemGuid type.
 */
Q_DEFINE_LATIN1_LITERAL(QOrganizerItemGuid::FieldGuid, "Guid");

/*!
   \variable QOrganizerItemNote::FieldNote

   The constant key for which the note value is stored in details of
   the QOrganizerItemNote type.
 */
Q_DEFINE_LATIN1_LITERAL(QOrganizerItemNote::FieldNote, "Note");

/*!
   \variable QOrganizerItemTimestamp::FieldModificationTimestamp

   The constant key for the value which is stored in details of the
   QOrganizerItemTimestamp type which describes the last modification date
   and time of a contact.
 */
Q_DEFINE_LATIN1_LITERAL(QOrganizerItemTimestamp::FieldModificationTimestamp, "ModificationTimestamp");

/*!
   \variable QOrganizerItemTimestamp::FieldCreationTimestamp

   The constant key for the value which is stored in details of the
   QOrganizerItemTimestamp type which describes the creation date and time
   of a contact.
 */
Q_DEFINE_LATIN1_LITERAL(QOrganizerItemTimestamp::FieldCreationTimestamp, "CreationTimestamp");

/*!
   \variable QOrganizerItemType::FieldType

   The constant key for the type value which is stored in details of
   the QOrganizerItemType definition.
 */
Q_DEFINE_LATIN1_LITERAL(QOrganizerItemType::FieldType, "Type");

/*!
   \variable QOrganizerItemType::TypeEvent

   The constant attribute value which describes the organizer item as being an event.
 */
Q_DEFINE_LATIN1_LITERAL(QOrganizerItemType::TypeEvent, "Event");

/*!
   \variable QOrganizerItemType::TypeInvitation

   The constant attribute value which describes the organizer item as being an invitation.
 */
Q_DEFINE_LATIN1_LITERAL(QOrganizerItemType::TypeInvitation, "Invitation");

/*!
   \variable QOrganizerItemType::TypeJournal

   The constant attribute value which describes the organizer item as being a journal.
 */
Q_DEFINE_LATIN1_LITERAL(QOrganizerItemType::TypeJournal, "Journal");

/*!
   \variable QOrganizerItemType::TypeNote

   The constant attribute value which describes the organizer item as being a note.
 */
Q_DEFINE_LATIN1_LITERAL(QOrganizerItemType::TypeNote, "Note");

/*!
   \variable QOrganizerItemType::TypeTodo

   The constant attribute value which describes the organizer item as being a todo.
 */
Q_DEFINE_LATIN1_LITERAL(QOrganizerItemType::TypeTodo, "Todo");





/*!
   \fn QOrganizerItemSyncTarget::syncTarget() const

   Returns the identifier of the backend store to which the contact
   containing this detail should be synchronised.
 */

/*!
   \fn QOrganizerItemSyncTarget::setSyncTarget(const QString& syncTarget)

   Sets the identifier of the backend store to which the contact
   containing this detail should be synchronised to \a syncTarget.
 */

/*!
   \fn QOrganizerItemEmailAddress::emailAddress() const
   Returns the email address of the contact which is stored in this detail.
 */

/*!
   \fn QOrganizerItemEmailAddress::setEmailAddress(const QString& emailAddress)
   Sets the email address of the contact which is stored in this detail to \a emailAddress.
 */

/*!
   \fn QOrganizerItemFamily::spouse() const
   Returns the name of the spouse of the contact which is stored in this detail.
 */

/*!
   \fn QOrganizerItemFamily::setSpouse(const QString& spouseName)
   Sets the name of the spouse of the contact which is stored in this detail to \a spouseName.
 */

/*!
   \fn QOrganizerItemFamily::children() const
   Returns the names of the children of the contact which is stored in this detail.
 */

/*!
   \fn QOrganizerItemFamily::setChildren(const QStringList& childrenNames)
   Sets the names of the children of the contact which is stored in this detail to \a childrenNames.
 */

/*!
   \fn QOrganizerItemAnniversary::originalDate() const
   Returns the original date of occurrance of the event stored in this detail.
 */

/*!
   \fn QOrganizerItemAnniversary::setOriginalDate(const QDate& date)
   Sets the original date of occurrance of the event stored in this detail to \a date.
 */

/*!
   \fn QOrganizerItemAnniversary::calendarId() const
 * Returns the identifier of the calendar entry associated with this anniversary.
 */

/*!
   \fn QOrganizerItemAnniversary::setCalendarId(const QString& calendarId)
   Sets the identifier of the calendar entry associated with this anniversary to \a calendarId.
 */

/*!
   \fn QOrganizerItemAnniversary::event() const
   Returns the name of the event for which this detail contains information.
 */

/*!
   \fn QOrganizerItemAnniversary::setEvent(const QString& event)
   Sets the name of the event for which this detail contains information to \a event.
 */

/*!
   \fn QOrganizerItemAnniversary::setSubType(const QString& subType)
   Sets the subtype which this detail implements to be the given \a subType.
 */

/*!
   \fn QOrganizerItemAnniversary::subType() const
   Returns the subtype that this detail implements, if defined.
 */

/*!
   \fn QOrganizerItemAvatar::avatar() const
   Returns the location of an avatar file associated with the contact.
 */

/*!
   \fn QOrganizerItemAvatar::setAvatar(const QString& avatar)
   Sets the location of an avatar file associated with the contact to \a avatar.
 */

/*!
   \fn QOrganizerItemAvatar::setSubType(const QString& subType)
   Sets the subtype which this detail implements to be the given \a subType.
 */

/*!
   \fn QOrganizerItemAvatar::subType() const
   Returns the subtype that this detail implements, if defined.
 */

/*!
   \fn QOrganizerItemAddress::postOfficeBox() const
   Returns the post office box segment of the address stored in this detail.
 */

/*!
   \fn QOrganizerItemAddress::setPostOfficeBox(const QString& postOfficeBox)
   Sets the post office box segment of the address stored in this detail to \a postOfficeBox.
 */

/*!
   \fn QOrganizerItemAddress::street() const
   Returns the street segment of the address stored in this detail.
 */

/*!
   \fn QOrganizerItemAddress::setStreet(const QString& street)
   Sets the street segment of the address stored in this detail to \a street.
 */

/*!
   \fn QOrganizerItemAddress::locality() const
   Returns the locality segment of the address stored in this detail.
 */

/*!
   \fn QOrganizerItemAddress::setLocality(const QString& locality)
   Sets the locality segment of the address stored in this detail to \a locality.
 */

/*!
   \fn QOrganizerItemAddress::region() const
   Returns the region segment of the address stored in this detail.
 */

/*!
   \fn QOrganizerItemAddress::setRegion(const QString& region)
   Sets the region segment of the address stored in this detail to \a region.
 */

/*!
   \fn QOrganizerItemAddress::postcode() const
   Returns the postcode segment of the address stored in this detail.
 */

/*!
   \fn QOrganizerItemAddress::setPostcode(const QString& postcode)
   Sets the postcode segment of the address stored in this detail to \a postcode.
 */

/*!
   \fn QOrganizerItemAddress::country() const
   Returns the country segment of the address stored in this detail.
 */

/*!
   \fn QOrganizerItemAddress::setCountry(const QString& country)
   Sets the country segment of the address stored in this detail to \a country.
 */

/*!
   \fn QOrganizerItemAddress::setSubTypes(const QStringList& subTypes)
   Sets the subtypes which this detail implements to be those contained in the list of given \a subTypes.
 */

/*!
   \fn QOrganizerItemAddress::setSubTypes(const QString& subType)
   Sets the subtypes which this detail implements to be just the given \a subType.
 */

/*!
   \fn QOrganizerItemAddress::subTypes() const
   Returns the list of subtypes that this detail implements.
 */

/*!
   \fn QOrganizerItemUrl::url() const
   Returns the url stored in this detail.
 */

/*!
   \fn QOrganizerItemUrl::setUrl(const QString& url)
   Sets the url stored in this detail to \a url.
 */

/*!
   \fn QOrganizerItemUrl::setSubType(const QString& subType)
   Sets the subtype which this detail implements to be the given \a subType.
 */

/*!
   \fn QOrganizerItemUrl::subType() const
   Returns the subtype that this detail implements, if defined.
 */

/*!
   \fn QOrganizerItemPhoneNumber::number() const
   Returns the phone number stored in this detail.
 */

/*!
   \fn QOrganizerItemPhoneNumber::setNumber(const QString& number)
   Sets the phone number stored in this detail to \a number.
 */

/*!
   \fn QOrganizerItemPhoneNumber::setSubTypes(const QStringList& subTypes)
   Sets the subtypes which this detail implements to be those contained in the list of given \a subTypes
 */

/*!
   \fn QOrganizerItemPhoneNumber::setSubTypes(const QString& subType)
   Sets the subtypes which this detail implements to be just the given \a subType.
 */

/*!
   \fn QOrganizerItemPhoneNumber::subTypes() const
   Returns the list of subtypes that this detail implements.
 */

/*!
   \fn QOrganizerItemBirthday::date() const
   Returns the date of the birthday which is stored in this detail.
 */

/*!
   \fn QOrganizerItemBirthday::setDate(const QDate& date)
   Sets the date of the birthday which is stored in this detail to \a date.
 */

/*!
   \fn QOrganizerItemGender::gender() const

   Returns the gender of the contact, as stored in this detail.  The
   possible values for the value stored are "Male", "Female" and
   "Unspecified".
 */

/*!
   \fn QOrganizerItemGender::setGender(const QString& gender)

   Sets the gender of the contact (as stored in this detail) to \a
   gender, if \a gender is either "Male" or "Female", otherwise sets
   it to "Unspecified".
 */

/*!
   \fn QOrganizerItemGeolocation::setLabel(const QString& label)
   Sets the label of the location stored in the detail to \a label.
 */

/*!
   \fn QOrganizerItemGeolocation::label() const
   Returns the label of the location stored in the detail.
 */

/*!
   \fn QOrganizerItemGeolocation::setLatitude(double latitude)

   Sets the latitude portion of the coordinate (in decimal degrees) of
   the location stored in the detail to \a latitude.
 */

/*!
   \fn QOrganizerItemGeolocation::latitude() const

   Returns the latitude portion of the coordinate (specified in
   decimal degrees) of the location stored in the detail.
 */

/*!
   \fn QOrganizerItemGeolocation::setLongitude(double longitude)

   Sets the longitude portion of the coordinate (in decimal degrees)
   of the location stored in the detail to \a longitude.
 */

/*!
   \fn QOrganizerItemGeolocation::longitude() const

   Returns the longitude portion of the coordinate (specified in
   decimal degrees) of the location stored in the detail.
 */

/*!
   \fn QOrganizerItemGeolocation::setAccuracy(double accuracy)

   Specifies that the latitude and longitude portions of the location
   stored in the detail are accurate to within \a accuracy metres.
 */

/*!
   \fn QOrganizerItemGeolocation::accuracy() const

   Returns the accuracy (in metres) of the latitude and longitude of
   the location stored in the detail.
 */

/*!
   \fn QOrganizerItemGeolocation::setAltitude(double altitude)

   Sets the altitude portion of the coordinate (in metres above the
   ellipsoid) of the location stored in the detail to \a altitude.
 */

/*!
   \fn QOrganizerItemGeolocation::altitude() const
   Returns the altitude (in metres) of the location stored in the detail.
 */

/*!
   \fn QOrganizerItemGeolocation::setAltitudeAccuracy(double altitudeAccuracy)

   Sets the altitude-accuracy portion of the coordinate (in metres) of
   the location stored in the detail to \a altitudeAccuracy.
 */

/*!
   \fn QOrganizerItemGeolocation::altitudeAccuracy() const

   Returns the accuracy of the altitude portion of the location stored
   in the detail.
 */

/*!
   \fn QOrganizerItemGeolocation::setHeading(double heading)

   Sets the heading portion of the coordinate (in decimal degrees
   clockwise relative to true north) of the location-aware device at
   the time of measurement to \a heading.
 */

/*!
   \fn QOrganizerItemGeolocation::heading() const

   Returns the heading (at the time of measurement) of the
   location-aware device that recorded (or was provided) the
   measurement.
 */

/*!
   \fn QOrganizerItemGeolocation::setSpeed(double speed)

   Sets the speed portion of the coordinate (in metres per second) of
   the location-aware device at the time of measurement to \a speed.
 */

/*!
   \fn QOrganizerItemGeolocation::speed() const

   Returns the speed (at the time of measurement) of the
   location-aware device that recorded (or was provided) the
   measurement.
 */

/*!
   \fn QOrganizerItemGeolocation::setTimestamp(const QDateTime& timestamp)

   Sets the creation (or first-valid) timestamp of the location
   information to \a timestamp.
 */

/*!
   \fn QOrganizerItemGeolocation::timestamp() const

   Returns the timestamp associated with the location stored in the
   detail.
 */

/*!
   \fn QOrganizerItemGuid::guid() const

   Returns the globally unique identifier which is stored in this
   detail.
 */

/*!
   \fn QOrganizerItemGuid::setGuid(const QString& guid)
   Sets the globally unique identifier which is stored in this detail to \a guid.
 */

/*!
   \fn QOrganizerItemName::prefix() const
   Returns the prefix segment of the name stored in this detail.
 */

/*!
   \fn QOrganizerItemName::setPrefix(const QString& prefix)
   Sets the prefix segment of the name stored in this detail to \a prefix.
 */

/*!
   \fn QOrganizerItemName::first() const
   Returns the first (given) name segment of the name stored in this detail.
 */

/*!
   \fn QOrganizerItemName::setFirst(const QString& first)
   Sets the first name segment of the name stored in this detail to \a first.
 */

/*!
   \fn QOrganizerItemName::middle() const
   
   Returns the middle (additional, or other) name segment of the name
   stored in this detail.
 */

/*!
   \fn QOrganizerItemName::setMiddle(const QString& middle)
   Sets the middle name segment of the name stored in this detail to \a middle.
 */

/*!
   \fn QOrganizerItemName::last() const

   Returns the last (family, or surname) name segment of the name
   stored in this detail.
 */

/*!
   \fn QOrganizerItemName::setLast(const QString& last)
   Sets the last name segment of the name stored in this detail to \a last.
 */

/*!
   \fn QOrganizerItemName::suffix() const
   Returns the suffix segment of the name stored in this detail.
 */

/*!
   \fn QOrganizerItemName::setSuffix(const QString& suffix)
   Sets the suffix segment of the name stored in this detail to \a suffix.
 */

/*!
   \fn QOrganizerItemName::customLabel() const
   Returns the custom label of the name stored in this detail.
 */

/*!
   \fn QOrganizerItemName::setCustomLabel(const QString& customLabel)
   Sets the custom label of the name stored in this detail to \a customLabel.
 */

/*!
   \fn QOrganizerItemNickname::setNickname(const QString& nickname)
   Sets the nickname of the contact which is stored in this detail to \a nickname.
 */

/*!
   \fn QOrganizerItemNickname::nickname() const
   Returns the nickname of the contact which is stored in this detail.
 */

/*!
   \fn QOrganizerItemNote::setNote(const QString& note)
   Sets a note associated with a contact to \a note.
 */

/*!
   \fn QOrganizerItemNote::note() const
   Returns a string for a note associated with a contact.
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
   Returns the contact type value stored in this detail.
 */

/*!
   \fn QOrganizerItemType::setType(const QString& type)
   Sets the type of the contact to be the give \a type.
 */


/*!
   \fn QOrganizerItemDisplayLabel::label() const
   Returns the display label of the contact.
 */

/*!
   \fn QOrganizerItemOnlineAccount::setAccountUri(const QString& accountUri)
   
   Sets the universal resource identifier of the contact's online
   account to \a accountUri.
 */

/*!
   \fn QOrganizerItemOnlineAccount::accountUri() const

   Returns the universal resource identifier of the online account of
   the contact.
 */

/*!
   \fn QOrganizerItemOnlineAccount::setServiceProvider(const QString& serviceProvider)

   Sets the service provider of the contact's online account to \a
   serviceProvider.
 */

/*!
   \fn QOrganizerItemOnlineAccount::serviceProvider() const
   Returns the service provider of the online account of the contact.
 */

/*!
   \fn QOrganizerItemOnlineAccount::setSubTypes(const QStringList& subTypes)
   
   Sets the subtypes which this detail implements to be those
   contained in the list of given \a subTypes.
 */

/*!
   \fn QOrganizerItemOnlineAccount::setSubTypes(const QString& subType)
   Sets the subtypes which this detail implements to be just the given \a subType.
 */

/*!
   \fn QOrganizerItemOnlineAccount::subTypes() const
   Returns the list of subtypes that this detail implements.
 */

/*!
   \fn QOrganizerItemOnlineAccount::setNickname(const QString& nickname)
   
   Sets the last-known nickname used by the contact during
   communications via the online account about which this detail
   stores presence information to \a nickname.
 */

/*!
   \fn QOrganizerItemOnlineAccount::nickname() const
   
   Returns the last-known nickname used by the contact during
   communications via the online account.
 */

/*!
   \fn QOrganizerItemOnlineAccount::setPresence(const QString& presence)
   
   Sets the presence of the online account according to the presence
   information provider to \a presence.
 */

/*!
   \fn QOrganizerItemOnlineAccount::presence() const
   
   Returns the presence of the online account according to the
   presence provider.
 */

/*!
   \fn QOrganizerItemOnlineAccount::setStatusMessage(const QString& statusMessage)

   Sets the last-known status message of the contact which was
   communicated via the online account about which this detail stores
   presence information to \a statusMessage.
 */

/*!
   \fn QOrganizerItemOnlineAccount::statusMessage() const
   
   Returns the last-known status message of the contact which was
   communicated via the online account about which this detail stores
   presence information.
 */

/*!
   \fn QOrganizerItemOrganization::setName(const QString& name)
   Sets the name of the organization stored in this detail to \a name.
 */

/*!
   \fn QOrganizerItemOrganization::name() const
   Returns the name of the organization stored in this detail.
 */

/*!
   \fn QOrganizerItemOrganization::setLogo(const QString& logo)
   Sets the logo of the organization stored in this detail to \a logo.
 */

/*!
   \fn QOrganizerItemOrganization::logo() const
   Returns the logo of the organization stored in this detail.
 */


/*!
   \fn QOrganizerItemOrganization::setDepartment(const QStringList& department)
   
   Sets the contact's department of the organization stored in this
   detail to \a department.  The department is a list of progressively
   finer-grained information.
 */

/*!
   \fn QOrganizerItemOrganization::department() const
   Returns the contact's department stored in this detail.
 */


/*!
   \fn QOrganizerItemOrganization::setLocation(const QString& location)
   
   Sets the location (e.g. city or suburb) of the organization stored
   in this detail to \a location.
 */

/*!
   \fn QOrganizerItemOrganization::location() const
   Returns the location of the organization stored in this detail.
 */


/*!
   \fn QOrganizerItemOrganization::setRole(const QString& role)
   Sets the contact's role within the organization stored in this detail to \a role.
 */

/*!
   \fn QOrganizerItemOrganization::role() const
   Returns the contact's role within the organization stored in this detail.
 */


/*!
   \fn QOrganizerItemOrganization::setTitle(const QString& title)
   Sets the contact's title within the organization stored in this detail to \a title.
 */

/*!
   \fn QOrganizerItemOrganization::title() const
   Returns the contact's title within the organization stored in this detail.
 */

/*!
   \fn QOrganizerItemOrganization::setAssistantName(const QString& assistantName)
   
   Sets the name of the default assistant of contacts belonging to
   this organization to \a assistantName.
 */

/*!
   \fn QOrganizerItemOrganization::assistantName() const

   Returns the name of the default assistant of contacts belonging to
   this organization.
 */

QTM_END_NAMESPACE
