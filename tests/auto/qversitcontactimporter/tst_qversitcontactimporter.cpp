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

//TESTED_COMPONENT=src/versit

#include "tst_qversitcontactimporter.h"
#include "qversitcontactimporter.h"
#include "qversitcontactimporter_p.h"
#include <qversitproperty.h>
#include <qversitdocument.h>
#include <qtcontacts.h>
#include <QtTest/QtTest>
#include <QDir>

QTM_BEGIN_NAMESPACE
class MyQVersitContactImporterPropertyHandler : public QVersitContactImporterPropertyHandler
{
public:
    MyQVersitContactImporterPropertyHandler()
        : mPreProcess(false)
    {
    }

    bool preProcessProperty(const QVersitDocument& document,
                            const QVersitProperty& property,
                            int contactIndex,
                            QContact* contact)
    {
        Q_UNUSED(document)
        Q_UNUSED(contact)
        Q_UNUSED(contactIndex);
        mPreProcessedProperties.append(property);
        return mPreProcess;
    }

    bool postProcessProperty(const QVersitDocument& document,
                             const QVersitProperty& property,
                             bool alreadyProcessed,
                             int contactIndex,
                             QContact* contact)
    {
        Q_UNUSED(document)
        Q_UNUSED(contact)
        Q_UNUSED(contactIndex)
        if (!alreadyProcessed)
            mUnknownProperties.append(property);
        else
            mPostProcessedProperties.append(property);
        return false;
    }

    void clear()
    {
        mPreProcess = false;
        mPropertyNamesToProcess.clear();
        mUnknownProperties.clear();
        mPreProcessedProperties.clear();
        mPostProcessedProperties.clear();
    }

    // a hook to control what preProcess returns:
    bool mPreProcess;
    QStringList mPropertyNamesToProcess;
    QList<QVersitProperty> mUnknownProperties;
    QList<QVersitProperty> mPreProcessedProperties;
    QList<QVersitProperty> mPostProcessedProperties;
};

class MyQVersitResourceHandler : public QVersitResourceHandler
{
public:
    MyQVersitResourceHandler() : mIndex(0)
    {
    }

    bool saveResource(const QByteArray& contents, const QVersitProperty& property,
                      QString* location)
    {
        Q_UNUSED(property);
        *location = QString::number(mIndex++);
        mObjects.insert(*location, contents);
        return true;
    }

    bool loadResource(const QString &location, QByteArray *contents, QString *mimeType)
    {
        Q_UNUSED(location)
        Q_UNUSED(contents)
        Q_UNUSED(mimeType)
        return false;
    }

    void clear()
    {
        mIndex = 0;
        mObjects.clear();
    }

    int mIndex;
    QMap<QUrl, QByteArray> mObjects;
};

const static QByteArray SAMPLE_GIF(QByteArray::fromBase64(
        "R0lGODlhEgASAIAAAAAAAP///yH5BAEAAAEALAAAAAASABIAAAIdjI+py+0G"
        "wEtxUmlPzRDnzYGfN3KBaKGT6rDmGxQAOw=="));

const static QByteArray NOKIA_GIF(QByteArray::fromBase64(
        "R0lGODdhOAAKAIQRAAAvwQAwwwAwxAAxxwAyygAzywAzzBBHwC9nz0+A0HCf35+/4LDQ78/f79/o"
        "8O/v8PD3/////////////////////////////////////////////////////////////ywAAAAA"
        "OAAKAAAFsCAiik9kRqPJHIfhGixjisuJpqk9Inb0vjaBC0UwFH+uhM+gNBUCw6Wh92vYDAXkCZhF"
        "apMmA3Qajppav6tr8TqUp0DqEIwtqsmRR/Kl2A4RfFKCcnBMbYR+Uw5xg2lAjIlLCS88dyYNLn1S"
        "TYwvk3NmkXSQLgVvXmQuBCcQXlI7Io9MpyWCbKgublgCNgxfP0eOs6dvUgsPyMgvEAUAeCafUWhe"
        "bpI2LQMFenuhZy8hADs="));

QTM_END_NAMESPACE

QTM_USE_NAMESPACE

void tst_QVersitContactImporter::init()
{
    mImporter = new QVersitContactImporter();
    mResourceHandler = new MyQVersitResourceHandler();
    mImporter->setResourceHandler(mResourceHandler);
    mPropertyHandler = new MyQVersitContactImporterPropertyHandler();
    mImporter->setPropertyHandler(mPropertyHandler);
}

void tst_QVersitContactImporter::cleanup()
{
    QVERIFY(mImporter->propertyHandler() == mPropertyHandler);
    mImporter->setPropertyHandler(static_cast<QVersitContactImporterPropertyHandlerV2*>(0));
    delete mPropertyHandler;
    QVERIFY(mImporter->resourceHandler() == mResourceHandler);
    mImporter->setResourceHandler(0);
    delete mResourceHandler;
    delete mImporter;
}

void tst_QVersitContactImporter::testName()
{
    QVersitDocument document(QVersitDocument::VCard30Type);
    QVersitProperty nameProperty;
    QStringList value;
    value.append(QLatin1String("John"));//FirstName
    value.append(QLatin1String("Citizen"));//LastName
    value.append(QLatin1String("Anonymous"));//GivenName
    value.append(QLatin1String("Dr"));//PreFix
    value.append(QLatin1String("MSc"));//Suffix
    nameProperty.setName(QLatin1String("N"));
    nameProperty.setValue(value);
    nameProperty.setValueType(QVersitProperty::CompoundType);
    document.addProperty(nameProperty);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    QContact contact = mImporter->contacts().first();
    QContactName name = (QContactName)contact.detail(QContactName::DefinitionName);
    QCOMPARE(name.lastName(),value[0]);
    QCOMPARE(name.firstName(),value[1]);
    QCOMPARE(name.middleName(),value[2]);
    QCOMPARE(name.prefix(),value[3]);
    QCOMPARE(name.suffix(),value[4]);

    // Multiple names, first one will be picked and rest will be discarded
    nameProperty = QVersitProperty();
    QStringList anotherValue;
    anotherValue.append(QLatin1String("FakeJohn"));//FirstName
    anotherValue.append(QLatin1String("FakeCitizen"));//LastName
    anotherValue.append(QLatin1String("FakeAnonymous"));//GivenName
    anotherValue.append(QLatin1String("FakeDr"));//PreFix
    anotherValue.append(QLatin1String("FakeMSc"));//Suffix
    nameProperty.setName(QLatin1String("N"));
    nameProperty.setValue(anotherValue);
    nameProperty.setValueType(QVersitProperty::CompoundType);
    document.addProperty(nameProperty);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    QList<QContactDetail> names = contact.details(QContactName::DefinitionName);
    QCOMPARE(names.count(),1);
    // anotherValue should be discarded, so check for value
    name = (QContactName)names[0];
    QCOMPARE(name.lastName(),value[0]);
    QCOMPARE(name.firstName(),value[1]);
    QCOMPARE(name.middleName(),value[2]);
    QCOMPARE(name.prefix(),value[3]);
    QCOMPARE(name.suffix(),value[4]);
}

// check that it doesn't crash if the FN property comes before the N property.
void tst_QVersitContactImporter::testNameWithFormatted()
{
    QVersitDocument document(QVersitDocument::VCard30Type);
    QVersitProperty fnProperty;
    fnProperty.setName(QLatin1String("FN"));
    fnProperty.setValue(QLatin1String("First Last"));
    document.addProperty(fnProperty);
    QVersitProperty nProperty;
    nProperty.setName(QLatin1String("N"));
    nProperty.setValue(QStringList() << QLatin1String("Last") << QLatin1String("First") << QLatin1String("Middle") << QLatin1String("Prefix") << QLatin1String("Suffix"));
    nProperty.setValueType(QVersitProperty::CompoundType);
    document.addProperty(nProperty);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    QContact contact = mImporter->contacts().first();
    QContactName name = contact.detail<QContactName>();
    QCOMPARE(name.firstName(), QLatin1String("First"));
    QCOMPARE(name.lastName(), QLatin1String("Last"));
    QCOMPARE(name.middleName(), QLatin1String("Middle"));
    QCOMPARE(name.prefix(), QLatin1String("Prefix"));
    QCOMPARE(name.suffix(), QLatin1String("Suffix"));
    QCOMPARE(name.customLabel(), QLatin1String("First Last"));
}

void tst_QVersitContactImporter::testAddress()
{
    QVersitDocument document(QVersitDocument::VCard30Type);
    QVersitProperty property;
    property.setName(QLatin1String("ADR"));
    property.setValue(QStringList(QString()));
    property.setValueType(QVersitProperty::CompoundType);

    // Empty value for the address
    document = createDocumentWithProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    QContact contact = mImporter->contacts().first();
    QContactAddress address = contact.detail<QContactAddress>();
    QCOMPARE(address.postOfficeBox(),QString());
    QCOMPARE(address.street(),QString());
    QCOMPARE(address.locality(),QString());
    QCOMPARE(address.region(),QString());
    QCOMPARE(address.postcode(),QString());
    QCOMPARE(address.country(),QString());

    // Address with all the fields filled
    property.setValue(QStringList()
                      << QLatin1String("PO Box")
                      << QLatin1String("E")
                      << QLatin1String("My Street")
                      << QLatin1String("My Town")
                      << QLatin1String("My State")
                      << QLatin1String("12345")
                      << QLatin1String("My Country")
                      );
    property.setValueType(QVersitProperty::CompoundType);
    document = createDocumentWithProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    address = contact.detail<QContactAddress>();
    QCOMPARE(address.postOfficeBox(),QLatin1String("PO Box"));
    QCOMPARE(address.street(),QLatin1String("My Street"));
    QCOMPARE(address.locality(),QLatin1String("My Town"));
    QCOMPARE(address.region(),QLatin1String("My State"));
    QCOMPARE(address.postcode(),QLatin1String("12345"));
    QCOMPARE(address.country(),QLatin1String("My Country"));

    // Address with TYPE parameters converted to contexts and subtypes
    property.insertParameter(QLatin1String("TYPE"),QLatin1String("HOME"));
    property.insertParameter(QLatin1String("TYPE"),QLatin1String("WORK"));
    property.insertParameter(QLatin1String("TYPE"),QLatin1String("DOM"));
    property.insertParameter(QLatin1String("TYPE"),QLatin1String("INTL"));
    property.insertParameter(QLatin1String("TYPE"),QLatin1String("POSTAL"));
    property.insertParameter(QLatin1String("TYPE"),QLatin1String("PARCEL"));
    property.insertParameter(QLatin1String("TYPE"),QLatin1String("X-EXTENSION"));
    document = createDocumentWithProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    address = contact.detail<QContactAddress>();
    QStringList contexts = address.contexts();
    QVERIFY(contexts.contains(QContactDetail::ContextHome));
    QVERIFY(contexts.contains(QContactDetail::ContextWork));
    QStringList subTypes = address.subTypes();
    QVERIFY(subTypes.contains(QContactAddress::SubTypeDomestic));
    QVERIFY(subTypes.contains(QContactAddress::SubTypeInternational));
    QVERIFY(subTypes.contains(QContactAddress::SubTypePostal));
    QVERIFY(subTypes.contains(QContactAddress::SubTypeParcel));
}

void tst_QVersitContactImporter::testOrganizationName()
{
    QVersitDocument document(QVersitDocument::VCard30Type);
    QVersitProperty property;

    // Empty value for the organization
    property.setName(QLatin1String("ORG"));
    document = createDocumentWithProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    QContact contact = mImporter->contacts().first();
    QContactOrganization organization = contact.detail<QContactOrganization>();
    QCOMPARE(organization.name(),QString());
    QCOMPARE(organization.department().count(),0);

    // Organization with single value
    property.setValueType(QVersitProperty::CompoundType);
    property.setValue(QStringList(QLatin1String("Nokia")));
    document = createDocumentWithProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    organization = contact.detail<QContactOrganization>();
    QCOMPARE(organization.name(),QLatin1String("Nokia"));
    QCOMPARE(organization.department().count(),0);

    // Organization with one Organizational Unit
    property.setValue(QStringList() << QLatin1String("Nokia") << QLatin1String("R&D"));
    document = createDocumentWithProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    organization = contact.detail<QContactOrganization>();
    QCOMPARE(organization.name(),QLatin1String("Nokia"));
    QCOMPARE(organization.department().count(),1);
    QCOMPARE(organization.department().at(0),QLatin1String("R&D"));

    // Organization with more Organizational Units
    property.setValue(QStringList()
                      << QLatin1String("Nokia")
                      << QLatin1String("R&D")
                      << QLatin1String("Devices")
                      << QLatin1String("Qt"));
    property.setValueType(QVersitProperty::CompoundType);
    document = createDocumentWithProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    organization = contact.detail<QContactOrganization>();
    QCOMPARE(organization.name(),QLatin1String("Nokia"));
    QCOMPARE(organization.department().count(),3);
    QCOMPARE(organization.department().at(0),QLatin1String("R&D"));
    QCOMPARE(organization.department().at(1),QLatin1String("Devices"));
    QCOMPARE(organization.department().at(2),QLatin1String("Qt"));
}

void tst_QVersitContactImporter::testOrganizationTitle()
{
    QVersitDocument document(QVersitDocument::VCard30Type);
    QVersitProperty property;

    // One title
    property.setName(QLatin1String("TITLE"));
    QString titleValue(QLatin1String("Developer"));
    property.setValue(titleValue);
    document = createDocumentWithProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    QContact contact = mImporter->contacts().first();
    QList<QContactDetail> organizationDetails =
        contact.details(QContactOrganization::DefinitionName);
    QCOMPARE(organizationDetails.count(), 1);
    QContactOrganization organization = static_cast<QContactOrganization>(organizationDetails[0]);
    QCOMPARE(organization.title(),titleValue);

    // Two titles -> two QContactOrganizations created
    property.setName(QLatin1String("TITLE"));
    QString secondTitleValue(QLatin1String("Hacker"));
    property.setValue(secondTitleValue);
    document.addProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    organizationDetails = contact.details(QContactOrganization::DefinitionName);
    QCOMPARE(organizationDetails.count(), 2);
    QContactOrganization firstOrganization =
        static_cast<QContactOrganization>(organizationDetails[0]);
    QCOMPARE(firstOrganization.title(),titleValue);
    QContactOrganization secondOrganization =
        static_cast<QContactOrganization>(organizationDetails[1]);
    QCOMPARE(secondOrganization.title(),secondTitleValue);

    // Two titles and one organization name -> two QContactOrganizations created
    property.setName(QLatin1String("ORG"));
    property.setValueType(QVersitProperty::CompoundType);
    property.setValue(QStringList(QLatin1String("Nokia")));
    document.addProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    organizationDetails = contact.details(QContactOrganization::DefinitionName);
    QCOMPARE(organizationDetails.count(), 2);
    firstOrganization = static_cast<QContactOrganization>(organizationDetails[0]);
    QCOMPARE(firstOrganization.title(),titleValue);
    QCOMPARE(firstOrganization.name(),QLatin1String("Nokia"));
    secondOrganization = static_cast<QContactOrganization>(organizationDetails[1]);
    QCOMPARE(secondOrganization.title(),secondTitleValue);
    QCOMPARE(secondOrganization.name(),QString());
}

void tst_QVersitContactImporter::testOrganizationAssistant()
{
    QContact contact;
    QVersitDocument document(QVersitDocument::VCard30Type);
    QVersitProperty property;
    property.setName(QLatin1String("X-ASSISTANT"));
    QString assistantValue(QLatin1String("Jenny"));
    property.setValue(assistantValue);
    document = createDocumentWithProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    QContactOrganization organization = contact.detail<QContactOrganization>();
    QCOMPARE(organization.assistantName(), assistantValue);
}

void tst_QVersitContactImporter::testOrganizationLogo()
{
    QContact contact;
    QVersitDocument document(QVersitDocument::VCard30Type);
    QVersitProperty property;

    // Embedded LOGO
    property.setName(QLatin1String("LOGO"));
    QByteArray logo(QByteArray::fromBase64(
            "R0lGODlhEgASAIAAAAAAAP///yH5BAEAAAEALAAAAAASABIAAAIdjI+py+0G"));
    property.setValue(logo);
    property.insertParameter(QLatin1String("TYPE"),
                          QLatin1String("GIF"));
    document = createDocumentWithProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    QContactOrganization organization = contact.detail<QContactOrganization>();
    QByteArray content = mResourceHandler->mObjects.value(organization.logoUrl().toString());
    QCOMPARE(content, logo);

    // LOGO as a URL
    property.setName(QLatin1String("LOGO"));
    QString logoUrl(QLatin1String("http://www.organization.org/logo.gif"));
    property.setValue(logoUrl);
    property.insertParameter(QLatin1String("VALUE"),QLatin1String("URL"));
    document = createDocumentWithProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    organization = contact.detail<QContactOrganization>();
    QCOMPARE(organization.logoUrl().toString(),logoUrl);
}

void tst_QVersitContactImporter::testOrganizationRole()
{
    QContact contact;
    QVersitDocument document(QVersitDocument::VCard30Type);
    QVersitProperty property;

    // Setting the role is not yet supported by QContactOrganization
    property.setName(QLatin1String("ROLE"));
    QString roleValue(QLatin1String("Very important manager and proud of it"));
    property.setValue(roleValue);
    document = createDocumentWithProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    QContactOrganization organization = contact.detail<QContactOrganization>();
    QCOMPARE(organization.role(), roleValue);
}

void tst_QVersitContactImporter::testTel()
{
    QVersitDocument document(QVersitDocument::VCard30Type);
    QVersitProperty property;
    property.setName(QLatin1String("TEL"));
    QString value(QLatin1String("+35850987654321"));
    property.setValue(value);

    property.insertParameter(QLatin1String("TYPE"),QLatin1String("VOICE"));
    property.insertParameter(QLatin1String("TYPE"),QLatin1String("CELL"));
    property.insertParameter(QLatin1String("TYPE"),QLatin1String("MODEM"));
    property.insertParameter(QLatin1String("TYPE"),QLatin1String("CAR"));
    property.insertParameter(QLatin1String("TYPE"),QLatin1String("VIDEO"));
    property.insertParameter(QLatin1String("TYPE"),QLatin1String("FAX"));
    property.insertParameter(QLatin1String("TYPE"),QLatin1String("BBS"));
    property.insertParameter(QLatin1String("TYPE"),QLatin1String("PAGER"));
    property.insertParameter(QLatin1String("TYPE"),QLatin1String("HOME"));
    property.insertParameter(QLatin1String("TYPE"),QLatin1String("WORK"));

    document.addProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    QContact contact = mImporter->contacts().first();
    const QContactPhoneNumber& phone = contact.detail<QContactPhoneNumber>();
    QCOMPARE(phone.number(),QString(value));

    const QStringList subTypes = phone.subTypes();
    QCOMPARE(subTypes.count(),8);
    QVERIFY(subTypes.contains(QContactPhoneNumber::SubTypeVoice));
    QVERIFY(subTypes.contains(QContactPhoneNumber::SubTypeMobile));
    QVERIFY(subTypes.contains(QContactPhoneNumber::SubTypeModem));
    QVERIFY(subTypes.contains(QContactPhoneNumber::SubTypeCar));
    QVERIFY(subTypes.contains(QContactPhoneNumber::SubTypeVideo));
    QVERIFY(subTypes.contains(QContactPhoneNumber::SubTypeFax));
    QVERIFY(subTypes.contains(QContactPhoneNumber::SubTypeBulletinBoardSystem));
    QVERIFY(subTypes.contains(QContactPhoneNumber::SubTypePager));

    const QStringList contexts = phone.contexts();
    QCOMPARE(contexts.count(),2);
    QVERIFY(contexts.contains(QContactDetail::ContextWork));
    QVERIFY(contexts.contains(QContactDetail::ContextHome));
}

void tst_QVersitContactImporter::testEmail()
{
    QVersitProperty property;
    property.setName(QLatin1String("EMAIL"));
    QString value(QLatin1String("john.citizen@example.com"));
    property.setValue(value);
    property.insertParameter(QLatin1String("TYPE"),QLatin1String("WORK"));
    QVersitDocument document = createDocumentWithProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    QContact contact = mImporter->contacts().first();
    QContactEmailAddress email = contact.detail<QContactEmailAddress>();
    QCOMPARE(email.emailAddress(),value);
    const QStringList contexts = email.contexts();
    QCOMPARE(contexts.count(),1);
    QVERIFY(contexts.contains(QContactDetail::ContextWork));

    QCOMPARE(mPropertyHandler->mUnknownProperties.size(), 0);
}

void tst_QVersitContactImporter::testUrl()
{
    QVersitProperty property;
    property.setName(QLatin1String("URL"));
    QString value(QLatin1String("http://example.com"));
    property.setValue(value);
    property.insertParameter(QLatin1String("TYPE"),QLatin1String("WORK"));
    QVersitDocument document = createDocumentWithProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    QContact contact = mImporter->contacts().first();
    QContactUrl url = contact.detail<QContactUrl>();
    QCOMPARE(url.url(),value);
    const QStringList contexts = url.contexts();
    QCOMPARE(contexts.count(),1);
    QVERIFY(contexts.contains(QContactDetail::ContextWork));
}

void tst_QVersitContactImporter::testUid()
{
    QVersitProperty property;
    property.setName(QLatin1String("UID"));
    QString value(QLatin1String("unique identifier"));
    property.setValue(value);
    QVersitDocument document = createDocumentWithProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    QContact contact = mImporter->contacts().first();
    QContactGuid uid = contact.detail<QContactGuid>();
    QCOMPARE(uid.guid(),value);
}

void tst_QVersitContactImporter::testTimeStamp()
{
    // Simple date : ISO 8601 extended format
    QVersitProperty property;
    property.setName(QLatin1String("REV"));
    QString dateValue(QLatin1String("1981-05-20"));
    property.setValue(dateValue);
    QVersitDocument document = createDocumentWithProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    QContact contact = mImporter->contacts().first();
    QContactTimestamp timeStamp = contact.detail<QContactTimestamp>();
    QCOMPARE(timeStamp.lastModified().date().toString(Qt::ISODate),dateValue);

    // Date and Time : ISO 8601 extended format without utc offset
    QString dateAndTimeValue(QLatin1String("1981-05-20T23:55:55"));
    property.setValue(dateAndTimeValue);
    document = createDocumentWithProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    timeStamp = contact.detail<QContactTimestamp>();
    QCOMPARE(timeStamp.lastModified().toString(Qt::ISODate),dateAndTimeValue);

    // Date and Time : ISO 8601 extented format with utc offset
    QString utcOffset(QLatin1String("Z"));
    QString dateAndTimeWithUtcValue = dateAndTimeValue+utcOffset;
    property.setValue(dateAndTimeWithUtcValue);
    document = createDocumentWithProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    timeStamp = contact.detail<QContactTimestamp>();
    QCOMPARE(timeStamp.lastModified().toString(Qt::ISODate),dateAndTimeValue);
    QCOMPARE(timeStamp.lastModified().timeSpec(),Qt::UTC);

    // Date and Time : ISO 8601 in basic format without utc offset
    dateAndTimeValue = QLatin1String("19810520T235555");
    property.setValue(dateAndTimeValue);
    document = createDocumentWithProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    timeStamp = contact.detail<QContactTimestamp>();

    QCOMPARE(timeStamp.lastModified().toString(QLatin1String("yyyyMMddThhmmss")),
             dateAndTimeValue);

    // Date and Time : ISO 8601 in basic format with utc offset
    dateAndTimeValue = QLatin1String("19810520T235555");
    dateAndTimeWithUtcValue = dateAndTimeValue+utcOffset;
    property.setValue(dateAndTimeWithUtcValue);
    document = createDocumentWithProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    timeStamp = contact.detail<QContactTimestamp>();
    QCOMPARE(timeStamp.lastModified().toString(QLatin1String("yyyyMMddThhmmss")),
             dateAndTimeValue);
    QCOMPARE(timeStamp.lastModified().timeSpec(),Qt::UTC);
}

void tst_QVersitContactImporter::testAnniversary()
{
    // Date : ISO 8601 extended format
    QVersitProperty property;
    property.setName(QLatin1String("X-ANNIVERSARY"));
    property.setValue(QLatin1String("1981-05-20"));
    QVersitDocument document = createDocumentWithProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    QContact contact = mImporter->contacts().first();
    QContactAnniversary anniversary = contact.detail<QContactAnniversary>();
    QCOMPARE(anniversary.originalDate(), QDate(1981, 5, 20));

    // Evolution format
    property.setName(QLatin1String("X-EVOLUTION-ANNIVERSARY"));
    property.setValue(QLatin1String("1981-05-20"));
    document = createDocumentWithProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    anniversary = contact.detail<QContactAnniversary>();
    QCOMPARE(anniversary.originalDate(), QDate(1981, 5, 20));

    // Date : ISO 8601 in basic format
    property.setName(QLatin1String("X-ANNIVERSARY"));
    property.setValue(QLatin1String("19810520"));
    document = createDocumentWithProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    anniversary = contact.detail<QContactAnniversary>();
    QCOMPARE(anniversary.originalDate(), QDate(1981, 5, 20));
}

void tst_QVersitContactImporter::testBirthday()
{
    // Date : ISO 8601 extended format
    QVersitProperty property;
    property.setName(QLatin1String("BDAY"));
    QString dateValue(QLatin1String("1981-05-20"));
    property.setValue(dateValue);
    QVersitDocument document = createDocumentWithProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    QContact contact = mImporter->contacts().first();
    QContactBirthday bday = contact.detail<QContactBirthday>();
    QCOMPARE(bday.date().toString(Qt::ISODate),
             dateValue);

    // Date : ISO 8601 in basic format
    dateValue = QLatin1String("19810520");
    property.setValue(dateValue);
    document = createDocumentWithProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    bday = contact.detail<QContactBirthday>();
    QCOMPARE(bday.date().toString(QLatin1String("yyyyMMdd")),
             dateValue);

}

void tst_QVersitContactImporter::testGender()
{
    // Date : ISO 8601 extended format
    QVersitProperty property;
    property.setName(QLatin1String("X-GENDER"));
    QString val(QLatin1String("Male"));
    property.setValue(val);
    QVersitDocument document = createDocumentWithProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    QContact contact = mImporter->contacts().first();
    QContactGender  gender = contact.detail<QContactGender >();
    QCOMPARE(gender.gender(),val);
}

void tst_QVersitContactImporter::testNickname()
{
    // one value
    QVersitDocument document(QVersitDocument::VCard30Type);
    QVersitProperty nameProperty;
    nameProperty.setName(QLatin1String("NICKNAME"));
    nameProperty.setValue(QStringList(QLatin1String("Homie")));
    nameProperty.setValueType(QVersitProperty::ListType);
    document.addProperty(nameProperty);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    QContact contact = mImporter->contacts().first();
    QContactNickname nickName = (QContactNickname)contact.detail(QContactNickname::DefinitionName);
    QCOMPARE(nickName.nickname(), QLatin1String("Homie"));

    // comma separated values should generate multiple nickname fields
    contact.clearDetails();
    document.clear();
    document.setType(QVersitDocument::VCard30Type);
    QStringList multiVal;
    multiVal.append(QLatin1String("Homie"));
    multiVal.append(QLatin1String("SuperHero"));
    multiVal.append(QLatin1String("NukeSpecialist"));
    nameProperty.setName(QLatin1String("NICKNAME"));
    nameProperty.setValue(multiVal);
    nameProperty.setValueType(QVersitProperty::ListType);
    document.addProperty(nameProperty);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    QList<QContactNickname> nickNames = contact.details<QContactNickname>();
    QCOMPARE(nickNames.count(),3);
    nickName = nickNames[0];
    QCOMPARE(nickName.nickname(),QLatin1String("Homie"));
    nickName = nickNames[1];
    QCOMPARE(nickName.nickname(),QLatin1String("SuperHero"));
    nickName = nickNames[2];
    QCOMPARE(nickName.nickname(),QLatin1String("NukeSpecialist"));

    // X-NICKNAME
    document.clear();
    document.setType(QVersitDocument::VCard30Type);
    nameProperty = QVersitProperty();
    nameProperty.setName(QLatin1String("X-NICKNAME"));
    nameProperty.setValue(QStringList(QLatin1String("Homie")));
    nameProperty.setValueType(QVersitProperty::ListType);
    document.addProperty(nameProperty);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    nickName = contact.detail<QContactNickname>();
    QCOMPARE(nickName.nickname(),QLatin1String("Homie"));

    // both X-NICKNAME and NICKNAME
    nameProperty.setName(QLatin1String("NICKNAME"));
    document.addProperty(nameProperty);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    nickNames = contact.details<QContactNickname>();
    QCOMPARE(nickNames.size(), 1);
    QCOMPARE(nickNames.first().nickname(), QLatin1String("Homie"));
}

void tst_QVersitContactImporter::testAvatarThumbnail()
{
    QByteArray gif(SAMPLE_GIF);
    QString name = QLatin1String("John Citizen");
    QVersitDocument document = createDocumentWithNameAndPhoto(name, gif, QLatin1String("GIF"));
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    QContact contact = mImporter->contacts().first();
    QContactAvatar avatar = contact.detail<QContactAvatar>();
    QByteArray content = mResourceHandler->mObjects.value(avatar.imageUrl());
    QCOMPARE(content, gif);
    QContactThumbnail thumbnail = contact.detail<QContactThumbnail>();
    QImage image(thumbnail.thumbnail());
    QImage expectedImage;
    expectedImage.loadFromData(gif);
    QCOMPARE(image, expectedImage);

    // Without the resource handler, the thumbnail should still be set, but no avatar should be made
    mImporter->setResourceHandler(0);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    QCOMPARE(contact.details<QContactAvatar>().size(), 0);
    thumbnail = contact.detail<QContactThumbnail>();
    image = thumbnail.thumbnail();
    QCOMPARE(image, expectedImage);

    mImporter->setResourceHandler(mResourceHandler);

    // Empty photo.  The avatar should not be added to the QContact and the thumbnail will be empty.
    QVersitProperty property;
    property.setName(QLatin1String("PHOTO"));
    property.setValue(QByteArray());
    document.clear();
    document.setType(QVersitDocument::VCard30Type);
    document.addProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    QCOMPARE(contact.details<QContactAvatar>().size(), 0);
    thumbnail = contact.detail<QContactThumbnail>();
    QVERIFY(thumbnail.isEmpty());

    // Test multiple PHOTOs.  The chosen Thumbnail should be the smallest image supplied.
    // All should be made into Avatars
    QByteArray nonPhoto(QByteArray::fromBase64("UXQgaXMgZ3JlYXQh")); // the string "Qt is great!"
    QByteArray bigPhoto(NOKIA_GIF);
    document.clear();
    document.setType(QVersitDocument::VCard30Type);
    property.setName(QLatin1String("PHOTO"));
    property.setValue(nonPhoto); // shouldn't be the thumbnail because it's not an image
    document.addProperty(property);
    property.setValue(bigPhoto); // shouldn't be the thumbnail because it's not the smallest
    document.addProperty(property);
    property.setValue(gif); // should be the thumbnail
    document.addProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    QList<QContactThumbnail> thumbnails = contact.details<QContactThumbnail>();
    QCOMPARE(thumbnails.size(), 1);
    thumbnail = thumbnails.first();
    image = thumbnail.thumbnail();
    QCOMPARE(image, expectedImage);
    QCOMPARE(contact.details<QContactAvatar>().size(), 3);
}

void tst_QVersitContactImporter::testAvatarUrl()
{
    QVersitProperty property;
    property.setName(QLatin1String("PHOTO"));
    QString value(QLatin1String("http://example.com/example.jpg"));
    property.setValue(value);
    property.insertParameter(QLatin1String("VALUE"), QLatin1String("URL"));

    QVersitDocument document(QVersitDocument::VCard30Type);
    document.addProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    QContact contact = mImporter->contacts().first();
    QContactAvatar avatar = contact.detail<QContactAvatar>();
    QCOMPARE(avatar.imageUrl(), QUrl(QLatin1String("http://example.com/example.jpg")));


    // A URL disguised inside a QByteArray.
    document.clear();
    document.setType(QVersitDocument::VCard30Type);
    property.clear();
    property.setName(QLatin1String("PHOTO"));
    property.setValue(QByteArray("http://example.com/example.jpg"));
    property.insertParameter(QLatin1String("VALUE"), QLatin1String("URL"));
    property.insertParameter(QLatin1String("CHARSET"), QLatin1String("UTF-8"));
    document.addProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    avatar = contact.detail<QContactAvatar>();
    QCOMPARE(avatar.imageUrl(), QUrl(QLatin1String("http://example.com/example.jpg")));
}

void tst_QVersitContactImporter::testAvatarInvalid()
{
    // An avatar that's a QVersitDocument?  It shouldn't work.
    QVersitDocument document(QVersitDocument::VCard30Type);
    QVersitProperty property;
    property.setName(QLatin1String("PHOTO"));
    QVersitDocument nestedDocument;
    property.setValue(QVariant::fromValue(nestedDocument));
    property.insertParameter(QLatin1String("VALUE"), QLatin1String("URL"));
    document.addProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    QContact contact = mImporter->contacts().first();
    QCOMPARE(contact.details(QContactAvatar::DefinitionName).size(), 0);

    document.clear();
    document.setType(QVersitDocument::VCard30Type);
    property.clear();
    property.setName(QLatin1String("PHOTO"));
    property.setValue(QVariant::fromValue(nestedDocument));
    document.addProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    QCOMPARE(contact.details(QContactAvatar::DefinitionName).size(), 0);
}

void tst_QVersitContactImporter::testGeo()
{
    // some positive values
    QVersitDocument document(QVersitDocument::VCard30Type);
    QVersitProperty nameProperty;
    QStringList val;
    val.append(QLatin1String("18.53"));// Longtitude
    val.append(QLatin1String("45.32")); // Latitude
    nameProperty.setName(QLatin1String("GEO"));
    nameProperty.setValue(val);
    nameProperty.setValueType(QVersitProperty::CompoundType);
    document.addProperty(nameProperty);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    QContact contact = mImporter->contacts().first();
    QContactGeoLocation geo = (QContactGeoLocation)contact.detail(QContactGeoLocation::DefinitionName);
    QString str;
    str.setNum(geo.longitude(),'.',2);
    QCOMPARE(str,val[0]);
    str.setNum(geo.latitude(),'.',2);
    QCOMPARE(str,val[1]);

    // some negative values
    document.clear();
    document.setType(QVersitDocument::VCard30Type);
    nameProperty = QVersitProperty();
    val.append(QLatin1String("18.53"));// Longtitude
    val.append(QLatin1String("-45.32")); // Latitude
    nameProperty.setName(QLatin1String("GEO"));
    nameProperty.setValue(val);
    nameProperty.setValueType(QVersitProperty::CompoundType);
    document.addProperty(nameProperty);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    geo = (QContactGeoLocation)contact.detail(QContactGeoLocation::DefinitionName);
    str.setNum(geo.longitude(),'.',2);
    QCOMPARE(str,val[0]);
    str.setNum(geo.latitude(),'.',2);
    QCOMPARE(str,val[1]);
}

void tst_QVersitContactImporter::testNote()
{
    // single line value
    QVersitDocument document(QVersitDocument::VCard30Type);
    QVersitProperty nameProperty;
    QString val(QLatin1String("I will not sleep at my work -John"));
    nameProperty.setName(QLatin1String("NOTE"));
    nameProperty.setValue(val);
    document.addProperty(nameProperty);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    QContact contact = mImporter->contacts().first();
    QContactNote note = (QContactNote)contact.detail(QContactNote::DefinitionName);
    QCOMPARE(note.note(),val);

    // Multiline value and quoted printable encoding
    document.clear();
    document.setType(QVersitDocument::VCard30Type);
    nameProperty = QVersitProperty();
    val = QLatin1String("My Dad acts like he belongs,=0D=0AHe belongs in the zoo.=0D=0A");
    nameProperty.setName(QLatin1String("NOTE"));
    nameProperty.setValue(val);
    QMultiHash<QString,QString> params;
    params.insert(QLatin1String("QUOTED-PRINTABLE"),val);
    nameProperty.setParameters(params);
    document.addProperty(nameProperty);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    note = (QContactNote)contact.detail(QContactNote::DefinitionName);
    QCOMPARE(note.note(),val);
}

void tst_QVersitContactImporter::testCustomLabel()
{
    QVersitDocument document(QVersitDocument::VCard30Type);
    QVersitProperty nameProperty;
    QString val(QLatin1String("John Citizen"));
    nameProperty.setName(QLatin1String("FN"));
    nameProperty.setValue(val);
    document.addProperty(nameProperty);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    QContact contact = mImporter->contacts().first();
    QContactName name =
            (QContactName)contact.detail(QContactName::DefinitionName);
    QCOMPARE(name.customLabel(),val);
}

void tst_QVersitContactImporter::testDisplayLabel()
{
    QVersitDocument document(QVersitDocument::VCard30Type);
    QVersitProperty orgProperty;
    // ORG: should be used as last resort
    orgProperty.setName(QLatin1String("ORG"));
    orgProperty.setValue(QStringList(QLatin1String("org")));
    orgProperty.setValueType(QVersitProperty::CompoundType);
    document.addProperty(orgProperty);

    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    QContact contact = mImporter->contacts().first();
    QString displayLabel = contact.displayLabel();
    QCOMPARE(displayLabel, QLatin1String("org"));

    // NICKNAME: should be used if FN and N don't exist
    QVersitProperty nickProperty;
    nickProperty.setName(QLatin1String("NICKNAME"));
    nickProperty.setValue(QStringList(QLatin1String("nick")));
    nickProperty.setValueType(QVersitProperty::ListType);
    document.addProperty(nickProperty);

    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    displayLabel = contact.displayLabel();
    QCOMPARE(displayLabel, QLatin1String("nick"));

    // N: should be used in FN doesn't exist
    QVersitProperty nameProperty;
    nameProperty.setName(QLatin1String("N"));
    nameProperty.setValue(QStringList()
                          << QLatin1String("last")
                          << QLatin1String("first")
                          << QLatin1String("middle")
                          << QLatin1String("prefix")
                          << QLatin1String("suffix"));
    nameProperty.setValueType(QVersitProperty::CompoundType);
    document.addProperty(nameProperty);

    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    displayLabel = contact.displayLabel();
    QCOMPARE(displayLabel, QLatin1String("prefix first middle last suffix"));

    // FN: should be used if it exists
    QVersitProperty fnProperty;
    fnProperty.setName(QLatin1String("FN"));
    fnProperty.setValue(QLatin1String("fn"));
    document.addProperty(fnProperty);

    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    displayLabel = contact.displayLabel();
    QCOMPARE(displayLabel, QLatin1String("fn"));
}

void tst_QVersitContactImporter::testOnlineAccount()
{
    QString accountUri(QLatin1String("sip:john.citizen@example.com"));

    // Plain X-SIP, no TYPE ->
    QVersitDocument document(QVersitDocument::VCard30Type);
    QVersitProperty property;
    property.setName(QLatin1String("X-SIP"));
    property.setValue(accountUri);
    document.addProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    QContact contact = mImporter->contacts().first();
    QContactOnlineAccount onlineAccount = contact.detail<QContactOnlineAccount>();
    QCOMPARE(onlineAccount.accountUri(),accountUri);
    QStringList subTypes = onlineAccount.subTypes();
    QCOMPARE(subTypes.count(),1);
    QVERIFY(subTypes.first() == QContactOnlineAccount::SubTypeSip);

    // X-SIP;SWIS
    document.clear();
    document.setType(QVersitDocument::VCard30Type);
    property = QVersitProperty();
    property.setName(QLatin1String("X-SIP"));
    property.setValue(accountUri);
    QMultiHash<QString,QString> params;
    params.insert(QLatin1String("TYPE"),QLatin1String("SWIS"));
    property.setParameters(params);
    document.addProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    onlineAccount =  contact.detail<QContactOnlineAccount>();
    QCOMPARE(onlineAccount.accountUri(),accountUri);
    subTypes = onlineAccount.subTypes();
    QCOMPARE(subTypes.count(),1);
    QVERIFY(subTypes.first() == QContactOnlineAccount::SubTypeVideoShare);

    // X-SIP;VOIP
    document.clear();
    document.setType(QVersitDocument::VCard30Type);
    property = QVersitProperty();
    property.setName(QLatin1String("X-SIP"));
    property.setValue(accountUri);
    params.clear();
    params.insert(QLatin1String("TYPE"),QLatin1String("VOIP"));
    property.setParameters(params);
    document.addProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    onlineAccount =  contact.detail<QContactOnlineAccount>();
    QCOMPARE(onlineAccount.accountUri(),accountUri);
    subTypes = onlineAccount.subTypes();
    QCOMPARE(subTypes.count(),1);
    QVERIFY(subTypes.first() == QContactOnlineAccount::SubTypeSipVoip);

    // X-IMPP
    document.clear();
    document.setType(QVersitDocument::VCard30Type);
    property = QVersitProperty();
    property.setName(QLatin1String("X-IMPP"));
    property.setValue(accountUri);
    document.addProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    onlineAccount =  contact.detail<QContactOnlineAccount>();
    QCOMPARE(onlineAccount.accountUri(),accountUri);
    subTypes = onlineAccount.subTypes();
    QCOMPARE(subTypes.count(),1);
    QVERIFY(subTypes.first() == QContactOnlineAccount::SubTypeImpp);

    // IMPP
    document.clear();
    document.setType(QVersitDocument::VCard30Type);
    property = QVersitProperty();
    property.setName(QLatin1String("IMPP"));
    property.setValue(accountUri);
    document.addProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    onlineAccount =  contact.detail<QContactOnlineAccount>();
    QCOMPARE(onlineAccount.accountUri(),accountUri);
    subTypes = onlineAccount.subTypes();
    QCOMPARE(subTypes.count(),1);
    QVERIFY(subTypes.first() == QContactOnlineAccount::SubTypeImpp);

    // X-JABBER
    document.clear();
    document.setType(QVersitDocument::VCard30Type);
    property = QVersitProperty();
    property.setName(QLatin1String("X-JABBER"));
    property.setValue(accountUri);
    document.addProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    onlineAccount =  contact.detail<QContactOnlineAccount>();
    QCOMPARE(onlineAccount.accountUri(),accountUri);
    subTypes = onlineAccount.subTypes();
    QCOMPARE(subTypes.count(),1);
    QVERIFY(subTypes.first() == QContactOnlineAccount::SubTypeImpp);
}

void tst_QVersitContactImporter::testFamily()
{
    // Interesting : kid but no wife :)
    QVersitDocument document(QVersitDocument::VCard30Type);
    QVersitProperty nameProperty;
    QString val(QLatin1String("Jane")); // one is enough
    nameProperty.setName(QLatin1String("X-CHILDREN"));
    nameProperty.setValue(QStringList(val));
    nameProperty.setValueType(QVersitProperty::ListType);
    document.addProperty(nameProperty);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    QContact contact = mImporter->contacts().first();
    QContactFamily family = (QContactFamily)contact.detail(QContactFamily::DefinitionName);
    QStringList children = family.children();
    QCOMPARE(children.count(),1); // ensure no other kids in list
    QCOMPARE(family.spouse(),QString()); // make sure no wife
    QCOMPARE(children[0],val); // ensure it is your kid

    // Critical : wife but no kids , happy hours
    document.clear();
    document.setType(QVersitDocument::VCard30Type);
    nameProperty = QVersitProperty();
    nameProperty.setName(QLatin1String("X-SPOUSE"));
    val = QLatin1String("Jenny");
    nameProperty.setValue(val);
    document.addProperty(nameProperty);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    family = (QContactFamily)contact.detail(QContactFamily::DefinitionName);
    children = family.children();
    QCOMPARE(children.count(),0); // list should be empty as you know
    QCOMPARE(family.spouse(),val); // make sure thats your wife:(

    // Hopeless : couple of kids and wife
    document.clear();
    document.setType(QVersitDocument::VCard30Type);
    // Add kids first
    nameProperty = QVersitProperty();
    nameProperty.setName(QLatin1String("X-CHILDREN"));
    QStringList kidsVal;
    kidsVal.append(QLatin1String("James"));
    kidsVal.append(QLatin1String("Jake"));
    kidsVal.append(QLatin1String("Jane"));
    nameProperty.setValue(kidsVal);
    nameProperty.setValueType(QVersitProperty::ListType);
    document.addProperty(nameProperty);
    // Add wife next
    val = QLatin1String("Jenny");
    nameProperty = QVersitProperty();
    nameProperty.setName(QLatin1String("X-SPOUSE"));
    nameProperty.setValue(val);
    document.addProperty(nameProperty);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    family = (QContactFamily)contact.detail(QContactFamily::DefinitionName);
    children = family.children();
    QCOMPARE(children.count(),3); // too late , count them now.
    // painfull but ensure they are your kids
    QCOMPARE(children, kidsVal);
    QCOMPARE(family.spouse(),val); // make sure thats your wife:(
}

void tst_QVersitContactImporter::testSound()
{
    QVersitDocument document(QVersitDocument::VCard30Type);
    QVersitProperty soundProperty;
    QMultiHash<QString,QString> param;
    param.insert(QLatin1String("TYPE"),QLatin1String("WAVE"));
    soundProperty.setName(QLatin1String("SOUND"));
    QByteArray val("111110000011111");
    soundProperty.setValue(val);
    soundProperty.setParameters(param);
    document.addProperty(soundProperty);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    QContact contact = mImporter->contacts().first();
    QContactRingtone ringtone = contact.detail<QContactRingtone>();
    QByteArray content = mResourceHandler->mObjects.value(ringtone.audioRingtoneUrl());
    QCOMPARE(content, val);
}

void tst_QVersitContactImporter::testTag()
{
    // one value
    QVersitDocument document(QVersitDocument::VCard30Type);
    QVersitProperty tagProperty;
    tagProperty.setName(QLatin1String("CATEGORIES"));
    tagProperty.setValue(QStringList(QLatin1String("red")));
    tagProperty.setValueType(QVersitProperty::ListType);
    document.addProperty(tagProperty);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    QContact contact = mImporter->contacts().first();
    QContactTag tagDetail = contact.detail<QContactTag>();
    QCOMPARE(tagDetail.tag(), QLatin1String("red"));

    // multiple values
    tagProperty.setName(QLatin1String("CATEGORIES"));
    tagProperty.setValue(QStringList()
                         << QLatin1String("red")  // duplicate from previous property should be pruned
                         << QLatin1String("green")
                         << QLatin1String("blue")
                         << QLatin1String("blue"));  // duplicates should be pruned
    tagProperty.setValueType(QVersitProperty::ListType);
    document.addProperty(tagProperty);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    QList<QContactTag> tagDetails = contact.details<QContactTag>();
    QCOMPARE(tagDetails.count(), 3);
    QCOMPARE(tagDetails.at(0).tag(), QLatin1String("red"));
    QCOMPARE(tagDetails.at(1).tag(), QLatin1String("green"));
    QCOMPARE(tagDetails.at(2).tag(), QLatin1String("blue"));
}

void tst_QVersitContactImporter::testPref()
{
    QVersitDocument document(QVersitDocument::VCard30Type);
    QVersitProperty property1;
    property1.setName(QLatin1String("TEL"));
    property1.setValue(QLatin1String("1"));
    document.addProperty(property1);
    QVersitProperty property2;
    property2.setName(QLatin1String("TEL"));
    property2.setValue(QLatin1String("2"));
    property2.insertParameter(QLatin1String("TYPE"), QLatin1String("PREF"));
    document.addProperty(property2);
    QVersitProperty property3;
    property3.setName(QLatin1String("TEL"));
    property3.setValue(QLatin1String("3"));
    property3.insertParameter(QLatin1String("TYPE"), QLatin1String("PREF"));
    document.addProperty(property3);
    QVersitProperty property4;
    property4.setName(QLatin1String("TEL"));
    property4.setValue(QLatin1String("4"));
    document.addProperty(property4);

    // Test that pref details comes first.
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    QContact contact = mImporter->contacts().first();
    QContactPhoneNumber firstNumber = contact.detail<QContactPhoneNumber>();
    QCOMPARE(firstNumber.number(), QLatin1String("2"));
    QList<QContactPhoneNumber> numbers = contact.details<QContactPhoneNumber>();
    QCOMPARE(numbers.at(0).number(), QLatin1String("2"));
    QCOMPARE(numbers.at(1).number(), QLatin1String("3"));
    QCOMPARE(numbers.at(2).number(), QLatin1String("1"));
    QCOMPARE(numbers.at(3).number(), QLatin1String("4"));
}

void tst_QVersitContactImporter::testPropertyHandler()
{
    QVersitDocument document(QVersitDocument::VCard30Type);
    QVersitProperty property;

    // No unconverted properties, no converted properties either.  Fails with EmptyDocumentError
    QVERIFY(!mImporter->importDocuments(QList<QVersitDocument>() << document));
    QCOMPARE(mPropertyHandler->mUnknownProperties.size(), 0);
    QCOMPARE(mPropertyHandler->mPreProcessedProperties.size(), 0);
    QCOMPARE(mPropertyHandler->mPostProcessedProperties.size(), 0);

    // No unconverted properties, one converted property
    mPropertyHandler->clear();
    property.setName(QLatin1String("FN"));
    property.setValue(QLatin1String("John Citizen"));
    document.addProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    QContact contact = mImporter->contacts().first();
    QCOMPARE(mPropertyHandler->mUnknownProperties.size(), 0);
    QCOMPARE(mPropertyHandler->mPreProcessedProperties.size(), 1);
    QCOMPARE(mPropertyHandler->mPostProcessedProperties.size(), 1);

    // Set the handler to override handling of the property
    mPropertyHandler->clear();
    mPropertyHandler->mPreProcess = true;
    document.clear();
    document.setType(QVersitDocument::VCard30Type);
    property.setName(QLatin1String("FN"));
    property.setValue(QLatin1String("John Citizen"));
    document.addProperty(property);
    QVERIFY(mImporter->importDocuments(QList<QVersitDocument>() << document));
    contact = mImporter->contacts().first();
    QCOMPARE(mPropertyHandler->mUnknownProperties.size(), 0);
    QCOMPARE(mPropertyHandler->mPreProcessedProperties.size(), 1);
    QCOMPARE(mPropertyHandler->mPostProcessedProperties.size(), 0);
    QContactDetail nameDetail = contact.detail(QContactName::DefinitionName);
    QVERIFY(nameDetail.isEmpty());

    // One unknown property
    mPropertyHandler->clear();
    property.setName(QLatin1String("X-EXTENSION-1"));
    property.setValue(QLatin1String("extension value 1"));
    document.addProperty(property);
    mImporter->importDocuments(QList<QVersitDocument>() << document);
    QList<QVersitProperty> unknownProperties = mPropertyHandler->mUnknownProperties;
    QCOMPARE(unknownProperties.count(), 1);
    QCOMPARE(unknownProperties[0].name(), QLatin1String("X-EXTENSION-1"));
    QCOMPARE(unknownProperties[0].value(), QLatin1String("extension value 1"));

    // Two unknown properties
    mPropertyHandler->clear();
    property.setName(QLatin1String("X-EXTENSION-2"));
    property.setValue(QLatin1String("extension value 2"));
    document.addProperty(property);
    mImporter->importDocuments(QList<QVersitDocument>() << document);
    unknownProperties = mPropertyHandler->mUnknownProperties;
    QCOMPARE(unknownProperties.count(), 2);
    QCOMPARE(unknownProperties[0].name(), QLatin1String("X-EXTENSION-1"));
    QCOMPARE(unknownProperties[0].value(), QLatin1String("extension value 1"));
    QCOMPARE(unknownProperties[1].name(), QLatin1String("X-EXTENSION-2"));
    QCOMPARE(unknownProperties[1].value(), QLatin1String("extension value 2"));
}

void tst_QVersitContactImporter::testInvalidDocument()
{
    // invalid document (invalid type)
    QList<QVersitDocument> documents;
    QVersitDocument document(QVersitDocument::InvalidType);
    QVersitProperty nameProperty;
    nameProperty.setName(QLatin1String("FN"));
    nameProperty.setValue(QLatin1String("John Citizen"));
    document.addProperty(nameProperty);
    documents.append(document);

    // valid document in the same list
    QVersitProperty telProperty;
    telProperty.setName(QLatin1String("TEL"));
    telProperty.setValue(QLatin1String("1234"));
    document.addProperty(telProperty);
    document.setType(QVersitDocument::VCard21Type);
    documents.append(document);
    QVERIFY(!mImporter->importDocuments(documents));
    QMap<int, QVersitContactImporter::Error> errorMap = mImporter->errorMap();
    QCOMPARE(errorMap.size(), 1);
    QVERIFY(errorMap.contains(0));
    QVERIFY(errorMap.value(0) == QVersitContactImporter::InvalidDocumentError);
    QList<QContact> contacts = mImporter->contacts();
    QCOMPARE(contacts.size(), 1);
    QContactPhoneNumber phoneDetail = contacts.first().detail<QContactPhoneNumber>();
    QCOMPARE(phoneDetail.number(), QLatin1String("1234"));

    // empty document
    document.clear();
    document.setType(QVersitDocument::VCard21Type);
    QVERIFY(!mImporter->importDocuments(QList<QVersitDocument>() << document));
    errorMap = mImporter->errorMap();
    QCOMPARE(errorMap.size(), 1);
    QCOMPARE(errorMap.value(0), QVersitContactImporter::EmptyDocumentError);
    QCOMPARE(mImporter->errors(), errorMap);

}

QVersitDocument tst_QVersitContactImporter::createDocumentWithProperty(
    const QVersitProperty& property)
{
    QVersitDocument document(QVersitDocument::VCard30Type);
    document.addProperty(property);
    return document;
}

QVersitDocument tst_QVersitContactImporter::createDocumentWithNameAndPhoto(
    const QString& name,
    QByteArray image,
    const QString& imageType)
{
    QVersitDocument document(QVersitDocument::VCard30Type);

    QVersitProperty nameProperty;
    nameProperty.setName(QLatin1String("FN"));
    nameProperty.setValue(name);
    document.addProperty(nameProperty);

    QVersitProperty property;
    property.setName(QLatin1String("PHOTO"));
    property.setValue(image);
    if (imageType != QString()) {
        property.insertParameter(QLatin1String("TYPE"), imageType);
    }
    document.addProperty(property);

    return document;
}

QTEST_MAIN(tst_QVersitContactImporter)

