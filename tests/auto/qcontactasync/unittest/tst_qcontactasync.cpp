/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>

#include "qtcontacts.h"

//TESTED_CLASS=
//TESTED_FILES=

class tst_QContactAsync : public QObject
{
    Q_OBJECT

public:
    tst_QContactAsync();
    virtual ~tst_QContactAsync();

public slots:
    void init();
    void cleanup();

private slots:
    void testDestructor();

    void contactFetch();
    void contactIdFetch();
    void contactRemove();
    void contactSave();

    void groupFetch();
    void groupRemove();
    void groupSave();

    void definitionFetch();
    void definitionRemove();
    void definitionSave();

    void maliciousManager();

private:
    bool containsIgnoringTimestamps(const QList<QContact>& list, const QContact& c);
    bool compareIgnoringTimestamps(const QContact& ca, const QContact& cb);
    QContactManager* prepareModel();
};

tst_QContactAsync::tst_QContactAsync()
{
}

tst_QContactAsync::~tst_QContactAsync()
{
}

void tst_QContactAsync::init()
{
}

void tst_QContactAsync::cleanup()
{
}

bool tst_QContactAsync::containsIgnoringTimestamps(const QList<QContact>& list, const QContact& c)
{
    QList<QContact> cl = list;
    QContact a(c);
    for (int i = 0; i < cl.size(); i++) {
        QContact b(cl.at(i));
        if (compareIgnoringTimestamps(a, b))
            return true;
    }

    return false;
}

bool tst_QContactAsync::compareIgnoringTimestamps(const QContact& ca, const QContact& cb)
{
    // Compares two contacts, ignoring any timestamp details
    QContact a(ca);
    QContact b(cb);
    QList<QContactDetail> aDetails = a.details();
    QList<QContactDetail> bDetails = b.details();

    // They can be in any order, so loop
    // First remove any matches, and any timestamps
    foreach (QContactDetail d, aDetails) {
        foreach (QContactDetail d2, bDetails) {
            if (d == d2) {
                a.removeDetail(&d);
                b.removeDetail(&d2);
                break;
            }

            if (d.definitionName() == QContactTimestamp::DefinitionName) {
                a.removeDetail(&d);
            }

            if (d2.definitionName() == QContactTimestamp::DefinitionName) {
                b.removeDetail(&d2);
            }
        }
    }

    if (a == b)
        return true;
    return false;
}

void tst_QContactAsync::testDestructor()
{
    QContactManager* cm = prepareModel();
    QContactFetchRequest* req = new QContactFetchRequest;
    req->setManager(cm);

    QContactManager* cm2 = prepareModel();
    QContactFetchRequest* req2 = new QContactFetchRequest;
    req2->setManager(cm2);

    // first, delete manager then request
    delete cm;
    delete req;

    // second, delete request then manager
    delete req2;
    delete cm2;
}

void tst_QContactAsync::contactFetch()
{
    QContactManager* cm = prepareModel();
    QContactFetchRequest cfr;
    QVERIFY(cfr.type() == QContactAbstractRequest::ContactFetch);

    // initial state - not started, no manager.
    QVERIFY(!cfr.isActive());
    QVERIFY(!cfr.isFinished());
    QVERIFY(!cfr.start());
    QVERIFY(!cfr.cancel());
    QVERIFY(!cfr.waitForFinished());
    QVERIFY(!cfr.waitForProgress());

    // "all contacts" retrieval
    QContactFilter fil;
    cfr.setManager(cm);
    QCOMPARE(cfr.manager(), cm);
    QVERIFY(!cfr.isActive());
    QVERIFY(!cfr.isFinished());
    QVERIFY(!cfr.cancel());
    QVERIFY(!cfr.waitForFinished());
    QVERIFY(!cfr.waitForProgress());
    qRegisterMetaType<QContactFetchRequest*>("QContactFetchRequest*");
    QSignalSpy spy(&cfr, SIGNAL(progress(QContactFetchRequest*, bool)));
    cfr.setFilter(fil);
    QCOMPARE(cfr.filter(), fil);
    QVERIFY(!cfr.cancel()); // not started
    QVERIFY(cfr.start());
    QVERIFY(cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!cfr.isFinished());
    QVERIFY(!cfr.start());  // already started.
    QVERIFY(cfr.waitForFinished());
    int expectedCount = 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(cfr.isFinished());
    QVERIFY(!cfr.isActive());

    QList<QUniqueId> contactIds = cm->contacts();
    QList<QContact> contacts = cfr.contacts();
    QCOMPARE(contactIds.size(), contacts.size());
    for (int i = 0; i < contactIds.size(); i++) {
        QContact curr = cm->contact(contactIds.at(i));
        QVERIFY(contacts.at(i) == curr);
    }

    // asynchronous detail filtering
    QContactDetailFilter dfil;
    dfil.setDetailDefinitionName(QContactUrl::DefinitionName, QContactUrl::FieldUrl);
    cfr.setFilter(dfil);
    QVERIFY(cfr.filter() == dfil);
    QVERIFY(!cfr.cancel()); // not started
    QVERIFY(cfr.start());
    QVERIFY(cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!cfr.isFinished());
    QVERIFY(!cfr.start());  // already started.
    QVERIFY(cfr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(cfr.isFinished());
    QVERIFY(!cfr.isActive());

    contactIds = cm->contacts(dfil);
    contacts = cfr.contacts();
    QCOMPARE(contactIds.size(), contacts.size());
    for (int i = 0; i < contactIds.size(); i++) {
        QContact curr = cm->contact(contactIds.at(i));
        QVERIFY(contacts.at(i) == curr);
    }

    // sort order
    QContactSortOrder sortOrder;
    sortOrder.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
    QList<QContactSortOrder> sorting;
    sorting.append(sortOrder);
    cfr.setFilter(fil);
    cfr.setSorting(sorting);
    QCOMPARE(cfr.sorting(), sorting);
    QVERIFY(!cfr.cancel()); // not started
    QVERIFY(cfr.start());
    QVERIFY(cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!cfr.isFinished());
    QVERIFY(!cfr.start());  // already started.
    QVERIFY(cfr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(cfr.isFinished());
    QVERIFY(!cfr.isActive());

    contactIds = cm->contacts(sorting);
    contacts = cfr.contacts();
    QCOMPARE(contactIds.size(), contacts.size());
    for (int i = 0; i < contactIds.size(); i++) {
        QContact curr = cm->contact(contactIds.at(i));
        QVERIFY(contacts.at(i) == curr);
    }

    // restrictions
    sorting.clear();
    cfr.setFilter(fil);
    cfr.setSorting(sorting);
    cfr.setDefinitionRestrictions(QStringList(QContactDisplayLabel::DefinitionName));
    QCOMPARE(cfr.definitionRestrictions(), QStringList(QContactDisplayLabel::DefinitionName));
    QVERIFY(!cfr.cancel()); // not started
    QVERIFY(cfr.start());
    QVERIFY(cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!cfr.isFinished());
    QVERIFY(!cfr.start());  // already started.
    QVERIFY(cfr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(cfr.isFinished());
    QVERIFY(!cfr.isActive());

    contactIds = cm->contacts(sorting);
    contacts = cfr.contacts();
    QCOMPARE(contactIds.size(), contacts.size());
    for (int i = 0; i < contactIds.size(); i++) {
        // create a contact from the restricted data only (id + display label)
        QContact currFull = cm->contact(contactIds.at(i));
        QContact currRestricted;
        currRestricted.setId(currFull.id());
        currRestricted.setDisplayLabel(currFull.displayLabel());
        QVERIFY(contacts.at(i) == currRestricted);
    }

    // cancelling
    sorting.clear();
    cfr.setFilter(fil);
    cfr.setSorting(sorting);
    cfr.setDefinitionRestrictions(QStringList());
    QVERIFY(!cfr.cancel()); // not started
    QVERIFY(cfr.start());
    QVERIFY(cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!cfr.isFinished());
    QVERIFY(cfr.cancel());
    QVERIFY(cfr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(cfr.isActive());    // still cancelling
    QVERIFY(!cfr.isFinished()); // not finished cancelling
    QVERIFY(!cfr.start());      // already started.
    QVERIFY(cfr.waitForFinished());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(cfr.isFinished());
    QVERIFY(!cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Cancelled);

    // restart, and wait for progress after cancel.
    QVERIFY(!cfr.cancel()); // not started
    QVERIFY(cfr.start());
    QVERIFY(cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!cfr.isFinished());
    QVERIFY(cfr.cancel());
    QVERIFY(cfr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(cfr.isActive());    // still cancelling
    QVERIFY(!cfr.isFinished()); // not finished cancelling
    QVERIFY(!cfr.start());      // already started.
    QVERIFY(cfr.waitForProgress());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(cfr.isFinished());
    QVERIFY(!cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Cancelled);

    delete cm;
}

void tst_QContactAsync::contactIdFetch()
{
    QContactManager* cm = prepareModel();
    QContactIdFetchRequest cfr;
    QVERIFY(cfr.type() == QContactAbstractRequest::ContactIdFetch);

    // initial state - not started, no manager.
    QVERIFY(!cfr.isActive());
    QVERIFY(!cfr.isFinished());
    QVERIFY(!cfr.start());
    QVERIFY(!cfr.cancel());
    QVERIFY(!cfr.waitForFinished());
    QVERIFY(!cfr.waitForProgress());

    // "all contacts" retrieval
    QContactFilter fil;
    cfr.setManager(cm);
    QCOMPARE(cfr.manager(), cm);
    QVERIFY(!cfr.isActive());
    QVERIFY(!cfr.isFinished());
    QVERIFY(!cfr.cancel());
    QVERIFY(!cfr.waitForFinished());
    QVERIFY(!cfr.waitForProgress());
    qRegisterMetaType<QContactIdFetchRequest*>("QContactIdFetchRequest*");
    QSignalSpy spy(&cfr, SIGNAL(progress(QContactIdFetchRequest*, bool)));
    cfr.setFilter(fil);
    QCOMPARE(cfr.filter(), fil);
    QVERIFY(!cfr.cancel()); // not started
    QVERIFY(cfr.start());
    QVERIFY(cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!cfr.isFinished());
    QVERIFY(!cfr.start());  // already started.
    QVERIFY(cfr.waitForFinished());
    int expectedCount = 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(cfr.isFinished());
    QVERIFY(!cfr.isActive());

    QList<QUniqueId> contactIds = cm->contacts();
    QList<QUniqueId> result = cfr.ids();
    QCOMPARE(contactIds, result);

    // asynchronous detail filtering
    QContactDetailFilter dfil;
    dfil.setDetailDefinitionName(QContactUrl::DefinitionName, QContactUrl::FieldUrl);
    cfr.setFilter(dfil);
    QVERIFY(cfr.filter() == dfil);
    QVERIFY(!cfr.cancel()); // not started
    QVERIFY(cfr.start());
    QVERIFY(cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!cfr.isFinished());
    QVERIFY(!cfr.start());  // already started.
    QVERIFY(cfr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(cfr.isFinished());
    QVERIFY(!cfr.isActive());

    contactIds = cm->contacts(dfil);
    result = cfr.ids();
    QCOMPARE(contactIds, result);

    // sort order
    QContactSortOrder sortOrder;
    sortOrder.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
    QList<QContactSortOrder> sorting;
    sorting.append(sortOrder);
    cfr.setFilter(fil);
    cfr.setSorting(sorting);
    QCOMPARE(cfr.sorting(), sorting);
    QVERIFY(!cfr.cancel()); // not started
    QVERIFY(cfr.start());
    QVERIFY(cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!cfr.isFinished());
    QVERIFY(!cfr.start());  // already started.
    QVERIFY(cfr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(cfr.isFinished());
    QVERIFY(!cfr.isActive());

    contactIds = cm->contacts(sorting);
    result = cfr.ids();
    QCOMPARE(contactIds, result);

    // cancelling
    sorting.clear();
    cfr.setFilter(fil);
    cfr.setSorting(sorting);
    QVERIFY(!cfr.cancel()); // not started
    QVERIFY(cfr.start());
    QVERIFY(cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!cfr.isFinished());
    QVERIFY(cfr.cancel());
    QVERIFY(cfr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(cfr.isActive());    // still cancelling
    QVERIFY(!cfr.isFinished()); // not finished cancelling
    QVERIFY(!cfr.start());      // already started.
    QVERIFY(cfr.waitForFinished());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(cfr.isFinished());
    QVERIFY(!cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Cancelled);

    // restart, and wait for progress after cancel.
    QVERIFY(!cfr.cancel()); // not started
    QVERIFY(cfr.start());
    QVERIFY(cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!cfr.isFinished());
    QVERIFY(cfr.cancel());
    QVERIFY(cfr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(cfr.isActive());    // still cancelling
    QVERIFY(!cfr.isFinished()); // not finished cancelling
    QVERIFY(!cfr.start());      // already started.
    QVERIFY(cfr.waitForProgress());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(cfr.isFinished());
    QVERIFY(!cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Cancelled);

    delete cm;
}

void tst_QContactAsync::contactRemove()
{
    QContactManager* cm = prepareModel();
    QContactRemoveRequest crr;
    QVERIFY(crr.type() == QContactAbstractRequest::ContactRemove);

    // initial state - not started, no manager.
    QVERIFY(!crr.isActive());
    QVERIFY(!crr.isFinished());
    QVERIFY(!crr.start());
    QVERIFY(!crr.cancel());
    QVERIFY(!crr.waitForFinished());
    QVERIFY(!crr.waitForProgress());

    // specific contact removal
    int originalCount = cm->contacts().size();
    QContactDetailFilter dfil;
    dfil.setDetailDefinitionName(QContactUrl::DefinitionName, QContactUrl::FieldUrl);
    crr.setFilter(dfil);
    crr.setManager(cm);
    QCOMPARE(crr.manager(), cm);
    QVERIFY(!crr.isActive());
    QVERIFY(!crr.isFinished());
    QVERIFY(!crr.cancel());
    QVERIFY(!crr.waitForFinished());
    QVERIFY(!crr.waitForProgress());
    qRegisterMetaType<QContactRemoveRequest*>("QContactRemoveRequest*");
    QSignalSpy spy(&crr, SIGNAL(progress(QContactRemoveRequest*)));
    QVERIFY(crr.filter() == dfil);
    QVERIFY(!crr.cancel()); // not started
    QVERIFY(crr.start());
    QVERIFY(crr.isActive());
    QVERIFY(crr.status() == QContactAbstractRequest::Active);
    QVERIFY(!crr.isFinished());
    QVERIFY(!crr.start());  // already started.
    QVERIFY(crr.waitForFinished());
    int expectedCount = 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(crr.isFinished());
    QVERIFY(!crr.isActive());

    QCOMPARE(cm->contacts().size(), originalCount - 1);
    QVERIFY(cm->contacts(dfil).isEmpty());

    // remove all contacts
    dfil.setDetailDefinitionName(QContactDisplayLabel::DefinitionName); // delete everything.
    crr.setFilter(dfil);
    QVERIFY(crr.filter() == dfil);
    QVERIFY(!crr.cancel()); // not started
    QVERIFY(crr.start());
    QVERIFY(crr.isActive());
    QVERIFY(crr.status() == QContactAbstractRequest::Active);
    QVERIFY(!crr.isFinished());
    QVERIFY(!crr.start());  // already started.
    QVERIFY(crr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(crr.isFinished());
    QVERIFY(!crr.isActive());

    QCOMPARE(cm->contacts().size(), 0); // no contacts should be left.

    // cancelling
    QContact temp;
    temp.setDisplayLabel("should not be removed");
    cm->saveContact(&temp);
    crr.setFilter(dfil);
    QVERIFY(!crr.cancel()); // not started
    QVERIFY(crr.start());
    QVERIFY(crr.isActive());
    QVERIFY(crr.status() == QContactAbstractRequest::Active);
    QVERIFY(!crr.isFinished());
    QVERIFY(crr.cancel());
    QVERIFY(crr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(crr.isActive());    // still cancelling
    QVERIFY(!crr.isFinished()); // not finished cancelling
    QVERIFY(!crr.start());      // already started.
    QVERIFY(crr.waitForFinished());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(crr.isFinished());
    QVERIFY(!crr.isActive());
    QVERIFY(crr.status() == QContactAbstractRequest::Cancelled);

    QCOMPARE(cm->contacts().size(), 1);
    QCOMPARE(cm->contact(cm->contacts().first()), temp);

    // restart, and wait for progress after cancel.
    QVERIFY(!crr.cancel()); // not started
    QVERIFY(crr.start());
    QVERIFY(crr.isActive());
    QVERIFY(crr.status() == QContactAbstractRequest::Active);
    QVERIFY(!crr.isFinished());
    QVERIFY(crr.cancel());
    QVERIFY(crr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(crr.isActive());    // still cancelling
    QVERIFY(!crr.isFinished()); // not finished cancelling
    QVERIFY(!crr.start());      // already started.
    QVERIFY(crr.waitForProgress());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(crr.isFinished());
    QVERIFY(!crr.isActive());
    QVERIFY(crr.status() == QContactAbstractRequest::Cancelled);

    QCOMPARE(cm->contacts().size(), 1);
    QCOMPARE(cm->contact(cm->contacts().first()), temp);

    delete cm;
}

void tst_QContactAsync::contactSave()
{
    QContactManager* cm = prepareModel();
    QContactSaveRequest csr;
    QVERIFY(csr.type() == QContactAbstractRequest::ContactSave);

    // initial state - not started, no manager.
    QVERIFY(!csr.isActive());
    QVERIFY(!csr.isFinished());
    QVERIFY(!csr.start());
    QVERIFY(!csr.cancel());
    QVERIFY(!csr.waitForFinished());
    QVERIFY(!csr.waitForProgress());

    // save a new contact
    int originalCount = cm->contacts().size();
    QContact testContact;
    testContact.setDisplayLabel("Test Contact");
    QList<QContact> saveList;
    saveList << testContact;
    csr.setManager(cm);
    QCOMPARE(csr.manager(), cm);
    QVERIFY(!csr.isActive());
    QVERIFY(!csr.isFinished());
    QVERIFY(!csr.cancel());
    QVERIFY(!csr.waitForFinished());
    QVERIFY(!csr.waitForProgress());
    qRegisterMetaType<QContactSaveRequest*>("QContactSaveRequest*");
    QSignalSpy spy(&csr, SIGNAL(progress(QContactSaveRequest*)));
    csr.setContacts(saveList);
    QCOMPARE(csr.contacts(), saveList);
    QVERIFY(!csr.cancel()); // not started
    QVERIFY(csr.start());
    QVERIFY(csr.isActive());
    QVERIFY(csr.status() == QContactAbstractRequest::Active);
    QVERIFY(!csr.isFinished());
    QVERIFY(!csr.start());  // already started.
    QVERIFY(csr.waitForFinished());
    int expectedCount = 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(csr.isFinished());
    QVERIFY(!csr.isActive());

    QList<QContact> expected;
    expected << cm->contact(cm->contacts().last());
    QList<QContact> result = csr.contacts();
    QCOMPARE(expected, result);
    QCOMPARE(cm->contacts().size(), originalCount + 1);

    // update a previously saved contact
    QContactPhoneNumber phn;
    phn.setNumber("12345678");
    testContact = expected.first();
    testContact.saveDetail(&phn);
    saveList.clear();
    saveList << testContact;
    csr.setContacts(saveList);
    QCOMPARE(csr.contacts(), saveList);
    QVERIFY(!csr.cancel()); // not started
    QVERIFY(csr.start());
    QVERIFY(csr.isActive());
    QVERIFY(csr.status() == QContactAbstractRequest::Active);
    QVERIFY(!csr.isFinished());
    QVERIFY(!csr.start());  // already started.
    QVERIFY(csr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(csr.isFinished());
    QVERIFY(!csr.isActive());

    expected.clear();
    expected << cm->contact(cm->contacts().last());
    result = csr.contacts();
    QCOMPARE(expected, result);
    QVERIFY(containsIgnoringTimestamps(expected, testContact));
    QCOMPARE(cm->contacts().size(), originalCount + 1);

    // cancelling
    QContact temp = testContact;
    QContactUrl url;
    url.setUrl("should not get saved");
    temp.saveDetail(&url);
    saveList.clear();
    saveList << temp;
    csr.setContacts(saveList);
    QVERIFY(!csr.cancel()); // not started
    QVERIFY(csr.start());
    QVERIFY(csr.isActive());
    QVERIFY(csr.status() == QContactAbstractRequest::Active);
    QVERIFY(!csr.isFinished());
    QVERIFY(csr.cancel());
    QVERIFY(csr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(csr.isActive());    // still cancelling
    QVERIFY(!csr.isFinished()); // not finished cancelling
    QVERIFY(!csr.start());      // already started.
    QVERIFY(csr.waitForFinished());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(csr.isFinished());
    QVERIFY(!csr.isActive());
    QVERIFY(csr.status() == QContactAbstractRequest::Cancelled);

    // verify that the changes were not saved
    expected.clear();
    QList<QUniqueId> allContacts = cm->contacts();
    for (int i = 0; i < allContacts.size(); i++) {
        expected.append(cm->contact(allContacts.at(i)));
    }
    QVERIFY(!expected.contains(temp));
    QCOMPARE(cm->contacts().size(), originalCount + 1);

    // restart, and wait for progress after cancel.
    QVERIFY(!csr.cancel()); // not started
    QVERIFY(csr.start());
    QVERIFY(csr.isActive());
    QVERIFY(csr.status() == QContactAbstractRequest::Active);
    QVERIFY(!csr.isFinished());
    QVERIFY(csr.cancel());
    QVERIFY(csr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(csr.isActive());    // still cancelling
    QVERIFY(!csr.isFinished()); // not finished cancelling
    QVERIFY(!csr.start());      // already started.
    QVERIFY(csr.waitForProgress());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(csr.isFinished());
    QVERIFY(!csr.isActive());
    QVERIFY(csr.status() == QContactAbstractRequest::Cancelled);

    // verify that the changes were not saved
    expected.clear();
    allContacts = cm->contacts();
    for (int i = 0; i < allContacts.size(); i++) {
        expected.append(cm->contact(allContacts.at(i)));
    }
    QVERIFY(!expected.contains(temp));
    QCOMPARE(cm->contacts().size(), originalCount + 1);

    delete cm;
}

void tst_QContactAsync::groupFetch()
{
    QContactManager* cm = prepareModel();
    QContactGroupFetchRequest gfr;
    QVERIFY(gfr.type() == QContactAbstractRequest::GroupFetch);

    // initial state - not started, no manager.
    QVERIFY(!gfr.isActive());
    QVERIFY(!gfr.isFinished());
    QVERIFY(!gfr.start());
    QVERIFY(!gfr.cancel());
    QVERIFY(!gfr.waitForFinished());
    QVERIFY(!gfr.waitForProgress());

    // "all groups" retrieval
    gfr.setManager(cm);
    QCOMPARE(gfr.manager(), cm);
    QVERIFY(!gfr.isActive());
    QVERIFY(!gfr.isFinished());
    QVERIFY(!gfr.cancel());
    QVERIFY(!gfr.waitForFinished());
    QVERIFY(!gfr.waitForProgress());
    qRegisterMetaType<QContactGroupFetchRequest*>("QContactGroupFetchRequest*");
    QSignalSpy spy(&gfr, SIGNAL(progress(QContactGroupFetchRequest*, bool)));
    gfr.setIds(QList<QUniqueId>());
    QVERIFY(!gfr.cancel()); // not started
    QVERIFY(gfr.start());
    QVERIFY(gfr.isActive());
    QVERIFY(gfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!gfr.isFinished());
    QVERIFY(!gfr.start());  // already started.
    QVERIFY(gfr.waitForFinished());
    int expectedCount = 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(gfr.isFinished());
    QVERIFY(!gfr.isActive());

    QList<QUniqueId> groupIds = cm->groups();
    QList<QContactGroup> result = gfr.groups();
    QCOMPARE(groupIds.size(), result.size());
    for (int i = 0; i < groupIds.size(); i++) {
        QContactGroup curr = cm->group(groupIds.at(i));
        QContactGroup currResult = result.at(i);
        QVERIFY(currResult == curr);
    }

    // specific group retrieval
    groupIds.clear();
    groupIds.append(cm->groups().first());
    gfr.setIds(groupIds);
    QCOMPARE(gfr.ids(), groupIds);
    QVERIFY(!gfr.cancel()); // not started
    QVERIFY(gfr.start());
    QVERIFY(gfr.isActive());
    QVERIFY(gfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!gfr.isFinished());
    QVERIFY(!gfr.start());  // already started.
    QVERIFY(gfr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(gfr.isFinished());
    QVERIFY(!gfr.isActive());

    QList<QContactGroup> expected;
    expected.append(cm->group(cm->groups().first()));
    result = gfr.groups();
    QCOMPARE(expected, result);

    // cancelling
    gfr.setIds(QList<QUniqueId>());
    QVERIFY(!gfr.cancel()); // not started
    QVERIFY(gfr.start());
    QVERIFY(gfr.isActive());
    QVERIFY(gfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!gfr.isFinished());
    QVERIFY(gfr.cancel());
    QVERIFY(gfr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(gfr.isActive());    // still cancelling
    QVERIFY(!gfr.isFinished()); // not finished cancelling
    QVERIFY(!gfr.start());      // already started.
    QVERIFY(gfr.waitForFinished());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(gfr.isFinished());
    QVERIFY(!gfr.isActive());
    QVERIFY(gfr.status() == QContactAbstractRequest::Cancelled);

    // restart, and wait for progress after cancel.
    QVERIFY(!gfr.cancel()); // not started
    QVERIFY(gfr.start());
    QVERIFY(gfr.isActive());
    QVERIFY(gfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!gfr.isFinished());
    QVERIFY(gfr.cancel());
    QVERIFY(gfr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(gfr.isActive());    // still cancelling
    QVERIFY(!gfr.isFinished()); // not finished cancelling
    QVERIFY(!gfr.start());      // already started.
    QVERIFY(gfr.waitForProgress());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(gfr.isFinished());
    QVERIFY(!gfr.isActive());
    QVERIFY(gfr.status() == QContactAbstractRequest::Cancelled);

    delete cm;
}

void tst_QContactAsync::groupRemove()
{
    QContactManager* cm = prepareModel();
    QContactGroupRemoveRequest grr;
    QVERIFY(grr.type() == QContactAbstractRequest::GroupRemove);

    // initial state - not started, no manager.
    QVERIFY(!grr.isActive());
    QVERIFY(!grr.isFinished());
    QVERIFY(!grr.start());
    QVERIFY(!grr.cancel());
    QVERIFY(!grr.waitForFinished());
    QVERIFY(!grr.waitForProgress());

    // specific group removal
    int originalCount = cm->groups().size();
    QList<QUniqueId> removeIds;
    removeIds << cm->groups().first();
    grr.setIds(removeIds);
    grr.setManager(cm);
    QCOMPARE(grr.manager(), cm);
    QVERIFY(!grr.isActive());
    QVERIFY(!grr.isFinished());
    QVERIFY(!grr.cancel());
    QVERIFY(!grr.waitForFinished());
    QVERIFY(!grr.waitForProgress());
    qRegisterMetaType<QContactGroupRemoveRequest*>("QContactGroupRemoveRequest*");
    QSignalSpy spy(&grr, SIGNAL(progress(QContactGroupRemoveRequest*)));
    QVERIFY(grr.ids() == removeIds);
    QVERIFY(!grr.cancel()); // not started
    QVERIFY(grr.start());
    QVERIFY(grr.isActive());
    QVERIFY(grr.status() == QContactAbstractRequest::Active);
    QVERIFY(!grr.isFinished());
    QVERIFY(!grr.start());  // already started.
    QVERIFY(grr.waitForFinished());
    int expectedCount = 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(grr.isFinished());
    QVERIFY(!grr.isActive());

    QCOMPARE(cm->groups().size(), originalCount - 1);
    cm->group(removeIds.first()); // check that it has already been removed.
    QCOMPARE(cm->error(), QContactManager::DoesNotExistError);

    // remove (asynchronously) a nonexistent group - should fail.
    grr.setIds(removeIds);
    QVERIFY(!grr.cancel()); // not started
    QVERIFY(grr.start());
    QVERIFY(grr.isActive());
    QVERIFY(grr.status() == QContactAbstractRequest::Active);
    QVERIFY(!grr.isFinished());
    QVERIFY(!grr.start());  // already started.
    QVERIFY(grr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(grr.isFinished());
    QVERIFY(!grr.isActive());

    QCOMPARE(cm->groups().size(), originalCount - 1); // hasn't changed
    QCOMPARE(grr.error(), QContactManager::DoesNotExistError);

    // remove with list containing one valid and one invalid id.
    removeIds << cm->groups().first();
    grr.setIds(removeIds);
    QVERIFY(!grr.cancel()); // not started
    QVERIFY(grr.start());
    QVERIFY(grr.isActive());
    QVERIFY(grr.status() == QContactAbstractRequest::Active);
    QVERIFY(!grr.isFinished());
    QVERIFY(!grr.start());  // already started.
    QVERIFY(grr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(grr.isFinished());
    QVERIFY(!grr.isActive());

    QCOMPARE(cm->groups().size(), originalCount - 2); // only one more has been removed
    QCOMPARE(grr.errors().first(), QContactManager::DoesNotExistError);
    QCOMPARE(grr.errors().at(1), QContactManager::NoError);

    // remove with empty list - nothing should happen.
    removeIds.clear();
    grr.setIds(removeIds);
    QVERIFY(!grr.cancel()); // not started
    QVERIFY(grr.start());
    QVERIFY(grr.isActive());
    QVERIFY(grr.status() == QContactAbstractRequest::Active);
    QVERIFY(!grr.isFinished());
    QVERIFY(!grr.start());  // already started.
    QVERIFY(grr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(grr.isFinished());
    QVERIFY(!grr.isActive());

    QCOMPARE(cm->groups().size(), originalCount - 2); // hasn't changed
    QCOMPARE(grr.error(), QContactManager::NoError);  // no error but no effect.

    // cancelling
    removeIds.clear();
    removeIds << cm->groups().first();
    grr.setIds(removeIds);
    QVERIFY(!grr.cancel()); // not started
    QVERIFY(grr.start());
    QVERIFY(grr.isActive());
    QVERIFY(grr.status() == QContactAbstractRequest::Active);
    QVERIFY(!grr.isFinished());
    QVERIFY(grr.cancel());
    QVERIFY(grr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(grr.isActive());    // still cancelling
    QVERIFY(!grr.isFinished()); // not finished cancelling
    QVERIFY(!grr.start());      // already started.
    QVERIFY(grr.waitForFinished());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(grr.isFinished());
    QVERIFY(!grr.isActive());
    QVERIFY(grr.status() == QContactAbstractRequest::Cancelled);

    QCOMPARE(cm->groups().size(), originalCount - 2); // hasn't changed

    // restart, and wait for progress after cancel.
    QVERIFY(!grr.cancel()); // not started
    QVERIFY(grr.start());
    QVERIFY(grr.isActive());
    QVERIFY(grr.status() == QContactAbstractRequest::Active);
    QVERIFY(!grr.isFinished());
    QVERIFY(grr.cancel());
    QVERIFY(grr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(grr.isActive());    // still cancelling
    QVERIFY(!grr.isFinished()); // not finished cancelling
    QVERIFY(!grr.start());      // already started.
    QVERIFY(grr.waitForProgress());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(grr.isFinished());
    QVERIFY(!grr.isActive());
    QVERIFY(grr.status() == QContactAbstractRequest::Cancelled);

    QCOMPARE(cm->groups().size(), originalCount - 2); // hasn't changed

    delete cm;
}

void tst_QContactAsync::groupSave()
{
    QContactManager* cm = prepareModel();
    QContactGroupSaveRequest gsr;
    QVERIFY(gsr.type() == QContactAbstractRequest::GroupSave);

    // initial state - not started, no manager.
    QVERIFY(!gsr.isActive());
    QVERIFY(!gsr.isFinished());
    QVERIFY(!gsr.start());
    QVERIFY(!gsr.cancel());
    QVERIFY(!gsr.waitForFinished());
    QVERIFY(!gsr.waitForProgress());

    // save a new group
    int originalCount = cm->groups().size();
    QContactGroup testGroup;
    testGroup.setName("Test Group");
    testGroup.addMember(cm->contacts().first());
    QList<QContactGroup> saveList;
    saveList << testGroup;
    gsr.setManager(cm);
    QCOMPARE(gsr.manager(), cm);
    QVERIFY(!gsr.isActive());
    QVERIFY(!gsr.isFinished());
    QVERIFY(!gsr.cancel());
    QVERIFY(!gsr.waitForFinished());
    QVERIFY(!gsr.waitForProgress());
    qRegisterMetaType<QContactGroupSaveRequest*>("QContactGroupSaveRequest*");
    QSignalSpy spy(&gsr, SIGNAL(progress(QContactGroupSaveRequest*)));
    gsr.setGroups(saveList);
    QCOMPARE(gsr.groups(), saveList);
    QVERIFY(!gsr.cancel()); // not started
    QVERIFY(gsr.start());
    QVERIFY(gsr.isActive());
    QVERIFY(gsr.status() == QContactAbstractRequest::Active);
    QVERIFY(!gsr.isFinished());
    QVERIFY(!gsr.start());  // already started.
    QVERIFY(gsr.waitForFinished());
    int expectedCount = 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(gsr.isFinished());
    QVERIFY(!gsr.isActive());

    QList<QContactGroup> expected;
    expected << cm->group(cm->groups().last());
    QList<QContactGroup> result = gsr.groups();
    QCOMPARE(expected, result);
    QCOMPARE(cm->groups().size(), originalCount + 1);

    // update a previously saved group
    testGroup = gsr.groups().first();
    testGroup.addMember(cm->contacts().last());
    saveList.clear();
    saveList << testGroup;
    gsr.setGroups(saveList);
    QCOMPARE(gsr.groups(), saveList);
    QVERIFY(!gsr.cancel()); // not started
    QVERIFY(gsr.start());
    QVERIFY(gsr.isActive());
    QVERIFY(gsr.status() == QContactAbstractRequest::Active);
    QVERIFY(!gsr.isFinished());
    QVERIFY(!gsr.start());  // already started.
    QVERIFY(gsr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(gsr.isFinished());
    QVERIFY(!gsr.isActive());

    expected.clear();
    expected << cm->group(cm->groups().last());
    result = gsr.groups();
    QCOMPARE(expected, result);
    QVERIFY(expected.contains(testGroup));
    QCOMPARE(cm->groups().size(), originalCount + 1);

    // cancelling
    testGroup.setName("this shouldn't get saved");
    saveList.clear();
    saveList << testGroup;
    gsr.setGroups(saveList);
    QVERIFY(!gsr.cancel()); // not started
    QVERIFY(gsr.start());
    QVERIFY(gsr.isActive());
    QVERIFY(gsr.status() == QContactAbstractRequest::Active);
    QVERIFY(!gsr.isFinished());
    QVERIFY(gsr.cancel());
    QVERIFY(gsr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(gsr.isActive());    // still cancelling
    QVERIFY(!gsr.isFinished()); // not finished cancelling
    QVERIFY(!gsr.start());      // already started.
    QVERIFY(gsr.waitForFinished());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(gsr.isFinished());
    QVERIFY(!gsr.isActive());
    QVERIFY(gsr.status() == QContactAbstractRequest::Cancelled);

    // verify that the changes were not saved
    expected.clear();
    QList<QUniqueId> allGroups = cm->groups();
    for (int i = 0; i < allGroups.size(); i++) {
        expected.append(cm->group(allGroups.at(i)));
    }
    QVERIFY(!expected.contains(testGroup));
    QCOMPARE(cm->groups().size(), originalCount + 1);

    // restart, and wait for progress after cancel.
    QVERIFY(!gsr.cancel()); // not started
    QVERIFY(gsr.start());
    QVERIFY(gsr.isActive());
    QVERIFY(gsr.status() == QContactAbstractRequest::Active);
    QVERIFY(!gsr.isFinished());
    QVERIFY(gsr.cancel());
    QVERIFY(gsr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(gsr.isActive());    // still cancelling
    QVERIFY(!gsr.isFinished()); // not finished cancelling
    QVERIFY(!gsr.start());      // already started.
    QVERIFY(gsr.waitForProgress());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(gsr.isFinished());
    QVERIFY(!gsr.isActive());
    QVERIFY(gsr.status() == QContactAbstractRequest::Cancelled);

    // verify that the changes were not saved
    expected.clear();
    allGroups = cm->groups();
    for (int i = 0; i < allGroups.size(); i++) {
        expected.append(cm->group(allGroups.at(i)));
    }
    QVERIFY(!expected.contains(testGroup));
    QCOMPARE(cm->groups().size(), originalCount + 1);

    delete cm;
}

void tst_QContactAsync::definitionFetch()
{
    QContactManager* cm = prepareModel();
    QContactDetailDefinitionFetchRequest dfr;
    QVERIFY(dfr.type() == QContactAbstractRequest::DetailDefinitionFetch);

    // initial state - not started, no manager.
    QVERIFY(!dfr.isActive());
    QVERIFY(!dfr.isFinished());
    QVERIFY(!dfr.start());
    QVERIFY(!dfr.cancel());
    QVERIFY(!dfr.waitForFinished());
    QVERIFY(!dfr.waitForProgress());

    // "all definitions" retrieval
    dfr.setManager(cm);
    QCOMPARE(dfr.manager(), cm);
    QVERIFY(!dfr.isActive());
    QVERIFY(!dfr.isFinished());
    QVERIFY(!dfr.cancel());
    QVERIFY(!dfr.waitForFinished());
    QVERIFY(!dfr.waitForProgress());
    qRegisterMetaType<QContactDetailDefinitionFetchRequest*>("QContactDetailDefinitionFetchRequest*");
    QSignalSpy spy(&dfr, SIGNAL(progress(QContactDetailDefinitionFetchRequest*, bool)));
    dfr.setNames(QStringList());
    QVERIFY(!dfr.cancel()); // not started
    QVERIFY(dfr.start());
    QVERIFY(dfr.isActive());
    QVERIFY(dfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!dfr.isFinished());
    QVERIFY(!dfr.start());  // already started.
    QVERIFY(dfr.waitForFinished());
    int expectedCount = 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(dfr.isFinished());
    QVERIFY(!dfr.isActive());

    QMap<QString, QContactDetailDefinition> defs = cm->detailDefinitions();
    QMap<QString, QContactDetailDefinition> result = dfr.definitions();
    QCOMPARE(defs, result);

    // specific definition retrieval
    QStringList specific;
    specific << QContactUrl::DefinitionName;
    dfr.setNames(specific);
    QVERIFY(!dfr.cancel()); // not started
    QVERIFY(dfr.start());
    QVERIFY(dfr.isActive());
    QVERIFY(dfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!dfr.isFinished());
    QVERIFY(!dfr.start());  // already started.
    QVERIFY(dfr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(dfr.isFinished());
    QVERIFY(!dfr.isActive());

    defs.clear();
    defs.insert(QContactUrl::DefinitionName, cm->detailDefinition(QContactUrl::DefinitionName));
    result = dfr.definitions();
    QCOMPARE(defs, result);

    // cancelling
    dfr.setNames(QStringList());
    QVERIFY(!dfr.cancel()); // not started
    QVERIFY(dfr.start());
    QVERIFY(dfr.isActive());
    QVERIFY(dfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!dfr.isFinished());
    QVERIFY(dfr.cancel());
    QVERIFY(dfr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(dfr.isActive());    // still cancelling
    QVERIFY(!dfr.isFinished()); // not finished cancelling
    QVERIFY(!dfr.start());      // already started.
    QVERIFY(dfr.waitForFinished());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(dfr.isFinished());
    QVERIFY(!dfr.isActive());
    QVERIFY(dfr.status() == QContactAbstractRequest::Cancelled);

    // restart, and wait for progress after cancel.
    QVERIFY(!dfr.cancel()); // not started
    QVERIFY(dfr.start());
    QVERIFY(dfr.isActive());
    QVERIFY(dfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!dfr.isFinished());
    QVERIFY(dfr.cancel());
    QVERIFY(dfr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(dfr.isActive());    // still cancelling
    QVERIFY(!dfr.isFinished()); // not finished cancelling
    QVERIFY(!dfr.start());      // already started.
    QVERIFY(dfr.waitForProgress());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(dfr.isFinished());
    QVERIFY(!dfr.isActive());
    QVERIFY(dfr.status() == QContactAbstractRequest::Cancelled);

    delete cm;
}

void tst_QContactAsync::definitionRemove()
{
    QContactManager* cm = prepareModel();
    QContactDetailDefinitionRemoveRequest drr;
    QVERIFY(drr.type() == QContactAbstractRequest::DetailDefinitionRemove);

    // initial state - not started, no manager.
    QVERIFY(!drr.isActive());
    QVERIFY(!drr.isFinished());
    QVERIFY(!drr.start());
    QVERIFY(!drr.cancel());
    QVERIFY(!drr.waitForFinished());
    QVERIFY(!drr.waitForProgress());

    // specific group removal
    int originalCount = cm->detailDefinitions().keys().size();
    QStringList removeIds;
    removeIds << cm->detailDefinitions().keys().first();
    drr.setNames(removeIds);
    drr.setManager(cm);
    QCOMPARE(drr.manager(), cm);
    QVERIFY(!drr.isActive());
    QVERIFY(!drr.isFinished());
    QVERIFY(!drr.cancel());
    QVERIFY(!drr.waitForFinished());
    QVERIFY(!drr.waitForProgress());
    qRegisterMetaType<QContactDetailDefinitionRemoveRequest*>("QContactDetailDefinitionRemoveRequest*");
    QSignalSpy spy(&drr, SIGNAL(progress(QContactDetailDefinitionRemoveRequest*)));
    QVERIFY(drr.names() == removeIds);
    QVERIFY(!drr.cancel()); // not started
    QVERIFY(drr.start());
    QVERIFY(drr.isActive());
    QVERIFY(drr.status() == QContactAbstractRequest::Active);
    QVERIFY(!drr.isFinished());
    QVERIFY(!drr.start());  // already started.
    QVERIFY(drr.waitForFinished());
    int expectedCount = 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(drr.isFinished());
    QVERIFY(!drr.isActive());

    QCOMPARE(cm->detailDefinitions().keys().size(), originalCount - 1);
    cm->detailDefinition(removeIds.first()); // check that it has already been removed.
    QCOMPARE(cm->error(), QContactManager::DoesNotExistError);

    // remove (asynchronously) a nonexistent group - should fail.
    drr.setNames(removeIds);
    QVERIFY(!drr.cancel()); // not started
    QVERIFY(drr.start());
    QVERIFY(drr.isActive());
    QVERIFY(drr.status() == QContactAbstractRequest::Active);
    QVERIFY(!drr.isFinished());
    QVERIFY(!drr.start());  // already started.
    QVERIFY(drr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(drr.isFinished());
    QVERIFY(!drr.isActive());

    QCOMPARE(cm->detailDefinitions().keys().size(), originalCount - 1); // hasn't changed
    QCOMPARE(drr.error(), QContactManager::DoesNotExistError);

    // remove with list containing one valid and one invalid id.
    removeIds << cm->detailDefinitions().keys().first();
    drr.setNames(removeIds);
    QVERIFY(!drr.cancel()); // not started
    QVERIFY(drr.start());
    QVERIFY(drr.isActive());
    QVERIFY(drr.status() == QContactAbstractRequest::Active);
    QVERIFY(!drr.isFinished());
    QVERIFY(!drr.start());  // already started.
    QVERIFY(drr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(drr.isFinished());
    QVERIFY(!drr.isActive());

    QCOMPARE(cm->detailDefinitions().keys().size(), originalCount - 2); // only one more has been removed
    QCOMPARE(drr.errors().first(), QContactManager::DoesNotExistError);
    QCOMPARE(drr.errors().at(1), QContactManager::NoError);

    // remove with empty list - nothing should happen.
    removeIds.clear();
    drr.setNames(removeIds);
    QVERIFY(!drr.cancel()); // not started
    QVERIFY(drr.start());
    QVERIFY(drr.isActive());
    QVERIFY(drr.status() == QContactAbstractRequest::Active);
    QVERIFY(!drr.isFinished());
    QVERIFY(!drr.start());  // already started.
    QVERIFY(drr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(drr.isFinished());
    QVERIFY(!drr.isActive());

    QCOMPARE(cm->detailDefinitions().keys().size(), originalCount - 2); // hasn't changed
    QCOMPARE(drr.error(), QContactManager::NoError);  // no error but no effect.

    // cancelling
    removeIds.clear();
    removeIds << cm->detailDefinitions().keys().first();
    drr.setNames(removeIds);
    QVERIFY(!drr.cancel()); // not started
    QVERIFY(drr.start());
    QVERIFY(drr.isActive());
    QVERIFY(drr.status() == QContactAbstractRequest::Active);
    QVERIFY(!drr.isFinished());
    QVERIFY(drr.cancel());
    QVERIFY(drr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(drr.isActive());    // still cancelling
    QVERIFY(!drr.isFinished()); // not finished cancelling
    QVERIFY(!drr.start());      // already started.
    QVERIFY(drr.waitForFinished());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(drr.isFinished());
    QVERIFY(!drr.isActive());
    QVERIFY(drr.status() == QContactAbstractRequest::Cancelled);

    QCOMPARE(cm->detailDefinitions().keys().size(), originalCount - 2); // hasn't changed

    // restart, and wait for progress after cancel.
    QVERIFY(!drr.cancel()); // not started
    QVERIFY(drr.start());
    QVERIFY(drr.isActive());
    QVERIFY(drr.status() == QContactAbstractRequest::Active);
    QVERIFY(!drr.isFinished());
    QVERIFY(drr.cancel());
    QVERIFY(drr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(drr.isActive());    // still cancelling
    QVERIFY(!drr.isFinished()); // not finished cancelling
    QVERIFY(!drr.start());      // already started.
    QVERIFY(drr.waitForProgress());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(drr.isFinished());
    QVERIFY(!drr.isActive());
    QVERIFY(drr.status() == QContactAbstractRequest::Cancelled);

    QCOMPARE(cm->detailDefinitions().keys().size(), originalCount - 2); // hasn't changed

    delete cm;
}

void tst_QContactAsync::definitionSave()
{
    QContactManager* cm = prepareModel();
    QContactDetailDefinitionSaveRequest dsr;
    QVERIFY(dsr.type() == QContactAbstractRequest::DetailDefinitionSave);

    // initial state - not started, no manager.
    QVERIFY(!dsr.isActive());
    QVERIFY(!dsr.isFinished());
    QVERIFY(!dsr.start());
    QVERIFY(!dsr.cancel());
    QVERIFY(!dsr.waitForFinished());
    QVERIFY(!dsr.waitForProgress());

    // save a new group
    int originalCount = cm->detailDefinitions().keys().size();
    QContactDetailDefinition testDef;
    testDef.setName("TestDefinitionId");
    QMap<QString, QContactDetailDefinition::Field> fields;
    QContactDetailDefinition::Field f;
    f.dataType = QVariant::String;
    fields.insert("TestDefinitionField", f);
    testDef.setFields(fields);
    QList<QContactDetailDefinition> saveList;
    saveList << testDef;
    dsr.setManager(cm);
    QCOMPARE(dsr.manager(), cm);
    QVERIFY(!dsr.isActive());
    QVERIFY(!dsr.isFinished());
    QVERIFY(!dsr.cancel());
    QVERIFY(!dsr.waitForFinished());
    QVERIFY(!dsr.waitForProgress());
    qRegisterMetaType<QContactDetailDefinitionSaveRequest*>("QContactDetailDefinitionSaveRequest*");
    QSignalSpy spy(&dsr, SIGNAL(progress(QContactDetailDefinitionSaveRequest*)));
    dsr.setDefinitions(saveList);
    QCOMPARE(dsr.definitions(), saveList);
    QVERIFY(!dsr.cancel()); // not started
    QVERIFY(dsr.start());
    QVERIFY(dsr.isActive());
    QVERIFY(dsr.status() == QContactAbstractRequest::Active);
    QVERIFY(!dsr.isFinished());
    QVERIFY(!dsr.start());  // already started.
    QVERIFY(dsr.waitForFinished());
    int expectedCount = 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(dsr.isFinished());
    QVERIFY(!dsr.isActive());

    QList<QContactDetailDefinition> expected;
    expected << cm->detailDefinition("TestDefinitionId");
    QList<QContactDetailDefinition> result = dsr.definitions();
    QCOMPARE(expected, result);
    QVERIFY(expected.contains(testDef));
    QCOMPARE(cm->detailDefinitions().values().size(), originalCount + 1);

    // update a previously saved group
    fields.insert("TestDefinitionFieldTwo", f);
    testDef.setFields(fields);
    saveList.clear();
    saveList << testDef;
    dsr.setDefinitions(saveList);
    QCOMPARE(dsr.definitions(), saveList);
    QVERIFY(!dsr.cancel()); // not started
    QVERIFY(dsr.start());
    QVERIFY(dsr.isActive());
    QVERIFY(dsr.status() == QContactAbstractRequest::Active);
    QVERIFY(!dsr.isFinished());
    QVERIFY(!dsr.start());  // already started.
    QVERIFY(dsr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(dsr.isFinished());
    QVERIFY(!dsr.isActive());

    expected.clear();
    expected << cm->detailDefinition("TestDefinitionId");
    result = dsr.definitions();
    QCOMPARE(expected, result);
    QVERIFY(expected.contains(testDef));
    QCOMPARE(cm->detailDefinitions().values().size(), originalCount + 1);

    // cancelling
    fields.insert("TestDefinitionFieldThree - shouldn't get saved", f);
    testDef.setFields(fields);
    saveList.clear();
    saveList << testDef;
    dsr.setDefinitions(saveList);
    QCOMPARE(dsr.definitions(), saveList);
    QVERIFY(!dsr.cancel()); // not started
    QVERIFY(dsr.start());
    QVERIFY(dsr.isActive());
    QVERIFY(dsr.status() == QContactAbstractRequest::Active);
    QVERIFY(!dsr.isFinished());
    QVERIFY(dsr.cancel());
    QVERIFY(dsr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(dsr.isActive());    // still cancelling
    QVERIFY(!dsr.isFinished()); // not finished cancelling
    QVERIFY(!dsr.start());      // already started.
    QVERIFY(dsr.waitForFinished());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(dsr.isFinished());
    QVERIFY(!dsr.isActive());
    QVERIFY(dsr.status() == QContactAbstractRequest::Cancelled);

    // verify that the changes were not saved
    expected.clear();
    QList<QContactDetailDefinition> allDefs = cm->detailDefinitions().values();
    QVERIFY(!expected.contains(testDef));
    QCOMPARE(cm->detailDefinitions().values().size(), originalCount + 1);

    // restart, and wait for progress after cancel.
    QVERIFY(!dsr.cancel()); // not started
    QVERIFY(dsr.start());
    QVERIFY(dsr.isActive());
    QVERIFY(dsr.status() == QContactAbstractRequest::Active);
    QVERIFY(!dsr.isFinished());
    QVERIFY(dsr.cancel());
    QVERIFY(dsr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(dsr.isActive());    // still cancelling
    QVERIFY(!dsr.isFinished()); // not finished cancelling
    QVERIFY(!dsr.start());      // already started.
    QVERIFY(dsr.waitForProgress());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(dsr.isFinished());
    QVERIFY(!dsr.isActive());
    QVERIFY(dsr.status() == QContactAbstractRequest::Cancelled);

    // verify that the changes were not saved
    expected.clear();
    allDefs = cm->detailDefinitions().values();
    QVERIFY(!expected.contains(testDef));
    QCOMPARE(cm->detailDefinitions().values().size(), originalCount + 1);

    delete cm;
}

QContactManager* tst_QContactAsync::prepareModel()
{
    QContactManager* cm = new QContactManager("memory");

    QContact a, b, c;
    a.setDisplayLabel("Aaron Aaronson");
    b.setDisplayLabel("Bob Aaronsen");
    c.setDisplayLabel("Borris Aaronsun");

    QContactPhoneNumber phn;
    phn.setNumber("0123");
    c.saveDetail(&phn);
    phn.setNumber("3456");
    b.saveDetail(&phn);
    phn.setNumber("6789");
    a.saveDetail(&phn);

    QContactUrl url;
    url.setUrl("http://test.nokia.com");
    a.saveDetail(&url);

    cm->saveContact(&a);
    cm->saveContact(&b);
    cm->saveContact(&c);

    QContactGroup ga, gb, gc;
    ga.setName("Group One");
    gb.setName("Group Two");
    gc.setName("Group Three");

    ga.addMember(a.id());
    gb.addMember(b.id());
    gb.addMember(c.id());
    gc.addMember(a.id());
    gc.addMember(c.id());

    cm->saveGroup(&ga);
    cm->saveGroup(&gb);
    cm->saveGroup(&gc);

    return cm;
}

void tst_QContactAsync::maliciousManager()
{
    /* Add a path to our plugin path */
    QString path = QApplication::applicationDirPath() + "/maliciousplugin";
    QApplication::addLibraryPath(path);
    QApplication::addLibraryPath("/"); // strictly to test a cdUp :/

    // use the invalid manager: passes all requests through to base class
    QContactManager cm("invalid");
    QContactFilter fil; // matches everything
    QContactFetchRequest cfr;
    cfr.setFilter(fil);
    cfr.setManager(&cm);
    QVERIFY(!cfr.start());
    QVERIFY(!cfr.cancel());
    QVERIFY(!cfr.waitForFinished());
    QVERIFY(!cfr.start());
    QVERIFY(!cfr.waitForProgress());

    // now use a malicious manager that deliberately calls
    // incorrect "updateRequest" functions in base class:
    QContactManager mcm("maliciousplugin");
    cfr.setFilter(fil);
    cfr.setManager(&mcm);
    QVERIFY(cfr.start());
    QVERIFY(cfr.cancel());
    QVERIFY(!cfr.waitForFinished(100));
    QVERIFY(cfr.start());
    QVERIFY(!cfr.waitForProgress(100));
    QVERIFY(cfr.cancel());
}

QTEST_MAIN(tst_QContactAsync)
#include "tst_qcontactasync.moc"
