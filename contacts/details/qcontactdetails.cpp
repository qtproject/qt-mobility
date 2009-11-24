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

#include "qcontactdetails.h"
#include <QImage>
#include <QUrl>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
/*!
 * \class QContactAddress
 * \brief The QContactAddress class provides a container for an address of a contact
 */

/*!
 * \class QContactAnniversary
 * \brief The QContactAnniversary class provides a container for an anniversary of a contact
 */

/*!
 * \class QContactAvatar
 * \brief The QContact class provides a container for an avatar of a contact
 */

/*!
 * \class QContactBirthday
 * \brief The QContactBirthday class provides a container for the birthday of a contact
 */

/*!
 * \class QContactDisplayLabel
 * \brief The QContactDisplayLabel class provides a container for the (possibly synthesized) display label of a contact
 */

/*!
 * \class QContactEmailAddress
 * \brief The QContactEmailAddress class provides a container for an email address of a contact
 */

/*!
 * \class QContactFamily
 * \brief The QContactFamily class provides a container for the names of the family members of a contact
 */

/*!
 * \class QContactGender
 * \brief The QContactGender class provides a container for the gender of a contact
 */

/*!
 * \class QContactGeolocation
 * \brief The QContactGeolocation class provides a container for a global location coordinate associated with a contact
 */

/*!
 * \class QContactGuid
 * \brief The QContactGuid class provides a container for the globally unique Id of a contact
 */

/*!
 * \class QContactName
 * \brief The QContactName class provides a container for the name of a contact
 */

/*!
 * \class QContactNickname
 * \brief The QContactNickname class provides a container for a nickname of a contact
 */

/*!
 * \class QContactNote
 * \brief The QContactNote class provides a container for a note associated with a contact
 */

/*!
 * \class QContactOnlineAccount
 * \brief The QContactOnlineAccount class provides a container for an online account which the contact uses to communicate with friends and family
 */

/*!
 * \class QContactOrganization
 * \brief The QContactOrganization class provides a container for details about an organization that the contact is either a part of, or stands for
 */

/*!
 * \class QContactPhoneNumber
 * \brief The QContactPhoneNumber class provides a container for a phone number of a contact
 */

/*!
 * \class QContactSyncTarget
 * \brief The QContactSyncTarget class provides a container for a sync target for a contact
 */

/*!
 * \class QContactTimestamp
 * \brief The QContactTimestamp class provides a container for the creation and last-modified timestamp associated with the contact
 */

/*!
 * \class QContactType
 * \brief The QContactType class provides a container for the type of the contact
 */

/*!
 * \class QContactUrl
 * \brief The QContactUrl class provides a container for a url associated with a contact
 */

/*!
 * \variable QContactName::DefinitionName
 * The constant string which identifies the definition of details which are names
 */
Q_DEFINE_LATIN1_LITERAL(QContactName::DefinitionName, "Name");

/*!
 * \variable QContactNickname::DefinitionName
 * The constant string which identifies the definition of details which are nicknames
 */
Q_DEFINE_LATIN1_LITERAL(QContactNickname::DefinitionName, "Nickname");

/*!
 * \variable QContactNote::DefinitionName
 * The constant string which identifies the definition of details which are notes
 */
Q_DEFINE_LATIN1_LITERAL(QContactNote::DefinitionName, "Note");

/*!
 * \variable QContactAvatar::DefinitionName
 * The constant string which identifies the definition of details which are avatars
 */
Q_DEFINE_LATIN1_LITERAL(QContactAvatar::DefinitionName, "Avatar");

/*!
 * \variable QContactAddress::DefinitionName
 * The constant string which identifies the definition of details which are street addresses
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::DefinitionName, "StreetAddress");

/*!
 * \variable QContactPhoneNumber::DefinitionName
 * The constant string which identifies the definition of details which are phone numbers
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::DefinitionName, "PhoneNumber");

/*!
 * \variable QContactSyncTarget::DefinitionName
 * The constant string which identifies the definition of details which are synchronisation target stores
 */
Q_DEFINE_LATIN1_LITERAL(QContactSyncTarget::DefinitionName, "SyncTarget");

/*!
 * \variable QContactTimestamp::DefinitionName
 * The constant string which identifies the definition of details which are contact synchronisation timestamps
 */
Q_DEFINE_LATIN1_LITERAL(QContactTimestamp::DefinitionName, "Timestamp");

/*!
 * \variable QContactType::DefinitionName
 * The constant string which identifies the definition of details which identify the type of the contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactType::DefinitionName, "Type");

/*!
 * \variable QContactGuid::DefinitionName
 * The constant string which identifies the definition of details which are globally unique identifiers
 */
Q_DEFINE_LATIN1_LITERAL(QContactGuid::DefinitionName, "Guid");

/*!
 * \variable QContactEmailAddress::DefinitionName
 * The constant string which identifies the definition of details which are email addresses
 */
Q_DEFINE_LATIN1_LITERAL(QContactEmailAddress::DefinitionName, "EmailAddress");

/*!
 * \variable QContactFamily::DefinitionName
 * The constant string which identifies the definition of details which contain the names of the contact's family
 */
Q_DEFINE_LATIN1_LITERAL(QContactFamily::DefinitionName, "Family");

/*!
 * \variable QContactUrl::DefinitionName
 * The constant string which identifies the definition of details which are universal resource location paths
 */
Q_DEFINE_LATIN1_LITERAL(QContactUrl::DefinitionName, "Url");

/*!
 * \variable QContactBirthday::DefinitionName
 * The constant string which identifies the definition of details which are the dates of birthdays
 */
Q_DEFINE_LATIN1_LITERAL(QContactBirthday::DefinitionName, "Birthday");

/*!
 * \variable QContactAnniversary::DefinitionName
 * The constant string which identifies the definition of details which are anniversary dates
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::DefinitionName, "Anniversary");

/*!
 * \variable QContactGender::DefinitionName
 * The constant string which identifies the definition of details which identify the gender of a contact in a given context
 */
Q_DEFINE_LATIN1_LITERAL(QContactGender::DefinitionName, "Gender");

/*!
 * \variable QContactGeolocation::DefinitionName
 * The constant string which identifies the definition of details which describe a location associated with a contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactGeolocation::DefinitionName, "Geolocation");

/*!
 * \variable QContactOnlineAccount::DefinitionName
 * The constant string which identifies the definition of details which identify the organization to which a contact belongs in a given context
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::DefinitionName, "OnlineAccount");

/*!
 * \variable QContactOnlineAccount::FieldNickname
 * The constant key for which the nickname value is stored in details of the QContactOnlineAccount type
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::FieldNickname, "Nickname");

/*!
 * \variable QContactOnlineAccount::FieldPresence
 * The constant key for which the presence value is stored in details of the QContactOnlineAccount type
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::FieldPresence, "Presence");

/*!
 * \variable QContactOnlineAccount::PresenceAvailable
 * The value for presence which specifies that the contact's current status is available
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::PresenceAvailable, "Available");

/*!
 * \variable QContactOnlineAccount::PresenceHidden
 * The value for presence which specifies that the contact's current status is hidden
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::PresenceHidden, "Hidden");

/*!
 * \variable QContactOnlineAccount::PresenceBusy
 * The value for presence which specifies that the contact's current status is busy
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::PresenceBusy, "Busy");

/*!
 * \variable QContactOnlineAccount::PresenceAway
 * The value for presence which specifies that the contact's current status is away
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::PresenceAway, "Away");

/*!
 * \variable QContactOnlineAccount::PresenceExtendedAway
 * The value for presence which specifies that the contact's current status is extended away
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::PresenceExtendedAway, "ExtendedAway");

/*!
 * \variable QContactOnlineAccount::PresenceUnknown
 * The value for presence which specifies that the contact's current status is unknown
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::PresenceUnknown, "Unknown");

/*!
 * \variable QContactOnlineAccount::PresenceOffline
 * The value for presence which specifies that the contact's current status is offline
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::PresenceOffline, "Offline");

/*!
 * \variable QContactOnlineAccount::FieldStatusMessage
 * The constant key for which the status message value is stored in details of the QContactOnlineAccount type
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::FieldStatusMessage, "StatusMessage");

/*!
 * \variable QContactOrganization::DefinitionName
 * The constant string which identifies the definition of details which identify the organization to which a contact belongs in a given context
 */
Q_DEFINE_LATIN1_LITERAL(QContactOrganization::DefinitionName, "Organization");

/*!
 * \variable QContactDisplayLabel::DefinitionName
 * The constant string which identifies the definition of details which contain a display label of a contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactDisplayLabel::DefinitionName, "DisplayLabel");


/*!
 * \variable QContactPhoneNumber::FieldNumber
 * The constant key for which the phone number value is stored in details of the QContactPhoneNumber type
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::FieldNumber, "PhoneNumber");

/*!
 * \variable QContactPhoneNumber::FieldSubType
 * The constant key for which the subtypes value is stored in details of the QContactPhoneNumber type
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::FieldSubTypes, "SubTypes");

/*!
 * \variable QContactEmailAddress::FieldEmailAddress
 * The constant key for which the email address value is stored in details of the QContactEmailAddress type
 */
Q_DEFINE_LATIN1_LITERAL(QContactEmailAddress::FieldEmailAddress, "EmailAddress");

/*!
 * \variable QContactFamily::FieldSpouse
 * The constant key for which the spouse name value is stored in details of the QContactFamily type
 */
Q_DEFINE_LATIN1_LITERAL(QContactFamily::FieldSpouse, "Spouse");

/*!
 * \variable QContactFamily::FieldChildren
 * The constant key for which the children names value is stored in details of the QContactFamily type
 */
Q_DEFINE_LATIN1_LITERAL(QContactFamily::FieldChildren, "Children");

/*!
 * \variable QContactGuid::FieldGuid
 * The constant key for which the globally unique identifier value is stored in details of the QContactGuid type
 */
Q_DEFINE_LATIN1_LITERAL(QContactGuid::FieldGuid, "Guid");

/*!
 * \variable QContactSyncTarget::FieldSyncTarget
 * The constant key for which the value of the target store for synchronisation is stored in details of the QContactSyncTarget type
 */
Q_DEFINE_LATIN1_LITERAL(QContactSyncTarget::FieldSyncTarget, "SyncTarget");

/*!
 * \variable QContactAvatar::FieldAvatar
 * The constant key for which the path the avatar value is stored in details of the QContactAvatar type
 */
Q_DEFINE_LATIN1_LITERAL(QContactAvatar::FieldAvatar, "Avatar");

/*!
 * \variable QContactAvatar::FieldSubType
 * The constant key for which the subtypes value is stored in details of the QContactAvatar type
 */
Q_DEFINE_LATIN1_LITERAL(QContactAvatar::FieldSubType, "SubType");

/*!
 * \variable QContactName::FieldPrefix
 * The constant key for which the name prefix value is stored in details of the QContactName type
 */
Q_DEFINE_LATIN1_LITERAL(QContactName::FieldPrefix, "Prefix");

/*!
 * \variable QContactName::FieldFirst
 * The constant key for which the first name value is stored in details of the QContactName type
 */
Q_DEFINE_LATIN1_LITERAL(QContactName::FieldFirst, "First");

/*!
 * \variable QContactName::FieldMiddle
 * The constant key for which the middle name value is stored in details of the QContactName type
 */
Q_DEFINE_LATIN1_LITERAL(QContactName::FieldMiddle, "Middle");

/*!
 * \variable QContactName::FieldLast
 * The constant key for which the last name value is stored in details of the QContactName type
 */
Q_DEFINE_LATIN1_LITERAL(QContactName::FieldLast, "Last");

/*!
 * \variable QContactName::FieldSuffix
 * The constant key for which the name suffix value is stored in details of the QContactName type
 */
Q_DEFINE_LATIN1_LITERAL(QContactName::FieldSuffix, "Suffix");

/*!
 * \variable QContactName::FieldCustomLabel
 * The constant key for which the custom name label value is stored in details of the QContactName type, if supported
 */
Q_DEFINE_LATIN1_LITERAL(QContactName::FieldCustomLabel, "CustomLabel");

/*!
 * \variable QContactNickname::FieldNickname
 * The constant key for which the nickname value is stored in details of the QContactNickname type
 */
Q_DEFINE_LATIN1_LITERAL(QContactNickname::FieldNickname, "Nickname");

/*!
 * \variable QContactNote::FieldNote
 * The constant key for which the note value is stored in details of the QContactNote type
 */
Q_DEFINE_LATIN1_LITERAL(QContactNote::FieldNote, "Note");

/*!
 * \variable QContactAddress::FieldStreet
 * The constant key for which the street value is stored in details of the QContactAddress type
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::FieldStreet, "Street");

/*!
 * \variable QContactAddress::FieldLocality
 * The constant key for which the locality value is stored in details of the QContactAddress type
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::FieldLocality, "Locality");

/*!
 * \variable QContactAddress::FieldRegion
 * The constant key for which the region value is stored in details of the QContactAddress type
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::FieldRegion, "Region");

/*!
 * \variable QContactAddress::FieldPostcode
 * The constant key for which the postcode value is stored in details of the QContactAddress type
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::FieldPostcode, "Postcode");

/*!
 * \variable QContactAddress::FieldCountry
 * The constant key for which the country value is stored in details of the QContactAddress type
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::FieldCountry, "Country");

/*!
 * \variable QContactAddress::FieldPostOfficeBox
 * The constant key for which the post office box value is stored in details of the QContactAddress type
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::FieldPostOfficeBox, "PostOfficeBox");

/*!
 * \variable QContactAddress::FieldSubTypes
 * The constant key for which the subtypes value is stored in details of the QContactAddress type
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::FieldSubTypes, "SubTypes");

/*!
 * \variable QContactUrl::FieldUrl
 * The constant key for which the url value is stored in details of the QContactUrl type
 */
Q_DEFINE_LATIN1_LITERAL(QContactUrl::FieldUrl, "Url");

/*!
 * \variable QContactUrl::FieldSubType
 * The constant key for which the subtypes value is stored in details of the QContactUrl type
 */
Q_DEFINE_LATIN1_LITERAL(QContactUrl::FieldSubType, "SubType");

/*!
 * \variable QContactBirthday::FieldBirthday
 * The constant key for which the birthday date value is stored in details of the QContactBirthday type
 */
Q_DEFINE_LATIN1_LITERAL(QContactBirthday::FieldBirthday, "Birthday");

/*!
 * \variable QContactAnniversary::FieldOriginalDate
 * The constant key for which the anniversary original event date value is stored in details of the QContactAnniversary type
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::FieldOriginalDate, "OriginalDate");

/*!
 * \variable QContactAnniversary::FieldEvent
 * The constant key for which the name of the event is stored in details of the QContactAnniversary type
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::FieldEvent, "Event");

/*!
 * \variable QContactAnniversary::FieldCalendarId
 * The constant key for which the value of the identifier of the associated calendar entry is stored in details of the QContactAnniversary type
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::FieldCalendarId, "CalendarId");

/*!
 * \variable QContactAnniversary::FieldSubType
 * The constant key for which the subtypes value is stored in details of the QContactAnniversary type
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::FieldSubType, "SubType");

/*!
 * \variable QContactGender::FieldGender
 * The constant key for which the gender value is stored in details of the QContactGender type
 */
Q_DEFINE_LATIN1_LITERAL(QContactGender::FieldGender, "Gender");

/*!
 * \variable QContactGender::GenderMale
 * The value that identifies this contact as being male
 */
Q_DEFINE_LATIN1_LITERAL(QContactGender::GenderMale, "Male");

/*!
 * \variable QContactGender::GenderFemale
 * The value that identifies this contact as being female
 */
Q_DEFINE_LATIN1_LITERAL(QContactGender::GenderFemale, "Female");

/*!
 * \variable QContactGender::GenderUnspecified
 * The value that identifies this contact as being of unspecified gender
 */
Q_DEFINE_LATIN1_LITERAL(QContactGender::GenderUnspecified, "Unspecified");

/*!
 * \variable QContactGeolocation::FieldLabel
 * The constant key for which the location label value is stored in details of the QContactGeolocation type
 */
Q_DEFINE_LATIN1_LITERAL(QContactGeolocation::FieldLabel, "Label");

/*!
 * \variable QContactGeolocation::FieldLatitude
 * The constant key for which the latitude value is stored in details of the QContactGeolocation type
 */
Q_DEFINE_LATIN1_LITERAL(QContactGeolocation::FieldLatitude, "Latitude");

/*!
 * \variable QContactGeolocation::FieldLongitude
 * The constant key for which the longitude value is stored in details of the QContactGeolocation type
 */
Q_DEFINE_LATIN1_LITERAL(QContactGeolocation::FieldLongitude, "Longitude");

/*!
 * \variable QContactGeolocation::FieldAccuracy
 * The constant key for which the location accuracy value is stored in details of the QContactGeolocation type
 */
Q_DEFINE_LATIN1_LITERAL(QContactGeolocation::FieldAccuracy, "Accuracy");

/*!
 * \variable QContactGeolocation::FieldAltitude
 * The constant key for which the altitude value is stored in details of the QContactGeolocation type
 */
Q_DEFINE_LATIN1_LITERAL(QContactGeolocation::FieldAltitude, "Altitude");


/*!
 * \variable QContactGeolocation::FieldAltitudeAccuracy
 * The constant key for which the altitude accuracy value is stored in details of the QContactGeolocation type
 */
Q_DEFINE_LATIN1_LITERAL(QContactGeolocation::FieldAltitudeAccuracy, "AltitudeAccuracy");

/*!
 * \variable QContactGeolocation::FieldHeading
 * The constant key for which the heading value is stored in details of the QContactGeolocation type
 */
Q_DEFINE_LATIN1_LITERAL(QContactGeolocation::FieldHeading, "Heading");

/*!
 * \variable QContactGeolocation::FieldSpeed
 * The constant key for which the speed value is stored in details of the QContactGeolocation type
 */
Q_DEFINE_LATIN1_LITERAL(QContactGeolocation::FieldSpeed, "Speed");

/*!
 * \variable QContactGeolocation::FieldTimestamp
 * The constant key for which the timestamp value is stored in details of the QContactGeolocation type
 */
Q_DEFINE_LATIN1_LITERAL(QContactGeolocation::FieldTimestamp, "Timestamp");

/*!
 * \variable QContactOnlineAccount::FieldAccountUri
 * The constant key for which the remote account uri value is stored in details of the QContactOnlineAccount type
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::FieldAccountUri, "AccountUri");

/*!
 * \variable QContactOnlineAccount::FieldServiceProvider
 * The constant key for which the service provider value is stored in details of the QContactOnlineAccount type
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::FieldServiceProvider, "ServiceProvider");

/*!
 * \variable QContactOnlineAccount::FieldSubTypes
 * The constant key for which the subtypes value is stored in details of the QContactOnlineAccount type
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::FieldSubTypes, "SubTypes");

/*!
 * \variable QContactOnlineAccount::SubTypeSip
 * The constant attribute value which describes the online account as supporting SIP
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::SubTypeSip, "Sip");

/*!
 * \variable QContactOnlineAccount::SubTypeSipVoip
 * The constant attribute value which describes the online account as supporting SIP-based VoIP
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::SubTypeSipVoip, "SipVoip");

/*!
 * \variable QContactOnlineAccount::SubTypeImpp
 * The constant attribute value which describes the online account as supporting IMPP
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::SubTypeImpp, "Impp");

/*!
 * \variable QContactOnlineAccount::SubTypeVideoShare
 * The constant attribute value which describes the online account as supporting VideoShare
 */
Q_DEFINE_LATIN1_LITERAL(QContactOnlineAccount::SubTypeVideoShare, "VideoShare");

/*!
 * \variable QContactOrganization::FieldName
 * The constant key for which the name value is stored in details of the QContactOrganization type
 */
Q_DEFINE_LATIN1_LITERAL(QContactOrganization::FieldName, "Name");

/*!
 * \variable QContactOrganization::FieldLogo
 * The constant key for which the logo path value is stored in details of the QContactOrganization type
 */
Q_DEFINE_LATIN1_LITERAL(QContactOrganization::FieldLogo, "Logo");

/*!
 * \variable QContactOrganization::FieldDepartment
 * The constant key for which the organization's department value is stored in details of the QContactOrganization type
 */
Q_DEFINE_LATIN1_LITERAL(QContactOrganization::FieldDepartment, "Department");

/*!
 * \variable QContactOrganization::FieldLocation
 * The constant key for which the organization's location (or the location of the contact's part of the organization) value is stored in details of the QContactOrganization type
 */
Q_DEFINE_LATIN1_LITERAL(QContactOrganization::FieldLocation, "Location");

/*!
 * \variable QContactOrganization::FieldRole
 * The constant key for which the contact's role within the organization is stored in details of the QContactOrganization type
 */
Q_DEFINE_LATIN1_LITERAL(QContactOrganization::FieldRole, "Role");

/*!
 * \variable QContactOrganization::FieldTitle
 * The constant key for which the contact's title within the organization is stored in details of the QContactOrganization type
 */
Q_DEFINE_LATIN1_LITERAL(QContactOrganization::FieldTitle, "Title");

/*!
 * \variable QContactOrganization::FieldAssistantName
 * The constant key for which the contact's assistant name within the organization is stored in details of the QContactOrganization type
 */
Q_DEFINE_LATIN1_LITERAL(QContactOrganization::FieldAssistantName, "AssistantName");


/*!
 * \variable QContactDisplayLabel::FieldLabel
 * The constant key for which the display label value is stored in details of the QContactDisplayLabel type
 */
Q_DEFINE_LATIN1_LITERAL(QContactDisplayLabel::FieldLabel, "Label");

/*!
 * \variable QContactTimestamp::FieldModificationTimestamp
 * The constant key for the value which is stored in details of the QContactTimestamp type which describes the last modification date and time of a contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactTimestamp::FieldModificationTimestamp, "ModificationTimestamp");

/*!
 * \variable QContactTimestamp::FieldCreationTimestamp
 * The constant key for the value which is stored in details of the QContactTimestamp type which describes the creation date and time of a contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactTimestamp::FieldCreationTimestamp, "CreationTimestamp");

/*!
 * \variable QContactType::FieldType
 * The constant key for the type value which is stored in details of the QContactType definition
 */
Q_DEFINE_LATIN1_LITERAL(QContactType::FieldType, "Type");


/*!
 * \variable QContactType::TypeContact
 * The constant attribute value which describes the contact as being an ordinary contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactType::TypeContact, "Contact");

/*!
 * \variable QContactType::TypeGroup
 * The constant attribute value which describes the contact as being a group
 */
Q_DEFINE_LATIN1_LITERAL(QContactType::TypeGroup, "Group");

/*!
 * \variable QContactPhoneNumber::SubTypeLandline
 * The constant attribute value which describes the phone number as identifying a landline phone
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeLandline, "Landline");

/*!
 * \variable QContactPhoneNumber::SubTypeMobile
 * The constant attribute value which describes the phone number as identifying a mobile phone
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeMobile, "Mobile");

/*!
 * \variable QContactPhoneNumber::SubTypeFacsimile
 * The constant attribute value which describes the phone number as identifying a facsimile machine
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeFacsimile, "Facsimile");

/*!
 * \variable QContactPhoneNumber::SubTypePager
 * The constant attribute value which describes the phone number as identifying a pager device
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypePager, "Pager");

/*!
 * \variable QContactPhoneNumber::SubTypeCar
 * The constant attribute value which describes the phone number as identifying a car phone
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeCar, "Car");

/*!
 * \variable QContactPhoneNumber::SubTypeBulletinBoardSystem
 * The constant attribute value which describes the phone number as identifying a bulletin board system
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeBulletinBoardSystem, "BulletinBoardSystem");

/*!
 * \variable QContactPhoneNumber::SubTypeVoice
 * The constant attribute value which describes the phone number as supporting voice transmission
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeVoice, "Voice");

/*!
 * \variable QContactPhoneNumber::SubTypeModem
 * The constant attribute value which describes the phone number as supporting digital data transfer
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeModem, "Modem");

/*!
 * \variable QContactPhoneNumber::SubTypeVideo
 * The constant attribute value which describes the phone number as supporting video transmission
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeVideo, "Video");

/*!
 * \variable QContactPhoneNumber::SubTypeMessagingCapable
 * The constant attribute value which describes the phone number as supporting messaging services
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeMessagingCapable, "MessagingCapable");

/*!
 * \variable QContactPhoneNumber::SubTypeAssistant
 * The constant attribute value which describes the phone number as an assistant phone number
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeAssistant, "Assistant");

/*!
 * \variable QContactPhoneNumber::SubTypeDtmfMenu
 * The constant attribute value which describes the phone number as supporting DTMF-controlled electronic menu navigation
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::SubTypeDtmfMenu, "DtmfMenu");

/*!
 * \variable QContactAddress::SubTypeParcel
 * The constant attribute value which describes the address as being an address for parcel delivery
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::SubTypeParcel, "Parcel");

/*!
 * \variable QContactAddress::SubTypePostal
 * The constant attribute value which describes the address as being an address for postal delivery
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::SubTypePostal, "Postal");

/*!
 * \variable QContactAddress::SubTypeDomestic
 * The constant attribute value which describes the address as being a domestic address
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::SubTypeDomestic, "Domestic");

/*!
 * \variable QContactAddress::SubTypeInternational
 * The constant attribute value which describes the address as being an international address
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::SubTypeInternational, "International");

/*!
 * \variable QContactAvatar::SubTypeImage
 * The constant attribute value which describes the avatar as being an image
 */
Q_DEFINE_LATIN1_LITERAL(QContactAvatar::SubTypeImage, "Image");

/*!
 * \variable QContactAvatar::SubTypeVideo
 * The constant attribute value which describes the avatar as being a video
 */
Q_DEFINE_LATIN1_LITERAL(QContactAvatar::SubTypeVideo, "Video");

/*!
 * \variable QContactAvatar::SubTypeAudioRingtone
 * The constant attribute value which describes the avatar as being an audio ringtone
 */
Q_DEFINE_LATIN1_LITERAL(QContactAvatar::SubTypeAudioRingtone, "AudioRingtone");

/*!
 * \variable QContactAvatar::SubTypeVideoRingtone
 * The constant attribute value which describes the avatar as being a video ringtone
 */
Q_DEFINE_LATIN1_LITERAL(QContactAvatar::SubTypeVideoRingtone, "VideoRingtone");


/*!
 * \variable QContactAvatar::SubTypeTexturedMesh
 * The constant attribute value which describes the avatar as being a textured, 3D mesh
 */
Q_DEFINE_LATIN1_LITERAL(QContactAvatar::SubTypeTexturedMesh, "TexturedMesh");

/*!
 * \variable QContactUrl::SubTypeHomePage
 * The constant attribute value which describes the url as being the homepage of the contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactUrl::SubTypeHomePage, "HomePage");

/*!
 * \variable QContactUrl::SubTypeFavourite
 * The constant attribute value which describes the url as being a favourite page of the contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactUrl::SubTypeFavourite, "Favourite");

/*!
 * \variable QContactAnniversary::SubTypeWedding
 * The constant attribute value which describes the anniversary as being a wedding anniversary
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::SubTypeWedding, "Wedding");

/*!
 * \variable QContactAnniversary::SubTypeEngagement
 * The constant attribute value which describes the anniversary as being an engagement anniversary
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::SubTypeEngagement, "Engagement");

/*!
 * \variable QContactAnniversary::SubTypeHouse
 * The constant attribute value which describes the anniversary as being an anniversary of ownership of a particular residence
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::SubTypeHouse, "House");

/*!
 * \variable QContactAnniversary::SubTypeEmployment
 * The constant attribute value which describes the anniversary as being an anniversary of employment at a particular company
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::SubTypeEmployment, "Employment");

/*!
 * \variable QContactAnniversary::SubTypeMemorial
 * The constant attribute value which describes the anniversary as being an anniversary of an event of sentimental significance
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::SubTypeMemorial, "Memorial");


/*!
 * \fn QContactSyncTarget::syncTarget() const
 * Returns the identifier of the backend store to which the contact containing this detail should be synchronised
 */

/*!
 * \fn QContactSyncTarget::setSyncTarget(const QString& syncTarget)
 * Sets the identifier of the backend store to which the contact containing this detail should be synchronised to \a syncTarget
 */

/*!
 * \fn QContactEmailAddress::emailAddress() const
 * Returns the email address of the contact which is stored in this detail
 */

/*!
 * \fn QContactEmailAddress::setEmailAddress(const QString& emailAddress)
 * Sets the email address of the contact which is stored in this detail to \a emailAddress
 */

/*!
 * \fn QContactFamily::spouse() const
 * Returns the name of the spouse of the contact which is stored in this detail
 */

/*!
 * \fn QContactFamily::setSpouse(const QString& spouseName)
 * Sets the name of the spouse of the contact which is stored in this detail to \a spouseName
 */

/*!
 * \fn QContactFamily::children() const
 * Returns the names of the children of the contact which is stored in this detail
 */

/*!
 * \fn QContactFamily::setChildren(const QStringList& childrenNames)
 * Sets the names of the children of the contact which is stored in this detail to \a childrenNames
 */

/*!
 * \fn QContactAnniversary::originalDate() const
 * Returns the original date of occurrance of the event stored in this detail
 */

/*!
 * \fn QContactAnniversary::setOriginalDate(const QDate& date)
 * Sets the original date of occurrance of the event stored in this detail to \a date
 */

/*!
 * \fn QContactAnniversary::calendarId() const
 * Returns the identifier of the calendar entry associated with this anniversary
 */

/*!
 * \fn QContactAnniversary::setCalendarId(const QString& calendarId)
 * Sets the identifier of the calendar entry associated with this anniversary to \a calendarId
 */

/*!
 * \fn QContactAnniversary::event() const
 * Returns the name of the event for which this detail contains information
 */

/*!
 * \fn QContactAnniversary::setEvent(const QString& event)
 * Sets the name of the event for which this detail contains information to \a event
 */

/*!
 * \fn QContactAnniversary::setSubType(const QString& subType)
 * Sets the subtype which this detail implements to be the given \a subType
 */

/*!
 * \fn QContactAnniversary::subType() const
 * Returns the subtype that this detail implements, if defined
 */

/*!
 * \fn QContactAvatar::avatar() const
 * Returns the location of an avatar file associated with the contact
 */

/*!
 * \fn QContactAvatar::setAvatar(const QString& avatar)
 * Sets the location of an avatar file associated with the contact to \a avatar
 */

/*!
 * \fn QContactAvatar::setSubType(const QString& subType)
 * Sets the subtype which this detail implements to be the given \a subType
 */

/*!
 * \fn QContactAvatar::subType() const
 * Returns the subtype that this detail implements, if defined
 */

/*!
 * Returns the avatar's image if the avatar's subtype is SubTypeImage.
 */
QImage QContactAvatar::avatarImage() const
{
    QImage img;
    QUrl url(avatar());
    
    if (!url.isValid())
        url =  QUrl::fromLocalFile(avatar());

    if (url.isValid()) {
        QNetworkAccessManager* manager = new QNetworkAccessManager();
        QNetworkRequest req;

        req.setUrl(url);
        QNetworkReply* reply = manager->get(req);
        
        reply->waitForReadyRead(-1);
        //XXX the QImage can detect the image format automatically?
        img = QImage::fromData(reply->readAll());
        reply->deleteLater();
        delete manager;
    }
    return img;
    
}

/*!
 * Returns the avatar's thumbnail image.
 */
QImage QContactAvatar::thumbnail() const
{
    //XXX todo how to report the default thumbnail size?
    // or perhaps let the user call scaled() function directly?
    QImage result = avatarImage().scaled(200, 150);
    return result;
}
/*!
 * \fn QContactAddress::postOfficeBox() const
 * Returns the post office box segment of the address stored in this detail
 */

/*!
 * \fn QContactAddress::setPostOfficeBox(const QString& postOfficeBox)
 * Sets the post office box segment of the address stored in this detail to \a postOfficeBox
 */

/*!
 * \fn QContactAddress::street() const
 * Returns the street segment of the address stored in this detail
 */

/*!
 * \fn QContactAddress::setStreet(const QString& street)
 * Sets the street segment of the address stored in this detail to \a street
 */

/*!
 * \fn QContactAddress::locality() const
 * Returns the locality segment of the address stored in this detail
 */

/*!
 * \fn QContactAddress::setLocality(const QString& locality)
 * Sets the locality segment of the address stored in this detail to \a locality
 */

/*!
 * \fn QContactAddress::region() const
 * Returns the region segment of the address stored in this detail
 */

/*!
 * \fn QContactAddress::setRegion(const QString& region)
 * Sets the region segment of the address stored in this detail to \a region
 */

/*!
 * \fn QContactAddress::postcode() const
 * Returns the postcode segment of the address stored in this detail
 */

/*!
 * \fn QContactAddress::setPostcode(const QString& postcode)
 * Sets the postcode segment of the address stored in this detail to \a postcode
 */

/*!
 * \fn QContactAddress::country() const
 * Returns the country segment of the address stored in this detail
 */

/*!
 * \fn QContactAddress::setCountry(const QString& country)
 * Sets the country segment of the address stored in this detail to \a country
 */

/*!
 * \fn QContactAddress::setSubTypes(const QStringList& subTypes)
 * Sets the subtypes which this detail implements to be those contained in the list of given \a subTypes
 */

/*!
 * \fn QContactAddress::setSubTypes(const QString& subType)
 * Sets the subtypes which this detail implements to be just the given \a subType
 */

/*!
 * \fn QContactAddress::subTypes() const
 * Returns the list of subtypes that this detail implements
 */

/*!
 * \fn QContactUrl::url() const
 * Returns the url stored in this detail
 */

/*!
 * \fn QContactUrl::setUrl(const QString& url)
 * Sets the url stored in this detail to \a url
 */

/*!
 * \fn QContactUrl::setSubType(const QString& subType)
 * Sets the subtype which this detail implements to be the given \a subType
 */

/*!
 * \fn QContactUrl::subType() const
 * Returns the subtype that this detail implements, if defined
 */

/*!
 * \fn QContactPhoneNumber::number() const
 * Returns the phone number stored in this detail
 */

/*!
 * \fn QContactPhoneNumber::setNumber(const QString& number)
 * Sets the phone number stored in this detail to \a number
 */

/*!
 * \fn QContactPhoneNumber::setSubTypes(const QStringList& subTypes)
 * Sets the subtypes which this detail implements to be those contained in the list of given \a subTypes
 */

/*!
 * \fn QContactPhoneNumber::setSubTypes(const QString& subType)
 * Sets the subtypes which this detail implements to be just the given \a subType
 */

/*!
 * \fn QContactPhoneNumber::subTypes() const
 * Returns the list of subtypes that this detail implements
 */

/*!
 * \fn QContactBirthday::date() const
 * Returns the date of the birthday which is stored in this detail
 */

/*!
 * \fn QContactBirthday::setDate(const QDate& date)
 * Sets the date of the birthday which is stored in this detail to \a date
 */

/*!
 * \fn QContactGender::gender() const
 * Returns the gender of the contact, as stored in this detail.  The possible values for the value stored are "Male", "Female" and "Unspecified"
 */

/*!
 * \fn QContactGender::setGender(const QString& gender)
 * Sets the gender of the contact (as stored in this detail) to \a gender, if \a gender is either "Male" or "Female", otherwise sets it to "Unspecified"
 */

/*!
 * \fn QContactGeolocation::setLabel(const QString& label)
 * Sets the label of the location stored in the detail to \a label
 */

/*!
 * \fn QContactGeolocation::label() const
 * Returns the label of the location stored in the detail
 */

/*!
 * \fn QContactGeolocation::setLatitude(double latitude)
 * Sets the latitude portion of the coordinate (in decimal degrees) of the location stored in the detail to \a latitude
 */

/*!
 * \fn QContactGeolocation::latitude() const
 * Returns the latitude portion of the coordinate (specified in decimal degrees) of the location stored in the detail
 */

/*!
 * \fn QContactGeolocation::setLongitude(double longitude)
 * Sets the longitude portion of the coordinate (in decimal degrees) of the location stored in the detail to \a longitude
 */

/*!
 * \fn QContactGeolocation::longitude() const
 * Returns the longitude portion of the coordinate (specified in decimal degrees) of the location stored in the detail
 */

/*!
 * \fn QContactGeolocation::setAccuracy(double accuracy)
 * Specifies that the latitude and longitude portions of the location stored in the detail are accurate to within \a accuracy metres
 */

/*!
 * \fn QContactGeolocation::accuracy() const
 * Returns the accuracy (in metres) of the latitude and longitude of the location stored in the detail
 */

/*!
 * \fn QContactGeolocation::setAltitude(double altitude)
 * Sets the altitude portion of the coordinate (in metres above the ellipsoid) of the location stored in the detail to \a altitude
 */

/*!
 * \fn QContactGeolocation::altitude() const
 * Returns the altitude (in metres) of the location stored in the detail
 */

/*!
 * \fn QContactGeolocation::setAltitudeAccuracy(double altitudeAccuracy)
 * Sets the altitude-accuracy portion of the coordinate (in metres) of the location stored in the detail to \a altitudeAccuracy
 */

/*!
 * \fn QContactGeolocation::altitudeAccuracy() const
 * Returns the accuracy of the altitude portion of the location stored in the detail
 */

/*!
 * \fn QContactGeolocation::setHeading(double heading)
 * Sets the heading portion of the coordinate (in decimal degrees clockwise relative to true north) of the location-aware device at the time of measurement to \a heading
 */

/*!
 * \fn QContactGeolocation::heading() const
 * Returns the heading (at the time of measurement) of the location-aware device that recorded (or was provided) the measurement
 */

/*!
 * \fn QContactGeolocation::setSpeed(double speed)
 * Sets the speed portion of the coordinate (in metres per second) of the location-aware device at the time of measurement to \a speed
 */

/*!
 * \fn QContactGeolocation::speed() const
 * Returns the speed (at the time of measurement) of the location-aware device that recorded (or was provided) the measurement
 */

/*!
 * \fn QContactGeolocation::setTimestamp(const QDateTime& timestamp)
 * Sets the creation (or first-valid) timestamp of the location information to \a timestamp
 */

/*!
 * \fn QContactGeolocation::timestamp() const
 * Returns the timestamp associated with the location stored in the detail
 */

/*!
 * \fn QContactGuid::guid() const
 * Returns the globally unique identifier which is stored in this detail
 */

/*!
 * \fn QContactGuid::setGuid(const QString& guid)
 * Sets the globally unique identifier which is stored in this detail to \a guid
 */

/*!
 * \fn QContactName::prefix() const
 * Returns the prefix segment of the name stored in this detail
 */

/*!
 * \fn QContactName::setPrefix(const QString& prefix)
 * Sets the prefix segment of the name stored in this detail to \a prefix
 */

/*!
 * \fn QContactName::first() const
 * Returns the first (given) name segment of the name stored in this detail
 */

/*!
 * \fn QContactName::setFirst(const QString& first)
 * Sets the first name segment of the name stored in this detail to \a first
 */

/*!
 * \fn QContactName::middle() const
 * Returns the middle (additional, or other) name segment of the name stored in this detail
 */

/*!
 * \fn QContactName::setMiddle(const QString& middle)
 * Sets the middle name segment of the name stored in this detail to \a middle
 */

/*!
 * \fn QContactName::last() const
 * Returns the last (family, or surname) name segment of the name stored in this detail
 */

/*!
 * \fn QContactName::setLast(const QString& last)
 * Sets the last name segment of the name stored in this detail to \a last
 */

/*!
 * \fn QContactName::suffix() const
 * Returns the suffix segment of the name stored in this detail
 */

/*!
 * \fn QContactName::setSuffix(const QString& suffix)
 * Sets the suffix segment of the name stored in this detail to \a suffix
 */

/*!
 * \fn QContactName::customLabel() const
 * Returns the custom label of the name stored in this detail
 */

/*!
 * \fn QContactName::setCustomLabel(const QString& customLabel)
 * Sets the custom label of the name stored in this detail to \a customLabel
 */

/*!
 * \fn QContactNickname::setNickname(const QString& nickname)
 * Sets the nickname of the contact which is stored in this detail to \a nickname
 */

/*!
 * \fn QContactNickname::nickname() const
 * Returns the nickname of the contact which is stored in this detail
 */

/*!
 * \fn QContactNote::setNote(const QString& note)
 * Sets a note associated with a contact to \a note.
 */

/*!
 * \fn QContactNote::note() const
 * Returns a string for a note associated with a contact.
 */

/*!
 * \fn QContactTimestamp::created() const
 * Returns the creation timestamp saved in this detail
 */

/*!
 * \fn QContactTimestamp::lastModified() const
 * Returns the last-modified timestamp saved in this detail
 */

/*!
 * \fn QContactTimestamp::setCreated(const QDateTime& dateTime)
 * Sets the creation timestamp saved in this detail to \a dateTime
 */

/*!
 * \fn QContactTimestamp::setLastModified(const QDateTime& dateTime)
 * Sets the last-modified timestamp saved in this detail to \a dateTime
 */

/*!
 * \fn QContactType::type() const
 * Returns the contact type value stored in this detail
 */

/*!
 * \fn QContactType::setType(const QString& type)
 * Sets the type of the contact to be the give \a type
 */


/*!
 * \fn QContactDisplayLabel::label() const
 * Returns the display label of the contact
 */

/*!
 * \fn QContactOnlineAccount::setAccountUri(const QString& accountUri)
 * Sets the universal resource identifier of the contact's online account to \a accountUri
 */

/*!
 * \fn QContactOnlineAccount::accountUri() const
 * Returns the universal resource identifier of the online account of the contact
 */

/*!
 * \fn QContactOnlineAccount::setServiceProvider(const QString& serviceProvider)
 * Sets the service provider of the contact's online account to \a serviceProvider
 */

/*!
 * \fn QContactOnlineAccount::serviceProvider() const
 * Returns the service provider of the online account of the contact
 */

/*!
 * \fn QContactOnlineAccount::setSubTypes(const QStringList& subTypes)
 * Sets the subtypes which this detail implements to be those contained in the list of given \a subTypes
 */

/*!
 * \fn QContactOnlineAccount::setSubTypes(const QString& subType)
 * Sets the subtypes which this detail implements to be just the given \a subType
 */

/*!
 * \fn QContactOnlineAccount::subTypes() const
 * Returns the list of subtypes that this detail implements
 */

/*!
 * \fn QContactOnlineAccount::setNickname(const QString& nickname)
 * Sets the last-known nickname used by the contact during communications via the online account
 * about which this detail stores presence information to \a nickname
 */

/*!
 * \fn QContactOnlineAccount::nickname() const
 * Returns the last-known nickname used by the contact during communications via the online account
 */

/*!
 * \fn QContactOnlineAccount::setPresence(const QString& presence)
 * Sets the presence of the online account according to the presence information provider to \a presence
 */

/*!
 * \fn QContactOnlineAccount::presence() const
 * Returns the presence of the online account according to the presence provider
 */

/*!
 * \fn QContactOnlineAccount::setStatusMessage(const QString& statusMessage)
 * Sets the last-known status message of the contact which was communicated via the online account about
 * which this detail stores presence information to \a statusMessage
 */

/*!
 * \fn QContactOnlineAccount::statusMessage() const
 * Returns the last-known status message of the contact which was communicated via the online account about
 * which this detail stores presence information
 */

/*!
 * \fn QContactOrganization::setName(const QString& name)
 * Sets the name of the organization stored in this detail to \a name
 */

/*!
 * \fn QContactOrganization::name() const
 * Returns the name of the organization stored in this detail
 */

/*!
 * \fn QContactOrganization::setLogo(const QString& logo)
 * Sets the logo of the organization stored in this detail to \a logo
 */

/*!
 * \fn QContactOrganization::logo() const
 * Returns the logo of the organization stored in this detail
 */


/*!
 * \fn QContactOrganization::setDepartment(const QStringList& department)
 * Sets the contact's department of the organization stored in this detail to \a department.
 * The department is a list of progressively finer-grained information.
 */

/*!
 * \fn QContactOrganization::department() const
 * Returns the contact's department stored in this detail
 */


/*!
 * \fn QContactOrganization::setLocation(const QString& location)
 * Sets the location (e.g. city or suburb) of the organization stored in this detail to \a location
 */

/*!
 * \fn QContactOrganization::location() const
 * Returns the location of the organization stored in this detail
 */


/*!
 * \fn QContactOrganization::setRole(const QString& role)
 * Sets the contact's role within the organization stored in this detail to \a role
 */

/*!
 * \fn QContactOrganization::role() const
 * Returns the contact's role within the organization stored in this detail
 */


/*!
 * \fn QContactOrganization::setTitle(const QString& title)
 * Sets the contact's title within the organization stored in this detail to \a title
 */

/*!
 * \fn QContactOrganization::title() const
 * Returns the contact's title within the organization stored in this detail
 */

/*!
 * \fn QContactOrganization::setAssistantName(const QString& assistantName)
 * Sets the name of the default assistant of contacts belonging to this organization to \a assistantName
 */

/*!
 * \fn QContactOrganization::assistantName() const
 * Returns the name of the default assistant of contacts belonging to this organization
 */

