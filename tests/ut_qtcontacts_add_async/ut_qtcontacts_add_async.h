/* * This file is part of contacts *
 * Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 * Contact: Aleksandar Stojiljkovic <aleksandar.stojiljkovic@nokia.com>
 * This software, including documentation, is protected by copyright controlled by
 * Nokia Corporation. All rights are reserved. Copying, including reproducing, storing,
 * adapting or translating, any or all of this material requires the prior written consent
 * of Nokia Corporation. This material also contains confidential information which may
 * not be disclosed to others without the prior written consent of Nokia. */

#ifndef UT_CONTACTSGUI_EDITCONTACT_H_
#define UT_CONTACTSGUI_EDITCONTACT_H_

#include <QContact>
#include <QContactFetchRequest>
#include <QObject>
#include <QtTest/QtTest>

QTM_USE_NAMESPACE

class ut_qtcontacts_add : public QObject
{
Q_OBJECT
public:
    ut_qtcontacts_add();
    ~ut_qtcontacts_add();

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
    void onContactFoundThenRemoveAndTest();
    void onTimeoutAddContact();
    void onContactFoundThenCheck();
    void onContactFoundThenRemoveAndStop();

private:

    QContactManager* getContactManager();

    // Get the contact ID for the test contact if it exists already.
    QContact getExistingContact();

    typedef void (ut_qtcontacts_add::*FinishedCallbackFunc)(void);

    // Get the contact ID for the test contact if it exists already.
    void getExistingContact(FinishedCallbackFunc finishedCallback);

    // wait (allowing the mainloop to respond) until this->waiting is false.
    bool waitForStop();

    //A hacky way to bind an extra parameter to the Qt slot.
    FinishedCallbackFunc getExistingContactFinishedCallback;

    QContact contact;
    QContactFetchRequest contactFetchRequest;

    bool waiting;
};

#endif /* UT_CONTACTSGUI_H_ */
