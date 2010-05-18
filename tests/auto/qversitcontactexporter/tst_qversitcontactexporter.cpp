/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "qversitdocument.h"
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

/* This class just logs the arguments to the last call to postProcessDetail */
class MyQVersitContactExporterDetailHandlerV2 : public QVersitContactExporterDetailHandlerV2
{
public:
    MyQVersitContactExporterDetailHandlerV2()
    {
    }

    void detailProcessed(const QContact& contact,
                         const QContactDetail& detail,
                         const QSet<QString>& processedFields,
                         const QVersitDocument& document,
                         QList<QVersitProperty>* toBeRemoved,
                         QList<QVersitProperty>* toBeAdded)
    {
        mContact = contact;
        mDetail = detail;
        mProcessedFields = processedFields;
        mDocument = document;
        mToBeRemoved = *toBeRemoved;
        mToBeAdded = *toBeAdded;
    }

    void contactProcessed(const QContact& contact, QVersitDocument* document)
    {
        mEndContact = contact;
        mEndDocument = *document;
    }

    QContact mContact;
    QContactDetail mDetail;
    QSet<QString> mProcessedFields;
    QVersitDocument mDocument;
    QList<QVersitProperty> mToBeRemoved;
    QList<QVersitProperty> mToBeAdded;

    QContact mEndContact;
    QVersitDocument mEndDocument;
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

// The number of vCard properties generated by a contact made with createContactWithName()
const static int BASE_PROPERTY_COUNT = 2;

QTM_END_NAMESPACE

QTM_USE_NAMESPACE

const QString TEST_PHOTO_FILE(QLatin1String("versitTest001.jpg"));
const QString TEST_AUDIO_FILE(QLatin1String("versitTest001.wav"));

// Checks that the property has a value of the given expectedType and the given expectedValue.
#define CHECK_VALUE(property,expectedValueType,expectedValue) {\
    QCOMPARE(property.valueType(), expectedValueType); \
    QVariant value = property.variantValue(); \
    QCOMPARE(value.type(), QVariant::StringList); \
    QCOMPARE(value.toStringList(), expectedValue); \
}

void tst_QVersitContactExporter::init()
{
    mExporter = new QVersitContactExporter();
    mResourceHandler = new MyQVersitResourceHandler;
    mExporter->setResourceHandler(mResourceHandler);
}

void tst_QVersitContactExporter::cleanup()
{
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
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    QList<QVersitDocument> documents = mExporter->documents();

    // Each Contact has display label detail by default. Display label is enocded
    // if some value exist for the Label or if value for Name exist.
    QCOMPARE(documents.size(), 1);
    QCOMPARE(documents.first().properties().count(), 3);
}

void tst_QVersitContactExporter::testContactDetailHandler()
{
    MyQVersitContactExporterDetailHandler detailHandler;;
    mExporter->setDetailHandler(&detailHandler);

    // Test1: Un-supported Avatar Test
    QContact contact(createContactWithName(QLatin1String("asdf")));
    QContactDetail unknownDetail;
    unknownDetail.setValue(QLatin1String("Unknown"), QLatin1String("Detail"));
    contact.saveDetail(&unknownDetail);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    QVersitDocument document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 0);
    QList<QContactDetail> unknownDetails = detailHandler.mUnknownDetails;
    QVERIFY(unknownDetails.size() > 0);
    QString definitionName = unknownDetail.definitionName();
    QContactDetail detail = findDetailByName(unknownDetails,definitionName);
    QCOMPARE(definitionName, detail.definitionName());

    // Test2: Un-supported Online Account
    QContactOnlineAccount onlineAccount;
    QString testUri = QString::fromAscii("sip:abc@temp.com");
    onlineAccount.setAccountUri(testUri);
    onlineAccount.setSubTypes(QString::fromAscii("unsupported"));
    contact.saveDetail(&onlineAccount);
    detailHandler.clear();
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 0);
    unknownDetails = detailHandler.mUnknownDetails;
    QVERIFY(unknownDetails.size() > 0);
    definitionName = onlineAccount.definitionName();
    detail = findDetailByName(unknownDetails, definitionName);
    QCOMPARE(definitionName, detail.definitionName());

    // Test that preProcessDetail returns true stops the exporter from doing anything.
    contact.clearDetails();
    QContactName contactName;
    contactName.setFirstName(QLatin1String("John"));
    contact.saveDetail(&contactName);
    detailHandler.clear();
    detailHandler.mPreProcess = true;
    // Fails, with NoNameError
    QVERIFY(!mExporter->exportContacts(QList<QContact>() << contact,
            QVersitDocument::VCard30Type));
    QList<QVersitDocument> documents = mExporter->documents();
    QCOMPARE(documents.size(), 0);
    QVERIFY(detailHandler.mPreProcessedDetails.count() > BASE_PROPERTY_COUNT);
    QCOMPARE(detailHandler.mPostProcessedDetails.count(), 0);
    QCOMPARE(detailHandler.mUnknownDetails.count(), 0);

    QVERIFY(mExporter->detailHandler() == &detailHandler);
    mExporter->setDetailHandler(static_cast<QVersitContactExporterDetailHandler*>(0));
}

void tst_QVersitContactExporter::testContactDetailHandlerV2()
{
    MyQVersitContactExporterDetailHandlerV2 detailHandler;
    mExporter->setDetailHandler(&detailHandler);

    QContact contact(createContactWithName(QLatin1String("asdf")));
    QContactPhoneNumber phone;
    phone.setNumber(QLatin1String("1234"));
    phone.setValue(QLatin1String("ExtraField"), QLatin1String("Value"));
    contact.saveDetail(&phone);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));

    QCOMPARE(detailHandler.mProcessedFields.size(), 3);
    QVERIFY(detailHandler.mProcessedFields.contains(QLatin1String(QContactPhoneNumber::FieldContext)));
    QVERIFY(detailHandler.mProcessedFields.contains(QLatin1String(QContactPhoneNumber::FieldSubTypes)));
    QVERIFY(detailHandler.mProcessedFields.contains(QLatin1String(QContactPhoneNumber::FieldNumber)));
    QVersitProperty expectedProperty;
    expectedProperty.setName(QLatin1String("TEL"));
    expectedProperty.setValue(QLatin1String("1234"));
    QCOMPARE(detailHandler.mToBeAdded.size(), 1);
    QCOMPARE(detailHandler.mToBeAdded.first(), expectedProperty);

    mExporter->setDetailHandler(static_cast<QVersitContactExporterDetailHandlerV2*>(0));
}

void tst_QVersitContactExporter::testEncodeName()
{
    QContact contact;
    QContactName name;

    // Special characters are NOT backslash escaped by the exporter, only by the writer.
    name.setFirstName(QString::fromAscii("He;ido"));
    name.setLastName(QString::fromAscii("HH"));
    name.setMiddleName(QString::fromAscii("A"));
    name.setPrefix(QString::fromAscii("Mr."));
    contact.saveDetail(&name);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard21Type));
    QVersitDocument document = mExporter->documents().first();

    // Each Contact has display label detail by default. Display label is enocded
    // if some value exists for the Label or if value for Name exists.
    QCOMPARE(document.properties().count(), 2);

    QVersitProperty displayProperty = findPropertyByName(document, QLatin1String("FN"));
    // Check name
    QCOMPARE(displayProperty.name(), QString::fromAscii("FN"));
    // Check value
    QCOMPARE(displayProperty.value(), QString::fromAscii("He;ido HH"));

    QVersitProperty nameProperty = findPropertyByName(document, QLatin1String("N"));
    // Check parameters, contexts not allowed for N property
    QCOMPARE(nameProperty.parameters().count(), 0);
    // Check name
    QCOMPARE(nameProperty.name(), QString::fromAscii("N"));
    CHECK_VALUE(nameProperty, QVersitProperty::CompoundType,
                QStringList() << QLatin1String("HH") << QLatin1String("He;ido")
                << QLatin1String("A") << QLatin1String("Mr.") << QString());
}

void tst_QVersitContactExporter::testEncodePhoneNumber()
{
    QContact contact(createContactWithName(QLatin1String("asdf")));
    QContactPhoneNumber phoneNumber;
    phoneNumber.setNumber(QString::fromAscii("12345678"));
    phoneNumber.setContexts(QContactDetail::ContextHome);
    phoneNumber.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    contact.saveDetail(&phoneNumber);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    QVersitDocument document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 1);
    QVersitProperty property = findPropertyByName(document, QLatin1String("TEL"));
    QVERIFY(!property.isEmpty());
    // Check parameters
    QCOMPARE(property.parameters().count(), 2);
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("HOME")));
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("CELL")));
    // Check value
    QCOMPARE(property.value(), phoneNumber.number());

    QContactPhoneNumber assistantNumber;
    assistantNumber.setNumber(QLatin1String("4321"));
    assistantNumber.setContexts(QContactDetail::ContextWork);
    assistantNumber.setSubTypes(QContactPhoneNumber::SubTypeAssistant);
    contact.saveDetail(&assistantNumber);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 2);
    property = findPropertyByName(document, QLatin1String("X-ASSISTANT-TEL"));
    QVERIFY(!property.isEmpty());
    QCOMPARE(property.parameters().count(), 1);
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("WORK")));
    QCOMPARE(property.value(), assistantNumber.number());
}

void tst_QVersitContactExporter::testEncodeEmailAddress()
{
    QContact contact(createContactWithName(QLatin1String("asdf")));
    QContactEmailAddress email;
    email.setEmailAddress(QString::fromAscii("test@test"));
    email.setContexts(QContactDetail::ContextHome);
    contact.saveDetail(&email);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    QVersitDocument document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 1);
    QVersitProperty property = findPropertyByName(document, QLatin1String("EMAIL"));
    QVERIFY(!property.isEmpty());
    // Check parameters
    QCOMPARE(property.parameters().count(), 1);
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("HOME")));
    // Check value
    QCOMPARE(property.value(), email.emailAddress());
}

void tst_QVersitContactExporter::testEncodeStreetAddress()
{
    QContact contact(createContactWithName(QLatin1String("asdf")));
    QContactAddress address;

    address.setPostOfficeBox(QLatin1String("1234"));
    address.setCountry(QLatin1String("Finland"));
    address.setPostcode(QLatin1String("00440"));
    // Special characters are not escaped by the exporter, but by the writer
    address.setStreet(QLatin1String("HKKI; 1X 90"));
    address.setLocality(QLatin1String("Helsinki"));
    address.setContexts(QContactDetail::ContextHome);
    address.setSubTypes(QContactAddress::SubTypePostal);
    contact.saveDetail(&address);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard21Type));
    QVersitDocument document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 1);
    QVersitProperty property = findPropertyByName(document, QLatin1String("ADR"));
    QVERIFY(!property.isEmpty());
    // Check parameters
    QCOMPARE(property.parameters().count(), 2);
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("HOME")));
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("POSTAL")));
    // Check name
    QCOMPARE(property.name(), QString::fromAscii("ADR"));
    CHECK_VALUE(property, QVersitProperty::CompoundType,
                QStringList() << QLatin1String("1234") << QString() << QLatin1String("HKKI; 1X 90")
                << QLatin1String("Helsinki") << QString() << QLatin1String("00440")
                << QLatin1String("Finland"));
}

void tst_QVersitContactExporter::testEncodeUrl()
{
    QContact contact(createContactWithName(QLatin1String("asdf")));
    QContactUrl url;
    url.setUrl(QString::fromAscii("http://www.myhome.com"));
    url.setContexts(QContactDetail::ContextHome);
    url.setSubType(QContactUrl::SubTypeHomePage);
    contact.saveDetail(&url);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    QVersitDocument document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 1);
    QVersitProperty property = findPropertyByName(document, QLatin1String("URL"));
    QVERIFY(!property.isEmpty());
    // Check parameters
    QCOMPARE(property.parameters().count(), 1);
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("HOME")));
    // Check value
    QCOMPARE(property.value(), url.url());
}

void tst_QVersitContactExporter::testEncodeUid()
{
    QContact contact(createContactWithName(QLatin1String("asdf")));
    QContactGuid guid;

    guid.setContexts(QContactDetail::ContextHome);
    guid.setGuid(QString::fromAscii("0101222"));
    contact.saveDetail(&guid);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard21Type));
    QVersitDocument document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 1);
    QVersitProperty property = findPropertyByName(document, QLatin1String("UID"));
    QVERIFY(!property.isEmpty());
    // Check parameters
    // Contexts are not allowed for UID
    QCOMPARE(property.parameters().count(), 0);
    // Check value
    QCOMPARE(property.value(), guid.guid());
}

void tst_QVersitContactExporter::testEncodeRev()
{
    QContact contact(createContactWithName(QLatin1String("asdf")));
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
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    QVersitDocument document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 1);
    QVersitProperty property = findPropertyByName(document, QLatin1String("REV"));
    QVERIFY(!property.isEmpty());
    QCOMPARE(property.parameters().count(), 0);
    QString expectedValueUTCEncoded = QString::fromAscii("2009-01-01T06:01:02Z");
    QCOMPARE(property.value(), expectedValueUTCEncoded);

    // Last modified time not found, use the creation time
    QDateTime emptyTime;
    timeStamp.setLastModified(emptyTime);
    timeStamp.setCreated(revisionTime);
    contact.saveDetail(&timeStamp);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 1);
    property = findPropertyByName(document, QLatin1String("REV"));
    QVERIFY(!property.isEmpty());
    QCOMPARE(property.value(), expectedValueUTCEncoded);

    // Last modified time found, Local Time spec not UTC
    QDateTime localTime;
    revisionTime.setTimeSpec(Qt::LocalTime);
    timeStamp.setLastModified(revisionTime);
    localTime.setTimeSpec(Qt::LocalTime);
    timeStamp.setCreated(localTime);
    contact.saveDetail(&timeStamp);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 1);
    property = findPropertyByName(document, QLatin1String("REV"));
    QVERIFY(!property.isEmpty());
    QString expectedValueEncoded = QString::fromAscii("2009-01-01T06:01:02");
    QCOMPARE(property.value(), expectedValueEncoded);

    // Last modified time not found, creation time not found
    timeStamp.setLastModified(emptyTime);
    timeStamp.setCreated(emptyTime);
    contact.saveDetail(&timeStamp);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 0);
}

void tst_QVersitContactExporter::testEncodeBirthDay()
{
    QContact contact(createContactWithName(QLatin1String("asdf")));
    QDate date(2009,1,1);
    QContactBirthday birthDay;
    birthDay.setDate(date);
    // Contexts not allowed in BDAY property, check that they are not added
    birthDay.setContexts(QContactDetail::ContextHome);
    contact.saveDetail(&birthDay);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    QVersitDocument document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 1);
    QVersitProperty property = findPropertyByName(document, QLatin1String("BDAY"));
    QVERIFY(!property.isEmpty());
    QCOMPARE(property.parameters().count(), 0);
    QCOMPARE(property.value(), QString::fromAscii("2009-01-01"));
}

void tst_QVersitContactExporter::testEncodeNote()
{
    QContact contact(createContactWithName(QLatin1String("asdf")));
    QContactNote note;
    note.setNote(QString::fromAscii("My Note"));
    // Contexts not allowed in NOTE property, check that they are not added
    note.setContexts(QContactDetail::ContextHome);
    contact.saveDetail(&note);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    QVersitDocument document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 1);
    QVersitProperty property = findPropertyByName(document, QLatin1String("NOTE"));
    QVERIFY(!property.isEmpty());
    QCOMPARE(property.parameters().count(), 0);
    QCOMPARE(property.value(), note.note());
}

void tst_QVersitContactExporter::testEncodeGeoLocation()
{
    QContact contact(createContactWithName(QLatin1String("asdf")));
    QContactGeoLocation geoLocation;
    QString longitude = QString::fromAscii("99.9");
    geoLocation.setLongitude(longitude.toDouble());
    QString latitude = QString::fromAscii("98.9");
    geoLocation.setLatitude(latitude.toDouble());
    // Contexts not allowed in GEO property, check that they are not added
    geoLocation.setContexts(QContactDetail::ContextHome);
    contact.saveDetail(&geoLocation);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    QVersitDocument document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 1);
    QVersitProperty property = findPropertyByName(document, QLatin1String("GEO"));
    QVERIFY(!property.isEmpty());
    QCOMPARE(property.parameters().count(), 0);
    QCOMPARE(property.name(), QString::fromAscii("GEO"));
    CHECK_VALUE(property, QVersitProperty::CompoundType,
                QStringList() << QLatin1String("99.9") << QLatin1String("98.9"));
}

void tst_QVersitContactExporter::testEncodeOrganization()
{
    QContact contact(createContactWithName(QLatin1String("asdf")));
    QContactOrganization organization;
    QVersitDocument document;
    QVersitProperty property;
    QString title(QString::fromAscii("Developer"));

    // TITLE
    organization.setTitle(title);
    contact.saveDetail(&organization);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 1);
    property = findPropertyByName(document, QLatin1String("TITLE"));
    QVERIFY(!property.isEmpty());
    QCOMPARE(property.value(), title);

    // ORG with name
    organization.setTitle(QString());
    organization.setName(QString::fromAscii("Nokia"));
    contact.saveDetail(&organization);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 1);
    property = document.properties().at(BASE_PROPERTY_COUNT);
    QCOMPARE(property.name(), QString::fromAscii("ORG"));
    CHECK_VALUE(property, QVersitProperty::CompoundType, QStringList(QLatin1String("Nokia")));

    // ORG with department/unit
    organization.setName(QString());
    QStringList departments(QString::fromAscii("R&D"));
    departments.append(QString::fromAscii("Qt"));
    organization.setDepartment(departments);
    contact.saveDetail(&organization);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 1);
    property = document.properties().at(BASE_PROPERTY_COUNT);
    QCOMPARE(property.name(), QString::fromAscii("ORG"));
    CHECK_VALUE(property, QVersitProperty::CompoundType, QStringList()
                << QString() << QLatin1String("R&D") << QLatin1String("Qt"));

    // ORG with name and department/unit
    organization.setName(QString::fromAscii("Nokia"));
    contact.saveDetail(&organization);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 1);
    property = document.properties().at(BASE_PROPERTY_COUNT);
    QCOMPARE(property.name(), QString::fromAscii("ORG"));
    CHECK_VALUE(property, QVersitProperty::CompoundType, QStringList()
                << QLatin1String("Nokia") << QLatin1String("R&D") << QLatin1String("Qt"));

    // TITLE and ORG
    organization.setTitle(QString::fromAscii("Developer"));
    contact.saveDetail(&organization);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 2);
    property = findPropertyByName(document, QLatin1String("TITLE"));
    QVERIFY(!property.isEmpty());
    QCOMPARE(property.value(), title);
    property = findPropertyByName(document, QLatin1String("ORG"));
    QVERIFY(!property.isEmpty());
    QCOMPARE(property.name(), QString::fromAscii("ORG"));
    CHECK_VALUE(property, QVersitProperty::CompoundType, QStringList()
                << QLatin1String("Nokia") << QLatin1String("R&D") << QLatin1String("Qt"));

    // ORG LOGO Test1: LOGO as remote Resouce
    const QString url = QString::fromAscii("http://myhome.com/test.jpg");
    contact = createContactWithName(QLatin1String("asdf"));
    organization = QContactOrganization();
    organization.setLogoUrl(url);
    contact.saveDetail(&organization);
    mResourceHandler->mSimulatedMimeType = QLatin1String("image/jpeg");
    mExporter->setResourceHandler(mResourceHandler);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    document = mExporter->documents().first();
    QVERIFY(!mResourceHandler->mLoadResourceCalled);

    // Source type is encoded, but media type is not for a URL.
    property = findPropertyByName(document, QLatin1String("LOGO"));
    QVERIFY(!property.isEmpty());
    QCOMPARE(property.parameters().count(), 1);

    QVERIFY(property.parameters().contains(
            QString::fromAscii("VALUE"), QString::fromAscii("URL")));

    //Check property value
    QCOMPARE(property.value(), url);

    // ORG LOGO Test2: LOGO File.
    mResourceHandler->mSimulatedData = "simulated data";
    contact = createContactWithName(QLatin1String("asdf"));
    organization = QContactOrganization();
    organization.setLogoUrl(TEST_PHOTO_FILE);
    contact.saveDetail(&organization);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    document = mExporter->documents().first();
    QVERIFY(mResourceHandler->mLoadResourceCalled);
    QCOMPARE(mResourceHandler->mLocation, TEST_PHOTO_FILE);

    // It should be stored in the property as a QVariant of QByteArray
    property = findPropertyByName(document, QLatin1String("LOGO"));
    QVERIFY(!property.isEmpty());
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
    contact = createContactWithName(QLatin1String("asdf"));
    organization = QContactOrganization();
    organization.setAssistantName(QString::fromAscii("myAssistant"));
    contact.saveDetail(&organization);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 1);
    property = findPropertyByName(document, QLatin1String("X-ASSISTANT"));
    QVERIFY(!property.isEmpty());
    QCOMPARE(property.value(), QString::fromAscii("myAssistant"));

    // Test: Role
    contact = createContactWithName(QLatin1String("asdf"));
    organization = QContactOrganization();
    organization.setRole(QString::fromAscii("Executive"));
    contact.saveDetail(&organization);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 1);
    property = findPropertyByName(document, QLatin1String("ROLE"));
    QVERIFY(!property.isEmpty());
    QCOMPARE(property.value(), QString::fromAscii("Executive"));

}

void tst_QVersitContactExporter::testEncodeAvatar()
{
    QContact contact = createContactWithName(QLatin1String("asdf"));
    QContactAvatar contactAvatar;
    mResourceHandler->mSimulatedData = "simulated data";
    mResourceHandler->mSimulatedMimeType = QLatin1String("image/jpeg");

    // Test1: Web URL
    const QString url = QString::fromAscii("http://www.myhome.com/test.jpg");
    contactAvatar.setImageUrl(url);
    contact.saveDetail(&contactAvatar);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    QVersitDocument document = mExporter->documents().first();
    QVERIFY(document.properties().length() > BASE_PROPERTY_COUNT);
    QVersitProperty property = findPropertyByName(document, QLatin1String("PHOTO"));
    QVERIFY(!property.isEmpty());
    QCOMPARE(property.parameters().count(), 1);
    QCOMPARE(property.value(), url);
    QVERIFY(!mResourceHandler->mLoadResourceCalled);

    // Test 2: Local Media PHOTO
    contactAvatar.setImageUrl(TEST_PHOTO_FILE);
    contact.saveDetail(&contactAvatar);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    document = mExporter->documents().first();
    QVERIFY(mResourceHandler->mLoadResourceCalled);
    QCOMPARE(mResourceHandler->mLocation, TEST_PHOTO_FILE);
    // verify the value
    QVERIFY(document.properties().length() > BASE_PROPERTY_COUNT);
    property = findPropertyByName(document, QLatin1String("PHOTO"));
    QVERIFY(!property.isEmpty());
    QVariant variantValue = property.variantValue();
    QVERIFY(variantValue.type() == QVariant::ByteArray);
    QCOMPARE(variantValue.value<QByteArray>(), mResourceHandler->mSimulatedData);
    QVERIFY(property.parameters().contains(QString::fromAscii("TYPE"),
                                           QString::fromAscii("JPEG")));
}

void tst_QVersitContactExporter::testEncodeThumbnail() {
    QImage image;
    image.loadFromData(SAMPLE_GIF);
    if (QImageWriter::supportedImageFormats().contains("png")) {
        QContactThumbnail thumbnail;
        thumbnail.setThumbnail(image);
        QContact contact(createContactWithName(QLatin1String("asdf")));
        contact.saveDetail(&thumbnail);
        QVERIFY(mExporter->exportContacts(QList<QContact>() << contact,
                                          QVersitDocument::VCard30Type));
        QVersitDocument document = mExporter->documents().first();
        // verify the value
        QVERIFY(document.properties().length() > BASE_PROPERTY_COUNT);
        QVersitProperty property = findPropertyByName(document, QLatin1String("PHOTO"));
        QVERIFY(!property.isEmpty());
        QVariant variantValue = property.variantValue();
        QVERIFY(variantValue.type() == QVariant::ByteArray);
        QByteArray retrievedData = variantValue.value<QByteArray>();
        QImage retrievedImage;
        retrievedImage.loadFromData(retrievedData);
        QCOMPARE(retrievedImage, image);
    }
}


void tst_QVersitContactExporter::testEncodeEmbeddedContent()
{
    QContact contact = createContactWithName(QLatin1String("asdf"));
    QContactAvatar contactAvatar;
    QVariant variantValue;

    // Test 1: URL
    const QString url = QString::fromAscii("http://www.myhome.com/test.jpg");
    contactAvatar.setImageUrl(url);
    contact.saveDetail(&contactAvatar);
    mResourceHandler->mSimulatedMimeType = QLatin1String("image/jpeg");
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    QVersitDocument document = mExporter->documents().first();
    QVERIFY(!mResourceHandler->mLoadResourceCalled);
    QVERIFY(document.properties().size() > BASE_PROPERTY_COUNT);
    QVersitProperty photoProperty = findPropertyByName(document, QLatin1String("PHOTO"));
    QVERIFY(!photoProperty.isEmpty());
    QCOMPARE(photoProperty.parameters().count(), 1);
    QVERIFY(photoProperty.parameters().contains(
            QString::fromAscii("VALUE"),QString::fromAscii("URL")));
    QCOMPARE(photoProperty.value(), url);

    // Test 2: Local PHOTO, image loaded by the loader
    contactAvatar.setImageUrl(TEST_PHOTO_FILE);
    contact.saveDetail(&contactAvatar);
    mResourceHandler->clear();
    mResourceHandler->mSimulatedMimeType = QLatin1String("image/jpeg");
    mResourceHandler->mSimulatedData = "simulated image data";
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    document = mExporter->documents().first();
    QVERIFY(mResourceHandler->mLoadResourceCalled);
    QVERIFY(document.properties().size() > BASE_PROPERTY_COUNT);
    photoProperty = findPropertyByName(document, QLatin1String("PHOTO"));
    QVERIFY(!photoProperty.isEmpty());
    QCOMPARE(photoProperty.parameters().count(), 1);
    QVERIFY(photoProperty.parameters().contains(QString::fromAscii("TYPE"),
                                                QString::fromAscii("JPEG")));
    variantValue = photoProperty.variantValue();
    QVERIFY(variantValue.type() == QVariant::ByteArray);
    QCOMPARE(variantValue.value<QByteArray>(), mResourceHandler->mSimulatedData);

    // Without a resource handler
    mExporter->setResourceHandler(0);
    contactAvatar.setImageUrl(TEST_PHOTO_FILE);
    contact.saveDetail(&contactAvatar);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 0);

    mExporter->setResourceHandler(mResourceHandler);
}

void tst_QVersitContactExporter::testEncodeRingtone()
{
    QContactRingtone ringtone;
    mResourceHandler->clear();
    mResourceHandler->mSimulatedMimeType = QLatin1String("audio/wav");
    mResourceHandler->mSimulatedData = "simulated audio data";
    ringtone.setAudioRingtoneUrl(TEST_AUDIO_FILE);
    QContact contact(createContactWithName(QLatin1String("asdf")));
    contact.saveDetail(&ringtone);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    QVersitDocument document = mExporter->documents().first();
    QVERIFY(mResourceHandler->mLoadResourceCalled);
    QVersitProperty soundProperty = findPropertyByName(document, QLatin1String("SOUND"));
    QVERIFY(!soundProperty.isEmpty());
    QCOMPARE(soundProperty.parameters().count(), 1);
    QVERIFY(soundProperty.parameters().contains(
        QString::fromAscii("TYPE"),
        QString::fromAscii("WAV")));
    QVariant variantValue = soundProperty.variantValue();
    QVERIFY(variantValue.type() == QVariant::ByteArray);
    QCOMPARE(variantValue.value<QByteArray>(), mResourceHandler->mSimulatedData);
}

void tst_QVersitContactExporter::testEncodeParameters()
{
    QContact contact(createContactWithName(QLatin1String("asdf")));
    QContactPhoneNumber phoneNumber;
    phoneNumber.setNumber(QString::fromAscii("12345678"));
    QStringList subtypes;
    subtypes.append(QContactPhoneNumber::SubTypeMobile);
    subtypes.append(QContactPhoneNumber::SubTypeVideo);
    // Add a not supported subtype in vCard, to make sure its not encoded.
    subtypes.append(QContactPhoneNumber::SubTypeDtmfMenu);
    phoneNumber.setSubTypes(subtypes);
    contact.saveDetail(&phoneNumber);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    QVersitDocument document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 1);
    QVersitProperty property = findPropertyByName(document, QLatin1String("TEL"));
    QVERIFY(!property.isEmpty());
    QCOMPARE(property.parameters().count(), 2);
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"), QString::fromAscii("CELL")));
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("VIDEO")));
}

void tst_QVersitContactExporter::testEncodeGender()
{
    QContact contact(createContactWithName(QLatin1String("asdf")));
    QContactGender gender;
    gender.setGender(QContactGender::GenderMale);
    gender.setContexts(QContactGender::ContextHome); // Should not be encoded
    contact.saveDetail(&gender);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    QVersitDocument document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 1);
    QVersitProperty property = findPropertyByName(document, QLatin1String("X-GENDER"));
    QVERIFY(!property.isEmpty());
    QCOMPARE(property.parameters().count(), 0);
    QCOMPARE(property.value(), gender.gender());
}

void tst_QVersitContactExporter::testEncodeNickName()
{
    QContact contact(createContactWithName(QLatin1String("asdf")));

    // Add an extra detail
    QContactGender gender;
    gender.setGender(QContactGender::GenderMale);
    contact.saveDetail(&gender);

    // One nickname given
    QContactNickname firstNickname;
    firstNickname.setNickname(QLatin1String("Homie"));
    contact.saveDetail(&firstNickname);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    QVersitDocument document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 2);
    QVersitProperty property = document.properties().at(BASE_PROPERTY_COUNT+1);
    QCOMPARE(property.name(), QLatin1String("X-NICKNAME"));
    CHECK_VALUE(property, QVersitProperty::ListType, QStringList(QLatin1String("Homie")));

    // Two nicknames given, should be collated into a single property
    contact = createContactWithName(QLatin1String("asdf"));
    contact.saveDetail(&gender);
    contact.saveDetail(&firstNickname);
    QContactNickname secondNickname;
    secondNickname.setNickname(QLatin1String("Jay"));
    contact.saveDetail(&secondNickname);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 2);
    property = findPropertyByName(document, QLatin1String("X-NICKNAME"));
    QVERIFY(!property.isEmpty());
    QCOMPARE(property.name(), QString::fromAscii("X-NICKNAME"));
    CHECK_VALUE(property, QVersitProperty::ListType,
                QStringList() << QLatin1String("Homie") << QLatin1String("Jay"));
}

void tst_QVersitContactExporter::testEncodeTag()
{
    QContact contact(createContactWithName(QLatin1String("asdf")));

    // Add an extra detail
    QContactGender gender;
    gender.setGender(QContactGender::GenderMale);
    contact.saveDetail(&gender);

    // One tag given
    QContactTag firstTag;
    firstTag.setTag(QLatin1String("red"));
    contact.saveDetail(&firstTag);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    QVersitDocument document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 2);
    QVersitProperty property = document.properties().at(BASE_PROPERTY_COUNT+1);
    QCOMPARE(property.name(), QLatin1String("CATEGORIES"));
    CHECK_VALUE(property, QVersitProperty::ListType, QStringList(QLatin1String("red")));

    // Two tags given, should be collated into a single property
    contact = createContactWithName(QLatin1String("asdf"));
    contact.saveDetail(&firstTag);
    contact.saveDetail(&gender);
    QContactTag secondTag;
    secondTag.setTag(QLatin1String("green"));
    contact.saveDetail(&secondTag);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 2);
    property = document.properties().at(BASE_PROPERTY_COUNT+1);
    QCOMPARE(property.name(), QString::fromAscii("CATEGORIES"));
    CHECK_VALUE(property, QVersitProperty::ListType,
                QStringList() << QLatin1String("red") << QLatin1String("green"));
}

void tst_QVersitContactExporter::testEncodeAnniversary()
{
    QContact contact(createContactWithName(QLatin1String("asdf")));
    QContactAnniversary anniversary;
    QDate date(2009,1,1);
    anniversary.setOriginalDate(date);
    anniversary.setContexts(QContactDetail::ContextHome);
    anniversary.setSubType(QContactAnniversary::SubTypeWedding);
    contact.saveDetail(&anniversary);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    QVersitDocument document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 1);
    QVersitProperty property = findPropertyByName(document, QLatin1String("X-ANNIVERSARY"));
    QVERIFY(!property.isEmpty());
    // The contexts and subtypes are not defined for X-ANNIVERSARY property
    QCOMPARE(property.parameters().count(), 0);
    // Check value
    QCOMPARE(property.value(), date.toString(Qt::ISODate));
}


void tst_QVersitContactExporter::testEncodeOnlineAccount()
{
    QContact contact(createContactWithName(QLatin1String("asdf")));
    QContactOnlineAccount onlineAccount;
    QString accountUri(QString::fromAscii("sip:abc@temp.com"));
    onlineAccount.setAccountUri(accountUri);

    // Video sharing
    onlineAccount.setSubTypes(QContactOnlineAccount::SubTypeVideoShare);
    onlineAccount.setContexts(QContactDetail::ContextHome);
    contact.saveDetail(&onlineAccount);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    QVersitDocument document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 1);
    QVersitProperty property = findPropertyByName(document, QLatin1String("X-SIP"));
    QVERIFY(!property.isEmpty());
    // Check parameters
    QCOMPARE(property.parameters().count(), 2);
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("HOME")));
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("SWIS")));
    // Check value
    QCOMPARE(property.value(), accountUri);

    // VoIP
    onlineAccount.setSubTypes(QContactOnlineAccount::SubTypeSipVoip);
    onlineAccount.setContexts(QContactDetail::ContextWork);
    contact.saveDetail(&onlineAccount);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 1);
    property = findPropertyByName(document, QLatin1String("X-SIP"));
    QVERIFY(!property.isEmpty());
    // Check parameters
    QCOMPARE(property.parameters().count(), 2);
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("WORK")));
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("VOIP")));
    // Check value
    QCOMPARE(property.value(), accountUri);

    // Plain SIP
    onlineAccount.setSubTypes(QContactOnlineAccount::SubTypeSip);
    onlineAccount.setContexts(QContactDetail::ContextWork);
    contact.saveDetail(&onlineAccount);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 1);
    property = findPropertyByName(document, QLatin1String("X-SIP"));
    QVERIFY(!property.isEmpty());
    // Check parameters, SIP not added as a TYPE parameter
    QCOMPARE(property.parameters().count(), 1);
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("WORK")));
    // Check value
    QCOMPARE(property.value(), accountUri);

    // IMPP / X-IMPP
    onlineAccount.setSubTypes(QContactOnlineAccount::SubTypeImpp);
    onlineAccount.setContexts(QContactDetail::ContextHome);
    contact.saveDetail(&onlineAccount);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 1);
    property = findPropertyByName(document, QLatin1String("X-IMPP"));
    QVERIFY(!property.isEmpty());
    // Check parameters, SIP not added as a TYPE parameter
    QCOMPARE(property.parameters().count(), 1);
    QVERIFY(property.parameters().contains(
        QString::fromAscii("TYPE"),QString::fromAscii("HOME")));
    // Check value
    QCOMPARE(property.value(), accountUri);

    // Other subtypes not converted
    onlineAccount.setSubTypes(QString::fromAscii("INVALIDSUBTYPE"));
    contact.saveDetail(&onlineAccount);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 0);
}

void tst_QVersitContactExporter::testEncodeFamily()
{
    QContact contact(createContactWithName(QLatin1String("asdf")));
    QContactFamily family;

    // No spouse, no family
    family.setContexts(QContactDetail::ContextHome);
    contact.saveDetail(&family);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    QVersitDocument document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 0);

    // Only spouse present
    QString spouce = QString::fromAscii("ABC");
    family.setSpouse(spouce);
    contact.saveDetail(&family);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 1);
    QVersitProperty spouseProperty = findPropertyByName(document, QLatin1String("X-SPOUSE"));
    QVERIFY(!spouseProperty.isEmpty());
    QCOMPARE(spouseProperty.parameters().count(), 0);
    QCOMPARE(spouseProperty.value(), spouce);

    // Spouse and children
    QStringList children;
    children << QString::fromAscii("A") << QString::fromAscii("B") ;
    family.setChildren(children);
    family.setSpouse(spouce);
    contact.saveDetail(&family);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    document = mExporter->documents().first();
    QCOMPARE(countProperties(document), 2);
    spouseProperty = findPropertyByName(document, QLatin1String("X-SPOUSE"));
    QVERIFY(!spouseProperty.isEmpty());
    QCOMPARE(spouseProperty.parameters().count(), 0);
    QCOMPARE(spouseProperty.value(), spouce);
    QVersitProperty childrenProperty = findPropertyByName(document, QLatin1String("X-CHILDREN"));
    QVERIFY(!spouseProperty.isEmpty());
    QCOMPARE(childrenProperty.parameters().count(), 0);
    QCOMPARE(childrenProperty.name(), QString::fromAscii("X-CHILDREN"));
    CHECK_VALUE(childrenProperty, QVersitProperty::ListType, children);
}


void tst_QVersitContactExporter::testEncodeDisplayLabel()
{
    QContact contact;
    QContactName contactName;

    // No display label, but QContactName found
    contactName.setFirstName(QString::fromAscii("First"));
    contactName.setLastName(QString::fromAscii("Last"));
    contactName.setMiddleName(QString::fromAscii("Middle"));
    contact.saveDetail(&contactName);
    QVERIFY(mExporter->exportContacts(QList<QContact>() << contact, QVersitDocument::VCard21Type));
    QVersitDocument document = mExporter->documents().first();
    QCOMPARE(document.properties().count(), 2);
    QVersitProperty displayProperty = findPropertyByName(document, QLatin1String("FN"));
    QCOMPARE(displayProperty.name(), QString::fromAscii("FN"));
    QCOMPARE(displayProperty.value(), QString::fromAscii("First Last"));
    QVersitProperty nameProperty = findPropertyByName(document, QLatin1String("N"));
    QCOMPARE(nameProperty.name(), QString::fromAscii("N"));
    CHECK_VALUE(nameProperty, QVersitProperty::CompoundType, QStringList()
                << QLatin1String("Last") << QLatin1String("First") << QLatin1String("Middle")
                << QString() << QString());
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
QContact tst_QVersitContactExporter::createContactWithName(QString name)
{
    QContact contact;
    QContactName nameDetail;
    nameDetail.setFirstName(name);
    contact.saveDetail(&nameDetail);
    return contact;
}

/* Counts the properties, excluding the ones generated by a
 * "createContactWithName" contact and excluding default-generated properties
 */
int tst_QVersitContactExporter::countProperties(const QVersitDocument& document)
{
    int count = 0;
    foreach (const QVersitProperty& property, document.properties()) {
        if (property.name() != QLatin1String("FN")
            && property.name() != QLatin1String("N")
            && property.name() != QLatin1String("X-NOKIA-QCONTACTFIELD"))
            count++;
    }
    return count;
}

QContactDetail tst_QVersitContactExporter::findDetailByName(
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

QVersitProperty tst_QVersitContactExporter::findPropertyByName(
        const QVersitDocument &document, const QString &propertyName)
{
    foreach (const QVersitProperty& property, document.properties()) {
        if (property.name() == propertyName)
            return property;
    }
    return QVersitProperty();
}

QTEST_MAIN(tst_QVersitContactExporter)

