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

#include "ut_qtcontacts_fetch.h"

#include <QContactManager>
#include <QContactName>
#include <QContactFetchRequest>
#include <QContactRemoveRequest>
#include <QContactSaveRequest>
#include <QContactUrl>

#define CHECK_CURRENT_TEST_FAILED                                               \
do {                                                                            \
    if (QTest::currentTestFailed()) {                                           \
         qWarning("Failing test was called from %s(%d)", __FILE__, __LINE__);   \
         return;                                                                \
    }                                                                           \
} while (0)

ut_qtcontacts_fetch::ut_qtcontacts_fetch()
  : mContactManager(0)
{
    const QString uuid(QUuid::createUuid());

    mFirstName = "ut_qtcontacts_fetch_firstname_" + uuid;
    mLastName = "ut_qtcontacts_fetch_lastname_" + uuid;
    mWebPage = "ut_qtcontacts_fetch_url_" + uuid;

    mNameFilter.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirst);
    mNameFilter.setMatchFlags(QContactFilter::MatchExactly);
    mNameFilter.setValue(mFirstName);
}

ut_qtcontacts_fetch::~ut_qtcontacts_fetch()
{

}

void ut_qtcontacts_fetch::initTestCase()
{
}

void ut_qtcontacts_fetch::cleanupTestCase()
{
}

void ut_qtcontacts_fetch::init()
{
    QVERIFY(0 == mContactManager);
    mContactManager = new QContactManager("tracker");
    QVERIFY(0 != mContactManager);
}

void ut_qtcontacts_fetch::cleanup()
{
    if (mContactManager) {
        if (not mLocalIds.isEmpty()) {
#if 0 // FIXME: qtcontacts-tracker doesn't implement QContactRemoveRequest yet
            QContactRemoveRequest request;
            request.setManager(mContactManager);
            request.setContactIds(mLocalIds);

            if (not request.start())
                qDebug() << "error code" << request.error();
            //QVERIFY(request.start());

            waitForRequest(request);
            CHECK_CURRENT_TEST_FAILED;

            QVERIFY(request.isFinished());
            QCOMPARE(request.error(), QContactManager::NoError);
#else
            QMap<int, QContactManager::Error> errors;
            bool success = mContactManager->removeContacts(&mLocalIds, &errors);
            QCOMPARE(mContactManager->error(), QContactManager::NoError);
            QVERIFY(errors.isEmpty());
            QVERIFY(success);
#endif

            mLocalIds.clear();
        }

        mContactManager->deleteLater();
        mContactManager = 0;
    }
}

void ut_qtcontacts_fetch::waitForRequest(QContactAbstractRequest &request, int ms)
{
    // check pre-conditions
    QCOMPARE((int) request.state(), (int) QContactAbstractRequest::ActiveState);

    // wait for the request to do its work (or get canceled)
    QTime timer;
    timer.start();

    while (request.isActive() && timer.elapsed() < ms) {
        QTest::qWait(10);
    }

    // check post-conditions
    QVERIFY2(not request.isActive(), "timeout expired");
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

    // verify that there really is no test contact yet
    QVERIFY(request.isFinished());
    QVERIFY(request.contacts().isEmpty());
}

void ut_qtcontacts_fetch::saveContact(QContact &contact)
{
    // add the contact to database
    QContactSaveRequest request;
    request.setManager(mContactManager);
    request.setContacts(QList<QContact>() << contact);
    QVERIFY(request.start());

    waitForRequest(request);
    CHECK_CURRENT_TEST_FAILED;

    // verify the contact got saved
    QVERIFY(request.isFinished());
    QCOMPARE(request.error(), QContactManager::NoError);
    QCOMPARE(request.contacts().count(), 1);

    // copy back the saved contact
    contact = request.contacts().at(0);

    // verify the contact got a local id
    QVERIFY(contact.localId());

    // remember the local id so that we can remove the contact from database later
    mLocalIds.append(contact.localId());
}

void ut_qtcontacts_fetch::ut_checkDatabaseEmpty()
{
    // test if this test works in general
    checkDatabaseEmpty();
    CHECK_CURRENT_TEST_FAILED;
}

void ut_qtcontacts_fetch::ut_testSaveContact()
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

void ut_qtcontacts_fetch::ut_testSaveContactCopy()
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

void ut_qtcontacts_fetch::ut_testFetchSavedContact()
{
    // check that we start with a clean database
    checkDatabaseEmpty();
    CHECK_CURRENT_TEST_FAILED;

    // create a named contact and save it
    QContact savedContact;

    setupTestContact(savedContact);
    CHECK_CURRENT_TEST_FAILED;

    foreach(const QContactDetail &d, savedContact.details())
        qDebug() << d.definitionName() << d.variantValues();

    saveContact(savedContact);
    CHECK_CURRENT_TEST_FAILED;
    mLocalIds.clear();

    // fetch the saved contact
    QContactFetchRequest request;
    request.setManager(mContactManager);
    request.setFilter(mNameFilter);
    QVERIFY(request.start());

    waitForRequest(request);
    CHECK_CURRENT_TEST_FAILED;

    // check that exactly one contact can be fetched now
    QVERIFY(request.isFinished());
    QCOMPARE(request.contacts().count(), 1);

    const QContact &fetchedContact(request.contacts().at(0));
    QVERIFY(0 != fetchedContact.localId());

    foreach(const QContactDetail &d, fetchedContact.details())
        qDebug() << d.definitionName() << d.variantValues();

    QList<QContactDetail> details;

    // check that the fetched contact has the expected properties
    details = fetchedContact.details(QContactName::DefinitionName);

    QCOMPARE(details.count(), 1);
    QCOMPARE(details[0].value(QContactName::FieldFirstName), mFirstName);
    QCOMPARE(details[0].value(QContactName::FieldLastName), mLastName);

    details = fetchedContact.details(QContactUrl::DefinitionName);

    QCOMPARE(details.count(), 1);
    QCOMPARE(details[0].value(QContactUrl::FieldUrl), mWebPage);
}

QTEST_MAIN(ut_qtcontacts_fetch)
