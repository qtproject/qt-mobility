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

#include <QContactAbstractRequest>
#include <QContactDetailFilter>

#include <QtTest/QtTest>
#include <QtTracker/ontologies/nco.h>

QTM_USE_NAMESPACE

using namespace SopranoLive;

class ut_qtcontacts_sparql : public QObject
{
    Q_OBJECT

public:
    ut_qtcontacts_sparql();
    ~ut_qtcontacts_sparql();

// private slots are called by the QTest framework.
private slots:
    // Per test class:
    void initTestCase();
    void cleanupTestCase();

    // Per test-function:
    void init();
    void cleanup();

    // Test functions:
    void ut_checkDatabaseEmpty();
    void ut_testSaveContact();
    void ut_testModifyContact();

// protected or public slots are _not_ called by the QTest framework.
protected slots:

private:
    void waitForRequest(QContactAbstractRequest &request, int timeout = 5000);

    void checkDatabaseEmpty();
    void setupTestContact(QContact &contact);
    void saveContact(QContact &contact);

    template <class PhoneNumberType>
    void checkOntology(QContactLocalId contactId, int expectedCount = 1);

    QContactManager* mContactManager;
    QContactDetailFilter mNameFilter;
    QList<QContactLocalId> mLocalIds;

    QString mFirstName;
    QString mLastName;
    QString mPhoneNumber;
};

#endif /* UT_CONTACTSGUI_H_ */
