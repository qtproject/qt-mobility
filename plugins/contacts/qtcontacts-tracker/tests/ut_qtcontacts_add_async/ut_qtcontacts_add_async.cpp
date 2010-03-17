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

#include "ut_qtcontacts_add_async.h"

#include <QContactManager>
#include <QContactName>
#include <QContactDetailFilter>
#include <QContactPhoneNumber>
#include <QtTest/QtTest>
#include <QDebug>

// Note that we try to avoid using any names that might already be in the database:
const char* TESTNAME_FIRST = "ut_qtcontacts_add_firstname";
const char* TESTNAME_LAST = "ut_qtcontacts_add_firstlast";

ut_qtcontacts_add::ut_qtcontacts_add()
: getExistingContactFinishedCallback(0),
  waiting(false)
{
}

ut_qtcontacts_add::~ut_qtcontacts_add()
{

}

void ut_qtcontacts_add::initTestCase()
{
}

void ut_qtcontacts_add::cleanupTestCase()
{
}

void ut_qtcontacts_add::init()
{
}

void ut_qtcontacts_add::cleanup()
{
  waiting = false;
}

bool ut_qtcontacts_add::waitForStop()
{
    waiting = true;

    const int max_secs = 100000;

    // wait for signal
    int i = 0;
    while(waiting && i++ < max_secs) {
        // Allow the mainloop to run:
        QTest::qWait(10);
    }

    return !waiting;
}

QContactManager* ut_qtcontacts_add::getContactManager()
{
    static QContactManager manager("tracker");
    return &manager;
}

void ut_qtcontacts_add::onContactFetchRequestProgress()
{
    //qDebug() << "onContactFetchRequestProgress";
    if (!contactFetchRequest.isFinished())
        return;

    //Store the contact so the callback can use it.
    if(!(contactFetchRequest.contacts().isEmpty())) {
        contact = contactFetchRequest.contacts()[0];
        QVERIFY(contact.localId() != 0);
        contactFetchRequest.cancel(); //Stop any more slot calls.
    }

    //qDebug() << "debug: fetched localId=" << contact.localId();

    //Avoid more slot calls, though this is unlikely because it has finished.
    contactFetchRequest.cancel();

    //Call the callback method that was specified to getExistingContact():
    if(getExistingContactFinishedCallback) {
        FinishedCallbackFunc func = getExistingContactFinishedCallback;
        getExistingContactFinishedCallback = 0;
        (this->*func)();
    }
}


void ut_qtcontacts_add::getExistingContact(FinishedCallbackFunc finishedCallback)
{
    QContactManager* manager = getContactManager();
    Q_ASSERT(manager);

    // Stop pending fetch requests
    if (contactFetchRequest.isActive())
        contactFetchRequest.cancel();

    // Initialize the result.
    contact = QContact();

    //TODO: How can we AND on both the first and last name?
    getExistingContactFinishedCallback = finishedCallback; //Call this when the contact has been retrieved.
    connect(&contactFetchRequest, SIGNAL(resultsAvailable()),
        SLOT(onContactFetchRequestProgress()));

    QContactDetailFilter nameFilter;
    nameFilter.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirst);
    nameFilter.setValue(QLatin1String(TESTNAME_FIRST));
    nameFilter.setMatchFlags(QContactFilter::MatchExactly);
    contactFetchRequest.setManager(manager);
    contactFetchRequest.setFilter(nameFilter);

    //qDebug() << "debug: start request";
    contactFetchRequest.start();
}

//This is our actual test function:
void ut_qtcontacts_add::ut_testAddContact()
{
    //qDebug() << "debug: ut_testAddContact";
    //Make sure that the contact is not already in the database.
    getExistingContact(&ut_qtcontacts_add::onContactFoundThenRemoveAndTest);

    //Block (allowing the mainloop to run) until we have finished.
    waitForStop();
}

void ut_qtcontacts_add::onContactFoundThenRemoveAndTest()
{
    //qDebug() << "debug: Removing the existing contact, if it exists.";
    QContactManager* manager = getContactManager();
    Q_ASSERT(manager);

    //TODO: Find and use an async API that tells us when it has finished.
    manager->removeContact(contact.localId());

    // Run the Qt UI's main loop, showing the editor while it is running:
    QTimer::singleShot(2000, this, SLOT(onTimeoutAddContact()));
}

void ut_qtcontacts_add::onTimeoutAddContact()
{
    //qDebug() << "debug: Trying to add contact.";

    // Offer a UI to edit a prefilled contact.
    QContactName name;
    name.setFirstName(QLatin1String(TESTNAME_FIRST));
    name.setLastName(QLatin1String(TESTNAME_LAST));
    //TODO: Find and use an async API that tells us when it has finished.
    contact.saveDetail(&name);
    //const bool saved = contact.saveDetail(&name);
    //Q_ASSERT(saved); //This won't necessarily be useful because our implementation doesn't support sync methods.

    //Save the contact.
    //But note that our QContactManager backend does not set localId when returning.
    QContactManager* manager = getContactManager();
    Q_ASSERT(manager);


    manager->saveContact(&contact);
    //This works too:
    //QContact copy(contact);
    //manager->saveContact(&copy);

    //Check that it was really saved:
    //qDebug() << "debug: checking that the contact was saved.";
    getExistingContact(&ut_qtcontacts_add::onContactFoundThenCheck);
}

void ut_qtcontacts_add::onContactFoundThenCheck()
{
    //Check that it was really saved:
    // The ContactManager::saveContact() documentation suggests that localeId=0 is for non-saved contacts.
    //QEXPECT_FAIL("", "QContactManager::saveContact() saves the contact (see it by running the contacts UI), but returns false and doesn't set error(). Find out why.", Continue);
    QVERIFY(contact.localId() != 0);

    //Check that the correct details were saved:
    const QContactName name = contact.detail<QContactName>();
    QVERIFY(name.firstName() == QLatin1String(TESTNAME_FIRST));
    QVERIFY(name.lastName() == QLatin1String(TESTNAME_LAST));

    //Try to restore original conditions:
    getExistingContact(&ut_qtcontacts_add::onContactFoundThenRemoveAndStop);
}

void ut_qtcontacts_add::onContactFoundThenRemoveAndStop()
{
    //qDebug() << "debug: ut_qtcontacts_add::onContactFoundThenRemoveAndStop";
    QContactManager* manager = getContactManager();
    Q_ASSERT(manager);
    manager->removeContact(contact.localId());

    // Allow the actual test function to return:
    waiting = false;
}

void ut_qtcontacts_add::checkSubTypes()
{
    QContact contact;

    QContactManager* manager = getContactManager();

    QContactPhoneNumber phone;

    phone.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    phone.setContexts(QContactPhoneNumber::ContextHome);
    phone.setNumber("12345");

    contact.saveDetail(&phone);
    manager->saveContact(&contact);

    phone.setSubTypes(QContactPhoneNumber::SubTypeFacsimile);

    contact.saveDetail(&phone);
    manager->saveContact(&contact);

}

QTEST_MAIN(ut_qtcontacts_add)
