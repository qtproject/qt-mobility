/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ut_qtcontacts_sparql.h"

#include <QContactManager>
#include <QContactName>
#include <QContactPhoneNumber>
#include <QContactFetchRequest>

#include <QtTracker/RDFService>
#include <QtTracker/Tracker>

#include <QtTracker/ontologies/nco.h>

using namespace SopranoLive;

ut_qtcontacts_sparql::ut_qtcontacts_sparql()
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

void ut_qtcontacts_sparql::testSaveContact()
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
void ut_qtcontacts_sparql::testModifyContact()
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
