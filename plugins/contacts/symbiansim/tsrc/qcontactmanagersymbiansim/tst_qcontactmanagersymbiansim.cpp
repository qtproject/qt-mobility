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


#define QCOMPARE_WITH_RETURN_VALUE(actual, expected) \
    if (!QTest::qCompare(actual, expected, #actual, #expected, __FILE__, __LINE__))\
        return false;

#define QVERIFY_WITH_RETURN_VALUE(statement) \
    if (!QTest::qVerify((statement), #statement, "", __FILE__, __LINE__))\
        return false;

//TESTED_CLASS=
//TESTED_FILES=

/*!
*/
class tst_QContactManagerSymbianSim : public QObject
{
Q_OBJECT

public:
    tst_QContactManagerSymbianSim();
    virtual ~tst_QContactManagerSymbianSim();

public slots:
    void initTestCase();
    void cleanupTestCase();

private slots:
    /* Test cases that take no data */
    void hasFeature();
    void supportedContactTypes();
    void detailDefinitions();

    /* Test cases that need data */
    void addContact_data();
    void addContact();
    void updateContactDetail_data();
    void updateContactDetail();

private:
    void getEtelStoreInfoL(const TDesC &phonebook, TDes8 &infoPckg) const;
    void parseDetails(QContact &contact, QStringList details, QList<QContactDetail> &parsedDetails);
    bool isContactSupported(QContact contact);
    bool compareDetails(QContact contact, QList<QContactDetail> expectedDetails);

    QContactManager* m_cm;

    RMobilePhoneBookStore::TMobilePhoneBookInfoV5 m_etelStoreInfo;
    RMobilePhoneBookStore::TMobilePhoneBookInfoV5Pckg m_etelStoreInfoPckg;
};

tst_QContactManagerSymbianSim::tst_QContactManagerSymbianSim() :
    m_etelStoreInfoPckg( m_etelStoreInfo )
{
}

tst_QContactManagerSymbianSim::~tst_QContactManagerSymbianSim()
{
}

void tst_QContactManagerSymbianSim::initTestCase()
{
    m_cm = QContactManager::fromUri("qtcontacts:symbiansim");
    QVERIFY(m_cm);
    TRAPD(err, getEtelStoreInfoL(KETelIccAdnPhoneBook, m_etelStoreInfoPckg));
    QCOMPARE(err, KErrNone);
}

void tst_QContactManagerSymbianSim::cleanupTestCase()
{
    delete m_cm;
    m_cm = 0;
}

void tst_QContactManagerSymbianSim::getEtelStoreInfoL(const TDesC &phonebook, TDes8 &infoPckg) const
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

void tst_QContactManagerSymbianSim::hasFeature()
{
    // TODO: Groups may be supported by some SIM cards?
    QVERIFY(!m_cm->hasFeature(QContactManager::Groups));
    QVERIFY(!m_cm->hasFeature(QContactManager::Groups, QContactType::TypeContact));
    QVERIFY(!m_cm->hasFeature(QContactManager::Groups, QContactType::TypeGroup));

    QVERIFY(!m_cm->hasFeature(QContactManager::ActionPreferences));
    QVERIFY(!m_cm->hasFeature(QContactManager::ActionPreferences, QContactType::TypeContact));
    QVERIFY(!m_cm->hasFeature(QContactManager::ActionPreferences, QContactType::TypeGroup));

    QVERIFY(!m_cm->hasFeature(QContactManager::MutableDefinitions));
    QVERIFY(!m_cm->hasFeature(QContactManager::MutableDefinitions, QContactType::TypeContact));
    QVERIFY(!m_cm->hasFeature(QContactManager::MutableDefinitions, QContactType::TypeGroup));

    QVERIFY(!m_cm->hasFeature(QContactManager::Relationships));
    QVERIFY(!m_cm->hasFeature(QContactManager::Relationships, QContactType::TypeContact));
    QVERIFY(!m_cm->hasFeature(QContactManager::Relationships, QContactType::TypeGroup));

    QVERIFY(!m_cm->hasFeature(QContactManager::ArbitraryRelationshipTypes));
    QVERIFY(!m_cm->hasFeature(QContactManager::ArbitraryRelationshipTypes, QContactType::TypeContact));
    QVERIFY(!m_cm->hasFeature(QContactManager::ArbitraryRelationshipTypes, QContactType::TypeGroup));

    QVERIFY(!m_cm->hasFeature(QContactManager::RelationshipOrdering));
    QVERIFY(!m_cm->hasFeature(QContactManager::RelationshipOrdering, QContactType::TypeContact));
    QVERIFY(!m_cm->hasFeature(QContactManager::RelationshipOrdering, QContactType::TypeGroup));

    // TODO: self contact may be supported? (so called "own number store")
    QVERIFY(!m_cm->hasFeature(QContactManager::SelfContact));
    QVERIFY(!m_cm->hasFeature(QContactManager::SelfContact, QContactType::TypeContact));
    QVERIFY(!m_cm->hasFeature(QContactManager::SelfContact, QContactType::TypeGroup));

    QVERIFY(!m_cm->hasFeature(QContactManager::Anonymous));
    QVERIFY(!m_cm->hasFeature(QContactManager::Anonymous, QContactType::TypeContact));
    QVERIFY(!m_cm->hasFeature(QContactManager::Anonymous, QContactType::TypeGroup));

    QVERIFY(!m_cm->hasFeature(QContactManager::ChangeLogs));
    QVERIFY(!m_cm->hasFeature(QContactManager::ChangeLogs, QContactType::TypeContact));
    QVERIFY(!m_cm->hasFeature(QContactManager::ChangeLogs, QContactType::TypeGroup));
}

void tst_QContactManagerSymbianSim::supportedContactTypes()
{
    QStringList types = m_cm->supportedContactTypes();
    QVERIFY(types.count() > 0);
    // Assuming that contact type of TypeContact is always supported
    QVERIFY(types.contains(QContactType::TypeContact));
}

void tst_QContactManagerSymbianSim::detailDefinitions()
{
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

void tst_QContactManagerSymbianSim::addContact_data()
{
    // A string list containing the detail fields in format <detail definition name>:<field name>:<value>
    // For example first name: Name:First:James
    QTest::addColumn<int>("expectedResult"); // 1 = pass, 0 = fail, -1 = depends on the SIM card
    QTest::addColumn<QString>("expectedDisplayLabel");
    QTest::addColumn<QStringList>("details"); // format is <detail definition name>:<field name>:<value>
    QString unnamedLabel("Unnamed");
    QString es = QString();
    QString tooLongText("James Hunt the 12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890th");

    // TODO: what name field to use for a sim contact name?
    // Note: With the current implementation the value must not contain a ':' character
    QTest::newRow("custom label")
        << 1 // expected to pass
        << "James"
        << (QStringList()
            << "Name:CustomLabel:James");

    QTest::newRow("custom label 2")
        << 1 // expected to pass
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt");

    QTest::newRow("2 custom labels")
        << 0 // expected to fail. Custom label is unique.
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James"
            << "Name:CustomLabel:James Hunt");

    QTest::newRow("too long custom label")
        << 1 // expected to pass. Note: too long display label is truncated
        << tooLongText
        << (QStringList()
            << (QString("Name:CustomLabel:").append(tooLongText)));

    QTest::newRow("custom label and nick name")
        << -1 // Depends on SIM card support (some cards support second name)
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "Nickname:Nickname:Hunt the Shunt");

    QTest::newRow("custom label and too long nick name")
        << -1 // Depends on SIM card support (some cards support second name)
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << (QString("Nickname:Nickname:").append(tooLongText)));

    QTest::newRow("phone number")
        << 1
        << unnamedLabel
        << (QStringList()
            << "PhoneNumber:PhoneNumber:+44752222222");

    QTest::newRow("custom label and phone number")
        << 1
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "PhoneNumber:PhoneNumber:+44752222222");

    QTest::newRow("custom label and funny (but legal) phone number")
        << 1
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "PhoneNumber:PhoneNumber:+0123456789*#p");

    QTest::newRow("custom label and illegal phone number 1")
        << 0 // illegal characters in the phone number, should fail
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "PhoneNumber:PhoneNumber:+44(75)2222222");

    QTest::newRow("custom label and illegal phone number 2")
        << 0 // illegal characters in the phone number, should fail
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "PhoneNumber:PhoneNumber:asdfqwer");

    QTest::newRow("custom label and too long phone number")
        << 0 // long enough phone number to fail on any SIM card
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "PhoneNumber:PhoneNumber:1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");

    QTest::newRow("custom label and multiple phone numbers")
        << -1 // some SIM cards support multiple phone numbers
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "PhoneNumber:PhoneNumber:+44752222222"
            << "PhoneNumber:PhoneNumber:+44751111111");

    QTest::newRow("custom label and multiple phone numbers, one phone number too long")
        << 0 // Long enough additional phone number to fail on any SIM card
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "PhoneNumber:PhoneNumber:+44752222222"
            << "PhoneNumber:PhoneNumber:1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");

    QTest::newRow("custom label and email")
        << -1 // some SIM cards support e-mail address
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "EmailAddress:EmailAddress:james@mclaren.com");

    QTest::newRow("custom label and multiple emails")
        << 0 // some SIM cards support multiple e-mail addresses, but not this many
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "EmailAddress:EmailAddress:james@march.com"
            << "EmailAddress:EmailAddress:james@hesketh.com"
            << "EmailAddress:EmailAddress:james@mclaren.com"
            << "EmailAddress:EmailAddress:james.hunt@bbc.co.uk");

    QTest::newRow("custom label and too long email")
        << 0 // long enough e-mail to fail on any SIM card
        << "James Hunt"
        << (QStringList()
            << "Name:CustomLabel:James Hunt"
            << "EmailAddress:EmailAddress:james.hunt.the12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890th@mclaren.com");

    QTest::newRow("non-supported field")
        << 0 // expected to fail
        << es
        << (QStringList()
            << "Name:IllegalNameDetailFieldName:James");

    QTest::newRow("non-supported detail")
        << 0 // expected to fail
        << es
        << (QStringList()
            << "NotSupportedDetailDefinitionName:IllegalFieldName:FieldValue");

    QTest::newRow("empty, non-supported detail")
        << 0 // expected to fail, since no valid details provided
        << es
        << (QStringList()
            << "NotSupportedDetailDefinitionName:IllegalFieldName:");
}

/*
 * Steps:
 * 1. Parse contact details from test parameters
 * 2. Determine the expected result
 * 3.1 (if expected to pass) Save contact, verify result and remove contact
 * 3.2 (if expected to fail) Check that saving a contact fails
 */
void tst_QContactManagerSymbianSim::addContact()
{
    // Make debugging easier by getting the test case name
    QString tescaseName = QTest::currentDataTag();

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
    QCOMPARE(m_cm->error(), QContactManager::NoError);

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

        // verify display label, allow truncating to the max text length
        QCOMPARE(contact.displayLabel(), expectedDisplayLabel.left(m_etelStoreInfo.iMaxTextLength));

        // TODO: verify that no extra details were added?
        //?QCOMPARE(contact.details().count(), detailsUnderTest.count() + 2);

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

void tst_QContactManagerSymbianSim::updateContactDetail_data()
{
    // The initial contact details,
    // for example: <"Name:First:James">; <"PhoneNumber:PhoneNumber:1234567890">
    QTest::addColumn<QStringList>("details1");
    // The updated contact details,
    // for example: <"Name:First:James">; <"PhoneNumber:PhoneNumber:0987654321">
    QTest::addColumn<QStringList>("details2");

    QString es = QString();

    QTest::newRow("update custom label")
        << (QStringList()
            << "Name:CustomLabel:James")
        << (QStringList()
            << "Name:CustomLabel:James Hunt");

    QTest::newRow("update phone number")
        << (QStringList()
            << "Name:CustomLabel:James"
            << "PhoneNumber:PhoneNumber:+44751111111")
        << (QStringList()
            << "Name:CustomLabel:James"
            << "PhoneNumber:PhoneNumber:+44752222222");

    QTest::newRow("update e-mail")
        << (QStringList()
            << "Name:CustomLabel:James"
            << "EmailAddress:EmailAddress:james@march.com")
        << (QStringList()
            << "Name:CustomLabel:James"
            << "EmailAddress:EmailAddress:james@hesketh.com");

    QTest::newRow("update nickname")
        << (QStringList()
            << "Name:CustomLabel:James"
            << "Nickname:Nickname:James")
        << (QStringList()
            << "Name:CustomLabel:James"
            << "Nickname:Nickname:Hunt the Shunt");
}

void tst_QContactManagerSymbianSim::updateContactDetail()
{
    QString tescaseName = QTest::currentDataTag();

    QFETCH(QStringList, details1);
    QFETCH(QStringList, details2);

    // 1. Parse details
    QContact contact;
    QList<QContactDetail> parsedDetails;
    parseDetails(contact, details1, parsedDetails);

    // 2. Save contact and verify result
    if (!isContactSupported(contact)) {
        QSKIP("The contact cannot be saved onto the SIM card", SkipSingle);
    }
    QVERIFY(m_cm->saveContact(&contact));
    QCOMPARE(m_cm->error(), QContactManager::NoError);
    QVERIFY(compareDetails(contact, parsedDetails));

    // 3. Update contact detail and verify result
    foreach (QContactDetail detail, parsedDetails) {
        QContactDetail savedDetail = contact.detail(detail.definitionName());
        QVERIFY(contact.removeDetail(&savedDetail));
    }
    parseDetails(contact, details2, parsedDetails);
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

/*!
 * Private helper function for parsing test data (creates QContactDetails from
 * string lists).
 */
void tst_QContactManagerSymbianSim::parseDetails(QContact &contact, QStringList details, QList<QContactDetail> &parsedDetails)
{
    parsedDetails.clear();
    foreach (QString detail, details) {
        // the expected format is <detail definition name>:<field name>:<value>
        QStringList detailParts = detail.split(QChar(':'), QString::KeepEmptyParts, Qt::CaseSensitive);
        QVERIFY(detailParts.count() == 3);
    
        // Use existing detail if available and would not cause an overwrite of
        // a field value
        QContactDetail contactDetail = QContactDetail(detailParts[0]);
        if (contact.details().contains(detailParts[0])
            && contact.detail(detailParts[0]).variantValues().key(detailParts[1]).isNull()) {
            contactDetail = contact.detail(detailParts[0]);
        }

        // Set the field value only if not empty (do not add empty fields)  
        if (!detailParts[2].isEmpty()) {
            QVERIFY(contactDetail.setValue(detailParts[1], detailParts[2]));
        }

        QVERIFY(contact.saveDetail(&contactDetail));
        parsedDetails.append(contactDetail);
    }
}

/*!
 * Private helper function for checking if the SIM backend supports the
 * contact. This can be used in cases where it depends on the SIM card features
 * if the contact details can be saved or not.
 */
bool tst_QContactManagerSymbianSim::isContactSupported(QContact contact)
{
    QMap<QString, QContactDetailDefinition> detailDefinitions = m_cm->detailDefinitions();

    if(!m_cm->supportedContactTypes().contains(contact.type()))
        return false;
        
    QList<QString> uniqueDetails = QList<QString>();

    foreach(QContactDetail detail, contact.details()) {
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
            foreach (QString fieldKey, detail.variantValues().keys()) {
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

/*
 * Private helper function for comparing QContact details to a well-known set
 * of QContactDetails.
 * \return true if all the expected contact details have a match in the \contact.
 */
bool tst_QContactManagerSymbianSim::compareDetails(QContact contact, QList<QContactDetail> expectedDetails)
{
    foreach (QContactDetail expectedDetail, expectedDetails) {
        QContactDetail actualDetail = contact.detail(expectedDetail.definitionName());
        QVERIFY_WITH_RETURN_VALUE(!actualDetail.isEmpty());

        // Allow truncating the custom label to the max text length
        if (expectedDetail.definitionName() == QContactName::DefinitionName) {
            QContactName nameDetail = static_cast<QContactName>(expectedDetail);
            nameDetail.setCustomLabel(nameDetail.customLabel().left(m_etelStoreInfo.iMaxTextLength));
            QCOMPARE_WITH_RETURN_VALUE(actualDetail, static_cast<QContactDetail>(nameDetail));
        // Allow truncating the nick name to the max text length
        } else if (expectedDetail.definitionName() == QContactNickname::DefinitionName) {
                QContactNickname nick = static_cast<QContactNickname>(expectedDetail);
                nick.setNickname(nick.nickname().left(m_etelStoreInfo.iMaxTextLength));
                QCOMPARE_WITH_RETURN_VALUE(actualDetail, static_cast<QContactDetail>(nick));
        } else {
            if (actualDetail != expectedDetail) {
                // FAIL! Make it easier to debug the output by
                // comparing the contact detail field contents
                foreach (QString key, expectedDetail.variantValues().keys()) {
                    QVariant value1 = actualDetail.value(key);
                    QVariant value2 = expectedDetail.value(key);
                    QCOMPARE_WITH_RETURN_VALUE(actualDetail.value(key), expectedDetail.value(key));
                }
            }
        }
    }
    return true;
}

QTEST_MAIN(tst_QContactManagerSymbianSim)
#include "tst_qcontactmanagersymbiansim.moc"
