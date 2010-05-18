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

#include <QtTest/QtTest>
#include <QObject>

#include <qtcontacts.h>

#ifdef SYMBIANSIM_BACKEND_USE_ETEL_TESTSERVER
#include <etelmm_etel_test_server.h>
#else
#include <etelmm.h>
#endif
#include <mmtsy_names.h>

QTM_USE_NAMESPACE

typedef QList<QContactLocalId> QContactIds;
Q_DECLARE_METATYPE(QContactIds);

#ifndef QTRY_COMPARE
#define QTRY_COMPARE(__expr, __expected) \
    do { \
        const int __step = 50; \
        const int __timeout = 5000; \
        if ((__expr) != (__expected)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && ((__expr) != (__expected)); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QCOMPARE(__expr, __expected); \
    } while(0)
#endif

//TESTED_CLASS=
//TESTED_FILES=

/*!
*/
class tst_SimCM : public QObject
{
Q_OBJECT

public:
    tst_SimCM();
    virtual ~tst_SimCM();

public slots:
    void initTestCase();
    void cleanupTestCase();

private slots:
    /* Test cases that need data */
    void initManager_data();
    void initManager();
    void hasFeature_data();
    void hasFeature();
    void supportedContactTypes_data();
    void supportedContactTypes();
    void detailDefinitions_data();
    void detailDefinitions();
    void addContact_data();
    void addContact();
    void fetchContacts_data();
    void fetchContacts();
    void updateContactDetail_data();
    void updateContactDetail();
    void batchOperations_data();
    void batchOperations();
    void detailFilter_data();
    void detailFilter();
    void sortingAdn_data();
    void sortingAdn();

    /* Test cases that take no data */
    void signalEmission();
    void sdnContacts();
    void fillSlots();

private:
    void initManager(QString simStore);
    void getEtelStoreInfoL(const TDesC &phonebook, TDes8 &infoPckg) const;
    bool isContactSupported(QContact contact);
    void parseDetails(QContact &contact, QStringList details, QList<QContactDetail> &parsedDetails);
    bool compareDetails(QContact contact, QList<QContactDetail> expectedDetails);
    QContact createContact(QString name, QString number);
    QContact saveContact(QString name, QString number);
    void dumpStoreInfo();
    bool compareContactLists(QList<QContact> lista, QList<QContact> listb);
    bool compareContacts(QContact ca, QContact cb);
    

private:
    QContactManager* m_cm;
#ifdef SYMBIANSIM_BACKEND_PHONEBOOKINFOV1
    RMobilePhoneBookStore::TMobilePhoneBookInfoV1 m_etelStoreInfo;
    RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg m_etelStoreInfoPckg;
#else
    RMobilePhoneBookStore::TMobilePhoneBookInfoV5 m_etelStoreInfo;
    RMobilePhoneBookStore::TMobilePhoneBookInfoV5Pckg m_etelStoreInfoPckg;
#endif
};

tst_SimCM::tst_SimCM() :
    m_cm(0),
    m_etelStoreInfoPckg(m_etelStoreInfo)
{
}

tst_SimCM::~tst_SimCM()
{
}

void tst_SimCM::initTestCase()
{
    initManager(QString());

    // TODO: how about other stores?
    TRAPD(err, getEtelStoreInfoL(KETelIccAdnPhoneBook, m_etelStoreInfoPckg));
    QCOMPARE(err, KErrNone);

    // remove all contacts
    m_cm->removeContacts(m_cm->contactIds(), 0);   
}

void tst_SimCM::cleanupTestCase()
{
    // remove all contacts
    m_cm->removeContacts(m_cm->contactIds(), 0);   
    delete m_cm;
    m_cm = 0;
}

void tst_SimCM::initManager_data()
{
    QTest::addColumn<QString>("simStore"); // empty (defaults to ADN), "ADN", "SDN" or "FDN"

    QString es;

    QTest::newRow("Empty store string (defaults to ADN store)") << es;
    QTest::newRow("Initialize SDN store") << "SDN";
    QTest::newRow("Initialize FDN store") << "FDN";
    QTest::newRow("Initialize ADN store") << "ADN";
}

void tst_SimCM::initManager()
{
    delete m_cm;
    m_cm = 0;

    QString uri("qtcontacts:symbiansim");

    // Set the sim store if available (simbackend defaults to ADN if not available)
    QFETCH(QString, simStore);
    if(!simStore.isEmpty()) {
        uri.append(":store=");
        uri.append(simStore);
    }

    m_cm = QContactManager::fromUri(uri);
    QVERIFY(m_cm);

    // NotSupportedError is allowed (the sim card may not support FDN and SDN stores)
    if(m_cm->error() == QContactManager::NotSupportedError) {
        QSKIP("The store not supported by the SIM card", SkipSingle);
    } else {
        QCOMPARE(m_cm->error(), QContactManager::NoError);
    }
}

void tst_SimCM::initManager(QString simStore)
{
    delete m_cm;
    m_cm = 0;

    QString uri("qtcontacts:symbiansim");

    // Set the sim store if available (simbackend defaults to ADN if not available)
    if(!simStore.isEmpty()) {
        uri.append(":store=");
        uri.append(simStore);
    }

    m_cm = QContactManager::fromUri(uri);
}

void tst_SimCM::hasFeature_data()
{
    QTest::addColumn<QString>("simStore");      // empty (defaults to ADN), "ADN", "SDN" or "FDN"
    QTest::addColumn<int>("managerFeature");              // one of QContactManager::ManagerFeature
    QTest::addColumn<bool>("expectedResult");   // true = has feature, false = does not have feature
    QString es;

    QTest::newRow("ADN store (default)") << es << (int) QContactManager::Groups << false;
    QTest::newRow("ADN store (default)") << es << (int) QContactManager::ActionPreferences << false;
    QTest::newRow("ADN store (default)") << es << (int) QContactManager::MutableDefinitions << false;
    QTest::newRow("ADN store (default)") << es << (int) QContactManager::Relationships << false;
    QTest::newRow("ADN store (default)") << es << (int) QContactManager::ArbitraryRelationshipTypes << false;
    // TODO: self contact may be supported on ADN? (so called "own number store")
    QTest::newRow("ADN store (default)") << es << (int) QContactManager::SelfContact << false;
    QTest::newRow("ADN store (default)") << es << (int) QContactManager::Anonymous << false;
    QTest::newRow("ADN store (default)") << es << (int) QContactManager::ChangeLogs << false;

    QTest::newRow("ADN store") << "ADN" << (int) QContactManager::Groups << false;
    QTest::newRow("ADN store") << "ADN" << (int) QContactManager::ActionPreferences << false;
    QTest::newRow("ADN store") << "ADN" << (int) QContactManager::MutableDefinitions << false;
    QTest::newRow("ADN store") << "ADN" << (int) QContactManager::Relationships << false;
    QTest::newRow("ADN store") << "ADN" << (int) QContactManager::ArbitraryRelationshipTypes << false;
    // TODO: self contact may be supported on ADN? (so called "own number store")
    QTest::newRow("ADN store") << "ADN" << (int) QContactManager::SelfContact << false;
    QTest::newRow("ADN store") << "ADN" << (int) QContactManager::Anonymous << false;
    QTest::newRow("ADN store") << "ADN" << (int) QContactManager::ChangeLogs << false;

    QTest::newRow("SDN store") << "SDN" << (int) QContactManager::Groups << false;
    QTest::newRow("SDN store") << "SDN" << (int) QContactManager::ActionPreferences << false;
    QTest::newRow("SDN store") << "SDN" << (int) QContactManager::MutableDefinitions << false;
    QTest::newRow("SDN store") << "SDN" << (int) QContactManager::Relationships << false;
    QTest::newRow("SDN store") << "SDN" << (int) QContactManager::ArbitraryRelationshipTypes << false;
    QTest::newRow("SDN store") << "SDN" << (int) QContactManager::SelfContact << false;
    QTest::newRow("SDN store") << "SDN" << (int) QContactManager::Anonymous << false;
    QTest::newRow("SDN store") << "SDN" << (int) QContactManager::ChangeLogs << false;

    QTest::newRow("FDN store") << "FDN" << (int) QContactManager::Groups << false;
    QTest::newRow("FDN store") << "FDN" << (int) QContactManager::ActionPreferences << false;
    QTest::newRow("FDN store") << "FDN" << (int) QContactManager::MutableDefinitions << false;
    QTest::newRow("FDN store") << "FDN" << (int) QContactManager::Relationships << false;
    QTest::newRow("FDN store") << "FDN" << (int) QContactManager::ArbitraryRelationshipTypes << false;
    QTest::newRow("FDN store") << "FDN" << (int) QContactManager::SelfContact << false;
    QTest::newRow("FDN store") << "FDN" << (int) QContactManager::Anonymous << false;
    QTest::newRow("FDN store") << "FDN" << (int) QContactManager::ChangeLogs << false;
}

void tst_SimCM::hasFeature()
{
    // initialize
    QFETCH(QString, simStore);
    initManager(simStore);
    QVERIFY(m_cm);
    if(m_cm->error() == QContactManager::NotSupportedError) {
        QSKIP("The store not supported by the SIM card", SkipSingle);
    }
    QFETCH(bool, expectedResult);
    QFETCH(int, managerFeature);

    // verify
    QCOMPARE(m_cm->hasFeature((QContactManager::ManagerFeature) managerFeature), expectedResult);
    QCOMPARE(m_cm->hasFeature((QContactManager::ManagerFeature) managerFeature, QContactType::TypeContact), expectedResult);
    QCOMPARE(m_cm->hasFeature((QContactManager::ManagerFeature) managerFeature, QContactType::TypeGroup), expectedResult);
}

void tst_SimCM::supportedContactTypes_data()
{
    QTest::addColumn<QString>("simStore");
    QTest::newRow("custom label") << "ADN";
    QTest::newRow("custom label") << "SDN";
    QTest::newRow("custom label") << "FDN";
}

void tst_SimCM::supportedContactTypes()
{
    QFETCH(QString, simStore);
    initManager(simStore);
    QVERIFY(m_cm);
    if(m_cm->error() == QContactManager::NotSupportedError) {
        QSKIP("The store not supported by the SIM card", SkipSingle);
    }

    QStringList types = m_cm->supportedContactTypes();
    QCOMPARE(m_cm->error(), QContactManager::NoError);
    QVERIFY(types.count() > 0);
    // Contacts should always be supported
    QVERIFY(types.contains(QContactType::TypeContact));
    // Groups should not be supported
    QVERIFY(!types.contains(QContactType::TypeGroup));
}

void tst_SimCM::detailDefinitions_data()
{
    QTest::addColumn<QString>("simStore");
    QTest::newRow("custom label") << "ADN";
    QTest::newRow("custom label") << "SDN";
    QTest::newRow("custom label") << "FDN";
}

void tst_SimCM::detailDefinitions()
{
    QFETCH(QString, simStore);
    initManager(simStore);
    QVERIFY(m_cm);
    if(m_cm->error() == QContactManager::NotSupportedError) {
        QSKIP("The store not supported by the SIM card", SkipSingle);
    }

    QMap<QString, QContactDetailDefinition> detailDefinitions = m_cm->detailDefinitions();
    QCOMPARE(m_cm->error(), QContactManager::NoError);

    QVERIFY(detailDefinitions.count() >= 2);
    QVERIFY(detailDefinitions.count() <= 7);

    // check that at least definitions for name and phone number exist
    QVERIFY(detailDefinitions.contains(QContactName::DefinitionName));
    QVERIFY(detailDefinitions.contains(QContactPhoneNumber::DefinitionName));
    QVERIFY(detailDefinitions.contains(QContactDisplayLabel::DefinitionName));
    QVERIFY(detailDefinitions.contains(QContactType::DefinitionName));
    QVERIFY(detailDefinitions.contains(QContactSyncTarget::DefinitionName));

    // Dynamic definitions (that depend on SIM card type)
    if(detailDefinitions.count() == 7) {
        QVERIFY(detailDefinitions.contains(QContactNickname::DefinitionName));
        QVERIFY(detailDefinitions.contains(QContactEmailAddress::DefinitionName));
    }
}

void tst_SimCM::addContact_data()
{
    QTest::addColumn<QString>("simStore"); // empty (defaults to ADN), "ADN", "SDN" or "FDN"
    QTest::addColumn<int>("expectedResult"); // 1 = pass, 0 = fail, -1 = depends on the SIM card
    QTest::addColumn<QString>("expectedDisplayLabel");
    QTest::addColumn<QStringList>("details"); // format is <detail definition name>:<field name>:<value>
    QString es;
    QString tooLongText("James Hunt the 12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890th");

#ifdef __WINS__
    qWarning("Etel test server (emulator) does report an error when saving if contact has too long details!");
#endif

    // Note: With the current implementation the value must not contain a ':' character
    QTest::newRow("ADN custom label")
        << QString("ADN")
        << 1 // expected to pass
        << "James"
        << (QStringList()
            << "Name:CustomLabel:James");

    QTest::newRow("ADN (explicitly defined store) custom label")
        << QString("ADN")
        << 1 // expected to pass
        << "James"
        << (QStringList()
            << "Name:CustomLabel:James");

    QTest::newRow("ADN custom label 2")
        << QString("ADN")
        << 1 // expected to pass
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt");

    QTest::newRow("ADN 2 custom labels")
        << QString("ADN")
        << 0 // expected to fail. Custom label is unique.
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James"
            << "Name:CustomLabel:James Hunt");

    QTest::newRow("ADN too long custom label")
        << QString("ADN")
#ifdef __WINS__
        << 1 // Etel test server does not enforce the max lengths
#else
        << 0 // expected to fail
#endif
        << tooLongText
        << (QStringList()
            << (QString("Name:CustomLabel:").append(tooLongText)));

    QTest::newRow("ADN custom label and nick name")
        << QString("ADN")
        << -1 // Depends on SIM card support (some cards support second name)
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "Nickname:Nickname:Hunt the Shunt");

    QTest::newRow("ADN custom label and too long nick name")
        << QString("ADN")
#ifdef __WINS__
        << 1 // Etel test server does not enforce the max lengths
#else
        << 0 // expected to fail
#endif
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << (QString("Nickname:Nickname:").append(tooLongText)));

    QTest::newRow("ADN phone number")
        << QString("ADN")
        << 1
        << es
        << (QStringList()
            << "PhoneNumber:PhoneNumber:+44752222222");

    QTest::newRow("ADN empty name and phone number")
        << QString("ADN")
        << 1
        << es
        << (QStringList()
            << "Name:CustomLabel:"
            << "PhoneNumber:PhoneNumber:+44752222222");

    QTest::newRow("ADN custom label and phone number")
        << QString("ADN")
        << 1
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "PhoneNumber:PhoneNumber:+44752222222");

    QTest::newRow("ADN custom label and funny (but legal) phone number")
        << QString("ADN")
        << 1
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "PhoneNumber:PhoneNumber:+0123456789*#p");

    QTest::newRow("ADN custom label and illegal phone number 1")
        << QString("ADN")
        << 0 // illegal characters in the phone number, should fail
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "PhoneNumber:PhoneNumber:+44(75)2222222");

    QTest::newRow("ADN custom label and illegal phone number 2")
        << QString("ADN")
        << 0 // illegal characters in the phone number, should fail
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "PhoneNumber:PhoneNumber:asdfqwer");

    QTest::newRow("ADN custom label and illegal phone number 3")
        << QString("ADN")
        << 0 // illegal characters in the phone number, should fail
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "PhoneNumber:PhoneNumber:1234567a");

    QTest::newRow("ADN custom label and illegal phone number 4")
        << QString("ADN")
        << 0 // illegal characters in the phone number, should fail
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "PhoneNumber:PhoneNumber:&1234567");

    QTest::newRow("ADN custom label and too long phone number")
        << QString("ADN")
        << 0 // long enough phone number to fail on any SIM card
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "PhoneNumber:PhoneNumber:1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");

    QTest::newRow("ADN custom label and multiple phone numbers")
        << QString("ADN")
        << -1 // some SIM cards support multiple phone numbers
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "PhoneNumber:PhoneNumber:+44752222222"
            << "PhoneNumber:PhoneNumber:+44751111111");

    QTest::newRow("ADN custom label and multiple phone numbers, one phone number too long")
        << QString("ADN")
        << 0 // Long enough additional phone number to fail on any SIM card
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "PhoneNumber:PhoneNumber:+44752222222"
            << "PhoneNumber:PhoneNumber:1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");

    QTest::newRow("ADN custom label and email")
        << QString("ADN")
        << -1 // some SIM cards support e-mail address
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "EmailAddress:EmailAddress:james@mclaren.com");

    QTest::newRow("ADN custom label and multiple emails")
        << QString("ADN")
        << 0 // some SIM cards support multiple e-mail addresses, but not this many
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "EmailAddress:EmailAddress:james@march.com"
            << "EmailAddress:EmailAddress:james@hesketh.com"
            << "EmailAddress:EmailAddress:james@mclaren.com"
            << "EmailAddress:EmailAddress:james.hunt@bbc.co.uk");

    QTest::newRow("ADN custom label and too long email")
        << QString("ADN")
#ifdef __WINS__
        << 1 // Etel test server does not enforce the max lengths
#else
        << 0 // long enough e-mail to fail on any SIM card
#endif
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "EmailAddress:EmailAddress:james.hunt.the12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890th@mclaren.com");

    QTest::newRow("ADN non-supported field")
        << QString("ADN")
        << 0 // expected to fail
        << es
        << (QStringList()
            << "Name:IllegalNameDetailFieldName:James");

    QTest::newRow("ADN non-supported detail")
        << QString("ADN")
        << 0 // expected to fail
        << es
        << (QStringList()
            << "NotSupportedDetailDefinitionName:IllegalFieldName:FieldValue");

    QTest::newRow("ADN empty, non-supported detail")
        << QString("ADN")
        << 0 // expected to fail, since no valid details provided
        << es
        << (QStringList()
            << "NotSupportedDetailDefinitionName:IllegalFieldName:");

    QTest::newRow("SDN custom label")
        << "SDN"
        << 0 // It is not possible to save an SDN contact
        << "James"
        << (QStringList()
            << "Name:CustomLabel:James");

    // Note: Executing FDN test cases has a pre-condition that the user must
    // have been entered the PIN2 code successfully. On pre-10.1 platforms this
    // can be done by opening S60 platform Phonebook and making some
    // modifications that require PIN2 code; for example activate and
    // de-activate FDN feature.
    QTest::newRow("FDN custom label and phone number")
        << "FDN"
        << 1
        << "James"
        << (QStringList()
            << "Name:CustomLabel:James"
            << "PhoneNumber:PhoneNumber:+44752222222");
}

/*
 * Tests if different contacts can be saved to SIM card.
 * Steps:
 * 1. Parse contact details from test parameters
 * 2. Determine the expected result
 * 3.1 (if expected to pass) Save contact, verify result and remove contact
 * 3.2 (if expected to fail) Check that saving a contact fails
 */
void tst_SimCM::addContact()
{
    // Make debugging easier by getting the test case name
    QString tescaseName = QTest::currentDataTag();

    // init
    QFETCH(QString, simStore);
    initManager(simStore);
    QVERIFY(m_cm);
    if(m_cm->error() == QContactManager::NotSupportedError) {
        QSKIP("The store not supported by the SIM card", SkipSingle);
    }
    QFETCH(int, expectedResult);
    QFETCH(QString, expectedDisplayLabel);
    QFETCH(QStringList, details);

    QContact contact;
    QList<QContactDetail> expectedDetails;

    // 1. Parse details and add them to the contact
    parseDetails(contact, details, expectedDetails);

    // 2. Determine the expected result
    if (expectedResult == -1) {
        // Unknown expected result, so we need to check what details the SIM
        // card supports
        if (isContactSupported(contact)) {
            expectedResult = 1; 
        } else {
            expectedResult = 0;
        }
    }

    // Get the contact count for verification purposes
    QList<QContactLocalId> idsBefore = m_cm->contactIds();
    QVERIFY(m_cm->error() == QContactManager::NoError || m_cm->error() == QContactManager::DoesNotExistError);

    // 3.1 (if expected to pass) Save contact, verify result and remove contact
    if (expectedResult)
    {
        // verify contact can be saved
        QVERIFY(m_cm->saveContact(&contact));
        QCOMPARE(m_cm->error(), QContactManager::NoError);
        QList<QContactLocalId> idsAfterSave = m_cm->contactIds();
        QCOMPARE(idsAfterSave.count(), idsBefore.count() + 1);

        // verify contact id
        QVERIFY(contact.id() != QContactId());

        // verify that the details were saved as expected
        QVERIFY(compareDetails(contact, expectedDetails));

        // verify display label
        QCOMPARE(contact.displayLabel(), expectedDisplayLabel);

        // TODO: verify that no extra details were added?
        //?QCOMPARE(contact.details().count(), detailsUnderTest.count() + 2);

        // Read the contact from SIM and re-verify the result
        contact = m_cm->contact(contact.localId());
        QVERIFY(compareDetails(contact, expectedDetails));
        QCOMPARE(contact.displayLabel(), expectedDisplayLabel);

        // verify contact removal
        QVERIFY(m_cm->removeContact(contact.localId()));
        QCOMPARE(m_cm->error(), QContactManager::NoError);

    // 3.2 (if expected to fail) Check that saving a contact fails
    } else {
        // verify that the contact cannot be saved
        QVERIFY(!m_cm->saveContact(&contact));
        // TODO: what is the expected error code? does it depend on the case?

        // verify contact id is untouched 
        QVERIFY(contact.id() == QContactId());
    }

    QList<QContactLocalId> idsAfterRemove = m_cm->contactIds();
    QCOMPARE(idsAfterRemove.count(), idsBefore.count());
}

void tst_SimCM::fetchContacts_data()
{
    QTest::addColumn<QString>("simStore");
    QTest::addColumn<int>("contactCount");

    QTest::newRow("ADN")
        << QString("ADN")
        << int(10);

    QTest::newRow("SDN")
        << QString("SDN")
        << int(0); // You cannot save a contact to SDN

    // Requires that PIN2 has been given (for example by activating and
    // de-activating FDN via S60 Phonebook)
    QTest::newRow("FDN")
        << QString("FDN")
        << int(1);
}

/*
 * Tests if QContactManager::contacts and ::contactIds work.
 */
void tst_SimCM::fetchContacts()
{
    // init
    QFETCH(QString, simStore);
    initManager(simStore);
    QVERIFY(m_cm);
    if(m_cm->error() == QContactManager::NotSupportedError) {
        QSKIP("The store not supported by the SIM card", SkipSingle);
    }
    QFETCH(int, contactCount);

    const int existingContactCount = m_cm->contactIds().count();

    QList<QContactLocalId> addedContacts = QList<QContactLocalId> ();

    // 1. Add contacts (optionally)
    for(int i(0); i < contactCount; i++) {
        QContact contact = createContact(
            QString("James").append(QString::number(i + 1)),
            QString("1234567890").append(QString::number(i + 1)));

        QVERIFY(m_cm->saveContact(&contact));
        QCOMPARE(m_cm->error(), QContactManager::NoError);
        QVERIFY(contact.id() != QContactId());
        addedContacts.append(contact.localId());
    }

    // 2. Fetch contacts
    QList<QContact> contacts= m_cm->contacts();
    QCOMPARE(m_cm->error(), QContactManager::NoError);
    QList<QContactLocalId> contactIds = m_cm->contactIds();
    QCOMPARE(m_cm->error(), QContactManager::NoError);

    // 3. Verify result
    QVERIFY(contacts.count() > 0);
    QCOMPARE(contacts.count(), existingContactCount + contactCount);
    foreach (const QContact& contact, contacts) {
        QVERIFY(contact.id() != QContactId());
    }

    QVERIFY(contactIds.count() > 0);
    QCOMPARE(contactIds.count(), existingContactCount + contactCount);
    foreach (QContactLocalId id, addedContacts) {
        QVERIFY(id != QContactLocalId());
    }

    // 4. Remove contacts
    foreach (QContactLocalId id, addedContacts) {
        QVERIFY(m_cm->removeContact(id));
        QCOMPARE(m_cm->error(), QContactManager::NoError);
    }

}

void tst_SimCM::updateContactDetail_data()
{
    QTest::addColumn<QString>("simStore");
    // The initial contact details,
    // for example: <"Name:First:James">; <"PhoneNumber:PhoneNumber:1234567890">
    QTest::addColumn<QStringList>("initialDetails");
    // The updated contact details,
    // for example: <"Name:First:James">; <"PhoneNumber:PhoneNumber:0987654321">
    QTest::addColumn<QStringList>("updatedDetails");

    QTest::newRow("update custom label")
        << QString("ADN")
        << (QStringList()
            << "Name:CustomLabel:James")
        << (QStringList()
            << "Name:CustomLabel:James Hunt");

    QTest::newRow("add phone number detail")
        << QString("ADN")
        << (QStringList()
            << "Name:CustomLabel:James")
        << (QStringList()
            << "Name:CustomLabel:James"
            << "PhoneNumber:PhoneNumber:+44752222222");

    QTest::newRow("update phone number detail")
        << QString("ADN")
        << (QStringList()
            << "Name:CustomLabel:James"
            << "PhoneNumber:PhoneNumber:+44751111111")
        << (QStringList()
            << "Name:CustomLabel:James"
            << "PhoneNumber:PhoneNumber:+44752222222");

    QTest::newRow("remove phone number detail")
        << QString("ADN")
        << (QStringList()
            << "Name:CustomLabel:James"
            << "PhoneNumber:PhoneNumber:+44751111111")
        << (QStringList()
            << "Name:CustomLabel:James");

    QTest::newRow("add e-mail detail")
        << QString("ADN")
        << (QStringList()
            << "Name:CustomLabel:James")
        << (QStringList()
            << "Name:CustomLabel:James"
            << "EmailAddress:EmailAddress:james@hesketh.com");

    QTest::newRow("update e-mail detail")
        << QString("ADN")
        << (QStringList()
            << "Name:CustomLabel:James"
            << "EmailAddress:EmailAddress:james@march.com")
        << (QStringList()
            << "Name:CustomLabel:James"
            << "EmailAddress:EmailAddress:james@hesketh.com");

    QTest::newRow("remove e-mail detail")
        << QString("ADN")
        << (QStringList()
            << "Name:CustomLabel:James"
            << "EmailAddress:EmailAddress:james@march.com")
        << (QStringList()
            << "Name:CustomLabel:James");

    QTest::newRow("add nickname detail")
        << QString("ADN")
        << (QStringList()
            << "Name:CustomLabel:James")
        << (QStringList()
            << "Name:CustomLabel:James"
            << "Nickname:Nickname:Hunt the Shunt");

    QTest::newRow("update nickname detail")
        << QString("ADN")
        << (QStringList()
            << "Name:CustomLabel:James"
            << "Nickname:Nickname:James")
        << (QStringList()
            << "Name:CustomLabel:James"
            << "Nickname:Nickname:Hunt the Shunt");

    QTest::newRow("remove nickname detail")
        << QString("ADN")
        << (QStringList()
            << "Name:CustomLabel:James"
            << "Nickname:Nickname:James")
        << (QStringList()
            << "Name:CustomLabel:James");
}

/*
 * Tests if SIM contacts can be changed. I.e. add, update and remove contact
 * details. Steps:
 * 1. Parse details from test data
 * 2. Add a contact with initial details
 * 3. Modify the contact (save with updated details)
 * 4. Remove the contact
 */
void tst_SimCM::updateContactDetail()
{
    QString tescaseName = QTest::currentDataTag();

    // init
    QFETCH(QString, simStore);
    initManager(simStore);
    QVERIFY(m_cm);
    if(m_cm->error() == QContactManager::NotSupportedError) {
        QSKIP("The store not supported by the SIM card", SkipSingle);
    }
    QFETCH(QStringList, initialDetails);
    QFETCH(QStringList, updatedDetails);

    // 1. Parse details
    QContact contact;
    QList<QContactDetail> parsedDetails;
    parseDetails(contact, initialDetails, parsedDetails);

    // 2. Save contact and verify result
    if (!isContactSupported(contact)) {
        QSKIP("The contact cannot be saved onto the SIM card", SkipSingle);
    }
    QVERIFY(m_cm->saveContact(&contact));
    QCOMPARE(m_cm->error(), QContactManager::NoError);
    QVERIFY(compareDetails(contact, parsedDetails));

    // 3. Update contact detail and verify result
    foreach (const QContactDetail& detail, parsedDetails) {
        QContactDetail savedDetail = contact.detail(detail.definitionName());
        QVERIFY(contact.removeDetail(&savedDetail));
    }
    parseDetails(contact, updatedDetails, parsedDetails);
    if (!isContactSupported(contact)) {
        QVERIFY(m_cm->removeContact(contact.localId()));
        QSKIP("The contact cannot be saved onto the SIM card", SkipSingle);
    }
    QVERIFY(m_cm->saveContact(&contact));
    QCOMPARE(m_cm->error(), QContactManager::NoError);
    QVERIFY(compareDetails(contact, parsedDetails));

    // 4. Remove the contact
    QVERIFY(m_cm->removeContact(contact.localId()));
}

void tst_SimCM::batchOperations_data()
{
    QTest::addColumn<QString>("simStore");
    QTest::addColumn<int>("contactCount");
    QTest::addColumn<bool>("expectedResult");

    QTest::newRow("ADN")
        << QString("ADN")
        << 10
        << true;

    QTest::newRow("SDN")
        << QString("SDN")
        << 10
        << false; // You cannot save contacts to SDN

    // Requires that PIN2 has been given (for example by activating and
    // de-activating FDN via S60 Phonebook)
    QTest::newRow("FDN")
        << QString("FDN")
        << 10
        << true;
}

/*
 * Tests batch operations saveContacts and removeContacts.
 * 1. Add contacts
 * 2. Update contacts
 * 3. Remove contacts
 */
void tst_SimCM::batchOperations()
{
    // init
    QFETCH(QString, simStore);
    initManager(simStore);
    QVERIFY(m_cm);
    if(m_cm->error() == QContactManager::NotSupportedError) {
        QSKIP("The store not supported by the SIM card", SkipSingle);
    }
    QFETCH(int, contactCount);
    QFETCH(bool, expectedResult);

    QMap<int, QContactManager::Error> errorMap;
    QList<QContact> contacts;
    for(int i(0); i < contactCount; i++) {
        QContact contact = createContact(
            QString("James").append(QString::number(i + 1)),
            QString("1234567890").append(QString::number(i + 1)));
        contacts.append(contact);
    }

    // 1. Add contacts
    if (expectedResult) {
        QVERIFY(m_cm->saveContacts(&contacts, &errorMap));
        QCOMPARE(m_cm->error(), QContactManager::NoError);
        QCOMPARE(errorMap.count(), 0);
        foreach (const QContact& contact, contacts) {
            QVERIFY(contact.id() != QContactId());
        }
    } else {
        QVERIFY(!m_cm->saveContacts(&contacts, &errorMap));
        QVERIFY(m_cm->error() != QContactManager::NoError);
        QCOMPARE(errorMap.count(), 10);
        foreach (const QContact& contact, contacts) {
            QCOMPARE(contact.id(), QContactId());
        }
    }

    // 2. Update contacts (updates name of each contact)
    if(expectedResult) {
        foreach (QContact contact, contacts) {
            int index = contacts.indexOf(contact);
            QContactName name = contact.detail(QContactName::DefinitionName);
            name.setCustomLabel(name.customLabel() + QString("u"));
            contact.saveDetail(&name);
            contacts.replace(index, contact);
        }
        QVERIFY(m_cm->saveContacts(&contacts, &errorMap));
        QCOMPARE(m_cm->error(), QContactManager::NoError);
        QCOMPARE(errorMap.count(), 0);
    }

    // 3. Remove contacts
    if(expectedResult) {
        QList<QContactLocalId> contactIds;
        foreach (const QContact& contact, contacts) {
            contactIds.append(contact.localId());
        }
        QVERIFY(m_cm->removeContacts(contactIds, &errorMap));
        QCOMPARE(m_cm->error(), QContactManager::NoError);
        QCOMPARE(errorMap.count(), 0);
    }
}

void tst_SimCM::detailFilter_data()
{
    QTest::addColumn<QString>("detailName");
    QTest::addColumn<QString>("detailField");
    QTest::addColumn<QString>("value");
    QTest::addColumn<int>("flags");
    QTest::addColumn<bool>("filterSupported");
    QTest::addColumn<QString>("expected");

    // Phone number
    
    QString detail = QContactPhoneNumber::DefinitionName;
    QString field = QContactPhoneNumber::FieldNumber;
    
    QTest::newRow("phonenumber=123456789, flags=MatchExactly")
        << detail << field << "123456789" << (int) QContactFilter::MatchExactly << false << "a";
    
    QTest::newRow("phonenumber=123456789, flags=MatchContains")
        << detail << field << "123456789" << (int) QContactFilter::MatchContains << false << "abc";
    
    QTest::newRow("phonenumber=#, flags=MatchContains")
        << detail << field << "#" << (int) QContactFilter::MatchContains << false << "f";
    
    QTest::newRow("phonenumber=p, flags=MatchContains")
        << detail << field << "p" << (int) QContactFilter::MatchContains << false << "e";
        
    QTest::newRow("phonenumber=0, flags=MatchStartsWith")
        << detail << field << "0" << (int) QContactFilter::MatchStartsWith << false << "defi";    
    
    QTest::newRow("phonenumber=012, flags=MatchEndsWith")
        << detail << field << "012" << (int) QContactFilter::MatchEndsWith << false << "c";
    
    QTest::newRow("phonenumber=+358505555555, flags=MatchPhoneNumber")
        << detail << field << "+358505555555" << (int) QContactFilter::MatchPhoneNumber << true << "ij";
    
    QTest::newRow("phonenumber=313, flags=MatchPhoneNumber")
        << detail << field << "313" << (int) QContactFilter::MatchPhoneNumber << true << "h";
    
    // Custom label
    detail = (QLatin1String) QContactName::DefinitionName;
    field = (QLatin1String) QContactName::FieldCustomLabel;
    
    QTest::newRow("customlabel=frederik")
            << detail << field << "frederik" << 0 << false << "c";
    
    QTest::newRow("customlabel=Kallasvuo flags=MatchContains")
            << detail << field << "Kallasvuo" << (int) (QContactFilter::MatchContains) << false << "d";
    
    QTest::newRow("customlabel=Matti flags=MatchStartsWith")
            << detail << field << "Matti" << (int) (QContactFilter::MatchStartsWith) << false << "b";
    
    QTest::newRow("customlabel=co flags=MatchEndsWith")
            << detail << field << "co" << (int) (QContactFilter::MatchEndsWith) << false << "f";
    
    // ITU-T standard keypad collation:
    // 2 = abc, 3 = def, 4 = ghi, 5 = jkl, 6 = mno, 7 = pqrs, 8 = tuv, 9 = wxyz, 0 = space
    
    QTest::newRow("customlabel T9 olli, flags=MatchKeypadCollation|MatchExactly")
        << detail << field << "6554" << (int) (QContactFilter::MatchKeypadCollation | QContactFilter::MatchExactly) << false << "g";

    QTest::newRow("customlabel T9 olli, flags=MatchKeypadCollation|MatchContains")
        << detail << field << "6554" << (int) (QContactFilter::MatchKeypadCollation | QContactFilter::MatchContains) << false << "adg";
    
    QTest::newRow("customlabel T9 jorma, flags=MatchKeypadCollation|MatchStartsWith")
        << detail << field << "56762" << (int) (QContactFilter::MatchKeypadCollation | QContactFilter::MatchStartsWith) << false << "a";

    QTest::newRow("customlabel T9 nen, flags=MatchKeypadCollation|MatchEndsWith")
        << detail << field << "636" << (int) (QContactFilter::MatchKeypadCollation | QContactFilter::MatchEndsWith) << false << "b";
}

void tst_SimCM::detailFilter()
{
    QFETCH(QString, detailName);
    QFETCH(QString, detailField);
    QFETCH(QString, value);
    QFETCH(int, flags);
    QFETCH(bool, filterSupported);
    QFETCH(QString, expected);

    initManager("ADN");
    QVERIFY(m_cm->error() == QContactManager::NoError);

    QMap<QContactLocalId, QString> saved;
    saved.insert(saveContact("Jorma Ollila", "123456789").localId(), "a");
    saved.insert(saveContact("Matti Nykänen", "+123456789").localId(), "b");
    saved.insert(saveContact("Frederik", "+123456789012").localId(), "c");
    saved.insert(saveContact("Olli-Pekka Kallasvuo", "0718008000").localId(), "d");
    saved.insert(saveContact("Foobar", "0987654321p").localId(), "e");
    saved.insert(saveContact("Telco", "0718008000#1234#123").localId(), "f");
    saved.insert(saveContact("Olli", "543253425").localId(), "g");
    saved.insert(saveContact("Donald Duck", "313").localId(), "h");
    saved.insert(saveContact("Daisy Duck", "0505555555").localId(), "i");
    saved.insert(saveContact("Daisy Duck (int)", "+358505555555").localId(), "j");

    QContactDetailFilter f;
    f.setDetailDefinitionName(detailName, detailField);
    f.setMatchFlags(QContactFilter::MatchFlags(flags));
    f.setValue(value);
    
    QVERIFY(m_cm->isFilterSupported(f) == filterSupported);

    QList<QContactLocalId> ids = m_cm->contactIds(f);
    QVERIFY(m_cm->error() == QContactManager::NoError);

    QString result;
    foreach (QContactLocalId id, ids)
        result += saved.value(id);

    // Remove all 
    m_cm->removeContacts(m_cm->contactIds(), 0);   

    QCOMPARE(result, expected);
}



/*
 * Test if signals contactsAdded, contactsChanged and contactsRemoved are
 * emitted correctly.
 */
void tst_SimCM::signalEmission()
{
    initManager("ADN");
    qRegisterMetaType<QContactLocalId>("QContactLocalId");
    qRegisterMetaType<QList<QContactLocalId> >("QList<QContactLocalId>");
    QSignalSpy spyAdded(m_cm, SIGNAL(contactsAdded(QList<QContactLocalId>)));
    QSignalSpy spyChanged(m_cm, SIGNAL(contactsChanged(QList<QContactLocalId>)));
    QSignalSpy spyRemoved(m_cm, SIGNAL(contactsRemoved(QList<QContactLocalId>)));

    QContact contact = createContact("James Hunt", "+44751111111");

    // 1. one contact added
    QVERIFY(m_cm->saveContact(&contact));
    QTRY_COMPARE(spyAdded.count(), 1);

    // 2. one contact changed
    QVERIFY(m_cm->saveContact(&contact));
    QTRY_COMPARE(spyChanged.count(), 1);

    // 3. one contact removed
    QVERIFY(m_cm->removeContact(contact.localId()));
    QTRY_COMPARE(spyRemoved.count(), 1);

    // 4. contacts added
    spyAdded.clear();
    int batchOpCount(10);
    QList<QContact> contacts;
    for(int i(0); i < batchOpCount; i++) {
        QContact contact = createContact(
            QString("James").append(QString::number(i + 1)),
            QString("1234567890").append(QString::number(i + 1)));
        contacts.append(contact);
    }
    QMap<int, QContactManager::Error> errorMap;
    QVERIFY(m_cm->saveContacts(&contacts, &errorMap));
    QTRY_COMPARE(spyAdded.count(), batchOpCount);

    // 5. contacts changed
    spyChanged.clear();
    QVERIFY(m_cm->saveContacts(&contacts, &errorMap));
    QTRY_COMPARE(spyChanged.count(), batchOpCount);

    // 6. contacts removed
    spyRemoved.clear();
    QList<QContactLocalId> contactIds;
    foreach(const QContact& contact, contacts) {
        contactIds.append(contact.localId());
    }
    QVERIFY(m_cm->removeContacts(contactIds, &errorMap));
    QTRY_COMPARE(spyRemoved.count(), batchOpCount);
}

/*!
 * Tests SDN store specific stuff
 */
void tst_SimCM::sdnContacts()
{
    QString uri("qtcontacts:symbiansim:store=SDN");
    QScopedPointer<QContactManager> cm(QContactManager::fromUri(uri));
    if (cm->error() == QContactManager::NotSupportedError)
        QSKIP("The store not supported by the SIM card", SkipSingle);
    
    QVERIFY(cm->error() == QContactManager::NoError);
    
    // Verify that contact details have read only flag
    QList<QContact> contacts = cm->contacts();
    QVERIFY(contacts.count());
    foreach(const QContact& c, contacts) {
        // Assume a valid SDN contact always has a display label and a phone number
        QVERIFY(!c.displayLabel().isEmpty());
        QVERIFY(!c.detail(QContactPhoneNumber::DefinitionName).isEmpty());
        foreach(const QContactDetail& d, c.details()) {
            qDebug() << "Detail: " << d.definitionName();
            foreach (QVariant varianttt, d.variantValues()) {
                qDebug() << "Variant value: " << varianttt.toString();
            }
            qDebug() << "access constraints: " << d.accessConstraints();
            // Verify that read only details have the read only constraint set
            QVERIFY(d.accessConstraints().testFlag(QContactDetail::ReadOnly));
        }
    }

    // Writing should fail
    QContact c = createContact("foo", "1234567");
    QVERIFY(!cm->saveContact(&c));
}

void tst_SimCM::fillSlots()
{
    initManager("ADN");
    
    // remove all contacts
    QList<QContactLocalId> ids = m_cm->contactIds();
    m_cm->removeContacts(ids, 0); 
        
    // Update store info for empty sim card
    TRAPD(err, getEtelStoreInfoL(KETelIccAdnPhoneBook, m_etelStoreInfoPckg));
    QVERIFY(err == KErrNone);
    //dumpStoreInfo();
    
    // Get detail definitions
    QMap<QString, QContactDetailDefinition> defs = m_cm->detailDefinitions();
    bool nicknameSupported = defs.contains(QContactNickname::DefinitionName);
    bool additionalNumberSupported = !defs.value(QContactPhoneNumber::DefinitionName).isUnique();
    bool emailSupported = defs.contains(QContactNickname::DefinitionName);
    
    // Fill all slots with a name
    QList<QContact> savedContacts;
    int i;
    for (i=0; i<m_etelStoreInfo.iTotalEntries; i++)
    {
        QContact c;
        QString label;
        label.fill('x', 10);
        QString tmp = QString("%1-").arg(i);
        label.replace(0, tmp.size(), tmp);
        QContactName name;
        name.setCustomLabel(label);
        c.saveDetail(&name);
        QVERIFY(m_cm->saveContact(&c));
        savedContacts << c;        
    }
    qDebug() << QString("Wrote %1 contacts with a name").arg(i);
    
    // Sim card should be full now. Try writing one more.
    {
        QContact c;
        QContactName name;
        name.setCustomLabel("foobar");
        c.saveDetail(&name);
        QVERIFY(!m_cm->saveContact(&c));
    }
         
    // Write all slots with a number
    for (i=0; i<m_etelStoreInfo.iTotalEntries; i++)
    {
        QContact &c = savedContacts[i];
        QString num;
        num.fill('0', 10);
        QString tmp = QString("%1#").arg(i);
        num.replace(0, tmp.size(), tmp);
        QContactPhoneNumber number;
        number.setNumber(num);
        c.saveDetail(&number);
        QVERIFY(m_cm->saveContact(&c));
    }
    qDebug() << QString("Wrote %1 contacts with a number").arg(i);
    
    // Write all slots with a nickname
    for (i=0; i<m_etelStoreInfo.iTotalEntries && nicknameSupported; i++)
    {
        QContact c = savedContacts[i];
        QContactNickname nickname;
        QString nick;
        nick.fill('x', 10);
        QString tmp = QString("%1-").arg(i);
        nick.replace(0, tmp.size(), tmp);
        nickname.setNickname(nick);
        c.saveDetail(&nickname);
        if (!m_cm->saveContact(&c)) {
            if (m_cm->error() == QContactManager::LimitReachedError)
                break;
            else
                QFAIL("Failed to write nickname");
        }
        savedContacts[i] = c;
    }
    qDebug() << QString("Wrote %1 contacts with a nickname").arg(i);
    
    // Write all slots with a additional number
    for (i=0; i<m_etelStoreInfo.iTotalEntries && additionalNumberSupported; i++)
    {
        QContact c = savedContacts[i];
        QString num;
        num.fill('0', 10);
        QString tmp = QString("%1#1").arg(i);
        num.replace(0, tmp.size(), tmp);
        QContactPhoneNumber additionalNumber;
        additionalNumber.setNumber(num);
        c.saveDetail(&additionalNumber);
        if (!m_cm->saveContact(&c)) {
            if (m_cm->error() == QContactManager::LimitReachedError)
                break;
            else
                QFAIL("Failed to write additional number");
        }
        savedContacts[i] = c;
    }
    qDebug() << QString("Wrote %1 contacts with additional number").arg(i);
    
    // Write all slots with a email
    for (i=0; i<m_etelStoreInfo.iTotalEntries && emailSupported; i++)
    {
        QContact c = savedContacts[i];
        QContactEmailAddress emailAddress;
        QString email;
        email.fill('x', 10);
        QString tmp = QString("%1@").arg(i);
        email.replace(0, tmp.size(), tmp);
        emailAddress.setEmailAddress(email);
        c.saveDetail(&emailAddress);
        if (!m_cm->saveContact(&c)) {
            if (m_cm->error() == QContactManager::LimitReachedError)
                break;
            else
                QFAIL("Failed to write email");
        }
        savedContacts[i] = c;
    }
    qDebug() << QString("Wrote %1 contacts with an email").arg(i);
    
    TRAP(err, getEtelStoreInfoL(KETelIccAdnPhoneBook, m_etelStoreInfoPckg));
    QVERIFY(err == KErrNone);
    //dumpStoreInfo();

    QList<QContact> contacts = m_cm->contacts();
#ifdef __WINS__
    // Cannot do full compare in emulator because of etel test server bug.
    // If saving for example nickname fails with QContactManager::LimitReachedError
    // it will still save the contact.
    QVERIFY(contacts.count() == savedContacts.count());
#else
    QVERIFY(compareContactLists(contacts, savedContacts));
#endif
    QVERIFY(m_cm->removeContacts(m_cm->contactIds(), 0));
}

void tst_SimCM::sortingAdn_data()
{
    QTest::addColumn<QString>("definitionName");
    QTest::addColumn<QString>("fieldName");
    QTest::addColumn<int>("direction");
    QTest::addColumn<int>("caseSensitivity");
    QTest::addColumn<int>("blankPolicy");
    QTest::addColumn<QContactIds>("expectedResult");
    QTest::addColumn<QContactIds>("unknownResult");

    initManager("ADN");
    QContact blankName = createContact(QString(""), QString("+44752222222"));
    QVERIFY(m_cm->saveContact(&blankName));
    QContact james = createContact(QString("James Hunt"), QString("+44753333333"));
    QVERIFY(m_cm->saveContact(&james));
    QContact kimi = createContact(QString("Kimi Räikkönen"), QString("+358441111111"));
    QVERIFY(m_cm->saveContact(&kimi));
    QContact kimiNoCaps = createContact(QString("kimi räikkönen"), QString("+358442222222"));
    QVERIFY(m_cm->saveContact(&kimiNoCaps));
    QContact sebastianNoPhone = createContact(QString("Sebastian Vettel"), QString(""));
    QVERIFY(m_cm->saveContact(&sebastianNoPhone));

    QString contactNameDef = QContactName::DefinitionName;
    QString customLabel = QContactName::FieldCustomLabel;
    QString phoneNumberDef = QContactPhoneNumber::DefinitionName;
    QString phoneNumber = QContactPhoneNumber::FieldNumber;

    // The relative order of "kimi" and "Kimi" are undefined in this case
    QTest::newRow("custom label, ascending, case insensitive, blanks first")
        << contactNameDef
        << customLabel
        << (int) Qt::AscendingOrder
        << (int) Qt::CaseInsensitive
        << (int) QContactSortOrder::BlanksFirst
        << (QContactIds()
            << blankName.localId()
            << james.localId()
            << kimi.localId()
            << kimiNoCaps.localId()
            << sebastianNoPhone.localId())
        << (QContactIds()
            << kimi.localId()
            << kimiNoCaps.localId());

    QTest::newRow("custom label, ascending, case sensitive, blanks first")
        << contactNameDef
        << customLabel
        << (int) Qt::AscendingOrder
        << (int) Qt::CaseSensitive
        << (int) QContactSortOrder::BlanksFirst
        << (QContactIds()
            << blankName.localId()
            << james.localId()
            << kimiNoCaps.localId()
            << kimi.localId()
            << sebastianNoPhone.localId())
        << QContactIds();

    // The relative order of "kimi" and "Kimi" are undefined in this case
    QTest::newRow("custom label, descending, case insensitive, blanks first")
        << contactNameDef
        << customLabel
        << (int) Qt::DescendingOrder
        << (int) Qt::CaseInsensitive
        << (int) QContactSortOrder::BlanksFirst
        << (QContactIds()
            << blankName.localId()
            << sebastianNoPhone.localId()
            << kimi.localId()
            << kimiNoCaps.localId()
            << james.localId())
        << (QContactIds()
            << kimi.localId()
            << kimiNoCaps.localId());

    QTest::newRow("phone number, ascending, case insensitive, blanks first")
        << phoneNumberDef
        << phoneNumber
        << (int) Qt::AscendingOrder
        << (int) Qt::CaseInsensitive
        << (int) QContactSortOrder::BlanksFirst
        << (QContactIds()
            << sebastianNoPhone.localId()
            << kimi.localId()
            << kimiNoCaps.localId()
            << blankName.localId()
            << james.localId())
        << QContactIds();

    QTest::newRow("phone number, ascending, case sensitive, blanks last")
        << phoneNumberDef
        << phoneNumber
        << (int) Qt::AscendingOrder
        << (int) Qt::CaseSensitive
        << (int) QContactSortOrder::BlanksLast
        << (QContactIds()
            << kimi.localId()
            << kimiNoCaps.localId()
            << blankName.localId()
            << james.localId()
            << sebastianNoPhone.localId())
        << QContactIds();

    QTest::newRow("phone number, descending, case insensitive, blanks first")
        << phoneNumberDef
        << phoneNumber
        << (int) Qt::DescendingOrder
        << (int) Qt::CaseInsensitive
        << (int) QContactSortOrder::BlanksFirst
        << (QContactIds()
            << sebastianNoPhone.localId()
            << james.localId()
            << blankName.localId()
            << kimiNoCaps.localId()
            << kimi.localId())
        << QContactIds();
}

/*!
 * Contact sorting needs to be tested here, because the system tests in
 * \tests\auto\qcontactmanagerfiltering\tst_QContactManagerFiltering are not
 * compatible with the sim backend.
 */
void tst_SimCM::sortingAdn()
{
    QFETCH(QString, definitionName);
    QFETCH(QString, fieldName);
    QFETCH(int, direction);
    QFETCH(int, caseSensitivity);
    QFETCH(int, blankPolicy);
    QFETCH(QContactIds, expectedResult);
    QFETCH(QContactIds, unknownResult);

    QContactSortOrder sortOrder;
    sortOrder.setDetailDefinitionName(definitionName, fieldName);
    sortOrder.setDirection((Qt::SortOrder) direction);
    sortOrder.setCaseSensitivity((Qt::CaseSensitivity) caseSensitivity);
    sortOrder.setBlankPolicy((QContactSortOrder::BlankPolicy) blankPolicy);
    QVERIFY(sortOrder.isValid());

    QList<QContactSortOrder> sortOrders;
    sortOrders.append(sortOrder);

    /*
    TODO
    QList<QContact> contacts(const QList<QContactSortOrder>& sortOrders = QList<QContactSortOrder>(), const QContactFetchHint& fetchHint = QContactFetchHint()) const;
    QList<QContact> contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders = QList<QContactSortOrder>(), const QContactFetchHint& fetchHint = QContactFetchHint()) const;
    */

    // First variant
    QList<QContactLocalId> ids = m_cm->contactIds(sortOrders);
    qDebug() << "result: " << ids;
    qDebug() << "expected: " << expectedResult;
    QCOMPARE(m_cm->error(), QContactManager::NoError);
    if (unknownResult.count()) {
        // TODO: how to check all possible alternatives?
        foreach (QContactLocalId id, unknownResult) {
            QVERIFY(expectedResult.removeOne(id));
            QVERIFY(ids.removeOne(id));
        }
    }
    QCOMPARE(ids, expectedResult);

    // Second variant with filter
    ids = m_cm->contactIds(QContactFilter(), sortOrders);
    QCOMPARE(m_cm->error(), QContactManager::NoError);
    if (unknownResult.count()) {
        // TODO: how to check all possible alternatives?
        foreach (QContactLocalId id, unknownResult) {
            QVERIFY(ids.removeOne(id));
        }
    }
    QCOMPARE(ids, expectedResult);
}

/*!
 * Private helper function for checking the data format that the store supports
 */
void tst_SimCM::getEtelStoreInfoL(const TDesC &phonebook, TDes8 &infoPckg) const
{
    RTelServer etelServer;
    User::LeaveIfError(etelServer.Connect());
    CleanupClosePushL(etelServer);
    User::LeaveIfError(etelServer.LoadPhoneModule(KMmTsyModuleName));
 
    RMobilePhone etelPhone;
    RTelServer::TPhoneInfo info;
    User::LeaveIfError(etelServer.GetPhoneInfo(0, info));
    User::LeaveIfError(etelPhone.Open(etelServer, info.iName));
    CleanupClosePushL(etelPhone);

    //check what information can be saved to the Etel store
    RMobilePhoneBookStore etelStore;
    User::LeaveIfError(etelStore.Open(etelPhone, phonebook));
    CleanupClosePushL(etelStore);
    TRequestStatus requestStatus;
    etelStore.GetInfo(requestStatus, infoPckg);
    User::WaitForRequest(requestStatus);
    User::LeaveIfError(requestStatus.Int());

    CleanupStack::PopAndDestroy(&etelStore);
    CleanupStack::PopAndDestroy(&etelPhone);
    CleanupStack::PopAndDestroy(&etelServer);
}

/*!
 * Private helper function for checking if the SIM backend supports the
 * contact. This can be used in cases where it depends on the SIM card features
 * if the contact details can be saved or not.
 */
bool tst_SimCM::isContactSupported(QContact contact)
{
    QMap<QString, QContactDetailDefinition> detailDefinitions = m_cm->detailDefinitions();

    if(!m_cm->supportedContactTypes().contains(contact.type()))
        return false;
        
    QList<QString> uniqueDetails = QList<QString>();

    foreach(const QContactDetail& detail, contact.details()) {
        QString definitionName = detail.definitionName();

        // TODO: should we save a contact that has empty, non-supported details?
        // The current implementation is to ignore empty details here which
        // means that the backend should also ignore the empty details, even
        // if the detail in question is not supported.
        if (detail.isEmpty()) {
            continue;
        }

        // check if the detail is supported by the SIM
        if (detailDefinitions.contains(detail.definitionName())) {

            QContactDetailDefinition detailDef = detailDefinitions.value(detail.definitionName());

            // If the detail is unique check that there are no duplicates
            if (detailDef.isUnique()) {
                if (uniqueDetails.contains(detail.definitionName())) {
                    return false;
                } else {
                    uniqueDetails.append(detail.definitionName());
                }
            }

            // check the fields of the detail
            foreach (const QString& fieldKey, detail.variantValues().keys()) {
                if (!detailDef.fields().contains(fieldKey)) {
                    return false;
                }
            }

        } else {
            qDebug() << "Detail" << definitionName << "Not supported";
            return false;
        }
    }

    return true;
}

/*!
 * Private helper function for parsing test data (creates QContactDetails from
 * string lists).
 */
void tst_SimCM::parseDetails(QContact &contact, QStringList details, QList<QContactDetail> &parsedDetails)
{
    parsedDetails.clear();
    foreach (const QString& detail, details) {
        // the expected format is <detail definition name>:<field name>:<value>
        QStringList detailParts = detail.split(QChar(':'), QString::KeepEmptyParts, Qt::CaseSensitive);
        QVERIFY(detailParts.count() == 3);
    
        // Use existing detail if available and would not cause an overwrite of
        // a field value
        QContactDetail contactDetail = QContactDetail(detailParts[0]);
        if (contact.details().contains(QContactDetail(detailParts[0]))
            && contact.detail(detailParts[0]).variantValues().key(detailParts[1]).isNull()) {
            contactDetail = contact.detail(detailParts[0]);
        }

        if (detailParts[2].isNull()) {
            QVERIFY(contactDetail.setValue(detailParts[1], QString("")));
        } else {
            QVERIFY(contactDetail.setValue(detailParts[1], detailParts[2]));
        }
        QVERIFY(contact.saveDetail(&contactDetail));
        parsedDetails.append(contactDetail);
    }
}

/*
 * Private helper function for comparing QContact details to a well-known set
 * of QContactDetails.
 */
bool tst_SimCM::compareDetails(QContact contact, QList<QContactDetail> expectedDetails)
{
    bool result(true);

    foreach (QContactDetail expectedDetail, expectedDetails) {
        QString detailName = expectedDetail.definitionName();
        foreach (const QString& key, expectedDetail.variantValues().keys()) {
            bool match(false);
            // Go through the actual details to see if the expected detail is included there
            foreach (QContactDetail actualDetail, contact.details(expectedDetail.definitionName())) {
                QString a = actualDetail.variantValue(key).toString();
                QString b = expectedDetail.variantValue(key).toString();
                if (!actualDetail.variantValues().contains(key)) {
                    qDebug() << "field does not exist, detail:"
                        << detailName
                        << "key:"
                        << key
                        << " value: "
                        << expectedDetail.variantValue(key);
                } else if (actualDetail.variantValue(key) != expectedDetail.variantValue(key)) {
                    qDebug() << "Detail found, but value does not match. Detail:"
                        << detailName
                        << "Expected: "
                        << expectedDetail.variantValue(key)
                        << " actual: "
                        << actualDetail.variantValue(key);
                } else {
                    match = true;
                }
            }

            // Allow empty details, since they are not saved
            if (!match && !expectedDetail.variantValue(key).toString().isEmpty()) {
                result = false;
            }
        }
    }
    return result;
}

QContact tst_SimCM::createContact(QString name, QString number)
{
    QContact c;
    
    QContactName n;
    n.setCustomLabel(name);
    c.saveDetail(&n);
    
    QContactPhoneNumber nb;
    nb.setNumber(number);
    c.saveDetail(&nb);

    return c;
}

QContact tst_SimCM::saveContact(QString name, QString number)
{
    QContact c;
    
    QContactName n;
    n.setCustomLabel(name);
    c.saveDetail(&n);
    
    QContactPhoneNumber nb;
    nb.setNumber(number);
    c.saveDetail(&nb);
    
    if (!m_cm->saveContact(&c)) {
        qWarning() << 
            QString("Failed to save a contact! (name=%1,number=%2) error:%3")
                .arg(name).arg(number).arg(m_cm->error());
    }

    return c;
}

void tst_SimCM::dumpStoreInfo()
{
    TPtrC name = m_etelStoreInfo.iName;
    TPtrC8 identity = m_etelStoreInfo.iIdentity;
    
    qDebug() << "Store info:"
        << "\nType                         " << m_etelStoreInfo.iType
        << "\nTotalEntries                 " << m_etelStoreInfo.iTotalEntries
        << "\nUsedEntries                  " << m_etelStoreInfo.iUsedEntries
        << "\nCaps                         " << m_etelStoreInfo.iCaps
        << "\nName                         " << QString::fromUtf16(name.Ptr(), name.Length())
        << "\nMaxNumLength                 " << m_etelStoreInfo.iMaxNumLength
        << "\nMaxTextLength                " << m_etelStoreInfo.iMaxTextLength
        << "\nLocation                     " << m_etelStoreInfo.iLocation
        << "\nChangeCounter                " << m_etelStoreInfo.iChangeCounter
        << "\nIdentity                     " << QString::fromUtf8((const char*)identity.Ptr(), identity.Length())
#ifdef SYMBIANSIM_BACKEND_PHONEBOOKINFOV1
        ;
#else
        << "\nMaxSecondNames               " << m_etelStoreInfo.iMaxSecondNames
        << "\nMaxTextLengthSecondName      " << m_etelStoreInfo.iMaxTextLengthSecondName
        << "\nMaxAdditionalNumbers         " << m_etelStoreInfo.iMaxAdditionalNumbers
        << "\nMaxNumLengthAdditionalNumber " << m_etelStoreInfo.iMaxNumLengthAdditionalNumber
        << "\nMaxTextLengthAdditionalNumber" << m_etelStoreInfo.iMaxTextLengthAdditionalNumber
        << "\nMaxGroupNames                " << m_etelStoreInfo.iMaxGroupNames
        << "\nMaxTextLengthGroupName       " << m_etelStoreInfo.iMaxTextLengthGroupName
        << "\nMaxEmailAddr                 " << m_etelStoreInfo.iMaxEmailAddr
        << "\nMaxTextLengthEmailAddr       " << m_etelStoreInfo.iMaxTextLengthEmailAddr;    
#endif
}

bool tst_SimCM::compareContactLists(QList<QContact> lista, QList<QContact> listb)
{
    // NOTE: This compare is contact order insensitive.  
    
    // Remove matching contacts
    foreach (QContact a, lista) {
        foreach (QContact b, listb) {
            if (compareContacts(a, b)) {
                lista.removeOne(a);
                listb.removeOne(b);
                break;
            }
        }
    }
    
    //if (lista.count() != 0) qDebug() << "\nList A:\n" << lista;
    //if (listb.count() != 0) qDebug() << "\nList B:\n" << listb;
        
    return (lista.count() == 0 && listb.count() == 0);
}

bool tst_SimCM::compareContacts(QContact ca, QContact cb)
{
    // NOTE: This compare is contact detail order insensitive.
    
    if (ca.localId() != cb.localId())
        return false;
    
    QList<QContactDetail> aDetails = ca.details();
    QList<QContactDetail> bDetails = cb.details();

    // Remove matching details
    foreach (QContactDetail ad, aDetails) {
        foreach (QContactDetail bd, bDetails) {
            if (ad == bd) {
                ca.removeDetail(&ad);
                cb.removeDetail(&bd);
                break;
            }
        }
    }
    
    if (ca != cb)
        qDebug() << "\nCompare failed:\n" << "A:\n" << ca << "\nB:\n" << cb;
    
    return (ca == cb);
}

QTEST_MAIN(tst_SimCM)
#include "tst_simcm.moc"
