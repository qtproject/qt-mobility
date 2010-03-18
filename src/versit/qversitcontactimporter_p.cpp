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

#include "qversitdefs_p.h"
#include "qversitcontactimporter_p.h"
#include "qversitdocument.h"
#include "qversitproperty.h"
#include "qmobilityglobal.h"

#include <qcontact.h>
#include <qcontactdetail.h>
#include <qcontactname.h>
#include <qcontactphonenumber.h>
#include <qcontactaddress.h>
#include <qcontactemailaddress.h>
#include <qcontactorganization.h>
#include <qcontacturl.h>
#include <qcontactguid.h>
#include <qcontacttimestamp.h>
#include <qcontactanniversary.h>
#include <qcontactbirthday.h>
#include <qcontactgender.h>
#include <qcontactnickname.h>
#include <qcontactavatar.h>
#include <qcontactgeolocation.h>
#include <qcontactnote.h>
#include <qcontactonlineaccount.h>
#include <qcontactfamily.h>
#include <qcontactdisplaylabel.h>

#include <QHash>
#include <QFile>

QTM_USE_NAMESPACE

/*!
 * Constructor.
 */
QVersitContactImporterPrivate::QVersitContactImporterPrivate() :
    mPropertyHandler(NULL),
    mDefaultResourceHandler(new QVersitDefaultResourceHandler)
{
    mResourceHandler = mDefaultResourceHandler;

    // Contact detail mappings
    int versitPropertyCount =
        sizeof(versitContactDetailMappings)/sizeof(VersitContactDetailMapping);
    for (int i=0; i < versitPropertyCount; i++) {
        QString versitPropertyName =
            QLatin1String(versitContactDetailMappings[i].versitPropertyName);
        QPair<QString,QString> contactDetail;
        contactDetail.first =
            QLatin1String(versitContactDetailMappings[i].contactDetailDefinitionName);
        contactDetail.second =
            QLatin1String(versitContactDetailMappings[i].contactDetailValueKey);
        mDetailMappings.insert(versitPropertyName,contactDetail);
    }

    // Context mappings
    int contextCount = sizeof(versitContextMappings)/sizeof(VersitMapping);
    for (int i=0; i < contextCount; i++) {
        mContextMappings.insert(
            QLatin1String(versitContextMappings[i].versitString),
            QLatin1String(versitContextMappings[i].contactString));
    }

    // Subtype mappings
    int subTypeCount = sizeof(versitSubTypeMappings)/sizeof(VersitMapping);
    for (int i=0; i < subTypeCount; i++) {
        mSubTypeMappings.insert(
            QLatin1String(versitSubTypeMappings[i].versitString),
            QLatin1String(versitSubTypeMappings[i].contactString));
    }
}

/*!
 * Destructor.
 */
QVersitContactImporterPrivate::~QVersitContactImporterPrivate()
{
    delete mDefaultResourceHandler;
}

/*!
 * Generates a QContact from \a versitDocument.
 */
bool QVersitContactImporterPrivate::importContact(
        const QVersitDocument& document, int contactIndex, QContact* contact,
        QVersitContactImporter::Error* error)
{
    if (document.type() != QVersitDocument::VCard21Type
        && document.type() != QVersitDocument::VCard30Type) {
        *error = QVersitContactImporter::InvalidDocumentError;
        return false;
    }
    const QList<QVersitProperty> properties = document.properties();
    if (properties.size() == 0) {
        *error = QVersitContactImporter::EmptyDocumentError;
        return false;
    }

    // First, do the properties with PREF set so they appear first in the contact details
    foreach (const QVersitProperty& property, properties) {
        if (property.parameters().contains(QLatin1String("TYPE"), QLatin1String("PREF")))
            importProperty(document, property, contactIndex, contact);
    }
    // ... then, do the rest of the properties.
    foreach (const QVersitProperty& property, properties) {
        if (!property.parameters().contains(QLatin1String("TYPE"), QLatin1String("PREF")))
            importProperty(document, property, contactIndex, contact);
    }

    contact->setType(QContactType::TypeContact);
    return true;
}

void QVersitContactImporterPrivate::importProperty(
        const QVersitDocument& document, const QVersitProperty& property, int contactIndex,
        QContact* contact) const
{
    if (mPropertyHandler
        && mPropertyHandler->preProcessProperty(document, property, contactIndex, contact))
        return;

    QPair<QString,QString> detailDefinition =
        mDetailMappings.value(property.name());
    QString detailDefinitionName = detailDefinition.first;
    bool success = false;
    if (detailDefinitionName == QContactAddress::DefinitionName) {
        success = createAddress(property, contact);
    } else if (detailDefinitionName == QContactName::DefinitionName) {
        success = createName(property, contact);
    } else if (detailDefinitionName == QContactBirthday::DefinitionName) {
        success = createBirthday(property, contact);
    } else if (detailDefinitionName == QContactGeoLocation::DefinitionName){
        success = createGeoLocation(property, contact);
    } else if (detailDefinitionName == QContactOrganization::DefinitionName) {
        success = createOrganization(property, contact);
    } else if (detailDefinitionName == QContactNickname::DefinitionName) {
        success = createNicknames(property, contact);
    } else if (detailDefinitionName == QContactAvatar::DefinitionName) {
        success = createAvatar(property,detailDefinition.second, contact);
    } else if (detailDefinitionName == QContactTimestamp::DefinitionName) {
        success = createTimeStamp(property, contact);
    } else if (detailDefinitionName == QContactPhoneNumber::DefinitionName) {
        success = createPhone(property, contact);
    } else if (detailDefinitionName == QContactAnniversary::DefinitionName) {
        success = createAnniversary(property, contact);
    } else if (detailDefinitionName == QContactFamily::DefinitionName) {
        success = createFamily(property, contact);
    } else if (detailDefinitionName == QContactOnlineAccount::DefinitionName) {
        success = createOnlineAccount(property, contact);
    } else if (detailDefinitionName == QContactTag::DefinitionName) {
        success = createTags(property, contact);
    } else if (detailDefinitionName == QContactDisplayLabel::DefinitionName) {
        // This actually sets the QContactName's customLabel field (not QContactDisplayLabel)
        success = createLabel(property, contact);
    } else {
        // Look up mDetailMappings for a simple mapping from property to detail.
        success = createNameValueDetail(property, contact);
    }

    if (mPropertyHandler)
        mPropertyHandler->postProcessProperty(document, property, success, contactIndex, contact);
}
/*!
 * Creates a QContactName from \a property
 */
bool QVersitContactImporterPrivate::createName(
    const QVersitProperty& property, QContact* contact) const
{
    QContactName name;
    QContactDetail detail = contact->detail(QContactName::DefinitionName);
    if (!detail.isEmpty()) {
        // If multiple name properties exist,
        // discard all except the first occurrence
        if (!detail.value(QContactName::FieldFirstName).isEmpty())
            return false;
        else
            name = QContactName(static_cast<QContactName>(detail));
    }

    QStringList values = property.value().split(QLatin1Char(';'));
    name.setLastName(takeFirst(values));
    name.setFirstName(takeFirst(values));
    name.setMiddleName(takeFirst(values));
    name.setPrefix(takeFirst(values));
    name.setSuffix(takeFirst(values));

    saveDetailWithContext(contact, &name, extractContexts(property));
    return true;
}

/*!
 * Creates a QContactPhoneNumber from \a property
 */
bool QVersitContactImporterPrivate::createPhone(
    const QVersitProperty& property, QContact* contact) const
{
    QContactPhoneNumber phone;
    phone.setNumber(property.value());
    phone.setSubTypes(extractSubTypes(property));

    saveDetailWithContext(contact, &phone, extractContexts(property));
    return true;
}

/*!
 * Creates a QContactAddress from \a property
 */
bool QVersitContactImporterPrivate::createAddress(
    const QVersitProperty& property, QContact* contact) const
{
    QContactAddress address;

    QStringList addressParts = property.value().split(QLatin1Char(';'));
    address.setPostOfficeBox(takeFirst(addressParts));
    // There is no setter for the Extended Address in QContactAddress:
    if (!addressParts.isEmpty())
        addressParts.removeFirst();
    address.setStreet(takeFirst(addressParts));
    address.setLocality(takeFirst(addressParts));
    address.setRegion(takeFirst(addressParts));
    address.setPostcode(takeFirst(addressParts));
    address.setCountry(takeFirst(addressParts));
    address.setSubTypes(extractSubTypes(property));

    saveDetailWithContext(contact, &address, extractContexts(property));
    return true;
}

/*!
 * Creates a QContactOrganization from \a property
 */
bool QVersitContactImporterPrivate::createOrganization(
    const QVersitProperty& property, QContact* contact) const
{
    QContactOrganization organization;
    QPair<QString,QString> detailNameAndFieldName =
        mDetailMappings.value(property.name());
    QString fieldName = detailNameAndFieldName.second;
    QList<QContactOrganization> organizations = contact->details<QContactOrganization>();
    foreach(const QContactOrganization& current, organizations) {
        if (current.value(fieldName).length() == 0) {
            organization = current;
            break;
        }
    }
    if (fieldName == QContactOrganization::FieldName) {
        setOrganizationNames(organization, property);
    } else if (fieldName == QContactOrganization::FieldTitle) {
        organization.setTitle(property.value());
    } else if (fieldName == QContactOrganization::FieldRole) {
        organization.setRole(property.value());
    } else if (fieldName == QContactOrganization::FieldLogo) {
        setOrganizationLogo(organization, property);
    } else if (fieldName == QContactOrganization::FieldAssistantName) {
        organization.setAssistantName(property.value());
    } else {
        return false;
    }

    saveDetailWithContext(contact, &organization, extractContexts(property));
    return true;
}

/*!
 * Set the organization name and department(s) from \a property.
 */
void QVersitContactImporterPrivate::setOrganizationNames(
    QContactOrganization& organization, const QVersitProperty& property) const
{
    QString value = property.value();
    int firstSemicolon = value.indexOf(QLatin1Char(';'));
    if (firstSemicolon >= 0) {
        organization.setName(value.left(firstSemicolon));
        QString departmentsStr(value.mid(firstSemicolon+1));
        QStringList departments =
            departmentsStr.split(QLatin1Char(';'), QString::SkipEmptyParts);
        organization.setDepartment(departments);
    } else {
        organization.setName(value);
    }
}

/*!
 * Set the organization logo from \a property.
 */
void QVersitContactImporterPrivate::setOrganizationLogo(
    QContactOrganization& org, const QVersitProperty& property) const
{
    QString location;
    QByteArray data;
    saveDataFromProperty(property, &location, &data);
    if (!location.isEmpty())
        org.setLogo(location);
}

/*!
 * Creates a QContactTimeStamp from \a property
 */
bool QVersitContactImporterPrivate::createTimeStamp(
    const QVersitProperty& property, QContact* contact) const
{
    QContactTimestamp timeStamp;
    QString value(property.value());
    bool utc = value.endsWith(QLatin1Char('Z'), Qt::CaseInsensitive);
    if (utc)
        value.chop(1); // take away z from end;

    QDateTime dateTime = parseDateTime(value,QLatin1String("yyyyMMddThhmmss"));
    if (utc)
        dateTime.setTimeSpec(Qt::UTC);
    timeStamp.setLastModified(dateTime);

    saveDetailWithContext(contact, &timeStamp, extractContexts(property));
    return true;
}

/*!
 * Creates a QContactAnniversary from \a property
 */
bool QVersitContactImporterPrivate::createAnniversary(
    const QVersitProperty& property, QContact* contact) const
{
    QContactAnniversary anniversary;
    QDateTime dateTime =
        parseDateTime(property.value(), QLatin1String("yyyyMMdd"));
    anniversary.setOriginalDate(dateTime.date());

    saveDetailWithContext(contact, &anniversary, extractContexts(property));
    return true;
}

/*!
 * Creates a QContactBirthday from \a property
 */
bool QVersitContactImporterPrivate::createBirthday(
    const QVersitProperty& property, QContact* contact) const
{
    QContactBirthday bday;
    QDateTime dateTime =
        parseDateTime(property.value(), QLatin1String("yyyyMMdd"));
    bday.setDate(dateTime.date());

    saveDetailWithContext(contact, &bday, extractContexts(property));
    return true;
}

/*!
 * Creates QContactNicknames from \a property and adds them to \a contact
 */
bool QVersitContactImporterPrivate::createNicknames(
    const QVersitProperty& property, QContact* contact) const
{
    QStringList values = property.value().split(QLatin1Char(','), QString::SkipEmptyParts);
    QStringList contexts = extractContexts(property);
    foreach(const QString& value, values) {
        QContactNickname nickName;
        nickName.setNickname(value);
        saveDetailWithContext(contact, &nickName, contexts);
    }
    return true;
}

/*!
 * Creates QContactTags from \a property and adds them to \a contact
 */
bool QVersitContactImporterPrivate::createTags(
    const QVersitProperty& property, QContact* contact) const
{
    QStringList values = property.value().split(QLatin1Char(','), QString::SkipEmptyParts);
    QStringList contexts = extractContexts(property);
    foreach(const QString& value, values) {
        QContactTag tag;
        tag.setTag(value);
        saveDetailWithContext(contact, &tag, contexts);
    }
    return true;
}

/*!
 * Creates a QContactOnlineAccount from \a property
 */
bool QVersitContactImporterPrivate::createOnlineAccount(
    const QVersitProperty& property, QContact* contact) const
{
    QContactOnlineAccount onlineAccount;
    onlineAccount.setAccountUri(property.value());
    if (property.name() == QLatin1String("X-SIP")) {
        QStringList subTypes = extractSubTypes(property);
        if (subTypes.count() == 0)
            subTypes.append(QContactOnlineAccount::SubTypeSip);
        onlineAccount.setSubTypes(subTypes);
    }
    else if (property.name() == QLatin1String("X-IMPP") ||
             property.name() == QLatin1String("IMPP") ||
             property.name() == QLatin1String("X-JABBER")) {
        onlineAccount.setSubTypes(QContactOnlineAccount::SubTypeImpp);
    }
    else {
        // NOP
    }

    saveDetailWithContext(contact, &onlineAccount, extractContexts(property));
    return true;
}

/*!
 * Creates a QContactAvatar from \a property
 */
bool QVersitContactImporterPrivate::createAvatar(
    const QVersitProperty& property, const QString& subType, QContact* contact) const
{
    QString location;
    QByteArray data;
    if (!(saveDataFromProperty(property, &location, &data)))
        return false;

    QContactAvatar avatar;
    if (!location.isEmpty())
        avatar.setAvatar(location);
    // Creating a pixmap in a non-GUI thread crashes on S60.
    // XXX reenable this when the QtContacts stores QImages.
//    if (subType == QContactAvatar::SubTypeImage && !data.isEmpty()) {
//        QPixmap pixmap;
//        if (pixmap.loadFromData(data))
//            avatar.setPixmap(pixmap);
//    }
    avatar.setSubType(subType);

    saveDetailWithContext(contact, &avatar, extractContexts(property));
    return true;
}

/*!
 * Creates a QContactGeoLocation from \a property
 */
bool QVersitContactImporterPrivate::createGeoLocation(
    const QVersitProperty& property, QContact* contact) const
{
    QContactGeoLocation geo;
    QStringList values = property.value().split(QLatin1Char(','));
    geo.setLongitude(takeFirst(values).toDouble());
    geo.setLatitude(takeFirst(values).toDouble());

    saveDetailWithContext(contact, &geo, extractContexts(property));
    return true;
}

/*!
 * Creates a QContactFamily from \a property
 */
bool QVersitContactImporterPrivate::createFamily(
    const QVersitProperty& property, QContact* contact) const
{
    QString val = property.value();
    QContactFamily family = contact->detail<QContactFamily>();
    if (property.name() == QLatin1String("X-SPOUSE")) {
        family.setSpouse(val);
    } else if (property.name() == QLatin1String("X-CHILDREN")) {
        family.setChildren(val.split(QLatin1String(",")));
    }

    saveDetailWithContext(contact, &family, extractContexts(property));
    return true;
}

/*!
 * Creates a simple name-value contact detail.
 */
bool QVersitContactImporterPrivate::createNameValueDetail(
    const QVersitProperty& property, QContact* contact) const
{
    QPair<QString,QString> nameAndValueType =
        mDetailMappings.value(property.name());
    if (nameAndValueType.first.isEmpty()) {
        return false;
    }
    QContactDetail detail(nameAndValueType.first);
    detail.setValue(nameAndValueType.second, property.value());

    saveDetailWithContext(contact, &detail, extractContexts(property));
    return true;
}

/*!
 * Creates a simple name-value contact detail.
 */
bool QVersitContactImporterPrivate::createLabel(
    const QVersitProperty& property, QContact* contact) const
{
    QContactName name;
    QContactName existingName = contact->detail<QContactName>();
    if (!existingName.isEmpty()) {
        name = existingName;
    }

    name.setCustomLabel(property.value());

    saveDetailWithContext(contact, &name, extractContexts(property));
    return true;
}

/*!
 * Extracts the list of contexts from \a types
 */
QStringList QVersitContactImporterPrivate::extractContexts(
    const QVersitProperty& property) const
{
    QStringList types =
        property.parameters().values(QLatin1String("TYPE"));
    QStringList contexts;
    foreach (const QString& type, types) {
        QString value = mContextMappings.value(type);
        if (value.length() > 0)
            contexts.append(value);
    }
    return contexts;
}

/*!
 * Extracts the list of subtypes from \a property
 */
QStringList QVersitContactImporterPrivate::extractSubTypes(
    const QVersitProperty& property) const
{
    QStringList types =
        property.parameters().values(QLatin1String("TYPE"));
    QStringList subTypes;
    foreach (const QString& type, types) {
        QString subType = mSubTypeMappings.value(type);
        if (subType.length() > 0)
            subTypes += subType;
    }
    return subTypes;
}

/*!
 * Takes the first value in \a list, or an empty QString is if the list is empty.
 */
QString QVersitContactImporterPrivate::takeFirst(QList<QString>& list) const
{
    return list.empty() ? QString() : list.takeFirst();
}

/*!
 * Parses a date and time from text
 */
QDateTime QVersitContactImporterPrivate::parseDateTime(
    const QString& value,
    const QString& format) const
{
    QDateTime dateTime;
    if (value.contains(QLatin1Char('-'))) {
        dateTime = QDateTime::fromString(value,Qt::ISODate);
    } else {
        dateTime = QDateTime::fromString(value, format);
    }
    return dateTime;
}

/*!
 * Extracts either a location (URI/filepath) from a \a property, or data (eg. if it was base64
 * encoded).  If the property contains data, an attempt is made to save it and the location of the
 * saved resource is recovered to *\a location.  The data is stored into *\a data.
 */
bool QVersitContactImporterPrivate::saveDataFromProperty(const QVersitProperty &property,
                                                            QString *location,
                                                            QByteArray *data) const
{
    bool found = false;
    const QString valueParam = property.parameters().value(QLatin1String("VALUE"));
    QVariant variant(property.variantValue());
    if (variant.type() == QVariant::String
        || valueParam == QLatin1String("URL")) {
        *location = property.value();
        found |= !location->isEmpty();
    } else if (variant.type() == QVariant::ByteArray) {
        *data = variant.toByteArray();
        if (!data->isEmpty()) {
            found = true;
            *location = saveContentToFile(property, *data);
        }
    }
    return found;
}

/*!
 * Writes \a data to a file and returns the filename.  \a property specifies the context in which
 * the data was found.
 */
QString QVersitContactImporterPrivate::saveContentToFile(
    const QVersitProperty& property, const QByteArray& data) const
{
    QString filename;
    bool ok = false;
    if (mResourceHandler)
        ok = mResourceHandler->saveResource(data, property, &filename);
    return ok ? filename : QString();
}

/*!
 * Saves \a detail to the \a contact.  Also sets the contexts to \a contexts if it is not empty.
 */
void QVersitContactImporterPrivate::saveDetailWithContext(
        QContact* contact, QContactDetail* detail, const QStringList& contexts) const
{
    if (!contexts.isEmpty())
        detail->setContexts(contexts);
    contact->saveDetail(detail);
}
