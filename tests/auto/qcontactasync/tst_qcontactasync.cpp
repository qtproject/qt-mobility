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

    // "all contacts" retrieval
    QContactFilter fil;
    QContactFetchRequest cfr;
    cfr.setManager(cm);
    qRegisterMetaType<QContactFetchRequest*>("QContactFetchRequest*");
    QSignalSpy spy(&cfr, SIGNAL(progress(QContactFetchRequest*, bool)));
    cfr.setFilter(fil);
    QCOMPARE(cfr.filter(), fil);
    QVERIFY(cfr.start());
    QVERIFY(cfr.waitForFinished());
    int expectedCount = 2;
    QCOMPARE(spy.count(), expectedCount); // pending + finished progress signals.

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
    QVERIFY(cfr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // pending + finished progress signals.

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
    QVERIFY(cfr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // pending + finished progress signals.

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
    QVERIFY(cfr.waitForFinished());
    expectedCount += 2;
    QCOMPARE(spy.count(), expectedCount); // pending + finished progress signals.

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

    delete cm;
}

void tst_QContactAsync::contactIdFetch()
{
}

void tst_QContactAsync::contactRemove()
{
}

void tst_QContactAsync::contactSave()
{
}


void tst_QContactAsync::groupFetch()
{
}

void tst_QContactAsync::groupRemove()
{
}

void tst_QContactAsync::groupSave()
{
}

void tst_QContactAsync::definitionFetch()
{
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
