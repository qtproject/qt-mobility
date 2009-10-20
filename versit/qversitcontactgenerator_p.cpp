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
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qversitdefs.h"
#include "qversitcontactgenerator_p.h"
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
#include <qcontactavatar.h>
#include <QHash>
#include <QImage>

/*!
 * Constructor.
 */
QVersitContactGeneratorPrivate::QVersitContactGeneratorPrivate()
{
    mContextMappings.insert(
        QString::fromAscii(versitContextWorkId),QContactDetail::ContextWork);
    mContextMappings.insert(
        QString::fromAscii(versitContextHomeId),QContactDetail::ContextHome);
    
    mSubTypeMappings.insert(
        QString::fromAscii(versitDomesticId),QContactAddress::SubTypeDomestic);
    mSubTypeMappings.insert(
        QString::fromAscii(versitInternationalId),QContactAddress::SubTypeInternational);
    mSubTypeMappings.insert(
        QString::fromAscii(versitPostalId),QContactAddress::SubTypePostal);
    mSubTypeMappings.insert(
        QString::fromAscii(versitParcelId),QContactAddress::SubTypeParcel);
    mSubTypeMappings.insert(
        QString::fromAscii(versitVoiceId),QContactPhoneNumber::SubTypeVoice);
    mSubTypeMappings.insert(
        QString::fromAscii(versitCellId),QContactPhoneNumber::SubTypeMobile);
    mSubTypeMappings.insert(
        QString::fromAscii(versitModemId),QContactPhoneNumber::SubTypeModem);
    mSubTypeMappings.insert(
        QString::fromAscii(versitCarId),QContactPhoneNumber::SubTypeCar);
    mSubTypeMappings.insert(
        QString::fromAscii(versitVideoId),QContactPhoneNumber::SubTypeVideo);
    mSubTypeMappings.insert(
        QString::fromAscii(versitFaxId),QContactPhoneNumber::SubTypeFacsimile);
    mSubTypeMappings.insert(
        QString::fromAscii(versitBbsId),QContactPhoneNumber::SubTypeBulletinBoardSystem);
    mSubTypeMappings.insert(
        QString::fromAscii(versitPagerId),QContactPhoneNumber::SubTypePager);
}

/*!
 * Destructor.
 */
QVersitContactGeneratorPrivate::~QVersitContactGeneratorPrivate()
{
}

/*!
 * Generates a QContact from \a versitDocument.
 */
QContact QVersitContactGeneratorPrivate::generateContact(const QVersitDocument& versitDocument)
{
    QContact contact;
    const QList<QVersitProperty> properties = versitDocument.properties();
    foreach (QVersitProperty property, properties) {
        QContactDetail* detail = 0;
        if (property.name() == QString::fromAscii(versitNameId)) {
            detail = createName(property);
        }
        else if (property.name() == QString::fromAscii(versitPhoneId)) {
            detail = createPhone(property);
        }
        else if (property.name() == QString::fromAscii(versitAddressId)) {
            detail = createAddress(property);
        }
        else if (property.name() == QString::fromAscii(versitOrganizationId)) {
            detail = createOrganization(property);
        }
        else if (property.name() == QString::fromAscii(versitEmailId)) {
            detail = createEmail(property);
        }
        else if (property.name() == QString::fromAscii(versitUrlId)) {
            detail = createUrl(property);
        }
        else if (property.name() == QString::fromAscii(versitUidId)) {
            detail = createUid(property);
        }
        else if (property.name() == QString::fromAscii(versitRevisionId)) {
            detail = createTimeStamp(property);
        }
        else if (property.name() == QString::fromAscii(versitAnniversaryId)) {
            detail = createAnniversary(property);
        }
        else if (property.name() == QString::fromAscii(versitPhotoId)) {
            detail = createAvatar(property, versitDocument);
        }
        else {
            // NOP
        }
        if (detail) {
            detail->setContexts(extractContexts(property));
            contact.saveDetail(detail);
        }
    }
    return contact;
}

/*!
 * Creates a QContactName from \a property
 */
QContactDetail* QVersitContactGeneratorPrivate::createName(
    const QVersitProperty& property) const
{
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
QContactDetail* QVersitContactGeneratorPrivate::createPhone(
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
QContactDetail* QVersitContactGeneratorPrivate::createAddress(
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

QContactDetail* QVersitContactGeneratorPrivate::createOrganization(
    const QVersitProperty& property) const
{
    QContactOrganization* org = new QContactOrganization();
    
    // TODO: now assuming that only the ORG part is present
    QByteArray value = property.value();
    int firstSemic = value.indexOf(";");
    QByteArray orgName = value.left(firstSemic);
    QByteArray orgUnit = value.mid(firstSemic+1,value.size());
    
    org->setName(QString::fromAscii(orgName));
    org->setDepartment(QString::fromAscii(orgUnit));
    
    return org;
}

/*!
 * Creates a QContactEmailAddress from \a property
 */
QContactDetail* QVersitContactGeneratorPrivate::createEmail(
    const QVersitProperty& property) const
{
    QContactEmailAddress* email = new QContactEmailAddress();
    email->setEmailAddress(QString::fromAscii(property.value()));
    return email;
}

/*!
 * Creates a QContactUrl from \a property
 */
QContactDetail* QVersitContactGeneratorPrivate::createUrl(
    const QVersitProperty& property) const
{
    QContactUrl* url = new QContactUrl();
    url->setUrl(QString::fromAscii(property.value()));
    return url;
}

/*!
 * Creates a QContactUid from \a property
 */
QContactDetail* QVersitContactGeneratorPrivate::createUid(
    const QVersitProperty& property) const
{
    QContactGuid* uid = new QContactGuid();
    uid->setGuid(QString::fromAscii(property.value()));
    return uid;
}

/*!
 * Creates a QContactTimeStamp from \a property
 */
QContactDetail* QVersitContactGeneratorPrivate::createTimeStamp(
    const QVersitProperty& property) const
{
    QContactTimestamp* timeStamp = new QContactTimestamp();
    QString value(QString::fromAscii(property.value()));
    bool utc = value.endsWith(QString::fromAscii("Z"),Qt::CaseInsensitive);
    if (utc)
        value.chop(1); // take away z from end;
    QDateTime dateTime;
    if (value.contains(QString::fromAscii("-"))) {
        dateTime = QDateTime::fromString(value,Qt::ISODate);
    }
    else {
        dateTime = QDateTime::fromString(value,
                        QString::fromAscii(versitDateTimeSpecIso8601Basic));
    }    
    if (utc)
        dateTime.setTimeSpec(Qt::UTC);
    timeStamp->setLastModified(dateTime);
    return timeStamp;
}

/*!
 * Creates a QContactAnniversary from \a property
 */
QContactDetail* QVersitContactGeneratorPrivate::createAnniversary(
    const QVersitProperty& property) const
{
    QContactAnniversary* anniversary = new QContactAnniversary();
    QString value(QString::fromAscii(property.value()));
    QDate date = ( value.contains(QString::fromAscii("-")))
                 ? QDate::fromString(value,Qt::ISODate)
                 : QDate::fromString(value,versitDateSpecIso8601Basic);
    anniversary->setOriginalDate(date);
    return anniversary;
}

/*!
 * Creates a QContactAvatar from \a property
 */
QContactDetail* QVersitContactGeneratorPrivate::createAvatar(
    const QVersitProperty& property, const QVersitDocument& versitDocument) const
{
    QContactAvatar* avatar = new QContactAvatar();

    // Image name: <FirstName><LastName>.<ext>
    QString imgName(versitPhotoDir);
    QVersitProperty nameProperty;
    const QList<QVersitProperty> properties = versitDocument.properties();
    foreach(const QVersitProperty& nameP, properties) {
        if (nameP.name() == "N") {
            nameProperty = nameP;
            break;
        }
    }

    QList<QByteArray> values = nameProperty.value().split(';');
    imgName.append("/");
    imgName.append(takeFirst(values));
    imgName.append(takeFirst(values));
    imgName.append(".");

    // Image format
    const QString format =
            property.parameters().value(QString::fromAscii(versitType));

    imgName.append(QString(format).toLower());

    QByteArray value = property.value();

    QImage image;
    image.fromData(value);
    image.save(imgName, format.toAscii());


    avatar->setAvatar(imgName);
    avatar->setSubType(QContactAvatar::SubTypeImage);

    return avatar;
}

/*!
 * Extracts the list of contexts from \a types
 */
QStringList QVersitContactGeneratorPrivate::extractContexts(
    const QVersitProperty& property) const
{   
    QStringList types = 
        property.parameters().values(QString::fromAscii(versitType));
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
QStringList QVersitContactGeneratorPrivate::extractSubTypes(
    const QVersitProperty& property) const
{
    QStringList types = 
        property.parameters().values(QString::fromAscii(versitType));
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
QString QVersitContactGeneratorPrivate::takeFirst(QList<QByteArray>& list) const
{
    QString first;
    if (!list.isEmpty())
        first = QString::fromAscii(list.takeFirst());
    return first; 
}
