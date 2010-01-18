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

#include "ut_qversitcontactexporter.h"
#include "qversitcontactexporter.h"
#include "qversitcontactexporter_p.h"
#include "qversitproperty.h"
#include "qversitdefs.h"
#include <QString>
#include <QStringList>
#include <QList>
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
class MyQVersitContactExporterPrivate : public QVersitContactExporterPrivate {
public:
    void encodeName(QVersitProperty& property, const QContactDetail& detail);
    void encodePhoneNumber(QVersitProperty& property, const QContactDetail& detail);
    void encodeEmail(QVersitProperty& property, const QContactDetail& detail);
    void encodeAddress(QVersitProperty& property, const QContactDetail& detail);
    void encodeUrl(QVersitProperty& property, const QContactDetail& detail);
    void encodeUid(QVersitProperty& property, const QContactDetail& detail);
    bool encodeRev(QVersitProperty& property, const QContactDetail& detail);
    void encodeBirthDay(QVersitProperty& property, const QContactDetail& detail);
    void encodeNote(QVersitProperty& property, const QContactDetail& detail);
    void encodeGeoLocation(QVersitProperty& property, const QContactDetail& detail);
    void encodeOrganization(QVersitDocument& document, const QContactDetail& detail);
    void encodeGender(QVersitProperty& property, const QContactDetail& detail);
    void encodeNickname(QVersitDocument& document, const QContactDetail& detail);
    void encodeAnniversary(QVersitProperty& property, const QContactDetail& detail);
    bool encodeOnlineAccount(QVersitProperty& property, const QContactDetail& detail);
    bool encodeFamily(QVersitDocument& document, const QContactDetail& detail);
    bool encodeAvatar(QVersitProperty& property,
            const QContactDetail& detail);
    bool encodeDisplayLabel(QVersitProperty& property,
            const QContactDetail& detail,
            const QContact& contact) {return QVersitContactExporterPrivate::encodeDisplayLabel(property, detail, contact);}
    bool isValidRemoteUrl(const QString& resourceIdentifier) {return QVersitContactExporterPrivate::isValidRemoteUrl(resourceIdentifier);}
    void encodeParameters(QVersitProperty& property,
            const QStringList& contexts,
            const QStringList& subTypes=QStringList()) {return QVersitContactExporterPrivate::encodeParameters(property, contexts, subTypes);}
    bool encodeEmbeddedContent(const QString& resourcePath,
            QVersitProperty& property) {return QVersitContactExporterPrivate::encodeEmbeddedContent(resourcePath, property);}
    void setEscapedValue(QVersitProperty& property,const QString& value) {QVersitContactExporterPrivate::setEscapedValue(property, value);}
    QString escape(const QString& value) {return QVersitContactExporterPrivate::escape(value);}


    // private data
    QHash<QString, QString>& PropertyMappings() {return mPropertyMappings;}
    QHash<QString, QString>& ParameterMappings() {return mParameterMappings;}

};

class MyQVersitContactDetailExporter : public QVersitContactDetailExporter
{
public:
    bool processDetail(const QContactDetail& detail, QVersitDocument* document)
    {
        mProcessedDetails.append(detail);
        if (mDefinitionNamesToProcess.contains(detail.definitionName())) {
            QVersitProperty property;
            property.setName(detail.definitionName());
            property.setValue(detail.definitionName());
            document->addProperty(property);
            return true;
        } else {
            return false;
        }
    }

    bool processUnknownDetail(const QContactDetail& detail, QVersitDocument* document)
    {
        Q_UNUSED(document)
        mUnknownDetails.append(detail);
        return false;
    }

    QStringList mDefinitionNamesToProcess;
    QList<QContactDetail> mUnknownDetails;
    QList<QContactDetail> mProcessedDetails;
};

class MyQVersitFileLoader : public QVersitFileLoader
{
public:
    MyQVersitFileLoader()
        : mLoadFileCalled(false)
    {
    }

    bool loadFile(const QString& filename, QByteArray* contents, QString* mimeType)
    {
        mFilename = filename;
        *contents = mSimulatedData;
        *mimeType = mSimulatedMimeType;
        mLoadFileCalled = true;
        return true;
    }

    QByteArray mSimulatedData;
    QString mSimulatedMimeType;
    QString mFilename;
    bool mLoadFileCalled;
};

QTM_END_NAMESPACE

QTM_USE_NAMESPACE

const QString TEST_PHOTO_FILE(QLatin1String("versitTest001.jpg"));
const QString TEST_AUDIO_FILE(QLatin1String("versitTest001.wav"));

void UT_QVersitContactExporter::init()
{
    mExporter = new QVersitContactExporter();
    mExporterPrivate = new MyQVersitContactExporterPrivate();
    QVERIFY(!mExporterPrivate->ParameterMappings().empty());
    QVERIFY(!mExporterPrivate->PropertyMappings().empty());

}

void UT_QVersitContactExporter::cleanup()
{
    delete mExporterPrivate;
    delete mExporter;
}

void UT_QVersitContactExporter::testConvertContact()
{
    QContact contact;

    // Adding name to the contact
    QContactName name;
    name.setFirst(QString::fromAscii("Moido"));
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

void UT_QVersitContactExporter::testUnknownContactDetails()
{
    // Test1: Un-supported Avatar Test
    QContact contact;
    QVersitDocument document;
    QContactAvatar contactAvatar;
    const QString url = QString::fromAscii("http://www.myhome.com/test.jpg");
    contactAvatar.setSubType(QContactAvatar::SubTypeTexturedMesh);
    contact.saveDetail(&contactAvatar);
    QList<QContact> contacts;
    contacts.append(contact);
    MyQVersitContactDetailExporter detailExporter;
    mExporter->setDetailExporter(&detailExporter);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 0);
    QList<QContactDetail> unknownDetails = detailExporter.mUnknownDetails;
    QString defintionName = contactAvatar.definitionName();
    QContactDetail detail = searchDetail(unknownDetails,defintionName);
    QCOMPARE(defintionName, detail.definitionName());

    // Test2: Un-supported Online Account
    QContactOnlineAccount onlineAccount;
    QString testUri = QString::fromAscii("sip:abc@temp.com");
    onlineAccount.setAccountUri(testUri);
    onlineAccount.setSubTypes(QString::fromAscii("unsupported"));
    contact.saveDetail(&onlineAccount);
    contacts.clear();
    contacts.append(contact);
    detailExporter = MyQVersitContactDetailExporter();
    mExporter->setDetailExporter(&detailExporter);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 0);
    unknownDetails = detailExporter.mUnknownDetails;
    defintionName = onlineAccount.definitionName();
    detail = QContactDetail();
    detail = searchDetail(unknownDetails, defintionName);
    QCOMPARE(defintionName, detail.definitionName());
}

void UT_QVersitContactExporter::testEncodeName()
{
    QContact contact;
    QContactName name;

    // vCard 2.1
    name.setFirst(QString::fromAscii("Heido"));
    name.setLast(QString::fromAscii("HH"));
    name.setMiddle(QString::fromAscii("A"));
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
    QCOMPARE(displayProperty.valueString(), QString::fromAscii("Heido HH"));

    QVersitProperty nameProperty = document.properties().at(1);
    // Check parameters, contexts not allowed for N property
    QCOMPARE(nameProperty.parameters().count(), 0);
    // Check name
    QCOMPARE(nameProperty.name(), QString::fromAscii("N"));
    // Check value
    QCOMPARE(nameProperty.valueString(), QString::fromAscii("HH;Heido;A;Mr.;"));

    // vCard 3.0, special characters in the name parts are backslash escaped
    contact.removeDetail(&name);
    name.setFirst(QString::fromAscii("Hom,er"));
    name.setLast(QString::fromAscii("Simp;son"));
    name.setMiddle(QString::fromAscii("J;"));
    name.setPrefix(QString::fromAscii(";Mr."));
    name.setSuffix(QString::fromAscii("Sir,"));
    contact.saveDetail(&name);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts, QVersitDocument::VCard30Type).first();
    QCOMPARE(document.versitType(),QVersitDocument::VCard30Type);

    // Each Contact has display label detail by default. Display label is enocded
    // if some value exisit for the Label or if value for Name exisit.
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

    QCOMPARE(displayProperty.valueString(), QString::fromAscii("Hom\\,er Simp\\;son"));

    QCOMPARE(nameProperty.valueString(),
             QString::fromAscii("Simp\\;son;Hom\\,er;J\\;;\\;Mr.;Sir\\,"));
}

void UT_QVersitContactExporter::testEncodePhoneNumber()
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
    QCOMPARE(property.valueString(), phoneNumber.number());
}

void UT_QVersitContactExporter::testEncodeEmailAddress()
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
    QCOMPARE(property.valueString(), email.emailAddress());
}

void UT_QVersitContactExporter::testEncodeStreetAddress()
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
    QCOMPARE(property.valueString(), QString::fromAscii(";;HKKI 1X 90;Helsinki;;00440;Finland"));

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
    QCOMPARE(document.versitType(),QVersitDocument::VCard30Type);
    QCOMPARE(document.properties().count(), 1);
    property = document.properties().at(0);
    // Check name
    QCOMPARE(property.name(), QString::fromAscii("ADR"));
    // Check value
    QCOMPARE(property.valueString(),
             QString::fromAscii("PO\\;Box;;My\\;Street;My\\;Town;My\\;State;12345\\;;My\\;Country"));
}

void UT_QVersitContactExporter::testEncodeUrl()
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
    QCOMPARE(property.valueString(), url.url());
}

void UT_QVersitContactExporter::testEncodeUid()
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
    QCOMPARE(property.valueString(), guid.guid());

    // vCard 3.0, special characters in the value are backslash escaped
    contact.removeDetail(&guid);
    guid.setGuid(QString::fromAscii("1;2,3\r\n4\\5"));
    contact.saveDetail(&guid);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts, QVersitDocument::VCard30Type).first();
    QCOMPARE(document.versitType(),QVersitDocument::VCard30Type);
    QCOMPARE(document.properties().count(), 1);
    property = document.properties().at(0);
    // Check name
    QCOMPARE(property.name(), QString::fromAscii("UID"));
    // Check value
    QCOMPARE(property.valueString(), QString::fromAscii("1\\;2\\,3\\n4\\\\5"));
}

void UT_QVersitContactExporter::testEncodeRev()
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
    QCOMPARE(property.valueString(), expectedValueUTCEncoded);

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
    QCOMPARE(property.valueString(), expectedValueUTCEncoded);

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
    QCOMPARE(property.valueString(), expectedValueUTCEncoded);

    // Last modified time not found, creation time not found
    timeStamp.setLastModified(emptyTime);
    timeStamp.setCreated(emptyTime);
    contact.saveDetail(&timeStamp);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 0);
}

void UT_QVersitContactExporter::testEncodeBirthDay()
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
    QCOMPARE(property.valueString(), QString::fromAscii("2009-01-01"));
}

void UT_QVersitContactExporter::testEncodeNote()
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
    QCOMPARE(property.valueString(), note.note());
}

void UT_QVersitContactExporter::testEncodeGeoLocation()
{
    QContact contact;
    QContactGeolocation geoLocation;
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
    QCOMPARE(property.valueString(), expectedValue);
}

void UT_QVersitContactExporter::testEncodeOrganization()
{
    QList<QContact> contacts;
    QContact contact;
    QContactOrganization organization;
    QVersitDocument document;
    QVersitProperty property;
    MyQVersitFileLoader fileLoader;
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
    QCOMPARE(property.valueString(), title);

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
    QCOMPARE(property.valueString(), QString::fromAscii("Nokia;"));

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
    QCOMPARE(property.valueString(), QString::fromAscii(";R&D;Qt"));

    // ORG with name and department/unit
    organization.setName(QString::fromAscii("Nokia"));
    contact.saveDetail(&organization);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 1);
    property = document.properties().at(0);
    QCOMPARE(property.name(), QString::fromAscii("ORG"));
    QCOMPARE(property.valueString(), QString::fromAscii("Nokia;R&D;Qt"));

    // TITLE and ORG
    organization.setTitle(QString::fromAscii("Developer"));
    contact.saveDetail(&organization);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 2);
    property = document.properties().at(0);
    QCOMPARE(property.name(), QString::fromAscii("TITLE"));
    QCOMPARE(property.valueString(), title);
    property = document.properties().at(1);
    QCOMPARE(property.name(), QString::fromAscii("ORG"));
    QCOMPARE(property.valueString(), QString::fromAscii("Nokia;R&D;Qt"));

    // ORG LOGO Test1: LOGO as remote Resouce
    const QString url = QString::fromAscii("http://myhome.com/test.jpg");
    contact = QContact();
    organization = QContactOrganization();
    organization.setLogo(url);
    contact.saveDetail(&organization);
    contacts.clear();
    contacts.append(contact);
    mExporter->setFileLoader(&fileLoader);
    document = mExporter->exportContacts(contacts).first();
    QVERIFY(!fileLoader.mLoadFileCalled);

    //Media type, and source type are encoded.
    QCOMPARE(document.properties().at(0).parameters().count(), 2);

    QVERIFY(document.properties().at(0).parameters().contains(
            QString::fromAscii("TYPE"), QString::fromAscii("JPEG")));
    QVERIFY(document.properties().at(0).parameters().contains(
            QString::fromAscii("VALUE"), QString::fromAscii("URL")));

    //Check property Name
    QString propertyName = document.properties().at(0).name();
    QCOMPARE(propertyName, QString::fromAscii("LOGO"));

    //Check property value
    QString value = document.properties().at(0).valueString();
    QCOMPARE(value, url);

    // ORG LOGO Test2: LOGO File.
    fileLoader.mSimulatedData = "simulated data";
    contact = QContact();
    organization = QContactOrganization();
    organization.setLogo(TEST_PHOTO_FILE);
    contact.saveDetail(&organization);
    contacts.clear();
    contacts.append(contact);
    mExporter->setFileLoader(&fileLoader);
    document = mExporter->exportContacts(contacts).first();
    QVERIFY(fileLoader.mLoadFileCalled);
    QCOMPARE(fileLoader.mFilename, TEST_PHOTO_FILE);

    // It should be stored in the property as a QVariant of QByteArray, not base64 encoded
    property = document.properties().at(0);
    QMultiHash<QString,QString> parameters = property.parameters();
    // Media type is encoded
    QCOMPARE(parameters.count(), 1);
    QVERIFY(parameters.contains(
            QString::fromAscii("TYPE"), QString::fromAscii("JPEG")));
    // Verify value.
    QVariant variantValue = property.value();
    QVERIFY(variantValue.type() == QVariant::ByteArray);
    QCOMPARE(variantValue.value<QByteArray>(), fileLoader.mSimulatedData);

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
    QCOMPARE(property.valueString(), QString::fromAscii("myAssistant"));

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
    QCOMPARE(property.valueString(), QString::fromAscii("Executive"));

}

void UT_QVersitContactExporter::testEncodeAvatar()
{
    QContact contact;
    QContactAvatar contactAvatar;
    MyQVersitFileLoader fileLoader;
    fileLoader.mSimulatedData = "simulated data";

    // Test1: Web URL
    const QString url = QString::fromAscii("http://www.myhome.com/test.jpg");
    contactAvatar.setAvatar(url);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    QList<QContact> contacts;
    contacts.append(contact);
    mExporter->setFileLoader(&fileLoader);
    QVersitDocument document = mExporter->exportContacts(contacts).first();
    QVersitProperty property = document.properties().at(0);
    QCOMPARE(property.parameters().count(), 2);
    QVERIFY(!fileLoader.mLoadFileCalled);

    // Test 2: Local Media PHOTO
    contactAvatar.setAvatar(TEST_PHOTO_FILE);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QVERIFY(fileLoader.mLoadFileCalled);
    QCOMPARE(fileLoader.mFilename, TEST_PHOTO_FILE);

    property = document.properties().at(0);
    //Media type is encoded
    QCOMPARE(property.parameters().count(), 1);
    // verify the value
    QVariant variantValue = property.value();
    QVERIFY(variantValue.type() == QVariant::ByteArray);
    QCOMPARE(variantValue.value<QByteArray>(), fileLoader.mSimulatedData);

    // Test3: UnSupported Media Type, properties and parameters are not encoded
    fileLoader = MyQVersitFileLoader();
    const QString testUrl2 = QString::fromAscii("http://www.myhome.com/test.jpg");
    contactAvatar.setAvatar(testUrl2);
    // un-supported media type is encoded
    contactAvatar.setSubType(QContactAvatar::SubTypeTexturedMesh);
    contact.saveDetail(&contactAvatar);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 0);
    QVERIFY(!fileLoader.mLoadFileCalled);
}


void UT_QVersitContactExporter::testEncodeEmbeddedContent()
{
    QContact contact;
    QContactAvatar contactAvatar;
    MyQVersitFileLoader fileLoader;
    QVariant variantValue;

    // Test 1: URL
    const QString url = QString::fromAscii("http://www.myhome.com/test.jpg");
    contactAvatar.setAvatar(url);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    QList<QContact> contacts;
    contacts.append(contact);
    mExporter->setFileLoader(&fileLoader);
    QVersitDocument document = mExporter->exportContacts(contacts).first();
    QVERIFY(!fileLoader.mLoadFileCalled);
    QVersitProperty photoProperty = document.properties().at(0);
    QCOMPARE(photoProperty.parameters().count(), 2);
    QVERIFY(photoProperty.parameters().contains(
            QString::fromAscii("TYPE"),QString::fromAscii("JPEG")));
    QVERIFY(photoProperty.parameters().contains(
            QString::fromAscii("VALUE"),QString::fromAscii("URL")));
    QCOMPARE(photoProperty.name(), QString::fromAscii("PHOTO"));
    QCOMPARE(photoProperty.valueString(), url);

    // Test 2: Local PHOTO, image loaded by the loader
    contactAvatar.setAvatar(TEST_PHOTO_FILE);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    contacts.clear();
    contacts.append(contact);
    fileLoader = MyQVersitFileLoader();
    fileLoader.mSimulatedData = "simulated image data";
    document = mExporter->exportContacts(contacts).first();
    QVERIFY(fileLoader.mLoadFileCalled);
    photoProperty = document.properties().at(0);
    QCOMPARE(photoProperty.parameters().count(), 1);
    QVERIFY(photoProperty.parameters().contains(
        QString::fromAscii("TYPE"),
        QString::fromAscii("JPEG")));
    variantValue = photoProperty.value();
    QVERIFY(variantValue.type() == QVariant::ByteArray);
    QCOMPARE(variantValue.value<QByteArray>(), fileLoader.mSimulatedData);

    // Test 3: Local SOUND
    fileLoader = MyQVersitFileLoader();
    fileLoader.mSimulatedData = "simulated audio data";
    contactAvatar.setAvatar(TEST_AUDIO_FILE);
    contactAvatar.setSubType(QContactAvatar::SubTypeAudioRingtone);
    contact.saveDetail(&contactAvatar);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QVERIFY(fileLoader.mLoadFileCalled);
    QVersitProperty soundProperty = document.properties().at(0);
    QCOMPARE(soundProperty.parameters().count(), 1);
    QVERIFY(soundProperty.parameters().contains(
        QString::fromAscii("TYPE"),
        QString::fromAscii("WAVE")));
    variantValue = soundProperty.value();
    QVERIFY(variantValue.type() == QVariant::ByteArray);
    QCOMPARE(variantValue.value<QByteArray>(), fileLoader.mSimulatedData);

    // Test 4: New media format will be encoded also
    fileLoader = MyQVersitFileLoader();
    const QString testUrl = QString::fromAscii("http://www.myhome.com/test.ggg");
    contactAvatar.setAvatar(testUrl);
    contactAvatar.setSubType(QContactAvatar::SubTypeImage);
    contact.saveDetail(&contactAvatar);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QVERIFY(!fileLoader.mLoadFileCalled);
    QCOMPARE(document.properties().count(), 1);
    QCOMPARE(document.properties().at(0).parameters().count(), 2);
    QVERIFY(document.properties().at(0).parameters().contains(
            QString::fromAscii("TYPE"), QString::fromAscii("GGG")));
    QVERIFY(document.properties().at(0).parameters().contains(
            QString::fromAscii("VALUE"),QString::fromAscii("URL")));

    // Test 5: Unsupported media type, properties and parameters are not encoded
    fileLoader = MyQVersitFileLoader();
    const QString testUrl2 = QString::fromAscii("http://www.myhome.com/test.jpg");
    contactAvatar.setAvatar(testUrl2);
    // un-supported media type is encoded
    contactAvatar.setSubType(QContactAvatar::SubTypeTexturedMesh);
    contact.saveDetail(&contactAvatar);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 0);
    QVERIFY(!fileLoader.mLoadFileCalled);
}

void UT_QVersitContactExporter::testEncodeParameters()
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

void UT_QVersitContactExporter::testIsValidRemoteUrl()
{
    QContact contact;
    QContactAvatar contactAvatar;

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

void UT_QVersitContactExporter::testEncodeGender()
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
    QCOMPARE(property.valueString(), gender.gender());
}

void UT_QVersitContactExporter::testEncodeNickName()
{
    QContact contact;
    QVersitDocument document;

    // Nickname not yet in the document
    QContactNickname nicknameDetail;
    QString firstNickname(QString::fromAscii("Homie"));
    nicknameDetail.setNickname(firstNickname);

    contact.saveDetail(&nicknameDetail);
    mExporterPrivate->exportContact(document,contact);

    QCOMPARE(document.properties().count(), 1);
    QVersitProperty property = document.properties().at(0);
    QCOMPARE(property.name(), QString::fromAscii("X-NICKNAME"));
    QCOMPARE(property.valueString(), firstNickname);

    // Nickname already in the document, append to the existing property
    QString secondNickname(QString::fromAscii("Jay"));
    nicknameDetail.setNickname(secondNickname);

    contact.saveDetail(&nicknameDetail);
    mExporterPrivate->exportContact(document,contact);
    QCOMPARE(document.properties().count(), 1);
    property = document.properties().at(0);
    QCOMPARE(property.name(), QString::fromAscii("X-NICKNAME"));
    QCOMPARE(property.valueString(), QString::fromAscii("Homie,Jay"));
}

void UT_QVersitContactExporter::testEncodeAnniversary()
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
    QCOMPARE(property.valueString(), date.toString(Qt::ISODate));
}


void UT_QVersitContactExporter::testEncodeOnlineAccount()
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
    QCOMPARE(property.valueString(), accountUri);

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
    QCOMPARE(property.valueString(), accountUri);

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
    QCOMPARE(property.valueString(), accountUri);

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
    QCOMPARE(property.valueString(), accountUri);

    // Other subtypes not converted
    onlineAccount.setSubTypes(QString::fromAscii("INVALIDSUBTYPE"));
    contact.saveDetail(&onlineAccount);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 0);
}

void UT_QVersitContactExporter::testEncodeFamily()
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
    QCOMPARE(spouseProperty.valueString(), spouce);

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
    QCOMPARE(spouseProperty.valueString(), spouce);
    QVersitProperty childrenProperty = document.properties().at(1);
    QCOMPARE(childrenProperty.parameters().count(), 0);
    QCOMPARE(childrenProperty.name(), QString::fromAscii("X-CHILDREN"));
    QCOMPARE(childrenProperty.valueString(), QString::fromAscii("A,B"));
}


void UT_QVersitContactExporter::testEncodeDisplayLabel()
{
    QContact contact;
    QContactDisplayLabel displayLaebl;
    QContactName contactName;

    // No display label and no QContactName
    QList<QContact> contacts;
    contacts.append(contact);
    QVersitDocument document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 0);

    // No display label, but QContactName found
    contactName.setFirst(QString::fromAscii("First"));
    contactName.setLast(QString::fromAscii("Last"));
    contactName.setMiddle(QString::fromAscii("Middle"));
    contact.saveDetail(&contactName);
    contacts.clear();
    contacts.append(contact);
    document = mExporter->exportContacts(contacts).first();
    QCOMPARE(document.properties().count(), 2);
    QVersitProperty displayProperty = document.properties().at(0);
    QCOMPARE(displayProperty.name(), QString::fromAscii("FN"));
    QCOMPARE(displayProperty.valueString(), QString::fromAscii("First Last"));
    QVersitProperty nameProperty = document.properties().at(1);
    QCOMPARE(nameProperty.name(), QString::fromAscii("N"));
    QCOMPARE(nameProperty.valueString(),
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
    QCOMPARE(displayProperty.valueString(),
        QString::fromAscii("Custom\\,Label"));
}

// Test utility functions
QContactDetail UT_QVersitContactExporter::searchDetail(
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

QTEST_MAIN(UT_QVersitContactExporter)

