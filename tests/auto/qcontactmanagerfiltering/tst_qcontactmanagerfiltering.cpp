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

/*
 * This test is mostly just for testing sorting and filtering -
 * having it in tst_QContactManager makes maintenance more
 * difficult!
 */

class tst_QContactManagerFiltering : public QObject
{
Q_OBJECT

public:
    tst_QContactManagerFiltering();
    virtual ~tst_QContactManagerFiltering();

private:
    void dumpContactDifferences(const QContact& a, const QContact& b);
    void dumpContact(const QContact &c);
    void dumpContacts();
    bool isSuperset(const QContact& ca, const QContact& cb);

    void addManagers(); // add standard managers to the data
public slots:
    void init();
    void cleanup();
private slots:

    void filtering(); // XXX should take all managers
    void sorting(); // XXX should take all managers
};

tst_QContactManagerFiltering::tst_QContactManagerFiltering()
{
}

tst_QContactManagerFiltering::~tst_QContactManagerFiltering()
{
}

void tst_QContactManagerFiltering::init()
{
    /* Make sure these other test plugins are NOT loaded by default */
    QVERIFY(!QContactManager::availableManagers().contains("testdummy"));
    QVERIFY(!QContactManager::availableManagers().contains("teststaticdummy"));
}

void tst_QContactManagerFiltering::cleanup()
{
}

void tst_QContactManagerFiltering::dumpContactDifferences(const QContact& ca, const QContact& cb)
{
    // Try to narrow down the differences
    QContact a(ca);
    QContact b(cb);

    QContactName n1 = a.detail(QContactName::DefinitionName);
    QContactName n2 = b.detail(QContactName::DefinitionName);

    // Check the name components in more detail
    QCOMPARE(n1.first(), n2.first());
    QCOMPARE(n1.middle(), n2.middle());
    QCOMPARE(n1.last(), n2.last());
    QCOMPARE(n1.prefix(), n2.prefix());
    QCOMPARE(n1.suffix(), n2.suffix());

    // Check the display label
    QCOMPARE(a.displayLabel().label(), b.displayLabel().label());
    QCOMPARE(a.displayLabel().isSynthesised(), b.displayLabel().isSynthesised());

    // Now look at the rest
    QList<QContactDetail> aDetails = a.details();
    QList<QContactDetail> bDetails = b.details();

    // They can be in any order, so loop
    // First remove any matches
    foreach(QContactDetail d, aDetails) {
        foreach(QContactDetail d2, bDetails) {
            if(d == d2) {
                a.removeDetail(&d);
                b.removeDetail(&d2);
                break;
            }
        }
    }

    // Now dump the extra details that were unmatched in A
    aDetails = a.details();
    bDetails = b.details();
    foreach(QContactDetail d, aDetails) {
        if (d.definitionName() != QContactDisplayLabel::DefinitionName)
            qDebug() << "A contact had extra detail:" << d.definitionName() << d.attributes() << d.values();
    }
    // and same for B
    foreach(QContactDetail d, bDetails) {
        if (d.definitionName() != QContactDisplayLabel::DefinitionName)
            qDebug() << "B contact had extra detail:" << d.definitionName() << d.attributes() << d.values();
    }

    QCOMPARE(b, a);
}

bool tst_QContactManagerFiltering::isSuperset(const QContact& ca, const QContact& cb)
{
    // returns true if contact ca is a superset of contact cb
    // we use this test instead of equality because dynamic information
    // such as presence/location, and synthesised information such as
    // display label, may differ between a contact in memory and the
    // contact in the managed store.

    QContact a(ca);
    QContact b(cb);
    QList<QContactDetail> aDetails = a.details();
    QList<QContactDetail> bDetails = b.details();

    // They can be in any order, so loop
    // First remove any matches
    foreach(QContactDetail d, aDetails) {
        foreach(QContactDetail d2, bDetails) {
            if(d == d2) {
                a.removeDetail(&d);
                b.removeDetail(&d2);
                break;
            }
        }
    }

    // Now check to see if b has any details remaining; if so, a is not a superset.
    // Note that the DisplayLabel can never be removed.
    if (b.details().size() > 1 || (b.details().size() == 1 && b.details().value(0).definitionName() != QContactDisplayLabel::DefinitionName))
        return false;
    return true;
}

void tst_QContactManagerFiltering::dumpContact(const QContact& contact)
{
    QContactManager m;
    qDebug() << "Contact: " << contact.id() << "(" << m.synthesiseDisplayLabel(contact) << ")";
    QList<QContactDetail> details = contact.details();
    foreach(QContactDetail d, details) {
        qDebug() << "  " << d.definitionName() << ":";
        qDebug() << "    Attr:" << d.attributes();
        qDebug() << "    Vals:" << d.values();
    }
}

void tst_QContactManagerFiltering::dumpContacts()
{
    QContactManager m;
    QList<QUniqueId> ids = m.contacts();

    foreach(QUniqueId id, ids) {
        QContact c = m.contact(id);
        dumpContact(c);
    }
}

void tst_QContactManagerFiltering::filtering()
{
    /* Try the memory database first */
    QContactManager* cm = new QContactManager("memory");

    /* Make sure it's empty */
    QList<QUniqueId> ids = cm->contacts();
    cm->removeContacts(&ids);

    /* Register an int detail */
    QContactDetailDefinition def;
    QMap<QString, QContactDetailDefinition::Field> fields;
    QContactDetailDefinition::Field field;
    def.setId("Integer");
    field.dataType = QVariant::Int;
    fields.insert("value", field);
    def.setFields(fields);

    QVERIFY(cm->saveDetailDefinition(def));

    def.setId("DateTime");
    fields.clear();
    field.dataType = QVariant::DateTime;
    fields.insert("value", field);
    def.setFields(fields);

    QVERIFY(cm->saveDetailDefinition(def));

    /* Add some contacts */
    QContact a, b, c;
    QContactName aName, bName, cName;
    aName.setFirst("Aaron");
    aName.setLast("Aaronson");
    a.saveDetail(&aName);
    QContactPhoneNumber number;
    number.setNumber("555-1212");
    a.saveDetail(&number);

    QContactDetail integer("Integer");
    integer.setValue("value", 10);
    a.saveDetail(&integer);

    QContactDetail datetime("DateTime");
    datetime.setValue("value", QDateTime(QDate(2009, 06, 29), QTime(16, 52, 23, 0)));
    a.saveDetail(&datetime);

    bName.setFirst("Bob");
    bName.setLast("Aaronsen");
    b.saveDetail(&bName);
    number.setNumber("555-3456");
    b.saveDetail(&number);

    integer.setValue("value", 20);
    b.saveDetail(&integer);

    cName.setFirst("Boris");
    cName.setLast("Aaronsun");
    c.saveDetail(&cName);

    integer.setValue("value", -20);
    datetime.setValue("value", QDateTime(QDate(2009, 06, 29), QTime(16, 54, 17, 0)));
    c.saveDetail(&integer);
    c.saveDetail(&datetime);

    QVERIFY(cm->saveContact(&a));
    QVERIFY(cm->saveContact(&b));
    QVERIFY(cm->saveContact(&c));

    QCOMPARE(cm->contacts().count(), 3);

    /* Reload the contacts to pick up any changes */
    a = cm->contact(a.id());
    b = cm->contact(b.id());
    c = cm->contact(c.id());

    QContactDetailFilter df;
    df.setDetailDefinitionName(QContactPhoneNumber::DefinitionName);

    ids = cm->contacts(df);
    QCOMPARE(ids.count(), 2);

    QContact a1 = cm->contact(ids.at(0));
    if (a1.id() == a.id()) {
        dumpContactDifferences(a1, a);
        dumpContactDifferences(cm->contact(ids.at(1)), b);
    } else {
        dumpContactDifferences(a1, b);
        dumpContactDifferences(cm->contact(ids.at(1)), a);
    }

    df.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, "Hamburger");
    ids = cm->contacts(df);
    QCOMPARE(ids.count(), 0);

    df.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
    ids = cm->contacts(df);
    QCOMPARE(ids.count(), 2);

    df.setValue("555-1212");
    ids = cm->contacts(df);
    QCOMPARE(ids.count(), 1);
    dumpContactDifferences(cm->contact(ids.at(0)), a);
    QCOMPARE(cm->contact(ids.at(0)), a);

    QContactDetailRangeFilter drf;
    drf.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);

    drf.setRange("555-1200", "555-1220");
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 1);
    dumpContactDifferences(cm->contact(ids.at(0)), a);
    QCOMPARE(cm->contact(ids.at(0)), a);

    /* Some name matching */
    df.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirst);
    df.setValue("Bob");
    ids = cm->contacts(df);
    QCOMPARE(ids.count(), 1);
    dumpContactDifferences(cm->contact(ids.at(0)), b);
    QCOMPARE(cm->contact(ids.at(0)), b);

    /* Starts with */
    df.setValue("B");
    ids = cm->contacts(df);
    QCOMPARE(ids.count(), 0);

    df.setMatchFlags(Qt::MatchStartsWith);
    ids = cm->contacts(df);
    QCOMPARE(ids.count(), 2);
    dumpContactDifferences(cm->contact(ids.at(0)), b);
    QCOMPARE(cm->contact(ids.at(0)), b);
    dumpContactDifferences(cm->contact(ids.at(1)), c);
    QCOMPARE(cm->contact(ids.at(1)), c);

    /* Ends with */
    df.setValue("ob");
    ids = cm->contacts(df);
    QCOMPARE(ids.count(), 0);

    df.setMatchFlags(Qt::MatchEndsWith);
    ids = cm->contacts(df);
    QCOMPARE(ids.count(), 1);
    dumpContactDifferences(cm->contact(ids.at(0)), b);
    QCOMPARE(cm->contact(ids.at(0)), b);

    df.setValue("OB");
    ids = cm->contacts(df);
    QCOMPARE(ids.count(), 1);
    dumpContactDifferences(cm->contact(ids.at(0)), b);
    QCOMPARE(cm->contact(ids.at(0)), b);

    df.setMatchFlags(Qt::MatchEndsWith | Qt::MatchCaseSensitive);
    ids = cm->contacts(df);
    QCOMPARE(ids.count(), 0);

    // Test contains
    df.setMatchFlags(Qt::MatchContains);
    df.setValue("r");
    ids = cm->contacts(df);
    QCOMPARE(ids.count(), 2);
    dumpContactDifferences(cm->contact(ids.at(0)), a);
    QCOMPARE(cm->contact(ids.at(0)), a);
    dumpContactDifferences(cm->contact(ids.at(1)), c);
    QCOMPARE(cm->contact(ids.at(1)), c);

    df.setValue("R");
    ids = cm->contacts(df);
    QCOMPARE(ids.count(), 2);
    dumpContactDifferences(cm->contact(ids.at(0)), a);
    QCOMPARE(cm->contact(ids.at(0)), a);
    dumpContactDifferences(cm->contact(ids.at(1)), c);
    QCOMPARE(cm->contact(ids.at(1)), c);

    df.setMatchFlags(Qt::MatchContains | Qt::MatchCaseSensitive);
    df.setValue("r");
    ids = cm->contacts(df);
    QCOMPARE(ids.count(), 2);
    dumpContactDifferences(cm->contact(ids.at(0)), a);
    QCOMPARE(cm->contact(ids.at(0)), a);
    dumpContactDifferences(cm->contact(ids.at(1)), c);
    QCOMPARE(cm->contact(ids.at(1)), c);

    df.setValue("R");
    ids = cm->contacts(df);
    QCOMPARE(ids.count(), 0);

    /* Range testing */
    drf.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirst);
    drf.setMatchFlags(0);
    drf.setRange("A", "Bob");
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 1);
    dumpContactDifferences(cm->contact(ids.at(0)), a);
    QCOMPARE(cm->contact(ids.at(0)), a);
    drf.setRange("A", "Bob", QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::ExcludeUpper);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 1);
    dumpContactDifferences(cm->contact(ids.at(0)), a);
    QCOMPARE(cm->contact(ids.at(0)), a);

    drf.setRange("A", "Bob", QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 1);
    dumpContactDifferences(cm->contact(ids.at(0)), a);
    QCOMPARE(cm->contact(ids.at(0)), a);

    drf.setRange("A", "Bob", QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 2);
    dumpContactDifferences(cm->contact(ids.at(0)), a);
    QCOMPARE(cm->contact(ids.at(0)), a);
    dumpContactDifferences(cm->contact(ids.at(1)), b);
    QCOMPARE(cm->contact(ids.at(1)), b);

    drf.setRange("A", "Bob", QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::IncludeUpper);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 2);
    dumpContactDifferences(cm->contact(ids.at(0)), a);
    QCOMPARE(cm->contact(ids.at(0)), a);
    dumpContactDifferences(cm->contact(ids.at(1)), b);
    QCOMPARE(cm->contact(ids.at(1)), b);

    drf.setRange("Bob", "C");
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 2);
    dumpContactDifferences(cm->contact(ids.at(0)), b);
    QCOMPARE(cm->contact(ids.at(0)), b);
    dumpContactDifferences(cm->contact(ids.at(1)), c);
    QCOMPARE(cm->contact(ids.at(1)), c);

    drf.setRange("Bob", "C", QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::ExcludeUpper);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 2);
    dumpContactDifferences(cm->contact(ids.at(0)), b);
    QCOMPARE(cm->contact(ids.at(0)), b);
    dumpContactDifferences(cm->contact(ids.at(1)), c);
    QCOMPARE(cm->contact(ids.at(1)), c);

    drf.setRange("Bob", "C", QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 2);
    dumpContactDifferences(cm->contact(ids.at(0)), b);
    QCOMPARE(cm->contact(ids.at(0)), b);
    dumpContactDifferences(cm->contact(ids.at(1)), c);
    QCOMPARE(cm->contact(ids.at(1)), c);

    drf.setRange("Bob", "C", QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::IncludeUpper);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 1);
    dumpContactDifferences(cm->contact(ids.at(0)), c);
    QCOMPARE(cm->contact(ids.at(0)), c);

    drf.setRange("Bob", "C", QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 1);
    dumpContactDifferences(cm->contact(ids.at(0)), c);
    QCOMPARE(cm->contact(ids.at(0)), c);

    /* Check that starts with gives the same results */
    drf.setMatchFlags(Qt::MatchStartsWith);
    drf.setRange("A", "Bob");
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 1);
    dumpContactDifferences(cm->contact(ids.at(0)), a);
    QCOMPARE(cm->contact(ids.at(0)), a);

    drf.setRange("A", "Bob", QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::ExcludeUpper);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 1);
    dumpContactDifferences(cm->contact(ids.at(0)), a);
    QCOMPARE(cm->contact(ids.at(0)), a);

    drf.setRange("A", "Bob", QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 1);
    dumpContactDifferences(cm->contact(ids.at(0)), a);
    QCOMPARE(cm->contact(ids.at(0)), a);

    drf.setRange("A", "Bob", QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 2);
    dumpContactDifferences(cm->contact(ids.at(0)), a);
    QCOMPARE(cm->contact(ids.at(0)), a);
    dumpContactDifferences(cm->contact(ids.at(1)), b);
    QCOMPARE(cm->contact(ids.at(1)), b);

    drf.setRange("A", "Bob", QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::IncludeUpper);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 2);
    dumpContactDifferences(cm->contact(ids.at(0)), a);
    QCOMPARE(cm->contact(ids.at(0)), a);
    dumpContactDifferences(cm->contact(ids.at(1)), b);
    QCOMPARE(cm->contact(ids.at(1)), b);

    drf.setRange("Bob", "C");
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 2);
    dumpContactDifferences(cm->contact(ids.at(0)), b);
    QCOMPARE(cm->contact(ids.at(0)), b);
    dumpContactDifferences(cm->contact(ids.at(1)), c);
    QCOMPARE(cm->contact(ids.at(1)), c);

    drf.setRange("Bob", "C", QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::ExcludeUpper);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 2);
    dumpContactDifferences(cm->contact(ids.at(0)), b);
    QCOMPARE(cm->contact(ids.at(0)), b);
    dumpContactDifferences(cm->contact(ids.at(1)), c);
    QCOMPARE(cm->contact(ids.at(1)), c);

    drf.setRange("Bob", "C", QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 2);
    dumpContactDifferences(cm->contact(ids.at(0)), b);
    QCOMPARE(cm->contact(ids.at(0)), b);
    dumpContactDifferences(cm->contact(ids.at(1)), c);
    QCOMPARE(cm->contact(ids.at(1)), c);

    drf.setRange("Bob", "C", QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::IncludeUpper);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 1);
    dumpContactDifferences(cm->contact(ids.at(0)), c);
    QCOMPARE(cm->contact(ids.at(0)), c);

    drf.setRange("Bob", "C", QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 1);
    dumpContactDifferences(cm->contact(ids.at(0)), c);
    QCOMPARE(cm->contact(ids.at(0)), c);

    // Check MatchContains with range (== invalid)
    drf.setMatchFlags(Qt::MatchContains);
    drf.setRange("A", "Bob");
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 0);

    // Check EndsWith with range
    // A == son, B == sen, C == sun
    drf.setMatchFlags(Qt::MatchEndsWith);
    drf.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldLast);
    drf.setRange("sen", "son");
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 1);
    dumpContactDifferences(cm->contact(ids.at(0)), b);
    QCOMPARE(cm->contact(ids.at(0)), b);

    drf.setRange("sen", "son", QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 2);
    dumpContactDifferences(cm->contact(ids.at(0)), a);
    QCOMPARE(cm->contact(ids.at(0)), a);
    dumpContactDifferences(cm->contact(ids.at(1)), b);
    QCOMPARE(cm->contact(ids.at(1)), b);

    drf.setRange("sen", "son", QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::IncludeUpper);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 1);
    dumpContactDifferences(cm->contact(ids.at(0)), a);
    QCOMPARE(cm->contact(ids.at(0)), a);

    drf.setRange("sen", "son", QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::ExcludeUpper);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 1);
    dumpContactDifferences(cm->contact(ids.at(0)), b);
    QCOMPARE(cm->contact(ids.at(0)), b);

    drf.setRange("sen", "son", QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 0);

    drf.setRange("sen", "sun", QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 1);
    dumpContactDifferences(cm->contact(ids.at(0)), a);
    QCOMPARE(cm->contact(ids.at(0)), a);

    /* Now try some integer range testing */
    drf.setDetailDefinitionName("Integer", "value");
    drf.setMatchFlags(0);

    drf.setRange(9, 9);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 0);

    drf.setRange(9, 10);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 0);

    drf.setRange(9, 11);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 1);
    dumpContactDifferences(cm->contact(ids.at(0)), a);
    QCOMPARE(cm->contact(ids.at(0)), a);

    drf.setRange(10, 10);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 0);

    drf.setRange(10, 10, QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::ExcludeUpper);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 0);

    drf.setRange(10, 10, QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::IncludeUpper);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 0);

    drf.setRange(10, 10, QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 0);

    drf.setRange(10, 10, QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 1);
    dumpContactDifferences(cm->contact(ids.at(0)), a);
    QCOMPARE(cm->contact(ids.at(0)), a);

    drf.setRange(10, 11);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 1);
    dumpContactDifferences(cm->contact(ids.at(0)), a);
    QCOMPARE(cm->contact(ids.at(0)), a);

    drf.setRange(11, 11);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 0);

    drf.setRange(-30, -19);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 1);
    dumpContactDifferences(cm->contact(ids.at(0)), c);
    QCOMPARE(cm->contact(ids.at(0)), c);

    drf.setRange(-20, -30);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 0);

    drf.setRange(9, QVariant());
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 2);
    dumpContactDifferences(cm->contact(ids.at(0)), a);
    QCOMPARE(cm->contact(ids.at(0)), a);
    dumpContactDifferences(cm->contact(ids.at(1)), b);
    QCOMPARE(cm->contact(ids.at(1)), b);

    drf.setRange(QVariant(), 11);
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 2);
    dumpContactDifferences(cm->contact(ids.at(0)), a);
    QCOMPARE(cm->contact(ids.at(0)), a);
    dumpContactDifferences(cm->contact(ids.at(1)), c);
    QCOMPARE(cm->contact(ids.at(1)), c);


    delete cm;
}

void tst_QContactManagerFiltering::sorting()
{
    QContactManager* cm = new QContactManager("memory");

    /* Make sure it's empty */
    QList<QUniqueId> ids = cm->contacts();
    cm->removeContacts(&ids);

    /* Register an int detail */
    QContactDetailDefinition def;
    QMap<QString, QContactDetailDefinition::Field> fields;
    QContactDetailDefinition::Field field;
    def.setId("Integer");
    field.dataType = QVariant::Int;
    fields.insert("value", field);
    def.setFields(fields);

    QVERIFY(cm->saveDetailDefinition(def));

    def.setId("DateTime");
    fields.clear();
    field.dataType = QVariant::DateTime;
    fields.insert("value", field);
    def.setFields(fields);

    QVERIFY(cm->saveDetailDefinition(def));

    /* add some contacts */
    QContact a, b, c;
    QContactName aName, bName, cName;
    aName.setFirst("Aaron");
    aName.setLast("Aaronson");
    a.saveDetail(&aName);
    QContactPhoneNumber number;
    number.setNumber("555-1212");
    a.saveDetail(&number);

    QContactDetail integer("Integer");
    integer.setValue("value", 10);
    a.saveDetail(&integer);

    QContactDetail datetime("DateTime");
    datetime.setValue("value", QDateTime(QDate(2009, 06, 29), QTime(16, 52, 23, 0)));
    a.saveDetail(&datetime);

    bName.setFirst("Bob");
    bName.setLast("Aaronsen");
    b.saveDetail(&bName);
    number.setNumber("555-3456");
    b.saveDetail(&number);

    integer.setValue("value", 20);
    b.saveDetail(&integer);

    cName.setFirst("Boris");
    cName.setLast("Aaronsun");
    c.saveDetail(&cName);

    datetime.setValue("value", QDateTime(QDate(2009, 06, 29), QTime(16, 54, 17, 0)));
    c.saveDetail(&datetime);

    QVERIFY(cm->saveContact(&a));
    QVERIFY(cm->saveContact(&b));
    QVERIFY(cm->saveContact(&c));

    QCOMPARE(cm->contacts().count(), 3);

    /* Reload the contacts to pick up any changes */
    a = cm->contact(a.id());
    b = cm->contact(b.id());
    c = cm->contact(c.id());

    /* Now try some different sort orders */
    QContactSortOrder so;
    so.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirst);

    /* First ascending */
    ids = cm->contacts(so);
    QVERIFY(ids.count() == 3);
    QVERIFY(ids[0] == a.id());
    QVERIFY(ids[1] == b.id());
    QVERIFY(ids[2] == c.id());

    so.setDirection(Qt::DescendingOrder);

    /* First descending */
    ids = cm->contacts(so);
    QVERIFY(ids.count() == 3);
    QVERIFY(ids[0] == c.id());
    QVERIFY(ids[1] == b.id());
    QVERIFY(ids[2] == a.id());

    /* Last descending */
    so.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldLast);
    ids = cm->contacts(so);
    QVERIFY(ids.count() == 3);
    QVERIFY(ids[0] == c.id());
    QVERIFY(ids[1] == a.id());
    QVERIFY(ids[2] == b.id());

    /* Last ascending */
    so.setDirection(Qt::AscendingOrder);
    ids = cm->contacts(so);
    QVERIFY(ids.count() == 3);
    QVERIFY(ids[0] == b.id());
    QVERIFY(ids[1] == a.id());
    QVERIFY(ids[2] == c.id());

    /* Integer ascending, blanks last */
    so.setDetailDefinitionName("Integer", "value");
    ids = cm->contacts(so);
    QVERIFY(ids.count() == 3);
    QVERIFY(ids[0] == a.id());
    QVERIFY(ids[1] == b.id());
    QVERIFY(ids[2] == c.id());

    /* Descending, blanks last */
    so.setDirection(Qt::DescendingOrder);
    ids = cm->contacts(so);
    QVERIFY(ids.count() == 3);
    QVERIFY(ids[0] == b.id());
    QVERIFY(ids[1] == a.id());
    QVERIFY(ids[2] == c.id());

    /* Descending, blanks first */
    so.setBlankPolicy(QContactSortOrder::BlanksFirst);
    ids = cm->contacts(so);
    QVERIFY(ids.count() == 3);
    QVERIFY(ids[0] == c.id());
    QVERIFY(ids[1] == b.id());
    QVERIFY(ids[2] == a.id());

    /* Ascending, blanks first */
    so.setDirection(Qt::AscendingOrder);
    ids = cm->contacts(so);
    QVERIFY(ids.count() == 3);
    QVERIFY(ids[0] == c.id());
    QVERIFY(ids[1] == a.id());
    QVERIFY(ids[2] == b.id());

    delete cm;
}

QTEST_MAIN(tst_QContactManagerFiltering)
#include "tst_qcontactmanagerfiltering.moc"
