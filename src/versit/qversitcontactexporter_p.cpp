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
void QVersitContactExporterPrivate::exportContact(
    const QContact& contact,
    QVersitDocument& document)
{
    mVersitType = document.type();
    QList<QContactDetail> allDetails = contact.details();
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
        } else if (detail.definitionName() == QContactAvatar::DefinitionName){
            addProperty = encodeAvatar(property, detail);
            if (!addProperty)
                unknown = true;
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

        if (addProperty)
            document.addProperty(property);

        if (mDetailHandler)
            mDetailHandler->postProcessDetail(contact, detail, !unknown, &document);
    }
}

/*!
 * Encode Contact Name Field Information into the Versit Document
 */
void QVersitContactExporterPrivate::encodeName(
    QVersitProperty& property,
    const QContactDetail& detail)
{
    QContactName contactName = static_cast<QContactName>(detail);
    property.setValue(QString::fromAscii("%1;%2;%3;%4;%5").arg(
            escape(contactName.lastName()),
            escape(contactName.firstName()),
            escape(contactName.middleName()),
            escape(contactName.prefix()),
            escape(contactName.suffix())));
}

/*!
 * Encode Phone Number Field Information into the Versit Document
 */
void QVersitContactExporterPrivate::encodePhoneNumber(
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
void QVersitContactExporterPrivate::encodeEmail(
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
void QVersitContactExporterPrivate::encodeAddress(
    QVersitProperty& property,
    const QContactDetail& detail)
{
    QContactAddress address = static_cast<QContactAddress>(detail);
    encodeParameters(property, address.contexts(), address.subTypes());
    // Leave out the extended address field:
    property.setValue(QString::fromAscii("%1;;%2;%3;%4;%5;%6").arg(
            escape(address.postOfficeBox()),
            escape(address.street()),
            escape(address.locality()),
            escape(address.region()),
            escape(address.postcode()),
            escape(address.country())));
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
    setEscapedValue(property,uid.guid());
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
    setEscapedValue(property,contactNote.note());
}

/*!
 * Encode Geo Prpoperties Field Information into the Versit Document
 */
void QVersitContactExporterPrivate::encodeGeoLocation(
    QVersitProperty& property,
    const QContactDetail& detail)
{
    QContactGeoLocation geoLocation = static_cast<QContactGeoLocation>(detail);
    QString value =
        QString::number(geoLocation.longitude()) + QLatin1Char(',') +
        QString::number(geoLocation.latitude());
    property.setValue(value);
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
        setEscapedValue(property,organization.title());
        document.addProperty(property);
    }
    if (organization.name().length() > 0 || organization.department().size() > 0) {
        QVersitProperty property;
        property.setName(QLatin1String("ORG"));
        QString value = escape(organization.name());
        QStringList departments(organization.department());
        if (departments.count() == 0)
            value += QLatin1Char(';');
        foreach (const QString& department, departments) {
            value += QLatin1Char(';');
            value += escape(department);
        }
        property.setValue(value);
        document.addProperty(property);
    }
    if (organization.logo().length() > 0) {
        QVersitProperty property;
        if (encodeContentFromFile(organization.logo(), property)) {
            property.setName(QLatin1String("LOGO"));
            document.addProperty(property);
        }
    }
    if (organization.assistantName().length() > 0) {
        QVersitProperty property;
        property.setName(QLatin1String("X-ASSISTANT"));
        setEscapedValue(property,organization.assistantName());
        document.addProperty(property);
    }

    if (organization.role().length() > 0) {
        QVersitProperty property;
        property.setName(QLatin1String("ROLE"));
        setEscapedValue(property,organization.role());
        document.addProperty(property);
    }
}

/*!
 * Encode avatar content into the Versit Document
 */
bool QVersitContactExporterPrivate::encodeAvatar(
    QVersitProperty& property,
    const QContactDetail& detail)
{
    QContactAvatar contactAvatar = static_cast<QContactAvatar>(detail);
    bool encoded = false;
    QString propertyName;
    if (contactAvatar.subType() == QContactAvatar::SubTypeImage) {
        propertyName = QLatin1String("PHOTO");
    } else if (contactAvatar.subType() == QContactAvatar::SubTypeAudioRingtone) {
        propertyName = QLatin1String("SOUND");
    } else {
        // NOP
    }
    if (propertyName.length() > 0) {
        encoded = encodeContentFromFile(contactAvatar.avatar(), property);
        if (!encoded)
            encoded = encodeContentFromPixmap(contactAvatar.pixmap(), property);
        if (encoded)
            property.setName(propertyName);
    }
    return encoded;
}

/*!
 * Encode gender property information into Versit Document
 */
void QVersitContactExporterPrivate::encodeGender(
    QVersitProperty& property,const
    QContactDetail& detail)
{
    QContactGender gender = static_cast<QContactGender>(detail);
    setEscapedValue(property,gender.gender());
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
    property.setName(QLatin1String("X-NICKNAME"));
    bool found = false;
    foreach (const QVersitProperty& currentProperty, document.properties()) {
        if (currentProperty.name() == QLatin1String("X-NICKNAME")) {
            property = currentProperty;
            found = true;
            break;
        }
    }
    QString value(property.value());
    if (found)
        value += QLatin1Char(',');
    QString nickname = escape(nicknameDetail.nickname());
    value.append(nickname);
    property.setValue(value);
    // Replace the current property
    document.removeProperties(QLatin1String("X-NICKNAME"));
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
        setEscapedValue(property,onlineAccount.accountUri());
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
        setEscapedValue(property,family.spouse());
        document.addProperty(property);
    }

    if (family.children().size()) {
        QVersitProperty property;
        property.setName(QLatin1String("X-CHILDREN"));
        QString children = family.children().join(QLatin1String(","));
        setEscapedValue(property,children);
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
        setEscapedValue(property,displayLabel.label());
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
            property.setValue(escape(value));
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

/*!
 * Encode embedded content from the given \a pixmap into \a property.
 */
bool QVersitContactExporterPrivate::encodeContentFromPixmap(const QPixmap& pixmap,
                                                            QVersitProperty& property)
{
    if (pixmap.isNull())
        return false;
    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);
    // Always store a pixmap as a PNG.
    if (!pixmap.save(&buffer, "PNG")) {
        return false;
    }
    property.setValue(imageData);
    property.insertParameter(QLatin1String("TYPE"), QLatin1String("PNG"));
    return true;
}

/*!
 * Escapes \a value if necessary and sets it to \a property
 */
void QVersitContactExporterPrivate::setEscapedValue(
    QVersitProperty& property,
    const QString& value)
{
    QString escapedValue(escape(value));
    property.setValue(escapedValue);
}

/*!
 * Escapes \a value if necessary.
 * For vCard there is no concept of escaping the property values.
 * Starting from 3.0 the property values having certain special
 * characters should be escaped.
 */
QString QVersitContactExporterPrivate::escape(const QString& value)
{
    QString escaped(value);
    if (mVersitType != QVersitDocument::VCard21Type) {
        VersitUtils::backSlashEscape(escaped);
    }
    return escaped;
}
