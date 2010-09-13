/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtTest/QtTest>

#include "qtorganizer.h"
#include "qorganizeritemid.h"
#include <QSet>

//TESTED_COMPONENT=src/organizer
//TESTED_CLASS=
//TESTED_FILES=

QTM_USE_NAMESPACE
class tst_QOrganizerItem: public QObject
{
Q_OBJECT

public:
    tst_QOrganizerItem();
    virtual ~tst_QOrganizerItem();

private slots:
    void details();
    void displayLabel();
    void description();
    void type();
    void emptiness();
    void idLessThan();
    void idHash();
    void hash();
    void datastream();
    void traits();
    void idTraits();
    void localIdTraits();
};

tst_QOrganizerItem::tst_QOrganizerItem()
{
}

tst_QOrganizerItem::~tst_QOrganizerItem()
{
}

void tst_QOrganizerItem::details()
{
    // Check that detail keys are unique, regardless of order of initialisation
    // First, construct the detail first, then the contact
    QOrganizerItemComment note;
    note.setComment("Example Note");
    QOrganizerItem keyTest;
    QVERIFY(keyTest.saveDetail(&note));
    QList<QOrganizerItemDetail> allDetails = keyTest.details();
    QList<int> detailKeys;
    foreach (const QOrganizerItemDetail& det, allDetails) {
        int currKey = det.key();
        QVERIFY(!detailKeys.contains(currKey));
        detailKeys.append(currKey);
    }
    // Second, construct the detail after the contact has been constructed
    QOrganizerItemLocation adr;
    adr.setAddress("this is a test address");
    QVERIFY(keyTest.saveDetail(&adr));
    allDetails = keyTest.details();
    detailKeys.clear();
    foreach (const QOrganizerItemDetail& det, allDetails) {
        int currKey = det.key();
        QVERIFY(!detailKeys.contains(currKey));
        detailKeys.append(currKey);
    }

    // now test for default construction sanity
    QOrganizerItem oi;

    // Test there are no details (apart from type) by default
    QVERIFY(oi.isEmpty() == true);
    QCOMPARE(oi.details().count(), 1);
    QCOMPARE(oi.details(QOrganizerItemLocation::DefinitionName).count(), 0);
    QCOMPARE(oi.details<QOrganizerItemLocation>().count(), 0);
    QVERIFY(oi.detail(QOrganizerItemLocation::DefinitionName).isEmpty());
    QVERIFY(oi.detail<QOrganizerItemLocation>().isEmpty());
    QCOMPARE(oi.details(QOrganizerItemLocation::DefinitionName.latin1()).count(), 0);
    QVERIFY(oi.detail(QOrganizerItemLocation::DefinitionName.latin1()).isEmpty());
    QCOMPARE(oi.details(QLatin1String(QOrganizerItemLocation::DefinitionName)).count(), 0);
    QVERIFY(oi.detail(QLatin1String(QOrganizerItemLocation::DefinitionName)).isEmpty());

    // Test retrieving the default details (type)
    QList<QOrganizerItemDetail> details = oi.details(QString());
    QVERIFY(details.at(0).definitionName() == QOrganizerItemType::DefinitionName);

    QOrganizerItemDetail detail = oi.detail("nonexistent");
    QVERIFY(detail.isEmpty());
    QVERIFY(detail.definitionName().isEmpty());

    // retrieve the first detail using the empty definition name accessor.
    detail = oi.detail(QString());
    QVERIFY(detail == details.at(0));

    QVERIFY(oi.details("nonexistent").count() == 0);

    // Add a detail
    QOrganizerItemLocation a;
    a.setAddress("another test address label");
    QVERIFY(oi.saveDetail(&a));
    QVERIFY(oi.isEmpty() == false);

    QVERIFY(oi.details().count() == 2);

    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName).count() == 1);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName, QOrganizerItemLocation::FieldAddress, QString("another test address label")).count() == 1);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName, QString(), QString("another test address label")).count() == oi.details(QOrganizerItemLocation::DefinitionName).count());

    QVERIFY(oi.details(QLatin1String(QOrganizerItemLocation::DefinitionName)).count() == 1);
    QVERIFY(oi.details(QLatin1String(QOrganizerItemLocation::DefinitionName), QOrganizerItemLocation::FieldAddress, QString("another test address label")).count() == 1);
    QVERIFY(oi.details(QLatin1String(QOrganizerItemLocation::DefinitionName), QString(), QString("another test address label")).count() == oi.details(QOrganizerItemLocation::DefinitionName).count());

    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName.latin1()).count() == 1);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName.latin1(), QOrganizerItemLocation::FieldAddress, QString("another test address label")).count() == 1);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName.latin1(), QString(), QString("another test address label")).count() == oi.details(QOrganizerItemLocation::DefinitionName).count());

    QVERIFY(oi.details<QOrganizerItemLocation>(QOrganizerItemLocation::FieldAddress, QString("another test address label")).count() == 1);
    QVERIFY(oi.details<QOrganizerItemLocation>(QOrganizerItemLocation::FieldAddress.latin1(), QString("another test address label")).count() == 1);
    QVERIFY(oi.details<QOrganizerItemLocation>(QLatin1String(QOrganizerItemLocation::FieldAddress), QString("another test address label")).count() == 1);

    QVERIFY(oi.details<QOrganizerItemLocation>().count() == 1);
    QVERIFY(!oi.detail(QOrganizerItemLocation::DefinitionName).isEmpty());
    QVERIFY(!oi.detail<QOrganizerItemLocation>().isEmpty());
    QCOMPARE(oi.detail<QOrganizerItemLocation>(), a);

    // Remove detail
    QVERIFY(oi.removeDetail(&a));
    QCOMPARE(oi.details().count(), 1);
    QVERIFY(oi.isEmpty() == true);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName).count() == 0);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName, QOrganizerItemLocation::FieldAddress, QString("another test address label")).count() == 0);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName, QString(), QString("another test address label")).count() == oi.details(QOrganizerItemLocation::DefinitionName).count());
    QCOMPARE(oi.details<QOrganizerItemLocation>().count(), 0);
    QVERIFY(oi.detail(QOrganizerItemLocation::DefinitionName).isEmpty());
    QVERIFY(oi.detail<QOrganizerItemLocation>().isEmpty());

    // Try removing it again
    QVERIFY(!oi.removeDetail(&a));

    // Add again, and remove a different way (retrieved copy)
    QVERIFY(oi.saveDetail(&a));
    QVERIFY(oi.isEmpty() == false);
    QCOMPARE(oi.details().count(), 2);
    QOrganizerItemLocation a2 = oi.detail(QOrganizerItemLocation::DefinitionName);
    QCOMPARE(a, a2);

    QVERIFY(oi.removeDetail(&a2));
    QCOMPARE(oi.details().count(), 1);
    QVERIFY(oi.isEmpty() == true);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName).count() == 0);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName, QOrganizerItemLocation::FieldAddress, QString("another test address label")).count() == 0);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName, QString(), QString("another test address label")).count() == oi.details(QOrganizerItemLocation::DefinitionName).count());
    QVERIFY(oi.details<QOrganizerItemLocation>().count() == 0);
    QVERIFY(oi.detail(QOrganizerItemLocation::DefinitionName).isEmpty());
    QVERIFY(oi.detail<QOrganizerItemLocation>().isEmpty());

    QCOMPARE(a, a2);

    // Add again again, and remove a different way (base class)
    QVERIFY(oi.saveDetail(&a));
    QCOMPARE(oi.details().count(), 2);
    QOrganizerItemDetail a3 = oi.detail(QOrganizerItemLocation::DefinitionName);
    QVERIFY(a == a3);

    QVERIFY(oi.removeDetail(&a3));
    QCOMPARE(oi.details().count(), 1);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName).count() == 0);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName, QOrganizerItemLocation::FieldAddress, QString("another test address label")).count() == 0);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName, QString(), QString("another test address label")).count() == oi.details(QOrganizerItemLocation::DefinitionName).count());
    QVERIFY(oi.details<QOrganizerItemLocation>().count() == 0);
    QVERIFY(oi.detail(QOrganizerItemLocation::DefinitionName).isEmpty());
    QVERIFY(oi.detail<QOrganizerItemLocation>().isEmpty());

    QVERIFY(a == a3);

    // now we want to add multiple details of the same type, and test that retrieval works correctly.
    a2 = QOrganizerItemLocation();
    a2.setAddress("22222");
    a2.setValue("nonexistent-field", QVariant("22222-2"));
    oi.saveDetail(&a);
    oi.saveDetail(&a2);
    QCOMPARE(oi.details().count(), 3);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName).count() == 2);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName, QOrganizerItemLocation::FieldAddress, QString("another test address label")).count() == 1);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName, QString(), QString("another test address label")).count() == oi.details(QOrganizerItemLocation::DefinitionName).count());
    QVERIFY(oi.details<QOrganizerItemLocation>().count() == 2);
    QVERIFY(!oi.detail(QOrganizerItemLocation::DefinitionName).isEmpty());
    QVERIFY(!oi.detail<QOrganizerItemLocation>().isEmpty());
    QCOMPARE(oi.detail<QOrganizerItemLocation>(), a);
    QVERIFY(oi.removeDetail(&a2));

    // Now try adding a detail to multiple contacts

    QOrganizerItem oi2;
    QVERIFY(oi2.isEmpty() == true);
    QVERIFY(oi.saveDetail(&a));
    QVERIFY(oi2.saveDetail(&a));
    QVERIFY(oi2.isEmpty() == false);

    QCOMPARE(oi.details().count(), 2);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName).count() == 1);
    QVERIFY(oi.details<QOrganizerItemLocation>().count() == 1);
    QVERIFY(!oi.detail(QOrganizerItemLocation::DefinitionName).isEmpty());
    QVERIFY(!oi.detail<QOrganizerItemLocation>().isEmpty());
    QCOMPARE(oi.detail<QOrganizerItemLocation>(), a);

    QCOMPARE(oi2.details().count(), 2);
    QVERIFY(oi2.details(QOrganizerItemLocation::DefinitionName).count() == 1);
    QVERIFY(oi2.details<QOrganizerItemLocation>().count() == 1);
    QVERIFY(!oi2.detail(QOrganizerItemLocation::DefinitionName).isEmpty());
    QVERIFY(!oi2.detail<QOrganizerItemLocation>().isEmpty());
    QCOMPARE(oi2.detail<QOrganizerItemLocation>(), a);

    // Now try removing it from one
    QVERIFY(oi.removeDetail(&a));

    // Make sure it's gone from the first contact
    QVERIFY(oi.isEmpty() == true);
    QCOMPARE(oi.details().count(), 1);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName).count() == 0);
    QVERIFY(oi.details<QOrganizerItemLocation>().count() == 0);
    QVERIFY(oi.detail(QOrganizerItemLocation::DefinitionName).isEmpty());
    QVERIFY(oi.detail<QOrganizerItemLocation>().isEmpty());

    // but not the second
    QVERIFY(oi2.isEmpty() == false);
    QCOMPARE(oi2.details().count(), 2);
    QVERIFY(oi2.details(QOrganizerItemLocation::DefinitionName).count() == 1);
    QVERIFY(oi2.details<QOrganizerItemLocation>().count() == 1);
    QVERIFY(!oi2.detail(QOrganizerItemLocation::DefinitionName).isEmpty());
    QVERIFY(!oi2.detail<QOrganizerItemLocation>().isEmpty());
    QCOMPARE(oi2.detail<QOrganizerItemLocation>(), a);

    // Now remove it from the second as well
    QVERIFY(oi2.removeDetail(&a));

    // Make sure it's gone from both
    QCOMPARE(oi.details().count(), 1);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName).count() == 0);
    QVERIFY(oi.details<QOrganizerItemLocation>().count() == 0);
    QVERIFY(oi.detail(QOrganizerItemLocation::DefinitionName).isEmpty());
    QVERIFY(oi.detail<QOrganizerItemLocation>().isEmpty());

    QCOMPARE(oi2.details().count(), 1);
    QVERIFY(oi2.details(QOrganizerItemLocation::DefinitionName).count() == 0);
    QVERIFY(oi2.details<QOrganizerItemLocation>().count() == 0);
    QVERIFY(oi2.detail(QOrganizerItemLocation::DefinitionName).isEmpty());
    QVERIFY(oi2.detail<QOrganizerItemLocation>().isEmpty());

    // add a, add b, remove a, add a, remove b, remove a
    QVERIFY(oi.saveDetail(&a));
    QVERIFY(oi2.saveDetail(&a));
    QVERIFY(oi.removeDetail(&a));
    QVERIFY(oi.saveDetail(&a));
    QVERIFY(oi2.removeDetail(&a));
    QVERIFY(oi.removeDetail(&a));

    // Now add a detail with the same values twice
    QOrganizerItemLocation one;
    QOrganizerItemLocation two;

    one.setAddress("12345");
    two.setAddress("12345");

    // add it once
    QVERIFY(oi.saveDetail(&one));
    QCOMPARE(oi.details().count(), 2);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName).count() == 1);
    QVERIFY(oi.details<QOrganizerItemLocation>().count() == 1);
    QVERIFY(!oi.detail(QOrganizerItemLocation::DefinitionName).isEmpty());
    QVERIFY(!oi.detail<QOrganizerItemLocation>().isEmpty());
    QCOMPARE(oi.detail<QOrganizerItemLocation>(), one);

    // add it twice
    QVERIFY(oi.saveDetail(&two));
    QCOMPARE(oi.details().count(), 3);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName).count() == 2);
    QVERIFY(oi.details<QOrganizerItemLocation>().count() == 2);
    QVERIFY(!oi.detail(QOrganizerItemLocation::DefinitionName).isEmpty());
    QVERIFY(!oi.detail<QOrganizerItemLocation>().isEmpty());
    QCOMPARE(oi.detail<QOrganizerItemLocation>(), one);
    QCOMPARE(oi.details<QOrganizerItemLocation>()[0], one);
    QCOMPARE(oi.details<QOrganizerItemLocation>()[1], two);

    // Remove it once
    QVERIFY(oi.removeDetail(&one));
    QCOMPARE(oi.details().count(), 2);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName).count() == 1);
    QVERIFY(oi.details<QOrganizerItemLocation>().count() == 1);
    QVERIFY(!oi.detail(QOrganizerItemLocation::DefinitionName).isEmpty());
    QVERIFY(!oi.detail<QOrganizerItemLocation>().isEmpty());
    QCOMPARE(oi.detail<QOrganizerItemLocation>(), two);

    // Remove it twice
    QVERIFY(oi.removeDetail(&two));
    QCOMPARE(oi.details().count(), 1);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName).count() == 0);
    QVERIFY(oi.details<QOrganizerItemLocation>().count() == 0);
    QVERIFY(oi.detail(QOrganizerItemLocation::DefinitionName).isEmpty());
    QVERIFY(oi.detail<QOrganizerItemLocation>().isEmpty());

    // Null pointer tests
    QVERIFY(oi.saveDetail(0) == false);
    QVERIFY(oi.removeDetail(0) == false);

    // Reference tests...
    QOrganizerItemDetail& ref = one;
    QVERIFY(oi.saveDetail(&one));
    QVERIFY(ref == one);
    one.setAddress("56678");
    QVERIFY(oi.saveDetail(&one));
    QVERIFY(ref == one);

    // Retrieve the detail again and modify it
    QOrganizerItemLocation three = oi.detail<QOrganizerItemLocation>();
    QVERIFY(ref == three);
    QVERIFY(one == three);
    three.setAddress("542343");
    QVERIFY(oi.saveDetail(&three));

    // Now see if we got any updates to ref/one
    QVERIFY(ref == one);
    QVERIFY(ref != three);

    // test saving of a detail with an empty field.
    QOrganizerItemLocation four;
    four.setAddress("");
    oi.saveDetail(&four);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName).count() == 2);
    QVERIFY(!four.variantValues().isEmpty()); // an empty qstring is not invalid; make sure it exists in the detail.

    // ensure that clearing a contact's details works correctly
    QOrganizerItemPriority priorityDetail;
    priorityDetail.setPriority(QOrganizerItemPriority::VeryHighPriority);
    oi.saveDetail(&priorityDetail);
    QCOMPARE(oi.detail(QOrganizerItemPriority::DefinitionName).value(QOrganizerItemPriority::FieldPriority).toInt(), static_cast<int>(QOrganizerItemPriority::VeryHighPriority));
    QVERIFY(oi.details().size() > 0);
    QVERIFY(!oi.isEmpty());
    QOrganizerItemId oldId = oi.id();
    oi.clearDetails();
    QCOMPARE(oi.details().size(), 1); // always has an item type.
    QCOMPARE(oi.detail(QOrganizerItemPriority::DefinitionName).value(QOrganizerItemPriority::FieldPriority), QString());
    QVERIFY(oi.isEmpty());
    QCOMPARE(oi.id(), oldId); // id shouldn't change.
}

void tst_QOrganizerItem::displayLabel()
{
    QOrganizerItem oi;

    QString label = oi.displayLabel();
    QVERIFY(label.isEmpty());
    oi.setDisplayLabel("Test Event Display Label");
    QVERIFY(oi.displayLabel() == QString("Test Event Display Label"));

    /* Remove the detail via removeDetail */
    QOrganizerItemDisplayLabel old;
    QCOMPARE(oi.details().count(), 2); // type + label
    QVERIFY(!oi.removeDetail(&old)); // should fail.
    QVERIFY(oi.isEmpty() == false);
    QVERIFY(oi.details().count() == 2); // it should not be removed!

    /* Test self assign */
    oi.operator =(oi);
    QVERIFY(oi.details().count() == 2);
    QVERIFY(oi.isEmpty() == false);
}


void tst_QOrganizerItem::description()
{
    QOrganizerItem oi;

    QString description = oi.description();
    QVERIFY(description.isEmpty());
    oi.setDescription("Test Event Description");
    QVERIFY(oi.description() == QString("Test Event Description"));

    /* Remove the detail via removeDetail */
    QOrganizerItemDescription old;
    QCOMPARE(oi.details().count(), 2); // type + description
    QVERIFY(!oi.removeDetail(&old)); // should fail.
    QVERIFY(oi.isEmpty() == false);
    QVERIFY(oi.details().count() == 2); // it should not be removed!

    /* Test self assign */
    oi.operator =(oi);
    QVERIFY(oi.details().count() == 2);
    QVERIFY(oi.isEmpty() == false);
}

void tst_QOrganizerItem::type()
{
    QOrganizerItem oi;
    QVERIFY(oi.isEmpty() == true);

    // ensure that the default type is the QOrganizerItemType::TypeNote type
    QVERIFY(oi.type() == QString(QLatin1String(QOrganizerItemType::TypeNote)));

    // now set it to be an event via the type mutator, and test that it works
    QOrganizerItemType eventType;
    eventType.setType(QString(QLatin1String(QOrganizerItemType::TypeEvent)));
    oi.setType(eventType);
    QVERIFY(oi.type() == QString(QLatin1String(QOrganizerItemType::TypeEvent)));

    // set it back to a note, via the string mutator
    oi.setType(QOrganizerItemType::TypeNote);
    QVERIFY(oi.type() == QString(QLatin1String(QOrganizerItemType::TypeNote)));
    QVERIFY(oi.isEmpty() == true); // type doesn't affect emptiness
}

void tst_QOrganizerItem::emptiness()
{
    QOrganizerItem oi;
    QVERIFY(oi.isEmpty() == true);

    oi.setType(QOrganizerItemType::TypeNote);
    QVERIFY(oi.type() == QString(QLatin1String(QOrganizerItemType::TypeNote)));
    QVERIFY(oi.isEmpty() == true); // type doesn't affect emptiness
}

void tst_QOrganizerItem::idLessThan()
{
    QOrganizerItemId id1;
    id1.setManagerUri("a");
    id1.setLocalId(1);
    QOrganizerItemId id2;
    id2.setManagerUri("a");
    id2.setLocalId(1);
    QVERIFY(!(id1 < id2));
    QVERIFY(!(id2 < id1));
    QOrganizerItemId id3;
    id3.setManagerUri("a");
    id3.setLocalId(2);
    QOrganizerItemId id4;
    id4.setManagerUri("b");
    id4.setLocalId(1);
    QOrganizerItemId id5; // no URI
    id5.setLocalId(2);
    QVERIFY(id1 < id3);
    QVERIFY(!(id3 < id1));
    QVERIFY(id1 < id4);
    QVERIFY(!(id4 < id1));
    QVERIFY(id3 < id4);
    QVERIFY(!(id4 < id3));
    QVERIFY(id5 < id1);
    QVERIFY(!(id1 < id5));
}

void tst_QOrganizerItem::idHash()
{
    QOrganizerItemId id1;
    id1.setManagerUri("a");
    id1.setLocalId(1);
    QOrganizerItemId id2;
    id2.setManagerUri("a");
    id2.setLocalId(1);
    QOrganizerItemId id3;
    id3.setManagerUri("b");
    id3.setLocalId(1);
    QVERIFY(qHash(id1) == qHash(id2));
    QVERIFY(qHash(id1) != qHash(id3));
    QSet<QOrganizerItemId> set;
    set.insert(id1);
    set.insert(id2);
    set.insert(id3);
    QCOMPARE(set.size(), 2);
}

void tst_QOrganizerItem::hash()
{
    QOrganizerItemId id;
    id.setManagerUri("a");
    id.setLocalId(1);
    QOrganizerItem oi1;
    oi1.setId(id);
    QOrganizerItemDetail detail1("definition");
    detail1.setValue("key", "value");
    oi1.saveDetail(&detail1);
    QOrganizerItem oi2;
    oi2.setId(id);
    oi2.saveDetail(&detail1);
    QOrganizerItem oi3;
    oi3.setId(id);
    QOrganizerItemDetail detail3("definition");
    detail3.setValue("key", "another value");
    oi3.saveDetail(&detail3);
    QOrganizerItem oi4; // no details
    oi4.setId(id);
    QOrganizerItem oi5;
    oi5.setId(id);
    oi5.saveDetail(&detail1);
    QVERIFY(qHash(oi1) == qHash(oi2));
    QVERIFY(qHash(oi1) != qHash(oi3));
    QVERIFY(qHash(oi1) != qHash(oi4));
    QVERIFY(qHash(oi1) == qHash(oi5));
}

void tst_QOrganizerItem::datastream()
{
    QByteArray buffer;
    QDataStream stream1(&buffer, QIODevice::WriteOnly);
    QOrganizerItem contactIn;
    QOrganizerItemId id;
    id.setManagerUri("manager");
    id.setLocalId(1234);
    contactIn.setId(id);
    QOrganizerItemLocation phone;
    phone.setAddress("5678");
    contactIn.saveDetail(&phone);
    stream1 << contactIn;

    QVERIFY(buffer.size() > 0);

    QDataStream stream2(buffer);
    QOrganizerItem contactOut;
    stream2 >> contactOut;
    QCOMPARE(contactOut, contactIn);
}

void tst_QOrganizerItem::traits()
{
    QVERIFY(sizeof(QOrganizerItem) == sizeof(void *));
    QTypeInfo<QTM_PREPEND_NAMESPACE(QOrganizerItem)> ti;
    QVERIFY(ti.isComplex);
    QVERIFY(!ti.isStatic);
    QVERIFY(!ti.isLarge);
    QVERIFY(!ti.isPointer);
    QVERIFY(!ti.isDummy);
}

void tst_QOrganizerItem::idTraits()
{
    QVERIFY(sizeof(QOrganizerItemId) == sizeof(void *));
    QTypeInfo<QTM_PREPEND_NAMESPACE(QOrganizerItemId)> ti;
    QVERIFY(ti.isComplex);
    QVERIFY(!ti.isStatic);
    QVERIFY(!ti.isLarge);
    QVERIFY(!ti.isPointer);
    QVERIFY(!ti.isDummy);
}

void tst_QOrganizerItem::localIdTraits()
{
    QVERIFY(sizeof(QOrganizerItemId) == sizeof(void *));
    QTypeInfo<QTM_PREPEND_NAMESPACE(QOrganizerItemLocalId)> ti;
    QVERIFY(!ti.isComplex);
    QVERIFY(!ti.isStatic);
    QVERIFY(!ti.isLarge);
    QVERIFY(!ti.isPointer);
    QVERIFY(!ti.isDummy);
}


QTEST_MAIN(tst_QOrganizerItem)
#include "tst_qorganizeritem.moc"
