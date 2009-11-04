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

#include "qversitcontactconverter_p.h"
#include "qversitdefs.h"
#include "versitutils.h"
#include <qcontact.h>
#include <qcontactdetail.h>
#include <qcontactname.h>
#include <qcontactemailaddress.h>
#include <qcontactphonenumber.h>
#include <qcontactaddress.h>
#include <qcontacturl.h>
#include <qcontactguid.h>
#include <qcontactorganization.h>
#include <qcontacttimestamp.h>
#include <qcontactbirthday.h>
#include <qcontactnote.h>
#include <qcontactgeolocation.h>
#include <qcontactavatar.h>
#include <qcontactgender.h>
#include <qcontactnickname.h>
#include <qcontactanniversary.h>
#include <qcontactonlineaccount.h>
#include <qcontactfamily.h>
#include <QFile>
#include <QUrl>
#include <QBuffer>
#include <QImageWriter>

/*!
 * Constructor.
 */
QVersitContactConverterPrivate::QVersitContactConverterPrivate() :
    mVersitType(QVersitDocument::VCard21)
{                
    //Properties Mapping

    mPropertyMappings.insert(
        QContactName::DefinitionName,QString::fromAscii(versitNameId));
    mPropertyMappings.insert(
        QContactEmailAddress::DefinitionName,QString::fromAscii(versitEmailId));  
    mPropertyMappings.insert(
        QContactPhoneNumber::DefinitionName,QString::fromAscii(versitPhoneId));
    mPropertyMappings.insert(
        QContactAddress::DefinitionName,QString::fromAscii(versitAddressId));
    mPropertyMappings.insert(
        QContactUrl::DefinitionName,QString::fromAscii(versitUrlId));
    mPropertyMappings.insert(
        QContactGuid::DefinitionName,QString::fromAscii(versitUidId));
    mPropertyMappings.insert(
        QContactOrganization::DefinitionName,QString::fromAscii(versitOrganizationId));
    mPropertyMappings.insert(
        QContactTimestamp::DefinitionName,QString::fromAscii(versitRevId));
    mPropertyMappings.insert(
        QContactBirthday::DefinitionName,QString::fromAscii(versitBirthdayId));
    mPropertyMappings.insert(
        QContactNote::DefinitionName,QString::fromAscii(versitNoteId));
    mPropertyMappings.insert(
        QContactGeolocation::DefinitionName,QString::fromAscii(versitGeoId));
    mPropertyMappings.insert(
        QContactGender::DefinitionName,QString::fromAscii(versitGenderId));
    mPropertyMappings.insert(
        QContactNickname::DefinitionName,QString::fromAscii(versitNicknameXId));
    mPropertyMappings.insert(
        QContactAnniversary::DefinitionName,QString::fromAscii(versitAnniversaryId));

    // Sound is mapped to the Contact Audio Ringingtones that was the nearest match
    // field for the Sound
    mPropertyMappings.insert(
        QContactAvatar::SubTypeAudioRingtone,QString::fromAscii(versitSoundId));

    mPropertyMappings.insert(
        QContactAvatar::SubTypeImage,QString::fromAscii(versitPhotoId));


    // Parameters Table
    // Contexts
    mParameterMappings.insert(
        QContactDetail::ContextWork,QString::fromAscii(versitContextWorkId));
    mParameterMappings.insert(
        QContactDetail::ContextHome,QString::fromAscii(versitContextHomeId));
    
    // Subtypes
    mParameterMappings.insert(
        QContactAddress::SubTypeDomestic,QString::fromAscii(versitDomesticId));
    mParameterMappings.insert(
        QContactAddress::SubTypeInternational,QString::fromAscii(versitInternationalId));
    mParameterMappings.insert(
        QContactAddress::SubTypePostal,QString::fromAscii(versitPostalId));
    mParameterMappings.insert(
        QContactAddress::SubTypeParcel,QString::fromAscii(versitParcelId));
    mParameterMappings.insert(
        QContactPhoneNumber::SubTypeVoice,QString::fromAscii(versitVoiceId));
    mParameterMappings.insert(
        QContactPhoneNumber::SubTypeMobile,QString::fromAscii(versitCellId));
    mParameterMappings.insert(
        QContactPhoneNumber::SubTypeModem,QString::fromAscii(versitModemId));
    mParameterMappings.insert(
        QContactPhoneNumber::SubTypeCar,QString::fromAscii(versitCarId));
    mParameterMappings.insert(
        QContactPhoneNumber::SubTypeVideo,QString::fromAscii(versitVideoId));
    mParameterMappings.insert(
        QContactPhoneNumber::SubTypeFacsimile,QString::fromAscii(versitFaxId));
    mParameterMappings.insert(
        QContactPhoneNumber::SubTypeBulletinBoardSystem,QString::fromAscii(versitBbsId));
    mParameterMappings.insert(
        QContactPhoneNumber::SubTypePager,QString::fromAscii(versitPagerId));
    mParameterMappings.insert(
        QContactAnniversary::SubTypeWedding,QContactAnniversary::SubTypeWedding);
    mParameterMappings.insert(
        QContactAnniversary::SubTypeEngagement,QContactAnniversary::SubTypeEngagement);
    mParameterMappings.insert(
        QContactAnniversary::SubTypeHouse,QContactAnniversary::SubTypeHouse);
    mParameterMappings.insert(
        QContactAnniversary::SubTypeEmployment,QContactAnniversary::SubTypeEmployment);
    mParameterMappings.insert(
        QContactAnniversary::SubTypeMemorial,QContactAnniversary::SubTypeMemorial);
    mParameterMappings.insert(
        QContactOnlineAccount::SubTypeShareVideo,QString::fromAscii(versitSwisId));
    mParameterMappings.insert(
        QContactOnlineAccount::SubTypeInternet,QString::fromAscii(versitVoipId));

    //Media Types.
    mParameterMappings.insert(
        QString::fromAscii(versitJPEGExtenId),QString::fromAscii(versitFormatJpeg));
    mParameterMappings.insert(
        QString::fromAscii(versitWAVEExtenId),QString::fromAscii(versitFormatWave));
    mParameterMappings.insert(
        QString::fromAscii(versitPICTExtenId),QString::fromAscii(versitFormatPict));
    mParameterMappings.insert(
        QString::fromAscii(versitTIFFExtenId),QString::fromAscii(versitFormatWave));
    mParameterMappings.insert(
        QString::fromAscii(versitMPEGExtenId),QString::fromAscii(versitFormatMpeg));
    mParameterMappings.insert(
        QString::fromAscii(versitMPEG2ExtenId),QString::fromAscii(versitFormatMpeg2));
    mParameterMappings.insert(
        QString::fromAscii(versitQTIMEExtenId),QString::fromAscii(versitFormatQtime));
    mParameterMappings.insert(
        QString::fromAscii(versitAIFFExtenId),QString::fromAscii(versitFormatAiff));
}

/*!
 * Destructor.
 */
QVersitContactConverterPrivate::~QVersitContactConverterPrivate()
{
}

/*!
 * Encode Contact Field Information into the Versit Document 
 */
void QVersitContactConverterPrivate::encodeFieldInfo(
    QVersitDocument& document,
    const QContactDetail& detail)
{
    mVersitType = document.versitType();
    QVersitProperty property;
    property.setName(mPropertyMappings.value(detail.definitionName()));
    bool addProperty = true;

    if (detail.definitionName() == QContactName::DefinitionName) {
        encodeName(property, detail);
    } else if (detail.definitionName() == QContactPhoneNumber::DefinitionName) {
        encodePhoneNumber(property, detail);
    } else if (detail.definitionName() == QContactEmailAddress::DefinitionName) {
        encodeEmail(property, detail);
    } else if (detail.definitionName() == QContactAddress::DefinitionName) {
        encodeAddress(property, detail);
    } else if (detail.definitionName() == QContactGuid::DefinitionName) {
        encodeUid(property, detail);
    } else if (detail.definitionName() == QContactUrl::DefinitionName) {
        encodeUrl(property, detail);
    } else if (detail.definitionName() == QContactTimestamp::DefinitionName) {
        addProperty = encodeRev(property, detail);
    } else if (detail.definitionName() == QContactBirthday::DefinitionName) {
        encodeBirthDay(property, detail);
    } else if (detail.definitionName() == QContactGeolocation::DefinitionName) {
        encodeGeoLocation(property, detail);
    } else if (detail.definitionName() == QContactNote::DefinitionName) {
        encodeNote(property, detail);
    } else if (detail.definitionName() == QContactOrganization::DefinitionName) {
        encodeOrganization(document, detail);
        addProperty = false;
    } else if (detail.definitionName() == QContactAvatar::DefinitionName){
        addProperty = encodeAvatar(property, detail);
        if (!addProperty)
            mUnconvertedContactDetails.append(detail);
    } else if (detail.definitionName() == QContactAnniversary::DefinitionName) {
        encodeAnniversary(property, detail);
    } else if (detail.definitionName() == QContactNickname::DefinitionName) {
        encodeNickname(document, detail);
        addProperty = false;
    } else if (detail.definitionName() == QContactGender::DefinitionName) {
        encodeGender(property, detail);
    } else if (detail.definitionName() == QContactOnlineAccount::DefinitionName) {
        addProperty = encodeOnlineAccount(property, detail);
        if (!addProperty)
            mUnconvertedContactDetails.append(detail);
    }else if (detail.definitionName() == QContactFamily::DefinitionName) {
        addProperty = encodeFamily(document, detail);
    }else {
        addProperty = false;
        mUnconvertedContactDetails.append(detail);
    }

    if (addProperty)
        document.addProperty(property);
}

/*!
 * Encode Contact Name Field Information into the Versit Document 
 */
void QVersitContactConverterPrivate::encodeName(
    QVersitProperty& property, 
    const QContactDetail& detail)
{   
    QContactName contactName = static_cast<QContactName>(detail);
    QByteArray value =
        escape(contactName.last().toAscii()) + ';' +
        escape(contactName.first().toAscii()) + ';' +
        escape(contactName.middle().toAscii()) + ';' +
        escape(contactName.prefix().toAscii()) + ';' +
        escape(contactName.suffix().toAscii());
    property.setValue(value);
}

/*!
 * Encode Phone Numer Field Information into the Versit Document 
 */
void QVersitContactConverterPrivate::encodePhoneNumber(
    QVersitProperty& property,
    const QContactDetail& detail)
{
    QContactPhoneNumber phoneNumber = static_cast<QContactPhoneNumber>(detail);
    encodeParameters(property, phoneNumber.contexts(), phoneNumber.subTypes());
    setEscapedValue(property,phoneNumber.number());
}

/*!
 * Encode Email Field Information into the Versit Document 
 */
void QVersitContactConverterPrivate::encodeEmail(
    QVersitProperty& property,
    const QContactDetail& detail)
{
    QContactEmailAddress emailAddress = static_cast<QContactEmailAddress>(detail);
    encodeParameters(property, emailAddress.contexts());
    setEscapedValue(property,emailAddress.emailAddress());
}

/*!
 * Encode Address Field Information into the Versit Document 
 */
void QVersitContactConverterPrivate::encodeAddress(
    QVersitProperty& property,
    const QContactDetail& detail)
{
    QContactAddress address = static_cast<QContactAddress>(detail);
    encodeParameters(property, address.contexts(), address.subTypes());
    QByteArray value =
        escape(address.postOfficeBox().toAscii()) + ';' +
        ';' + // Extended address
        escape(address.street().toAscii()) + ';' +
        escape(address.locality().toAscii()) + ';' +
        escape(address.region().toAscii()) + ';' +
        escape(address.postcode().toAscii()) + ';' +
        escape(address.country().toAscii());
    property.setValue(value);
}

/*!
 * Encode URL Field Information into the Versit Document
 */
void QVersitContactConverterPrivate::encodeUrl(
    QVersitProperty& property,
    const QContactDetail& detail)
{
    QContactUrl contactUrl = static_cast<QContactUrl>(detail);
    encodeParameters(property, contactUrl.contexts());
    // The vCard specifications do not define any TYPEs for URL property.
    // No need to try to convert the subtypes to TYPEs.
    property.setValue(contactUrl.url().toAscii());
}

/*!
 * Encode Uid Field Information into the Versit Document
 */
void QVersitContactConverterPrivate::encodeUid(
    QVersitProperty& property,
    const QContactDetail& detail)
{
    QContactGuid uid = static_cast<QContactGuid>(detail);
    setEscapedValue(property,uid.guid());
}

/*!
 * Encode REV Field Information into the Versit Document
 */
bool QVersitContactConverterPrivate::encodeRev(
    QVersitProperty& property,
    const QContactDetail& detail)
{
    QContactTimestamp  rev = static_cast<QContactTimestamp>(detail);
    QString value;
    bool encoded = false;

    if ( rev.lastModified().toString(Qt::ISODate).size() ) {
        encoded = true;
        if ( rev.lastModified().timeSpec() == Qt::UTC ) {
            value = rev.lastModified().toString(Qt::ISODate) +
                QString::fromAscii(versitISOFormatSuffix);
        }
        else {
            value = rev.lastModified().toString(Qt::ISODate);
        }
    }

    else if ( rev.created().toString(Qt::ISODate).size()) {
        encoded = true;
        if ( rev.created().timeSpec() == Qt::UTC ) {
            value = rev.created().toString(Qt::ISODate) +
                QString::fromAscii(versitISOFormatSuffix);
        }
        else {
            value = rev.created().toString(Qt::ISODate);
        }
    }
    property.setValue(value.toAscii());
    return encoded;
}

/*!
 * Encode BirthDay Field Information into the Versit Document
 */
void QVersitContactConverterPrivate::encodeBirthDay(
    QVersitProperty& property,
    const QContactDetail& detail)
{
    QContactBirthday bday = static_cast<QContactBirthday>(detail);
    QString value = bday.date().toString(Qt::ISODate);
    property.setValue(value.toAscii());
}

/*!
 * Encode Comment i.e. Note Field Information into the Versit Document
 */
void QVersitContactConverterPrivate::encodeNote(
    QVersitProperty& property,
    const QContactDetail& detail)
{
    QContactNote contactNote = static_cast<QContactNote>(detail);
    setEscapedValue(property,contactNote.note());
}

/*!
 * Encode Geo Prpoperties Field Information into the Versit Document
 */
void QVersitContactConverterPrivate::encodeGeoLocation(
    QVersitProperty& property,
    const QContactDetail& detail)
{
    QContactGeolocation geoLocation = static_cast<QContactGeolocation>(detail);
    QByteArray longitude;
    QByteArray latitude;
    QByteArray value =
        longitude.setNum(geoLocation.longitude()) + ',' +
        latitude.setNum(geoLocation.latitude());
    property.setValue(value);
}

/*!
 * Encode all the organization properties to the versit document
 */
void QVersitContactConverterPrivate::encodeOrganization(
     QVersitDocument& document,
     const QContactDetail& detail)
{
    QContactOrganization organization = static_cast<QContactOrganization>(detail);
    if (organization.title().length() > 0) {
        QVersitProperty property;
        property.setName(QString::fromAscii(versitTitleId));
        setEscapedValue(property,organization.title());
        document.addProperty(property);
    }
    if (organization.name().length() > 0 || organization.department().length() > 0) {
        QVersitProperty property;
        property.setName(QString::fromAscii(versitOrganizationId));
        QByteArray value =
            escape(organization.name().toAscii()) + ";" +
            escape(organization.department().toAscii());
        property.setValue(value);
        document.addProperty(property);
    }
    if (organization.logo().length() > 0) {
        QVersitProperty property;
        if (encodeEmbeddedContent(organization.logo(), property, true)) {
            property.setName(QString::fromAscii(versitLogoId));
            document.addProperty(property);
        }
    }
    if (organization.assistantName().length() > 0) {
        QVersitProperty property;
        property.setName(QString::fromAscii(versitAssistantId));
        setEscapedValue(property,organization.assistantName());
        document.addProperty(property);
    }
}

/*!
 * Encode avatar Content into the Versit Document
 */
bool QVersitContactConverterPrivate::encodeAvatar(
    QVersitProperty& property,
    const QContactDetail& detail)
{
    bool encoded = false;
    bool scaling = false;
    QContactAvatar contactAvatar = static_cast<QContactAvatar>(detail);
    QString resourcePath = contactAvatar.avatar();

    if (mPropertyMappings.contains(contactAvatar.subType())) {
        scaling = (contactAvatar.subType() == QContactAvatar::SubTypeImage);
        encoded = encodeEmbeddedContent(resourcePath, property, scaling);
        if (encoded)
            property.setName(mPropertyMappings.value(contactAvatar.subType()));
    }
    return encoded;
}

/*!
 * Encode gender property information into Versit Document
 */
void QVersitContactConverterPrivate::encodeGender(
    QVersitProperty& property,const
    QContactDetail& detail)
{
    QContactGender gender = static_cast<QContactGender>(detail);
    setEscapedValue(property,gender.gender());
}

/*!
 * Encodes nickname property information into the Versit Document
 */
void QVersitContactConverterPrivate::encodeNickname(
    QVersitDocument& document,
    const QContactDetail& detail)
{
    QContactNickname nicknameDetail = static_cast<QContactNickname>(detail);
    QVersitProperty property;
    property.setName(QString::fromAscii(versitNicknameXId));
    bool found = false;
    for (int i=0; i < document.properties().count() && !found; i++) {
        QVersitProperty currentProperty = document.properties()[i];
        if (currentProperty.name() == QString::fromAscii(versitNicknameXId)) {
            property = currentProperty;
            found = true;
        }
    }
    QByteArray value(property.value());
    if (found)
        value += ',';
    QByteArray nickname = escape(nicknameDetail.nickname().toAscii());
    value.append(nickname);
    property.setValue(value);
    // Replace the current property
    document.removeProperties(QString::fromAscii(versitNicknameXId));
    document.addProperty(property);
}

/*!
 * Encode anniversary information into Versit Document
 */
void QVersitContactConverterPrivate::encodeAnniversary(
    QVersitProperty& property,
    const QContactDetail& detail)
{
    QContactAnniversary anniversary = static_cast<QContactAnniversary>(detail);
    encodeParameters(property, QStringList(), QStringList(anniversary.subType()));
    property.setValue(anniversary.originalDate().toString(Qt::ISODate).toAscii());
}

/*!
 * Encode online account information into the Versit Document
 */
bool QVersitContactConverterPrivate::encodeOnlineAccount(
    QVersitProperty& property,
    const QContactDetail& detail)
{
    bool encoded = false;
    QContactOnlineAccount onlineAccount = static_cast<QContactOnlineAccount>(detail);
    QStringList subTypes = onlineAccount.subTypes();

    if (subTypes.contains(QContactOnlineAccount::SubTypeSip) ||
        subTypes.contains(QContactOnlineAccount::SubTypeInternet) ||
        subTypes.contains(QContactOnlineAccount::SubTypeShareVideo)) {
        encoded = true;
        encodeParameters(property, onlineAccount.contexts(), subTypes);
        property.setName(QString::fromAscii(versitSipId));
        setEscapedValue(property,onlineAccount.accountUri());
    }
    return encoded;
}

/*!
 * Encode family versit property if its supported in Versit Document
 */
bool QVersitContactConverterPrivate::encodeFamily(
    QVersitDocument& document,
    const QContactDetail& detail )
{
    QContactFamily family = static_cast<QContactFamily>(detail);

    if (family.spouse().size()) {
        QVersitProperty property;
        property.setName(QString::fromAscii(versitSpouseId));
        setEscapedValue(property,family.spouse());
        document.addProperty(property);
    }

    if (family.children().size()) {
        QVersitProperty property;
        property.setName(QString::fromAscii(versitChildrenId));
        QString children = family.children().join(QString::fromAscii(","));
        setEscapedValue(property,children);
        document.addProperty(property);
    }
    return false;
}

/*!
 * Check if \a resourceIdentifier represents a valid remote resource
 */
bool QVersitContactConverterPrivate::isValidRemoteUrl(
    const QString& resourceIdentifier)
{
    QUrl remoteResource(resourceIdentifier);
    if ((!remoteResource.scheme().isEmpty() && !remoteResource.host().isEmpty()) ||
         resourceIdentifier.contains(QString::fromAscii(versitConstWWW),Qt::CaseInsensitive))
        return true;
    return false;
}

/*!
 * Encode parameters to \a property
 */
void QVersitContactConverterPrivate::encodeParameters(
    QVersitProperty& property,
    const QStringList& contexts,
    const QStringList& subTypes)
{
    QStringList parameterList(contexts); // Contexts should be encoded first
    parameterList.append(subTypes);
    while (!parameterList.isEmpty()) {
        QString value = parameterList.takeLast();
        QString mappedValue = mParameterMappings.value(value);
        if (mappedValue.length() > 0) {
            // QVersitProperty::addParameter inserts into beginning.
            // This is why the last value is taken from the list
            property.addParameter(QString::fromAscii(versitType),mappedValue);
        }
    }
}

/*!
 * Encode embedded content into the Versit Document
 */
bool QVersitContactConverterPrivate::encodeEmbeddedContent(
    const QString& resourcePath,
    QVersitProperty& property,
    bool performScaling)
{
    bool encodeContent = false;
    QString resourceExt = resourcePath.section(QString::fromAscii("."), -1).toUpper();
    QString resourceFormat = mParameterMappings.value(resourceExt);

    if (!resourceFormat.length())
        resourceFormat = resourceExt;

    if (resourceFormat.length() > 0) {
        QByteArray value;
        QFile resourceFile(resourcePath);
        if ( resourceFile.open(QIODevice::ReadOnly)) {
            encodeContent = true;
            if (performScaling)
                emit scale(resourcePath,value);
            if (value.length() == 0)
                value = resourceFile.readAll(); // Image not scaled
            value = value.toBase64();
            property.addParameter(
                QString::fromAscii(versitEncoding),
                QString::fromAscii(versitEncodingBase64));
            property.addParameter(QString::fromAscii(versitType),resourceFormat);
        }
        else if (isValidRemoteUrl( resourcePath )) {
            encodeContent = true;
            value = resourcePath.toAscii();
            property.addParameter(
                QString::fromAscii(versitValue),
                QString::fromAscii(versitUrlId));
            property.addParameter(QString::fromAscii(versitType),resourceFormat);
        } else {
            // The file has been removed. Don't encode the path to a local file.
        }
        property.setValue(value);
    }
    return encodeContent;
}

/*!
 * Escapes \a value if necessary and sets it to \a property
 */
void QVersitContactConverterPrivate::setEscapedValue(
    QVersitProperty& property,
    const QString& value)
{
    QByteArray escapedValue(escape(value.toAscii()));
    property.setValue(escapedValue);
}

/*!
 * Escapes \a value if necessary.
 * For vCard there is no concept of escaping the property values.
 * Starting from 3.0 the property values having certain special
 * characters should be escaped.
 */
QByteArray QVersitContactConverterPrivate::escape(const QByteArray& value)
{
    QByteArray escaped(value);
    if (mVersitType != QVersitDocument::VCard21) {
        VersitUtils::backSlashEscape(escaped);
    }
    return escaped;
}
