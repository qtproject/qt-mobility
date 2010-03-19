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
Q_DEFINE_LATIN1_LITERAL(QContactName::DefinitionName, "Name");

/*!
   \variable QContactNickname::DefinitionName
   The constant string which identifies the definition of details which are nicknames.
 */
Q_DEFINE_LATIN1_LITERAL(QContactNickname::DefinitionName, "Nickname");

/*!
   \variable QContactNote::DefinitionName
   The constant string which identifies the definition of details which are notes.
 */
Q_DEFINE_LATIN1_LITERAL(QContactNote::DefinitionName, "Note");

/*!
   \variable QContactAvatar::DefinitionName
   The constant string which identifies the definition of details which are avatars.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAvatar::DefinitionName, "Avatar");

/*!
   \variable QContactAddress::DefinitionName
   The constant string which identifies the definition of details which are street addresses.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::DefinitionName, "Address");

/*!
   \variable QContactPhoneNumber::DefinitionName
   The constant string which identifies the definition of details which are phone numbers.
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::DefinitionName, "PhoneNumber");

/*!
   \variable QContactPresence::DefinitionName
   The constant string which identifies the definition of details which contain presence information.
 */
Q_DEFINE_LATIN1_LITERAL(QContactPresence::DefinitionName, "Presence");

/*!
   \variable QContactPresence::FieldTimestamp

   The constant key for which the last update timestamp value is stored in details
   of the QContactPresence type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactPresence::FieldTimestamp, "Timestamp");

/*!
   \variable QContactPresence::FieldNickname

   The constant key for which the nickname value is stored in details
   of the QContactPresence type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactPresence::FieldNickname, "Nickname");

/*!
   \variable QContactPresence::FieldPresence

   The constant key for which the presence state value is stored in details
   of the QContactPresence typel.
 */
Q_DEFINE_LATIN1_LITERAL(QContactPresence::FieldPresenceState, "PresenceState");

/*!
   \variable QContactPresence::FieldPresenceStateText

   The constant key for which the presence provider provided text representation
   of the presence state is stored in details of the QContactPresence type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactPresence::FieldPresenceStateText, "PresenceStateText");

/*!
   \variable QContactPresence::FieldPresenceStateImageUrl

   The constant key for which the image url value for the current presence state
   is stored in details of the QContactPresence type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactPresence::FieldPresenceStateImageUrl, "PresenceStateImageUrl");

/*!
   \variable QContactPresence::FieldCustomMessage

   The constant key for which the user-entered custom message for their state
   is stored in details of the QContactPresence type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactPresence::FieldCustomMessage, "CustomMessage");

/*!
   \variable QContactRingtone::DefinitionName
   The constant string which identifies the definition of details which are ringtones.
 */
Q_DEFINE_LATIN1_LITERAL(QContactRingtone::DefinitionName, "Ringtone");

/*!
   \variable QContactSyncTarget::DefinitionName
   The constant string which identifies the definition of details which are synchronization target stores.
 */
Q_DEFINE_LATIN1_LITERAL(QContactSyncTarget::DefinitionName, "SyncTarget");

/*!
   \variable QContactTimestamp::DefinitionName
   The constant string which identifies the definition of details which are contact synchronization timestamps.
 */
Q_DEFINE_LATIN1_LITERAL(QContactTimestamp::DefinitionName, "Timestamp");

/*!
   \variable QContactType::DefinitionName
   The constant string which identifies the definition of details which identify the type of the contact.
 */
Q_DEFINE_LATIN1_LITERAL(QContactType::DefinitionName, "Type");

/*!
   \variable QContactGuid::DefinitionName
   The constant string which identifies the definition of details which are globally unique identifiers.
 */
Q_DEFINE_LATIN1_LITERAL(QContactGuid::DefinitionName, "Guid");

/*!
   \variable QContactEmailAddress::DefinitionName
   The constant string which identifies the definition of details which are email addresses.
 */
Q_DEFINE_LATIN1_LITERAL(QContactEmailAddress::DefinitionName, "EmailAddress");

/*!
   \variable QContactFamily::DefinitionName
   The constant string which identifies the definition of details which contain the names of the contact's family.
 */
Q_DEFINE_LATIN1_LITERAL(QContactFamily::DefinitionName, "Family");

/*!
   \variable QContactUrl::DefinitionName
   The constant string which identifies the definition of details which are universal resource location paths.
 */
Q_DEFINE_LATIN1_LITERAL(QContactUrl::DefinitionName, "Url");

/*!
   \variable QContactBirthday::DefinitionName
   The constant string which identifies the definition of details which are the dates of birthdays.
 */
Q_DEFINE_LATIN1_LITERAL(QContactBirthday::DefinitionName, "Birthday");

/*!
   \variable QContactAnniversary::DefinitionName
   The constant string which identifies the definition of details which are anniversary dates.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::DefinitionName, "Anniversary");

/*!
   \variable QContactGender::DefinitionName

   The constant string which identifies the definition of details
   which identify the gender of a contact in a given context.
 */
Q_DEFINE_LATIN1_LITERAL(QContactGender::DefinitionName, "Gender");

/*!
   \variable QContactGeoLocation::DefinitionName

   The constant string which identifies the definition of details
   which describe a location associated with a contact.
 */
Q_DEFINE_LATIN1_LITERAL(QContactGeoLocation::DefinitionName, "GeoLocation");

/*!
   \variable QContactGlobalPresence::DefinitionName
   The constant string which identifies the definition of details which contain presence information.
 */
Q_DEFINE_LATIN1_LITERAL(QContactGlobalPresence::DefinitionName, "GlobalPresence");

/*!
   \variable QContactGlobalPresence::FieldTimestamp

   The constant key for which the last update timestamp value is stored in details
   of the QContactGlobalPresence type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactGlobalPresence::FieldTimestamp, "Timestamp");

/*!
   \variable QContactGlobalPresence::FieldNickname

   The constant key for which the nickname value is stored in details
   of the QContactGlobalPresence type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactGlobalPresence::FieldNickname, "Nickname");

/*!
   \variable QContactGlobalPresence::FieldPresence

   The constant key for which the presence state value is stored in details
   of the QContactGlobalPresence typel.
 */
Q_DEFINE_LATIN1_LITERAL(QContactGlobalPresence::FieldPresenceState, "PresenceState");

/*!
   \variable QContactGlobalPresence::FieldPresenceStateText

   The constant key for which the presence provider provided text representation
   of the presence state is stored in details of the QContactGlobalPresence type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactGlobalPresence::FieldPresenceStateText, "PresenceStateText");

/*!
   \variable QContactGlobalPresence::FieldPresenceStateImageUrl

   The constant key for which the image url value for the current presence state
   is stored in details of the QContactGlobalPresence type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactGlobalPresence::FieldPresenceStateImageUrl, "PresenceStateImageUrl");

/*!
   \variable QContactGlobalPresence::FieldCustomMessage

   The constant key for which the user-entered custom message for their state
   is stored in details of the QContactGlobalPresence type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactGlobalPresence::FieldCustomMessage, "CustomMessage");

/*!
   \variable QContactOnlineAccount::DefinitionName

   The constant string which identifies the definition of details
   which identify the organization to which a contact belongs in a
   given context.
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::DefinitionName, "OnlineAccount");

/*!
   \variable QContactTag::DefinitionName
   The constant string which identifies the definition of details which are tags.
 */
Q_DEFINE_LATIN1_LITERAL(QContactTag::DefinitionName, "Tag");

/*!
   \variable QContactThumbnail::DefinitionName
   The constant string which identifies the definition of details which are thumbnails.
 */
Q_DEFINE_LATIN1_LITERAL(QContactThumbnail::DefinitionName, "Thumbnail");

/*!
   \deprecated
   \variable QContactOnlineAccount::FieldNickname

   The constant key for which the nickname value is stored in details
   of the QContactOnlineAccount type.

   This field is deprecated and will be removed after the transition period has elapsed.
   Use QContactPresence::FieldNickname instead!
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::FieldNickname, "Nickname");

/*!
   \deprecated
   \variable QContactOnlineAccount::FieldPresence

   The constant key for which the presence value is stored in details
   of the QContactOnlineAccount type.

   This field is deprecated and will be removed after the transition period has elapsed.
   Use QContactPresence::FieldPresence instead!
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::FieldPresence, "Presence");

/*!
   \deprecated
   \variable QContactOnlineAccount::PresenceAvailable

   The value for presence which specifies that the contact's current

   This key is deprecated and will be removed after the transition period has elapsed.
   Use QContactPresence::PresenceAvailable instead!
   status is available.
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::PresenceAvailable, "Available");

/*!
   \deprecated
   \variable QContactOnlineAccount::PresenceHidden

   The value for presence which specifies that the contact's current.

   This key is deprecated and will be removed after the transition period has elapsed.
   Use QContactPresence::PresenceHidden instead!
   status is hidden
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::PresenceHidden, "Hidden");

/*!
   \deprecated
   \variable QContactOnlineAccount::PresenceBusy
   The value for presence which specifies that the contact's current status is busy.

   This key is deprecated and will be removed after the transition period has elapsed.
   Use QContactPresence::PresenceBusy instead!
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::PresenceBusy, "Busy");

/*!
   \deprecated
   \variable QContactOnlineAccount::PresenceAway
   The value for presence which specifies that the contact's current status is away.

   This key is deprecated and will be removed after the transition period has elapsed.
   Use QContactPresence::PresenceAway instead!
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::PresenceAway, "Away");

/*!
   \deprecated
   \variable QContactOnlineAccount::PresenceExtendedAway
   The value for presence which specifies that the contact's current status is extended away.

   This key is deprecated and will be removed after the transition period has elapsed.
   Use QContactPresence::PresenceExtendedAway instead!
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::PresenceExtendedAway, "ExtendedAway");

/*!
   \deprecated
   \variable QContactOnlineAccount::PresenceUnknown
   The value for presence which specifies that the contact's current status is unknown.

   This key is deprecated and will be removed after the transition period has elapsed.
   Use QContactPresence::PresenceUnknown instead!
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::PresenceUnknown, "Unknown");

/*!
   \deprecated
   \variable QContactOnlineAccount::PresenceOffline
   The value for presence which specifies that the contact's current status is offline.

   This key is deprecated and will be removed after the transition period has elapsed.
   Use QContactPresence::PresenceOffline instead!
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::PresenceOffline, "Offline");

/*!
   \deprecated
   \variable QContactOnlineAccount::FieldStatusMessage

   The constant key for which the status message value is stored in
   details of the QContactOnlineAccount type.

   This field is deprecated and will be removed after the transition period has elapsed.
   Use QContactPresence::FieldStatusMessage instead!
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::FieldStatusMessage, "StatusMessage");

/*!
   \variable QContactOnlineAccount::FieldCapabilities

   The constant key for which the account capabilities value is stored in
   details of the QContactOnlineAccount type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::FieldCapabilities, "Capabilities");

/*!
   \variable QContactOnlineAccount::FieldAccountUri

   The constant key for which the remote account uri value is stored
   in details of the QContactOnlineAccount type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::FieldAccountUri, "AccountUri");

/*!
   \variable QContactOnlineAccount::FieldServiceProvider

   The constant key for which the service provider value is stored in
   details of the QContactOnlineAccount type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::FieldServiceProvider, "ServiceProvider");

/*!
   \variable QContactOnlineAccount::FieldSubTypes

   The constant key for which the subtypes value is stored in details
   of the QContactOnlineAccount type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::FieldSubTypes, "SubTypes");

/*!
   \variable QContactOnlineAccount::SubTypeSip

   The constant attribute value which describes the online account as
   supporting SIP.
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::SubTypeSip, "Sip");

/*!
   \variable QContactOnlineAccount::SubTypeSipVoip

   The constant attribute value which describes the online account as
   supporting SIP-based VoIP.
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::SubTypeSipVoip, "SipVoip");

/*!
   \variable QContactOnlineAccount::SubTypeImpp

   The constant attribute value which describes the online account as
   supporting IMPP.
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::SubTypeImpp, "Impp");

/*!
   \variable QContactOnlineAccount::SubTypeVideoShare

   The constant attribute value which describes the online account as
   supporting VideoShare.
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::SubTypeVideoShare, "VideoShare");

/*!
   \variable QContactOrganization::DefinitionName

   The constant string which identifies the definition of details
   which identify the organization to which a contact belongs in a
   given context.
 */
Q_DEFINE_LATIN1_LITERAL(QContactOrganization::DefinitionName, "Organization");

/*!
   \variable QContactDisplayLabel::DefinitionName

   The constant string which identifies the definition of details
   which contain a display label of a contact.
 */
Q_DEFINE_LATIN1_LITERAL(QContactDisplayLabel::DefinitionName, "DisplayLabel");


/*!
   \variable QContactPhoneNumber::FieldNumber

   The constant key for which the phone number value is stored in
   details of the QContactPhoneNumber type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::FieldNumber, "PhoneNumber");

/*!
   \variable QContactPhoneNumber::FieldSubTypes

   The constant key for which the subtype values are stored in details
   of the QContactPhoneNumber type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::FieldSubTypes, "SubTypes");

/*!
   \variable QContactEmailAddress::FieldEmailAddress

   The constant key for which the email address value is stored in
   details of the QContactEmailAddress type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactEmailAddress::FieldEmailAddress, "EmailAddress");

/*!
   \variable QContactFamily::FieldSpouse

   The constant key for which the spouse name value is stored in
   details of the QContactFamily type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactFamily::FieldSpouse, "Spouse");

/*!
   \variable QContactFamily::FieldChildren

   The constant key for which the children names value is stored in
   details of the QContactFamily type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactFamily::FieldChildren, "Children");

/*!
   \variable QContactGuid::FieldGuid

   The constant key for which the globally unique identifier value is
   stored in details of the QContactGuid type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactGuid::FieldGuid, "Guid");

/*!
   \variable QContactSyncTarget::FieldSyncTarget

   The constant key for which the value of the target store for
   synchronization is stored in details of the QContactSyncTarget type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactSyncTarget::FieldSyncTarget, "SyncTarget");

/*!
   \variable QContactAvatar::FieldImageUrl

   The constant key for which the url of the avatar image value is stored in
   details of the QContactAvatar type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAvatar::FieldImageUrl, "ImageUrl");

/*!
   \variable QContactAvatar::FieldVideoUrl

   The constant key for which the url of the avatar video value is stored in
   details of the QContactAvatar type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAvatar::FieldVideoUrl, "VideoUrl");

/*!
   \deprecated
   \variable QContactAvatar::FieldAvatar

   The constant key for which the uri of the avatar value is stored in
   details of the QContactAvatar type.

   This field is deprecated and will be removed after the transition period has elapsed.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAvatar::FieldAvatar, "Avatar");

/*!
   \deprecated
   \variable QContactAvatar::FieldAvatarPixmap

   The constant key for which the pixmap of the avatar value is stored in
   details of the QContactAvatar type.

   This field is deprecated and will be removed after the transition period has elapsed.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAvatar::FieldAvatarPixmap, "AvatarPixmap");

/*!
   \deprecated
   \variable QContactAvatar::FieldSubType

   The constant key for which the subtype value is stored in
   details of the QContactAvatar type.

   This field is deprecated and will be removed after the transition period has elapsed.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAvatar::FieldSubType, "SubType");

/*!
   \variable QContactName::FieldPrefix

   The constant key for which the name prefix value is stored in
   details of the QContactName type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactName::FieldPrefix, "Prefix");

/*!
   \variable QContactName::FieldFirstName

   The constant key for which the first name value is stored in
   details of the QContactName type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactName::FieldFirstName, "FirstName");

/*!
   \variable QContactName::FieldMiddleName

   The constant key for which the middle name value is stored in
   details of the QContactName type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactName::FieldMiddleName, "MiddleName");

/*!
   \variable QContactName::FieldLastName

   The constant key for which the last name value is stored in details
   of the QContactName type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactName::FieldLastName, "LastName");

/*!
   \variable QContactName::FieldFirst

   The constant key for which the first name value is stored in
   details of the QContactName type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactName::FieldFirst, "FirstName");

/*!
   \variable QContactName::FieldMiddle

   The constant key for which the middle name value is stored in
   details of the QContactName type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactName::FieldMiddle, "MiddleName");

/*!
   \variable QContactName::FieldLast

   The constant key for which the last name value is stored in details
   of the QContactName type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactName::FieldLast, "LastName");

/*!
   \variable QContactName::FieldSuffix

   The constant key for which the name suffix value is stored in
   details of the QContactName type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactName::FieldSuffix, "Suffix");

/*!
   \variable QContactName::FieldCustomLabel

   The constant key for which the custom name label value is stored in
   details of the QContactName type, if supported.
 */
Q_DEFINE_LATIN1_LITERAL(QContactName::FieldCustomLabel, "CustomLabel");

/*!
   \variable QContactNickname::FieldNickname

   The constant key for which the nickname value is stored in details
   of the QContactNickname type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactNickname::FieldNickname, "Nickname");

/*!
   \variable QContactNote::FieldNote

   The constant key for which the note value is stored in details of
   the QContactNote type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactNote::FieldNote, "Note");

/*!
   \variable QContactAddress::FieldStreet

   The constant key for which the street value is stored in details of
   the QContactAddress type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::FieldStreet, "Street");

/*!
   \variable QContactAddress::FieldLocality

   The constant key for which the locality value is stored in details
   of the QContactAddress type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::FieldLocality, "Locality");

/*!
   \variable QContactAddress::FieldRegion

   The constant key for which the region value is stored in details of
   the QContactAddress type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::FieldRegion, "Region");

/*!
   \variable QContactAddress::FieldPostcode

   The constant key for which the postcode value is stored in details
   of the QContactAddress type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::FieldPostcode, "Postcode");

/*!
   \variable QContactAddress::FieldCountry

   The constant key for which the country value is stored in details
   of the QContactAddress type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::FieldCountry, "Country");

/*!
   \variable QContactAddress::FieldPostOfficeBox

   The constant key for which the post office box value is stored in
   details of the QContactAddress type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::FieldPostOfficeBox, "PostOfficeBox");

/*!
   \variable QContactAddress::FieldSubTypes

   The constant key for which the subtypes value is stored in details
   of the QContactAddress type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::FieldSubTypes, "SubTypes");

/*!
   \variable QContactUrl::FieldUrl

   The constant key for which the url value is stored in details of
   the QContactUrl type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactUrl::FieldUrl, "Url");

/*!
   \variable QContactUrl::FieldSubType

   The constant key for which the subtypes value is stored in details
   of the QContactUrl type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactUrl::FieldSubType, "SubType");

/*!
   \variable QContactBirthday::FieldBirthday

   The constant key for which the birthday date value is stored in
   details of the QContactBirthday type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactBirthday::FieldBirthday, "Birthday");

/*!
   \variable QContactAnniversary::FieldOriginalDate

   The constant key for which the anniversary original event date
   value is stored in details of the QContactAnniversary type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::FieldOriginalDate, "OriginalDate");

/*!
   \variable QContactAnniversary::FieldEvent

   The constant key for which the name of the event is stored in
   details of the QContactAnniversary type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::FieldEvent, "Event");

/*!
   \variable QContactAnniversary::FieldCalendarId

   The constant key for which the value of the identifier of the
   associated calendar entry is stored in details of the
   QContactAnniversary type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::FieldCalendarId, "CalendarId");

/*!
   \variable QContactAnniversary::FieldSubType

   The constant key for which the subtypes value is stored in details
   of the QContactAnniversary type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::FieldSubType, "SubType");

/*!
   \variable QContactDisplayLabel::FieldLabel

   The constant key for which the display label value is stored in
   details of the QContactDisplayLabel type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactDisplayLabel::FieldLabel, "Label");

/*!
   \variable QContactGender::FieldGender

   The constant key for which the gender value is stored in details of
   the QContactGender type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactGender::FieldGender, "Gender");

/*!
   \variable QContactGender::GenderMale
   The value that identifies this contact as being male.
 */
Q_DEFINE_LATIN1_LITERAL(QContactGender::GenderMale, "Male");

/*!
   \variable QContactGender::GenderFemale
   The value that identifies this contact as being female.
 */
Q_DEFINE_LATIN1_LITERAL(QContactGender::GenderFemale, "Female");

/*!
   \variable QContactGender::GenderUnspecified
   The value that identifies this contact as being of unspecified gender.
 */
Q_DEFINE_LATIN1_LITERAL(QContactGender::GenderUnspecified, "Unspecified");

/*!
   \variable QContactGeoLocation::FieldLabel

   The constant key for which the location label value is stored in
   details of the QContactGeoLocation type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactGeoLocation::FieldLabel, "Label");

/*!
   \variable QContactGeoLocation::FieldLatitude

   The constant key for which the latitude value is stored in details
   of the QContactGeoLocation type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactGeoLocation::FieldLatitude, "Latitude");

/*!
   \variable QContactGeoLocation::FieldLongitude

   The constant key for which the longitude value is stored in details
   of the QContactGeoLocation type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactGeoLocation::FieldLongitude, "Longitude");

/*!
   \variable QContactGeoLocation::FieldAccuracy

   The constant key for which the location accuracy value is stored in
   details of the QContactGeoLocation type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactGeoLocation::FieldAccuracy, "Accuracy");

/*!
   \variable QContactGeoLocation::FieldAltitude

   The constant key for which the altitude value is stored in details
   of the QContactGeoLocation type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactGeoLocation::FieldAltitude, "Altitude");


/*!
   \variable QContactGeoLocation::FieldAltitudeAccuracy

   The constant key for which the altitude accuracy value is stored in
   details of the QContactGeoLocation type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactGeoLocation::FieldAltitudeAccuracy, "AltitudeAccuracy");

/*!
   \variable QContactGeoLocation::FieldHeading

   The constant key for which the heading value is stored in details
   of the QContactGeoLocation type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactGeoLocation::FieldHeading, "Heading");

/*!
   \variable QContactGeoLocation::FieldSpeed

   The constant key for which the speed value is stored in details of
   the QContactGeoLocation type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactGeoLocation::FieldSpeed, "Speed");

/*!
   \variable QContactTag::FieldTag

   The constant key for which the tag value is stored in details
   of the QContactTag type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactTag::FieldTag, "Tag");

/*!
   \variable QContactGeoLocation::FieldTimestamp

   The constant key for which the timestamp value is stored in details
   of the QContactGeoLocation type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactGeoLocation::FieldTimestamp, "Timestamp");

/*!
   \variable QContactOrganization::FieldName

   The constant key for which the name value is stored in details of
   the QContactOrganization type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactOrganization::FieldName, "Name");

/*!
   \variable QContactOrganization::FieldLogo

   The constant key for which the logo path value is stored in details
   of the QContactOrganization type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactOrganization::FieldLogo, "Logo");

/*!
   \variable QContactOrganization::FieldDepartment

   The constant key for which the organization's department value is
   stored in details of the QContactOrganization type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactOrganization::FieldDepartment, "Department");

/*!
   \variable QContactOrganization::FieldLocation

   The constant key for which the organization's location (or the
   location of the contact's part of the organization) value is stored
   in details of the QContactOrganization type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactOrganization::FieldLocation, "Location");

/*!
   \variable QContactOrganization::FieldRole

   The constant key for which the contact's role within the
   organization is stored in details of the QContactOrganization type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactOrganization::FieldRole, "Role");

/*!
   \variable QContactOrganization::FieldTitle

   The constant key for which the contact's title within the
   organization is stored in details of the QContactOrganization type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactOrganization::FieldTitle, "Title");

/*!
   \variable QContactOrganization::FieldAssistantName

   The constant key for which the contact's assistant name within the
   organization is stored in details of the QContactOrganization type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactOrganization::FieldAssistantName, "AssistantName");

/*!
   \variable QContactRingtone::FieldAudioRingtone

   The constant key for which the uri of the audio ringtone value is
   stored in details of the QContactRingtone type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactRingtone::FieldAudioRingtone, "AudioRingtone");

/*!
   \variable QContactRingtone::FieldVideoRingtone

   The constant key for which the uri of the video ringtone value is
   stored in details of the QContactRingtone type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactRingtone::FieldVideoRingtone, "VideoRingtone");

/*!
   \variable QContactRingtone::FieldVibrationRingtone

   The constant key for which the uri of the vibration ringtone value is
   stored in details of the QContactRingtone type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactRingtone::FieldVibrationRingtone, "VibrationRingtone");

/*!
   \variable QContactThumbnail::FieldThumbnail

   The constant key for which the thumbnail image is stored in details
   of the QContactThumbnail type.
 */
Q_DEFINE_LATIN1_LITERAL(QContactThumbnail::FieldThumbnail, "Thumbnail");

/*!
   \variable QContactTimestamp::FieldModificationTimestamp

   The constant key for the value which is stored in details of the
   QContactTimestamp type which describes the last modification date
   and time of a contact.
 */
Q_DEFINE_LATIN1_LITERAL(QContactTimestamp::FieldModificationTimestamp, "ModificationTimestamp");

/*!
   \variable QContactTimestamp::FieldCreationTimestamp

   The constant key for the value which is stored in details of the
   QContactTimestamp type which describes the creation date and time
   of a contact.
 */
Q_DEFINE_LATIN1_LITERAL(QContactTimestamp::FieldCreationTimestamp, "CreationTimestamp");

/*!
   \variable QContactType::FieldType

   The constant key for the type value which is stored in details of
   the QContactType definition.
 */
Q_DEFINE_LATIN1_LITERAL(QContactType::FieldType, "Type");


/*!
   \variable QContactType::TypeContact

   The constant attribute value which describes the contact as being
   an ordinary contact.
 */
Q_DEFINE_LATIN1_LITERAL(QContactType::TypeContact, "Contact");

/*!
   \variable QContactType::TypeGroup

   The constant attribute value which describes the contact as being a
   group.
 */
Q_DEFINE_LATIN1_LITERAL(QContactType::TypeGroup, "Group");

/*!
   \variable QContactPhoneNumber::SubTypeLandline

   The constant attribute value which describes the phone number as
   identifying a landline phone.
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeLandline, "Landline");

/*!
   \variable QContactPhoneNumber::SubTypeMobile

   The constant attribute value which describes the phone number as
   identifying a mobile phone.
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeMobile, "Mobile");

/*!
   \variable QContactPhoneNumber::SubTypeFacsimile

   The constant attribute value which describes the phone number as
   identifying a facsimile machine.
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeFacsimile, "Facsimile");

/*!
   \variable QContactPhoneNumber::SubTypePager

   The constant attribute value which describes the phone number as
   identifying a pager device.
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypePager, "Pager");

/*!
   \variable QContactPhoneNumber::SubTypeCar

   The constant attribute value which describes the phone number as
   identifying a car phone.
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeCar, "Car");

/*!
   \variable QContactPhoneNumber::SubTypeBulletinBoardSystem

   The constant attribute value which describes the phone number as
   identifying a bulletin board system.
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeBulletinBoardSystem, "BulletinBoardSystem");

/*!
   \variable QContactPhoneNumber::SubTypeVoice

   The constant attribute value which describes the phone number as
   supporting voice transmission.
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeVoice, "Voice");

/*!
   \variable QContactPhoneNumber::SubTypeModem

   The constant attribute value which describes the phone number as
   supporting digital data transfer.
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeModem, "Modem");

/*!
   \variable QContactPhoneNumber::SubTypeVideo

   The constant attribute value which describes the phone number as
   supporting video transmission.
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeVideo, "Video");

/*!
   \variable QContactPhoneNumber::SubTypeMessagingCapable

   The constant attribute value which describes the phone number as
   supporting messaging services.
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeMessagingCapable, "MessagingCapable");

/*!
   \variable QContactPhoneNumber::SubTypeAssistant

   The constant attribute value which describes the phone number as an
   assistant phone number.
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeAssistant, "Assistant");

/*!
   \variable QContactPhoneNumber::SubTypeDtmfMenu

   The constant attribute value which describes the phone number as
   supporting DTMF-controlled electronic menu navigation.
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeDtmfMenu, "DtmfMenu");

/*!
   \variable QContactAddress::SubTypeParcel

   The constant attribute value which describes the address as being
   an address for parcel delivery.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::SubTypeParcel, "Parcel");

/*!
   \variable QContactAddress::SubTypePostal

   The constant attribute value which describes the address as being
   an address for postal delivery.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::SubTypePostal, "Postal");

/*!
   \variable QContactAddress::SubTypeDomestic

   The constant attribute value which describes the address as being a
   domestic address.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::SubTypeDomestic, "Domestic");

/*!
   \variable QContactAddress::SubTypeInternational

   The constant attribute value which describes the address as being
   an international address.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::SubTypeInternational, "International");

/*!
   \deprecated
   \variable QContactAvatar::SubTypeImage

   The constant attribute value which describes the avatar as being an
   image.

   This value is deprecated and should not be used, and will be removed after the transition period has elapsed.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAvatar::SubTypeImage, "Image");

/*!
   \deprecated
   \variable QContactAvatar::SubTypeVideo

   The constant attribute value which describes the avatar as being a
   video.

   This value is deprecated and should not be used, and will be removed after the transition period has elapsed.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAvatar::SubTypeVideo, "Video");

/*!
   \deprecated
   \variable QContactAvatar::SubTypeAudioRingtone

   The constant attribute value which describes the avatar as being an
   audio ringtone.

   This value is deprecated and should not be used, and will be removed after the transition period has elapsed.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAvatar::SubTypeAudioRingtone, "AudioRingtone");

/*!
   \deprecated
   \variable QContactAvatar::SubTypeVideoRingtone

   The constant attribute value which describes the avatar as being a
   video ringtone.

   This value is deprecated and should not be used, and will be removed after the transition period has elapsed.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAvatar::SubTypeVideoRingtone, "VideoRingtone");


/*!
   \deprecated
   \variable QContactAvatar::SubTypeTexturedMesh

   The constant attribute value which describes the avatar as being a
   textured, 3D mesh.

   This value is deprecated and should not be used, and will be removed after the transition period has elapsed.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAvatar::SubTypeTexturedMesh, "TexturedMesh");

/*!
   \variable QContactUrl::SubTypeHomePage

   The constant attribute value which describes the url as being the
   homepage of the contact.
 */
Q_DEFINE_LATIN1_LITERAL(QContactUrl::SubTypeHomePage, "HomePage");

/*!
   \variable QContactUrl::SubTypeFavourite

   The constant attribute value which describes the url as being a
   favourite page of the contact.
 */
Q_DEFINE_LATIN1_LITERAL(QContactUrl::SubTypeFavourite, "Favourite");

/*!
   \variable QContactAnniversary::SubTypeWedding

   The constant attribute value which describes the anniversary as
   being a wedding anniversary.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::SubTypeWedding, "Wedding");

/*!
   \variable QContactAnniversary::SubTypeEngagement

   The constant attribute value which describes the anniversary as
   being an engagement anniversary.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::SubTypeEngagement, "Engagement");

/*!
   \variable QContactAnniversary::SubTypeHouse

   The constant attribute value which describes the anniversary as
   being an anniversary of ownership of a particular residence.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::SubTypeHouse, "House");

/*!
   \variable QContactAnniversary::SubTypeEmployment

   The constant attribute value which describes the anniversary as
   being an anniversary of employment at a particular company.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::SubTypeEmployment, "Employment");

/*!
   \variable QContactAnniversary::SubTypeMemorial

   The constant attribute value which describes the anniversary as
   being an anniversary of an event of sentimental significance.
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::SubTypeMemorial, "Memorial");


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
  \fn QContactAvatar::imageUrl(const QUrl& imageUrl)
  Returns the url of an avatar image associated with the contact
 */

/*!
  \fn QContactAvatar::setImageUrl(const QUrl& imageUrl)
  Sets the url of an avatar image associated with the contact to \a imageUrl
 */

/*!
  \fn QContactAvatar::videoUrl(const QUrl& videoUrl)
  Returns the url of an avatar video associated with the contact
 */

/*!
  \fn QContactAvatar::setVideoUrl(const QUrl& videoUrl)
  Sets the url of an avatar video associated with the contact to \a imageUrl
 */

/*!
   \deprecated
   \fn QContactAvatar::avatar() const
   Returns the location of an avatar file associated with the contact.
   This function is deprecated and will be removed after the transition period has elapsed.
 */
QString QContactAvatar::avatar() const
{
    qWarning("QContactAvatar::avatar() This function is deprecated and will be removed after the transition period has elapsed!  Use imageUri() or QContactThumbnail instead!");
    return QString();
}

/*!
   \deprecated
   \fn QContactAvatar::setAvatar(const QString& avatar)
   Sets the location of an avatar file associated with the contact to \a avatar.
   This function is deprecated and will be removed after the transition period has elapsed.
 */
void QContactAvatar::setAvatar(const QString& avatar)
{
    Q_UNUSED(avatar);
    qWarning("QContactAvatar::setAvatar() This function is deprecated and will be removed after the transition period has elapsed!  Use setImageUri() or QContactThumbnail instead!");
}

/*!
   \deprecated
   \fn QContactAvatar::pixmap() const
   Returns a thumbnail for a picture associated with this contact.
   This function is deprecated and will be removed after the transition period has elapsed.
 */
QPixmap QContactAvatar::pixmap() const
{
    qWarning("QContactAvatar::pixmap() This function is deprecated and will be removed after the transition period has elapsed!  Use imageUri() or QContactThumbnail instead!");
    return QPixmap();
}

/*!
   \deprecated
   \fn QContactAvatar::setPixmap(const QPixmap& pixmap)
   Sets the thumbnail of a picture avatar associated with the contact to \a pixmap.
   If \a pixmap is empty, the thumbnail pixmap will be removed.

   Returns true if the pixmap could be set, and false otherwise.
   This function is deprecated and will be removed after the transition period has elapsed.
 */
void QContactAvatar::setPixmap(const QPixmap& pixmap)
{
    Q_UNUSED(pixmap);
    qWarning("QContactAvatar::setPixmap() This function is deprecated and will be removed after the transition period has elapsed!  Use setImageUri() or QContactThumbnail instead!");
}

/*!
   \deprecated
   \fn QContactAvatar::subType() const
   Returns the subtype that this detail implements, if defined.
   This function is deprecated and will be removed after the transition period has elapsed.
 */
QString QContactAvatar::subType() const
{
    qWarning("QContactAvatar::subType() This function is deprecated and will be removed after the transition period has elapsed!");
    return QString();
}

/*!
   \deprecated
   \fn QContactAvatar::setSubType(const QString& subType)
   Sets the subtype which this detail implements to be the given \a subType.
   This function is deprecated and will be removed after the transition period has elapsed.
 */
void QContactAvatar::setSubType(const QString& subtype)
{
    Q_UNUSED(subtype);
    qWarning("QContactAvatar::avatar() This function is deprecated and will be removed after the transition period has elapsed!");
}

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
   \deprecated
   \fn QContactOnlineAccount::setNickname(const QString& nickname)
   
   Sets the last-known nickname used by the contact during
   communications via the online account about which this detail
   stores presence information to \a nickname.

   This function is deprecated and will be removed after the transition period has elapsed.
   Use QContactPresence::setNickname() instead!
 */
void QContactOnlineAccount::setNickname(const QString& nickname)
{
    setValue(FieldNickname, nickname);
}

/*!
   \deprecated
   \fn QContactOnlineAccount::nickname() const
   
   Returns the last-known nickname used by the contact during
   communications via the online account.

   This function is deprecated and will be removed after the transition period has elapsed.
   Use QContactPresence::nickname() instead!
 */
QString QContactOnlineAccount::nickname() const
{
    return value(FieldNickname);
}

/*!
   \deprecated
   \fn QContactOnlineAccount::setPresence(const QString& presence)
   
   Sets the presence of the online account according to the presence
   information provider to \a presence.

   This function is deprecated and will be removed after the transition period has elapsed.
   Use QContactPresence::setPresence() instead!
 */
void QContactOnlineAccount::setPresence(const QString& presence)
{
    setValue(FieldPresence, presence);
}

/*!
   \deprecated
   \fn QContactOnlineAccount::presence() const
   
   Returns the presence of the online account according to the
   presence provider.

   This function is deprecated and will be removed after the transition period has elapsed.
   Use QContactPresence::presence() instead!
 */
QString QContactOnlineAccount::presence() const
{
    return value(FieldPresence);
}

/*!
   \deprecated
   \fn QContactOnlineAccount::setStatusMessage(const QString& statusMessage)

   Sets the last-known status message of the contact which was
   communicated via the online account about which this detail stores
   presence information to \a statusMessage.

   This function is deprecated and will be removed after the transition period has elapsed.
   Use QContactPresence::setStatusMessage() instead!
 */
void QContactOnlineAccount::setStatusMessage(const QString& statusMessage)
{
    setValue(FieldStatusMessage, statusMessage);
}

/*!
   \deprecated
   \fn QContactOnlineAccount::statusMessage() const
   
   Returns the last-known status message of the contact which was
   communicated via the online account about which this detail stores
   presence information.

   This function is deprecated and will be removed after the transition period has elapsed.
   Use QContactPresence::statusMessage() instead!
 */
QString QContactOnlineAccount::statusMessage() const
{
    return value(FieldStatusMessage);
}

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
   \fn QContactOrganization::setLogo(const QUrl& logo)
   Sets the logo of the organization stored in this detail to \a logo.
 */

/*!
   \fn QContactOrganization::logo() const
   Returns the logo of the organization stored in this detail.
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
  \fn QContactRingtone::audioRingtone() const

  Returns the uri of the audio ringtone stored in the ringtone detail.
 */

/*!
  \fn QContactRingtone::setAudioRingtone(const QUrl& audioRingtone)

  Sets the uri of the audio ringtone stored in the ringtone detail
  to \a audioRingtone.
 */

/*!
  \fn QContactRingtone::videoRingtone() const

  Returns the uri of the video ringtone stored in the ringtone detail.
 */

/*!
  \fn QContactRingtone::setVideoRingtone(const QUrl& videoRingtone)

  Sets the uri of the video ringtone stored in the ringtone detail
  to \a videoRingtone.
 */

/*!
  \fn QContactRingtone::vibrationRingtone() const

  Returns the uri of the vibration ringtone stored in the ringtone detail.
 */

/*!
  \fn QContactRingtone::setVibrationRingtone(const QUrl& vibrationRingtone)

  Sets the uri of the vibration ringtone stored in the ringtone detail
  to \a vibrationRingtone.
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

   Sets the text corresponding to the presence state.  This function is generally
   called by presence providers to allow custom naming of states, or to allow
   finer grained state reporting than is provided by the presence state API.
 */

/*!
   \fn QContactPresence::presenceStateText() const

   Returns the text corresponding to the current presence state.
 */

/*!
  \fn QContactPresence::setCustomMessage(const QString& customMessage)

   Sets the custom status message from the contact for the online account
   about which this detail stores presence information, to \a statusMessage.
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

   Sets the text corresponding to the presence state.  This function is generally
   called by presence providers to allow custom naming of states, or to allow
   finer grained state reporting than is provided by the presence state API.
 */

/*!
   \fn QContactGlobalPresence::presenceStateText() const

   Returns the text corresponding to the current presence state.
 */

/*!
  \fn QContactGlobalPresence::setCustomMessage(const QString& customMessage)

   Sets the custom status message from the contact for the aggregate presence
   detail, to \a statusMessage.
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
    Returns a filter suitable for finding
    contacts with a display label matching the specified \a label.
*/
QContactFilter QContactDisplayLabel::match(const QString &label)
{
    QContactDetailFilter f;
    f.setDetailDefinitionName(QContactDisplayLabel::DefinitionName);
    f.setValue(label);

    return f;
}

/*!
    Returns a filter suitable for finding
    contacts with a name matching the specified \a firstName and
    \a lastName.  If either parameter is empty, any value will match
    that component.
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
            return f;
        }
    } else {
        if (lastName.isEmpty()) {
            // Contact with matching firstName
            QContactDetailFilter f;
            f.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirstName);
            f.setValue(firstName);
            return f;
        } else {
            // Match a contact with the specified first and last names
            // XXX This needs multi detail filter!

            // Best we can currently do is "and" and assume there's only one name per contact
            QContactDetailFilter f;
            f.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirstName);
            f.setValue(firstName);
            QContactDetailFilter l;
            l.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldLastName);
            l.setValue(lastName);

            return f & l;
        }
    }
}

/*!
    Returns a filter suitable for finding
    contacts with a name field (e.g. first, last) that
    matches the supplied \a name.
*/
QContactFilter QContactName::match(const QString &name)
{
    QContactDetailFilter l;
    l.setDetailDefinitionName(QContactName::DefinitionName);
    l.setValue(name);
    return l;
}

/*!
    Returns a filter suitable for finding
    contacts with an email address matching the specified \a emailAddress.
*/
QContactFilter QContactEmailAddress::match(const QString &emailAddress)
{
    QContactDetailFilter l;
    l.setDetailDefinitionName(QContactEmailAddress::DefinitionName, QContactEmailAddress::FieldEmailAddress);
    l.setValue(emailAddress);
    return l;
}

/*!
    Returns a filter suitable for finding
    contacts with a phone number matching the specified \a number.
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
