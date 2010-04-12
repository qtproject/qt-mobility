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
#include <QByteArray>
#include <QUrl>
#include <QFile>
#include <QPixmap>
#include <QImage>
#include "qcontactmanager.h"
#include "qtcontacts.h"

#include "qcontactdetails.h"

QTM_BEGIN_NAMESPACE

/*!
   \class QContactAddress
   \brief The QContactAddress class contains an address of a contact.
   \ingroup contacts-details
 */

/*!
   \class QContactAnniversary
   \brief The QContactAnniversary class contains the anniversary of a contact.
   \ingroup contacts-details
 */

/*!
   \class QContactAvatar
   \brief The QContactAvatar class contains the avatar of a contact.
   \ingroup contacts-details
 */

/*!
   \class QContactBirthday
   \brief The QContactBirthday class contains the birthday of a contact.
   \ingroup contacts-details
 */

/*!
   \class QContactDisplayLabel
   \brief The QContactDisplayLabel class is the (possibly synthesized)
   display label of a contact.
   \ingroup contacts-details
 */

/*!
   \class QContactEmailAddress

   \brief The QContactEmailAddress class contains the email address of
   a contact.
   \ingroup contacts-details
 */

/*!
   \class QContactFamily
   \brief The QContactFamily class contains the names of the
   family members of a contact.
   \ingroup contacts-details
 */

/*!
   \class QContactGender
   \brief The QContactGender class contains the gender of a contact.
   \ingroup contacts-details
 */

/*!
   \class QContactGeoLocation
   \brief The QContactGeoLocation class contains the global location
   coordinate associated with a contact.
   \ingroup contacts-details
*/

/*!
   \class QContactGlobalPresence
   \brief The QContactGlobalPresence class provides aggregated presence information
   for a contact, synthesized or supplied by the backend.
   \ingroup contacts-details
 */

/*!
   \class QContactGuid
   \brief The QContactGuid class contains the globally unique
   Id of a contact.
   \ingroup contacts-details
 */

/*!
   \class QContactName
   \brief The QContactName class contains the name of a contact.
   \ingroup contacts-details
 */

/*!
   \class QContactNickname
   \brief The QContactNickname class contains a nickname of a contact.
   \ingroup contacts-details
 */

/*!
   \class QContactNote
   \brief The QContactNote class contains a note associated
   with a contact.
   \ingroup contacts-details
 */

/*!
   \class QContactOnlineAccount
   \brief The QContactOnlineAccount class provides the online account,
   which the contact uses to communicate with friends and family.
   \ingroup contacts-details
 */

/*!
   \class QContactOrganization
   \brief The QContactOrganization class provides details about an
   organization that the contact is either a part of, or stands for.
   \ingroup contacts-details
 */

/*!
   \class QContactPhoneNumber
   \brief The QContactPhoneNumber class provides a phone number
   of a contact.
   \ingroup contacts-details
 */

/*!
   \class QContactPresence
   \brief The QContactPresence class provides presence information
   for an online account of a contact.
   \ingroup contacts-details
 */

/*!
   \class QContactRingtone
   \brief The QContactRingtone class provides a ringtone associated
   with a contact
   \ingroup contacts-details
 */

/*!
   \class QContactSyncTarget
   \brief The QContactSyncTarget class provides a sync target
   for a contact.
   \ingroup contacts-details
 */

/*!
   \class QContactTag
   \brief The QContactTag class contains a tag associated with a
   contact.
   \ingroup contacts-details
 */

/*!
   \class QContactThumbnail
   \brief The QContactThumbnail class contains a thumbnail used
   in display lists to represent the contact.
   \ingroup contacts-details
 */

/*!
   \class QContactTimestamp
   \brief The QContactTimestamp class contains the creation and
   last-modified timestamp associated with the contact.
   \ingroup contacts-details
 */

/*!
   \class QContactType
   \brief The QContactType class describes the type of the contact.
   \ingroup contacts-details
 */

/*!
   \class QContactUrl
   \brief The QContactUrl class contains a url associated with
   a contact.
   \ingroup contacts-details
 */

/*!
   \variable QContactName::DefinitionName
   The constant string which identifies the definition of details which are names.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactName::DefinitionName, "Name");

/*!
   \variable QContactNickname::DefinitionName
   The constant string which identifies the definition of details which are nicknames.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactNickname::DefinitionName, "Nickname");

/*!
   \variable QContactNote::DefinitionName
   The constant string which identifies the definition of details which are notes.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactNote::DefinitionName, "Note");

/*!
   \variable QContactAvatar::DefinitionName
   The constant string which identifies the definition of details which are avatars.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAvatar::DefinitionName, "Avatar");

/*!
   \variable QContactAddress::DefinitionName
   The constant string which identifies the definition of details which are street addresses.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAddress::DefinitionName, "Address");

/*!
   \variable QContactPhoneNumber::DefinitionName
   The constant string which identifies the definition of details which are phone numbers.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPhoneNumber::DefinitionName, "PhoneNumber");

/*!
   \variable QContactPresence::DefinitionName
   The constant string which identifies the definition of details which contain presence information.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPresence::DefinitionName, "Presence");

/*!
   \variable QContactPresence::FieldTimestamp

   The constant key for which the last update timestamp value is stored in details
   of the QContactPresence type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPresence::FieldTimestamp, "Timestamp");

/*!
   \variable QContactPresence::FieldNickname

   The constant key for which the nickname value is stored in details
   of the QContactPresence type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPresence::FieldNickname, "Nickname");

/*!
   \variable QContactPresence::FieldPresenceState

   The constant key for which the presence state value is stored in details
   of the QContactPresence typel.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPresence::FieldPresenceState, "PresenceState");

/*!
   \variable QContactPresence::FieldPresenceStateText

   The constant key for which the presence provider provided text representation
   of the presence state is stored in details of the QContactPresence type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPresence::FieldPresenceStateText, "PresenceStateText");

/*!
   \variable QContactPresence::FieldPresenceStateImageUrl

   The constant key for which the image url value for the current presence state
   is stored in details of the QContactPresence type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPresence::FieldPresenceStateImageUrl, "PresenceStateImageUrl");

/*!
   \variable QContactPresence::FieldCustomMessage

   The constant key for which the user-entered custom message for their state
   is stored in details of the QContactPresence type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPresence::FieldCustomMessage, "CustomMessage");

/*!
   \variable QContactRingtone::DefinitionName
   The constant string which identifies the definition of details which are ringtones.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactRingtone::DefinitionName, "Ringtone");

/*!
   \variable QContactSyncTarget::DefinitionName
   The constant string which identifies the definition of details which are synchronization target stores.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactSyncTarget::DefinitionName, "SyncTarget");

/*!
   \variable QContactTimestamp::DefinitionName
   The constant string which identifies the definition of details which are contact synchronization timestamps.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactTimestamp::DefinitionName, "Timestamp");

/*!
   \variable QContactType::DefinitionName
   The constant string which identifies the definition of details which identify the type of the contact.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactType::DefinitionName, "Type");

/*!
   \variable QContactGuid::DefinitionName
   The constant string which identifies the definition of details which are globally unique identifiers.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGuid::DefinitionName, "Guid");

/*!
   \variable QContactEmailAddress::DefinitionName
   The constant string which identifies the definition of details which are email addresses.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactEmailAddress::DefinitionName, "EmailAddress");

/*!
   \variable QContactFamily::DefinitionName
   The constant string which identifies the definition of details which contain the names of the contact's family.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactFamily::DefinitionName, "Family");

/*!
   \variable QContactUrl::DefinitionName
   The constant string which identifies the definition of details which are universal resource location paths.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactUrl::DefinitionName, "Url");

/*!
   \variable QContactBirthday::DefinitionName
   The constant string which identifies the definition of details which are the dates of birthdays.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactBirthday::DefinitionName, "Birthday");

/*!
   \variable QContactAnniversary::DefinitionName
   The constant string which identifies the definition of details which are anniversary dates.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAnniversary::DefinitionName, "Anniversary");

/*!
   \variable QContactGender::DefinitionName

   The constant string which identifies the definition of details
   which identify the gender of a contact in a given context.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGender::DefinitionName, "Gender");

/*!
   \variable QContactGeoLocation::DefinitionName

   The constant string which identifies the definition of details
   which describe a location associated with a contact.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGeoLocation::DefinitionName, "GeoLocation");

/*!
   \variable QContactGlobalPresence::DefinitionName
   The constant string which identifies the definition of details which contain presence information.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGlobalPresence::DefinitionName, "GlobalPresence");

/*!
   \variable QContactGlobalPresence::FieldTimestamp

   The constant key for which the last update timestamp value is stored in details
   of the QContactGlobalPresence type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGlobalPresence::FieldTimestamp, "Timestamp");

/*!
   \variable QContactGlobalPresence::FieldNickname

   The constant key for which the nickname value is stored in details
   of the QContactGlobalPresence type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGlobalPresence::FieldNickname, "Nickname");

/*!
   \variable QContactGlobalPresence::FieldPresenceState

   The constant key for which the presence state value is stored in details
   of the QContactGlobalPresence typel.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGlobalPresence::FieldPresenceState, "PresenceState");

/*!
   \variable QContactGlobalPresence::FieldPresenceStateText

   The constant key for which the presence provider provided text representation
   of the presence state is stored in details of the QContactGlobalPresence type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGlobalPresence::FieldPresenceStateText, "PresenceStateText");

/*!
   \variable QContactGlobalPresence::FieldPresenceStateImageUrl

   The constant key for which the image url value for the current presence state
   is stored in details of the QContactGlobalPresence type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGlobalPresence::FieldPresenceStateImageUrl, "PresenceStateImageUrl");

/*!
   \variable QContactGlobalPresence::FieldCustomMessage

   The constant key for which the user-entered custom message for their state
   is stored in details of the QContactGlobalPresence type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGlobalPresence::FieldCustomMessage, "CustomMessage");

/*!
   \variable QContactOnlineAccount::DefinitionName

   The constant string which identifies the definition of details
   which identify the organization to which a contact belongs in a
   given context.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOnlineAccount::DefinitionName, "OnlineAccount");

/*!
   \variable QContactTag::DefinitionName
   The constant string which identifies the definition of details which are tags.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactTag::DefinitionName, "Tag");

/*!
   \variable QContactThumbnail::DefinitionName
   The constant string which identifies the definition of details which are thumbnails.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactThumbnail::DefinitionName, "Thumbnail");

/*!
   \variable QContactOnlineAccount::FieldCapabilities

   The constant key for which the account capabilities value is stored in
   details of the QContactOnlineAccount type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOnlineAccount::FieldCapabilities, "Capabilities");

/*!
   \variable QContactOnlineAccount::FieldAccountUri

   The constant key for which the remote account uri value is stored
   in details of the QContactOnlineAccount type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOnlineAccount::FieldAccountUri, "AccountUri");

/*!
   \variable QContactOnlineAccount::FieldServiceProvider

   The constant key for which the service provider value is stored in
   details of the QContactOnlineAccount type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOnlineAccount::FieldServiceProvider, "ServiceProvider");

/*!
   \variable QContactOnlineAccount::FieldSubTypes

   The constant key for which the subtypes value is stored in details
   of the QContactOnlineAccount type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOnlineAccount::FieldSubTypes, "SubTypes");

/*!
   \variable QContactOnlineAccount::SubTypeSip

   The constant attribute value which describes the online account as
   supporting SIP.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOnlineAccount::SubTypeSip, "Sip");

/*!
   \variable QContactOnlineAccount::SubTypeSipVoip

   The constant attribute value which describes the online account as
   supporting SIP-based VoIP.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOnlineAccount::SubTypeSipVoip, "SipVoip");

/*!
   \variable QContactOnlineAccount::SubTypeImpp

   The constant attribute value which describes the online account as
   supporting IMPP.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOnlineAccount::SubTypeImpp, "Impp");

/*!
   \variable QContactOnlineAccount::SubTypeVideoShare

   The constant attribute value which describes the online account as
   supporting VideoShare.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOnlineAccount::SubTypeVideoShare, "VideoShare");

/*!
   \variable QContactOrganization::DefinitionName

   The constant string which identifies the definition of details
   which identify the organization to which a contact belongs in a
   given context.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOrganization::DefinitionName, "Organization");

/*!
   \variable QContactDisplayLabel::DefinitionName

   The constant string which identifies the definition of details
   which contain a display label of a contact.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactDisplayLabel::DefinitionName, "DisplayLabel");


/*!
   \variable QContactPhoneNumber::FieldNumber

   The constant key for which the phone number value is stored in
   details of the QContactPhoneNumber type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPhoneNumber::FieldNumber, "PhoneNumber");

/*!
   \variable QContactPhoneNumber::FieldSubTypes

   The constant key for which the subtype values are stored in details
   of the QContactPhoneNumber type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPhoneNumber::FieldSubTypes, "SubTypes");

/*!
   \variable QContactEmailAddress::FieldEmailAddress

   The constant key for which the email address value is stored in
   details of the QContactEmailAddress type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactEmailAddress::FieldEmailAddress, "EmailAddress");

/*!
   \variable QContactFamily::FieldSpouse

   The constant key for which the spouse name value is stored in
   details of the QContactFamily type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactFamily::FieldSpouse, "Spouse");

/*!
   \variable QContactFamily::FieldChildren

   The constant key for which the children names value is stored in
   details of the QContactFamily type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactFamily::FieldChildren, "Children");

/*!
   \variable QContactGuid::FieldGuid

   The constant key for which the globally unique identifier value is
   stored in details of the QContactGuid type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGuid::FieldGuid, "Guid");

/*!
   \variable QContactSyncTarget::FieldSyncTarget

   The constant key for which the value of the target store for
   synchronization is stored in details of the QContactSyncTarget type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactSyncTarget::FieldSyncTarget, "SyncTarget");

/*!
   \variable QContactAvatar::FieldImageUrl

   The constant key for which the url of the avatar image value is stored in
   details of the QContactAvatar type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAvatar::FieldImageUrl, "ImageUrl");

/*!
   \variable QContactAvatar::FieldVideoUrl

   The constant key for which the url of the avatar video value is stored in
   details of the QContactAvatar type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAvatar::FieldVideoUrl, "VideoUrl");

/*!
   \variable QContactName::FieldPrefix

   The constant key for which the name prefix value is stored in
   details of the QContactName type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactName::FieldPrefix, "Prefix");

/*!
   \variable QContactName::FieldFirstName

   The constant key for which the first name value is stored in
   details of the QContactName type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactName::FieldFirstName, "FirstName");

/*!
   \variable QContactName::FieldMiddleName

   The constant key for which the middle name value is stored in
   details of the QContactName type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactName::FieldMiddleName, "MiddleName");

/*!
   \variable QContactName::FieldLastName

   The constant key for which the last name value is stored in details
   of the QContactName type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactName::FieldLastName, "LastName");

/*!
   \variable QContactName::FieldSuffix

   The constant key for which the name suffix value is stored in
   details of the QContactName type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactName::FieldSuffix, "Suffix");

/*!
   \variable QContactName::FieldCustomLabel

   The constant key for which the custom name label value is stored in
   details of the QContactName type, if supported.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactName::FieldCustomLabel, "CustomLabel");

/*!
   \variable QContactNickname::FieldNickname

   The constant key for which the nickname value is stored in details
   of the QContactNickname type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactNickname::FieldNickname, "Nickname");

/*!
   \variable QContactNote::FieldNote

   The constant key for which the note value is stored in details of
   the QContactNote type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactNote::FieldNote, "Note");

/*!
   \variable QContactAddress::FieldStreet

   The constant key for which the street value is stored in details of
   the QContactAddress type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAddress::FieldStreet, "Street");

/*!
   \variable QContactAddress::FieldLocality

   The constant key for which the locality value is stored in details
   of the QContactAddress type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAddress::FieldLocality, "Locality");

/*!
   \variable QContactAddress::FieldRegion

   The constant key for which the region value is stored in details of
   the QContactAddress type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAddress::FieldRegion, "Region");

/*!
   \variable QContactAddress::FieldPostcode

   The constant key for which the postcode value is stored in details
   of the QContactAddress type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAddress::FieldPostcode, "Postcode");

/*!
   \variable QContactAddress::FieldCountry

   The constant key for which the country value is stored in details
   of the QContactAddress type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAddress::FieldCountry, "Country");

/*!
   \variable QContactAddress::FieldPostOfficeBox

   The constant key for which the post office box value is stored in
   details of the QContactAddress type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAddress::FieldPostOfficeBox, "PostOfficeBox");

/*!
   \variable QContactAddress::FieldSubTypes

   The constant key for which the subtypes value is stored in details
   of the QContactAddress type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAddress::FieldSubTypes, "SubTypes");

/*!
   \variable QContactUrl::FieldUrl

   The constant key for which the url value is stored in details of
   the QContactUrl type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactUrl::FieldUrl, "Url");

/*!
   \variable QContactUrl::FieldSubType

   The constant key for which the subtypes value is stored in details
   of the QContactUrl type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactUrl::FieldSubType, "SubType");

/*!
   \variable QContactBirthday::FieldBirthday

   The constant key for which the birthday date value is stored in
   details of the QContactBirthday type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactBirthday::FieldBirthday, "Birthday");

/*!
   \variable QContactAnniversary::FieldOriginalDate

   The constant key for which the anniversary original event date
   value is stored in details of the QContactAnniversary type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAnniversary::FieldOriginalDate, "OriginalDate");

/*!
   \variable QContactAnniversary::FieldEvent

   The constant key for which the name of the event is stored in
   details of the QContactAnniversary type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAnniversary::FieldEvent, "Event");

/*!
   \variable QContactAnniversary::FieldCalendarId

   The constant key for which the value of the identifier of the
   associated calendar entry is stored in details of the
   QContactAnniversary type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAnniversary::FieldCalendarId, "CalendarId");

/*!
   \variable QContactAnniversary::FieldSubType

   The constant key for which the subtypes value is stored in details
   of the QContactAnniversary type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAnniversary::FieldSubType, "SubType");

/*!
   \variable QContactDisplayLabel::FieldLabel

   The constant key for which the display label value is stored in
   details of the QContactDisplayLabel type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactDisplayLabel::FieldLabel, "Label");

/*!
   \variable QContactGender::FieldGender

   The constant key for which the gender value is stored in details of
   the QContactGender type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGender::FieldGender, "Gender");

/*!
   \variable QContactGender::GenderMale
   The value that identifies this contact as being male.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGender::GenderMale, "Male");

/*!
   \variable QContactGender::GenderFemale
   The value that identifies this contact as being female.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGender::GenderFemale, "Female");

/*!
   \variable QContactGender::GenderUnspecified
   The value that identifies this contact as being of unspecified gender.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGender::GenderUnspecified, "Unspecified");

/*!
   \variable QContactGeoLocation::FieldLabel

   The constant key for which the location label value is stored in
   details of the QContactGeoLocation type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGeoLocation::FieldLabel, "Label");

/*!
   \variable QContactGeoLocation::FieldLatitude

   The constant key for which the latitude value is stored in details
   of the QContactGeoLocation type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGeoLocation::FieldLatitude, "Latitude");

/*!
   \variable QContactGeoLocation::FieldLongitude

   The constant key for which the longitude value is stored in details
   of the QContactGeoLocation type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGeoLocation::FieldLongitude, "Longitude");

/*!
   \variable QContactGeoLocation::FieldAccuracy

   The constant key for which the location accuracy value is stored in
   details of the QContactGeoLocation type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGeoLocation::FieldAccuracy, "Accuracy");

/*!
   \variable QContactGeoLocation::FieldAltitude

   The constant key for which the altitude value is stored in details
   of the QContactGeoLocation type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGeoLocation::FieldAltitude, "Altitude");


/*!
   \variable QContactGeoLocation::FieldAltitudeAccuracy

   The constant key for which the altitude accuracy value is stored in
   details of the QContactGeoLocation type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGeoLocation::FieldAltitudeAccuracy, "AltitudeAccuracy");

/*!
   \variable QContactGeoLocation::FieldHeading

   The constant key for which the heading value is stored in details
   of the QContactGeoLocation type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGeoLocation::FieldHeading, "Heading");

/*!
   \variable QContactGeoLocation::FieldSpeed

   The constant key for which the speed value is stored in details of
   the QContactGeoLocation type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGeoLocation::FieldSpeed, "Speed");

/*!
   \variable QContactTag::FieldTag

   The constant key for which the tag value is stored in details
   of the QContactTag type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactTag::FieldTag, "Tag");

/*!
   \variable QContactGeoLocation::FieldTimestamp

   The constant key for which the timestamp value is stored in details
   of the QContactGeoLocation type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGeoLocation::FieldTimestamp, "Timestamp");

/*!
   \variable QContactOrganization::FieldName

   The constant key for which the name value is stored in details of
   the QContactOrganization type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOrganization::FieldName, "Name");

/*!
   \variable QContactOrganization::FieldLogoUrl

   The constant key for which the logo url is stored in details
   of the QContactOrganization type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOrganization::FieldLogoUrl, "LogoUrl");

/*!
   \variable QContactOrganization::FieldDepartment

   The constant key for which the organization's department value is
   stored in details of the QContactOrganization type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOrganization::FieldDepartment, "Department");

/*!
   \variable QContactOrganization::FieldLocation

   The constant key for which the organization's location (or the
   location of the contact's part of the organization) value is stored
   in details of the QContactOrganization type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOrganization::FieldLocation, "Location");

/*!
   \variable QContactOrganization::FieldRole

   The constant key for which the contact's role within the
   organization is stored in details of the QContactOrganization type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOrganization::FieldRole, "Role");

/*!
   \variable QContactOrganization::FieldTitle

   The constant key for which the contact's title within the
   organization is stored in details of the QContactOrganization type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOrganization::FieldTitle, "Title");

/*!
   \variable QContactOrganization::FieldAssistantName

   The constant key for which the contact's assistant name within the
   organization is stored in details of the QContactOrganization type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOrganization::FieldAssistantName, "AssistantName");

/*!
   \variable QContactRingtone::FieldAudioRingtoneUrl

   The constant key for which the uri of the audio ringtone value is
   stored in details of the QContactRingtone type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactRingtone::FieldAudioRingtoneUrl, "AudioRingtoneUrl");

/*!
   \variable QContactRingtone::FieldVideoRingtoneUrl

   The constant key for which the uri of the video ringtone value is
   stored in details of the QContactRingtone type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactRingtone::FieldVideoRingtoneUrl, "VideoRingtoneUrl");

/*!
   \variable QContactRingtone::FieldVibrationRingtoneUrl

   The constant key for which the uri of the vibration ringtone value is
   stored in details of the QContactRingtone type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactRingtone::FieldVibrationRingtoneUrl, "VibrationRingtoneUrl");

/*!
   \variable QContactThumbnail::FieldThumbnail

   The constant key for which the thumbnail image is stored in details
   of the QContactThumbnail type.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactThumbnail::FieldThumbnail, "Thumbnail");

/*!
   \variable QContactTimestamp::FieldModificationTimestamp

   The constant key for the value which is stored in details of the
   QContactTimestamp type which describes the last modification date
   and time of a contact.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactTimestamp::FieldModificationTimestamp, "ModificationTimestamp");

/*!
   \variable QContactTimestamp::FieldCreationTimestamp

   The constant key for the value which is stored in details of the
   QContactTimestamp type which describes the creation date and time
   of a contact.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactTimestamp::FieldCreationTimestamp, "CreationTimestamp");

/*!
   \variable QContactType::FieldType

   The constant key for the type value which is stored in details of
   the QContactType definition.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactType::FieldType, "Type");


/*!
   \variable QContactType::TypeContact

   The constant attribute value which describes the contact as being
   an ordinary contact.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactType::TypeContact, "Contact");

/*!
   \variable QContactType::TypeGroup

   The constant attribute value which describes the contact as being a
   group.  Contacts of this type are able to be the first contact in
   relationships of the \c QContactRelationship::HasMember type.

   To enumerate the ids of members of a group, the client should
   retrieve the relationships which involve the group from the manager
   in which the group is saved.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactType::TypeGroup, "Group");

/*!
   \variable QContactPhoneNumber::SubTypeLandline

   The constant attribute value which describes the phone number as
   identifying a landline phone.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPhoneNumber::SubTypeLandline, "Landline");

/*!
   \variable QContactPhoneNumber::SubTypeMobile

   The constant attribute value which describes the phone number as
   identifying a mobile phone.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPhoneNumber::SubTypeMobile, "Mobile");

/*!
   \variable QContactPhoneNumber::SubTypeFax

   The constant attribute value which describes the phone number as
   identifying a fax machine.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPhoneNumber::SubTypeFax, "Fax");

/*!
   \variable QContactPhoneNumber::SubTypePager

   The constant attribute value which describes the phone number as
   identifying a pager device.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPhoneNumber::SubTypePager, "Pager");

/*!
   \variable QContactPhoneNumber::SubTypeCar

   The constant attribute value which describes the phone number as
   identifying a car phone.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPhoneNumber::SubTypeCar, "Car");

/*!
   \variable QContactPhoneNumber::SubTypeBulletinBoardSystem

   The constant attribute value which describes the phone number as
   identifying a bulletin board system.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPhoneNumber::SubTypeBulletinBoardSystem, "BulletinBoardSystem");

/*!
   \variable QContactPhoneNumber::SubTypeVoice

   The constant attribute value which describes the phone number as
   supporting voice transmission.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPhoneNumber::SubTypeVoice, "Voice");

/*!
   \variable QContactPhoneNumber::SubTypeModem

   The constant attribute value which describes the phone number as
   supporting digital data transfer.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPhoneNumber::SubTypeModem, "Modem");

/*!
   \variable QContactPhoneNumber::SubTypeVideo

   The constant attribute value which describes the phone number as
   supporting video transmission.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPhoneNumber::SubTypeVideo, "Video");

/*!
   \variable QContactPhoneNumber::SubTypeMessagingCapable

   The constant attribute value which describes the phone number as
   supporting messaging services.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPhoneNumber::SubTypeMessagingCapable, "MessagingCapable");

/*!
   \variable QContactPhoneNumber::SubTypeAssistant

   The constant attribute value which describes the phone number as an
   assistant phone number.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPhoneNumber::SubTypeAssistant, "Assistant");

/*!
   \variable QContactPhoneNumber::SubTypeDtmfMenu

   The constant attribute value which describes the phone number as
   supporting DTMF-controlled electronic menu navigation.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPhoneNumber::SubTypeDtmfMenu, "DtmfMenu");

/*!
   \variable QContactAddress::SubTypeParcel

   The constant attribute value which describes the address as being
   an address for parcel delivery.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAddress::SubTypeParcel, "Parcel");

/*!
   \variable QContactAddress::SubTypePostal

   The constant attribute value which describes the address as being
   an address for postal delivery.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAddress::SubTypePostal, "Postal");

/*!
   \variable QContactAddress::SubTypeDomestic

   The constant attribute value which describes the address as being a
   domestic address.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAddress::SubTypeDomestic, "Domestic");

/*!
   \variable QContactAddress::SubTypeInternational

   The constant attribute value which describes the address as being
   an international address.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAddress::SubTypeInternational, "International");

/*!
   \variable QContactUrl::SubTypeHomePage

   The constant attribute value which describes the url as being the
   homepage of the contact.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactUrl::SubTypeHomePage, "HomePage");

/*!
   \variable QContactUrl::SubTypeFavourite

   The constant attribute value which describes the url as being a
   favourite page of the contact.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactUrl::SubTypeFavourite, "Favourite");

/*!
   \variable QContactAnniversary::SubTypeWedding

   The constant attribute value which describes the anniversary as
   being a wedding anniversary.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAnniversary::SubTypeWedding, "Wedding");

/*!
   \variable QContactAnniversary::SubTypeEngagement

   The constant attribute value which describes the anniversary as
   being an engagement anniversary.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAnniversary::SubTypeEngagement, "Engagement");

/*!
   \variable QContactAnniversary::SubTypeHouse

   The constant attribute value which describes the anniversary as
   being an anniversary of ownership of a particular residence.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAnniversary::SubTypeHouse, "House");

/*!
   \variable QContactAnniversary::SubTypeEmployment

   The constant attribute value which describes the anniversary as
   being an anniversary of employment at a particular company.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAnniversary::SubTypeEmployment, "Employment");

/*!
   \variable QContactAnniversary::SubTypeMemorial

   The constant attribute value which describes the anniversary as
   being an anniversary of an event of sentimental significance.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAnniversary::SubTypeMemorial, "Memorial");


/*!
   \fn QContactSyncTarget::syncTarget() const

   Returns the identifier of the backend store to which the contact
   containing this detail should be synchronized.
 */

/*!
   \fn QContactSyncTarget::setSyncTarget(const QString& syncTarget)

   Sets the identifier of the backend store to which the contact
   containing this detail should be synchronized to \a syncTarget.
 */

/*!
   \fn QContactEmailAddress::emailAddress() const
   Returns the email address of the contact which is stored in this detail.
 */

/*!
   \fn QContactEmailAddress::setEmailAddress(const QString& emailAddress)
   Sets the email address of the contact which is stored in this detail to \a emailAddress.
 */

/*!
   \fn QContactFamily::spouse() const
   Returns the name of the spouse of the contact which is stored in this detail.
 */

/*!
   \fn QContactFamily::setSpouse(const QString& spouseName)
   Sets the name of the spouse of the contact which is stored in this detail to \a spouseName.
 */

/*!
   \fn QContactFamily::children() const
   Returns the names of the children of the contact which is stored in this detail.
 */

/*!
   \fn QContactFamily::setChildren(const QStringList& childrenNames)
   Sets the names of the children of the contact which is stored in this detail to \a childrenNames.
 */

/*!
   \fn QContactAnniversary::originalDate() const
   Returns the original date of occurrence of the event stored in this detail.
 */

/*!
   \fn QContactAnniversary::setOriginalDate(const QDate& date)
   Sets the original date of occurrence of the event stored in this detail to \a date.
 */

/*!
   \fn QContactAnniversary::calendarId() const
 * Returns the identifier of the calendar entry associated with this anniversary.
 */

/*!
   \fn QContactAnniversary::setCalendarId(const QString& calendarId)
   Sets the identifier of the calendar entry associated with this anniversary to \a calendarId.
 */

/*!
   \fn QContactAnniversary::event() const
   Returns the name of the event for which this detail contains information.
 */

/*!
   \fn QContactAnniversary::setEvent(const QString& event)
   Sets the name of the event for which this detail contains information to \a event.
 */

/*!
   \fn QContactAnniversary::setSubType(const QString& subType)
   Sets the subtype which this detail implements to be the given \a subType.
 */

/*!
   \fn QContactAnniversary::subType() const
   Returns the subtype that this detail implements, if defined.
 */

/*!
  \fn QContactAvatar::imageUrl() const
  Returns the url of an avatar image associated with the contact
 */

/*!
  \fn QContactAvatar::setImageUrl(const QUrl& imageUrl)
  Sets the url of an avatar image associated with the contact to \a imageUrl
 */

/*!
  \fn QContactAvatar::videoUrl() const
  Returns the url of an avatar video associated with the contact
 */

/*!
  \fn QContactAvatar::setVideoUrl(const QUrl& videoUrl)
  Sets the url of an avatar video associated with the contact to \a videoUrl
 */

/*!
   \fn QContactAddress::postOfficeBox() const
   Returns the post office box segment of the address stored in this detail.
 */

/*!
   \fn QContactAddress::setPostOfficeBox(const QString& postOfficeBox)
   Sets the post office box segment of the address stored in this detail to \a postOfficeBox.
 */

/*!
   \fn QContactAddress::street() const
   Returns the street segment of the address stored in this detail.
 */

/*!
   \fn QContactAddress::setStreet(const QString& street)
   Sets the street segment of the address stored in this detail to \a street.
 */

/*!
   \fn QContactAddress::locality() const
   Returns the locality segment of the address stored in this detail.
 */

/*!
   \fn QContactAddress::setLocality(const QString& locality)
   Sets the locality segment of the address stored in this detail to \a locality.
 */

/*!
   \fn QContactAddress::region() const
   Returns the region segment of the address stored in this detail.
 */

/*!
   \fn QContactAddress::setRegion(const QString& region)
   Sets the region segment of the address stored in this detail to \a region.
 */

/*!
   \fn QContactAddress::postcode() const
   Returns the postcode segment of the address stored in this detail.
 */

/*!
   \fn QContactAddress::setPostcode(const QString& postcode)
   Sets the postcode segment of the address stored in this detail to \a postcode.
 */

/*!
   \fn QContactAddress::country() const
   Returns the country segment of the address stored in this detail.
 */

/*!
   \fn QContactAddress::setCountry(const QString& country)
   Sets the country segment of the address stored in this detail to \a country.
 */

/*!
   \fn QContactAddress::setSubTypes(const QStringList& subTypes)
   Sets the subtypes which this detail implements to be those contained in the list of given \a subTypes.
 */

/*!
   \fn QContactAddress::setSubTypes(const QString& subType)
   Sets the subtypes which this detail implements to be just the given \a subType.
 */

/*!
   \fn QContactAddress::subTypes() const
   Returns the list of subtypes that this detail implements.
 */

/*!
   \fn QContactUrl::url() const
   Returns the url stored in this detail.
 */

/*!
   \fn QContactUrl::setUrl(const QString& url)
   Sets the url stored in this detail to \a url.
 */

/*!
   \fn QContactUrl::setSubType(const QString& subType)
   Sets the subtype which this detail implements to be the given \a subType.
 */

/*!
   \fn QContactUrl::subType() const
   Returns the subtype that this detail implements, if defined.
 */

/*!
   \fn QContactPhoneNumber::number() const
   Returns the phone number stored in this detail.
 */

/*!
   \fn QContactPhoneNumber::setNumber(const QString& number)
   Sets the phone number stored in this detail to \a number.
 */

/*!
   \fn QContactPhoneNumber::setSubTypes(const QStringList& subTypes)
   Sets the subtypes which this detail implements to be those contained in the list of given \a subTypes
 */

/*!
   \fn QContactPhoneNumber::setSubTypes(const QString& subType)
   Sets the subtypes which this detail implements to be just the given \a subType.
 */

/*!
   \fn QContactPhoneNumber::subTypes() const
   Returns the list of subtypes that this detail implements.
 */

/*!
   \fn QContactBirthday::date() const
   Returns the date of the birthday which is stored in this detail.
 */

/*!
   \fn QContactBirthday::setDate(const QDate& date)
   Sets the date of the birthday which is stored in this detail to \a date.
 */

/*!
   \fn QContactGender::gender() const

   Returns the gender of the contact, as stored in this detail.  The
   possible values for the value stored are "Male", "Female" and
   "Unspecified".
 */

/*!
   \fn QContactGender::setGender(const QString& gender)

   Sets the gender of the contact (as stored in this detail) to \a
   gender, if \a gender is either "Male" or "Female", otherwise sets
   it to "Unspecified".
 */

/*!
   \fn QContactGeoLocation::setLabel(const QString& label)
   Sets the label of the location stored in the detail to \a label.
 */

/*!
   \fn QContactGeoLocation::label() const
   Returns the label of the location stored in the detail.
 */

/*!
   \fn QContactGeoLocation::setLatitude(double latitude)

   Sets the latitude portion of the coordinate (in decimal degrees) of
   the location stored in the detail to \a latitude.
 */

/*!
   \fn QContactGeoLocation::latitude() const

   Returns the latitude portion of the coordinate (specified in
   decimal degrees) of the location stored in the detail.
 */

/*!
   \fn QContactGeoLocation::setLongitude(double longitude)

   Sets the longitude portion of the coordinate (in decimal degrees)
   of the location stored in the detail to \a longitude.
 */

/*!
   \fn QContactGeoLocation::longitude() const

   Returns the longitude portion of the coordinate (specified in
   decimal degrees) of the location stored in the detail.
 */

/*!
   \fn QContactGeoLocation::setAccuracy(double accuracy)

   Specifies that the latitude and longitude portions of the location
   stored in the detail are accurate to within \a accuracy metres.
 */

/*!
   \fn QContactGeoLocation::accuracy() const

   Returns the accuracy (in metres) of the latitude and longitude of
   the location stored in the detail.
 */

/*!
   \fn QContactGeoLocation::setAltitude(double altitude)

   Sets the altitude portion of the coordinate (in metres above the
   ellipsoid) of the location stored in the detail to \a altitude.
 */

/*!
   \fn QContactGeoLocation::altitude() const
   Returns the altitude (in metres) of the location stored in the detail.
 */

/*!
   \fn QContactGeoLocation::setAltitudeAccuracy(double altitudeAccuracy)

   Sets the altitude-accuracy portion of the coordinate (in metres) of
   the location stored in the detail to \a altitudeAccuracy.
 */

/*!
   \fn QContactGeoLocation::altitudeAccuracy() const

   Returns the accuracy of the altitude portion of the location stored
   in the detail.
 */

/*!
   \fn QContactGeoLocation::setHeading(double heading)

   Sets the heading portion of the coordinate (in decimal degrees
   clockwise relative to true north) of the location-aware device at
   the time of measurement to \a heading.
 */

/*!
   \fn QContactGeoLocation::heading() const

   Returns the heading (at the time of measurement) of the
   location-aware device that recorded (or was provided) the
   measurement.
 */

/*!
   \fn QContactGeoLocation::setSpeed(double speed)

   Sets the speed portion of the coordinate (in metres per second) of
   the location-aware device at the time of measurement to \a speed.
 */

/*!
   \fn QContactGeoLocation::speed() const

   Returns the speed (at the time of measurement) of the
   location-aware device that recorded (or was provided) the
   measurement.
 */

/*!
   \fn QContactGeoLocation::setTimestamp(const QDateTime& timestamp)

   Sets the creation (or first-valid) timestamp of the location
   information to \a timestamp.
 */

/*!
   \fn QContactGeoLocation::timestamp() const

   Returns the timestamp associated with the location stored in the
   detail.
 */

/*!
   \fn QContactGuid::guid() const

   Returns the globally unique identifier which is stored in this
   detail.
 */

/*!
   \fn QContactGuid::setGuid(const QString& guid)
   Sets the globally unique identifier which is stored in this detail to \a guid.
 */

/*!
   \fn QContactName::prefix() const
   Returns the prefix segment of the name stored in this detail.
 */

/*!
   \fn QContactName::setPrefix(const QString& prefix)
   Sets the prefix segment of the name stored in this detail to \a prefix.
 */

/*!
   \fn QContactName::firstName() const
   Returns the first (given) name segment of the name stored in this detail.
 */

/*!
   \fn QContactName::setFirstName(const QString& firstName)
   Sets the first name segment of the name stored in this detail to \a firstName.
 */

/*!
   \fn QContactName::middleName() const
   
   Returns the middle (additional, or other) name segment of the name
   stored in this detail.
 */

/*!
   \fn QContactName::setMiddleName(const QString& middleName)
   Sets the middle name segment of the name stored in this detail to \a middleName.
 */

/*!
   \fn QContactName::lastName() const

   Returns the last (family, or surname) name segment of the name
   stored in this detail.
 */

/*!
   \fn QContactName::setLastName(const QString& lastName)
   Sets the last name segment of the name stored in this detail to \a lastName.
 */

/*!
   \fn QContactName::suffix() const
   Returns the suffix segment of the name stored in this detail.
 */

/*!
   \fn QContactName::setSuffix(const QString& suffix)
   Sets the suffix segment of the name stored in this detail to \a suffix.
 */

/*!
   \fn QContactName::customLabel() const
   Returns the custom label of the name stored in this detail.
 */

/*!
   \fn QContactName::setCustomLabel(const QString& customLabel)
   Sets the custom label of the name stored in this detail to \a customLabel.
 */

/*!
   \fn QContactNickname::setNickname(const QString& nickname)
   Sets the nickname of the contact which is stored in this detail to \a nickname.
 */

/*!
   \fn QContactNickname::nickname() const
   Returns the nickname of the contact which is stored in this detail.
 */

/*!
   \fn QContactNote::setNote(const QString& note)
   Sets a note associated with a contact to \a note.
 */

/*!
   \fn QContactNote::note() const
   Returns a string for a note associated with a contact.
 */

/*!
   \fn QContactTag::setTag(const QString& tag)
   Sets the tag associated with a contact which is stored in this detail to \a tag.
 */

/*!
   \fn QContactTag::tag() const
   Returns the tag associated with a contact which is stored in this detail.
 */

/*!
  \fn QContactThumbnail::thumbnail() const
  Returns the thumbnail image of the contact
 */

/*!
  \fn QContactThumbnail::setThumbnail(const QImage& thumbnail)
  Sets the thumbnail image of the contact to be \a thumbnail
 */

/*!
   \fn QContactTimestamp::created() const
   Returns the creation timestamp saved in this detail.
 */

/*!
   \fn QContactTimestamp::lastModified() const
   Returns the last-modified timestamp saved in this detail.
 */

/*!
   \fn QContactTimestamp::setCreated(const QDateTime& dateTime)
   Sets the creation timestamp saved in this detail to \a dateTime.
 */

/*!
   \fn QContactTimestamp::setLastModified(const QDateTime& dateTime)
   Sets the last-modified timestamp saved in this detail to \a dateTime.
 */

/*!
   \fn QContactType::type() const
   Returns the contact type value stored in this detail.
 */

/*!
   \fn QContactType::setType(const QString& type)
   Sets the type of the contact to be the give \a type.
 */


/*!
   \fn QContactDisplayLabel::label() const
   Returns the display label of the contact.
 */

/*!
   \fn QContactOnlineAccount::setAccountUri(const QString& accountUri)
   
   Sets the universal resource identifier of the contact's online
   account to \a accountUri.
 */

/*!
   \fn QContactOnlineAccount::accountUri() const

   Returns the universal resource identifier of the online account of
   the contact.
 */

/*!
   \fn QContactOnlineAccount::setServiceProvider(const QString& serviceProvider)

   Sets the service provider of the contact's online account to \a
   serviceProvider.
 */

/*!
   \fn QContactOnlineAccount::serviceProvider() const
   Returns the service provider of the online account of the contact.
 */

/*!
   \fn QContactOnlineAccount::setSubTypes(const QStringList& subTypes)
   
   Sets the subtypes which this detail implements to be those
   contained in the list of given \a subTypes.
 */

/*!
   \fn QContactOnlineAccount::setSubTypes(const QString& subType)
   Sets the subtypes which this detail implements to be just the given \a subType.
 */

/*!
   \fn QContactOnlineAccount::subTypes() const
   Returns the list of subtypes that this detail implements.
 */

/*!
   \fn QContactOnlineAccount::setCapabilities(const QStringList& capabilities)

   Sets the capabilities of the online account about which this detail stores
   presence information to \a capabilities.  The \a capabilities list is a
   list of service-provider specified strings which together identify the
   types of communication which may be possible.
 */

/*!
   \fn QContactOnlineAccount::capabilities() const

   Returns the capabilities of the online account about which this detail stores
   presence information.
 */

/*!
   \fn QContactOrganization::setName(const QString& name)
   Sets the name of the organization stored in this detail to \a name.
 */

/*!
   \fn QContactOrganization::name() const
   Returns the name of the organization stored in this detail.
 */

/*!
   \fn QContactOrganization::setLogoUrl(const QUrl& logo)
   Sets the url of the logo of the organization stored in this detail to \a logo.
 */

/*!
   \fn QContactOrganization::logoUrl() const
   Returns the url of the logo of the organization stored in this detail.
 */

/*!
   \fn QContactOrganization::setDepartment(const QStringList& department)
   
   Sets the contact's department of the organization stored in this
   detail to \a department.  The department is a list of progressively
   finer-grained information.
 */

/*!
   \fn QContactOrganization::department() const
   Returns the contact's department stored in this detail.
 */


/*!
   \fn QContactOrganization::setLocation(const QString& location)
   
   Sets the location (e.g. city or suburb) of the organization stored
   in this detail to \a location.
 */

/*!
   \fn QContactOrganization::location() const
   Returns the location of the organization stored in this detail.
 */


/*!
   \fn QContactOrganization::setRole(const QString& role)
   Sets the contact's role within the organization stored in this detail to \a role.
 */

/*!
   \fn QContactOrganization::role() const
   Returns the contact's role within the organization stored in this detail.
 */


/*!
   \fn QContactOrganization::setTitle(const QString& title)
   Sets the contact's title within the organization stored in this detail to \a title.
 */

/*!
   \fn QContactOrganization::title() const
   Returns the contact's title within the organization stored in this detail.
 */

/*!
   \fn QContactOrganization::setAssistantName(const QString& assistantName)
   
   Sets the name of the default assistant of contacts belonging to
   this organization to \a assistantName.
 */

/*!
   \fn QContactOrganization::assistantName() const

   Returns the name of the default assistant of contacts belonging to
   this organization.
 */

/*!
  \fn QContactRingtone::audioRingtoneUrl() const

  Returns the uri of the audio ringtone stored in the ringtone detail.
 */

/*!
  \fn QContactRingtone::setAudioRingtoneUrl(const QUrl& audioRingtoneUrl)

  Sets the uri of the audio ringtone stored in the ringtone detail
  to \a audioRingtoneUrl.
 */

/*!
  \fn QContactRingtone::videoRingtoneUrl() const

  Returns the uri of the video ringtone stored in the ringtone detail.
 */

/*!
  \fn QContactRingtone::setVideoRingtoneUrl(const QUrl& videoRingtoneUrl)

  Sets the uri of the video ringtone stored in the ringtone detail
  to \a videoRingtoneUrl.
 */

/*!
  \fn QContactRingtone::vibrationRingtoneUrl() const

  Returns the uri of the vibration ringtone stored in the ringtone detail.
 */

/*!
  \fn QContactRingtone::setVibrationRingtoneUrl(const QUrl& vibrationRingtoneUrl)

  Sets the uri of the vibration ringtone stored in the ringtone detail
  to \a vibrationRingtoneUrl.
 */

/*!
   \fn QContactPresence::setTimestamp(const QDateTime& updateTimestamp)

   Sets the update timestamp of the presence detail to be
   \a updateTimestamp.
 */

/*!
   \fn QContactPresence::timestamp() const

    Returns the timestamp at which the data in the presence detail was valid.
 */

/*!
   \fn QContactPresence::setNickname(const QString& nickname)

   Sets the last-known nickname used by the contact during
   communications via the online account about which this detail
   stores presence information to \a nickname.
 */

/*!
   \fn QContactPresence::nickname() const

   Returns the last-known nickname used by the contact during
   communications via the online account.
 */

/*!
  \enum QContactPresence::PresenceState

  This enum defines the possible presence states supported by the default schema.

  \value PresenceUnknown Signifies that the presence state of the contact is not currently known
  \value PresenceAvailable Signifies that the contact is available
  \value PresenceHidden Signifies that the contact is hidden
  \value PresenceBusy Signifies that the contact is busy
  \value PresenceAway Signifies that the contact is away
  \value PresenceExtendedAway Signifies that the contact is away for an extended period of time
  \value PresenceOffline Signifies that the contact is offline
 */

/*!
   \fn QContactPresence::setPresenceState(QContactPresence::PresenceState presenceState)

   Sets the presence state of the online account according to the presence
   information provider to the given \a presenceState.
 */

/*!
   \fn QContactPresence::presenceState() const

   Returns the presence state of the online account according to the
   presence provider.
 */

/*!
   \fn QContactPresence::setPresenceStateText(const QString& presenceStateText)

   Sets the text corresponding to the presence state to \a presenceStateText.
   This function is generally called by presence providers to allow custom
   naming of states, or to allow finer grained state reporting than is
   provided by the presence state API.
 */

/*!
   \fn QContactPresence::presenceStateText() const

   Returns the text corresponding to the current presence state.
 */

/*!
  \fn QContactPresence::setCustomMessage(const QString& customMessage)

   Sets the custom status message from the contact for the online account
   about which this detail stores presence information, to \a customMessage.
 */

/*!
   \fn QContactPresence::customMessage() const

   Returns the custom status message from the contact for the online account
   about which this detail stores presence information.
 */

/*!
   \fn QContactPresence::setPresenceStateImageUrl(const QUrl& presenceStateImageUrl)

   Sets the last-known status image url of the contact for the online account
   about which this detail stores presence information, to \a presenceStateImageUrl.
 */

/*!
   \fn QContactPresence::presenceStateImageUrl() const

   Returns the last-known status image url of the contact for the online account
   about which this detail stores presence information.
 */

/*!
   \fn QContactGlobalPresence::setTimestamp(const QDateTime& updateTimestamp)

   Sets the update timestamp of the global presence detail to be
   \a updateTimestamp.
 */

/*!
   \fn QContactGlobalPresence::timestamp() const

    Returns the timestamp at which the data in the global presence detail was valid.
 */

/*!
   \fn QContactGlobalPresence::setNickname(const QString& nickname)

   Sets the last-known nickname used by the contact during
   communications via any online account about which this detail
   aggregates presence information to \a nickname.
 */

/*!
   \fn QContactGlobalPresence::nickname() const

   Returns the last-known nickname used by the contact during
   communications via any online account about which this detail
   aggregates presence information.
 */

/*!
   \fn QContactGlobalPresence::setPresenceState(QContactPresence::PresenceState presenceState)

   Sets the presence state of this aggregate detail according to the presence
   information available from the presence providers which this detail aggregates
   to the given \a presenceState.
 */

/*!
   \fn QContactGlobalPresence::presenceState() const

   Returns the aggregate presence state of any online accounts about which this detail
   aggregates presence information.
 */

/*!
   \fn QContactGlobalPresence::setPresenceStateText(const QString& presenceStateText)

   Sets the text corresponding to the presence state to \a presenceStateText.
   This function is generally called by presence providers to allow custom
   naming of states, or to allow finer grained state reporting than is
   provided by the presence state API.
 */

/*!
   \fn QContactGlobalPresence::presenceStateText() const

   Returns the text corresponding to the current presence state.
 */

/*!
  \fn QContactGlobalPresence::setCustomMessage(const QString& customMessage)

   Sets the custom status message from the contact for the aggregate presence
   detail, to \a customMessage.
 */

/*!
   \fn QContactGlobalPresence::customMessage() const

   Returns the custom status message from the contact for the aggregate presence
   detail.
 */

/*!
   \fn QContactGlobalPresence::setPresenceStateImageUrl(const QUrl& presenceStateImageUrl)

   Sets the last-known status image url of the contact to \a presenceStateImageUrl.
 */

/*!
   \fn QContactGlobalPresence::presenceStateImageUrl() const

   Returns the last-known status image url of the contact.
 */



/* Convenience filters */

/*!
    Returns a filter suitable for finding contacts with a display label containing the specified
    \a label.
*/
QContactFilter QContactDisplayLabel::match(const QString &label)
{
    QContactDetailFilter f;
    f.setDetailDefinitionName(QContactDisplayLabel::DefinitionName,
                              QContactDisplayLabel::FieldLabel);
    f.setValue(label);
    f.setMatchFlags(QContactFilter::MatchContains);

    return f;
}

/*!
    Returns a filter suitable for finding contacts with a name with a first name containing the
    specified \a firstName and a last name containing the specified \a lastName.  If either
    parameter is empty, any value will match that component.
*/
QContactFilter QContactName::match(const QString &firstName, const QString &lastName)
{
    if (firstName.isEmpty()) {
        if (lastName.isEmpty()) {
            // Matches contacts that have a name
            QContactDetailFilter f;
            f.setDetailDefinitionName(QContactName::DefinitionName);
            return f;
        } else {
            // Contact with matching lastname
            QContactDetailFilter f;
            f.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldLastName);
            f.setValue(lastName);
            f.setMatchFlags(QContactFilter::MatchContains);
            return f;
        }
    } else {
        if (lastName.isEmpty()) {
            // Contact with matching firstName
            QContactDetailFilter f;
            f.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirstName);
            f.setValue(firstName);
            f.setMatchFlags(QContactFilter::MatchContains);
            return f;
        } else {
            // Match a contact with the specified first and last names
            // XXX This needs multi detail filter!

            // Best we can currently do is "and" and assume there's only one name per contact
            QContactDetailFilter f;
            f.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirstName);
            f.setValue(firstName);
            f.setMatchFlags(QContactFilter::MatchContains);
            QContactDetailFilter l;
            l.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldLastName);
            l.setValue(lastName);
            l.setMatchFlags(QContactFilter::MatchContains);

            return f & l;
        }
    }
}

/*!
    Returns a filter suitable for finding contacts with any name field (e.g. first, last) that
    contains the supplied \a name.
*/
QContactFilter QContactName::match(const QString &name)
{
    QContactUnionFilter nameFilter;
    QStringList nameFields;
    nameFields << QContactName::FieldCustomLabel
            << QContactName::FieldFirstName
            << QContactName::FieldLastName
            << QContactName::FieldMiddleName
            << QContactName::FieldPrefix
            << QContactName::FieldSuffix;
    foreach (const QString& fieldName, nameFields) {
        QContactDetailFilter subFilter;
        subFilter.setDetailDefinitionName(QContactName::DefinitionName, fieldName);
        subFilter.setValue(name);
        subFilter.setMatchFlags(QContactFilter::MatchContains);
        nameFilter.append(subFilter);
    }
    return nameFilter;
}

/*!
    Returns a filter suitable for finding contacts with an email address containing the specified
    \a emailAddress.
*/
QContactFilter QContactEmailAddress::match(const QString &emailAddress)
{
    QContactDetailFilter l;
    l.setDetailDefinitionName(QContactEmailAddress::DefinitionName, QContactEmailAddress::FieldEmailAddress);
    l.setValue(emailAddress);
    l.setMatchFlags(QContactFilter::MatchContains);
    return l;
}

/*!
    Returns a filter suitable for finding contacts with a phone number containing the specified
    \a number.
*/
QContactFilter QContactPhoneNumber::match(const QString &number)
{
    QContactDetailFilter l;
    l.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
    l.setValue(number);
    l.setMatchFlags(QContactFilter::MatchPhoneNumber);
    return l;
}


QTM_END_NAMESPACE
