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
#include <qversitdocument.h>
#include <qversitproperty.h>
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
#include <QHash>
#include <QFile>

/*!
 * Constructor.
 */
QVersitContactImporterPrivate::QVersitContactImporterPrivate()
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
    mUnconvertedVersitProperties.clear();
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
            detail = createOrganization(property);
        } else if (detailDefinitionName == QContactNickname::DefinitionName) {
            createNicknames(property,contact);
        } else if (detailDefinitionName == QContactAvatar::DefinitionName) {
            detail = createAvatar(property,versitDocument,detailDefinition.second);
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
        } else {
            detail = createNameValueDetail(property);
            if (!detail)
                mUnconvertedVersitProperties.append(property);
        }

        if (detail) {
            QStringList contexts(extractContexts(property));
            if (!contexts.empty())
                detail->setContexts(contexts);
            contact.saveDetail(detail);
            delete detail;
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
    // Restrict only one name can exist, if multiple than choose first
    // and discard rest
    if( !contact.detail(QContactName::DefinitionName).isEmpty()){
        return 0;
    }
    QContactName* name = new QContactName();
    QList<QByteArray> values = property.value().split(';');
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
    phone->setNumber(QString::fromAscii(property.value()));
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
    
    QList<QByteArray> addressParts = property.value().split(';');
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
    const QVersitProperty& property) const
{
    QContactOrganization* org = new QContactOrganization;
    if (property.name() == QString::fromAscii("TITLE")) {
        org->setTitle(QString::fromAscii(property.value()));
    } else if (property.name() == QString::fromAscii("ORG")) {
        setOrganizationNames(*org,property);
    } else if (property.name() == QString::fromAscii("LOGO")) {
        setOrganizationLogo(*org,property);
    } else if (property.name() == QString::fromAscii("X-ASSISTANT")) {
        org->setAssistantName(QString::fromAscii(property.value()));
    } else {
         // NOP
    }
    if (org->isEmpty()) {
        delete org;
        org = 0;
    }
    return org;
}

/*!
 * Set the organization name and department(s) from \a property.
 */
void QVersitContactImporterPrivate::setOrganizationNames(
    QContactOrganization& org,
    const QVersitProperty& property) const
{
    QByteArray value = property.value();
    int firstSemicolon = value.indexOf(";");
    if (firstSemicolon >= 0) {
        org.setName(QString::fromAscii(value.left(firstSemicolon)));
        org.setDepartment(QString::fromAscii(value.mid(firstSemicolon+1)));
    } else {
        org.setName(QString::fromAscii(value));
    }
}

/*!
 * Set the organization logo from \a property.
 */
void QVersitContactImporterPrivate::setOrganizationLogo(
    QContactOrganization& org,
    const QVersitProperty& property) const
{
    QString value(QString::fromAscii(property.value()));

    const QString valueParam =
        property.parameters().value(QString::fromAscii("VALUE"));

    if (valueParam != QString::fromAscii("URL")) {
        QString path(mImagePath);
        if (!path.endsWith(QString::fromAscii("/")))
            path += QString::fromAscii("/");
        // Let saveContentToFile to generate a random string as the name
        value = saveContentToFile(path,property);
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
    QByteArray value(property.value());
    bool utc = (value.endsWith("Z") || value.endsWith("z"));
    if (utc)
        value.chop(1); // take away z from end;

    QDateTime dateTime = parseDateTime(value,"yyyyMMddThhmmss");
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
        parseDateTime(property.value(),"yyyyMMdd");
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
        parseDateTime(property.value(),"yyyyMMdd");
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
    QList<QByteArray> values = property.value().split(',');
    foreach(QByteArray value,values) {
        QContactNickname* nickName = new QContactNickname();
        nickName->setNickname(QString::fromAscii(value));
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
    onlineAccount->setAccountUri(QString::fromAscii(property.value()));
    QStringList subTypes = extractSubTypes(property);
    if (subTypes.count() == 0)
        subTypes.append(QContactOnlineAccount::SubTypeSip);
    onlineAccount->setSubTypes(subTypes);
    return onlineAccount;
}

/*!
 * Creates a QContactAvatar from \a property
 */
QContactDetail* QVersitContactImporterPrivate::createAvatar(
    const QVersitProperty& property,
    const QVersitDocument& versitDocument,
    const QString& subType) const
{
    QString value(QString::fromAscii(property.value()));

    const QString valueParam =
        property.parameters().value(QString::fromAscii("VALUE"));

    if (valueParam != QString::fromAscii("URL")) {
        QString pathAndName(mImagePath);
        if (subType == QContactAvatar::SubTypeAudioRingtone)
            pathAndName = mAudioClipPath;
        if (!pathAndName.endsWith(QString::fromAscii("/")))
            pathAndName += QString::fromAscii("/");
        pathAndName += getFirstAndLastName(versitDocument);
        value = saveContentToFile(pathAndName,property);
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
    QList<QByteArray> values = property.value().split(',');
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
    QString val = QString::fromAscii(property.value());
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
            nameAndValueType.second,QString::fromAscii(property.value()));
    }
    return detail;
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
 * Takes the first value in \a list and converts it to a QString.
 * An empty QString is returned, if the list is empty.
 */
QString QVersitContactImporterPrivate::takeFirst(QList<QByteArray>& list) const
{
    QString first;
    if (!list.isEmpty())
        first = QString::fromAscii(list.takeFirst());
    return first; 
}

/*!
 * Parses a date and time from text
 */
QDateTime QVersitContactImporterPrivate::parseDateTime(
    const QByteArray& text,
    const QByteArray& format) const
{
    QDateTime dateTime;
    QString value(QString::fromAscii(text));
    if (text.contains("-")) {
        dateTime = QDateTime::fromString(value,Qt::ISODate);
    } else {
        dateTime = QDateTime::fromString(value, QString::fromAscii(format));
    }
    return dateTime;
}

/*!
 * Save the value of the \a property to a file with name \a pathAndName.
 */
QString QVersitContactImporterPrivate::saveContentToFile(
    const QString& pathAndName,
    const QVersitProperty& property) const
{
    QString encoding =
        property.parameters().value(QString::fromAscii("ENCODING"));
    QByteArray content = property.value();

    QString type =
        property.parameters().value(QString::fromAscii("TYPE"));
    // Use the type parameter value as it is, if not found in the mapping table
    QString extension = mFileExtensionMappings.value(type,type);

    QString fileName(pathAndName);
    fileName += QString::number(qrand());
    fileName += QString::fromAscii(".");
    fileName += extension.toLower();

    QFile file(fileName);
    qint64 writeResult = -1;
    if (file.open(QIODevice::WriteOnly)) {
        if (encoding == QString::fromAscii("BASE64") ||
            encoding == QString::fromAscii("B")) {
            writeResult = file.write(QByteArray::fromBase64(property.value()));
        } else {
            // default assumption
            // quoted-printable encoding is parsed already in the reader
            writeResult = file.write(property.value());
        }
    }
    file.close();
    return (writeResult > 0) ? fileName : QString();
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
            QList<QByteArray> values = nameProperty.value().split(';');
            name.append(takeFirst(values));
            name.append(takeFirst(values));
            break;
        }
    }
    return name;
}
