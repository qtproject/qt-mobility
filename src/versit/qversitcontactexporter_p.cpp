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

#include "qversitcontactexporter.h"
#include "qversitcontactexporter_p.h"
#include "qversitdefs_p.h"
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
    mDefaultResourceHandler(new QVersitDefaultResourceHandler),
    mVersitType(QVersitDocument::InvalidType)
{
    mResourceHandler = mDefaultResourceHandler;

    // Detail mappings
    int versitPropertyCount =
        sizeof(versitContactDetailMappings)/sizeof(VersitContactDetailMapping);
    for (int i=0; i < versitPropertyCount; i++) {
        mPropertyMappings.insert(
                QLatin1String(versitContactDetailMappings[i].contactDetailDefinitionName),
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
    mVersitType = document.type();
    QList<QContactDetail> allDetails = contact.details();
    if (allDetails.isEmpty()) {
        *error = QVersitContactExporter::EmptyContactError;
        return false;
    }
    for (int i = 0; i < allDetails.size(); i++) {
        QContactDetail detail = allDetails.at(i);

        // If the custom detail handler handles it, we don't have to.
        if (mDetailHandler
            && mDetailHandler->preProcessDetail(contact, detail, &document))
            continue;

        QVersitProperty property;
        property.setName(mPropertyMappings.value(detail.definitionName()));
        bool addProperty = true;
        bool unknown = false;

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
        } else if (detail.definitionName() == QContactGeoLocation::DefinitionName) {
            encodeGeoLocation(property, detail);
        } else if (detail.definitionName() == QContactNote::DefinitionName) {
            encodeNote(property, detail);
        } else if (detail.definitionName() == QContactOrganization::DefinitionName) {
            encodeOrganization(document, detail);
            addProperty = false;
        } else if (detail.definitionName() == QContactRingtone::DefinitionName) {
            addProperty = encodeRingtone(property, detail);
        } else if (detail.definitionName() == QContactThumbnail::DefinitionName) {
            addProperty = encodeThumbnail(property, detail);
            if (!addProperty)
                unknown = true;
        } else if (detail.definitionName() == QContactAvatar::DefinitionName){
            addProperty = encodeAvatar(property, detail);
            if (!addProperty)
                unknown = true;
        } else if (detail.definitionName() == QContactAnniversary::DefinitionName) {
            encodeAnniversary(property, detail);
        } else if (detail.definitionName() == QContactNickname::DefinitionName) {
            encodeNickname(document, detail);
            addProperty = false;
        } else if (detail.definitionName() == QContactTag::DefinitionName) {
            encodeTag(document, detail);
            addProperty = false;
        } else if (detail.definitionName() == QContactGender::DefinitionName) {
            encodeGender(property, detail);
        } else if (detail.definitionName() == QContactOnlineAccount::DefinitionName) {
            addProperty = encodeOnlineAccount(property, detail);
            if (!addProperty)
                unknown = true;
        } else if (detail.definitionName() == QContactFamily::DefinitionName) {
            addProperty = encodeFamily(document, detail);
        } else if (detail.definitionName() == QContactDisplayLabel::DefinitionName) {
            addProperty = encodeDisplayLabel(property, detail, contact);
            if (!addProperty)
                unknown = true;
        } else {
            addProperty = false;
            unknown = true;
        }

        if (addProperty) {
            document.addProperty(property);
        }

        if (mDetailHandler)
            mDetailHandler->postProcessDetail(contact, detail, !unknown, &document);
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
    QVersitProperty& property,
    const QContactDetail& detail)
{
    QContactName contactName = static_cast<QContactName>(detail);
    property.setValue(QStringList()
                      << contactName.lastName()
                      << contactName.firstName()
                      << contactName.middleName()
                      << contactName.prefix()
                      << contactName.suffix());
    property.setValueType(QVersitProperty::CompoundType);
}

/*!
 * Encode Phone Number Field Information into the Versit Document
 */
void QVersitContactExporterPrivate::encodePhoneNumber(
    QVersitProperty& property,
    const QContactDetail& detail)
{
    QContactPhoneNumber phoneNumber = static_cast<QContactPhoneNumber>(detail);
    QStringList subTypes = phoneNumber.subTypes();
    encodeParameters(property, phoneNumber.contexts(), subTypes);
    if (subTypes.contains(QContactPhoneNumber::SubTypeAssistant))
        property.setName(QLatin1String("X-ASSISTANT-TEL"));
    else
        property.setName(QLatin1String("TEL"));
    property.setValue(phoneNumber.number());
}

/*!
 * Encode Email Field Information into the Versit Document
 */
void QVersitContactExporterPrivate::encodeEmail(
    QVersitProperty& property,
    const QContactDetail& detail)
{
    QContactEmailAddress emailAddress = static_cast<QContactEmailAddress>(detail);
    encodeParameters(property, emailAddress.contexts());
    property.setValue(emailAddress.emailAddress());
}

/*!
 * Encode Address Field Information into the Versit Document
 */
void QVersitContactExporterPrivate::encodeAddress(
    QVersitProperty& property,
    const QContactDetail& detail)
{
    QContactAddress address = static_cast<QContactAddress>(detail);
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
}

/*!
 * Encode URL Field Information into the Versit Document
 */
void QVersitContactExporterPrivate::encodeUrl(
    QVersitProperty& property,
    const QContactDetail& detail)
{
    QContactUrl contactUrl = static_cast<QContactUrl>(detail);
    encodeParameters(property, contactUrl.contexts());
    // The vCard specifications do not define any TYPEs for URL property.
    // No need to try to convert the subtypes to TYPEs.
    property.setValue(contactUrl.url());
}

/*!
 * Encode Uid Field Information into the Versit Document
 */
void QVersitContactExporterPrivate::encodeUid(
    QVersitProperty& property,
    const QContactDetail& detail)
{
    QContactGuid uid = static_cast<QContactGuid>(detail);
    property.setValue(uid.guid());
}

/*!
 * Encode REV Field Information into the Versit Document
 */
bool QVersitContactExporterPrivate::encodeRev(
    QVersitProperty& property,
    const QContactDetail& detail)
{
    QContactTimestamp  rev = static_cast<QContactTimestamp>(detail);
    QString value;
    bool encoded = false;

    if ( rev.lastModified().toString(Qt::ISODate).size() ) {
        encoded = true;
        if ( rev.lastModified().timeSpec() == Qt::UTC ) {
            value = rev.lastModified().toString(Qt::ISODate) + QLatin1Char('Z');
        }
        else {
            value = rev.lastModified().toString(Qt::ISODate);
        }
    }

    else if ( rev.created().toString(Qt::ISODate).size()) {
        encoded = true;
        if ( rev.created().timeSpec() == Qt::UTC ) {
            value = rev.created().toString(Qt::ISODate) + QLatin1Char('Z');
        }
        else {
            value = rev.created().toString(Qt::ISODate);
        }
    }
    property.setValue(value);

    return encoded;
}

/*!
 * Encode BirthDay Field Information into the Versit Document
 */
void QVersitContactExporterPrivate::encodeBirthDay(
    QVersitProperty& property,
    const QContactDetail& detail)
{
    QContactBirthday bday = static_cast<QContactBirthday>(detail);
    QString value = bday.date().toString(Qt::ISODate);
    property.setValue(value);
}

/*!
 * Encode Comment i.e. Note Field Information into the Versit Document
 */
void QVersitContactExporterPrivate::encodeNote(
    QVersitProperty& property,
    const QContactDetail& detail)
{
    QContactNote contactNote = static_cast<QContactNote>(detail);
    property.setValue(contactNote.note());
}

/*!
 * Encode Geo Prpoperties Field Information into the Versit Document
 */
void QVersitContactExporterPrivate::encodeGeoLocation(
    QVersitProperty& property,
    const QContactDetail& detail)
{
    QContactGeoLocation geoLocation = static_cast<QContactGeoLocation>(detail);
    property.setValue(QStringList() << QString::number(geoLocation.longitude())
                      << QString::number(geoLocation.latitude()));
    property.setValueType(QVersitProperty::CompoundType);
}

/*!
 * Encode organization properties to the versit document
 */
void QVersitContactExporterPrivate::encodeOrganization(
     QVersitDocument& document,
     const QContactDetail& detail)
{
    QContactOrganization organization = static_cast<QContactOrganization>(detail);
    if (organization.title().length() > 0) {
        QVersitProperty property;
        property.setName(QLatin1String("TITLE"));
        property.setValue(organization.title());
        document.addProperty(property);
    }
    if (organization.name().length() > 0 || organization.department().size() > 0) {
        QVersitProperty property;
        property.setName(QLatin1String("ORG"));
        QStringList values(organization.name());
        values.append(organization.department());
        property.setValue(values);
        property.setValueType(QVersitProperty::CompoundType);
        document.addProperty(property);
    }
    if (organization.logoUrl().isValid()) {
        QVersitProperty property;
        // XXX TODO: FIXME!
        if (encodeContentFromFile(organization.logoUrl().toString(), property)) {
            property.setName(QLatin1String("LOGO"));
            document.addProperty(property);
        }
    }
    if (organization.assistantName().length() > 0) {
        QVersitProperty property;
        property.setName(QLatin1String("X-ASSISTANT"));
        property.setValue(organization.assistantName());
        document.addProperty(property);
    }

    if (organization.role().length() > 0) {
        QVersitProperty property;
        property.setName(QLatin1String("ROLE"));
        property.setValue(organization.role());
        document.addProperty(property);
    }
}

bool QVersitContactExporterPrivate::encodeRingtone(QVersitProperty &property, const QContactDetail &detail)
{
    QContactRingtone ringtone = static_cast<QContactRingtone>(detail);
    Q_ASSERT(property.name() == QLatin1String("SOUND"));
    return encodeContentFromFile(ringtone.audioRingtoneUrl().toLocalFile(), property);
}

/*!
 * Encode thumbnail content into the Versit Document
 */
bool QVersitContactExporterPrivate::encodeThumbnail(
    QVersitProperty& property,
    const QContactDetail& detail)
{
    QContactThumbnail contactThumbnail = static_cast<QContactThumbnail>(detail);
    property.setName(QLatin1String("PHOTO"));
    QImage image = contactThumbnail.thumbnail();
    if (image.isNull())
        return false;
    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);
    // Always store a pixmap as a PNG.
    if (!image.save(&buffer, "PNG")) {
        return false;
    }
    property.setValue(imageData);
    property.insertParameter(QLatin1String("TYPE"), QLatin1String("PNG"));
    return true;
}

/*!
 * Encode avatar URIs into the Versit Document
 */
bool QVersitContactExporterPrivate::encodeAvatar(
    QVersitProperty& property,
    const QContactDetail& detail)
{
    property.setName(QLatin1String("PHOTO"));
    QContactAvatar contactAvatar = static_cast<QContactAvatar>(detail);
    QUrl imageUrl(contactAvatar.imageUrl());
    // XXX: fix up this mess: checking the scheme here and in encodeContentFromFile,
    // organisation logo and ringtone are QStrings but avatar is a QUrl
    if (!imageUrl.scheme().isEmpty() && !imageUrl.host().isEmpty()) {
        property.insertParameter(QLatin1String("VALUE"), QLatin1String("URL"));
        property.setValue(imageUrl.toString());
        return true;
    } else {
        return encodeContentFromFile(contactAvatar.imageUrl().toString(), property);
    }
}

/*!
 * Encode gender property information into Versit Document
 */
void QVersitContactExporterPrivate::encodeGender(
    QVersitProperty& property,const
    QContactDetail& detail)
{
    QContactGender gender = static_cast<QContactGender>(detail);
    property.setValue(gender.gender());
}

/*!
 * Encodes nickname property information into the Versit Document
 */
void QVersitContactExporterPrivate::encodeNickname(
    QVersitDocument& document,
    const QContactDetail& detail)
{
    QContactNickname nicknameDetail = static_cast<QContactNickname>(detail);
    QVersitProperty property;
    bool found = false;
    // XXX TODO: ensure it works for both X-NICKNAME and NICKNAME
    foreach (const QVersitProperty& currentProperty, document.properties()) {
        if (currentProperty.name() == QLatin1String("X-NICKNAME")) {
            property = currentProperty;
            found = true;
            break;
        }
    }
    QStringList value(property.variantValue().toStringList());
    if (!found)
        property.setName(QLatin1String("X-NICKNAME"));
    value.append(nicknameDetail.nickname());
    property.setValue(value);
    property.setValueType(QVersitProperty::ListType);
    // Replace the current property
    document.removeProperties(QLatin1String("X-NICKNAME"));
    document.addProperty(property);
}

/*!
 * Encodes a contact tag into the Versit Document
 */
void QVersitContactExporterPrivate::encodeTag(
    QVersitDocument& document,
    const QContactDetail& detail)
{
    QContactTag tagDetail = static_cast<QContactTag>(detail);
    QVersitProperty property;
    bool found = false;
    foreach (const QVersitProperty& currentProperty, document.properties()) {
        if (currentProperty.name() == QLatin1String("CATEGORIES")) {
            property = currentProperty;
            found = true;
            break;
        }
    }
    QStringList value(property.variantValue().toStringList());
    if (!found)
        property.setName(QLatin1String("CATEGORIES"));
    value.append(tagDetail.tag());
    property.setValue(value);
    property.setValueType(QVersitProperty::ListType);
    // Replace the current property
    document.removeProperties(QLatin1String("CATEGORIES"));
    document.addProperty(property);
}

/*!
 * Encode anniversary information into Versit Document
 */
void QVersitContactExporterPrivate::encodeAnniversary(
    QVersitProperty& property,
    const QContactDetail& detail)
{
    QContactAnniversary anniversary = static_cast<QContactAnniversary>(detail);
    property.setValue(anniversary.originalDate().toString(Qt::ISODate));
}

/*!
 * Encode online account information into the Versit Document
 */
bool QVersitContactExporterPrivate::encodeOnlineAccount(
    QVersitProperty& property,
    const QContactDetail& detail)
{
    bool encoded = false;
    QContactOnlineAccount onlineAccount = static_cast<QContactOnlineAccount>(detail);
    QStringList subTypes = onlineAccount.subTypes();

    if (subTypes.contains(QContactOnlineAccount::SubTypeSip) ||
        subTypes.contains(QContactOnlineAccount::SubTypeSipVoip) ||
        subTypes.contains(QContactOnlineAccount::SubTypeVideoShare) ||
        subTypes.contains(QContactOnlineAccount::SubTypeImpp)) {
        encoded = true;
        encodeParameters(property, onlineAccount.contexts(), subTypes);
        QString name(QLatin1String("X-SIP"));
        if (subTypes.contains(QContactOnlineAccount::SubTypeImpp))
            name = QLatin1String("X-IMPP");
        property.setName(name);
        property.setValue(onlineAccount.accountUri());
    }
    return encoded;
}

/*!
 * Encode family versit property if its supported in Versit Document
 */
bool QVersitContactExporterPrivate::encodeFamily(
    QVersitDocument& document,
    const QContactDetail& detail )
{
    QContactFamily family = static_cast<QContactFamily>(detail);

    if (family.spouse().size()) {
        QVersitProperty property;
        property.setName(QLatin1String("X-SPOUSE"));
        property.setValue(family.spouse());
        document.addProperty(property);
    }

    if (family.children().size()) {
        QVersitProperty property;
        property.setName(QLatin1String("X-CHILDREN"));
        property.setValue(family.children());
        property.setValueType(QVersitProperty::ListType);
        document.addProperty(property);
    }
    return false;
}


/*!
 * Encode family versit property if its supported in Versit Document
 */
bool QVersitContactExporterPrivate::encodeDisplayLabel(
    QVersitProperty& property,
    const QContactDetail& detail,
    const QContact& contact)
{
    bool encoded = false;
    QContactDisplayLabel displayLabel = static_cast<QContactDisplayLabel>(detail);
    if (displayLabel.label().size()) {
        encoded = true;
        property.setValue(displayLabel.label());
    } else {
        QContactDetail contactDetail;
        for (int i = 0; i < contact.details().count(); i++) {
            contactDetail = contact.details().at(i);
            if (contactDetail.definitionName() == QContactName::DefinitionName)
                break;
        }
        QContactName name = static_cast<QContactName>(contactDetail);
        QString value;
        if (name.customLabel().length()) {
            value = name.customLabel();
        } else {
            value = name.firstName() + QLatin1Char(' ') + name.lastName();
        }
        if (name.customLabel().length() ||
            name.firstName().length() ||
            name.lastName().length()) {
            encoded = true;
            property.setValue(value);
        }
    }
    return encoded;
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
