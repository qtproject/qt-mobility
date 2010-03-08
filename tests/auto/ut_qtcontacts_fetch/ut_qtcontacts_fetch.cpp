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
#include <QContactDetailFilter>
#include <QtTest/QtTest>
#include <QDebug>

// Note that we try to avoid using any names that might already be in the database:
const char* TESTNAME_FIRST = "ut_qtcontacts_fetch_firstname";
const char* TESTNAME_LAST = "ut_qtcontacts_fetch_firstlast";

ut_qtcontacts_fetch::ut_qtcontacts_fetch()
: waiting(false),
  state(STATE_START)
{
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
}

void ut_qtcontacts_fetch::cleanup()
{
  waiting = false;
}

void ut_qtcontacts_fetch::doNextOperation()
{
     qDebug() << "ut_qtcontacts_fetch::doNextOperation(): state=" << state;

     switch(state) {
     case STATE_START:
         //qDebug() << "debug: ut_testAddContact";
         //Make sure that the contact is not already in the database.
         getExistingContact();

         //Block (allowing the mainloop to run) until we have finished.
         waitForStop();
         break;
     case STATE_INITIAL_EXISTING_FETCHED:
         removeContact();
         break;
     case STATE_INITIAL_EXISTING_REMOVED:
         addContact();
         break;
     case STATE_CONTACT_SAVED:
         //Get the contact, to check that the save worked:
         getExistingContact(true /* any_contact */);
         break;
     case STATE_AFTER_SAVE_FETCHED_FOR_CHECK:
         checkSavedContact();
         break;
     case STATE_AFTER_SAVE_CHECKED:
         //Get the saved contact, to remove it, to restore original conditions:
         getExistingContact();
         break;
     case STATE_AFTER_SAVE_FETCHED_FOR_REMOVAL:
         removeContact();
         break;
     case STATE_SAVED_CONTACT_REMOVED:
         //Allow our actual test function to return:
         waiting = false; //Make waitForStop() return.
         break;
     default:
         qDebug() << "ut_qtcontacts_fetch::doNextOperation(): Unexpected state: " << state;
         waiting = false;
         break;
    }
}

bool ut_qtcontacts_fetch::waitForStop()
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

QContactManager* ut_qtcontacts_fetch::getContactManager()
{
    static QContactManager manager("tracker");
    return &manager;
}

void ut_qtcontacts_fetch::onContactFetchRequestProgress()
{
    if (!contactFetchRequest.isFinished())
        return;

    qDebug() << "onContactFetchRequestProgress: state=: " << state;

    //Store the contact so the callback can use it.
    if(!(contactFetchRequest.contacts().isEmpty())) {
        contact = contactFetchRequest.contacts()[0];
        QVERIFY(contact.localId() != 0);
    }

    qDebug() << "debug: fetched localId=" << contact.localId();

    //Avoid more slot calls, though this is unlikely because it has finished.
    if(contactFetchRequest.isActive()) {
        const bool cancelled = contactFetchRequest.cancel();
        Q_ASSERT(cancelled);
    }

    //Choose the next state and do the next appropriate operation:
    if(state == STATE_START)
        state = STATE_INITIAL_EXISTING_FETCHED; //Or not found.
    else if(state == STATE_CONTACT_SAVED)
        state = STATE_AFTER_SAVE_FETCHED_FOR_CHECK; //Or not found.
    else if(state == STATE_AFTER_SAVE_CHECKED)
        state = STATE_AFTER_SAVE_FETCHED_FOR_REMOVAL;
    else {
        qDebug() << "ut_qtcontacts_fetch::onContactFetchRequestProgress(): Ignoring unexpected state: " << state
        << "  (Probably due to progress callbacks even after contactFetchRequest.cancel())";
        return;
    }
    QTimer::singleShot(1000, this, SLOT(doNextOperation()));
}


void ut_qtcontacts_fetch::getExistingContact(bool any_contact)
{
    qDebug() << "getExistingContact: state=: " << state;
    QContactManager* manager = getContactManager();
    Q_ASSERT(manager);

    // Stop pending fetch requests
    if (contactFetchRequest.isActive())
        contactFetchRequest.cancel();

    //Initialize the result:
    contact = QContact();

    //TODO: How can we AND on both the first and last name?
    connect(&contactFetchRequest, SIGNAL(resultsAvailable()),
        SLOT(onContactFetchRequestProgress()));

    if(!any_contact) {
        QContactDetailFilter nameFilter;
        nameFilter.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirst);
        nameFilter.setValue(QLatin1String(TESTNAME_FIRST));
        nameFilter.setMatchFlags(QContactFilter::MatchExactly);
        contactFetchRequest.setManager(manager);
        contactFetchRequest.setFilter(nameFilter);
    }

    //qDebug() << "debug: start request";
    contactFetchRequest.start();
}

//This is our actual test function:
void ut_qtcontacts_fetch::ut_testAddContact()
{
    state = STATE_START;
    doNextOperation();
}

void ut_qtcontacts_fetch::removeContact()
{
    qDebug() << "ut_qtcontacts_fetch::removeContact(): state:" << state;

    if (contact.localId() != 0) { //If it was found.
        //qDebug() << "debug: Removing the existing contact, if it exists.";
        QContactManager* manager = getContactManager();
        Q_ASSERT(manager);

        //TODO: Find and use an async API that tells us when it has finished.
        qDebug() << "  ut_qtcontacts_fetch::removeContact(): removing localID=" << contact.localId();
        manager->removeContact(contact.localId());
    }

    //Choose the next state and do the next appropriate operation:
    if(state == STATE_INITIAL_EXISTING_FETCHED)
        state = STATE_INITIAL_EXISTING_REMOVED;
    else if(state == STATE_AFTER_SAVE_FETCHED_FOR_REMOVAL) //Or not found.
        state = STATE_SAVED_CONTACT_REMOVED;
    else
        qDebug() << "ut_qtcontacts_fetch::removeContact: Unexpected state.";

    QTimer::singleShot(1000, this, SLOT(doNextOperation()));
}

void ut_qtcontacts_fetch::addContact()
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


    //manager->saveContact(&contact);
    //This works too:
    QContact copy(contact);
    manager->saveContact(&copy);

    //Check that it was really saved:
    //qDebug() << "debug: checking that the contact was saved.";
    state = STATE_CONTACT_SAVED;
    QTimer::singleShot(1000, this, SLOT(doNextOperation()));
}

void ut_qtcontacts_fetch::checkSavedContact()
{
    //Check that it was really saved:
    // The ContactManager::saveContact() documentation suggests that localeId=0 is for non-saved contacts.
    QVERIFY(contact.localId() != 0);

    //Check that the correct details were saved:
    const QContactName name = contact.detail<QContactName>();
    QVERIFY(name.firstName() == QLatin1String(TESTNAME_FIRST));
    QVERIFY(name.lastName() == QLatin1String(TESTNAME_LAST));

    qDebug() << "ut_qtcontacts_fetch::checkSavedContact(): found contact: firstName=" <<  name.firstName() <<
      ", lastName=" << name.lastName();

    //Try to restore original conditions:
    state = STATE_AFTER_SAVE_CHECKED;
    doNextOperation();
}


QTEST_MAIN(ut_qtcontacts_fetch)
