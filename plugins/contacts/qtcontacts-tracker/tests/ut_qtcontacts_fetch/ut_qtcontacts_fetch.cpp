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

#include "ut_qtcontacts_fetch.h"

#include <QContactManager>
#include <QContactName>
#include <QContactFetchRequest>
#include <QContactUrl>

#include <QtTracker/tracker.h>

typedef QSet<QString> QStringSet;

ut_qtcontacts_fetch::ut_qtcontacts_fetch()
{
    mUuid = QUuid::createUuid();
    mFirstName = "ut_qtcontacts_fetch_firstname_" + mUuid;
    mLastName = "ut_qtcontacts_fetch_lastname_" + mUuid;
    mWebPage = "ut_qtcontacts_fetch_url_" + mUuid;

    mNameFilter.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirst);
    mNameFilter.setMatchFlags(QContactFilter::MatchExactly);
    mNameFilter.setValue(mFirstName);
}

void ut_qtcontacts_fetch::setupTestContact(QContact &contact)
{
    QContactName name;
    name.setFirstName(mFirstName);
    name.setLastName(mLastName);
    QVERIFY(contact.saveDetail(&name));

    QContactUrl url;
    url.setUrl(mWebPage);
    QVERIFY(contact.saveDetail(&url));
}

void ut_qtcontacts_fetch::checkDatabaseEmpty()
{
    // try to fetch our testing contact
    QContactFetchRequest request;
    request.setManager(mContactManager);
    request.setFilter(mNameFilter);
    QVERIFY(request.start());

    // wait for the request to finish
    waitForRequest(request);
    CHECK_CURRENT_TEST_FAILED;

    // dump unexpected contacts
    foreach(const QContact &c, request.contacts()) {
        qWarning() << "unexpected contact" << c.localId();
        foreach(const QContactDetail &d, c.details())
            qWarning() << "  " << d.definitionName() << d.variantValues();
    }

    // verify that there really is no test contact yet
    QVERIFY(request.isFinished());
    QVERIFY(request.contacts().isEmpty());
}

void ut_qtcontacts_fetch::testSaveContact()
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
}

void ut_qtcontacts_fetch::testSaveContactCopy()
{
    // check that we start with a clean database
    checkDatabaseEmpty();
    CHECK_CURRENT_TEST_FAILED;

    // create a named contact
    QContact contact;

    setupTestContact(contact);
    CHECK_CURRENT_TEST_FAILED;

    // add copy of the contact to database
    QContact copy(contact);

    saveContact(copy);
    CHECK_CURRENT_TEST_FAILED;
}

void ut_qtcontacts_fetch::testFetchSavedContact()
{
    // check that we start with a clean database
    checkDatabaseEmpty();
    CHECK_CURRENT_TEST_FAILED;

    // create a named contact and save it
    QContact savedContact;

    setupTestContact(savedContact);
    CHECK_CURRENT_TEST_FAILED;

    saveContact(savedContact);
    CHECK_CURRENT_TEST_FAILED;
    mLocalIds.clear();

    // fetch the saved contact
    QContact fetchedContact;
    fetchContact(mNameFilter, fetchedContact);
    CHECK_CURRENT_TEST_FAILED;

    QVERIFY(0 != fetchedContact.localId());

    // check that the fetched contact has the expected properties
    QList<QContactDetail> details(fetchedContact.details(QContactName::DefinitionName));

    QCOMPARE(details.count(), 1);
    QCOMPARE(details[0].value(QContactName::FieldFirstName), mFirstName);
    QCOMPARE(details[0].value(QContactName::FieldLastName), mLastName);

    details = fetchedContact.details(QContactUrl::DefinitionName);

    QEXPECT_FAIL("", "cannot restore URL field right now", Continue);

    QCOMPARE(details.count(), 1);
    QCOMPARE(details[0].value(QContactUrl::FieldUrl), mWebPage);
}

QTEST_MAIN(ut_qtcontacts_fetch)
