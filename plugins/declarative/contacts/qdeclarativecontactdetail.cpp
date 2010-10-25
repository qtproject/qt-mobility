/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativecontactdetails_p.h"
#include "qdeclarativecontact_p.h"
#include "qcontactdetails.h"
#include <QDebug>

/* ==================== QDeclarativeContactDetail ======================= */

/*!
   \qmlclass ContactDetail QDeclarativeContactDetail
   \brief The ContactDetail element represents a single, complete detail about a contact.
   \ingroup qml-contacts

   \sa QContactDetail

    The ContactDetail element is part of the \bold{QtMobility.contacts 1.1} module.
 */

QDeclarativeContactDetail::QDeclarativeContactDetail(QObject* parent)
    :QObject(parent)
{
    QDeclarativeContact* c = qobject_cast<QDeclarativeContact*>(parent);
    if (c)
        connect(this, SIGNAL(valueChanged()), c, SIGNAL(detailsChanged()));
}

QContactDetail& QDeclarativeContactDetail::detail()
{
    return m_detail;
}

const QContactDetail& QDeclarativeContactDetail::detail() const
{
    return m_detail;
}

void QDeclarativeContactDetail::setDetail(const QContactDetail& detail)
{
    m_detail = detail;
}

/*!
  \qmlproperty string ContactDetail::definitionName

  This property holds the string constant for the definition name of the detail.
  This property is read only.
  */
QString QDeclarativeContactDetail::definitionName() const
{
    return m_detail.definitionName();
}

/*!
  \qmlproperty list<string> ContactDetail::contexts
  This property holds one or more contexts that this detail is associated with.
  */
QStringList QDeclarativeContactDetail::contexts() const
{
    return m_detail.contexts();
}
void QDeclarativeContactDetail::setContexts(const QStringList& contexts)
{
    m_detail.setContexts(contexts);
}

/*!
  \qmlproperty bool ContactDetail::readOnly

  This property indicates whether or not this detail is writable.
  This property is read only.
  */
bool QDeclarativeContactDetail::readOnly() const
{
    return m_detail.accessConstraints().testFlag(QContactDetail::ReadOnly);
}

/*!
  \qmlproperty bool ContactDetail::removable

  This property indicates whether or not this detail is removale.
  This property is read only.
  */
bool QDeclarativeContactDetail::removable() const
{
    return !m_detail.accessConstraints().testFlag(QContactDetail::Irremovable);
}

/*!
  \qmlproperty string ContactDetail::detailUri

  This property holds the unique URI of the detail if one exists.
  */
QString QDeclarativeContactDetail::detailUri() const
{
    return m_detail.detailUri();
}
void QDeclarativeContactDetail::setDetailUri(const QString& detailUri)
{
    m_detail.setDetailUri(detailUri);
}

/*!
  \qmlproperty list<string> ContactDetail::linkedDetailUris

  This property holds a list of detail URIs to which this detail is linked.
  */
QStringList QDeclarativeContactDetail::linkedDetailUris() const
{
    return m_detail.linkedDetailUris();
}
void QDeclarativeContactDetail::setLinkedDetailUris(const QStringList& linkedDetailUris)
{
    m_detail.setLinkedDetailUris(linkedDetailUris);
}

/*!
  \qmlproperty enumeration ContactDetail::type

  This property holds the type of the detail.

   \list
   \o ContactDetail.Address
   \o ContactDetail.Anniversary
   \o ContactDetail.Avatar
   \o ContactDetail.Birthday
   \o ContactDetail.DisplayLabel
   \o ContactDetail.Email
   \o ContactDetail.Family
   \o ContactDetail.Favorite
   \o ContactDetail.Gender
   \o ContactDetail.Geolocation
   \o ContactDetail.GlobalPresence
   \o ContactDetail.Guid
   \o ContactDetail.Name
   \o ContactDetail.NickName
   \o ContactDetail.Note
   \o ContactDetail.OnlineAccount
   \o ContactDetail.Organization
   \o ContactDetail.PhoneNumber
   \o ContactDetail.Presence
   \o ContactDetail.Ringtone
   \o ContactDetail.SyncTarget
   \o ContactDetail.Tag
   \o ContactDetail.Timestamp
   \o ContactDetail.Url
   \o ContactDetail.Hobby
   \o ContactDetail.Customized
   \endlist

  This property is read only.
  */
QDeclarativeContactDetail::ContactDetailType QDeclarativeContactDetail::detailType() const
{
    return QDeclarativeContactDetail::Customized;
}

/*!
  \qmlproperty list<string> ContactDetail::fieldNames

  This property holds the list of all field names which this detail supports.

  This property is read only.
  */
QStringList QDeclarativeContactDetail::fieldNames() const
{
    return m_detail.variantValues().keys();
}

QVariant QDeclarativeContactDetail::value(const QString& key) const
{
    return m_detail.variantValue(key);
}

bool QDeclarativeContactDetail::setValue(const QString& key, const QVariant& v)
{
    bool changed = false;

    if (value(key) != v)
         changed = m_detail.setValue(key, v);

    if (changed)
        emit valueChanged();

    return changed;
}

QString QDeclarativeContactDetail::definitionName(QDeclarativeContactDetail::ContactDetailType type)
{
    switch (type) {
    case QDeclarativeContactDetail::Address:
        return QContactAddress::DefinitionName;
    case QDeclarativeContactDetail::Anniversary:
        return QContactAnniversary::DefinitionName;
    case QDeclarativeContactDetail::Avatar:
        return QContactAvatar::DefinitionName;
    case QDeclarativeContactDetail::Birthday:
        return QContactBirthday::DefinitionName;
    case QDeclarativeContactDetail::DisplayLabel:
        return QContactDisplayLabel::DefinitionName;
    case QDeclarativeContactDetail::Email:
        return QContactEmailAddress::DefinitionName;
    case QDeclarativeContactDetail::Family:
        return QContactFamily::DefinitionName;
    case QDeclarativeContactDetail::Favorite:
        return QContactFavorite::DefinitionName;
    case QDeclarativeContactDetail::Gender:
        return QContactGender::DefinitionName;
    case QDeclarativeContactDetail::Geolocation:
        return QContactGeoLocation::DefinitionName;
    case QDeclarativeContactDetail::GlobalPresence:
        return QContactGlobalPresence::DefinitionName;
    case QDeclarativeContactDetail::Guid:
        return QContactGuid::DefinitionName;
    case QDeclarativeContactDetail::Name:
        return QContactName::DefinitionName;
    case QDeclarativeContactDetail::NickName:
        return QContactNickname::DefinitionName;
    case QDeclarativeContactDetail::Note:
        return QContactNote::DefinitionName;
    case QDeclarativeContactDetail::OnlineAccount:
        return QContactOnlineAccount::DefinitionName;
    case QDeclarativeContactDetail::Organization:
        return QContactOrganization::DefinitionName;
    case QDeclarativeContactDetail::PhoneNumber:
        return QContactPhoneNumber::DefinitionName;
    case QDeclarativeContactDetail::Presence:
        return QContactPresence::DefinitionName;
    case QDeclarativeContactDetail::Ringtone:
        return QContactRingtone::DefinitionName;
    case QDeclarativeContactDetail::SyncTarget:
        return QContactSyncTarget::DefinitionName;
    case QDeclarativeContactDetail::Tag:
        return QContactTag::DefinitionName;
    case QDeclarativeContactDetail::Timestamp:
        return QContactTimestamp::DefinitionName;
    case QDeclarativeContactDetail::Url:
        return QContactUrl::DefinitionName;
    case QDeclarativeContactDetail::Customized:
    default:
        break;
    }
    return "";
}

QDeclarativeContactDetail::ContactDetailType QDeclarativeContactDetail::detailType(const QString& definitionName)
{
    if (definitionName == QContactAddress::DefinitionName)
        return QDeclarativeContactDetail::Address;
    if (definitionName == QContactAnniversary::DefinitionName)
        return QDeclarativeContactDetail::Anniversary;
    if (definitionName == QContactAvatar::DefinitionName)
        return QDeclarativeContactDetail::Avatar;
    if (definitionName == QContactBirthday::DefinitionName)
        return QDeclarativeContactDetail::Birthday;
    if (definitionName == QContactDisplayLabel::DefinitionName)
        return QDeclarativeContactDetail::DisplayLabel;
    if (definitionName == QContactEmailAddress::DefinitionName)
        return QDeclarativeContactDetail::Email;
    if (definitionName == QContactFamily::DefinitionName)
        return QDeclarativeContactDetail::Family;
    if (definitionName == QContactFavorite::DefinitionName)
        return QDeclarativeContactDetail::Favorite;
    if (definitionName == QContactGender::DefinitionName)
        return QDeclarativeContactDetail::Gender;
    if (definitionName == QContactGeoLocation::DefinitionName)
        return QDeclarativeContactDetail::Geolocation;
    if (definitionName == QContactGlobalPresence::DefinitionName)
        return QDeclarativeContactDetail::GlobalPresence;
    if (definitionName == QContactGuid::DefinitionName)
        return QDeclarativeContactDetail::Guid;
    if (definitionName == QContactName::DefinitionName)
        return QDeclarativeContactDetail::Name;
    if (definitionName == QContactNickname::DefinitionName)
        return QDeclarativeContactDetail::NickName;
    if (definitionName == QContactNote::DefinitionName)
        return QDeclarativeContactDetail::Note;
    if (definitionName == QContactOnlineAccount::DefinitionName)
        return QDeclarativeContactDetail::OnlineAccount;
    if (definitionName == QContactOrganization::DefinitionName)
        return QDeclarativeContactDetail::Organization;
    if (definitionName == QContactPhoneNumber::DefinitionName)
        return QDeclarativeContactDetail::PhoneNumber;
    if (definitionName == QContactPresence::DefinitionName)
        return QDeclarativeContactDetail::Presence;
    if (definitionName == QContactRingtone::DefinitionName)
        return QDeclarativeContactDetail::Ringtone;
    if (definitionName == QContactSyncTarget::DefinitionName)
        return QDeclarativeContactDetail::SyncTarget;
    if (definitionName == QContactTag::DefinitionName)
        return QDeclarativeContactDetail::Tag;
    if (definitionName == QContactTimestamp::DefinitionName)
        return QDeclarativeContactDetail::Timestamp;
    if (definitionName == QContactUrl::DefinitionName)
        return QDeclarativeContactDetail::Url;

    return QDeclarativeContactDetail::Customized;
}

QString QDeclarativeContactDetail::fieldName(ContactDetailType detailType, int fieldType)
{
    switch (detailType) {
    case QDeclarativeContactDetail::Address:
        return QDeclarativeContactAddress::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::Anniversary:
        return QDeclarativeContactAnniversary::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::Avatar:
        return QDeclarativeContactAvatar::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::Birthday:
        return QDeclarativeContactBirthday::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::DisplayLabel:
        return QDeclarativeContactDisplayLabel::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::Email:
        return QDeclarativeContactEmailAddress::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::Family:
        return QDeclarativeContactFamily::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::Favorite:
        return QDeclarativeContactFavorite::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::Gender:
        return QDeclarativeContactGender::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::Geolocation:
        return QDeclarativeContactGeoLocation::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::GlobalPresence:
        return QDeclarativeContactGlobalPresence::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::Guid:
        return QDeclarativeContactGuid::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::Name:
        return QDeclarativeContactName::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::NickName:
        return QDeclarativeContactNickname::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::Note:
        return QDeclarativeContactNote::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::OnlineAccount:
        return QDeclarativeContactOnlineAccount::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::Organization:
        return QDeclarativeContactOrganization::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::PhoneNumber:
        return QDeclarativeContactPhoneNumber::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::Presence:
        return QDeclarativeContactPresence::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::Ringtone:
        return QDeclarativeContactRingtone::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::SyncTarget:
        return QDeclarativeContactSyncTarget::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::Tag:
        return QDeclarativeContactTag::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::Timestamp:
        return QDeclarativeContactTimestamp::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::Url:
        return QDeclarativeContactUrl::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::Customized:
    default:
        break;
    }
    return "";
}


/* ==================== QDeclarativeContactAddress ======================= */

/*!
   \qmlclass Address QDeclarativeContactAddress
   \brief The Address element contains an address of a contact.
   \ingroup qml-contacts

   The fields in the Address element are based on the segments
   of the ADR property of a Versit vCard file.

   Address element contains the following field types:
   \list
   \o Address.Street
   \o Address.Locality
   \o Address.Region
   \o Address.PostCode
   \o Address.Country
   \o Address.SubTypes
   \o Address.PostOfficeBox
   \endlist

   Versit \reg is a trademark of the Internet Mail Consortium.
   This element is part of the \bold{QtMobility.contacts 1.1} module.
 */
/*!
  \qmlproperty string Address::street

  This property holds the street number and street name of the address.
  */
/*!
  \qmlproperty string Address::locality

  This property holds the name of the city, town or suburb of the address.
  */
/*!
  \qmlproperty string Address::region

  This property holds the name or identifier of the state, province or region of the address.
  */
/*!
  \qmlproperty string Address::postcode

  This property holds the postal code for the address.
  */
/*!
  \qmlproperty string Address::country

  This property holds the name of the country of the address.
  */
/*!
  \qmlproperty list<variant> Address::subTypes

  This property stores the sub types of the address.

  \list
  \o Address.Parcel - An address for parcel delivery.
  \o Address.Postal - An address for postal delivery.
  \o Address.Domestic - An address for domestic mail delivery.
  \o Address.International - An address for international mail delivery.
  \endlist
  */
/*!
  \qmlproperty string Address::postOfficeBox

  This property holds the post office box identifier of the mailing address.

  This element is part of the \bold{QtMobility.contacts 1.1} module.
  */

/* ==================== QDeclarativeContactAnniversary ======================= */

/*!
   \qmlclass Anniversary  QDeclarativeContactAnniversary
   \brief The Anniversary element contains an anniversary of a contact.
   \ingroup qml-contacts

   Anniversary element contains the following field types:
   \list
   \o Anniversary.CalendarId
   \o Anniversary.OriginalDate
   \o Anniversary.Event
   \o Anniversary.SubType
   \endlist

  This element is part of the \bold{QtMobility.contacts 1.1} module.
 */

/*!
  \qmlproperty string Anniversary::calendarId

  This property holds the id of the calendar event.
  */
/*!
  \qmlproperty date Anniversary::originalDate

  This property holds the original anniversary date value.
  This property is either a date, or a date and time.
  */
/*!
  \qmlproperty string Anniversary::event

  This property holds the name of the event value.
  */
/*!
  \qmlproperty enumeration Anniversary::subType

  This property holds the sub type of an Anniversary.

  \list
  \o Unknown - Unknown sub type (default).
  \o Wedding - A wedding anniversary.
  \o Engagement - An engagement anniversary.
  \o House - A new residence anniversary.
  \o Employment - A start of employment anniversary.
  \o Memorial - An event of sentimental significance.
  \endlist
  */

/* ==================== QDeclarativeContactAvatar ======================= */
/*!
   \qmlclass Avatar  QDeclarativeContactAvatar
   \brief The Avatar element contains avatar URLs of a contact.
   \ingroup qml-contacts

   Avatar element contains the following field types:
   \list
   \o Avatar.ImageUrl
   \o Avatar.VideoUrl
   \endlist

  This element is part of the \bold{QtMobility.contacts 1.1} module.
 */

/*!
  \qmlproperty string Avatar::imageUrl

  This property holds the URL of the avatar image.
  */
/*!
  \qmlproperty string Avatar::videoUrl

  This property holds the URL of a video avatar.
  */


/* ==================== QDeclarativeContactBirthday ======================= */
/*!
   \qmlclass Birthday QDeclarativeContactBirthday
   \brief The Birthday element contains a birthday of a contact.
   \ingroup qml-contacts

   Birthday element contains the following field types:
   \list
   \o Birthday.Birthday
   \endlist

  This element is part of the \bold{QtMobility.contacts 1.1} module.
 */

/*!
  \qmlproperty date Birthday::birthday

  This property holds the birthday date. The property value is either a date, or a date and time.
  */

/* ==================== QDeclarativeContactDisplayLabel ======================= */
/*!
   \qmlclass DisplayLabel QDeclarativeContactDisplayLabel
   \brief The DisplayLabel element contains the (possibly synthesized) display label of a contact.
   \ingroup qml-contacts

   DisplayLabel element contains the following field types:
   \list
   \o DisplayLabel.Label
   \endlist

  This element is part of the \bold{QtMobility.contacts 1.1} module.
 */

/*!
  \qmlproperty string DisplayLabel::label

  This property holds the value of the display label.
  This property is a read only property.
  */

/* ==================== QDeclarativeContactEmailAddress ======================= */
/*!
   \qmlclass EmailAddress QDeclarativeContactEmailAddress
   \brief The EmailAddress element contains an email address of a contact.
   \ingroup qml-contacts

   EmailAddress element contains the following field types:
   \list
   \o EmailAddress.EmailAddress
   \endlist

  This element is part of the \bold{QtMobility.contacts 1.1} module.
 */

/*!
  \qmlproperty string EmailAddress::emailAddress

  This property holds the email address value.
  */


/* ==================== QDeclarativeContactFamily ======================= */
/*!
   \qmlclass Family QDeclarativeContactFamily
   \brief The Family element contains names of family members of a contact.
   \ingroup qml-contacts

   Family element contains the following field types:
   \list
   \o Family.Spouse
   \o Family.Children
   \endlist

  This element is part of the \bold{QtMobility.contacts 1.1} module.
 */

/*!
  \qmlproperty string Family::spouse

  This property holds the name of a spouse.
  */

/*!
  \qmlproperty list<string> Family::children

  This property holds the the names of children.
  */

/* ==================== QDeclarativeContactFavorite ======================= */
/*!
   \qmlclass Favorite QDeclarativeContactFavorite
   \brief The Favorite element indicates if a contact is a favorite contact as well as the
   position it should appear in an ordered list of favorites.
   \ingroup qml-contacts

   Favorite element contains the following field types:
   \list
   \o Favorite.Favorite
   \o Favorite.Index
   \endlist

  This element is part of the \bold{QtMobility.contacts 1.1} module.
 */

/*!
  \qmlproperty bool Favorite::favorite

  This property holds the value that indicates whether a contact is a favorite.
  */

/*!
  \qmlproperty int Favorite::index

  This property holds the index of the favorite contact (which determines the order they appear).
  */


/* ==================== QDeclarativeContactGender ======================= */
/*!
   \qmlclass Gender QDeclarativeContactGender
   \brief The Gender element contains the gender of a contact.
   \ingroup qml-contacts

   Gender element contains the following field types:
   \list
   \o Gender.Gender
   \endlist

  This element is part of the \bold{QtMobility.contacts 1.1} module.
 */

/*!
  \qmlproperty enumeration Gender::gender

  This property holds the value of the gender.

  \list
  \o Gender.Male
  \o Gender.Female
  \endlist
  */


/* ==================== QDeclarativeContactGeoLocation ======================= */
/*!
   \qmlclass GeoLocation QDeclarativeContactGeoLocation
   \brief The GeoLocation element contains a global location coordinate associated with a contact.
   \ingroup qml-contacts

   GeoLocation element contains the following field types:
   \list
   \o GeoLocation.Label
   \o GeoLocation.Latitude
   \o GeoLocation.Longitude
   \o GeoLocation.Accuracy
   \o GeoLocation.Altitude
   \o GeoLocation.AltitudeAccuracy
   \o GeoLocation.Heading
   \o GeoLocation.Speed
   \o GeoLocation.Timestamp
   \endlist

  This element is part of the \bold{QtMobility.contacts 1.1} module.
 */

/*!
  \qmlproperty string GeoLocation::label

  This property holds the location label.
  */
/*!
  \qmlproperty double GeoLocation::latitude

  This property holds the value of the latitude.
  */
/*!
  \qmlproperty double GeoLocation::longitude

  This property holds the value of the longitude.
  */
/*!
  \qmlproperty double GeoLocation::accuracy

  This property holds the value of the location (latitude/longitude) accuracy.
  */
/*!
  \qmlproperty double GeoLocation::altitude

  This property holds the value of the altitude.
  */
/*!
  \qmlproperty double GeoLocation::altitudeAccuracy

  This property holds the value of the accuracy of the altitude.
  */
/*!
  \qmlproperty double GeoLocation::heading

  This property holds the value of the heading.
  */
/*!
  \qmlproperty double GeoLocation::speed

  This property holds the value of the speed.
  */
/*!
  \qmlproperty date GeoLocation::timestamp

  This property holds the value of the timestamp of the location information.
  */


/* ==================== QDeclarativeContactGlobalPresence ======================= */
/*!
   \qmlclass GlobalPresence QDeclarativeContactGlobalPresence
   \brief The GlobalPresence element provides aggregated presence information
   for a contact, synthesized or supplied by the backend.
   \ingroup qml-contacts

   GlobalPresence element contains the following field types:
   \list
   \o GlobalPresence.Timestamp
   \o GlobalPresence.Nickname
   \o GlobalPresence.State
   \o GlobalPresence.StateText
   \o GlobalPresence.ImageUrl
   \o GlobalPresence.CustomMessage
   \endlist

  This element is part of the \bold{QtMobility.contacts 1.1} module.
 */

/*!
  \qmlproperty date GlobalPresence::timestamp

  This property holds the timestamp value of the GlobalPresence.
  */
/*!
  \qmlproperty string GlobalPresence::nickname

  This property holds the nickname value of the GlobalPresence.
  */
/*!
  \qmlproperty enumeration GlobalPresence::state

  This property holds the presence state enumeration value.

  \list
  \o Presence.Unknown -  Signifies that the presence state of the contact is not currently known (default).
  \o Presence.Available - Signifies that the contact is available.
  \o Presence.Hidden - Signifies that the contact is hidden.
  \o Presence.Busy - Signifies that the contact is busy.
  \o Presence.Away - Signifies that the contact is away.
  \o Presence.ExtendedAway - Signifies that the contact is away for an extended period of time.
  \o Presence.Offline - Signifies that the contact is offline.
  \endlist
  */
/*!
  \qmlproperty string GlobalPresence::stateText

  This property holds the text corresponding to the current presence state.
  */
/*!
  \qmlproperty url GlobalPresence::imageUrl

  This property holds the last-known status image url of the contact for the online account
   about which this detail stores presence information.
  */
/*!
  \qmlproperty string GlobalPresence::customMessage

  This property holds the custom status message from the contact for the online account
   about which this detail stores presence information.
  */

/* ==================== QDeclarativeContactGuid ======================= */
/*!
   \qmlclass Guid QDeclarativeContactGuid
   \brief The Guid element contains a globally unique Id of a contact, for use in synchronization with other datastores.
   \ingroup qml-contacts

   Guid element contains the following field types:
   \list
   \o Guid.Guid
   \endlist

  This element is part of the \bold{QtMobility.contacts 1.1} module.
 */

/*!
  \qmlproperty string Guid::guid

  This property holds the value of the GUID.
  */

/* ==================== QDeclarativeContactHobby ======================= */
/*!
   \qmlclass Hobby QDeclarativeContactHobby
   \brief The Hobby element contains a hobby of the contact.
   \ingroup qml-contacts

   Hobby element contains the following field types:
   \list
   \o Hobby.Hobby
   \endlist

  This element is part of the \bold{QtMobility.contacts 1.1} module.
 */

/*!
  \qmlproperty string Hobby::hobby

  This property holds the name of the hobby.
  */

/* ==================== QDeclarativeContactName ======================= */
/*!
   \qmlclass Name QDeclarativeContactName
   \brief The Name element contains a name of a contact.
   \ingroup qml-contacts

   Name element contains the following field types:
   \list
   \o Name.Prefix
   \o Name.FirstName
   \o Name.MiddleName
   \o Name.LastName
   \o Name.Suffix
   \o Name.CustomLabel
   \endlist

  This element is part of the \bold{QtMobility.contacts 1.1} module.
 */

/*!
  \qmlproperty string Name::prefix

  This property holds the prefix name part of the name.
  */
/*!
  \qmlproperty string Name::firstName

  This property holds the first name part of the name.
  */
/*!
  \qmlproperty string Name::middleName

  This property holds the middle name part of the name.
  */
/*!
  \qmlproperty string Name::lastName

  This property holds the last name part of the name.
  */
/*!
  \qmlproperty string Name::suffix

  This property holds the suffix part of the name.
  */
/*!
  \qmlproperty string Name::customLabel

  This property holds a custom formatted label for the name.
  */


/* ==================== QDeclarativeContactNickname ======================= */
/*!
   \qmlclass Nickname QDeclarativeContactNickname
   \brief The Nickname element contains a nickname of a contact.
   \ingroup qml-contacts

   Nickname element contains the following field types:
   \list
   \o Nickname.Nickname
   \endlist

  This element is part of the \bold{QtMobility.contacts 1.1} module.
 */

/*!
  \qmlproperty string Nickname::nickname

  This property holds the value of the nickname.
  */


/* ==================== QDeclarativeContactNote ======================= */
/*!
   \qmlclass Note QDeclarativeContactNote
   \brief The Note element contains a note associated with a contact.
   \ingroup qml-contacts

   Note element contains the following field types:
   \list
   \o Note.Note
   \endlist

  This element is part of the \bold{QtMobility.contacts 1.1} module.
 */

/*!
  \qmlproperty string Note::note

  This property holds the value of the note.
  */


/* ==================== QDeclarativeContactOnlineAccount ======================= */
/*!
   \qmlclass OnlineAccount QDeclarativeContactOnlineAccount
   \brief The OnlineAccount element contains a note associated with a contact.
   \ingroup qml-contacts

   OnlineAccount element contains the following field types:
   \list
   \o OnlineAccount.AccountUri - the account uri value.
   \o OnlineAccount.ServiceProvider - the account service provider name.
   \o OnlineAccount.Capabilities - the account capabilities value.
   \o OnlineAccount.SubTypes - the sub types of an online account.
   \endlist

  This element is part of the \bold{QtMobility.contacts 1.1} module.
 */

/*!
  \qmlproperty string OnlineAccount::accountUri

  This property holds the value of the account uri.
  */
/*!
  \qmlproperty string OnlineAccount::serviceProvider

  This property holds the value of the account service provider name.
  */
/*!
  \qmlproperty list<string> OnlineAccount::capabilities

  This property holds the value of the account capabilities.
  */
/*!
  \qmlproperty list<variant> OnlineAccount::subTypes

  This property holds the value of the sub types of an online account.

  \list
  \o OnlineAccount.Unknown (default)
  \o OnlineAccount.Sip - indicating this online account supports SIP.
  \o OnlineAccount.SipVoip - indicating this online account supports SIP based VOIP.
  \o OnlineAccount.Impp - indicating this online account supports IMPP.
  \o OnlineAccount.VideoShare - indicating this online account supports VideoShare.
  \endlist
  */

/* ==================== QDeclarativeContactOrganization ======================= */
/*!
   \qmlclass Organization QDeclarativeContactOrganization
   \brief The Organization element provides details about an
   organization that the contact is either a part of, or stands for.
   \ingroup qml-contacts

   Organization element contains the following field types:
   \list
   \o Organization.Name
   \o Organization.LogoUrl
   \o Organization.Department
   \o Organization.Location
   \o Organization.Role
   \o Organization.Title
   \o Organization.AssistantName
   \endlist

  This element is part of the \bold{QtMobility.contacts 1.1} module.
 */

/*!
  \qmlproperty string Organization::name

  This property holds the value of the organization name.
  */
/*!
  \qmlproperty url Organization::logoUrl

  This property holds the URL of the organization logo image.
  */
/*!
  \qmlproperty list<string> Organization::department

  This property holds the value of the department name.
  */
/*!
  \qmlproperty string Organization::location

  This property holds the value of the location of the organization.
  */
/*!
  \qmlproperty string Organization::role

  This property holds the value of the contact's role in the organization.
  */
/*!
  \qmlproperty string Organization::title

  This property holds the value of the contact's title in the organization.
  */
/*!
  \qmlproperty string Organization::assistantName

  This property holds the value of the name of the contact's assistant.
  */

/* ==================== QDeclarativeContactPhoneNumber ======================= */
/*!
   \qmlclass PhoneNumber QDeclarativeContactPhoneNumber
   \brief The PhoneNumber element provides a phone number of a contact.
   \ingroup qml-contacts

   PhoneNumber element contains the following field types:
   \list
   \o PhoneNumber.Number
   \o PhoneNumber.SubTypes
   \endlist

  This element is part of the \bold{QtMobility.contacts 1.1} module.
 */

/*!
  \qmlproperty string PhoneNumber::number

  This property holds the value of the phone number.
  */

/*!
  \qmlproperty list<variant> PhoneNumber::subTypes

  This property holds the sub types of a PhoneNumber.

  \list
  \o PhoneNumber.Unknown - indicating this phone number type is unknown(default).
  \o PhoneNumber.Landline - indicating this phone number is a landline number.
  \o PhoneNumber.Mobile - ndicating this phone number is a mobile (cellular) number.
  \o PhoneNumber.Fax - indicating this phone number is a fax number.
  \o PhoneNumber.Pager - indicating this phone number is a pager number.
  \o PhoneNumber.Voice - indicating this phone number supports voice transmission.
  \o PhoneNumber.Modem - indicating this phone number supports data transmission.
  \o PhoneNumber.Video - indicating this phone number supports video transmission.
  \o PhoneNumber.Car - indicating this phone number is a car phone.
  \o PhoneNumber.BulletinBoardSystem - indicating this phone number is a bulletin board system.
  \o PhoneNumber.MessagingCapable - indicating this phone number supports messaging services.
  \o PhoneNumber.Assistant - indicating this phone number is the number of an assistant.
  \o PhoneNumber.DtmfMenu - indicating this phone number supports DTMF-controlled voice menu navigation.
  \endlist
  */

/* ==================== QDeclarativeContactPresence ======================= */
/*!
   \qmlclass Presence QDeclarativeContactPresence
   \brief The Presence element provides presence information for an online account of a contact.
   \ingroup qml-contacts

   Presence element contains the following field types:
   \list
   \o Presence.Timestamp
   \o Presence.Nickname
   \o Presence.State
   \o Presence.StateText
   \o Presence.ImageUrl
   \o Presence.CustomMessage
   \endlist

  This element is part of the \bold{QtMobility.contacts 1.1} module.
 */

/*!
  \qmlproperty date Presence::timestamp

  This property holds the timestamp value of the Presence.
  */
/*!
  \qmlproperty string Presence::nickname

  This property holds the nickname value of the Presence.
  */
/*!
  \qmlproperty enumeration Presence::state

  This property holds the presence state enumeration value.

  \list
  \o Presence.Unknown -  Signifies that the presence state of the contact is not currently known (default).
  \o Presence.Available - Signifies that the contact is available.
  \o Presence.Hidden - Signifies that the contact is hidden.
  \o Presence.Busy - Signifies that the contact is busy.
  \o Presence.Away - Signifies that the contact is away.
  \o Presence.ExtendedAway - Signifies that the contact is away for an extended period of time.
  \o Presence.Offline - Signifies that the contact is offline.
  \endlist
  */
/*!
  \qmlproperty string Presence::stateText

  This property holds the text corresponding to the current presence state.
  */
/*!
  \qmlproperty url Presence::imageUrl

  This property holds the last-known status image url of the contact for the online account
   about which this detail stores presence information.
  */
/*!
  \qmlproperty string Presence::customMessage

  This property holds the custom status message from the contact for the online account
   about which this detail stores presence information.
  */



/* ==================== QDeclarativeContactRingtone ======================= */
/*!
   \qmlclass Ringtone QDeclarativeContactRingtone
   \brief The Ringtone element provides a ringtone associated with a contact.
   \ingroup qml-contacts

   Ringtone element contains the following field types:
   \list
   \o Ringtone.AudioRingtoneUrl
   \o Ringtone.VideoRingtoneUrl
   \o Ringtone.VibrationRingtoneUrl
   \endlist

  This element is part of the \bold{QtMobility.contacts 1.1} module.
 */

/*!
  \qmlproperty url Ringtone::audioRingtoneUrl

  This property holds the value of the URL for an audio ringtone.
  */
/*!
  \qmlproperty url Ringtone::videoRingtoneUrl

  This property holds the value of the URL for a video ringtone.
  */
/*!
  \qmlproperty url Ringtone::vibrationRingtoneUrl

  This property holds the value of the URL for a vibration ringtone.
  */

/* ==================== QDeclarativeContactSyncTarget ======================= */
/*!
   \qmlclass SyncTarget QDeclarativeContactSyncTarget
   \brief The SyncTarget element provides a sync target for a contact.
   \ingroup qml-contacts

   SyncTarget element contains the following field types:
   \list
   \o SyncTarget.SyncTarget
   \endlist

  This element is part of the \bold{QtMobility.contacts 1.1} module.
 */
/*!
  \qmlproperty string SyncTarget::syncTarget

  This property holds the sync target value.
  */

/* ==================== QDeclarativeContactTag ======================= */
/*!
   \qmlclass Tag QDeclarativeContactTag
   \brief The Tag element provides a contains a tag associated with a contact.
   \ingroup qml-contacts

   Tag element contains the following field types:
   \list
   \o Tag.Tag
   \endlist

  This element is part of the \bold{QtMobility.contacts 1.1} module.
 */
/*!
  \qmlproperty string Tag::tag

  This property holds the value of the tag.
  */

/* ==================== QDeclarativeContactTimestamp ======================= */
/*!
   \qmlclass Timestamp QDeclarativeContactTimestamp
   \brief The Timestamp element contains the creation and last-modified timestamp associated with the contact.
   \ingroup qml-contacts

   Timestamp element contains the following field types:
   \list
   \o Timestamp.LastModified
   \o Timestamp.Created
   \endlist

  This element is part of the \bold{QtMobility.contacts 1.1} module.
 */
/*!
  \qmlproperty date Timestamp::lastModified

  This property holds the value of the last modified timestamp.
  */
/*!
  \qmlproperty date Timestamp::created

  This property holds the value of the timestamp a contact was created.
  */

/* ==================== QDeclarativeContactUrl ======================= */
/*!
   \qmlclass Url QDeclarativeContactUrl
   \brief The Url element contains a url associated with a contact.
   \ingroup qml-contacts

   Url element contains the following field types:
   \list
   \o Url.Url
   \o Url.SubType
   \endlist

  This element is part of the \bold{QtMobility.contacts 1.1} module.
 */
/*!
  \qmlproperty string Url::url

  This property holds the value of the URL.
  */
/*!
  \qmlproperty enumeration Url::subType

  This property holds the sub type of a QContactUrl.

  \list
  \o Url.Unknown - indicating this url type is unknown (default).
  \o Url.HomePage - indicating this url is a contact's home page.
  \o Url.Favourite - indicating this url is one of the contact's favourite URLs (or bookmarks).
  \endlist
  */

