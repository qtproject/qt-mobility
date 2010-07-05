/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qversitdefs_p.h"
#include "qversitcontactimporter_p.h"
#include "qversitdocument.h"
#include "qversitproperty.h"
#include "qmobilityglobal.h"

#include <qcontactmanagerengine.h>
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
#include <qcontactthumbnail.h>
#include <qcontactringtone.h>

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
        QStringList typeParameters = property.parameters().values(QLatin1String("TYPE"));
        if (typeParameters.contains(QLatin1String("PREF"), Qt::CaseInsensitive))
            importProperty(document, property, contactIndex, contact);
    }
    // ... then, do the rest of the properties.
    foreach (const QVersitProperty& property, properties) {
        QStringList typeParameters = property.parameters().values(QLatin1String("TYPE"));
        if (!typeParameters.contains(QLatin1String("PREF"), Qt::CaseInsensitive))
            importProperty(document, property, contactIndex, contact);
    }

    contact->setType(QContactType::TypeContact);
    QContactManagerEngine::setContactDisplayLabel(contact, QVersitContactImporterPrivate::synthesizedDisplayLabel(*contact));
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
    } else if (detailDefinitionName == QContactRingtone::DefinitionName) {
        success = createRingtone(property, contact);
    } else if (detailDefinitionName == QContactThumbnail::DefinitionName) {
        success = createThumbnail(property, contact);
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
        success = createCustomLabel(property, contact);
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

    QVariant variant = property.variantValue();
    if (property.valueType() != QVersitProperty::CompoundType
            || variant.type() != QVariant::StringList)
        return false;
    QStringList values = variant.toStringList();
    QString value(takeFirst(values));
    if (!value.isEmpty())
        name.setLastName(value);
    value = takeFirst(values);
    if (!value.isEmpty())
        name.setFirstName(value);
    value = takeFirst(values);
    if (!value.isEmpty())
        name.setMiddleName(value);
    value = takeFirst(values);
    if (!value.isEmpty())
        name.setPrefix(value);
    value = takeFirst(values);
    if (!value.isEmpty())
        name.setSuffix(value);

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
    QString value(property.value());
    if (value.isEmpty())
        return false;
    phone.setNumber(property.value());
    QStringList subTypes(extractSubTypes(property));
    if (property.name() == QLatin1String("X-ASSISTANT-TEL"))
        subTypes << QContactPhoneNumber::SubTypeAssistant;
    if (!subTypes.isEmpty())
        phone.setSubTypes(subTypes);

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

    QVariant variant = property.variantValue();
    if (property.valueType() != QVersitProperty::CompoundType
            || variant.type() != QVariant::StringList)
        return false;
    QStringList addressParts = variant.toStringList();
    QString value(takeFirst(addressParts));
    if (!value.isEmpty())
        address.setPostOfficeBox(value);
    // There is no setter for the Extended Address in QContactAddress:
    if (!addressParts.isEmpty())
        addressParts.removeFirst();
    value = takeFirst(addressParts);
    if (!value.isEmpty())
        address.setStreet(value);
    value = takeFirst(addressParts);
    if (!value.isEmpty())
        address.setLocality(value);
    value = takeFirst(addressParts);
    if (!value.isEmpty())
        address.setRegion(value);
    value = takeFirst(addressParts);
    if (!value.isEmpty())
        address.setPostcode(value);
    value = takeFirst(addressParts);
    if (!value.isEmpty())
        address.setCountry(value);
    QStringList subTypes(extractSubTypes(property));
    if (!subTypes.isEmpty())
        address.setSubTypes(subTypes);

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
    } else if (fieldName == QContactOrganization::FieldLogoUrl) {
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
    QVariant variant = property.variantValue();
    if (property.valueType() == QVersitProperty::CompoundType
        && variant.type() == QVariant::StringList) {
        QStringList values = variant.toStringList();
        QString name(takeFirst(values));
        if (!name.isEmpty())
            organization.setName(name);
        if (!values.isEmpty())
            organization.setDepartment(values);
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
        org.setLogoUrl(QUrl(location));
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
    if (!dateTime.isValid())
        return false;
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
    QDateTime dateTime = parseDateTime(property.value(), QLatin1String("yyyyMMdd"));
    if (!dateTime.isValid())
        return false;
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
    QDateTime dateTime = parseDateTime(property.value(), QLatin1String("yyyyMMdd"));
    if (!dateTime.isValid())
        return false;
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
    QVariant variant = property.variantValue();
    if (property.valueType() != QVersitProperty::ListType
            || variant.type() != QVariant::StringList)
        return false;
    QStringList values = variant.toStringList();
    QStringList contexts = extractContexts(property);
    foreach(const QString& value, values) {
        if (!value.isEmpty()) {
            QContactNickname nickName;
            nickName.setNickname(value);
            saveDetailWithContext(contact, &nickName, contexts);
        }
    }
    return true;
}

/*!
 * Creates QContactTags from \a property and adds them to \a contact
 */
bool QVersitContactImporterPrivate::createTags(
    const QVersitProperty& property, QContact* contact) const
{
    QVariant variant = property.variantValue();
    if (property.valueType() != QVersitProperty::ListType
            || variant.type() != QVariant::StringList)
        return false;
    QStringList values = variant.toStringList();
    QStringList contexts = extractContexts(property);
    foreach(const QString& value, values) {
        if (!value.isEmpty()) {
            QContactTag tag;
            tag.setTag(value);
            saveDetailWithContext(contact, &tag, contexts);
        }
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
    QString value(property.value());
    if (value.isEmpty())
        return false;
    onlineAccount.setAccountUri(property.value());
    if (property.name() == QLatin1String("X-SIP")) {
        QStringList subTypes = extractSubTypes(property);
        if (subTypes.isEmpty())
            subTypes.append(QContactOnlineAccount::SubTypeSip);
        onlineAccount.setSubTypes(subTypes);
    } else if (property.name() == QLatin1String("X-IMPP") ||
               property.name() == QLatin1String("IMPP") ||
               property.name() == QLatin1String("X-JABBER")) {
        onlineAccount.setSubTypes(QContactOnlineAccount::SubTypeImpp);
    }

    saveDetailWithContext(contact, &onlineAccount, extractContexts(property));
    return true;
}

bool QVersitContactImporterPrivate::createRingtone(const QVersitProperty &property,
                                                   QContact *contact) const
{
    QString location;
    QByteArray data;
    if (saveDataFromProperty(property, &location, &data) && !location.isEmpty()) {
        QContactRingtone ringtone;
        ringtone.setAudioRingtoneUrl(location);
        saveDetailWithContext(contact, &ringtone, extractContexts(property));
        return true;
    }
    return false;
}

/*!
 * Creates a QContactAvatar from \a property
 */
bool QVersitContactImporterPrivate::createThumbnail(
    const QVersitProperty& property, QContact* contact) const
{
    QString location;
    QByteArray data;
    bool success = false;

    if (saveDataFromProperty(property, &location, &data) && !location.isEmpty()) {
        QContactAvatar avatar;
        avatar.setImageUrl(location);
        saveDetailWithContext(contact, &avatar, extractContexts(property));
        success = true;
    }
    if (!data.isEmpty()) {
        QImage image;
        if (image.loadFromData(data)) {
            QContactThumbnail thumbnail = contact->detail<QContactThumbnail>();
            // In the case of multiple thumbnails, pick the smallest one.
            if (thumbnail.isEmpty() || image.byteCount() < thumbnail.thumbnail().byteCount()) {
                thumbnail.setThumbnail(image);
            }
            saveDetailWithContext(contact, &thumbnail, extractContexts(property));
            success = true;
        }
    }

    return success;
}

/*!
 * Creates a QContactGeoLocation from \a property
 */
bool QVersitContactImporterPrivate::createGeoLocation(
    const QVersitProperty& property, QContact* contact) const
{
    QContactGeoLocation geo;
    QVariant variant = property.variantValue();
    if (property.valueType() != QVersitProperty::CompoundType
            || variant.type() != QVariant::StringList)
        return false;
    QStringList values = variant.toStringList();
    bool ok1;
    geo.setLongitude(takeFirst(values).toDouble(&ok1));
    bool ok2;
    geo.setLatitude(takeFirst(values).toDouble(&ok2));

    if (ok1 && ok2) {
        saveDetailWithContext(contact, &geo, extractContexts(property));
        return true;
    } else {
        return false;
    }
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
        if (val.isEmpty())
            return false;
        family.setSpouse(val);
    } else if (property.name() == QLatin1String("X-CHILDREN")) {
        QVariant variant = property.variantValue();
        if (property.valueType() != QVersitProperty::ListType
                || variant.type() != QVariant::StringList)
            return false;
        QStringList values = variant.toStringList();
        if (values.isEmpty())
            return false;
        family.setChildren(values);
    } else {
        return false;
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
    QString value(property.value());
    if (value.isEmpty())
        return false;
    QPair<QString,QString> nameAndValueType =
        mDetailMappings.value(property.name());
    if (nameAndValueType.first.isEmpty())
        return false;

    QContactDetail detail(nameAndValueType.first);
    detail.setValue(nameAndValueType.second, value);

    saveDetailWithContext(contact, &detail, extractContexts(property));
    return true;
}

/*!
 * Find an existing QContactName and set the CustomLabel field on it
 */
bool QVersitContactImporterPrivate::createCustomLabel(
    const QVersitProperty& property, QContact* contact) const
{
    QString label(property.value());
    if (!label.isEmpty()) {
        QContactName name;
        QContactName existingName = contact->detail<QContactName>();
        if (!existingName.isEmpty()) {
            name = existingName;
        }

        name.setCustomLabel(property.value());

        saveDetailWithContext(contact, &name, extractContexts(property));
        return true;
    } else {
        return false;
    }
}

/*!
 * Extracts the list of contexts from \a types
 */
QStringList QVersitContactImporterPrivate::extractContexts(
    const QVersitProperty& property) const
{
    QStringList types = property.parameters().values(QLatin1String("TYPE"));
    QStringList contexts;
    foreach (const QString& type, types) {
        QString value = mContextMappings.value(type.toUpper());
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
    QStringList types = property.parameters().values(QLatin1String("TYPE"));
    QStringList subTypes;
    foreach (const QString& type, types) {
        QString subType = mSubTypeMappings.value(type.toUpper());
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
    if (!contexts.isEmpty()) {
        detail->setContexts(contexts);
    }
    contact->saveDetail(detail);
}

/*! Synthesize the display label from the name of the contact, or, failing that, the nickname of
the contact, or failing that, the organisation of the contact.
Returns the synthesized display label.
 */
QString QVersitContactImporterPrivate::synthesizedDisplayLabel(const QContact& contact)
{
    /* XXX This is copied and modified from QContactManagerEngine.  This should be made a public
       static function in QCME and called here */
    QList<QContactName> allNames = contact.details<QContactName>();

    const QLatin1String space(" ");

    // synthesize the display label from the name.
    foreach (const QContactName& name, allNames) {
        if (!name.customLabel().isEmpty()) {
            // default behaviour is to allow the user to define a custom display label.
            return name.customLabel();
        }

        QString result;
        if (!name.value(QContactName::FieldPrefix).trimmed().isEmpty()) {
           result += name.value(QContactName::FieldPrefix);
        }
        if (!name.value(QContactName::FieldFirstName).trimmed().isEmpty()) {
            if (!result.isEmpty())
                result += space;
            result += name.value(QContactName::FieldFirstName);
        }
        if (!name.value(QContactName::FieldMiddleName).trimmed().isEmpty()) {
            if (!result.isEmpty())
                result += space;
            result += name.value(QContactName::FieldMiddleName);
        }
        if (!name.value(QContactName::FieldLastName).trimmed().isEmpty()) {
            if (!result.isEmpty())
                result += space;
            result += name.value(QContactName::FieldLastName);
        }
        if (!name.value(QContactName::FieldSuffix).trimmed().isEmpty()) {
            if (!result.isEmpty())
                result += space;
            result += name.value(QContactName::FieldSuffix);
        }
        if (!result.isEmpty())
            return result;
    }

    QList<QContactNickname> allNicknames = contact.details<QContactNickname>();
    foreach (const QContactNickname& nickname, allNicknames) {
        if (!nickname.nickname().isEmpty())
            return nickname.nickname();
    }

    /* Well, we had no non empty names. if we have orgs, fall back to those */
    QList<QContactOrganization> allOrgs = contact.details<QContactOrganization>();
    foreach (const QContactOrganization& org, allOrgs) {
        if (!org.name().isEmpty())
            return org.name();
    }

    return QString();
}
