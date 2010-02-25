/* * This file is part of contacts *
 * Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 * Contact: Aleksandar Stojiljkovic <aleksandar.stojiljkovic@nokia.com>
 * This software, including documentation, is protected by copyright controlled by
 * Nokia Corporation. All rights are reserved. Copying, including reproducing, storing,
 * adapting or translating, any or all of this material requires the prior written consent
 * of Nokia Corporation. This material also contains confidential information which may
 * not be disclosed to others without the prior written consent of Nokia. */

#ifndef UT_QTCONTACTS_TRACKERPLUGIN_FETCH_H
#define UT_QTCONTACTS_TRACKERPLUGIN_FETCH_H

#include <QContact>
#include <QContactFetchRequest>
#include <QObject>
#include <QtTest/QtTest>

QTM_USE_NAMESPACE

class ut_qtcontacts_fetch : public QObject
{
Q_OBJECT
public:
    ut_qtcontacts_fetch();
    ~ut_qtcontacts_fetch();

// Private slots are called by the QTest framework.
private slots:
    // Per test class:
    void initTestCase();
    void cleanupTestCase();

    // Per test-function:
    void init();
    void cleanup();

    // Test functions:
    void ut_testAddContact();

// Protected or public slots are _not_ called by the QTest framework.
protected slots:
    void onContactFetchRequestProgress();

    /** Do the next step, as dictated by the value of this->state.
     * This is a slot so it can be called in a timeout, to make sure that
     * we use the QContacts API semi-asynchronously.
     */
    void doNextOperation();

private:

    void removeContact();
    void addContact();
    void checkSavedContact();
    void fetchSavedForRemoval();

    QContactManager* getContactManager();

    /** Get the contact ID for the test contact if it exists already.
     * This then sets the next appropriate state and does the next step.
     *
     * @param any_contact Whether we should check for any contact at all. Otherwise check for the specific one.
     */
    void getExistingContact(bool any_contact = false);

    // wait (allowing the mainloop to respond) until this->waiting is false.
    bool waitForStop();

    QContact contact;
    QContactFetchRequest contactFetchRequest;

    bool waiting;

    // We must use async QtContacts API,
    // because the sync API is not supported with qtcontacts-tracker,
    // so this tells us what our callbacks should do next.
    enum State
    {
        STATE_START = 0,
        STATE_INITIAL_EXISTING_FETCHED, //Or not found.
        STATE_INITIAL_EXISTING_REMOVED,
        STATE_CONTACT_SAVED,
        STATE_AFTER_SAVE_FETCHED_FOR_CHECK, //Or not found.
        STATE_AFTER_SAVE_CHECKED,
        STATE_AFTER_SAVE_FETCHED_FOR_REMOVAL, //Or not found.
        STATE_SAVED_CONTACT_REMOVED
    };
    State state;



};

#endif /* UT_CONTACTSGUI_H_ */
