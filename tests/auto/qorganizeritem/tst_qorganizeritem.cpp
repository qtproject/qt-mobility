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
#include "qorganizeritemengineid.h"
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
    void comments();
    void type();
    void emptiness();
    void idLessThan();
    void idHash();
    void hash();
    void datastream();
    void traits();
    void idTraits();
    void localIdTraits();
    void debugOutput();

    void event();
    void todo();
    void journal();
    void note();
    void eventOccurrence();
    void todoOccurrence();
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
    adr.setLabel("this is a test address");
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
    a.setLabel("another test address label");
    QVERIFY(oi.saveDetail(&a));
    QVERIFY(oi.isEmpty() == false);

    QVERIFY(oi.details().count() == 2);

    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName).count() == 1);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName, QOrganizerItemLocation::FieldLabel, QString("another test address label")).count() == 1);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName, QString(), QString("another test address label")).count() == oi.details(QOrganizerItemLocation::DefinitionName).count());

    QVERIFY(oi.details(QLatin1String(QOrganizerItemLocation::DefinitionName)).count() == 1);
    QVERIFY(oi.details(QLatin1String(QOrganizerItemLocation::DefinitionName), QOrganizerItemLocation::FieldLabel, QString("another test address label")).count() == 1);
    QVERIFY(oi.details(QLatin1String(QOrganizerItemLocation::DefinitionName), QString(), QString("another test address label")).count() == oi.details(QOrganizerItemLocation::DefinitionName).count());

    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName.latin1()).count() == 1);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName.latin1(), QOrganizerItemLocation::FieldLabel, QString("another test address label")).count() == 1);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName.latin1(), QString(), QString("another test address label")).count() == oi.details(QOrganizerItemLocation::DefinitionName).count());

    QVERIFY(oi.details<QOrganizerItemLocation>(QOrganizerItemLocation::FieldLabel, QString("another test address label")).count() == 1);
    QVERIFY(oi.details<QOrganizerItemLocation>(QOrganizerItemLocation::FieldLabel.latin1(), QString("another test address label")).count() == 1);
    QVERIFY(oi.details<QOrganizerItemLocation>(QLatin1String(QOrganizerItemLocation::FieldLabel), QString("another test address label")).count() == 1);

    QVERIFY(oi.details<QOrganizerItemLocation>().count() == 1);
    QVERIFY(!oi.detail(QOrganizerItemLocation::DefinitionName).isEmpty());
    QVERIFY(!oi.detail<QOrganizerItemLocation>().isEmpty());
    QCOMPARE(oi.detail<QOrganizerItemLocation>(), a);

    // Remove detail
    QVERIFY(oi.removeDetail(&a));
    QCOMPARE(oi.details().count(), 1);
    QVERIFY(oi.isEmpty() == true);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName).count() == 0);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName, QOrganizerItemLocation::FieldLabel, QString("another test address label")).count() == 0);
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
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName, QOrganizerItemLocation::FieldLabel, QString("another test address label")).count() == 0);
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
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName, QOrganizerItemLocation::FieldLabel, QString("another test address label")).count() == 0);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName, QString(), QString("another test address label")).count() == oi.details(QOrganizerItemLocation::DefinitionName).count());
    QVERIFY(oi.details<QOrganizerItemLocation>().count() == 0);
    QVERIFY(oi.detail(QOrganizerItemLocation::DefinitionName).isEmpty());
    QVERIFY(oi.detail<QOrganizerItemLocation>().isEmpty());

    QVERIFY(a == a3);

    // now we want to add multiple details of the same type, and test that retrieval works correctly.
    a2 = QOrganizerItemLocation();
    a2.setLabel("22222");
    a2.setValue("nonexistent-field", QVariant("22222-2"));
    oi.saveDetail(&a);
    oi.saveDetail(&a2);
    QCOMPARE(oi.details().count(), 3);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName).count() == 2);
    QVERIFY(oi.details(QOrganizerItemLocation::DefinitionName, QOrganizerItemLocation::FieldLabel, QString("another test address label")).count() == 1);
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

    one.setLabel("12345");
    two.setLabel("12345");

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
    one.setLabel("56678");
    QVERIFY(oi.saveDetail(&one));
    QVERIFY(ref == one);

    // Retrieve the detail again and modify it
    QOrganizerItemLocation three = oi.detail<QOrganizerItemLocation>();
    QVERIFY(ref == three);
    QVERIFY(one == three);
    three.setLabel("542343");
    QVERIFY(oi.saveDetail(&three));

    // Now see if we got any updates to ref/one
    QVERIFY(ref == one);
    QVERIFY(ref != three);

    // test saving of a detail with an empty field.
    QOrganizerItemLocation four;
    four.setLabel("");
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

    QOrganizerItemDisplayLabel displayLabel;
    displayLabel.setLabel("test label");
    oi.setDisplayLabel(displayLabel);
    QVERIFY(oi.displayLabel() == displayLabel.label());

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

    QOrganizerItemDescription descr;
    descr.setDescription("test description");
    oi.setDescription(descr);
    QVERIFY(oi.description() == descr.description());

    /* Test self assign */
    oi.operator =(oi);
    QVERIFY(oi.details().count() == 2);
    QVERIFY(oi.isEmpty() == false);
}

void tst_QOrganizerItem::comments()
{
    QOrganizerItem oi;

    QStringList comments = oi.comments();
    QVERIFY(comments.isEmpty());
    oi.addComment("test comment");
    QCOMPARE(oi.comments().size(), 1);
    QVERIFY(oi.details().count() == 2);
    QVERIFY(oi.comments().at(0) == QString("test comment"));
    oi.addComment("another test comment");
    QCOMPARE(oi.comments().size(), 2);
    QVERIFY(oi.details().count() == 3);
    QVERIFY(oi.comments().contains(QString("test comment")));
    QVERIFY(oi.comments().contains(QString("another test comment")));

    oi.clearComments();
    QVERIFY(oi.comments().size() == 0);
    QVERIFY(oi.details().count() == 1); // should have a type detail left.

    QOrganizerItemComment comment;
    comment.setComment("yet another test comment");
    oi.saveDetail(&comment);
    QCOMPARE(oi.comments().size(), 1);
    QVERIFY(oi.details().count() == 2);
    QVERIFY(oi.comments().at(0) == QString("yet another test comment"));

    oi.removeDetail(&comment);
    QVERIFY(oi.comments().size() == 0);
    QVERIFY(oi.details().count() == 1); // should have a type detail left.

    oi.addComment("and yet another test comment");

    /* Test self assign */
    oi.operator =(oi);
    QVERIFY(oi.details().count() == 2);
    QVERIFY(oi.isEmpty() == false);
}

void tst_QOrganizerItem::debugOutput()
{
    QOrganizerCollection c;
    QOrganizerItem item;
    QOrganizerItemDetail d;

    // Test that these can be used as the first argument to qDebug()
    qDebug() << item.id();
    qDebug() << item.collectionId();
    qDebug() << item;
    qDebug() << d;
    qDebug() << c;

    // And that other things can come after them (return type)
    qDebug() << item.id() << "id";
    qDebug() << item.collectionId() << "collection id";
    qDebug() << item << "item";
    qDebug() << d << "detail";
    qDebug() << c << "collection";

    // And for kicks, that other things can come first
    qDebug() << " " << item.id();
    qDebug() << " " << item.collectionId();
    qDebug() << " " << item;
    qDebug() << " " << d;
    qDebug() << " " << c;
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

class BasicItemLocalId : public QOrganizerItemEngineId
{
public:
    BasicItemLocalId(uint id) : m_id(id) {}
    bool isEqualTo(const QOrganizerItemEngineId* other) const {
        return m_id == static_cast<const BasicItemLocalId*>(other)->m_id;
    }
    bool isLessThan(const QOrganizerItemEngineId* other) const {
        return m_id < static_cast<const BasicItemLocalId*>(other)->m_id;
    }
    QString managerUri() const {
        static const QString uri(QLatin1String("qtorganizer:basicid:"));
        return uri;
    }
    QOrganizerItemEngineId* clone() const {
        BasicItemLocalId* cloned = new BasicItemLocalId(m_id);
        return cloned;
    }
    QDebug& debugStreamOut(QDebug& dbg) const {
        return dbg << m_id;
    }
    QString toString() const {
        return QString::number(m_id);
    }
    uint hash() const {
        return m_id;
    }

private:
    uint m_id;
};

QOrganizerItemId makeId(uint id)
{
    return QOrganizerItemId(new BasicItemLocalId(id));
}

void tst_QOrganizerItem::idLessThan()
{
    // TODO: review test
/*    QOrganizerItemId id1;
    id1.setManagerUri("a");
    id1.setLocalId(makeId(1));
    QOrganizerItemId id2;
    id2.setManagerUri("a");
    id2.setLocalId(makeId(1));
    QVERIFY(!(id1 < id2));
    QVERIFY(!(id2 < id1));
    QOrganizerItemId id3;
    id3.setManagerUri("a");
    id3.setLocalId(makeId(2));
    QOrganizerItemId id4;
    id4.setManagerUri("b");
    id4.setLocalId(makeId(1));
    QOrganizerItemId id5; // no URI
    id5.setLocalId(makeId(2));
    QVERIFY(id1 < id3);
    QVERIFY(!(id3 < id1));
    QVERIFY(id1 < id4);
    QVERIFY(!(id4 < id1));
    QVERIFY(id3 < id4);
    QVERIFY(!(id4 < id3));
    QVERIFY(id5 < id1);
    QVERIFY(!(id1 < id5));*/
}

void tst_QOrganizerItem::idHash()
{
    // TODO: review test
/*    QOrganizerItemId id1;
    id1.setManagerUri("a");
    id1.setLocalId(makeId(1));
    QOrganizerItemId id2;
    id2.setManagerUri("a");
    id2.setLocalId(makeId(1));
    QOrganizerItemId id3;
    id3.setManagerUri("b");
    id3.setLocalId(makeId(1));
    QVERIFY(qHash(id1) == qHash(id2));
    QVERIFY(qHash(id1) != qHash(id3));
    QSet<QOrganizerItemId> set;
    set.insert(id1);
    set.insert(id2);
    set.insert(id3);
    QCOMPARE(set.size(), 2);*/
}

void tst_QOrganizerItem::hash()
{
    // TODO: review test
/*    QOrganizerItemId id;
    id.setManagerUri("a");
    id.setLocalId(makeId(1));
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
    QVERIFY(qHash(oi1) == qHash(oi5));*/
}

void tst_QOrganizerItem::datastream()
{
    // item datastreaming
    QByteArray buffer;
    QOrganizerItem itemIn;
    itemIn.addComment("test comment");
    QOrganizerItem itemOut;
    QOrganizerItemId originalId;
    QOrganizerCollectionId originalCollectionId;

    // first, stream an item with a complete id
    {
        QDataStream stream1(&buffer, QIODevice::WriteOnly);
        QOrganizerManager om("memory");
        QVERIFY(om.saveItem(&itemIn)); // fill in its ID
        originalId = itemIn.id();
        originalCollectionId = itemIn.collectionId();
        stream1 << itemIn;
        QVERIFY(buffer.size() > 0);
        QDataStream stream2(buffer);
        stream2 >> itemOut;
        //QCOMPARE(itemOut, itemIn); // can't do QCOMPARE because detail keys get changed.
        QVERIFY(itemOut.details() == itemIn.details());
        QVERIFY(itemOut.id() == itemIn.id());
    }

    // TODO : review tests
    // second, stream an item with an id with the mgr uri set, local id null
/*    {
        QDataStream stream1(&buffer, QIODevice::WriteOnly);
        QOrganizerItemId modifiedId = originalId;
        modifiedId.setLocalId(QOrganizerItemLocalId());
        itemIn.setId(modifiedId);
        stream1 << itemIn;
        QVERIFY(buffer.size() > 0);
        QDataStream stream2(buffer);
        stream2 >> itemOut;
        //QCOMPARE(itemOut, itemIn); // can't do QCOMPARE because detail keys get changed.
        QVERIFY(itemOut.details() == itemIn.details());
        QVERIFY(itemOut.id() == itemIn.id());
    }

    // third, stream an item with an id with the mgr uri null, local id set
    {
        QDataStream stream1(&buffer, QIODevice::WriteOnly);
        QOrganizerItemId modifiedId = originalId;
        modifiedId.setManagerUri(QString()); // this will clear the local id!
        modifiedId.setLocalId(originalId.localId()); // so reset it and make sure things don't fall over.
        itemIn.setId(modifiedId);
        stream1 << itemIn;
        QVERIFY(buffer.size() > 0);
        QDataStream stream2(buffer);
        stream2 >> itemOut;
        //QCOMPARE(itemOut, itemIn); // can't do QCOMPARE because detail keys get changed.
        QVERIFY(itemOut.details() == itemIn.details());
        QVERIFY(itemOut.id() != itemIn.id()); // in this case, with null mgr uri, the id doesn't get serialized.
    }*/

    // fourth, stream an item with null ids
    {
        QDataStream stream1(&buffer, QIODevice::WriteOnly);
        itemIn.setId(QOrganizerItemId());
        itemIn.setCollectionId(QOrganizerCollectionId());
        stream1 << itemIn;
        QVERIFY(buffer.size() > 0);
        QDataStream stream2(buffer);
        stream2 >> itemOut;
        //QCOMPARE(itemOut, itemIn); // can't do QCOMPARE because detail keys get changed.
        QVERIFY(itemOut.details() == itemIn.details());
        QVERIFY(itemOut.id() == itemIn.id());
    }

    // fifth, stream an item with a collection id
    {
        QDataStream stream1(&buffer, QIODevice::WriteOnly);
        itemIn.setId(QOrganizerItemId());
        itemIn.setCollectionId(originalCollectionId);
        stream1 << itemIn;
        QVERIFY(buffer.size() > 0);
        QDataStream stream2(buffer);
        stream2 >> itemOut;
        //QCOMPARE(itemOut, itemIn); // can't do QCOMPARE because detail keys get changed.
        QVERIFY(itemOut.details() == itemIn.details());
        QVERIFY(itemOut.collectionId() == itemIn.collectionId());
        QVERIFY(itemOut.id() == itemIn.id());
    }

    // id datastreaming
    buffer.clear();
    QOrganizerItemId inputId;
    QOrganizerItemId outputId;

    // first, stream the whole id (mgr uri set, local id set)
    {
        inputId = originalId;
        QString serializedId = inputId.toString();
        outputId = QOrganizerItemId::fromString(serializedId);
        QCOMPARE(inputId, outputId);

        inputId = originalId;
        buffer.clear();
        QDataStream stream1(&buffer, QIODevice::WriteOnly);
        stream1 << inputId;
        QVERIFY(buffer.size() > 0);
        QDataStream stream2(buffer);
        stream2 >> outputId;
        QCOMPARE(inputId, outputId);
    }

    // TODO : review tests
    // second, stream a partial id (mgr uri null, local id set)
/*    {
        inputId.setManagerUri(QString());
        inputId.setLocalId(originalId.localId());
        buffer.clear();
        QDataStream stream1(&buffer, QIODevice::WriteOnly);
        stream1 << inputId;
        QVERIFY(buffer.size() > 0);
        QDataStream stream2(buffer);
        stream2 >> outputId;

        // because the manager uri is null, we cannot stream it in.
        QVERIFY(outputId.isNull());
        QVERIFY(!inputId.isNull());
    }

    // third, stream a partial id (mgr uri set, local id null).
    {
        inputId.setManagerUri(originalId.managerUri());
        inputId.setLocalId(QOrganizerItemLocalId());
        buffer.clear();
        QDataStream stream1(&buffer, QIODevice::WriteOnly);
        stream1 << inputId;
        QVERIFY(buffer.size() > 0);
        QDataStream stream2(buffer);
        stream2 >> outputId;
        QCOMPARE(inputId, outputId);
    }*/

    // fourth, stream a null id
    {
        inputId = QOrganizerItemId();
        QString serializedId = inputId.toString();
        outputId = QOrganizerItemId::fromString(serializedId);
        QCOMPARE(inputId, outputId);

        inputId = QOrganizerItemId();
        buffer.clear();
        QDataStream stream1(&buffer, QIODevice::WriteOnly);
        stream1 << inputId;
        QVERIFY(buffer.size() > 0);
        QDataStream stream2(buffer);
        stream2 >> outputId;
        QCOMPARE(inputId, outputId);
    }

    // TODO : review tests
    // fifth, stream an id after changing it's manager uri string.
/*    {
        inputId.setManagerUri(originalId.managerUri());
        inputId.setLocalId(originalId.localId());
        inputId.setManagerUri("test manager uri"); // should clear the local id.
        QVERIFY(inputId.localId() == QOrganizerItemLocalId());
        buffer.clear();
        QDataStream stream1(&buffer, QIODevice::WriteOnly);
        stream1 << inputId;
        QVERIFY(buffer.size() > 0);
        QDataStream stream2(buffer);
        stream2 >> outputId;
        QCOMPARE(inputId, outputId);
    }

    // sixth, stream an id after changing it's manager uri string, and resetting the local id.
    // this should cause great problems, because the manager doesn't exist so it shouldn't
    // be able to deserialize.  Make sure it's handled gracefully.
    {
        inputId.setManagerUri(originalId.managerUri());
        inputId.setManagerUri("test manager uri"); // should clear the local id.
        inputId.setLocalId(originalId.localId());
        buffer.clear();
        QDataStream stream1(&buffer, QIODevice::WriteOnly);
        stream1 << inputId;
        QVERIFY(buffer.size() > 0);
        QDataStream stream2(buffer);
        stream2 >> outputId;
        QVERIFY(outputId.isNull());
    }*/
}

void tst_QOrganizerItem::traits()
{
    QCOMPARE(sizeof(QOrganizerItem), sizeof(void *));
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
    QTypeInfo<QTM_PREPEND_NAMESPACE(QOrganizerItemId)> ti;
    QVERIFY(ti.isComplex); // unlike QContactLocalId (int typedef), we have a ctor
    QVERIFY(!ti.isStatic);
    QVERIFY(!ti.isLarge);
    QVERIFY(!ti.isPointer);
    QVERIFY(!ti.isDummy);
}

void tst_QOrganizerItem::event()
{
    QOrganizerEvent testEvent;
    QCOMPARE(testEvent.type(), QString(QLatin1String(QOrganizerItemType::TypeEvent)));

    testEvent.setLocation("test address");
    QCOMPARE(testEvent.location(), QString("test address"));

    testEvent.setStartDateTime(QDateTime(QDate::currentDate()));
    QCOMPARE(testEvent.startDateTime(), QDateTime(QDate::currentDate()));
    testEvent.setEndDateTime(QDateTime(QDate::currentDate().addDays(1)));
    QCOMPARE(testEvent.endDateTime(), QDateTime(QDate::currentDate().addDays(1)));
    QVERIFY(!testEvent.isAllDay()); // default to not all day
    testEvent.setAllDay(true);
    QVERIFY(testEvent.isAllDay());

    testEvent.setPriority(QOrganizerItemPriority::VeryHighPriority);
    QCOMPARE(testEvent.priority(), QOrganizerItemPriority::VeryHighPriority);
    testEvent.setPriority(QOrganizerItemPriority::VeryLowPriority);
    QCOMPARE(testEvent.priority(), QOrganizerItemPriority::VeryLowPriority);


    QSet<QDate> rdates;
    testEvent.setRecurrenceDate(QDate::currentDate());
    QCOMPARE(testEvent.recurrenceDates(), QSet<QDate>() << QDate::currentDate());
    rdates << QDate::currentDate() << QDate::currentDate().addDays(3) << QDate::currentDate().addDays(8);
    testEvent.setRecurrenceDates(rdates);
    QCOMPARE(testEvent.recurrenceDates(), rdates);

    QSet<QDate> exdates;
    testEvent.setExceptionDate(QDate::currentDate());
    QCOMPARE(testEvent.exceptionDates(), QSet<QDate>() << QDate::currentDate());
    exdates << QDate::currentDate().addDays(3);
    testEvent.setExceptionDates(exdates);
    QCOMPARE(testEvent.exceptionDates(), exdates);

    QSet<QOrganizerRecurrenceRule> rrules;
    QOrganizerRecurrenceRule rrule;

    QVERIFY(rrule.limitType() == QOrganizerRecurrenceRule::NoLimit);
    QVERIFY(rrule.limitCount() == -1);
    QVERIFY(rrule.limitDate().isNull());

    rrule.setLimit(1);
    QVERIFY(rrule.limitType() == QOrganizerRecurrenceRule::CountLimit);
    QVERIFY(rrule.limitCount() == 1);
    QVERIFY(rrule.limitDate().isNull());

    rrule.setLimit(-1);
    QVERIFY(rrule.limitType() == QOrganizerRecurrenceRule::NoLimit);
    QVERIFY(rrule.limitCount() == -1);
    QVERIFY(rrule.limitDate().isNull());

    rrule.setLimit(0);
    QVERIFY(rrule.limitType() == QOrganizerRecurrenceRule::CountLimit);
    QVERIFY(rrule.limitCount() == 0);
    QVERIFY(rrule.limitDate().isNull());

    rrule.setLimit(-100);
    QVERIFY(rrule.limitType() == QOrganizerRecurrenceRule::NoLimit);
    QVERIFY(rrule.limitCount() == -1);
    QVERIFY(rrule.limitDate().isNull());

    rrule.setLimit(QDate());
    QVERIFY(rrule.limitType() == QOrganizerRecurrenceRule::NoLimit);
    QVERIFY(rrule.limitCount() == -1);
    QVERIFY(rrule.limitDate().isNull());

    rrule.setLimit(QDate(2010, 10, 6));
    QVERIFY(rrule.limitType() == QOrganizerRecurrenceRule::DateLimit);
    QVERIFY(rrule.limitCount() == -1);
    QVERIFY(rrule.limitDate() == QDate(2010, 10, 6));

    rrule.setLimit(QDate(2010, 13, 34));
    QVERIFY(rrule.limitType() == QOrganizerRecurrenceRule::NoLimit);
    QVERIFY(rrule.limitCount() == -1);
    QVERIFY(rrule.limitDate() == QDate());


    rrule.setLimit(2);
    rrule.setFrequency(QOrganizerRecurrenceRule::Daily);
    rrules << rrule;
    testEvent.setRecurrenceRule(rrule);
    QVERIFY(testEvent.recurrenceRules() == rrules);
    QVERIFY(testEvent.recurrenceRule() == rrule);
    testEvent.setRecurrenceRule(rrule);
    QVERIFY(testEvent.recurrenceRules() == rrules);
    QVERIFY(testEvent.recurrenceRule() == rrule);

    QSet<QOrganizerRecurrenceRule> exrules;
    QOrganizerRecurrenceRule exrule;
    exrule.setLimit(1);
    exrule.setFrequency(QOrganizerRecurrenceRule::Weekly);
    exrules << exrule;
    testEvent.setExceptionRules(exrules);
    QVERIFY(testEvent.exceptionRules() == exrules);
    QVERIFY(testEvent.exceptionRule() == exrule);
    testEvent.setExceptionRule(exrule);
    QVERIFY(testEvent.exceptionRules() == exrules);
    QVERIFY(testEvent.exceptionRule() == exrule);
}

void tst_QOrganizerItem::todo()
{
    QOrganizerTodo testTodo;
    QCOMPARE(testTodo.type(), QString(QLatin1String(QOrganizerItemType::TypeTodo)));

    QCOMPARE(testTodo.status(), QOrganizerTodoProgress::StatusNotStarted);
    testTodo.setStatus(QOrganizerTodoProgress::StatusInProgress);
    QCOMPARE(testTodo.status(), QOrganizerTodoProgress::StatusInProgress);

    QCOMPARE(testTodo.progressPercentage(), 0);
    testTodo.setProgressPercentage(50);
    QCOMPARE(testTodo.progressPercentage(), 50);
    testTodo.setStatus(QOrganizerTodoProgress::StatusComplete);
    QCOMPARE(testTodo.progressPercentage(), 50); // XXX TODO: should this update automatically?

    QDateTime currentDateTime = QDateTime::currentDateTime();
    testTodo.setStartDateTime(currentDateTime);
    QCOMPARE(testTodo.startDateTime(), currentDateTime);
    testTodo.setDueDateTime(QDateTime(QDate::currentDate()));
    QCOMPARE(testTodo.dueDateTime(), QDateTime(QDate::currentDate()));
    testTodo.setFinishedDateTime(QDateTime(QDate::currentDate().addDays(1)));
    QCOMPARE(testTodo.finishedDateTime(), QDateTime(QDate::currentDate().addDays(1)));

    QVERIFY(testTodo.isAllDay() == false); // default should not be all day event.
    testTodo.setAllDay(true);
    QVERIFY(testTodo.isAllDay() == true);
    testTodo.setAllDay(false);
    QVERIFY(testTodo.isAllDay() == false);

    testTodo.setPriority(QOrganizerItemPriority::VeryHighPriority);
    QCOMPARE(testTodo.priority(), QOrganizerItemPriority::VeryHighPriority);
    testTodo.setPriority(QOrganizerItemPriority::VeryLowPriority);
    QCOMPARE(testTodo.priority(), QOrganizerItemPriority::VeryLowPriority);

    QSet<QDate> rdates;
    rdates << QDate::currentDate() << QDate::currentDate().addDays(3) << QDate::currentDate().addDays(8);
    testTodo.setRecurrenceDates(rdates);
    QCOMPARE(testTodo.recurrenceDates(), rdates);

    QSet<QDate> exdates;
    exdates << QDate::currentDate().addDays(3);
    testTodo.setExceptionDates(exdates);
    QCOMPARE(testTodo.exceptionDates(), exdates);

    QSet<QOrganizerRecurrenceRule> rrules;
    QOrganizerRecurrenceRule rrule;
    rrule.setLimit(2);
    rrule.setFrequency(QOrganizerRecurrenceRule::Daily);
    rrules << rrule;
    testTodo.setRecurrenceRules(rrules);
    QVERIFY(testTodo.recurrenceRules() == rrules);
    QVERIFY(testTodo.recurrenceRule() == rrule);
    testTodo.setRecurrenceRule(rrule);
    QVERIFY(testTodo.recurrenceRules() == rrules);
    QVERIFY(testTodo.recurrenceRule() == rrule);

    QSet<QOrganizerRecurrenceRule> exrules;
    QOrganizerRecurrenceRule exrule;
    exrule.setLimit(1);
    exrule.setFrequency(QOrganizerRecurrenceRule::Weekly);
    exrules << exrule;
    testTodo.setExceptionRules(exrules);
    QVERIFY(testTodo.exceptionRules() == exrules);
    QVERIFY(testTodo.exceptionRule() == exrule);
    testTodo.setExceptionRule(exrule);
    QVERIFY(testTodo.exceptionRules() == exrules);
    QVERIFY(testTodo.exceptionRule() == exrule);
}

void tst_QOrganizerItem::journal()
{
    QOrganizerJournal testJournal;
    QCOMPARE(testJournal.type(), QString(QLatin1String(QOrganizerItemType::TypeJournal)));

    QDateTime currDateTime = QDateTime::currentDateTime();
    testJournal.setDateTime(currDateTime);
    QCOMPARE(testJournal.dateTime(), currDateTime);
}

void tst_QOrganizerItem::note()
{
    QOrganizerNote testNote;
    QCOMPARE(testNote.type(), QString(QLatin1String(QOrganizerItemType::TypeNote)));
}

void tst_QOrganizerItem::eventOccurrence()
{
    QOrganizerEventOccurrence testEventOccurrence;
    QCOMPARE(testEventOccurrence.type(), QString(QLatin1String(QOrganizerItemType::TypeEventOccurrence)));

    testEventOccurrence.setLocation("test address");
    QCOMPARE(testEventOccurrence.location(), QString("test address"));

    testEventOccurrence.setStartDateTime(QDateTime(QDate::currentDate()));
    QCOMPARE(testEventOccurrence.startDateTime(), QDateTime(QDate::currentDate()));
    testEventOccurrence.setEndDateTime(QDateTime(QDate::currentDate().addDays(1)));
    QCOMPARE(testEventOccurrence.endDateTime(), QDateTime(QDate::currentDate().addDays(1)));

    testEventOccurrence.setPriority(QOrganizerItemPriority::VeryHighPriority);
    QCOMPARE(testEventOccurrence.priority(), QOrganizerItemPriority::VeryHighPriority);
    testEventOccurrence.setPriority(QOrganizerItemPriority::VeryLowPriority);
    QCOMPARE(testEventOccurrence.priority(), QOrganizerItemPriority::VeryLowPriority);

    // the parent id and original date time must be tested properly in the manager unit test
    // but we will test the API mutator/accessor functions here.
    QOrganizerItemId id;
    testEventOccurrence.setParentId(id);
    QCOMPARE(testEventOccurrence.parentId(), id);
    QDateTime originalDateTime = QDateTime::currentDateTime();
    testEventOccurrence.setOriginalDate(originalDateTime.date());
    QCOMPARE(testEventOccurrence.originalDate(), originalDateTime.date());
}

void tst_QOrganizerItem::todoOccurrence()
{
    QOrganizerTodoOccurrence testTodoOccurrence;
    QCOMPARE(testTodoOccurrence.type(), QString(QLatin1String(QOrganizerItemType::TypeTodoOccurrence)));

    QCOMPARE(testTodoOccurrence.status(), QOrganizerTodoProgress::StatusNotStarted);
    testTodoOccurrence.setStatus(QOrganizerTodoProgress::StatusInProgress);
    QCOMPARE(testTodoOccurrence.status(), QOrganizerTodoProgress::StatusInProgress);

    QCOMPARE(testTodoOccurrence.progressPercentage(), 0);
    testTodoOccurrence.setProgressPercentage(50);
    QCOMPARE(testTodoOccurrence.progressPercentage(), 50);
    testTodoOccurrence.setStatus(QOrganizerTodoProgress::StatusComplete);
    QCOMPARE(testTodoOccurrence.progressPercentage(), 50); // XXX TODO: should this update automatically?

    testTodoOccurrence.setStartDateTime(QDateTime(QDate::currentDate()));
    QCOMPARE(testTodoOccurrence.startDateTime(), QDateTime(QDate::currentDate()));
    testTodoOccurrence.setDueDateTime(QDateTime(QDate::currentDate()));
    QCOMPARE(testTodoOccurrence.dueDateTime(), QDateTime(QDate::currentDate()));
    testTodoOccurrence.setFinishedDateTime(QDateTime(QDate::currentDate().addDays(1)));
    QCOMPARE(testTodoOccurrence.finishedDateTime(), QDateTime(QDate::currentDate().addDays(1)));

    testTodoOccurrence.setPriority(QOrganizerItemPriority::VeryHighPriority);
    QCOMPARE(testTodoOccurrence.priority(), QOrganizerItemPriority::VeryHighPriority);
    testTodoOccurrence.setPriority(QOrganizerItemPriority::VeryLowPriority);
    QCOMPARE(testTodoOccurrence.priority(), QOrganizerItemPriority::VeryLowPriority);

    // the parent id and original date time must be tested properly in the manager unit test
    // but we will test the API mutator/accessor functions here.
    QOrganizerItemId id;
    testTodoOccurrence.setParentId(id);
    QCOMPARE(testTodoOccurrence.parentId(), id);
    QDateTime originalDateTime = QDateTime::currentDateTime();
    testTodoOccurrence.setOriginalDate(originalDateTime.date());
    QCOMPARE(testTodoOccurrence.originalDate(), originalDateTime.date());
}

QTEST_MAIN(tst_QOrganizerItem)
#include "tst_qorganizeritem.moc"
