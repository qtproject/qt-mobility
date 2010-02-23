/* * This file is part of contacts *
 * Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 * Contact: Aleksandar Stojiljkovic <aleksandar.stojiljkovic@nokia.com>
 * This software, including documentation, is protected by copyright controlled by
 * Nokia Corporation. All rights are reserved. Copying, including reproducing, storing,
 * adapting or translating, any or all of this material requires the prior written consent
 * of Nokia Corporation. This material also contains confidential information which may
 * not be disclosed to others without the prior written consent of Nokia. */

#include "ut_qtcontacts_add_async.h"

#include <QContactManager>
#include <QContactName>
#include <QContactDetailFilter>
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

QContact ut_qtcontacts_add::getExistingContact()
{
    QContactManager* manager = getContactManager();
    Q_ASSERT(manager);

    //TODO: How can we AND on both the first and last name?
    QContactDetailFilter nameFilter;
    nameFilter.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirst);
    nameFilter.setValue(TESTNAME_FIRST);
    nameFilter.setMatchFlags(QContactFilter::MatchExactly);

    //TODO: The extra empty parameters are necessary until the deprecated contacts() overloads are removed.
    QList<QContact> matchingContacts = manager->contacts(nameFilter,  QList<QContactSortOrder>(), QStringList());
    if (!matchingContacts.isEmpty()) {
        return matchingContacts.at(0);
    }

    return QContact();
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

    //TODO: How can we AND on both the first and last name?
    getExistingContactFinishedCallback = finishedCallback; //Call this when the contact has been retrieved.
    connect(&contactFetchRequest, SIGNAL(resultsAvailable()),
        SLOT(onContactFetchRequestProgress()));

    QContactDetailFilter nameFilter;
    nameFilter.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirst);
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
    name.setFirstName(TESTNAME_FIRST);
    name.setLastName(TESTNAME_LAST);
    //TODO: Find and use an async API that tells us when it has finished.
    contact.saveDetail(&name);
    //const bool saved = contact.saveDetail(&name);
    //Q_ASSERT(saved); //This won't necessarily be useful because our implementation doesn't support sync methods.

    //Save the contact.
    //But note that our QContactManager backend does not set localId when returning.
    QContactManager* manager = getContactManager();
    Q_ASSERT(manager);


    manager->saveContact(&contact);
    //TODO: Saving a copy of the QContact causes QContactFetchRequest to not find the saved contact afterwards.
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
    QVERIFY(name.firstName() == TESTNAME_FIRST);
    QVERIFY(name.lastName() == TESTNAME_LAST);

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



QTEST_MAIN(ut_qtcontacts_add)
