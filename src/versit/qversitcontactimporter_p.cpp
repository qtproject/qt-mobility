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

#include "qversitdefs.h"
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
    mPropertyHandler(NULL)
{
    // Contact detail mappings
    int versitPropertyCount =
        sizeof(versitContactDetailMappings)/sizeof(versitContactDetailMapping);
    for (int i=0; i < versitPropertyCount; i++) {
        QString versitPropertyName =
            QString::fromAscii(versitContactDetailMappings[i].versitPropertyName);
        QPair<QString,QString> contactDetail;
        contactDetail.first =
            QString::fromAscii(versitContactDetailMappings[i].contactDetailDefinitionName);
        contactDetail.second =
            QString::fromAscii(versitContactDetailMappings[i].contactDetailValueKey);
        mDetailMappings.insert(versitPropertyName,contactDetail);
    }

    // Context mappings
    int contextCount = sizeof(versitContextMappings)/sizeof(versitMapping);
    for (int i=0; i < contextCount; i++) {
        mContextMappings.insert(
            QString::fromAscii(versitContextMappings[i].versitString),
            QString::fromAscii(versitContextMappings[i].contactString));
    }

    // Subtype mappings
    int subTypeCount = sizeof(versitSubTypeMappings)/sizeof(versitMapping);
    for (int i=0; i < subTypeCount; i++) {
        mSubTypeMappings.insert(
            QString::fromAscii(versitSubTypeMappings[i].versitString),
            QString::fromAscii(versitSubTypeMappings[i].contactString));
    }

    // File extension mappings
    int fileExtensionCount = sizeof(versitFileExtensionMappings)/sizeof(versitMapping);
    for (int i=0; i < fileExtensionCount; i++) {
        mFileExtensionMappings.insert(
            QString::fromAscii(versitFileExtensionMappings[i].versitString),
            QString::fromAscii(versitFileExtensionMappings[i].contactString));
    }
}

/*!
 * Destructor.
 */
QVersitContactImporterPrivate::~QVersitContactImporterPrivate()
{
}

/*!
 * Generates a QContact from \a versitDocument.
 */
QContact QVersitContactImporterPrivate::importContact(
     const QVersitDocument& versitDocument)
{
    QContact contact;
    const QList<QVersitProperty> properties = versitDocument.properties();
    foreach (QVersitProperty property, properties) {
        QPair<QString,QString> detailDefinition =
            mDetailMappings.value(property.name());
        QString detailDefinitionName = detailDefinition.first;
        QContactDetail* detail = 0;
        if (detailDefinitionName == QContactAddress::DefinitionName) {
            detail = createAddress(property);
        } else if (detailDefinitionName == QContactName::DefinitionName) {
            detail = createName(property,contact);
        } else if (detailDefinitionName == QContactBirthday::DefinitionName) {
            detail = createBirthday(property);
        } else if (detailDefinitionName == QContactGeolocation::DefinitionName){
            detail = createGeoLocation(property);
        } else if (detailDefinitionName == QContactOrganization::DefinitionName) {
            detail = createOrganization(property,contact);
        } else if (detailDefinitionName == QContactNickname::DefinitionName) {
            createNicknames(property,contact);
        } else if (detailDefinitionName == QContactAvatar::DefinitionName) {
            detail = createAvatar(property,detailDefinition.second);
        } else if (detailDefinitionName == QContactTimestamp::DefinitionName) {
            detail = createTimeStamp(property);
        } else if (detailDefinitionName == QContactPhoneNumber::DefinitionName) {
            detail = createPhone(property);
        } else if (detailDefinitionName == QContactAnniversary::DefinitionName) {
            detail = createAnniversary(property);
        } else if (detailDefinitionName == QContactFamily::DefinitionName) {
            detail = createFamily(property,contact);
        } else if (detailDefinitionName == QContactOnlineAccount::DefinitionName) {
            detail = createOnlineAccount(property);
        } else if (detailDefinitionName == QContactDisplayLabel::DefinitionName) {
            detail = createLabel(property, contact);
        } else {
            detail = createNameValueDetail(property);
        }

        if (detail) {
            QStringList contexts(extractContexts(property));
            if (!contexts.empty())
                detail->setContexts(contexts);
            contact.saveDetail(detail);
            delete detail;
        } else {
            if (mPropertyHandler)
                mPropertyHandler->processUnknownProperty(property, &contact);
        }
    }

    return contact;
}

/*!
 * Creates a QContactName from \a property
 */
QContactDetail* QVersitContactImporterPrivate::createName(
    const QVersitProperty& property,const QContact& contact) const
{
    QContactName* name = 0;
    QContactDetail detail = contact.detail(QContactName::DefinitionName);
    if (!detail.isEmpty()) {
        // If multiple name properties exist,
        // discard all except the first occurence
        if (!detail.value(QContactName::FieldFirst).isEmpty())
            return 0;
        else
            name = new QContactName(static_cast<QContactName>(detail));
    } else {
        name = new QContactName();
    }

    QStringList values = property.valueString().split(QChar::fromAscii(';'));
    name->setLast(takeFirst(values));
    name->setFirst(takeFirst(values));
    name->setMiddle(takeFirst(values));
    name->setPrefix(takeFirst(values));
    name->setSuffix(takeFirst(values));
    return name;
}

/*!
 * Creates a QContactPhoneNumber from \a property
 */
QContactDetail* QVersitContactImporterPrivate::createPhone(
    const QVersitProperty& property) const
{
    QContactPhoneNumber* phone = new QContactPhoneNumber();
    phone->setNumber(property.valueString());
    phone->setSubTypes(extractSubTypes(property));
    return phone;
}

/*!
 * Creates a QContactAddress from \a property
 */
QContactDetail* QVersitContactImporterPrivate::createAddress(
    const QVersitProperty& property) const
{
    QContactAddress* address = new QContactAddress();
    
    QStringList addressParts = property.valueString().split(QChar::fromAscii(';'));
    address->setPostOfficeBox(takeFirst(addressParts));
    // There is no setter for the Extended Address in QContactAddress:
    if (!addressParts.isEmpty())
        addressParts.removeFirst(); 
    address->setStreet(takeFirst(addressParts));
    address->setLocality(takeFirst(addressParts));
    address->setRegion(takeFirst(addressParts));
    address->setPostcode(takeFirst(addressParts));
    address->setCountry(takeFirst(addressParts));
    
    address->setSubTypes(extractSubTypes(property));
    
    return address;
}

/*!
 * Creates a QContactOrganization from \a property
 */
QContactDetail* QVersitContactImporterPrivate::createOrganization(
    const QVersitProperty& property,
    const QContact& contact) const
{
    QContactOrganization* organization = 0;
    QPair<QString,QString> detailNameAndFieldName =
        mDetailMappings.value(property.name());
    QString fieldName = detailNameAndFieldName.second;
    QList<QContactDetail> organizations =
        contact.details(QContactOrganization::DefinitionName);
    foreach(QContactDetail detail, organizations) {
        QContactOrganization current = static_cast<QContactOrganization>(detail);
        if (current.value(fieldName).length() == 0) {
            organization = new QContactOrganization(current);
            break;
        }
    }
    if (!organization) {
        organization = new QContactOrganization();
    }
    if (fieldName == QContactOrganization::FieldName) {
        setOrganizationNames(*organization,property);
    } else if (fieldName == QContactOrganization::FieldTitle) {
        organization->setTitle(property.valueString());
    } else if (fieldName == QContactOrganization::FieldRole) {
        organization->setRole(property.valueString());
    } else if (fieldName == QContactOrganization::FieldLogo) {
        setOrganizationLogo(*organization,property);
    } else if (fieldName == QContactOrganization::FieldAssistantName) {
        organization->setAssistantName(property.valueString());
    } else {
        delete organization;
        organization = 0;
    }
    return organization;
}

/*!
 * Set the organization name and department(s) from \a property.
 */
void QVersitContactImporterPrivate::setOrganizationNames(
    QContactOrganization& organization,
    const QVersitProperty& property) const
{
    QString value = property.valueString();
    int firstSemicolon = value.indexOf(QString::fromAscii(";"));
    if (firstSemicolon >= 0) {
        organization.setName(value.left(firstSemicolon));
        QString departmentsStr(value.mid(firstSemicolon+1));
        QStringList departments =
            departmentsStr.split(QString::fromAscii(";"),QString::SkipEmptyParts);
        organization.setDepartment(departments);
    } else {
        organization.setName(value);
    }
}

/*!
 * Set the organization logo from \a property.
 */
void QVersitContactImporterPrivate::setOrganizationLogo(
    QContactOrganization& org,
    const QVersitProperty& property) const
{
    QString value(property.valueString());

    const QString valueParam =
        property.parameters().value(QString::fromAscii("VALUE"));

    if (valueParam != QString::fromAscii("URL")) {
        value = saveContentToFile(property);
    }

    org.setLogo(value);
}

/*!
 * Creates a QContactTimeStamp from \a property
 */
QContactDetail* QVersitContactImporterPrivate::createTimeStamp(
    const QVersitProperty& property) const
{
    QContactTimestamp* timeStamp = new QContactTimestamp();
    QString value(property.valueString());
    bool utc = (value.endsWith(QString::fromAscii("Z")) || value.endsWith(QString::fromAscii("z")));
    if (utc)
        value.chop(1); // take away z from end;

    QDateTime dateTime = parseDateTime(value,QString::fromAscii("yyyyMMddThhmmss"));
    if (utc)
        dateTime.setTimeSpec(Qt::UTC);
    timeStamp->setLastModified(dateTime);
    return timeStamp;
}

/*!
 * Creates a QContactAnniversary from \a property
 */
QContactDetail* QVersitContactImporterPrivate::createAnniversary(
    const QVersitProperty& property) const
{
    QContactAnniversary* anniversary = new QContactAnniversary();
    QDateTime dateTime =
        parseDateTime(property.valueString(),QString::fromAscii("yyyyMMdd"));
    anniversary->setOriginalDate(dateTime.date());
    return anniversary;
}

/*!
 * Creates a QContactBirthday from \a property
 */
QContactDetail* QVersitContactImporterPrivate::createBirthday(
    const QVersitProperty& property) const
{
    QContactBirthday* bday = new QContactBirthday();
    QDateTime dateTime =
        parseDateTime(property.valueString(),QString::fromAscii("yyyyMMdd"));
    bday->setDate(dateTime.date());
    return bday;
}

/*!
 * Creates QContactNicknames from \a property and adds them to \a contact
 */
void QVersitContactImporterPrivate::createNicknames(
    const QVersitProperty& property,
    QContact& contact) const
{
    QStringList values = property.valueString().split(QChar::fromAscii(','));
    foreach(QString value,values) {
        // XXX use correct encoding instead of fromAscii
        QContactNickname* nickName = new QContactNickname();
        nickName->setNickname(value);
        contact.saveDetail(nickName);
        delete nickName;
    }
}

/*!
 * Creates a QContactOnlineAccount from \a property
 */
QContactDetail* QVersitContactImporterPrivate::createOnlineAccount(
    const QVersitProperty& property) const
{    
    QContactOnlineAccount* onlineAccount = new QContactOnlineAccount();
    onlineAccount->setAccountUri(property.valueString());
    if (property.name() == QString::fromAscii("X-SIP")) {
        QStringList subTypes = extractSubTypes(property);
        if (subTypes.count() == 0)
            subTypes.append(QContactOnlineAccount::SubTypeSip);
        onlineAccount->setSubTypes(subTypes);
    }
    else if (property.name() == QString::fromAscii("X-IMPP") ||
             property.name() == QString::fromAscii("IMPP")) {
        onlineAccount->setSubTypes(QContactOnlineAccount::SubTypeImpp);
    }
    else {
        // NOP
    }
    return onlineAccount;
}

/*!
 * Creates a QContactAvatar from \a property
 */
QContactDetail* QVersitContactImporterPrivate::createAvatar(
    const QVersitProperty& property,
    const QString& subType) const
{
    QString value(property.valueString());

    const QString valueParam =
        property.parameters().value(QString::fromAscii("VALUE"));

    if (valueParam != QString::fromAscii("URL")) {
        value = saveContentToFile(property);
    }

    QContactAvatar* avatar = 0;
    if (!value.isEmpty()) {
        avatar = new QContactAvatar();
        avatar->setAvatar(value);
        avatar->setSubType(subType);
    }
    return avatar;
}

/*!
 * Creates a QContactGeolocation from \a property
 */
QContactDetail* QVersitContactImporterPrivate::createGeoLocation(
    const QVersitProperty& property) const
{
    QContactGeolocation* geo = new QContactGeolocation();
    QStringList values = property.valueString().split(QChar::fromAscii(','));
    geo->setLongitude(takeFirst(values).toDouble());
    geo->setLatitude(takeFirst(values).toDouble());
    return geo;
}

/*!
 * Creates a QContactFamily from \a property
 */
QContactDetail* QVersitContactImporterPrivate::createFamily(
    const QVersitProperty& property,
    const QContact& contact) const
{
    QString val = property.valueString();
    QContactFamily family =
        static_cast<QContactFamily>(contact.detail(QContactFamily::DefinitionName));
    if (property.name() == QString::fromAscii("X-SPOUSE")) {
        family.setSpouse(val);
    } else if (property.name() == QString::fromAscii("X-CHILDREN")) {
        family.setChildren(val.split(QString::fromAscii(",")));
    }
    return new QContactDetail(family);
}

/*!
 * Creates a simple name-value contact detail.
 */
QContactDetail* QVersitContactImporterPrivate::createNameValueDetail(
    const QVersitProperty& property) const
{
    QContactDetail* detail = 0;
    QPair<QString,QString> nameAndValueType =
        mDetailMappings.value(property.name());
    if (nameAndValueType.first.length() > 0) {
        detail = new QContactDetail(nameAndValueType.first);
        detail->setValue(
            nameAndValueType.second,property.valueString());
    }
    return detail;
}

/*!
 * Creates a simple name-value contact detail.
 */
QContactDetail* QVersitContactImporterPrivate::createLabel(
    const QVersitProperty& property,
    const QContact& contact) const
{
    QContactName* name = 0;
    QContactDetail detail = contact.detail(QContactName::DefinitionName);
    if (!detail.isEmpty()) {
        name = new QContactName(static_cast<QContactName>(detail));
    } else {
        name = new QContactName();
    }

    // Setting the QContactDisplayLabel is done by the backend
    name->setCustomLabel(property.valueString());
    return name;
}

/*!
 * Extracts the list of contexts from \a types
 */
QStringList QVersitContactImporterPrivate::extractContexts(
    const QVersitProperty& property) const
{   
    QStringList types = 
        property.parameters().values(QString::fromAscii("TYPE"));
    QStringList contexts;
    foreach (QString type, types) {   
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
        property.parameters().values(QString::fromAscii("TYPE"));
    QStringList subTypes;
    foreach (QString type, types) {
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
    if (value.contains(QString::fromAscii("-"))) {
        dateTime = QDateTime::fromString(value,Qt::ISODate);
    } else {
        dateTime = QDateTime::fromString(value, format);
    }
    return dateTime;
}

/*!
 * Save the value of the \a property to a file and returns the file name.
 * The property value must be a QByteArray, and this is written directly to the file.
 */
QString QVersitContactImporterPrivate::saveContentToFile(
    const QVersitProperty& property) const
{
    QVariant variant = property.value();
    QString filename;
    bool ok = false;
    if (variant.type() == QVariant::ByteArray && mResourceSaver) {
        ok = mResourceSaver->saveResource(variant.toByteArray(), property, &filename);
    }
    return ok ? filename : QString();
}

/*!
 * Extracts the first and last name from \a document to a string.
 */
QString QVersitContactImporterPrivate::getFirstAndLastName(
    const QVersitDocument& document) const
{
    QString name;
    const QList<QVersitProperty> properties = document.properties();
    foreach(const QVersitProperty& nameProperty, properties) {
        if (nameProperty.name() == QString::fromAscii("N")) {
            QStringList values = nameProperty.valueString().split(QChar::fromAscii(';'));
            name.append(takeFirst(values));
            name.append(takeFirst(values));
            break;
        }
    }
    return name;
}
