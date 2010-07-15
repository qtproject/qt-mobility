/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


/* template docs:

   XXXX::FieldYYYY:
       The field key constant for the YYYY value.
       \sa yyyy(), setYyyy()

   XXXX::DefinitionName:
        The string constant for the definition name of QContactXXXX details.

   XXXX::FieldSubType
        The field key constant for the field that stores the sub type of a XXXX.
        \sa subType(), setSubType()

   XXXX::SubTypeYYYY
        The predefined string constant for a sub type value,
        indicating blah blah blah.
        \sa subTypes(), setSubTypes()
 */


/* ==================== QContactSyncTarget ======================= */

/*!
   \class QContactSyncTarget
   \brief The QContactSyncTarget class provides a sync target
   for a contact.
  
  \inmodule QtContacts
  
   \ingroup contacts-details
 */

/*!
    \variable QContactSyncTarget::DefinitionName
    The string constant for the definition name of QContactSyncTarget details.
*/
Q_DEFINE_LATIN1_CONSTANT(QContactSyncTarget::DefinitionName, "SyncTarget");

/*!
    \variable QContactSyncTarget::FieldSyncTarget

   The field key constant for the sync target value.
   \sa syncTarget(), setSyncTarget()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactSyncTarget::FieldSyncTarget, "SyncTarget");

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

/* ==================== QContactEmailAddress ======================= */


/*!
   \class QContactEmailAddress

   \brief The QContactEmailAddress class contains an email address of
   a contact.
   \ingroup contacts-details
 */

/*!
   \variable QContactEmailAddress::DefinitionName
   The string constant for the definition name of QContactEmailAddress details.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactEmailAddress::DefinitionName, "EmailAddress");

/*!
   \variable QContactEmailAddress::FieldEmailAddress

   The field key constant for the email address value.
   \sa emailAddress(), setEmailAddress()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactEmailAddress::FieldEmailAddress, "EmailAddress");

/*!
   \fn QContactEmailAddress::emailAddress() const
   Returns the email address of the contact which is stored in this detail.
 */

/*!
   \fn QContactEmailAddress::setEmailAddress(const QString& emailAddress)
   Sets the email address of the contact which is stored in this detail to \a emailAddress.
 */

/* ==================== QContactFamily ======================= */
/*!
   \class QContactFamily
   \brief The QContactFamily class contains names of
   family members of a contact.
   \ingroup contacts-details
 */

/*!
   \variable QContactFamily::DefinitionName
   The string constant for the definition name of QContactFamily details.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactFamily::DefinitionName, "Family");

/*!
   \variable QContactFamily::FieldSpouse

   The field key constant for the value containing the name of a spouse.
   \sa spouse(), setSpouse()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactFamily::FieldSpouse, "Spouse");

/*!
   \variable QContactFamily::FieldChildren

   The field key constant for the value containing the names of children.
   \sa children(), setChildren()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactFamily::FieldChildren, "Children");

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

/* ==================== QContactAnniversary ======================= */

/*!
   \class QContactAnniversary
   \brief The QContactAnniversary class contains an anniversary of a contact.
   \ingroup contacts-details
 */

/*!
   \variable QContactAnniversary::DefinitionName
   The string constant for the definition name of QContactAnniversary details.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAnniversary::DefinitionName, "Anniversary");

/*!
   \variable QContactAnniversary::FieldOriginalDate

   The field key constant for the original anniversary date value.
   \sa originalDate(), setOriginalDate()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAnniversary::FieldOriginalDate, "OriginalDate");

/*!
   \variable QContactAnniversary::FieldEvent

   The field key constant for the name of the event value.
   \sa event(), setEvent()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAnniversary::FieldEvent, "Event");

/*!
   \variable QContactAnniversary::FieldCalendarId

   The field key constant for the value containing the id of the calendar event.
   \sa calendarId(), setCalendarId()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAnniversary::FieldCalendarId, "CalendarId");

/*!
   \variable QContactAnniversary::FieldSubType

   The field key constant for the field that stores the sub type of a QContactAnniversary.
   \sa subType(), setSubType()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAnniversary::FieldSubType, "SubType");


/*!
   \variable QContactAnniversary::SubTypeWedding

    The predefined string constant for a sub type value,
    indicating this anniversary is a wedding anniversary.
    \sa subType(), setSubType()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAnniversary::SubTypeWedding, "Wedding");

/*!
   \variable QContactAnniversary::SubTypeEngagement

    The predefined string constant for a sub type value,
    indicating this anniversary is the anniversary of an engagement.
    \sa subType(), setSubType()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAnniversary::SubTypeEngagement, "Engagement");

/*!
   \variable QContactAnniversary::SubTypeHouse
   \internal

    The predefined string constant for a sub type value,
    indicating this anniversary is the anniversary of a new residence.
    \sa subType(), setSubType()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAnniversary::SubTypeHouse, "House");

/*!
   \variable QContactAnniversary::SubTypeEmployment

    The predefined string constant for a sub type value,
    indicating this anniversary is the anniversary of a start of
    employment.
    \sa subType(), setSubType()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAnniversary::SubTypeEmployment, "Employment");

/*!
   \variable QContactAnniversary::SubTypeMemorial

    The predefined string constant for a sub type value,
    indicating this anniversary is an anniversary of an event of sentimental significance.
    \sa subType(), setSubType()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAnniversary::SubTypeMemorial, "Memorial");

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

/* ==================== QContactAvatar ======================= */

/*!
   \class QContactAvatar
   \brief The QContactAvatar class contains the avatar of a contact.
   \ingroup contacts-details
 */

/*!
   \variable QContactAvatar::DefinitionName
   The string constant for the definition name of QContactAvatar details.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAvatar::DefinitionName, "Avatar");

/*!
   \variable QContactAvatar::FieldImageUrl

   The field key constant for the value containing the URL of the avatar image.
   \sa imageUrl(), setImageUrl()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAvatar::FieldImageUrl, "ImageUrl");

/*!
   \variable QContactAvatar::FieldVideoUrl

   The field key constant for the value containing the URL of a video avatar.
   \sa videoUrl(), setVideoUrl()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAvatar::FieldVideoUrl, "VideoUrl");

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

/* ==================== QContactAddress ======================= */


// XXX TODO - explain the different segments and their typical mapping
/*!
   \class QContactAddress
   \brief The QContactAddress class contains an address of a contact.
   \ingroup contacts-details
 */

/*!
   \variable QContactAddress::DefinitionName
   The string constant for the definition name of QContactAddress details.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAddress::DefinitionName, "Address");

/*!
   \variable QContactAddress::FieldStreet

   The field key constant for the value containing the street segment.
   \sa street(), setStreet()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAddress::FieldStreet, "Street");

/*!
   \variable QContactAddress::FieldLocality

   The field key constant for the value containing the locality segment.
   \sa locality(), setLocality()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAddress::FieldLocality, "Locality");


/*!
   \variable QContactAddress::FieldRegion

   The field key constant for the value containing the region segment.
   \sa region(), setRegion()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAddress::FieldRegion, "Region");

/*!
   \variable QContactAddress::FieldPostcode

   The field key constant for the value containing the postcode segment.
   \sa postcode(), setPostcode()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAddress::FieldPostcode, "Postcode");

/*!
   \variable QContactAddress::FieldCountry

   The field key constant for the value containing the country segment.
   \sa country(), setCountry()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAddress::FieldCountry, "Country");

/*!
   \variable QContactAddress::FieldPostOfficeBox

   The field key constant for the value containing the post office box segment.
   \sa postOfficeBox(), setPostOfficeBox()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAddress::FieldPostOfficeBox, "PostOfficeBox");

/*!
   \variable QContactAddress::FieldSubTypes

   The field key constant for the field that stores the sub types of a QContactAddress.
   \sa subTypes(), setSubTypes()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAddress::FieldSubTypes, "SubTypes");

/*!
   \variable QContactAddress::SubTypeParcel

    The predefined string constant for a sub type value,
    indicating this address is an address for parcel delivery.
    \sa subTypes(), setSubTypes()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAddress::SubTypeParcel, "Parcel");

/*!
   \variable QContactAddress::SubTypePostal

    The predefined string constant for a sub type value,
    indicating this address is an address for postal delivery.
    \sa subTypes(), setSubTypes()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAddress::SubTypePostal, "Postal");

/*!
   \variable QContactAddress::SubTypeDomestic

    The predefined string constant for a sub type value,
    indicating this address is an address for domestic mail delivery.
    \sa subTypes(), setSubTypes()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAddress::SubTypeDomestic, "Domestic");

/*!
   \variable QContactAddress::SubTypeInternational

    The predefined string constant for a sub type value,
    indicating this address is an address for international mail delivery.
    \sa subTypes(), setSubTypes()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactAddress::SubTypeInternational, "International");

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

/* ==================== QContactUrl ======================= */

/*!
   \class QContactUrl
   \brief The QContactUrl class contains a url associated with
   a contact.
   \ingroup contacts-details
 */

/*!
   \variable QContactUrl::DefinitionName
   The string constant for the definition name of QContactUrl details.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactUrl::DefinitionName, "Url");

/*!
   \variable QContactUrl::FieldUrl

   The field key constant for the value containing the URL.
   \sa url(), setUrl()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactUrl::FieldUrl, "Url");

/*!
   \variable QContactUrl::FieldSubType

   The field key constant for the field that stores the sub type of a QContactUrl.
   \sa subType(), setSubType()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactUrl::FieldSubType, "SubType");

/*!
   \variable QContactUrl::SubTypeHomePage

    The predefined string constant for a sub type value,
    indicating this url is a contact's home page.
    \sa subType(), setSubType()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactUrl::SubTypeHomePage, "HomePage");

/*!
   \variable QContactUrl::SubTypeFavourite

    The predefined string constant for a sub type value,
    indicating this url is one of the contact's favourite URLs (or bookmarks).
    \sa subType(), setSubType()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactUrl::SubTypeFavourite, "Favourite");

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

/* ==================== QContactPhonenumber ======================= */

/*!
    \class QContactPhoneNumber
    \brief The QContactPhoneNumber class provides a phone number
    of a contact.
    \ingroup contacts-details
*/


/*!
    \variable QContactPhoneNumber::DefinitionName
    The string constant for the definition name of QContactPhoneNumber details.
*/
Q_DEFINE_LATIN1_CONSTANT(QContactPhoneNumber::DefinitionName, "PhoneNumber");

/*!
   \variable QContactPhoneNumber::FieldNumber

   The field key constant for the value containing the phone number.
   \sa number(), setNumber()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPhoneNumber::FieldNumber, "PhoneNumber");

/*!
   \variable QContactPhoneNumber::FieldSubTypes

   The field key constant for the field that stores the sub types of a QContactPhoneNumber.
   \sa subTypes(), setSubTypes()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPhoneNumber::FieldSubTypes, "SubTypes");

/*!
   \variable QContactPhoneNumber::SubTypeLandline

    The predefined string constant for a sub type value,
    indicating this phone number is a landline number.
    \sa subTypes(), setSubTypes()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPhoneNumber::SubTypeLandline, "Landline");

/*!
   \variable QContactPhoneNumber::SubTypeMobile

    The predefined string constant for a sub type value,
    indicating this phone number is a mobile (cellular) number.
    \sa subTypes(), setSubTypes()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPhoneNumber::SubTypeMobile, "Mobile");

/*!
   \variable QContactPhoneNumber::SubTypeFax

    The predefined string constant for a sub type value,
    indicating this phone number is a fax number.
    \sa subTypes(), setSubTypes()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPhoneNumber::SubTypeFax, "Fax");

/*!
   \variable QContactPhoneNumber::SubTypePager

    The predefined string constant for a sub type value,
    indicating this phone number is a pager number.
    \sa subTypes(), setSubTypes()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPhoneNumber::SubTypePager, "Pager");

/*!
   \variable QContactPhoneNumber::SubTypeCar

    The predefined string constant for a sub type value,
    indicating this phone number is a car phone.
    \sa subTypes(), setSubTypes()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPhoneNumber::SubTypeCar, "Car");

/*!
   \variable QContactPhoneNumber::SubTypeBulletinBoardSystem

    The predefined string constant for a sub type value,
    indicating this phone number is a bulletin board system.
    \sa subTypes(), setSubTypes()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPhoneNumber::SubTypeBulletinBoardSystem, "BulletinBoardSystem");

/*!
   \variable QContactPhoneNumber::SubTypeVoice

    The predefined string constant for a sub type value,
    indicating this phone number supports voice transmission.
    \sa subTypes(), setSubTypes()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPhoneNumber::SubTypeVoice, "Voice");

/*!
   \variable QContactPhoneNumber::SubTypeModem

    The predefined string constant for a sub type value,
    indicating this phone number supports data transmission.
    \sa subTypes(), setSubTypes()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPhoneNumber::SubTypeModem, "Modem");

/*!
   \variable QContactPhoneNumber::SubTypeVideo

    The predefined string constant for a sub type value,
    indicating this phone number supports video transmission.
    \sa subTypes(), setSubTypes()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPhoneNumber::SubTypeVideo, "Video");

/*!
   \variable QContactPhoneNumber::SubTypeMessagingCapable

    The predefined string constant for a sub type value,
    indicating this phone number supports messaging services.
    \sa subTypes(), setSubTypes()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPhoneNumber::SubTypeMessagingCapable, "MessagingCapable");

/*!
   \variable QContactPhoneNumber::SubTypeAssistant

    The predefined string constant for a sub type value,
    indicating this phone number is the number of an assistant.
    \sa subTypes(), setSubTypes()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPhoneNumber::SubTypeAssistant, "Assistant");

/*!
   \variable QContactPhoneNumber::SubTypeDtmfMenu

    The predefined string constant for a sub type value,
    indicating this phone number supports DTMF-controlled voice menu navigation.
    \sa subTypes(), setSubTypes()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPhoneNumber::SubTypeDtmfMenu, "DtmfMenu");


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

/* ==================== QContactBirthday ======================= */

/*!
   \class QContactBirthday
   \brief The QContactBirthday class contains a birthday of a contact.
   \ingroup contacts-details
 */

/*!
   \variable QContactBirthday::DefinitionName
   The string constant for the definition name of QContactBirthday details.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactBirthday::DefinitionName, "Birthday");

/*!
   \variable QContactBirthday::FieldBirthday

   The field key constant for the value containing the birthday date.
   \sa date(), setDate()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactBirthday::FieldBirthday, "Birthday");

/*!
   \fn QContactBirthday::date() const
   Returns the date of the birthday which is stored in this detail.
 */

/*!
   \fn QContactBirthday::setDate(const QDate& date)
   Sets the date of the birthday which is stored in this detail to \a date.
 */

/* ==================== QContactGender ======================= */

/*!
   \class QContactGender
   \brief The QContactGender class contains the gender of a contact.
   \ingroup contacts-details
 */

/*!
   \variable QContactGender::DefinitionName
   The string constant for the definition name of QContactGender details.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGender::DefinitionName, "Gender");

/*!
   \variable QContactGender::FieldGender

   The field key constant for the value containing the gender.
   \sa gender(), setGender()
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

/* ==================== QContactGeolocation ======================= */

/*!
   \class QContactGeoLocation
   \brief The QContactGeoLocation class contains a global location
   coordinate associated with a contact.
   \ingroup contacts-details
*/

/*!
   \variable QContactGeoLocation::DefinitionName
   The string constant for the definition name of QContactGeoLocation details.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGeoLocation::DefinitionName, "GeoLocation");

/*!
   \variable QContactGeoLocation::FieldLabel

   The field key constant for the value containing the location label.
   \sa label(), setLabel()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGeoLocation::FieldLabel, "Label");

/*!
   \variable QContactGeoLocation::FieldLatitude

   The field key constant for the value containing the latitude.
   \sa latitude(), setLatitude()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGeoLocation::FieldLatitude, "Latitude");

/*!
   \variable QContactGeoLocation::FieldLongitude

   The field key constant for the value containing the longitude.
   \sa longitude(), setLongitude()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGeoLocation::FieldLongitude, "Longitude");

/*!
   \variable QContactGeoLocation::FieldAccuracy

   The field key constant for the value containing the location (latitude/longitude) accuracy.
   \sa accuracy(), setAccuracy()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGeoLocation::FieldAccuracy, "Accuracy");

/*!
   \variable QContactGeoLocation::FieldAltitude

   The field key constant for the value containing the altitude.
   \sa altitude(), setAltitude()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGeoLocation::FieldAltitude, "Altitude");


/*!
   \variable QContactGeoLocation::FieldAltitudeAccuracy

   The field key constant for the value containing the accuracy of the altitude.
   \sa altitudeAccuracy(), setAltitudeAccuracy()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGeoLocation::FieldAltitudeAccuracy, "AltitudeAccuracy");

/*!
   \variable QContactGeoLocation::FieldHeading

   The field key constant for the value containing the heading.
   \sa heading(), setHeading()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGeoLocation::FieldHeading, "Heading");

/*!
   \variable QContactGeoLocation::FieldSpeed

   The field key constant for the value containing the speed.
   \sa speed(), setSpeed()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGeoLocation::FieldSpeed, "Speed");

/*!
   \variable QContactGeoLocation::FieldTimestamp

   The field key constant for the value containing the timestamp of the location information.
   \sa timestamp(), setTimestamp()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGeoLocation::FieldTimestamp, "Timestamp");

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

/* ==================== QContactGuid ======================= */

/*!
   \class QContactGuid
   \brief The QContactGuid class contains a globally unique
   Id of a contact, for use in synchronization with other datastores.
   \ingroup contacts-details
 */

/*!
   \variable QContactGuid::DefinitionName
   The string constant for the definition name of QContactGuid details.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGuid::DefinitionName, "Guid");

/*!
   \variable QContactGuid::FieldGuid

   The field key constant for the value containing the GUID.
   \sa guid(), setGuid()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGuid::FieldGuid, "Guid");

/*!
   \fn QContactGuid::guid() const

   Returns the globally unique identifier which is stored in this
   detail.
 */

/*!
   \fn QContactGuid::setGuid(const QString& guid)
   Sets the globally unique identifier which is stored in this detail to \a guid.
 */

/* ==================== QContactName ======================= */

/*!
   \class QContactName
   \brief The QContactName class contains a name of a contact.
   \ingroup contacts-details
 */

/*!
   \variable QContactName::DefinitionName
   The string constant for the definition name of QContactName details.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactName::DefinitionName, "Name");

/*!
   \variable QContactName::FieldPrefix

   The field key constant for the value containing the prefix part of the name.
   \sa prefix(), setPrefix()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactName::FieldPrefix, "Prefix");

/*!
   \variable QContactName::FieldFirstName

   The field key constant for the value containing the first name part of the name.
   \sa firstName(), setFirstName()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactName::FieldFirstName, "FirstName");

/*!
   \variable QContactName::FieldMiddleName

   The field key constant for the value containing the middle name part of the name.
   \sa middleName(), setMiddleName()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactName::FieldMiddleName, "MiddleName");

/*!
   \variable QContactName::FieldLastName

   The field key constant for the value containing the last name part of the name.
   \sa lastName(), setLastName()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactName::FieldLastName, "LastName");

/*!
   \variable QContactName::FieldSuffix

   The field key constant for the value containing the suffix part of the name.
   \sa suffix(), setSuffix()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactName::FieldSuffix, "Suffix");

/*!
   \variable QContactName::FieldCustomLabel

   The field key constant for the value containing a custom formatted label.
   \sa customLabel(), setCustomLabel()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactName::FieldCustomLabel, "CustomLabel");

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

/* ==================== QContactNickname ======================= */

/*!
   \class QContactNickname
   \brief The QContactNickname class contains a nickname of a contact.
   \ingroup contacts-details
 */

/*!
\variable QContactNickname::DefinitionName
The string constant for the definition name of QContactNickname details.
*/
Q_DEFINE_LATIN1_CONSTANT(QContactNickname::DefinitionName, "Nickname");

/*!
   \variable QContactNickname::FieldNickname

   The field key constant for the value containing the nickname.
   \sa nickname(), setNickname()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactNickname::FieldNickname, "Nickname");

/*!
   \fn QContactNickname::setNickname(const QString& nickname)
   Sets the nickname of the contact which is stored in this detail to \a nickname.
 */

/*!
   \fn QContactNickname::nickname() const
   Returns the nickname of the contact which is stored in this detail.
 */

/* ==================== QContactNote ======================= */

/*!
   \class QContactNote
   \brief The QContactNote class contains a note associated
   with a contact.
   \ingroup contacts-details
 */

/*!
   \variable QContactNote::DefinitionName
   The string constant for the definition name of QContactNote details.
*/
Q_DEFINE_LATIN1_CONSTANT(QContactNote::DefinitionName, "Note");

/*!
   \variable QContactNote::FieldNote

   The field key constant for the value containing the note.
   \sa note(), setNote()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactNote::FieldNote, "Note");


/*!
   \fn QContactNote::setNote(const QString& note)
   Sets a note associated with a contact to \a note.
 */

/*!
   \fn QContactNote::note() const
   Returns a string for a note associated with a contact.
 */

/* ==================== QContactTag ======================= */

/*!
   \class QContactTag
   \brief The QContactTag class contains a tag associated with a
   contact.
   \ingroup contacts-details

   Typically the tags associated with a contact will be distinct,
   although this is usually only enforced when the contact is saved
   in the manager.

   Here is an example of retrieving all the tags for a contact:
   \snippet doc/src/snippets/qtcontactsdocsample/qtcontactsdocsample.cpp Getting all tags

   Here is an example of checking for a specific tag value:
   \snippet doc/src/snippets/qtcontactsdocsample/qtcontactsdocsample.cpp Checking for a specific tag

 */

/*!
   \variable QContactTag::DefinitionName
   The string constant for the definition name of QContactTag details.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactTag::DefinitionName, "Tag");

/*!
   \variable QContactTag::FieldTag

    The field key constant for the value containing the tag.
   \sa tag(), setTag()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactTag::FieldTag, "Tag");

/*!
   \fn QContactTag::setTag(const QString& tag)
   Sets the tag associated with a contact which is stored in this detail to \a tag.
 */

/*!
   \fn QContactTag::tag() const
   Returns the tag associated with a contact which is stored in this detail.
 */

/* ==================== QContactThumbnail ======================= */

/*!
   \class QContactThumbnail
   \brief The QContactThumbnail class contains a thumbnail used
   in display lists to represent the contact.
   \ingroup contacts-details
 */

/*!
   \variable QContactThumbnail::DefinitionName
   The string constant for the definition name of QContactThumbnail details.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactThumbnail::DefinitionName, "Thumbnail");

/*!
   \variable QContactThumbnail::FieldThumbnail

   The field key constant for the value containing the thumbnail image.
   \sa thumbnail(), setThumbnail()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactThumbnail::FieldThumbnail, "Thumbnail");

/*!
  \fn QContactThumbnail::thumbnail() const
  Returns the thumbnail image of the contact
 */

/*!
  \fn QContactThumbnail::setThumbnail(const QImage& thumbnail)
  Sets the thumbnail image of the contact to be \a thumbnail
 */

/* ==================== QContactTimestamp ======================= */

/*!
   \class QContactTimestamp
   \brief The QContactTimestamp class contains the creation and
   last-modified timestamp associated with the contact.
   \ingroup contacts-details
 */

/*!
\variable QContactTimestamp::DefinitionName
The string constant for the definition name of QContactTimestamp details.
*/
Q_DEFINE_LATIN1_CONSTANT(QContactTimestamp::DefinitionName, "Timestamp");

/*!
   \variable QContactTimestamp::FieldModificationTimestamp

   The field key constant for the value of the last modified timestamp.
   \sa lastModified(), setLastModified()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactTimestamp::FieldModificationTimestamp, "ModificationTimestamp");

/*!
   \variable QContactTimestamp::FieldCreationTimestamp

   The field key constant for the value of the timestamp a contact was created.
   \sa created(), setCreated()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactTimestamp::FieldCreationTimestamp, "CreationTimestamp");

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

/* ==================== QContactType ======================= */

/*!
   \class QContactType
   \brief The QContactType class describes the type of the contact.
   \ingroup contacts-details
 */

/*!
\variable QContactType::DefinitionName
The string constant for the definition name of QContactType details.
*/
Q_DEFINE_LATIN1_CONSTANT(QContactType::DefinitionName, "Type");

/*!
   \variable QContactType::FieldType

   The field key constant for the type value which is stored in details of
   the QContactType definition.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactType::FieldType, "Type");


/*!
   \variable QContactType::TypeContact

    The predefined string constant for a type value,
    indicating this contact is an ordinary contact.
    \sa setType(), type()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactType::TypeContact, "Contact");

/*!
   \variable QContactType::TypeGroup

    The predefined string constant for a type value,
    indicating this contact is a group contact.

    Contacts of this type are able to be the first contact in
    relationships of the \c QContactRelationship::HasMember type.

    To enumerate the ids of members of a group, the client should
    retrieve the relationships which involve the group from the manager
    in which the group is saved.

    \sa setType(), type()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactType::TypeGroup, "Group");

/*!
   \fn QContactType::type() const
   Returns the contact type value stored in this detail.
 */

/*!
   \fn QContactType::setType(const QString& type)
   Sets the type of the contact to be the give \a type.
 */

/* ==================== QContactDisplayLabel ======================= */

// XXX TODO make this better
/*!
   \class QContactDisplayLabel
   \brief The QContactDisplayLabel class is the (possibly synthesized)
   display label of a contact.
   \ingroup contacts-details
 */

/*!
   \variable QContactDisplayLabel::DefinitionName

   The string constant for the definition name of QContactDisplayLabel details.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactDisplayLabel::DefinitionName, "DisplayLabel");


/*!
   \variable QContactDisplayLabel::FieldLabel

   The field key constant for the value of the display label.
   \sa label()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactDisplayLabel::FieldLabel, "Label");

/*!
   \fn QContactDisplayLabel::label() const
   Returns the display label of the contact.
 */

/* ==================== QContactOnlineAccount ======================= */

// XXX TODO explain link to QContactPresence

/*!
   \class QContactOnlineAccount
   \brief The QContactOnlineAccount class provides an online account,
   which the contact uses to communicate with friends and family.

   A QContactOnlineAccount consists of the account details required to
   communicate with the contact, including the account URI, the capabilities
   of the account, the service provider of the account, and the type of the account.

   Presence information for a particular QContactOnlineAccount detail is provided
   in a QContactPresence detail which is linked (via linkedDetailUris()) to the
   account detail.  This information is generally provided by the backend, and is
   not modifiable by clients.

   \sa QContactPresence

   \ingroup contacts-details
 */

/*!
   \variable QContactOnlineAccount::DefinitionName
   The string constant for the definition name of QContactOnlineAccount details.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOnlineAccount::DefinitionName, "OnlineAccount");

/*!
   \variable QContactOnlineAccount::FieldCapabilities

   The field key constant for the account capabilities value.
   \sa capabilities(), setCapabilities()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOnlineAccount::FieldCapabilities, "Capabilities");

/*!
   \variable QContactOnlineAccount::FieldAccountUri

   The field key constant for the account uri value.
   \sa accountUri(), setAccountUri()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOnlineAccount::FieldAccountUri, "AccountUri");

/*!
   \variable QContactOnlineAccount::FieldServiceProvider

   The field key constant for the account service provider name.
   \sa serviceProvider(), setServiceProvider()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOnlineAccount::FieldServiceProvider, "ServiceProvider");

/*!
   \variable QContactOnlineAccount::FieldSubTypes

   The field key constant for the field that stores the sub types of a QContactOnlineAccount.
   \sa subTypes(), setSubTypes()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOnlineAccount::FieldSubTypes, "SubTypes");

/*!
   \variable QContactOnlineAccount::SubTypeSip

    The predefined string constant for a sub type value,
    indicating this online account supports SIP.
    \sa subTypes(), setSubTypes()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOnlineAccount::SubTypeSip, "Sip");

/*!
   \variable QContactOnlineAccount::SubTypeSipVoip

    The predefined string constant for a sub type value,
    indicating this online account supports SIP based VOIP.
    \sa subTypes(), setSubTypes()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOnlineAccount::SubTypeSipVoip, "SipVoip");

/*!
   \variable QContactOnlineAccount::SubTypeImpp

    The predefined string constant for a sub type value,
    indicating this online account supports IMPP.
    \sa subTypes(), setSubTypes()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOnlineAccount::SubTypeImpp, "Impp");

/*!
   \variable QContactOnlineAccount::SubTypeVideoShare

    The predefined string constant for a sub type value,
    indicating this online account supports VideoShare.
    \sa subTypes(), setSubTypes()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOnlineAccount::SubTypeVideoShare, "VideoShare");


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

/* ==================== QContactOrganization ======================= */

/*!
   \class QContactOrganization
   \brief The QContactOrganization class provides details about an
   organization that the contact is either a part of, or stands for.
   \ingroup contacts-details
 */

/*!
   \variable QContactOrganization::DefinitionName
   The string constant for the definition name of QContactOrganization details.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOrganization::DefinitionName, "Organization");

/*!
   \variable QContactOrganization::FieldName

   The field key constant for the value of the organization name.
   \sa name(), setName()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOrganization::FieldName, "Name");

/*!
   \variable QContactOrganization::FieldLogoUrl

   The field key constant for the URL of the organization logo image.
   \sa logoUrl(), setLogoUrl()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOrganization::FieldLogoUrl, "LogoUrl");

/*!
   \variable QContactOrganization::FieldDepartment

   The field key constant for the value of the department name.
   \sa department(), setDepartment()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOrganization::FieldDepartment, "Department");

/*!
   \variable QContactOrganization::FieldLocation

   The field key constant for the value of the location of the organization.
   \sa location(), setLocation()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOrganization::FieldLocation, "Location");

/*!
   \variable QContactOrganization::FieldRole

   The field key constant for the value of the contact's role in the organization.
   \sa role(), setRole()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOrganization::FieldRole, "Role");

/*!
   \variable QContactOrganization::FieldTitle

   The field key constant for the value of the contact's title in the organization.
   \sa title(), setTitle()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOrganization::FieldTitle, "Title");

/*!
   \variable QContactOrganization::FieldAssistantName

   The field key constant for the value of the name of the contact's assistant.
   \sa assistantName(), setAssistantName()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactOrganization::FieldAssistantName, "AssistantName");

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

/* ==================== QContactRingtone ======================= */

/*!
   \class QContactRingtone
   \brief The QContactRingtone class provides a ringtone associated
   with a contact
   \ingroup contacts-details
 */

/*!
\variable QContactRingtone::DefinitionName
The string constant for the definition name of QContactRingtone details.
*/
Q_DEFINE_LATIN1_CONSTANT(QContactRingtone::DefinitionName, "Ringtone");

/*!
   \variable QContactRingtone::FieldAudioRingtoneUrl

   The field key constant for the value of the URL for an audio ringtone.
   \sa setAudioRingtoneUrl(), audioRingtoneUrl()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactRingtone::FieldAudioRingtoneUrl, "AudioRingtoneUrl");

/*!
   \variable QContactRingtone::FieldVideoRingtoneUrl

   The field key constant for the value of the URL for a video ringtone.
   \sa setVideoRingtoneUrl(), videoRingtoneUrl()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactRingtone::FieldVideoRingtoneUrl, "VideoRingtoneUrl");

/*!
   \variable QContactRingtone::FieldVibrationRingtoneUrl

   The field key constant for the value of the URL for a vibration ringtone.
   \sa setVibrationRingtoneUrl(), vibrationRingtoneUrl()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactRingtone::FieldVibrationRingtoneUrl, "VibrationRingtoneUrl");

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

/* ==================== QContactPresence ======================= */

// XXX TODO add more stuff here
/*!
   \class QContactPresence
   \brief The QContactPresence class provides presence information
   for an online account of a contact.

   Presence information for a particular QContactOnlineAccount detail is provided
   in a QContactPresence detail which is linked (via linkedDetailUris()) to the
   account detail.  This information is generally provided by the backend, and is
   not modifiable by clients.

   Presence information can include update timestamp, screen name,
   and the status icon, status value, and status text provided by
   the service provider, as well as user defined status message.

   \sa QContactOnlineAccount

   \ingroup contacts-details
 */

/*!
   \variable QContactPresence::DefinitionName
   The string constant for the definition name of QContactPresence details.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPresence::DefinitionName, "Presence");

/*!
   \variable QContactPresence::FieldTimestamp
   The field key constant for the timestamp value.
   \sa setTimestamp(), timestamp()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPresence::FieldTimestamp, "Timestamp");

/*!
   \variable QContactPresence::FieldNickname
   The field key constant for the nickname value.
   \sa setNickname(), nickname()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPresence::FieldNickname, "Nickname");

/*!
   \variable QContactPresence::FieldPresenceState
   The field key constant for the presence state enumeration value.
   \sa setPresenceState(), presenceState()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPresence::FieldPresenceState, "PresenceState");

/*!
   \variable QContactPresence::FieldPresenceStateText
   The field key constant for the presence state description value.
   \sa setPresenceStateText(), presenceStateText()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPresence::FieldPresenceStateText, "PresenceStateText");

/*!
   \variable QContactPresence::FieldPresenceStateImageUrl
   The field key constant for the presence state image URL.
   \sa setPresenceStateImageUrl(), presenceStateImageUrl()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPresence::FieldPresenceStateImageUrl, "PresenceStateImageUrl");

/*!
   \variable QContactPresence::FieldCustomMessage
   The field key constant for the user-entered custom presence message.
   \sa setCustomMessage(), customMessage()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactPresence::FieldCustomMessage, "CustomMessage");

/*!
   \fn QContactPresence::setTimestamp(const QDateTime& updateTimestamp)

   Sets the timestamp for the last update of the presence detail to be
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
  Not all presence providers support all of these states.

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
   This custom message would have been set by the contact,
   and does not necessarily correspond to a particular presence state.
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

/* ==================== QContactGlobalPresence ======================= */

/*!
   \class QContactGlobalPresence
   \brief The QContactGlobalPresence class provides aggregated presence information
   for a contact, synthesized or supplied by the backend.
   \ingroup contacts-details
 */

/*!
   \variable QContactGlobalPresence::DefinitionName
   The string constant for the definition name of QContactGlobalPresence details.
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGlobalPresence::DefinitionName, "GlobalPresence");

/*!
   \variable QContactGlobalPresence::FieldTimestamp
   The field key constant for the timestamp value.
   \sa setTimestamp(), timestamp()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGlobalPresence::FieldTimestamp, "Timestamp");

/*!
   \variable QContactGlobalPresence::FieldNickname
   The field key constant for the nickname value.
   \sa setNickname(), nickname()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGlobalPresence::FieldNickname, "Nickname");

/*!
   \variable QContactGlobalPresence::FieldPresenceState
   The field key constant for the presence state enumeration value.
   \sa setPresenceState(), presenceState()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGlobalPresence::FieldPresenceState, "PresenceState");

/*!
   \variable QContactGlobalPresence::FieldPresenceStateText
   The field key constant for the presence state description value.
   \sa setPresenceStateText(), presenceStateText()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGlobalPresence::FieldPresenceStateText, "PresenceStateText");

/*!
   \variable QContactGlobalPresence::FieldPresenceStateImageUrl
   The field key constant for the presence state image URL.
   \sa setPresenceStateImageUrl(), presenceStateImageUrl()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGlobalPresence::FieldPresenceStateImageUrl, "PresenceStateImageUrl");

/*!
   \variable QContactGlobalPresence::FieldCustomMessage

   The field key constant for the user-entered custom presence message.
   \sa setCustomMessage(), customMessage()
 */
Q_DEFINE_LATIN1_CONSTANT(QContactGlobalPresence::FieldCustomMessage, "CustomMessage");

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




/* ==================== Convenience Filters ======================= */

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
