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

#include "qversitcontactexporter.h"
#include "qversitcontactexporter_p.h"
#include "qversitorganizerdefs_p.h"
#include "versitutils_p.h"
#include "qmobilityglobal.h"

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
#include <qcontactdisplaylabel.h>
#include <qcontactthumbnail.h>

#include <QUrl>
#include <QBuffer>

QTM_USE_NAMESPACE

/*!
 * Constructor.
 */
QVersitContactExporterPrivate::QVersitContactExporterPrivate() :
    mDetailHandler(NULL),
    mDetailHandler2(NULL),
    mDetailHandlerVersion(0),
    mDefaultResourceHandler(new QVersitDefaultResourceHandler),
    mResourceHandler(mDefaultResourceHandler)
{
    // Detail mappings
    int versitPropertyCount =
        sizeof(versitContactDetailMappings)/sizeof(VersitDetailMapping);
    for (int i=0; i < versitPropertyCount; i++) {
        mPropertyMappings.insert(
                QLatin1String(versitContactDetailMappings[i].detailDefinitionName),
                QLatin1String(versitContactDetailMappings[i].versitPropertyName));
    }

    // Contexts mappings
    int contextCount = sizeof(versitContextMappings)/sizeof(VersitMapping);
    for (int i=0; i < contextCount; i++) {
        mParameterMappings.insert(
                QLatin1String(versitContextMappings[i].contactString),
                QLatin1String(versitContextMappings[i].versitString));
    }

    // Subtypes mappings
    int subTypeCount = sizeof(versitSubTypeMappings)/sizeof(VersitMapping);
    for (int i=0; i < subTypeCount; i++) {
        mParameterMappings.insert(
                QLatin1String(versitSubTypeMappings[i].contactString),
                QLatin1String(versitSubTypeMappings[i].versitString));
    }
}

/*!
 * Destructor.
 */
QVersitContactExporterPrivate::~QVersitContactExporterPrivate()
{
    delete mDefaultResourceHandler;
}


/*!
 * Export QT Contact into Versit Document.
 */
bool QVersitContactExporterPrivate::exportContact(
    const QContact& contact,
    QVersitDocument& document,
    QVersitContactExporter::Error* error)
{
    QList<QContactDetail> allDetails = contact.details();
    if (allDetails.isEmpty()) {
        *error = QVersitContactExporter::EmptyContactError;
        return false;
    }
    foreach (const QContactDetail& detail, allDetails) {
        // If the custom detail handler handles it, we don't have to.
        if (mDetailHandler
            && mDetailHandler->preProcessDetail(contact, detail, &document))
            continue;

        QList<QVersitProperty> removedProperties;
        QList<QVersitProperty> generatedProperties;
        QSet<QString> processedFields;

        if (detail.definitionName() == QContactName::DefinitionName) {
            encodeName(detail, document, &removedProperties, &generatedProperties, &processedFields);
        } else if (detail.definitionName() == QContactPhoneNumber::DefinitionName) {
            encodePhoneNumber(detail, &generatedProperties, &processedFields);
        } else if (detail.definitionName() == QContactEmailAddress::DefinitionName) {
            encodeEmail(detail, &generatedProperties, &processedFields);
        } else if (detail.definitionName() == QContactAddress::DefinitionName) {
            encodeAddress(detail, &generatedProperties, &processedFields);
        } else if (detail.definitionName() == QContactGuid::DefinitionName) {
            encodeUid(detail, &generatedProperties, &processedFields);
        } else if (detail.definitionName() == QContactUrl::DefinitionName) {
            encodeUrl(detail, &generatedProperties, &processedFields);
        } else if (detail.definitionName() == QContactTimestamp::DefinitionName) {
            encodeRev(detail, &generatedProperties, &processedFields);
        } else if (detail.definitionName() == QContactBirthday::DefinitionName) {
            encodeBirthDay(detail, &generatedProperties, &processedFields);
        } else if (detail.definitionName() == QContactGeoLocation::DefinitionName) {
            encodeGeoLocation(detail, &generatedProperties, &processedFields);
        } else if (detail.definitionName() == QContactNote::DefinitionName) {
            encodeNote(detail, &generatedProperties, &processedFields);
        } else if (detail.definitionName() == QContactOrganization::DefinitionName) {
            encodeOrganization(detail, &generatedProperties, &processedFields);
        } else if (detail.definitionName() == QContactRingtone::DefinitionName) {
            encodeRingtone(detail, &generatedProperties, &processedFields);
        } else if (detail.definitionName() == QContactThumbnail::DefinitionName) {
            encodeThumbnail(detail, &generatedProperties, &processedFields);
        } else if (detail.definitionName() == QContactAvatar::DefinitionName){
            encodeAvatar(detail, &generatedProperties, &processedFields);
        } else if (detail.definitionName() == QContactAnniversary::DefinitionName) {
            encodeAnniversary(detail, &generatedProperties, &processedFields);
        } else if (detail.definitionName() == QContactNickname::DefinitionName) {
            encodeNickname(detail, document, &removedProperties, &generatedProperties, &processedFields);
        } else if (detail.definitionName() == QContactTag::DefinitionName) {
            encodeTag(detail, document, &removedProperties, &generatedProperties, &processedFields);
        } else if (detail.definitionName() == QContactGender::DefinitionName) {
            encodeGender(detail, &generatedProperties, &processedFields);
        } else if (detail.definitionName() == QContactOnlineAccount::DefinitionName) {
            encodeOnlineAccount(detail, &generatedProperties, &processedFields);
        } else if (detail.definitionName() == QContactFamily::DefinitionName) {
            encodeFamily(detail, &generatedProperties, &processedFields);
        } else if (detail.definitionName() == QContactDisplayLabel::DefinitionName) {
            encodeDisplayLabel(detail, document, &removedProperties, &generatedProperties, &processedFields);
        }

        if (mDetailHandler2 && mDetailHandlerVersion > 1) {
            mDetailHandler2->detailProcessed(contact, detail, processedFields, document,
                                             &removedProperties, &generatedProperties);
        }

        foreach(const QVersitProperty& property, removedProperties) {
            document.removeProperty(property);
        }
        foreach(const QVersitProperty& property, generatedProperties) {
            document.addProperty(property);
        }

        if (mDetailHandler && mDetailHandlerVersion == 1) {
            mDetailHandler->postProcessDetail(contact, detail, !processedFields.isEmpty(), &document);
        }
    }

    if (mDetailHandler2 && mDetailHandlerVersion > 1) {
        mDetailHandler2->contactProcessed(contact, &document);
    }

    // Search through the document for FN or N properties.  This will find it even if it was added
    // by a detail handler.
    if (!documentContainsName(document)) {
        *error = QVersitContactExporter::NoNameError;
        return false;
    }
    return true;
}

/*!
 * Returns true if and only if \a document has a "FN" or "N" property.
 */
bool QVersitContactExporterPrivate::documentContainsName(const QVersitDocument &document)
{
    foreach (const QVersitProperty& property, document.properties()) {
        const QString& name = property.name();
        if (name == QLatin1String("FN") || name == QLatin1String("N"))
            return true;
    }
    return false;
}

/*!
 * Encode Contact Name Field Information into the Versit Document
 */
void QVersitContactExporterPrivate::encodeName(
    const QContactDetail& detail,
    const QVersitDocument& document,
    QList<QVersitProperty>* removedProperties,
    QList<QVersitProperty>* generatedProperties,
    QSet<QString>* processedFields)
{
    QContactName contactName = static_cast<QContactName>(detail);
    if (!contactName.lastName().isEmpty()
        || !contactName.firstName().isEmpty()
        || !contactName.middleName().isEmpty()
        || !contactName.prefix().isEmpty()
        || !contactName.suffix().isEmpty()) {
        QVersitProperty property;
        property.setName(mPropertyMappings.value(detail.definitionName()));
        property.setValue(QStringList()
                          << contactName.lastName()
                          << contactName.firstName()
                          << contactName.middleName()
                          << contactName.prefix()
                          << contactName.suffix());
        property.setValueType(QVersitProperty::CompoundType);
        *generatedProperties << property;
    }

    // Generate an FN field if none is already there
    // Don't override previously exported FN properties (eg. exported by a DisplayLabel detail)
    QVersitProperty fnProperty =
        VersitUtils::takeProperty(document, QLatin1String("FN"), removedProperties);
    if (fnProperty.value().isEmpty()) {
        fnProperty.setName(QLatin1String("FN"));
        if (!contactName.customLabel().isEmpty()) {
            fnProperty.setValue(contactName.customLabel());
        } else if (!contactName.firstName().isEmpty()) {
            if (!contactName.lastName().isEmpty()) {
                fnProperty.setValue(QString::fromAscii("%1 %2").arg(contactName.firstName(),
                                                                    contactName.lastName()));
            } else {
                fnProperty.setValue(contactName.firstName());
            }
        } else if (!contactName.lastName().isEmpty()) {
            fnProperty.setValue(contactName.lastName());
        }
    }

    *generatedProperties << fnProperty;
    *processedFields << QContactName::FieldLastName
                     << QContactName::FieldFirstName
                     << QContactName::FieldMiddleName
                     << QContactName::FieldPrefix
                     << QContactName::FieldSuffix
                     << QContactName::FieldCustomLabel;
}

/*!
 * Encode Phone Number Field Information into the Versit Document
 */
void QVersitContactExporterPrivate::encodePhoneNumber(
    const QContactDetail& detail,
    QList<QVersitProperty>* generatedProperties,
    QSet<QString>* processedFields)
{
    QContactPhoneNumber phoneNumber = static_cast<QContactPhoneNumber>(detail);
    QStringList subTypes = phoneNumber.subTypes();
    QVersitProperty property;
    if (subTypes.contains(QContactPhoneNumber::SubTypeAssistant))
        property.setName(QLatin1String("X-ASSISTANT-TEL"));
    else
        property.setName(QLatin1String("TEL"));
    encodeParameters(property, phoneNumber.contexts(), subTypes);
    property.setValue(phoneNumber.number());
    *generatedProperties << property;
    *processedFields << QContactPhoneNumber::FieldContext
                      << QContactPhoneNumber::FieldSubTypes
                      << QContactPhoneNumber::FieldNumber;
}

/*!
 * Encode Email Field Information into the Versit Document
 */
void QVersitContactExporterPrivate::encodeEmail(
    const QContactDetail& detail,
    QList<QVersitProperty>* generatedProperties,
    QSet<QString>* processedFields)
{
    QContactEmailAddress emailAddress = static_cast<QContactEmailAddress>(detail);
    QVersitProperty property;
    property.setName(mPropertyMappings.value(detail.definitionName()));
    encodeParameters(property, emailAddress.contexts());
    property.setValue(emailAddress.emailAddress());
    *generatedProperties << property;
    *processedFields << QContactEmailAddress::FieldContext
                      << QContactEmailAddress::FieldEmailAddress;
}

/*!
 * Encode Address Field Information into the Versit Document
 */
void QVersitContactExporterPrivate::encodeAddress(
    const QContactDetail& detail,
    QList<QVersitProperty>* generatedProperties,
    QSet<QString>* processedFields)
{
    QContactAddress address = static_cast<QContactAddress>(detail);
    QVersitProperty property;
    property.setName(mPropertyMappings.value(detail.definitionName()));
    encodeParameters(property, address.contexts(), address.subTypes());
    property.setValue(QStringList()
                      << address.postOfficeBox()
                      << QString() // Leave out the extended address field
                      << address.street()
                      << address.locality()
                      << address.region()
                      << address.postcode()
                      << address.country());
    property.setValueType(QVersitProperty::CompoundType);
    *generatedProperties << property;
    *processedFields << QContactAddress::FieldContext
                      << QContactAddress::FieldSubTypes
                      << QContactAddress::FieldPostOfficeBox
                      << QContactAddress::FieldStreet
                      << QContactAddress::FieldLocality
                      << QContactAddress::FieldRegion
                      << QContactAddress::FieldPostcode
                      << QContactAddress::FieldCountry;
}

/*!
 * Encode URL Field Information into the Versit Document
 */
void QVersitContactExporterPrivate::encodeUrl(
    const QContactDetail& detail,
    QList<QVersitProperty>* generatedProperties,
    QSet<QString>* processedFields)
{
    QContactUrl contactUrl = static_cast<QContactUrl>(detail);
    QVersitProperty property;
    property.setName(mPropertyMappings.value(detail.definitionName()));
    encodeParameters(property, contactUrl.contexts());
    // The vCard specifications do not define any TYPEs for URL property.
    // No need to try to convert the subtypes to TYPEs.
    property.setValue(contactUrl.url());
    *generatedProperties << property;
    *processedFields << QContactUrl::FieldContext
                      << QContactUrl::FieldUrl;
}

/*!
 * Encode Uid Field Information into the Versit Document
 */
void QVersitContactExporterPrivate::encodeUid(
    const QContactDetail& detail,
    QList<QVersitProperty>* generatedProperties,
    QSet<QString>* processedFields)
{
    QContactGuid uid = static_cast<QContactGuid>(detail);
    QVersitProperty property;
    property.setName(mPropertyMappings.value(detail.definitionName()));
    property.setValue(uid.guid());
    *generatedProperties << property;
    *processedFields << QContactGuid::FieldGuid;
}

/*!
 * Encode REV Field Information into the Versit Document
 */
void QVersitContactExporterPrivate::encodeRev(
    const QContactDetail& detail,
    QList<QVersitProperty>* generatedProperties,
    QSet<QString>* processedFields)
{
    QContactTimestamp  rev = static_cast<QContactTimestamp>(detail);
    QString value;
    QVersitProperty property;
    property.setName(mPropertyMappings.value(detail.definitionName()));

    if ( rev.lastModified().toString(Qt::ISODate).size() ) {
        if ( rev.lastModified().timeSpec() == Qt::UTC ) {
            value = rev.lastModified().toString(Qt::ISODate) + QLatin1Char('Z');
        }
        else {
            value = rev.lastModified().toString(Qt::ISODate);
        }
        property.setValue(value);
        *generatedProperties << property;
        *processedFields << QContactTimestamp::FieldModificationTimestamp;
    } else if ( rev.created().toString(Qt::ISODate).size()) {
        if ( rev.created().timeSpec() == Qt::UTC ) {
            value = rev.created().toString(Qt::ISODate) + QLatin1Char('Z');
        }
        else {
            value = rev.created().toString(Qt::ISODate);
        }
        property.setValue(value);
        *generatedProperties << property;
        *processedFields << QContactTimestamp::FieldCreationTimestamp;
    }
}

/*!
 * Encode BirthDay Field Information into the Versit Document
 */
void QVersitContactExporterPrivate::encodeBirthDay(
    const QContactDetail& detail,
    QList<QVersitProperty>* generatedProperties,
    QSet<QString>* processedFields)
{
    QContactBirthday bday = static_cast<QContactBirthday>(detail);
    QVersitProperty property;
    property.setName(mPropertyMappings.value(detail.definitionName()));
    QString value = bday.date().toString(Qt::ISODate);
    property.setValue(value);
    *generatedProperties << property;
    *processedFields << QContactBirthday::FieldBirthday;
}

/*!
 * Encode Comment i.e. Note Field Information into the Versit Document
 */
void QVersitContactExporterPrivate::encodeNote(
    const QContactDetail& detail,
    QList<QVersitProperty>* generatedProperties,
    QSet<QString>* processedFields)
{
    QContactNote contactNote = static_cast<QContactNote>(detail);
    QVersitProperty property;
    property.setName(mPropertyMappings.value(detail.definitionName()));
    property.setValue(contactNote.note());
    *generatedProperties << property;
    *processedFields << QContactNote::FieldNote;
}

/*!
 * Encode Geo Prpoperties Field Information into the Versit Document
 */
void QVersitContactExporterPrivate::encodeGeoLocation(
    const QContactDetail& detail,
    QList<QVersitProperty>* generatedProperties,
    QSet<QString>* processedFields)
{
    QContactGeoLocation geoLocation = static_cast<QContactGeoLocation>(detail);
    QVersitProperty property;
    property.setName(mPropertyMappings.value(detail.definitionName()));
    property.setValue(QStringList() << QString::number(geoLocation.longitude())
                      << QString::number(geoLocation.latitude()));
    property.setValueType(QVersitProperty::CompoundType);
    *generatedProperties << property;
    *processedFields << QContactGeoLocation::FieldLongitude
                      << QContactGeoLocation::FieldLatitude;
}

/*!
 * Encode organization properties to the versit document
 */
void QVersitContactExporterPrivate::encodeOrganization(
    const QContactDetail& detail,
    QList<QVersitProperty>* generatedProperties,
    QSet<QString>* processedFields)
{
    QContactOrganization organization = static_cast<QContactOrganization>(detail);
    if (organization.title().length() > 0) {
        QVersitProperty property;
        property.setName(QLatin1String("TITLE"));
        property.setValue(organization.title());
        *generatedProperties << property;
        *processedFields << QContactOrganization::FieldTitle;
    }
    if (organization.name().length() > 0 || organization.department().size() > 0) {
        QVersitProperty property;
        property.setName(QLatin1String("ORG"));
        QStringList values(organization.name());
        values.append(organization.department());
        property.setValue(values);
        property.setValueType(QVersitProperty::CompoundType);
        *generatedProperties << property;
        *processedFields << QContactOrganization::FieldName
                          << QContactOrganization::FieldDepartment;
    }
    if (organization.logoUrl().isValid()) {
        QVersitProperty property;
        if (encodeContentFromFile(organization.logoUrl().toString(), property)) {
            property.setName(QLatin1String("LOGO"));
            *generatedProperties << property;
            *processedFields << QContactOrganization::FieldLogoUrl;
        }
    }
    if (organization.assistantName().length() > 0) {
        QVersitProperty property;
        property.setName(QLatin1String("X-ASSISTANT"));
        property.setValue(organization.assistantName());
        *generatedProperties << property;
        *processedFields << QContactOrganization::FieldAssistantName;
    }

    if (organization.role().length() > 0) {
        QVersitProperty property;
        property.setName(QLatin1String("ROLE"));
        property.setValue(organization.role());
        *generatedProperties << property;
        *processedFields << QContactOrganization::FieldRole;
    }
}

void QVersitContactExporterPrivate::encodeRingtone(
    const QContactDetail &detail,
    QList<QVersitProperty>* generatedProperties,
    QSet<QString>* processedFields)
{
    QContactRingtone ringtone = static_cast<QContactRingtone>(detail);
    QVersitProperty property;
    property.setName(mPropertyMappings.value(detail.definitionName()));
    if (encodeContentFromFile(ringtone.audioRingtoneUrl().toLocalFile(), property)) {
        *generatedProperties << property;
        *processedFields << QContactRingtone::FieldAudioRingtoneUrl;
    }
}

/*!
 * Encode thumbnail content into the Versit Document
 */
void QVersitContactExporterPrivate::encodeThumbnail(
    const QContactDetail &detail,
    QList<QVersitProperty>* generatedProperties,
    QSet<QString>* processedFields)
{
    QContactThumbnail contactThumbnail = static_cast<QContactThumbnail>(detail);
    QVersitProperty property;
    property.setName(QLatin1String("PHOTO"));
    QImage image = contactThumbnail.thumbnail();
    if (image.isNull())
        return;
    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);
    // Always store a pixmap as a PNG.
    if (!image.save(&buffer, "PNG"))
        return;
    property.setValue(imageData);
    property.insertParameter(QLatin1String("TYPE"), QLatin1String("PNG"));
    *generatedProperties << property;
    *processedFields << QContactThumbnail::FieldThumbnail;
}

/*!
 * Encode avatar URIs into the Versit Document
 */
void QVersitContactExporterPrivate::encodeAvatar(
    const QContactDetail &detail,
    QList<QVersitProperty>* generatedProperties,
    QSet<QString>* processedFields)
{
    QVersitProperty property;
    property.setName(QLatin1String("PHOTO"));
    QContactAvatar contactAvatar = static_cast<QContactAvatar>(detail);
    QUrl imageUrl(contactAvatar.imageUrl());
    // XXX: fix up this mess: checking the scheme here and in encodeContentFromFile,
    // organisation logo and ringtone are QStrings but avatar is a QUrl
    if (!imageUrl.scheme().isEmpty() && !imageUrl.host().isEmpty()) {
        property.insertParameter(QLatin1String("VALUE"), QLatin1String("URL"));
        property.setValue(imageUrl.toString());
        *generatedProperties << property;
        *processedFields << QContactAvatar::FieldImageUrl;
    } else {
        if (encodeContentFromFile(contactAvatar.imageUrl().toString(), property)) {
            *generatedProperties << property;
            *processedFields << QContactAvatar::FieldImageUrl;
        }
    }
}

/*!
 * Encode gender property information into Versit Document
 */
void QVersitContactExporterPrivate::encodeGender(
    const QContactDetail &detail,
    QList<QVersitProperty>* generatedProperties,
    QSet<QString>* processedFields)
{
    QContactGender gender = static_cast<QContactGender>(detail);
    QVersitProperty property;
    property.setName(mPropertyMappings.value(detail.definitionName()));
    property.setValue(gender.gender());
    *generatedProperties << property;
    *processedFields << QContactGender::FieldGender;
}

/*!
 * Encodes nickname property information into the Versit Document
 */
void QVersitContactExporterPrivate::encodeNickname(
    const QContactDetail &detail,
    const QVersitDocument& document,
    QList<QVersitProperty>* removedProperties,
    QList<QVersitProperty>* generatedProperties,
    QSet<QString>* processedFields)
{
    QContactNickname nicknameDetail = static_cast<QContactNickname>(detail);
    QVersitProperty property =
        VersitUtils::takeProperty(document, QLatin1String("X-NICKNAME"), removedProperties);
    property.setName(QLatin1String("X-NICKNAME"));
    QStringList value(property.variantValue().toStringList());
    value.append(nicknameDetail.nickname());
    property.setValue(value);
    property.setValueType(QVersitProperty::ListType);
    *generatedProperties << property;
    *processedFields << QContactNickname::FieldNickname;
}

/*!
 * Encodes a contact tag into the Versit Document
 */
void QVersitContactExporterPrivate::encodeTag(
    const QContactDetail &detail,
    const QVersitDocument& document,
    QList<QVersitProperty>* removedProperties,
    QList<QVersitProperty>* generatedProperties,
    QSet<QString>* processedFields)
{
    QContactTag tagDetail = static_cast<QContactTag>(detail);
    QVersitProperty property =
        VersitUtils::takeProperty(document, QLatin1String("CATEGORIES"), removedProperties);
    property.setName(QLatin1String("CATEGORIES"));
    QStringList value(property.variantValue().toStringList());
    value.append(tagDetail.tag());
    property.setValue(value);
    property.setValueType(QVersitProperty::ListType);
    *generatedProperties << property;
    *processedFields << QContactTag::FieldTag;
}

/*!
 * Encode anniversary information into Versit Document
 */
void QVersitContactExporterPrivate::encodeAnniversary(
    const QContactDetail &detail,
    QList<QVersitProperty>* generatedProperties,
    QSet<QString>* processedFields)
{
    QContactAnniversary anniversary = static_cast<QContactAnniversary>(detail);
    QVersitProperty property;
    property.setName(mPropertyMappings.value(detail.definitionName()));
    property.setValue(anniversary.originalDate().toString(Qt::ISODate));
    *generatedProperties << property;
    *processedFields << QContactAnniversary::FieldOriginalDate;
}

/*!
 * Encode online account information into the Versit Document
 */
void QVersitContactExporterPrivate::encodeOnlineAccount(
    const QContactDetail &detail,
    QList<QVersitProperty>* generatedProperties,
    QSet<QString>* processedFields)
{
    QContactOnlineAccount onlineAccount = static_cast<QContactOnlineAccount>(detail);
    QStringList subTypes = onlineAccount.subTypes();

    if (subTypes.contains(QContactOnlineAccount::SubTypeSip) ||
        subTypes.contains(QContactOnlineAccount::SubTypeSipVoip) ||
        subTypes.contains(QContactOnlineAccount::SubTypeVideoShare) ||
        subTypes.contains(QContactOnlineAccount::SubTypeImpp)) {
        QVersitProperty property;
        encodeParameters(property, onlineAccount.contexts(), subTypes);
        QString name(QLatin1String("X-SIP"));
        if (subTypes.contains(QContactOnlineAccount::SubTypeImpp))
            name = QLatin1String("X-IMPP");
        property.setName(name);
        property.setValue(onlineAccount.accountUri());
        *generatedProperties << property;
        *processedFields << QContactOnlineAccount::FieldSubTypes
                          << QContactOnlineAccount::FieldAccountUri;
    }
}

/*!
 * Encode family versit property if its supported in Versit Document
 */
void QVersitContactExporterPrivate::encodeFamily(
    const QContactDetail &detail,
    QList<QVersitProperty>* generatedProperties,
    QSet<QString>* processedFields)
{
    QContactFamily family = static_cast<QContactFamily>(detail);

    if (family.spouse().size()) {
        QVersitProperty property;
        property.setName(QLatin1String("X-SPOUSE"));
        property.setValue(family.spouse());
        *generatedProperties << property;
        *processedFields << QContactFamily::FieldSpouse;
    }

    if (family.children().size()) {
        QVersitProperty property;
        property.setName(QLatin1String("X-CHILDREN"));
        property.setValue(family.children());
        property.setValueType(QVersitProperty::ListType);
        *generatedProperties << property;
        *processedFields << QContactFamily::FieldChildren;
    }
}


/*!
 * Encode family versit property if its supported in Versit Document
 */
void QVersitContactExporterPrivate::encodeDisplayLabel(
    const QContactDetail &detail,
    const QVersitDocument& document,
    QList<QVersitProperty>* removedProperties,
    QList<QVersitProperty>* generatedProperties,
    QSet<QString>* processedFields)
{
    // Override any previous FN property
    QVersitProperty property =
        VersitUtils::takeProperty(document, QLatin1String("FN"), removedProperties);
    property.setName(mPropertyMappings.value(detail.definitionName()));
    QContactDisplayLabel displayLabel = static_cast<QContactDisplayLabel>(detail);
    if (!displayLabel.label().isEmpty()) {
        property.setValue(displayLabel.label());
        *generatedProperties << property;
        *processedFields << QContactDisplayLabel::FieldLabel;
    }
}

/*!
 * Check if \a resourceIdentifier represents a valid remote resource
 */
bool QVersitContactExporterPrivate::isValidRemoteUrl(
    const QString& resourceIdentifier)
{
    QUrl remoteResource(resourceIdentifier);
    if ((!remoteResource.scheme().isEmpty() &&
         !remoteResource.host().isEmpty()) ||
        resourceIdentifier.contains(QLatin1String("www."), Qt::CaseInsensitive))
        return true;
    return false;
}

/*!
 * Encode parameters to \a property
 */
void QVersitContactExporterPrivate::encodeParameters(
    QVersitProperty& property,
    const QStringList& contexts,
    const QStringList& subTypes)
{
    QStringList parameterList(contexts); // Contexts should be encoded first
    parameterList << subTypes;
    while (!parameterList.isEmpty()) {
        QString value = parameterList.takeLast();
        QString mappedValue = mParameterMappings.value(value);
        if (mappedValue.length() > 0) {
            // QVersitProperty::addParameter inserts into beginning.
            // This is why the last value is taken from the list
            property.insertParameter(QLatin1String("TYPE"),mappedValue);
        }
    }
}

/*!
 * Encode embedded content from the given \a resourcePath into \a property.
 */
bool QVersitContactExporterPrivate::encodeContentFromFile(const QString& resourcePath,
                                                          QVersitProperty& property)
{
    bool encodeContent = false;
    QVariant value;
    QByteArray imageData;
    QString mimeType;
    if (isValidRemoteUrl( resourcePath )) {
        encodeContent = true;
        value.setValue(resourcePath);
        property.insertParameter(QLatin1String("VALUE"), QLatin1String("URL"));
    } else if (mResourceHandler
               && mResourceHandler->loadResource(resourcePath, &imageData, &mimeType)) {
        value.setValue(imageData);
        if (!mimeType.isEmpty()) {
            // If mimeType is (eg.) "image/jpeg", set type parameter to "JPEG"
            int slashIndex = mimeType.indexOf(QLatin1Char('/'));
            if (slashIndex >= 0)
                property.insertParameter(QLatin1String("TYPE"),
                                         mimeType.remove(0, slashIndex+1).toUpper());
        }
        encodeContent = true;
    } else {
        // The file doesn't exist. Don't encode the path to a local file.
    }
    property.setValue(value);
    return encodeContent;
}
