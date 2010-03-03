/* * This file is part of contacts *
 * Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 * Contact: Aleksandar Stojiljkovic <aleksandar.stojiljkovic@nokia.com>
 * This software, including documentation, is protected by copyright controlled by
 * Nokia Corporation. All rights are reserved. Copying, including reproducing, storing,
 * adapting or translating, any or all of this material requires the prior written consent
 * of Nokia Corporation. This material also contains confidential information which may
 * not be disclosed to others without the prior written consent of Nokia. */

#include "ut_qtcontacts_sparql.h"

#include <QContactManager>
#include <QContactName>
#include <QContactPhoneNumber>
#include <QContactFetchRequest>

#include <QtTracker/RDFService>
#include <QtTracker/Tracker>

using namespace SopranoLive;

#define CHECK_CURRENT_TEST_FAILED                                               \
do {                                                                            \
    if (QTest::currentTestFailed()) {                                           \
         qWarning("Failing test was called from %s(%d)", __FILE__, __LINE__);   \
         return;                                                                \
    }                                                                           \
} while (0)

ut_qtcontacts_sparql::ut_qtcontacts_sparql()
  : mContactManager(0)
{
    const QString uuid(QUuid::createUuid());

    mFirstName = "ut_qtcontacts_sparql_firstname_" + uuid;
    mLastName = "ut_qtcontacts_sparql_lastname_" + uuid;
    mPhoneNumber = "ut_qtcontacts_sparql_phone_" + uuid;
    mPhoneNumber = mPhoneNumber.replace('-', '_');

    mNameFilter.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirst);
    mNameFilter.setMatchFlags(QContactFilter::MatchExactly);
    mNameFilter.setValue(mFirstName);
}

ut_qtcontacts_sparql::~ut_qtcontacts_sparql()
{

}

void ut_qtcontacts_sparql::initTestCase()
{
}

void ut_qtcontacts_sparql::cleanupTestCase()
{
}

void ut_qtcontacts_sparql::init()
{
    QVERIFY(0 == mContactManager);
    mContactManager = new QContactManager("tracker");
    QVERIFY(0 != mContactManager);
}

void ut_qtcontacts_sparql::cleanup()
{
    if (mContactManager) {
        if (not mLocalIds.isEmpty()) {
            // eliminate duplicate contact ids
            mLocalIds = mLocalIds.toSet().toList();

            QMap<int, QContactManager::Error> errors;

            if (not mContactManager->removeContacts(&mLocalIds, &errors)) {
                QFAIL(qPrintable(QString("removing contacts failed: %1").
                                 arg(mContactManager->error())));
            }

            mLocalIds.clear();
        }

        delete mContactManager;
        mContactManager = 0;
    }
}

void ut_qtcontacts_sparql::waitForRequest(QContactAbstractRequest &request, int ms)
{
    // check pre-conditions
    QVERIFY2(request.isActive(), "request must be active");

    // wait for the request to do its work (or get canceled)
    QTime timer;
    timer.start();

    while (request.isActive() && timer.elapsed() < ms) {
        QTest::qWait(10);
    }

    // check post-conditions
    QVERIFY2(not request.isActive(), "timeout expired");
}

void ut_qtcontacts_sparql::setupTestContact(QContact &contact)
{
    // attach a name to the contact
    QContactName name;

    name.setFirstName(mFirstName);
    name.setLastName(mLastName);

    QVERIFY(contact.saveDetail(&name));

    // attach a phone number to the contact
    QContactPhoneNumber phone;

    phone.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    phone.setContexts(QContactPhoneNumber::ContextHome);
    phone.setNumber(mPhoneNumber);

    QVERIFY(contact.saveDetail(&phone));
}

void ut_qtcontacts_sparql::checkDatabaseEmpty()
{
    // try to sparql our testing contact
    QContactFetchRequest request;

    request.setManager(mContactManager);
    request.setFilter(mNameFilter);
    request.start();

    // wait for the request to finish
    waitForRequest(request);
    CHECK_CURRENT_TEST_FAILED;

    // verify that there really is no test contact yet
    QVERIFY(request.isFinished());
    QVERIFY(request.contacts().isEmpty());
}

void ut_qtcontacts_sparql::saveContact(QContact &contact)
{
    // add the contact to database
    bool contactSaved = mContactManager->saveContact(&contact);
    QCOMPARE(mContactManager->error(), QContactManager::NoError);
    QVERIFY(contactSaved);

    // check local Id
    QVERIFY(contact.localId());

    // remember the local Id so that we can remove the contact from database later
    mLocalIds.append(contact.localId());
}

template <class PhoneNumberType>
void ut_qtcontacts_sparql::checkOntology(QContactLocalId contactId, int expectedCount)
{
    // fetch contact from tracker
    const QUrl id(QString("contact:%1").arg(contactId));
    Live<nco::PersonContact> contact = ::tracker()->liveNode(id);
    QCOMPARE(contact->getContactUID(), QString::number(contactId));

    // check first name property of the contact
    const QStringList givenNames(contact->getNameGivens());
    QCOMPARE(givenNames.count(), 1);
    QCOMPARE(givenNames[0], mFirstName);

    // check last name property of the contact
    const QStringList familyNames(contact->getNameFamilys());
    QCOMPARE(familyNames.count(), 1);
    QCOMPARE(familyNames[0], mLastName);

    // check if the contact has the expected number of phone numbers
    const QList< Live<PhoneNumberType> > hasPhoneNumbers(contact->getHasPhoneNumbers());
    QCOMPARE(hasPhoneNumbers.count(), expectedCount);

    // check if the contact's actual phone number
    const QStringList phoneNumbers(hasPhoneNumbers[0]->getPhoneNumbers());
    QCOMPARE(phoneNumbers.count(), 1);
    QCOMPARE(phoneNumbers[0], mPhoneNumber);
}

void ut_qtcontacts_sparql::ut_checkDatabaseEmpty()
{
    // test if this test works in general
    checkDatabaseEmpty();
    CHECK_CURRENT_TEST_FAILED;
}

void ut_qtcontacts_sparql::ut_testSaveContact()
{
    // check that we start with a clean database
    checkDatabaseEmpty();
    CHECK_CURRENT_TEST_FAILED;

    // create a named contact and save it
    QContact contact;

    setupTestContact(contact);
    CHECK_CURRENT_TEST_FAILED;

    saveContact(contact);
    CHECK_CURRENT_TEST_FAILED;

    // now use RDF queries to check what landed within the database
    checkOntology<nco::CellPhoneNumber>(contact.localId());
    CHECK_CURRENT_TEST_FAILED;
}

// this test basically checks for NB#158859
void ut_qtcontacts_sparql::ut_testModifyContact()
{
    // check that we start with a clean database
    checkDatabaseEmpty();
    CHECK_CURRENT_TEST_FAILED;

    // create a named contact and save it
    QContact contact;

    setupTestContact(contact);
    CHECK_CURRENT_TEST_FAILED;

    saveContact(contact);
    CHECK_CURRENT_TEST_FAILED;

    // check that the contact's phone number has the expected subtype
    QList<QContactDetail> details(contact.details(QContactPhoneNumber::DefinitionName));
    QCOMPARE(details.count(), 1);

    QContactPhoneNumber phoneNumber(details[0]);
    QCOMPARE(phoneNumber.subTypes(), QStringList(QContactPhoneNumber::SubTypeMobile));

    // modify the phone number's subtype and save the modified contact
    phoneNumber.setSubTypes(QContactPhoneNumber::SubTypeFacsimile);
    QVERIFY(contact.saveDetail(&phoneNumber));

    saveContact(contact);
    CHECK_CURRENT_TEST_FAILED;

    // check that the saved contact's phone number has the expected subtype
    details = contact.details(QContactPhoneNumber::DefinitionName);
    QCOMPARE(details.count(), 1);

    phoneNumber = details[0];
    QCOMPARE(phoneNumber.subTypes(), QStringList(QContactPhoneNumber::SubTypeFacsimile));

    // now use RDF queries to check what landed within the database
    checkOntology<nco::CellPhoneNumber>(contact.localId());
    CHECK_CURRENT_TEST_FAILED;

    // we should have got a fax number assigned...
    checkOntology<nco::FaxNumber>(contact.localId());
    CHECK_CURRENT_TEST_FAILED;

    checkOntology<nco::PhoneNumber>(contact.localId());
    CHECK_CURRENT_TEST_FAILED;

    // ...and the cell phone numbers shall be gone
    checkOntology<nco::CellPhoneNumber>(contact.localId(), 0);
    CHECK_CURRENT_TEST_FAILED;
}

QTEST_MAIN(ut_qtcontacts_sparql)
