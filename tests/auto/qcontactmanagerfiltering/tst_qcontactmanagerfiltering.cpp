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

    QList<QContact> prepareModel(QContactManager* cm); // add the standard contacts
public slots:
    void init();
    void cleanup();

private slots:
    void rangeFiltering(); // XXX should take all managers
    void detailStringFiltering(); // XXX should take all managers

    void detailVariantFiltering();
    void detailVariantFiltering_data();

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
}

void tst_QContactManagerFiltering::cleanup()
{
}

void tst_QContactManagerFiltering::detailStringFiltering()
{
    /* Try the memory database first */
    QContactManager* cm = new QContactManager("memory");

    QList<QContact> contacts = prepareModel(cm);
    QList<QUniqueId> ids;

    QVERIFY(contacts.count() == 4);
    QContact a = contacts.at(0);
    QContact b = contacts.at(1);
    QContact c = contacts.at(2);
    QContact d = contacts.at(3);

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
}

Q_DECLARE_METATYPE(QVariant);

void tst_QContactManagerFiltering::detailVariantFiltering_data()
{
    QTest::addColumn<QString>("defname");
    QTest::addColumn<QString>("fieldname");
    QTest::addColumn<bool>("setValue");
    QTest::addColumn<QVariant>("value");
    QTest::addColumn<QString>("expected");

    QVariant ev; // empty variant
    QString es; // empty string

    /* Nothings */
    QTest::newRow("no name") << es << es << false << ev << es;
    QTest::newRow("no def name") << es << "value" << false << ev << es;

    /* Doubles */
    QTest::newRow("double presence") << "Double" << es << false << ev << "bcd";
    QTest::newRow("double presence (inc field)") << "Double" << "value" << false << ev << "bcd";
    QTest::newRow("double presence (wrong field)") << "Double" << "Trouble" << false << ev << es;

    QTest::newRow("double value (no match)") << "Double" << "value" << true << QVariant(3.5) << es;
    QTest::newRow("double value (wrong type)") << "Double" << "value" << true << QVariant(QDateTime()) << es;
//    QTest::newRow("double value (string type)") << "Double" << "value" << true << QVariant("4.0") << es;
    QTest::newRow("double value (wrong field, no match)") << "Double" << "Trouble" << true << QVariant(3.5) << es;
    QTest::newRow("double value") << "Double" << "value" << true << QVariant(4.0) << "bc";
    QTest::newRow("double value (wrong field)") << "Double" << "Trouble" << true << QVariant(4.0) << es;
    QTest::newRow("double value 2") << "Double" << "value" << true << QVariant(-128.0) << "d";
    QTest::newRow("double value 2 (wrong field)") << "Double" << "Trouble" << true << QVariant(-128.0) << es;

    /*
     * Integers
     * A has 10
     * B has 20
     * C has -20
     */
    QTest::newRow("integer presence") << "Integer" << es << false << ev << "abc";
    QTest::newRow("integer presence (inc field)") << "Integer" << "value" << false << ev << "abc";
    QTest::newRow("integer presence (wrong field)") << "Integer" << "Trouble" << false << ev << es;

    QTest::newRow("integer value (no match)") << "Integer" << "value" << true << QVariant(50) << es;
    QTest::newRow("integer value (wrong type)") << "Integer" << "value" << true << QVariant(3.5) << es;
//    QTest::newRow("integer value (string type)") << "Integer" << "value" << true << QVariant("20") << es;
    QTest::newRow("integer value (wrong field, no match)") << "Integer" << "Trouble" << true << QVariant(20) << es;
    QTest::newRow("integer value") << "Integer" << "value" << true << QVariant(10) << "a";
    QTest::newRow("integer value (wrong field)") << "Integer" << "Trouble" << true << QVariant(10) << es;
    QTest::newRow("integer value 2") << "Integer" << "value" << true << QVariant(-20) << "c";
    QTest::newRow("integer value 2 (wrong field)") << "Integer" << "Trouble" << true << QVariant(-20) << es;

    /*
     * Date times
     * A has QDateTime(QDate(2009, 06, 29), QTime(16, 52, 23, 0))
     * C has QDateTime(QDate(2009, 06, 29), QTime(16, 54, 17, 0))
     */
    const QDateTime adt(QDate(2009, 06, 29), QTime(16, 52, 23, 0));
    const QDateTime cdt(QDate(2009, 06, 29), QTime(16, 54, 17, 0));

    QTest::newRow("datetime presence") << "DateTime" << es << false << ev << "ac";
    QTest::newRow("datetime presence (inc field)") << "DateTime" << "value" << false << ev << "ac";
    QTest::newRow("datetime presence (wrong field)") << "DateTime" << "Trouble" << false << ev << es;

    QTest::newRow("datetime value (no match)") << "DateTime" << "value" << true << QVariant(QDateTime(QDate(2100,5,13), QTime(5,5,5))) << es;
    QTest::newRow("datetime value (wrong type)") << "DateTime" << "value" << true << QVariant(3.5) << es;
//    QTest::newRow("datetime value (string type)") << "DateTime" << "value" << true << QVariant(adt.toString(Qt::ISODate)) << es;
    QTest::newRow("datetime value (wrong field, no match)") << "DateTime" << "Trouble" << true << QVariant(adt) << es;
    QTest::newRow("datetime value") << "DateTime" << "value" << true << QVariant(adt) << "a";
    QTest::newRow("datetime value (wrong field)") << "DateTime" << "Trouble" << true << QVariant(adt) << es;
    QTest::newRow("datetime value 2") << "DateTime" << "value" << true << QVariant(cdt)<< "c";
    QTest::newRow("datetime value 2 (wrong field)") << "DateTime" << "Trouble" << true << QVariant(cdt) << es;


}

void tst_QContactManagerFiltering::detailVariantFiltering()
{
    QFETCH(QString, defname);
    QFETCH(QString, fieldname);
    QFETCH(bool, setValue);
    QFETCH(QVariant, value);
    QFETCH(QString, expected);

    /* Try the memory database first */
    QContactManager* cm = new QContactManager("memory");

    QList<QContact> contacts = prepareModel(cm);
    QList<QUniqueId> ids;

    QVERIFY(contacts.count() == 4);
    QContact a = contacts.at(0);
    QContact b = contacts.at(1);
    QContact c = contacts.at(2);
    QContact d = contacts.at(3);

    QContactDetailFilter df;
    df.setDetailDefinitionName(defname, fieldname);
    if (setValue)
        df.setValue(value);

    ids = cm->contacts(df);

    QCOMPARE(ids.count(), expected.count());
    QVERIFY(expected.count() <= contacts.count());

    /* Expected is of the form "abcd".. */
    for (int i = 0; i < expected.size(); i++) {
        QVERIFY(contacts.at(expected.at(i).toLower().toAscii() -'a').id() == ids.at(i));
    }
}

void tst_QContactManagerFiltering::rangeFiltering()
{
    QContactManager* cm = new QContactManager("memory");

    QList<QContact> contacts = prepareModel(cm);
    QList<QUniqueId> ids;

    QVERIFY(contacts.count() == 4);
    QContact a = contacts.at(0);
    QContact b = contacts.at(1);
    QContact c = contacts.at(2);
    QContact d = contacts.at(3);

    QContactDetailRangeFilter drf;
    drf.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);

    drf.setRange("555-1200", "555-1220");
    ids = cm->contacts(drf);
    QCOMPARE(ids.count(), 1);
    dumpContactDifferences(cm->contact(ids.at(0)), a);
    QCOMPARE(cm->contact(ids.at(0)), a);

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

    QList<QContact> contacts = prepareModel(cm);
    QList<QUniqueId> ids;

    QVERIFY(contacts.count() == 4);
    QContact a = contacts.at(0);
    QContact b = contacts.at(1);
    QContact c = contacts.at(2);
    QContact d = contacts.at(3);

    /* Now try some different sort orders */
    QContactSortOrder so;
    so.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirst);

    /* First ascending */
    ids = cm->contacts(so);
    QVERIFY(ids.count() == 4);
    QVERIFY(ids[0] == a.id());
    QVERIFY(ids[1] == b.id());
    QVERIFY(ids[2] == c.id());
    QVERIFY(ids[3] == d.id());

    so.setDirection(Qt::DescendingOrder);

    /* First descending */
    ids = cm->contacts(so);
    QVERIFY(ids.count() == 4);
    QVERIFY(ids[0] == d.id());
    QVERIFY(ids[1] == c.id());
    QVERIFY(ids[2] == b.id());
    QVERIFY(ids[3] == a.id());

    /* Last descending */
    so.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldLast);
    ids = cm->contacts(so);
    QVERIFY(ids.count() == 4);
    QVERIFY(ids[0] == d.id());
    QVERIFY(ids[1] == c.id());
    QVERIFY(ids[2] == a.id());
    QVERIFY(ids[3] == b.id());

    /* Last ascending */
    so.setDirection(Qt::AscendingOrder);
    ids = cm->contacts(so);
    QVERIFY(ids.count() == 4);
    QVERIFY(ids[0] == b.id());
    QVERIFY(ids[1] == a.id());
    QVERIFY(ids[2] == c.id());
    QVERIFY(ids[3] == d.id());

    /* Integer ascending, blanks last */
    so.setDetailDefinitionName("Integer", "value");
    ids = cm->contacts(so);
    QVERIFY(ids.count() == 4);
    QVERIFY(ids[0] == c.id());
    QVERIFY(ids[1] == a.id());
    QVERIFY(ids[2] == b.id());
    QVERIFY(ids[3] == d.id());

    /* Descending, blanks last */
    so.setDirection(Qt::DescendingOrder);
    ids = cm->contacts(so);
    QVERIFY(ids.count() == 4);
    QVERIFY(ids[0] == b.id());
    QVERIFY(ids[1] == a.id());
    QVERIFY(ids[2] == c.id());
    QVERIFY(ids[3] == d.id());

    /* Descending, blanks first */
    so.setBlankPolicy(QContactSortOrder::BlanksFirst);
    ids = cm->contacts(so);
    QVERIFY(ids.count() == 4);
    QVERIFY(ids[0] == d.id());
    QVERIFY(ids[1] == b.id());
    QVERIFY(ids[2] == a.id());
    QVERIFY(ids[3] == c.id());

    /* Ascending, blanks first */
    so.setDirection(Qt::AscendingOrder);
    ids = cm->contacts(so);
    QVERIFY(ids.count() == 4);
    QVERIFY(ids[0] == d.id());
    QVERIFY(ids[1] == c.id());
    QVERIFY(ids[2] == a.id());
    QVERIFY(ids[3] == b.id());

    /* XXX Doubles.. there's a problem with no sub sort for stable results */

    delete cm;
}

QList<QContact> tst_QContactManagerFiltering::prepareModel(QContactManager *cm)
{
    /* Make sure it's empty */
    QList<QUniqueId> ids = cm->contacts();
    cm->removeContacts(&ids);

    /* Register details for testing */
    QContactDetailDefinition def;
    QMap<QString, QContactDetailDefinition::Field> fields;
    QContactDetailDefinition::Field field;

    /* Integer */
    def.setId("Integer");
    fields["value"].dataType = QVariant::Int;
    def.setFields(fields);
    Q_ASSERT(cm->saveDetailDefinition(def));

    /* Date time detail */
    def.setId("DateTime");
    fields["value"].dataType = QVariant::DateTime;
    def.setFields(fields);
    Q_ASSERT(cm->saveDetailDefinition(def));

    /* double detail */
    def.setId("Double");
    fields["value"].dataType = QVariant::Double;
    def.setFields(fields);
    Q_ASSERT(cm->saveDetailDefinition(def));

    /* bool */
    def.setId("Bool");
    fields["value"].dataType = QVariant::Bool;
    def.setFields(fields);
    Q_ASSERT(cm->saveDetailDefinition(def));

    /* long long */
    def.setId("LongLong");
    fields["value"].dataType = QVariant::LongLong;
    def.setFields(fields);
    Q_ASSERT(cm->saveDetailDefinition(def));

    /* unsigned long long */
    def.setId("ULongLong");
    fields["value"].dataType = QVariant::ULongLong;
    def.setFields(fields);
    Q_ASSERT(cm->saveDetailDefinition(def));

    /* date */
    def.setId("Date");
    fields["value"].dataType = QVariant::Date;
    def.setFields(fields);
    Q_ASSERT(cm->saveDetailDefinition(def));

    /* time */
    def.setId("Time");
    fields["value"].dataType = QVariant::Time;
    def.setFields(fields);
    Q_ASSERT(cm->saveDetailDefinition(def));

    /* uint */
    def.setId("UInt");
    fields["value"].dataType = QVariant::UInt;
    def.setFields(fields);
    Q_ASSERT(cm->saveDetailDefinition(def));

    /* char */
    def.setId("Char");
    fields["value"].dataType = QVariant::Char;
    def.setFields(fields);
    Q_ASSERT(cm->saveDetailDefinition(def));

    /* Add some contacts */
    QContact a, b, c, d;
    QContactName name;;
    QContactPhoneNumber number;
    QContactDetail integer("Integer");
    QContactDetail datetime("DateTime");
    QContactDetail dubble("Double");
    QContactDetail boool("Bool");
    QContactDetail llong("LongLong");
    QContactDetail ullong("ULongLong");
    QContactDetail date("Date");
    QContactDetail time("Time");
    QContactDetail uintt("UInt");
    QContactDetail charr("Char");

    name.setFirst("Aaron");
    name.setLast("Aaronson");
    number.setNumber("555-1212");
    integer.setValue("value", 10);
    datetime.setValue("value", QDateTime(QDate(2009, 06, 29), QTime(16, 52, 23, 0)));
    boool.setValue("value", true);
    ullong.setValue("value", (qulonglong)120000000000LL); // 120B
    date.setValue("value", QDate(1988, 1, 26));

    a.saveDetail(&name);
    a.saveDetail(&number);
    a.saveDetail(&integer);
    a.saveDetail(&datetime);
    a.saveDetail(&boool);
    a.saveDetail(&ullong);
    //a.saveDetail(&date);

    name.setFirst("Bob");
    name.setLast("Aaronsen");
    number.setNumber("555-3456");
    integer.setValue("value", 20);
    dubble.setValue("value", 4.0);
    boool.setValue("value", false);
    ullong.setValue("value", (qulonglong) 80000000000LL); // 80B
    date.setValue("value", QDate(1492, 5, 5));

    b.saveDetail(&name);
    b.saveDetail(&number);
    b.saveDetail(&integer);
    b.saveDetail(&dubble);
    b.saveDetail(&boool);
    b.saveDetail(&ullong);
    b.saveDetail(&date);

    name.setFirst("Boris");
    name.setLast("Aaronsun");
    integer.setValue("value", -20);
    datetime.setValue("value", QDateTime(QDate(2009, 06, 29), QTime(16, 54, 17, 0)));
    llong.setValue("value", (qlonglong)8000000000LL); // 8B

    c.saveDetail(&name);
    c.saveDetail(&integer);
    c.saveDetail(&datetime);
    c.saveDetail(&dubble);
    c.saveDetail(&boool);
    c.saveDetail(&llong);
    c.saveDetail(&ullong);

    name.setFirst("Dennis");
    name.setLast("FitzMacintyre");
    dubble.setValue("value", -128.0);
    llong.setValue("value", (qlonglong)-14000000000LL);
    date.setValue("value", QDate(1770, 10, 1));

    d.saveDetail(&name);
    d.saveDetail(&dubble);
    d.saveDetail(&llong);
    d.saveDetail(&date);

    Q_ASSERT(cm->saveContact(&a));
    Q_ASSERT(cm->saveContact(&b));
    Q_ASSERT(cm->saveContact(&c));
    Q_ASSERT(cm->saveContact(&d));

    Q_ASSERT(cm->contacts().count() == 4);

    /* Reload the contacts to pick up any changes */
    a = cm->contact(a.id());
    b = cm->contact(b.id());
    c = cm->contact(c.id());
    d = cm->contact(d.id());

    QList<QContact> list;
    if (!a.isEmpty())
        list << a;
    if (!b.isEmpty())
        list << b;
    if (!c.isEmpty())
        list << c;
    if (!d.isEmpty())
        list << d;
    return list;
}

/* ============ Utility functions ============= */

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





QTEST_MAIN(tst_QContactManagerFiltering)
#include "tst_qcontactmanagerfiltering.moc"
