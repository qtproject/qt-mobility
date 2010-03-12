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

#include "tst_qversitcontactexporter.h"
#include "qversitcontactexporter.h"
#include "qversitcontactexporter_p.h"
#include "qversitproperty.h"
#include "qversitdefs_p.h"
#include <QString>
#include <QStringList>
#include <QList>
#include <QPixmap>
#include <QImageWriter>
#include <QtTest/QtTest>
#include <qcontact.h>
#include <qcontactaddress.h>
#include <qcontactemailaddress.h>
#include <qcontactname.h>
#include <qcontactphonenumber.h>
#include <qcontacturl.h>
#include <qcontactguid.h>
#include <qcontacttimestamp.h>
#include <qcontactbirthday.h>
#include <qcontactnote.h>
#include <qcontactgeolocation.h>
#include <qcontactorganization.h>
#include <qcontactavatar.h>
#include <qcontactgender.h>
#include <qcontactnickname.h>
#include <qcontactanniversary.h>
#include <qcontactonlineaccount.h>
#include <qcontactfamily.h>
#include <qcontactdisplaylabel.h>

QTM_BEGIN_NAMESPACE

class MyQVersitContactExporterDetailHandler : public QVersitContactExporterDetailHandler
{
public:
    MyQVersitContactExporterDetailHandler() : mPreProcess(false)
    {
    }

    bool preProcessDetail(const QContact& contact,
                          const QContactDetail& detail,
                          QVersitDocument* document)
    {
        Q_UNUSED(contact)
        Q_UNUSED(document)
        mPreProcessedDetails.append(detail);
        return mPreProcess;
    }

    bool postProcessDetail(const QContact& contact,
                           const QContactDetail& detail,
                           bool alreadyProcessed,
                           QVersitDocument* document)
    {
        Q_UNUSED(contact)
        Q_UNUSED(document)
        if (!alreadyProcessed)
            mUnknownDetails.append(detail);
        else
            mPostProcessedDetails.append(detail);
        return false;
    }

    void clear()
    {
        mPreProcess = false;
        mDefinitionNamesToProcess.clear();
        mUnknownDetails.clear();
        mPreProcessedDetails.clear();
        mPostProcessedDetails.clear();
    }

    // a hook to control what preProcess returns:
    bool mPreProcess;
    QStringList mDefinitionNamesToProcess;
    QList<QContactDetail> mUnknownDetails;
    QList<QContactDetail> mPreProcessedDetails;
    QList<QContactDetail> mPostProcessedDetails;
};

class MyQVersitResourceHandler : public QVersitResourceHandler
{
public:
    MyQVersitResourceHandler()
        : mLoadResourceCalled(false),
        mLoadSuccess(true)
    {
    }

    bool loadResource(const QString& location, QByteArray* contents, QString* mimeType)
    {
        mLocation = location;
        *contents = mSimulatedData;
        *mimeType = mSimulatedMimeType;
        mLoadResourceCalled = true;
        return mLoadSuccess;
    }

    bool saveResource(const QByteArray &contents, const QVersitProperty &property, QString *location)
    {
        Q_UNUSED(contents)
        Q_UNUSED(property)
        Q_UNUSED(location)
        return false;
    }

    void clear()
    {
        mSimulatedData.clear();
        mSimulatedMimeType.clear();
        mLocation.clear();
        mLoadResourceCalled = false;
        mLoadSuccess = true;
    }

    QByteArray mSimulatedData;
    QString mSimulatedMimeType;
    QString mLocation;
    bool mLoadResourceCalled;
    bool mLoadSuccess; // A hook to control what loadResource returns.
};

const static QByteArray SAMPLE_GIF(QByteArray::fromBase64(
        "R0lGODlhEgASAIAAAAAAAP///yH5BAEAAAEALAAAAAASABIAAAIdjI+py+0G"
        "wEtxUmlPzRDnzYGfN3KBaKGT6rDmGxQAOw=="));

QTM_END_NAMESPACE

QTM_USE_NAMESPACE

const QString TEST_PHOTO_FILE(QLatin1String("versitTest001.jpg"));
const QString TEST_AUDIO_FILE(QLatin1String("versitTest001.wav"));

void tst_QVersitContactExporter::init()
{
    mExporter = new QVersitContactExporter();
    mDetailHandler = new MyQVersitContactExporterDetailHandler;
    mExporter->setDetailHandler(mDetailHandler);
    mResourceHandler = new MyQVersitResourceHandler;
    mExporter->setResourceHandler(mResourceHandler);
}

void tst_QVersitContactExporter::cleanup()
{
    QVERIFY(mExporter->detailHandler() == mDetailHandler);
    mExporter->setDetailHandler(0);
    delete mDetailHandler;
    QVERIFY(mExporter->resourceHandler() == mResourceHandler);
    mExporter->setResourceHandler(0);
    delete mResourceHandler;
    delete mExporter;
}

void tst_QVersitContactExporter::testConvertContact()
{
    QContact contact;

    // Adding name to the contact
    QContactName name;
    name.setFirstName(QString::fromAscii("Moido"));
    contact.saveDetail(&name);

    // Adding phone number to the Contact.
    QContactPhoneNumber phoneNumber;
    phoneNumber.setNumber(QString::fromAscii("12345678"));
    contact.saveDetail(&phoneNumber);

    // Convert contact into versit properties
    QList<QContact> list;
    list.append(contact);
    QList<QVersitDocument> documents = mExporter->exportContacts(list);

    // Each Contact has display label detail by default. Display label is enocded
    // if some value exisit for the Label or if value for Name exisit.
    QCOMPARE(documents.size(), 1);
    QCOMPARE(documents.first().properties().count(), 3);
}

void tst_QVersitContactExporter::testContactDetailHandler()
{
    // Test1: Un-supported Avatar Test
    QContact contact;
    QVersitDocument document;
    QContactAvatar contactAvatar;
    contactAvatar.setSubType(QContactAvatar::SubTypeTexturedMesh);
    contact.saveDetail(&contactAvatar);
    QList<QContact> contacts;
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 0);
    QList<QContactDetail> unknownDetails = mDetailHandler->mUnknownDetails;
    QVERIFY(unknownDetails.size() > 0);
    QString definitionName = contactAvatar.definitionName();
    QContactDetail detail = searchDetail(unknownDetails,definitionName);
    QCOMPARE(definitionName, detail.definitionName());

    // Test2: Un-supported Online Account
    QContactOnlineAccount onlineAccount;
    QString testUri = QString::fromAscii("sip:abc@temp.com");
    onlineAccount.setAccountUri(testUri);
    onlineAccount.setSubTypes(QString::fromAscii("unsupported"));
    contact.saveDetail(&onlineAccount);
    contacts.clear();
    contacts.append(contact);
    mDetailHandler->clear();
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 0);
    unknownDetails = mDetailHandler->mUnknownDetails;
    QVERIFY(unknownDetails.size() > 0);
    definitionName = onlineAccount.definitionName();
    detail = searchDetail(unknownDetails, definitionName);
    QCOMPARE(definitionName, detail.definitionName());

    // Test that preProcessDetail return true stops the exporter from doing anything.
    contact.clearDetails();
    QContactName contactName;
    contactName.setFirstName(QLatin1String("John"));
    contact.saveDetail(&contactName);
    mDetailHandler->clear();
    mDetailHandler->mPreProcess = true;
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 0);
    QVERIFY(mDetailHandler->mPreProcessedDetails.count() > 0);
    QCOMPARE(mDetailHandler->mPostProcessedDetails.count(), 0);
    QCOMPARE(mDetailHandler->mUnknownDetails.count(), 0);

    QVERIFY(mExporter->detailHandler() == mDetailHandler);
}

void tst_QVersitContactExporter::testEncodeName()
{
    QContact contact;
    QContactName name;

    // vCard 2.1
    name.setFirstName(QString::fromAscii("Heido"));
    name.setLastName(QString::fromAscii("HH"));
    name.setMiddleName(QString::fromAscii("A"));
    name.setPrefix(QString::fromAscii("Mr."));
    name.setContexts(QContactDetail::ContextHome);
    contact.saveDetail(&name);
    QList<QContact> contacts;
    contacts.append(contact);
    QVersitDocument document = mExporter->exportContacts(contacts).first();

    // Each Contact has display label detail by default. Display label is enocded
    // if some value exisit for the Label or if value for Name exisit.
    QCOMPARE(document.properties().count(), 2);

    QVersitProperty displayProperty = document.properties().at(0);
    // Check name
    QCOMPARE(displayProperty.name(), QString::fromAscii("FN"));
    // Check value
    QCOMPARE(displayProperty.value(), QString::fromAscii("Heido HH"));

    QVersitProperty nameProperty = document.properties().at(1);
    // Check parameters, contexts not allowed for N property
    QCOMPARE(nameProperty.parameters().count(), 0);
    // Check name
    QCOMPARE(nameProperty.name(), QString::fromAscii("N"));
    // Check value
    QCOMPARE(nameProperty.value(), QString::fromAscii("HH;Heido;A;Mr.;"));

    // vCard 3.0, special characters in the name parts are backslash escaped
    contact.removeDetail(&name);
    name.setFirstName(QString::fromAscii("Hom,er"));
    name.setLastName(QString::fromAscii("Simp;son"));
    name.setMiddleName(QString::fromAscii("J;"));
    name.setPrefix(QString::fromAscii(";Mr."));
    name.setSuffix(QString::fromAscii("Sir,"));
    contact.saveDetail(&name);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts, QVersitDocument::VCard30Type).first();
    QCOMPARE(document.type(),QVersitDocument::VCard30Type);

    // Each Contact has display label detail by default. Display label is enocded
    // if some value exists for the Label or if value for Name exisit.
    QCOMPARE(document.properties().count(), 2);
    displayProperty = document.properties().at(0);
    nameProperty = document.properties().at(1);
    // Check parameters
    QCOMPARE(displayProperty.parameters().count(), 0);
    QCOMPARE(nameProperty.parameters().count(), 0);
    // Check name
    QCOMPARE(displayProperty.name(), QString::fromAscii("FN"));
    QCOMPARE(nameProperty.name(), QString::fromAscii("N"));
    // Check value

    QCOMPARE(displayProperty.value(), QString::fromAscii("Hom\\,er Simp\\;son"));

    QCOMPARE(nameProperty.value(),
             QString::fromAscii("Simp\\;son;Hom\\,er;J\\;;\\;Mr.;Sir\\,"));
}

void tst_QVersitContactExporter::testEncodePhoneNumber()
{
    QContact contact;
    QContactPhoneNumber phoneNumber;
    phoneNumber.setNumber(QString::fromAscii("12345678"));
    phoneNumber.setContexts(QContactDetail::ContextHome);
    phoneNumber.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    contact.saveDetail(&phoneNumber);
    QList<QContact> contacts;
    contacts.append(contact);
    QVersitDocument document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    // Check parameters
    QCOMPARE(property.parameters().count(), 2);
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("HOME")));
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("CELL")));
    // Check name
    QCOMPARE(property.name(), QString::fromAscii("TEL"));
    // Check value
    QCOMPARE(property.value(), phoneNumber.number());
}

void tst_QVersitContactExporter::testEncodeEmailAddress()
{
    QContact contact;
    QContactEmailAddress email;
    email.setEmailAddress(QString::fromAscii("test@test"));
    email.setContexts(QContactDetail::ContextHome);
    contact.saveDetail(&email);
    QList<QContact> contacts;
    contacts.append(contact);
    QVersitDocument document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    // Check parameters
    QCOMPARE(property.parameters().count(), 1);
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("HOME")));
    // Check name
    QCOMPARE(property.name(), QString::fromAscii("EMAIL"));
    // Check value
    QCOMPARE(property.value(), email.emailAddress());
}

void tst_QVersitContactExporter::testEncodeStreetAddress()
{
    QContact contact;
    QContactAddress address;

    // vCard 2.1
    address.setCountry(QString::fromAscii("Finland"));
    address.setPostcode(QString::fromAscii("00440"));
    address.setStreet(QString::fromAscii("HKKI 1X 90"));
    address.setLocality(QString::fromAscii("Helsinki"));
    address.setContexts(QContactDetail::ContextHome);
    address.setSubTypes(QContactAddress::SubTypePostal);
    contact.saveDetail(&address);
    QList<QContact> contacts;
    contacts.append(contact);
    QVersitDocument document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    // Check parameters
    QCOMPARE(property.parameters().count(), 2);
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("HOME")));
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("POSTAL")));
    // Check name
    QCOMPARE(property.name(), QString::fromAscii("ADR"));
    // Check value
    QCOMPARE(property.value(), QString::fromAscii(";;HKKI 1X 90;Helsinki;;00440;Finland"));

    // vCard 3.0, special characters in the address parts are backslash escaped
    contact.removeDetail(&address);
    address.setPostOfficeBox(QString::fromAscii("PO;Box"));
    address.setStreet(QString::fromAscii("My;Street"));
    address.setLocality(QString::fromAscii("My;Town"));
    address.setRegion(QString::fromAscii("My;State"));
    address.setPostcode(QString::fromAscii("12345;"));
    address.setCountry(QString::fromAscii("My;Country"));
    contact.saveDetail(&address);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts, QVersitDocument::VCard30Type).first();
    QCOMPARE(document.type(),QVersitDocument::VCard30Type);
    QCOMPARE(document.properties().count(), 1);
    property = document.properties().at(0);
    // Check name
    QCOMPARE(property.name(), QString::fromAscii("ADR"));
    // Check value
    QCOMPARE(property.value(),
             QString::fromAscii("PO\\;Box;;My\\;Street;My\\;Town;My\\;State;12345\\;;My\\;Country"));
}

void tst_QVersitContactExporter::testEncodeUrl()
{
    QContact contact;
    QContactUrl url;
    url.setUrl(QString::fromAscii("http://www.myhome.com"));
    url.setContexts(QContactDetail::ContextHome);
    url.setSubType(QContactUrl::SubTypeHomePage);
    contact.saveDetail(&url);
    QList<QContact> contacts;
    contacts.append(contact);
    QVersitDocument document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    // Check parameters
    QCOMPARE(property.parameters().count(), 1);
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("HOME")));
    // Check name
    QCOMPARE(property.name(), QString::fromAscii("URL"));
    // Check value
    QCOMPARE(property.value(), url.url());
}

void tst_QVersitContactExporter::testEncodeUid()
{
    QContact contact;
    QContactGuid guid;

    // vCard 2.1
    guid.setContexts(QContactDetail::ContextHome);
    guid.setGuid(QString::fromAscii("0101222"));
    contact.saveDetail(&guid);
    QList<QContact> contacts;
    contacts.append(contact);
    QVersitDocument document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    // Check parameters
    // Contexts are not allowed for UID
    QCOMPARE(property.parameters().count(), 0);
    // Check name
    QCOMPARE(property.name(), QString::fromAscii("UID"));
    // Check value
    QCOMPARE(property.value(), guid.guid());

    // vCard 3.0, special characters in the value are backslash escaped
    contact.removeDetail(&guid);
    guid.setGuid(QString::fromAscii("1;2,3\r\n4\\5"));
    contact.saveDetail(&guid);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts, QVersitDocument::VCard30Type).first();
    QCOMPARE(document.type(),QVersitDocument::VCard30Type);
    QCOMPARE(document.properties().count(), 1);
    property = document.properties().at(0);
    // Check name
    QCOMPARE(property.name(), QString::fromAscii("UID"));
    // Check value
    QCOMPARE(property.value(), QString::fromAscii("1\\;2\\,3\\n4\\\\5"));
}

void tst_QVersitContactExporter::testEncodeRev()
{
    QContact contact;
    QContactTimestamp timeStamp;

    // Last modified time found
    QDateTime revisionTime =
        QDateTime::fromString(
            QString::fromAscii("M1d1y200906:01:02"),
            QString::fromAscii("'M'M'd'd'y'yyyyhh:mm:ss"));
    revisionTime.setTimeSpec(Qt::UTC);
    timeStamp.setLastModified(revisionTime);
    // Contexts not allowed in REV property, check that they are not added
    timeStamp.setContexts(QContactDetail::ContextHome);
    contact.saveDetail(&timeStamp);
    QList<QContact> contacts;
    contacts.append(contact);
    QVersitDocument document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    QCOMPARE(property.parameters().count(), 0);
    QCOMPARE(property.name(), QString::fromAscii("REV"));
    QString expectedValueUTCEncoded = QString::fromAscii("2009-01-01T06:01:02Z");
    QCOMPARE(property.value(), expectedValueUTCEncoded);

    // Last modified time not found, use the creation time
    QDateTime emptyTime;
    timeStamp.setLastModified(emptyTime);
    timeStamp.setCreated(revisionTime);
    contact.saveDetail(&timeStamp);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 1);
    property = document.properties().at(0);
    QCOMPARE(property.value(), expectedValueUTCEncoded);

    // Last modified time found, Local Time spec not UTC
    QDateTime localTime;
    timeStamp.setLastModified(revisionTime);
    timeStamp.setCreated(localTime);
    revisionTime.setTimeSpec(Qt::LocalTime);
    contact.saveDetail(&timeStamp);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 1);
    property = document.properties().at(0);
    QString expectedValueEncoded = QString::fromAscii("2009-01-01T06:01:02");
    QCOMPARE(property.value(), expectedValueUTCEncoded);

    // Last modified time not found, creation time not found
    timeStamp.setLastModified(emptyTime);
    timeStamp.setCreated(emptyTime);
    contact.saveDetail(&timeStamp);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 0);
}

void tst_QVersitContactExporter::testEncodeBirthDay()
{
    QContact contact;
    QDate date(2009,1,1);
    QContactBirthday birthDay;
    birthDay.setDate(date);
    // Contexts not allowed in BDAY property, check that they are not added
    birthDay.setContexts(QContactDetail::ContextHome);
    contact.saveDetail(&birthDay);
    QList<QContact> contacts;
    contacts.append(contact);
    QVersitDocument document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    QCOMPARE(property.parameters().count(), 0);
    QCOMPARE(property.name(), QString::fromAscii("BDAY"));
    QCOMPARE(property.value(), QString::fromAscii("2009-01-01"));
}

void tst_QVersitContactExporter::testEncodeNote()
{
    QContact contact;
    QContactNote note;
    note.setNote(QString::fromAscii("My Note"));
    // Contexts not allowed in NOTE property, check that they are not added
    note.setContexts(QContactDetail::ContextHome);
    contact.saveDetail(&note);
    QList<QContact> contacts;
    contacts.append(contact);
    QVersitDocument document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    QCOMPARE(property.parameters().count(), 0);
    QCOMPARE(property.name(), QString::fromAscii("NOTE"));
    QCOMPARE(property.value(), note.note());
}

void tst_QVersitContactExporter::testEncodeGeoLocation()
{
    QContact contact;
    QContactGeoLocation geoLocation;
    QString longitude = QString::fromAscii("99.9");
    geoLocation.setLongitude(longitude.toDouble());
    QString latitude = QString::fromAscii("98.9");
    geoLocation.setLatitude(latitude.toDouble());
    // Contexts not allowed in GEO property, check that they are not added
    geoLocation.setContexts(QContactDetail::ContextHome);
    contact.saveDetail(&geoLocation);
    QList<QContact> contacts;
    contacts.append(contact);
    QVersitDocument document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    QCOMPARE(property.parameters().count(), 0);
    QCOMPARE(property.name(), QString::fromAscii("GEO"));
    QString expectedValue = longitude + QString::fromAscii(",") + latitude;
    QCOMPARE(property.value(), expectedValue);
}

void tst_QVersitContactExporter::testEncodeOrganization()
{
    QList<QContact> contacts;
    QContact contact;
    QContactOrganization organization;
    QVersitDocument document;
    QVersitProperty property;
    QString title(QString::fromAscii("Developer"));
    QString organizationName(QString::fromAscii("Nokia"));
    QString department(QString::fromAscii("R&D"));

    // TITLE
    organization.setTitle(title);
    contact.saveDetail(&organization);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 1);
    property = document.properties().at(0);
    QCOMPARE(property.name(), QString::fromAscii("TITLE"));
    QCOMPARE(property.value(), title);

    // ORG with name
    organization.setTitle(QString());
    organization.setName(QString::fromAscii("Nokia"));
    contact.saveDetail(&organization);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 1);
    property = document.properties().at(0);
    QCOMPARE(property.name(), QString::fromAscii("ORG"));
    QCOMPARE(property.value(), QString::fromAscii("Nokia;"));

    // ORG with department/unit
    organization.setName(QString());
    QStringList departments(QString::fromAscii("R&D"));
    departments.append(QString::fromAscii("Qt"));
    organization.setDepartment(departments);
    contact.saveDetail(&organization);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 1);
    property = document.properties().at(0);
    QCOMPARE(property.name(), QString::fromAscii("ORG"));
    QCOMPARE(property.value(), QString::fromAscii(";R&D;Qt"));

    // ORG with name and department/unit
    organization.setName(QString::fromAscii("Nokia"));
    contact.saveDetail(&organization);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 1);
    property = document.properties().at(0);
    QCOMPARE(property.name(), QString::fromAscii("ORG"));
    QCOMPARE(property.value(), QString::fromAscii("Nokia;R&D;Qt"));

    // TITLE and ORG
    organization.setTitle(QString::fromAscii("Developer"));
    contact.saveDetail(&organization);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 2);
    property = document.properties().at(0);
    QCOMPARE(property.name(), QString::fromAscii("TITLE"));
    QCOMPARE(property.value(), title);
    property = document.properties().at(1);
    QCOMPARE(property.name(), QString::fromAscii("ORG"));
    QCOMPARE(property.value(), QString::fromAscii("Nokia;R&D;Qt"));

    // ORG LOGO Test1: LOGO as remote Resouce
    const QString url = QString::fromAscii("http://myhome.com/test.jpg");
    contact = QContact();
    organization = QContactOrganization();
    organization.setLogo(url);
    contact.saveDetail(&organization);
    contacts.clear();
    contacts.append(contact);
    mResourceHandler->mSimulatedMimeType = QLatin1String("image/jpeg");
    mExporter->setResourceHandler(mResourceHandler);
    document = mExporter->exportContacts(contacts).first();
    QVERIFY(!mResourceHandler->mLoadResourceCalled);

    // Source type is encoded, but media type is not for a URL.
    QCOMPARE(document.properties().at(0).parameters().count(), 1);

    QVERIFY(document.properties().at(0).parameters().contains(
            QString::fromAscii("VALUE"), QString::fromAscii("URL")));

    //Check property Name
    QString propertyName = document.properties().at(0).name();
    QCOMPARE(propertyName, QString::fromAscii("LOGO"));

    //Check property value
    QString value = document.properties().at(0).value();
    QCOMPARE(value, url);

    // ORG LOGO Test2: LOGO File.
    mResourceHandler->mSimulatedData = "simulated data";
    contact = QContact();
    organization = QContactOrganization();
    organization.setLogo(TEST_PHOTO_FILE);
    contact.saveDetail(&organization);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QVERIFY(mResourceHandler->mLoadResourceCalled);
    QCOMPARE(mResourceHandler->mLocation, TEST_PHOTO_FILE);

    // It should be stored in the property as a QVariant of QByteArray
    property = document.properties().at(0);
    QMultiHash<QString,QString> parameters = property.parameters();
    // Media type is encoded
    QCOMPARE(parameters.count(), 1);
    QVERIFY(parameters.contains(
            QString::fromAscii("TYPE"), QString::fromAscii("JPEG")));
    // Verify value.
    QVariant variantValue = property.variantValue();
    QVERIFY(variantValue.type() == QVariant::ByteArray);
    QCOMPARE(variantValue.value<QByteArray>(), mResourceHandler->mSimulatedData);

    // Assistant Name Test.
    contact = QContact();
    organization = QContactOrganization();
    organization.setAssistantName(QString::fromAscii("myAssistant"));
    contact.saveDetail(&organization);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 1);
    property = document.properties().at(0);
    QCOMPARE(property.name(), QString::fromAscii("X-ASSISTANT"));
    QCOMPARE(property.value(), QString::fromAscii("myAssistant"));

    // Test: Role
    contact = QContact();
    organization = QContactOrganization();
    organization.setRole(QString::fromAscii("Executive"));
    contact.saveDetail(&organization);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 1);
    property = document.properties().at(0);
    QCOMPARE(property.name(), QString::fromAscii("ROLE"));
    QCOMPARE(property.value(), QString::fromAscii("Executive"));

}

void tst_QVersitContactExporter::testEncodeAvatar()
{
    QContact contact;
    QContactAvatar contactAvatar;
    QPixmap pixmap;
    pixmap.loadFromData(SAMPLE_GIF);
    mResourceHandler->mSimulatedData = "simulated data";
    mResourceHandler->mSimulatedMimeType = QLatin1String("image/jpeg");

    // Test1: Web URL
    const QString url = QString::fromAscii("http://www.myhome.com/test.jpg");
    contactAvatar.setAvatar(url);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    QList<QContact> contacts;
    contacts.append(contact);
    QVersitDocument document = mExporter->exportContacts(contacts).first();
    QVERIFY(document.properties().length() > 0);
    QVersitProperty property = document.properties().at(0);
    QCOMPARE(property.parameters().count(), 1);
    QVERIFY(!mResourceHandler->mLoadResourceCalled);

    // Test 2: Local Media PHOTO
    contactAvatar.setAvatar(TEST_PHOTO_FILE);
    contactAvatar.setPixmap(pixmap); // Should be ignored if the file can be loaded.
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QVERIFY(mResourceHandler->mLoadResourceCalled);
    QCOMPARE(mResourceHandler->mLocation, TEST_PHOTO_FILE);
    // verify the value
    QVERIFY(document.properties().length() > 0);
    property = document.properties().at(0);
    QVariant variantValue = property.variantValue();
    QVERIFY(variantValue.type() == QVariant::ByteArray);
    QCOMPARE(variantValue.value<QByteArray>(), mResourceHandler->mSimulatedData);
    QVERIFY(property.parameters().contains(QString::fromAscii("TYPE"),
                                           QString::fromAscii("JPEG")));

    // Test3: UnSupported Media Type, properties and parameters are not encoded
    mResourceHandler->clear();
    const QString testUrl2 = QString::fromAscii("http://www.myhome.com/test.jpg");
    contactAvatar.setAvatar(testUrl2);
    // un-supported media type is encoded
    contactAvatar.setSubType(QContactAvatar::SubTypeTexturedMesh);
    contact.saveDetail(&contactAvatar);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 0);
    QVERIFY(!mResourceHandler->mLoadResourceCalled);

    // Test 4: Load resource fails but there is a pixmap.  The pixmap should be saved.
    // This feature is only supported if we can write PNGs.
    if (QImageWriter::supportedImageFormats().contains("png")) {
        mResourceHandler->clear();
        mResourceHandler->mLoadSuccess = false;
        contactAvatar.setAvatar(QLatin1String(""));
        contactAvatar.setSubType(QContactAvatar::SubTypeImage);
        contactAvatar.setPixmap(pixmap);
        contact.saveDetail(&contactAvatar);
        contacts.clear();
        contacts.append(contact);
        document = mExporter->exportContacts(contacts).first();
        // verify the value
        QVERIFY(document.properties().length() > 0);
        property = document.properties().at(0);
        variantValue = property.variantValue();
        QVERIFY(variantValue.type() == QVariant::ByteArray);
        QByteArray retrievedData = variantValue.value<QByteArray>();
        QPixmap retrievedPixmap;
        retrievedPixmap.loadFromData(retrievedData);
        QCOMPARE(retrievedPixmap, pixmap);
    }
}


void tst_QVersitContactExporter::testEncodeEmbeddedContent()
{
    QContact contact;
    QContactAvatar contactAvatar;
    QVariant variantValue;

    // Test 1: URL
    const QString url = QString::fromAscii("http://www.myhome.com/test.jpg");
    contactAvatar.setAvatar(url);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    QList<QContact> contacts;
    contacts.append(contact);
    mResourceHandler->mSimulatedMimeType = QLatin1String("image/jpeg");
    QVersitDocument document = mExporter->exportContacts(contacts).first();
    QVERIFY(!mResourceHandler->mLoadResourceCalled);
    QVersitProperty photoProperty = document.properties().at(0);
    QCOMPARE(photoProperty.parameters().count(), 1);
    QVERIFY(photoProperty.parameters().contains(
            QString::fromAscii("VALUE"),QString::fromAscii("URL")));
    QCOMPARE(photoProperty.name(), QString::fromAscii("PHOTO"));
    QCOMPARE(photoProperty.value(), url);

    // Test 2: Local PHOTO, image loaded by the loader
    contactAvatar.setAvatar(TEST_PHOTO_FILE);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    contacts.clear();
    contacts.append(contact);
    mResourceHandler->clear();
    mResourceHandler->mSimulatedMimeType = QLatin1String("image/jpeg");
    mResourceHandler->mSimulatedData = "simulated image data";
    document = mExporter->exportContacts(contacts).first();
    QVERIFY(mResourceHandler->mLoadResourceCalled);
    photoProperty = document.properties().at(0);
    QCOMPARE(photoProperty.parameters().count(), 1);
    QVERIFY(photoProperty.parameters().contains(QString::fromAscii("TYPE"),
                                                QString::fromAscii("JPEG")));
    variantValue = photoProperty.variantValue();
    QVERIFY(variantValue.type() == QVariant::ByteArray);
    QCOMPARE(variantValue.value<QByteArray>(), mResourceHandler->mSimulatedData);

    // Test 3: Local SOUND
    mResourceHandler->clear();
    mResourceHandler->mSimulatedMimeType = QLatin1String("audio/wav");
    mResourceHandler->mSimulatedData = "simulated audio data";
    contactAvatar.setAvatar(TEST_AUDIO_FILE);
    contactAvatar.setSubType(QContactAvatar::SubTypeAudioRingtone);
    contact.saveDetail(&contactAvatar);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QVERIFY(mResourceHandler->mLoadResourceCalled);
    QVersitProperty soundProperty = document.properties().at(0);
    QCOMPARE(soundProperty.parameters().count(), 1);
    QVERIFY(soundProperty.parameters().contains(
        QString::fromAscii("TYPE"),
        QString::fromAscii("WAV")));
    variantValue = soundProperty.variantValue();
    QVERIFY(variantValue.type() == QVariant::ByteArray);
    QCOMPARE(variantValue.value<QByteArray>(), mResourceHandler->mSimulatedData);

    // Test 4: Unsupported media type, properties and parameters are not encoded
    mResourceHandler->clear();
    mResourceHandler->mSimulatedMimeType = QLatin1String("text/jpeg");
    const QString testUrl2 = QString::fromAscii("http://www.myhome.com/test.jpg");
    contactAvatar.setAvatar(testUrl2);
    // un-supported media type is encoded
    contactAvatar.setSubType(QContactAvatar::SubTypeTexturedMesh);
    contact.saveDetail(&contactAvatar);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 0);
    QVERIFY(!mResourceHandler->mLoadResourceCalled);

    // Without a resource handler
    mExporter->setResourceHandler(0);
    contactAvatar.setAvatar(TEST_PHOTO_FILE);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 0);

    mExporter->setResourceHandler(mResourceHandler);
}

void tst_QVersitContactExporter::testEncodeParameters()
{
    QContact contact;
    QContactPhoneNumber phoneNumber;
    phoneNumber.setNumber(QString::fromAscii("12345678"));
    QStringList subtypes;
    subtypes.append(QContactPhoneNumber::SubTypeMobile);
    subtypes.append(QContactPhoneNumber::SubTypeVideo);
    // Add a not supported subtype in vCard, to make sure its not encoded.
    subtypes.append(QContactPhoneNumber::SubTypeDtmfMenu);
    phoneNumber.setSubTypes(subtypes);
    contact.saveDetail(&phoneNumber);
    QList<QContact> contacts;
    contacts.append(contact);
    QVersitDocument document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    QCOMPARE(property.parameters().count(), 2);
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"), QString::fromAscii("CELL")));
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("VIDEO")));
}

void tst_QVersitContactExporter::testIsValidRemoteUrl()
{
    QContact contact;
    QContactAvatar contactAvatar;
    mResourceHandler->mLoadSuccess = false;

    // Test1: http URL
    QString url = QString::fromAscii("http://www.nonoh.com/test.jpg");
    contactAvatar.setAvatar(url);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    QList<QContact> contacts;
    contacts.append(contact);
    QVersitDocument document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 1);

    // Test2: FTP URL
    url = QString::fromAscii("ftp://nonoh.com/test.jpg");
    contactAvatar.setAvatar(url);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 1);

    // Test3: NEW Protocol URL
    url = QString::fromAscii("myProtocol://nonoh.com/test.jpg");
    contactAvatar.setAvatar(url);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 1);

    // Test4: URL without scheme
    url = QString::fromAscii("www.nonoh.com/test.jpg");
    contactAvatar.setAvatar(url);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 1);

    // Test5: File Name but File does not Exisit
    url = QString::fromAscii("c:/filedoesnotexisit.jok");
    contactAvatar.setAvatar(url);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 0);
}

void tst_QVersitContactExporter::testEncodeGender()
{
    QContact contact;
    QContactGender gender;
    gender.setGender(QContactGender::GenderMale);
    gender.setContexts(QContactGender::ContextHome); // Should not be encoded
    contact.saveDetail(&gender);
    QList<QContact> contacts;
    contacts.append(contact);
    QVersitDocument document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    QCOMPARE(property.parameters().count(), 0);
    QCOMPARE(property.name(), QString::fromAscii("X-GENDER"));
    QCOMPARE(property.value(), gender.gender());
}

void tst_QVersitContactExporter::testEncodeNickName()
{
    QContact contact;

    // Add an extra detail
    QContactGender gender;
    gender.setGender(QContactGender::GenderMale);
    contact.saveDetail(&gender);

    // One nickname given
    QContactNickname firstNickname;
    firstNickname.setNickname(QLatin1String("Homie"));
    contact.saveDetail(&firstNickname);
    QList<QContact> contacts;
    contacts.append(contact);
    QVersitDocument document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 2);
    QVersitProperty property = document.properties().at(1);
    QCOMPARE(property.name(), QLatin1String("X-NICKNAME"));
    QCOMPARE(property.value(), QLatin1String("Homie"));

    // Two nicknames given, should be collated into a single property
    contact.clearDetails();
    contact.saveDetail(&gender);
    contact.saveDetail(&firstNickname);
    QContactNickname secondNickname;
    secondNickname.setNickname(QLatin1String("Jay"));
    contact.saveDetail(&secondNickname);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 2);
    property = document.properties().at(1);
    QCOMPARE(property.name(), QString::fromAscii("X-NICKNAME"));
    QCOMPARE(property.value(), QString::fromAscii("Homie,Jay"));
}

void tst_QVersitContactExporter::testEncodeAnniversary()
{
    QContact contact;
    QContactAnniversary anniversary;
    QDate date(2009,1,1);
    anniversary.setOriginalDate(date);
    anniversary.setContexts(QContactDetail::ContextHome);
    anniversary.setSubType(QContactAnniversary::SubTypeWedding);
    contact.saveDetail(&anniversary);
    QList<QContact> contacts;
    contacts.append(contact);
    QVersitDocument document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    // The contexts and subtypes are not defined for X-ANNIVERSARY property
    QCOMPARE(property.parameters().count(), 0);
    // Check name
    QCOMPARE(property.name(), QString::fromAscii("X-ANNIVERSARY"));
    // Check value
    QCOMPARE(property.value(), date.toString(Qt::ISODate));
}


void tst_QVersitContactExporter::testEncodeOnlineAccount()
{
    QContact contact;
    QContactOnlineAccount onlineAccount;
    QString accountUri(QString::fromAscii("sip:abc@temp.com"));
    onlineAccount.setAccountUri(accountUri);

    // Video sharing
    onlineAccount.setSubTypes(QContactOnlineAccount::SubTypeVideoShare);
    onlineAccount.setContexts(QContactDetail::ContextHome);
    contact.saveDetail(&onlineAccount);
    QList<QContact> contacts;
    contacts.append(contact);
    QVersitDocument document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    // Check parameters
    QCOMPARE(property.parameters().count(), 2);
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("HOME")));
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("SWIS")));
    // Check name
    QCOMPARE(property.name(), QString::fromAscii("X-SIP"));
    // Check value
    QCOMPARE(property.value(), accountUri);

    // VoIP
    onlineAccount.setSubTypes(QContactOnlineAccount::SubTypeSipVoip);
    onlineAccount.setContexts(QContactDetail::ContextWork);
    contact.saveDetail(&onlineAccount);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 1);
    property = document.properties().at(0);
    // Check parameters
    QCOMPARE(property.parameters().count(), 2);
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("WORK")));
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("VOIP")));
    // Check name
    QCOMPARE(property.name(), QString::fromAscii("X-SIP"));
    // Check value
    QCOMPARE(property.value(), accountUri);

    // Plain SIP
    onlineAccount.setSubTypes(QContactOnlineAccount::SubTypeSip);
    onlineAccount.setContexts(QContactDetail::ContextWork);
    contact.saveDetail(&onlineAccount);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 1);
    property = document.properties().at(0);
    // Check parameters, SIP not added as a TYPE parameter
    QCOMPARE(property.parameters().count(), 1);
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("WORK")));
    // Check name
    QCOMPARE(property.name(), QString::fromAscii("X-SIP"));
    // Check value
    QCOMPARE(property.value(), accountUri);

    // IMPP / X-IMPP
    onlineAccount.setSubTypes(QContactOnlineAccount::SubTypeImpp);
    onlineAccount.setContexts(QContactDetail::ContextHome);
    contact.saveDetail(&onlineAccount);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 1);
    property = document.properties().at(0);
    // Check parameters, SIP not added as a TYPE parameter
    QCOMPARE(property.parameters().count(), 1);
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("HOME")));
    // Check name
    QCOMPARE(property.name(), QString::fromAscii("X-IMPP"));
    // Check value
    QCOMPARE(property.value(), accountUri);

    // Other subtypes not converted
    onlineAccount.setSubTypes(QString::fromAscii("INVALIDSUBTYPE"));
    contact.saveDetail(&onlineAccount);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 0);
}

void tst_QVersitContactExporter::testEncodeFamily()
{
    QContact contact;
    QContactFamily family;

    // No spouse, no family
    family.setContexts(QContactDetail::ContextHome);
    contact.saveDetail(&family);
    QList<QContact> contacts;
    contacts.append(contact);
    QVersitDocument document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 0);

    // Only spouse present
    QString spouce = QString::fromAscii("ABC");
    family.setSpouse(spouce);
    contact.saveDetail(&family);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 1);
    QVersitProperty spouseProperty = document.properties().at(0);
    QCOMPARE(spouseProperty.parameters().count(), 0);
    QCOMPARE(spouseProperty.name(), QString::fromAscii("X-SPOUSE"));
    QCOMPARE(spouseProperty.value(), spouce);

    // Spouse and children
    QStringList children;
    children << QString::fromAscii("A") << QString::fromAscii("B") ;
    family.setChildren(children);
    family.setSpouse(spouce);
    contact.saveDetail(&family);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 2);
    spouseProperty = document.properties().at(0);
    QCOMPARE(spouseProperty.parameters().count(), 0);
    QCOMPARE(spouseProperty.name(), QString::fromAscii("X-SPOUSE"));
    QCOMPARE(spouseProperty.value(), spouce);
    QVersitProperty childrenProperty = document.properties().at(1);
    QCOMPARE(childrenProperty.parameters().count(), 0);
    QCOMPARE(childrenProperty.name(), QString::fromAscii("X-CHILDREN"));
    QCOMPARE(childrenProperty.value(), QString::fromAscii("A\\,B"));
}


void tst_QVersitContactExporter::testEncodeDisplayLabel()
{
    QContact contact;
    QContactName contactName;

    // No display label and no QContactName
    QList<QContact> contacts;
    contacts.append(contact);
    QVersitDocument document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 0);

    // No display label, but QContactName found
    contactName.setFirstName(QString::fromAscii("First"));
    contactName.setLastName(QString::fromAscii("Last"));
    contactName.setMiddleName(QString::fromAscii("Middle"));
    contact.saveDetail(&contactName);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 2);
    QVersitProperty displayProperty = document.properties().at(0);
    QCOMPARE(displayProperty.name(), QString::fromAscii("FN"));
    QCOMPARE(displayProperty.value(), QString::fromAscii("First Last"));
    QVersitProperty nameProperty = document.properties().at(1);
    QCOMPARE(nameProperty.name(), QString::fromAscii("N"));
    QCOMPARE(nameProperty.value(),
        QString::fromAscii("Last;First;Middle;;"));

    // Custom label in QContactName, use vCard 3.0 to test the backslash escaping
    contact = QContact();
    contactName.setCustomLabel(QString::fromAscii("Custom,Label"));
    contact.saveDetail(&contactName);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts, QVersitDocument::VCard30Type).first();
    displayProperty = document.properties().at(0);
    QCOMPARE(displayProperty.name(), QString::fromAscii("FN"));
    QCOMPARE(displayProperty.value(),
        QString::fromAscii("Custom\\,Label"));
}

void tst_QVersitContactExporter::testDefaultResourceHandler()
{
    QVersitDefaultResourceHandler handler;
    QByteArray contents;
    QString mimeType;
    handler.loadResource(QLatin1String("test.jpg"), &contents, &mimeType);
    QCOMPARE(mimeType, QLatin1String("image/jpeg"));

    QVersitProperty property;
    QString location;
    QVERIFY(!handler.saveResource("test contents", property, &location));
}

// Test utility functions
QContactDetail tst_QVersitContactExporter::searchDetail(
    QList<QContactDetail> details,
    QString search)
{
    QContactDetail detail;
    for (int i= 0; i < details.count(); i++) {
        if ( details.at(i).definitionName() == search )
        detail = details.at(i);
    }
    return detail;
}

QTEST_MAIN(tst_QVersitContactExporter)

