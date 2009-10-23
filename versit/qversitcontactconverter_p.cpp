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

#include <QFile>
#include <QUrl>
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


/*!
 * Constructor.
 */
QVersitContactConverterPrivate::QVersitContactConverterPrivate()
{                
    // Field names
    mMappings.insert(
        QContactName::DefinitionName,QString::fromAscii(versitNameId));
    mMappings.insert(
        QContactEmailAddress::DefinitionName,QString::fromAscii(versitEmailId));  
    mMappings.insert(
        QContactPhoneNumber::DefinitionName,QString::fromAscii(versitPhoneId));
    mMappings.insert(
        QContactAddress::DefinitionName,QString::fromAscii(versitAddressId));
    mMappings.insert(
        QContactUrl::DefinitionName,QString::fromAscii(versitUrlId));
    mMappings.insert(
        QContactGuid::DefinitionName,QString::fromAscii(versitUidId));
    mMappings.insert(
        QContactOrganization::DefinitionName,QString::fromAscii(versitOrganizationId));
    mMappings.insert(
        QContactTimestamp::DefinitionName,QString::fromAscii(versitRevId));
    mMappings.insert(
        QContactBirthday::DefinitionName,QString::fromAscii(versitBirthdayId));
    mMappings.insert(
        QContactNote::DefinitionName,QString::fromAscii(versitNoteId));
    mMappings.insert(
        QContactGeolocation::DefinitionName,QString::fromAscii(versitGeoId));
    mMappings.insert(
        QContactAvatar::DefinitionName,QString::fromAscii(versitPhotoId));
    
    // Contexts
    mMappings.insert(
        QContactDetail::ContextWork,QString::fromAscii(versitContextWorkId));
    mMappings.insert(
        QContactDetail::ContextHome,QString::fromAscii(versitContextHomeId));
    
    // Subtypes
    mMappings.insert(
        QContactAddress::SubTypeDomestic,QString::fromAscii(versitDomesticId));
    mMappings.insert(
        QContactAddress::SubTypeInternational,QString::fromAscii(versitInternationalId));
    mMappings.insert(
        QContactAddress::SubTypePostal,QString::fromAscii(versitPostalId));
    mMappings.insert(
        QContactAddress::SubTypeParcel,QString::fromAscii(versitParcelId));
    mMappings.insert(
        QContactPhoneNumber::SubTypeVoice,QString::fromAscii(versitVoiceId));
    mMappings.insert(
        QContactPhoneNumber::SubTypeMobile,QString::fromAscii(versitCellId));
    mMappings.insert(
        QContactPhoneNumber::SubTypeModem,QString::fromAscii(versitModemId));
    mMappings.insert(
        QContactPhoneNumber::SubTypeCar,QString::fromAscii(versitCarId));
    mMappings.insert(
        QContactPhoneNumber::SubTypeVideo,QString::fromAscii(versitVideoId));
    mMappings.insert(
        QContactPhoneNumber::SubTypeFacsimile,QString::fromAscii(versitFaxId));
    mMappings.insert(
        QContactPhoneNumber::SubTypeBulletinBoardSystem,QString::fromAscii(versitBbsId));
    mMappings.insert(
        QContactPhoneNumber::SubTypePager,QString::fromAscii(versitPagerId));
    mMappings.insert(
        QContactAvatar::SubTypeImage,QString::fromAscii(versitPhotoId));

    // Sound is mapped to the Contact Audio Ringingtones that was the nearest match
    // field for the Sound
    mMappings.insert(
        QContactAvatar::SubTypeAudioRingtone,QString::fromAscii(versitSoundId));

    //Media Types.
    mMappings.insert(
        QString::fromAscii(versitJPEGExtenId),QString::fromAscii(versitPhotoJpeg));
    mMappings.insert(
        QString::fromAscii(versitWAVEExtenId),QString::fromAscii(versitAudioWave));
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
    QVersitDocument& versitDocument, 
    const QContactDetail& detail)
{
    QVersitProperty property;
    property.setName(mMappings.value(detail.definitionName()));
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
        addProperty = encodeOrganization(versitDocument, detail);
    } else if (detail.definitionName() == QContactAvatar::DefinitionName){
        addProperty = encodeEmbeddedContent(property, detail);
    }else {
        addProperty = false;
    }

    if (addProperty)
        versitDocument.addProperty(property);
}

/*!
 * Encode Contact Name Field Information into the Versit Document 
 */
void QVersitContactConverterPrivate::encodeName(
    QVersitProperty& property, 
    const QContactDetail& detail)
{   
    QContactName contactName = static_cast<QContactName>(detail);
    QString value = contactName.last() +  QString::fromAscii(";") + 
                    contactName.first() + QString::fromAscii(";") + 
                    contactName.middle() + QString::fromAscii(";") + 
                    contactName.prefix() + QString::fromAscii(";") + 
                    contactName.suffix();
    property.setValue(value.toAscii());
}

/*!
 * Encode Phone Numer Field Information into the Versit Document 
 */
void QVersitContactConverterPrivate::encodePhoneNumber(
    QVersitProperty& property,
    const QContactDetail& detail)
{
    QContactPhoneNumber phoneNumer = static_cast<QContactPhoneNumber>(detail);
    encodeParameters(property, phoneNumer.contexts());
    encodeParameters(property, phoneNumer.subTypes());
    property.setValue(phoneNumer.number().toAscii());
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
    property.setValue(emailAddress.emailAddress().toAscii());
}

/*!
 * Encode Address Field Information into the Versit Document 
 */
void QVersitContactConverterPrivate::encodeAddress(
    QVersitProperty& property,
    const QContactDetail& detail )
{
    QContactAddress address = static_cast<QContactAddress>(detail);    
    encodeParameters(property, address.contexts());
    encodeParameters(property, address.subTypes());
    QString value = address.postOfficeBox() + QString::fromAscii(";") +
                    QString::fromAscii(";") + address.street() +
                    QString::fromAscii(";") + address.locality() + 
                    QString::fromAscii(";") + address.region() + 
                    QString::fromAscii(";") + address.postcode() + 
                    QString::fromAscii(";") + address.country();
    property.setValue(value.toAscii());
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
    property.setValue(uid.guid().toAscii());
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
    bool encode = false;

    if ( rev.lastModified().toString(Qt::ISODate).size() ) {
        encode = true;
        if ( rev.lastModified().timeSpec() == Qt::UTC ) {
            value = rev.lastModified().toString(Qt::ISODate) +
                QString::fromAscii(versitISOFormatSuffix);
        }
        else {
            value = rev.lastModified().toString(Qt::ISODate);
        }
    }

    else if ( rev.created().toString(Qt::ISODate).size()) {
        encode = true;
        if ( rev.created().timeSpec() == Qt::UTC ) {
            value = rev.created().toString(Qt::ISODate) +
                QString::fromAscii(versitISOFormatSuffix);
        }
        else {
            value = rev.created().toString(Qt::ISODate);
        }
    }
    property.setValue(value.toAscii());
    return encode;
}



/*!
 * Encode BirthDay Field Information into the Versit Document
 */
void QVersitContactConverterPrivate::encodeBirthDay(
    QVersitProperty& property,
    const QContactDetail& detail )
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
    property.setValue(contactNote.note().toAscii());
}


/*!
 * Encode Geo Prpoperties Field Information into the Versit Document
 */
void QVersitContactConverterPrivate::encodeGeoLocation(
    QVersitProperty& property,
    const QContactDetail& detail)
{
    QContactGeolocation geoLocation = static_cast<QContactGeolocation>(detail);
    QString longitude;
    QString latitude;
    QString value = longitude.setNum(geoLocation.longitude()) +
                    QString::fromAscii(",") +
                    latitude.setNum(geoLocation.latitude());
    property.setValue(value.toAscii());
}

/*!
 * Encode all the organization properties to the versit document
 */
bool QVersitContactConverterPrivate::encodeOrganization(
     QVersitDocument& document,
     const QContactDetail& detail)
{
    QContactOrganization organization = static_cast<QContactOrganization>(detail);
    if (organization.title().length() > 0) {
        QVersitProperty property;
        property.setName(QString::fromAscii(versitTitleId));
        property.setValue(organization.title().toAscii());
        document.addProperty(property);
    }
    if (organization.name().length() > 0 || organization.department().length() > 0) {
        QVersitProperty property;
        property.setName(QString::fromAscii(versitOrganizationId));
        QByteArray value =
            organization.name().toAscii() + ";" +organization.department().toAscii();
        property.setValue(value);
        document.addProperty(property);
    }
    return false;
}



/*!
 * Encode Embedded Content into the Versit Document
 */

bool QVersitContactConverterPrivate::encodeEmbeddedContent(QVersitProperty& property,
                                                           const QContactDetail& detail )
{
    bool encodeProperty = false;
    QContactAvatar contactAvatar = static_cast<QContactAvatar >(detail);
    QString avatarPath = contactAvatar.avatar();
    QString avatarExt = avatarPath.section('.', -1).toUpper();
    QString avatarFormat = mMappings.value( avatarExt );

    if ( !avatarFormat.size())
        avatarFormat = avatarExt;

    if ( mMappings.contains(contactAvatar.subType()) &&
         avatarFormat.size()) {

        QString name = mMappings.value(contactAvatar.subType());
        QByteArray value;

        QFile avtarFile;
        avtarFile.setFileName(avatarPath);
        if ( avtarFile.open(QIODevice::ReadOnly )) {
            encodeProperty = true;
            value = avtarFile.readAll().toBase64();
            property.addParameter(QString::fromAscii(versitType),avatarFormat);
            property.addParameter(QString::fromAscii(versitEncoding),
                                  QString::fromAscii(versitEncodingBase64));
        }
        else if (isVaildRemoteURL( avatarPath )) {
            encodeProperty = true;
            value = avatarPath.toAscii();
            property.addParameter(QString::fromAscii(versitValue),versitUrlId);
            property.addParameter(QString::fromAscii(versitType),avatarFormat);
        }

        //Add Values
        property.setName(name);
        property.setValue(value);
    }

    return encodeProperty;
}

/*!
 * Encode Parameter for versit property if its supported in Versit Document 
 */
void QVersitContactConverterPrivate::encodeParameters(
    QVersitProperty& property,
    const QStringList& paramList)
{
    foreach (QString type, paramList) {
        QString parameterValue = mMappings.value(type);
        if (parameterValue.size())
            property.addParameter(QString::fromAscii(versitType),parameterValue);
    }
}



/*!
 * Check if the Remote resouce represents a Valid remote resouce
 */
bool QVersitContactConverterPrivate::isVaildRemoteURL(const QString& resouceIdentifier )
{
    QUrl remoteResouce(resouceIdentifier);
    if ( ( !remoteResouce.scheme().isEmpty() &&
         !remoteResouce.host().isEmpty()) ||
         resouceIdentifier.contains(QString::fromAscii(versitConstWWW), Qt::CaseInsensitive))
        return true;
    return false;
}
