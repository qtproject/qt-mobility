/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcontactdetails.h"

/*!
 * \class QContactAddress
 * \brief An address of a contact
 */

/*!
 * \class QContactAnniversary
 * \brief An anniversary of a contact
 */

/*!
 * \class QContactAvatar
 * \brief An avatar of a contact
 */

/*!
 * \class QContactBirthday
 * \brief The birthday of a contact
 */

/*!
 * \class QContactDisplayLabel
 * \brief The (possibly synthesised) display label of a contact
 */

/*!
 * \class QContactEmailAddress
 * \brief An email address of a contact
 */

/*!
 * \class QContactGender
 * \brief The gender of a contact
 */

/*!
 * \class QContactGuid
 * \brief The globally unique Id of a contact
 */

/*!
 * \class QContactName
 * \brief The name of a contact
 */

/*!
 * \class QContactOrganisation
 * \brief Details about the organisation of which the contact is either a part of, or stands for
 */

/*!
 * \class QContactPhoneNumber
 * \brief A phone number of a contact
 */

/*!
 * \class QContactSyncTarget
 * \brief A sync target for a contact
 */

/*!
 * \class QContactTimestamp
 * \brief Contains the creation and last-modified timestamp associated with the contact
 */

/*!
 * \class QContactUrl
 * \brief A url associated with a contact
 */

/*!
 * \variable QContactName::DefinitionName
 * The constant string which identifies the definition of details which are names
 */
Q_DEFINE_LATIN1_LITERAL(QContactName::DefinitionName, "Name");

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
 * \variable QContactOrganisation::DefinitionName
 * The constant string which identifies the definition of details which identify the organisation to which a contact belongs in a given context
 */
Q_DEFINE_LATIN1_LITERAL(QContactOrganisation::DefinitionName, "Organisation");

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
 * \variable QContactEmailAddress::FieldEmailAddress
 * The constant key for which the email address value is stored in details of the QContactEmailAddress type
 */
Q_DEFINE_LATIN1_LITERAL(QContactEmailAddress::FieldEmailAddress, "EmailAddress");

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
 * \variable QContactAddress::FieldDisplayLabel
 * The constant key for which the display label value is stored in details of the QContactAddress type
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::FieldDisplayLabel, "DisplayLabel");

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
 * \variable QContactUrl::FieldUrl
 * The constant key for which the url value is stored in details of the QContactUrl type
 */
Q_DEFINE_LATIN1_LITERAL(QContactUrl::FieldUrl, "Url");

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
 * \variable QContactGender::FieldGender
 * The constant key for which the gender value is stored in details of the QContactGender type
 */
Q_DEFINE_LATIN1_LITERAL(QContactGender::FieldGender, "Gender");

/*!
 * \variable QContactOrganisation::FieldDisplayLabel
 * The constant key for which the display label value is stored in details of the QContactOrganisation type
 */
Q_DEFINE_LATIN1_LITERAL(QContactOrganisation::FieldDisplayLabel, "DisplayLabel");

/*!
 * \variable QContactOrganisation::FieldLogo
 * The constant key for which the logo path value is stored in details of the QContactOrganisation type
 */
Q_DEFINE_LATIN1_LITERAL(QContactOrganisation::FieldLogo, "Logo");

/*!
 * \variable QContactDisplayLabel::FieldLabel
 * The constant key for which the display label value is stored in details of the QContactDisplayLabel type
 */
Q_DEFINE_LATIN1_LITERAL(QContactDisplayLabel::FieldLabel, "Label");

/*!
 * \variable QContactDisplayLabel::FieldSynthesised
 * The constant key for the value which is stored in details of the QContactDisplayLabel type which describes whether the label was synthesised by a QContactManager or set manually.
 */
Q_DEFINE_LATIN1_LITERAL(QContactDisplayLabel::FieldSynthesised, "Synthesised");

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
 * \variable QContactPhoneNumber::AttributeSubTypeLandline
 * The constant attribute value which describes the phone number as identifying a landline phone
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::AttributeSubTypeLandline, "Landline");

/*!
 * \variable QContactPhoneNumber::AttributeSubTypeMobile
 * The constant attribute value which describes the phone number as identifying a mobile phone
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::AttributeSubTypeMobile, "Mobile");

/*!
 * \variable QContactPhoneNumber::AttributeSubTypeFacsimile
 * The constant attribute value which describes the phone number as identifying a facsimile machine
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::AttributeSubTypeFacsimile, "Facsimile");

/*!
 * \variable QContactPhoneNumber::AttributeSubTypePager
 * The constant attribute value which describes the phone number as identifying a pager device
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::AttributeSubTypePager, "Pager");

/*!
 * \variable QContactPhoneNumber::AttributeSubTypeCar
 * The constant attribute value which describes the phone number as identifying a car phone
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::AttributeSubTypeCar, "Car");

/*!
 * \variable QContactPhoneNumber::AttributeSubTypeBulletinBoardSystem
 * The constant attribute value which describes the phone number as identifying a bulletin board system
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::AttributeSubTypeBulletinBoardSystem, "BulletinBoardSystem");

/*!
 * \variable QContactPhoneNumber::AttributeSubTypeVoice
 * The constant attribute value which describes the phone number as supporting voice transmission
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::AttributeSubTypeVoice, "Voice");

/*!
 * \variable QContactPhoneNumber::AttributeSubTypeModem
 * The constant attribute value which describes the phone number as supporting digital data transfer
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::AttributeSubTypeModem, "Modem");

/*!
 * \variable QContactPhoneNumber::AttributeSubTypeVideo
 * The constant attribute value which describes the phone number as supporting video transmission
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::AttributeSubTypeVideo, "Video");

/*!
 * \variable QContactPhoneNumber::AttributeSubTypeMessagingCapable
 * The constant attribute value which describes the phone number as supporting messaging services
 */
Q_DEFINE_LATIN1_LITERAL(QContactPhoneNumber::AttributeSubTypeMessagingCapable, "MessagingCapable");

/*!
 * \variable QContactAddress::AttributeSubTypeParcel
 * The constant attribute value which describes the address as being an address for parcel delivery
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::AttributeSubTypeParcel, "Parcel");

/*!
 * \variable QContactAddress::AttributeSubTypePostal
 * The constant attribute value which describes the address as being an address for postal delivery
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::AttributeSubTypePostal, "Postal");

/*!
 * \variable QContactAddress::AttributeSubTypeDomestic
 * The constant attribute value which describes the address as being a domestic address
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::AttributeSubTypeDomestic, "Domestic");

/*!
 * \variable QContactAddress::AttributeSubTypeInternational
 * The constant attribute value which describes the address as being an international address
 */
Q_DEFINE_LATIN1_LITERAL(QContactAddress::AttributeSubTypeInternational, "International");

/*!
 * \variable QContactEmailAddress::AttributeSubTypeInternet
 * The constant attribute value which describes the email address as being an internet email address
 */
Q_DEFINE_LATIN1_LITERAL(QContactEmailAddress::AttributeSubTypeInternet, "Internet");

/*!
 * \variable QContactAvatar::AttributeSubTypeImage
 * The constant attribute value which describes the avatar as being an image
 */
Q_DEFINE_LATIN1_LITERAL(QContactAvatar::AttributeSubTypeImage, "Image");

/*!
 * \variable QContactAvatar::AttributeSubTypeVideo
 * The constant attribute value which describes the avatar as being a video
 */
Q_DEFINE_LATIN1_LITERAL(QContactAvatar::AttributeSubTypeVideo, "Video");

/*!
 * \variable QContactAvatar::AttributeSubTypeTexturedMesh
 * The constant attribute value which describes the avatar as being a textured, 3D mesh
 */
Q_DEFINE_LATIN1_LITERAL(QContactAvatar::AttributeSubTypeTexturedMesh, "TexturedMesh");

/*!
 * \variable QContactGuid::AttributeSubTypeSync
 * The constant attribute value which describes the guid as being used for synchronisation
 */
Q_DEFINE_LATIN1_LITERAL(QContactGuid::AttributeSubTypeSync, "Sync");

/*!
 * \variable QContactGuid::AttributeSubTypeOther
 * The constant attribute value which describes the guid as being used for a purpose other than synchronisation
 */
Q_DEFINE_LATIN1_LITERAL(QContactGuid::AttributeSubTypeOther, "Other");

/*!
 * \variable QContactUrl::AttributeSubTypeHomePage
 * The constant attribute value which describes the url as being the homepage of the contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactUrl::AttributeSubTypeHomePage, "HomePage");

/*!
 * \variable QContactUrl::AttributeSubTypeFavourite
 * The constant attribute value which describes the url as being a favourite page of the contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactUrl::AttributeSubTypeFavourite, "Favourite");

/*!
 * \variable QContactUrl::AttributeSubTypeSocialNetworking
 * The constant attribute value which describes the url as being one of the social networking pages of the contact
 */
Q_DEFINE_LATIN1_LITERAL(QContactUrl::AttributeSubTypeSocialNetworking, "SocialNetworking");

///*!
// * \variable QContactMeeting::AttributeSubTypeBusiness
// * The constant attribute value which describes the meeting as being a business meeting
// */
//Q_DEFINE_LATIN1_LITERAL(QContactMeeting::AttributeSubTypeBusiness, "Business");

///*!
// * \variable QContactMeeting::AttributeSubTypePersonal
// * The constant attribute value which describes the url as being a personal meeting
// */
//Q_DEFINE_LATIN1_LITERAL(QContactMeeting::AttributeSubTypePersonal, "Personal");

///*!
// * \variable QContactMeeting::AttributeSubTypeAppointment
// * The constant attribute value which describes the url as being a professional appointment
// */
//Q_DEFINE_LATIN1_LITERAL(QContactMeeting::AttributeSubTypeAppointment, "Appointment");

/*!
 * \variable QContactAnniversary::AttributeSubTypeWedding
 * The constant attribute value which describes the anniversary as being a wedding anniversary
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::AttributeSubTypeWedding, "Wedding");

/*!
 * \variable QContactAnniversary::AttributeSubTypeEngagement
 * The constant attribute value which describes the anniversary as being an engagement anniversary
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::AttributeSubTypeEngagement, "Engagement");

/*!
 * \variable QContactAnniversary::AttributeSubTypeHouse
 * The constant attribute value which describes the anniversary as being an anniversary of ownership of a particular residence
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::AttributeSubTypeHouse, "House");

/*!
 * \variable QContactAnniversary::AttributeSubTypeEmployment
 * The constant attribute value which describes the anniversary as being an anniversary of employment at a particular company
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::AttributeSubTypeEmployment, "Employment");

/*!
 * \variable QContactAnniversary::AttributeSubTypeMemorial
 * The constant attribute value which describes the anniversary as being an anniversary of an event of sentimental significance
 */
Q_DEFINE_LATIN1_LITERAL(QContactAnniversary::AttributeSubTypeMemorial, "Memorial");



/* Functions that are platform specific or don't operate on strings */

QString QContactAddress::displayLabel() const
{
    QString dl = value(QContactAddress::FieldDisplayLabel);
    if (!dl.isNull())
        return dl;

    QString st = street();
    QString lc = locality();
    QString rg = region();
    QString pc = postcode();
    QString cn = country();
    QString result;

    if (!st.trimmed().isEmpty()) {
       result.append(st);
       result.append(QLatin1Char('\n'));
    }

    if (!lc.trimmed().isEmpty()) {
       result.append(lc);
       result.append(QLatin1Char('\n'));
    }

    bool skipPostcode = false;
    if (!rg.trimmed().isEmpty()) {
        skipPostcode = true;
        if (!pc.trimmed().isEmpty()) {
            result.append(rg);
            result.append(QLatin1String(", "));
            result.append(pc);
            result.append(QLatin1Char('\n'));
        } else {
            result.append(rg);
            result.append(QLatin1Char('\n'));
        }
    }

    if (!skipPostcode && !pc.trimmed().isEmpty()) {
        result.append(pc);
        result.append(QLatin1Char('\n'));
    }

    if (!cn.trimmed().isEmpty()) {
        result += cn;
    }

    if (result.endsWith(QLatin1Char('\n')))
        result.chop(1);

    return result;
}

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
 * \fn QContactAvatar::avatar() const
 * Returns the location of an avatar file associated with the contact
 */

/*!
 * \fn QContactAvatar::setAvatar(const QString& avatar)
 * Sets the location of an avatar file associated with the contact to \a avatar
 */

/*!
 * \fn QContactAddress::displayLabel() const
 * Returns the display (formatted) label of the address stored in this detail.
 * If no display label has been explicitly set for the detail, it is synthesised in
 * a platform specific manner
 */

/*!
 * \fn QContactAddress::setDisplayLabel(const QString& displayLabel)
 * Sets the display label of the address stored in this detail to \a displayLabel
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
 * \fn QContactUrl::url() const
 * Returns the url stored in this detail
 */

/*!
 * \fn QContactUrl::setUrl(const QString& url)
 * Sets the url stored in this detail to \a url
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
 * \fn QContactDisplayLabel::isSynthesised() const
 * Returns true if the display label was synthesised automatically by the manager backend
 */

/*!
 * \fn QContactDisplayLabel::label() const
 * Returns the display label of the contact
 */

/*!
 * \fn QContactDisplayLabel::setLabel(const QString& label)
 * Sets the display label of the contact to \a label
 */

/*!
 * \fn QContactDisplayLabel::setSynthesised(bool synthesised)
 * Sets the synthetic status of the display label to \a synthesised
 */

/*!
 * \fn QContactOrganisation::setDisplayLabel(const QString& displayLabel)
 * Sets the display label of the organisation stored in this detail to \a displayLabel
 */

/*!
 * \fn QContactOrganisation::setLogo(const QString& logo)
 * Sets the logo of the organisation stored in this detail to \a logo
 */

/*!
 * \fn QContactOrganisation::logo() const
 * Returns the logo of the organisation stored in this detail
 */

