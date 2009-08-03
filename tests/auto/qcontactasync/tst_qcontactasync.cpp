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

private:
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

void tst_QContactAsync::contactFetch()
{
    QContactManager* cm = prepareModel();
    QContactFetchRequest cfr;

    // initial state
    QVERIFY(!cfr.isActive());   // not started
    QVERIFY(!cfr.isFinished()); // not started

    // "all contacts" retrieval
    QContactFilter fil;
    cfr.setManager(cm);
    qRegisterMetaType<QContactFetchRequest*>("QContactFetchRequest*");
    QSignalSpy spy(&cfr, SIGNAL(progress(QContactFetchRequest*, bool)));
    cfr.setFilter(fil);
    QCOMPARE(cfr.filter(), fil);
    QVERIFY(cfr.start());
    QVERIFY(cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!cfr.isFinished());
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
    QVERIFY(cfr.start());
    QVERIFY(cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!cfr.isFinished());
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
    QVERIFY(cfr.start());
    QVERIFY(cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!cfr.isFinished());
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
    QVERIFY(cfr.start());
    QVERIFY(cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!cfr.isFinished());
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
    QVERIFY(cfr.start());
    QVERIFY(cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!cfr.isFinished());
    QVERIFY(cfr.cancel());
    QVERIFY(cfr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(cfr.isActive());    // still cancelling
    QVERIFY(!cfr.isFinished()); // not finished cancelling
    QVERIFY(cfr.waitForFinished());
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

    // initial state
    QVERIFY(!cfr.isActive());   // not started
    QVERIFY(!cfr.isFinished()); // not started

    // "all contacts" retrieval
    QContactFilter fil;
    cfr.setManager(cm);
    qRegisterMetaType<QContactIdFetchRequest*>("QContactIdFetchRequest*");
    QSignalSpy spy(&cfr, SIGNAL(progress(QContactIdFetchRequest*, bool)));
    cfr.setFilter(fil);
    QCOMPARE(cfr.filter(), fil);
    QVERIFY(cfr.start());
    QVERIFY(cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!cfr.isFinished());
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
    QVERIFY(cfr.start());
    QVERIFY(cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!cfr.isFinished());
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
    QVERIFY(cfr.start());
    QVERIFY(cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!cfr.isFinished());
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
    QVERIFY(cfr.start());
    QVERIFY(cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!cfr.isFinished());
    QVERIFY(cfr.cancel());
    QVERIFY(cfr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(cfr.isActive());    // still cancelling
    QVERIFY(!cfr.isFinished()); // not finished cancelling
    QVERIFY(cfr.waitForFinished());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(cfr.isFinished());
    QVERIFY(!cfr.isActive());
    QVERIFY(cfr.status() == QContactAbstractRequest::Cancelled);

    delete cm;
}

void tst_QContactAsync::contactRemove()
{
}

void tst_QContactAsync::contactSave()
{
    QContactManager* cm = prepareModel();
    QContactSaveRequest csr;

    // initial state
    QVERIFY(!csr.isActive());   // not started
    QVERIFY(!csr.isFinished()); // not started

    // save a new contact
    int originalCount = cm->contacts().size();
    QContact testContact;
    testContact.setDisplayLabel("Test Contact");
    QList<QContact> saveList;
    saveList << testContact;
    csr.setManager(cm);
    qRegisterMetaType<QContactSaveRequest*>("QContactSaveRequest*");
    QSignalSpy spy(&csr, SIGNAL(progress(QContactSaveRequest*)));
    csr.setContacts(saveList);
    QCOMPARE(csr.contacts(), saveList);
    QVERIFY(csr.start());
    QVERIFY(csr.isActive());
    QVERIFY(csr.status() == QContactAbstractRequest::Active);
    QVERIFY(!csr.isFinished());
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
    QVERIFY(csr.start());
    QVERIFY(csr.isActive());
    QVERIFY(csr.status() == QContactAbstractRequest::Active);
    QVERIFY(!csr.isFinished());
    QVERIFY(csr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // active + finished progress signals.
    QVERIFY(csr.isFinished());
    QVERIFY(!csr.isActive());

    expected.clear();
    expected << cm->contact(cm->contacts().last());
    result = csr.contacts();
    QCOMPARE(expected, result);
    QVERIFY(expected.contains(testContact));
    QCOMPARE(cm->contacts().size(), originalCount + 1);

    // cancelling
    QContact temp = testContact;
    QContactUrl url;
    url.setUrl("should not get saved");
    temp.saveDetail(&url);
    saveList.clear();
    saveList << temp;
    csr.setContacts(saveList);
    QVERIFY(csr.start());
    QVERIFY(csr.isActive());
    QVERIFY(csr.status() == QContactAbstractRequest::Active);
    QVERIFY(!csr.isFinished());
    QVERIFY(csr.cancel());
    QVERIFY(csr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(csr.isActive());    // still cancelling
    QVERIFY(!csr.isFinished()); // not finished cancelling
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

    delete cm;
}

void tst_QContactAsync::groupFetch()
{
    QContactManager* cm = prepareModel();
    QContactGroupFetchRequest gfr;

    // initial state
    QVERIFY(!gfr.isActive());   // not started
    QVERIFY(!gfr.isFinished()); // not started

    // "all groups" retrieval
    gfr.setManager(cm);
    qRegisterMetaType<QContactGroupFetchRequest*>("QContactGroupFetchRequest*");
    QSignalSpy spy(&gfr, SIGNAL(progress(QContactGroupFetchRequest*, bool)));
    gfr.setIds(QList<QUniqueId>());
    QVERIFY(gfr.start());
    QVERIFY(gfr.isActive());
    QVERIFY(gfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!gfr.isFinished());
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
    gfr.setManager(cm);
    gfr.setIds(groupIds);
    QCOMPARE(gfr.ids(), groupIds);
    QVERIFY(gfr.start());
    QVERIFY(gfr.isActive());
    QVERIFY(gfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!gfr.isFinished());
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
    QVERIFY(gfr.start());
    QVERIFY(gfr.isActive());
    QVERIFY(gfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!gfr.isFinished());
    QVERIFY(gfr.cancel());
    QVERIFY(gfr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(gfr.isActive());    // still cancelling
    QVERIFY(!gfr.isFinished()); // not finished cancelling
    QVERIFY(gfr.waitForFinished());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(gfr.isFinished());
    QVERIFY(!gfr.isActive());
    QVERIFY(gfr.status() == QContactAbstractRequest::Cancelled);

    delete cm;
}

void tst_QContactAsync::groupRemove()
{
}

void tst_QContactAsync::groupSave()
{
    QContactManager* cm = prepareModel();
    QContactGroupSaveRequest gsr;

    // initial state
    QVERIFY(!gsr.isActive());   // not started
    QVERIFY(!gsr.isFinished()); // not started

    // save a new group
    int originalCount = cm->groups().size();
    QContactGroup testGroup;
    testGroup.setName("Test Group");
    testGroup.addMember(cm->contacts().first());
    QList<QContactGroup> saveList;
    saveList << testGroup;
    gsr.setManager(cm);
    qRegisterMetaType<QContactGroupSaveRequest*>("QContactGroupSaveRequest*");
    QSignalSpy spy(&gsr, SIGNAL(progress(QContactGroupSaveRequest*)));
    gsr.setGroups(saveList);
    QCOMPARE(gsr.groups(), saveList);
    QVERIFY(gsr.start());
    QVERIFY(gsr.isActive());
    QVERIFY(gsr.status() == QContactAbstractRequest::Active);
    QVERIFY(!gsr.isFinished());
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
    QVERIFY(gsr.start());
    QVERIFY(gsr.isActive());
    QVERIFY(gsr.status() == QContactAbstractRequest::Active);
    QVERIFY(!gsr.isFinished());
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
    QVERIFY(gsr.start());
    QVERIFY(gsr.isActive());
    QVERIFY(gsr.status() == QContactAbstractRequest::Active);
    QVERIFY(!gsr.isFinished());
    QVERIFY(gsr.cancel());
    QVERIFY(gsr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(gsr.isActive());    // still cancelling
    QVERIFY(!gsr.isFinished()); // not finished cancelling
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

    delete cm;
}

void tst_QContactAsync::definitionFetch()
{
    QContactManager* cm = prepareModel();
    QContactDetailDefinitionFetchRequest dfr;

    // initial state
    QVERIFY(!dfr.isActive());   // not started
    QVERIFY(!dfr.isFinished()); // not started

    // "all definitions" retrieval
    dfr.setManager(cm);
    qRegisterMetaType<QContactDetailDefinitionFetchRequest*>("QContactDetailDefinitionFetchRequest*");
    QSignalSpy spy(&dfr, SIGNAL(progress(QContactDetailDefinitionFetchRequest*, bool)));
    dfr.setNames(QStringList());
    QVERIFY(dfr.start());
    QVERIFY(dfr.isActive());
    QVERIFY(dfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!dfr.isFinished());
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
    QVERIFY(dfr.start());
    QVERIFY(dfr.isActive());
    QVERIFY(dfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!dfr.isFinished());
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
    QVERIFY(dfr.start());
    QVERIFY(dfr.isActive());
    QVERIFY(dfr.status() == QContactAbstractRequest::Active);
    QVERIFY(!dfr.isFinished());
    QVERIFY(dfr.cancel());
    QVERIFY(dfr.status() == QContactAbstractRequest::Cancelling);
    QVERIFY(dfr.isActive());    // still cancelling
    QVERIFY(!dfr.isFinished()); // not finished cancelling
    QVERIFY(dfr.waitForFinished());
    expectedCount += 3;
    QCOMPARE(spy.count(), expectedCount); // active + cancelling + cancelled progress signals.
    QVERIFY(dfr.isFinished());
    QVERIFY(!dfr.isActive());
    QVERIFY(dfr.status() == QContactAbstractRequest::Cancelled);

    delete cm;
}

void tst_QContactAsync::definitionRemove()
{
}

void tst_QContactAsync::definitionSave()
{
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


QTEST_MAIN(tst_QContactAsync)
#include "tst_qcontactasync.moc"
