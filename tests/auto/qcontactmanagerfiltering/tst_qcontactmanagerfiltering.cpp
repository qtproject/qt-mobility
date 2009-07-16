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

#define QT_STATICPLUGIN
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

    QList<QUniqueId> prepareModel(QContactManager* cm); // add the standard contacts

    QString convertIds(QList<QUniqueId> allIds, QList<QUniqueId> ids); // convert back to "abcd"
public slots:
    void init();
    void cleanup();

private slots:
    void rangeFiltering(); // XXX should take all managers
    void rangeFiltering_data();

    void detailStringFiltering(); // XXX should take all managers

    void actionPlugins();
    void actionFiltering();
    void actionFiltering_data();

    void detailVariantFiltering();
    void detailVariantFiltering_data();

    void groupMembershipFiltering();
    void groupMembershipFiltering_data();

    void sorting(); // XXX should take all managers
    void sorting_data();
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

    QList<QUniqueId> contacts = prepareModel(cm);
    QList<QUniqueId> ids;

    QVERIFY(contacts.count() == 4);
    QContact a = cm->contact(contacts.at(0));
    QContact b = cm->contact(contacts.at(1));
    QContact c = cm->contact(contacts.at(2));
    QContact d = cm->contact(contacts.at(3));

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

    delete cm;
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

    /*
     * Doubles
     * B has double(4.0)
     * C has double(4.0)
     * D has double(-128.0)
     */
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
    QTest::newRow("integer value (wrong field, no match)") << "Integer" << "Trouble" << true << QVariant(50) << es;
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
    QTest::newRow("datetime value (wrong field, no match)") << "DateTime" << "Trouble" << true << QVariant(QDateTime(QDate(2100,5,13), QTime(5,5,5))) << es;
    QTest::newRow("datetime value") << "DateTime" << "value" << true << QVariant(adt) << "a";
    QTest::newRow("datetime value (wrong field)") << "DateTime" << "Trouble" << true << QVariant(adt) << es;
    QTest::newRow("datetime value 2") << "DateTime" << "value" << true << QVariant(cdt)<< "c";
    QTest::newRow("datetime value 2 (wrong field)") << "DateTime" << "Trouble" << true << QVariant(cdt) << es;

    /*
     * Dates
     * A has QDate(1988, 1, 26)
     * B has QDate(1492, 5, 5)
     * D has QDate(1770, 10, 1)
     */
    const QDate ad(1988, 1, 26);
    const QDate bd(1492, 5, 5);
    const QDate dd(1770, 10, 1);
    QTest::newRow("date presence") << "Date" << es << false << ev << "abd";
    QTest::newRow("date presence (inc field)") << "Date" << "value" << false << ev << "abd";
    QTest::newRow("date presence (wrong field)") << "Date" << "Trouble" << false << ev << es;

    QTest::newRow("date value (no match)") << "Date" << "value" << true << QVariant(QDate(2100,5,13)) << es;
    QTest::newRow("date value (wrong type)") << "Date" << "value" << true << QVariant(3.5) << es;
//    QTest::newRow("date value (string type)") << "Date" << "value" << true << QVariant(ad.toString(Qt::ISODate)) << es;
    QTest::newRow("date value (wrong field, no match)") << "Date" << "Trouble" << true << QVariant(QDate(2100,5,13)) << es;
    QTest::newRow("date value") << "Date" << "value" << true << QVariant(ad) << "a";
    QTest::newRow("date value (wrong field)") << "Date" << "Trouble" << true << QVariant(ad) << es;
    QTest::newRow("date value 2") << "Date" << "value" << true << QVariant(bd)<< "b";
    QTest::newRow("date value 2 (wrong field)") << "Date" << "Trouble" << true << QVariant(bd) << es;
    QTest::newRow("date value 3") << "Date" << "value" << true << QVariant(dd)<< "d";
    QTest::newRow("date value 3 (wrong field)") << "Date" << "Trouble" << true << QVariant(dd) << es;


    /*
     * Times
     * A has QTime(16,52,23,0)
     * B has QTime(15,52,23,0)
     */
    const QTime at = QTime(16,52,23,0);
    const QTime bt = QTime(15,52,23,0);
    QTest::newRow("time presence") << "Time" << es << false << ev << "ab";
    QTest::newRow("time presence (inc field)") << "Time" << "value" << false << ev << "ab";
    QTest::newRow("time presence (wrong field)") << "Time" << "Trouble" << false << ev << es;

    QTest::newRow("time value (no match)") << "Time" << "value" << true << QVariant(QTime(5,5,5)) << es;
    QTest::newRow("time value (wrong type)") << "Time" << "value" << true << QVariant(3.5) << es;
//    QTest::newRow("time value (string type)") << "Time" << "value" << true << QVariant(at.toString(Qt::ISOTime)) << es;
    QTest::newRow("time value (wrong field, no match)") << "Time" << "Trouble" << true << QVariant(QTime(5,5,5)) << es;
    QTest::newRow("time value") << "Time" << "value" << true << QVariant(at) << "a";
    QTest::newRow("time value (wrong field)") << "Time" << "Trouble" << true << QVariant(at) << es;
    QTest::newRow("time value 2") << "Time" << "value" << true << QVariant(bt)<< "b";
    QTest::newRow("time value 2 (wrong field)") << "Time" << "Trouble" << true << QVariant(bt) << es;


    /*
     * Bool
     * A has bool(true)
     * B has bool(false)
     * C has bool(false)
     */
    QTest::newRow("bool presence") << "Bool" << es << false << ev << "abc";
    QTest::newRow("bool presence (inc field)") << "Bool" << "value" << false << ev << "abc";
    QTest::newRow("bool presence (wrong field)") << "Bool" << "Trouble" << false << ev << es;

    //QTest::newRow("bool value (no match)") << "Bool" << "value" << true << QVariant(false) << es;// we have both possible
    QTest::newRow("bool value (wrong type)") << "Bool" << "value" << true << QVariant(4.0) << es;
//    QTest::newRow("bool value (string type)") << "Bool" << "value" << true << QVariant("4.0") << es;
    //QTest::newRow("bool value (wrong field, no match)") << "Bool" << "Trouble" << true << QVariant(false) << es;// ditto
    QTest::newRow("bool value") << "Bool" << "value" << true << QVariant(true) << "a";
    QTest::newRow("bool value (wrong field)") << "Bool" << "Trouble" << true << QVariant(true) << es;
    QTest::newRow("bool value 2") << "Bool" << "value" << true << QVariant(false) << "bc";
    QTest::newRow("bool value 2 (wrong field)") << "Bool" << "Trouble" << true << QVariant(false) << es;

    /*
     * LongLong
     * C has LongLong(8000000000LL)
     * D has LongLong(-14000000000LL)
     */
    QTest::newRow("longlong presence") << "LongLong" << es << false << ev << "cd";
    QTest::newRow("longlong presence (inc field)") << "LongLong" << "value" << false << ev << "cd";
    QTest::newRow("longlong presence (wrong field)") << "LongLong" << "Trouble" << false << ev << es;

    QTest::newRow("longlong value (no match)") << "LongLong" << "value" << true << QVariant(50000000000LL) << es;
    QTest::newRow("longlong value (wrong type)") << "LongLong" << "value" << true << QVariant(3.5) << es;
//    QTest::newRow("longlong value (string type)") << "LongLong" << "value" << true << QVariant("20") << es;
    QTest::newRow("longlong value (wrong field, no match)") << "LongLong" << "Trouble" << true << QVariant(50000000000LL) << es;
    QTest::newRow("longlong value") << "LongLong" << "value" << true << QVariant(8000000000LL) << "c";
    QTest::newRow("longlong value (wrong field)") << "LongLong" << "Trouble" << true << QVariant(8000000000LL) << es;
    QTest::newRow("longlong value 2") << "LongLong" << "value" << true << QVariant(-14000000000LL) << "d";
    QTest::newRow("longlong value 2 (wrong field)") << "LongLong" << "Trouble" << true << QVariant(-14000000000LL) << es;

    /*
     * ULongLong
     * A has ULongLong(120000000000ULL)
     * B has ULongLong(80000000000ULL)
     * C has ULongLong(80000000000ULL)
     */
        QTest::newRow("ulonglong presence") << "ULongLong" << es << false << ev << "abc";
    QTest::newRow("ulonglong presence (inc field)") << "ULongLong" << "value" << false << ev << "abc";
    QTest::newRow("ulonglong presence (wrong field)") << "ULongLong" << "Trouble" << false << ev << es;

    QTest::newRow("ulonglong value (no match)") << "ULongLong" << "value" << true << QVariant(50000000000ULL) << es;
    QTest::newRow("ulonglong value (wrong type)") << "ULongLong" << "value" << true << QVariant(3.5) << es;
//    QTest::newRow("ulonglong value (string type)") << "ULongLong" << "value" << true << QVariant("20") << es;
    QTest::newRow("ulonglong value (wrong field, no match)") << "ULongLong" << "Trouble" << true << QVariant(50000000000ULL) << es;
    QTest::newRow("ulonglong value") << "ULongLong" << "value" << true << QVariant(120000000000ULL) << "a";
    QTest::newRow("ulonglong value (wrong field)") << "ULongLong" << "Trouble" << true << QVariant(120000000000ULL) << es;
    QTest::newRow("ulonglong value 2") << "ULongLong" << "value" << true << QVariant(80000000000ULL) << "bc";
    QTest::newRow("ulonglong value 2 (wrong field)") << "ULongLong" << "Trouble" << true << QVariant(80000000000ULL) << es;

    /*
     * UInt
     * B has UInt(4000000000u)
     * D has UInt(3000000000u)
     */
    QTest::newRow("unsigned integer presence") << "UInt" << es << false << ev << "bd";
    QTest::newRow("unsigned integer presence (inc field)") << "UInt" << "value" << false << ev << "bd";
    QTest::newRow("unsigned integer presence (wrong field)") << "UInt" << "Trouble" << false << ev << es;

    QTest::newRow("unsigned integer value (no match)") << "UInt" << "value" << true << QVariant(3500000000u) << es;
    QTest::newRow("unsigned integer value (wrong type)") << "UInt" << "value" << true << QVariant(3.5) << es;
//    QTest::newRow("unsigned integer value (string type)") << "UInt" << "value" << true << QVariant("20") << es;
    QTest::newRow("unsigned integer value (wrong field, no match)") << "UInt" << "Trouble" << true << QVariant(3500000000u) << es;
    QTest::newRow("unsigned integer value") << "UInt" << "value" << true << QVariant(4000000000u) << "b";
    QTest::newRow("unsigned integer value (wrong field)") << "UInt" << "Trouble" << true << QVariant(4000000000u) << es;
    QTest::newRow("unsigned integer value 2") << "UInt" << "value" << true << QVariant(3000000000u) << "d";
    QTest::newRow("unsigned integer value 2 (wrong field)") << "UInt" << "Trouble" << true << QVariant(3000000000u) << es;

    /*
     * Char
     * B has QChar('b')
     * C has QChar('c')
     */
    const QChar bchar('b');
    const QChar cchar('c');
    QTest::newRow("char presence") << "Char" << es << false << ev << "bc";
    QTest::newRow("char presence (inc field)") << "Char" << "value" << false << ev << "bc";
    QTest::newRow("char presence (wrong field)") << "Char" << "Trouble" << false << ev << es;

    QTest::newRow("char value (no match)") << "Char" << "value" << true << QVariant(QChar('a')) << es;
    QTest::newRow("char value (wrong type)") << "Char" << "value" << true << QVariant(3.5) << es;
//    QTest::newRow("char value (string type)") << "Char" << "value" << true << QVariant(at.toString(Qt::ISOTime)) << es;
    QTest::newRow("char value (wrong field, no match)") << "Char" << "Trouble" << true << QVariant(QChar('a')) << es;
    QTest::newRow("char value") << "Char" << "value" << true << QVariant(bchar) << "b";
    QTest::newRow("char value (wrong field)") << "Char" << "Trouble" << true << QVariant(bchar) << es;
    QTest::newRow("char value 2") << "Char" << "value" << true << QVariant(cchar)<< "c";
    QTest::newRow("char value 2 (wrong field)") << "Char" << "Trouble" << true << QVariant(cchar) << es;
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

    QList<QUniqueId> contacts = prepareModel(cm);
    QList<QUniqueId> ids;

    QVERIFY(contacts.count() == 4);

    QContactDetailFilter df;
    df.setDetailDefinitionName(defname, fieldname);
    if (setValue)
        df.setValue(value);

    /* At this point, since we're using memory, assume the filter isn't really supported */
    QVERIFY(cm->information()->filterSupported(df) == false);

    ids = cm->contacts(df);

    QString output = convertIds(contacts, ids);
    QCOMPARE(output, expected);

    delete cm;
}

QString tst_QContactManagerFiltering::convertIds(QList<QUniqueId> allIds, QList<QUniqueId> ids)
{
    QString ret;
    /* Expected is of the form "abcd".. */
    for (int i = 0; i < ids.size(); i++) {
        ret += ('a' + allIds.indexOf(ids.at(i)));
    }

    return ret;
}

void tst_QContactManagerFiltering::rangeFiltering_data()
{
    QTest::addColumn<QString>("defname");
    QTest::addColumn<QString>("fieldname");
    QTest::addColumn<QVariant>("minrange");
    QTest::addColumn<QVariant>("maxrange");
    QTest::addColumn<bool>("setrfs");
    QTest::addColumn<int>("rangeflagsi");
    QTest::addColumn<bool>("setmfs");
    QTest::addColumn<int>("matchflagsi");
    QTest::addColumn<QString>("expected");

    QVariant ev; // empty variant
    QString es; // empty string

    /* First, cover the "empty defname / fieldname / ranges" cases */
    QTest::newRow("invalid defname") << es << QContactName::FieldFirst << QVariant("A") << QVariant("Bob") << false << 0 << true << 0 << es;
    QTest::newRow("invalid fieldname") << QContactName::DefinitionName << es << QVariant("A") << QVariant("Bob") << false << 0 << true << 0 << es;
    QTest::newRow("invalid ranges") << QContactName::DefinitionName << QContactName::FieldFirst << QVariant() << QVariant() << false << 0 << true << 0 << es;
    QTest::newRow("invalid ranges") << QContactName::DefinitionName << QContactName::FieldFirst << QVariant("A") << QVariant() << false << 0 << true << 0 << "abcd";
    QTest::newRow("invalid ranges") << QContactName::DefinitionName << QContactName::FieldFirst << QVariant() << QVariant("Bob") << false << 0 << true << 0 << "a";

    /* 'a' has phone number ("555-1212") */
    QTest::newRow("range1") << QContactPhoneNumber::DefinitionName << QContactPhoneNumber::FieldNumber << QVariant("555-1200") << QVariant("555-1220") << false << 0 << false << 0 << "a";

    /* A(Aaron Aaronson), B(Bob Aaronsen), C(Boris Aaronsun), D(Dennis FitzMacyntire) */
    // string range matching - no matchflags set.
    QTest::newRow("string range - no matchflags - 1") << QContactName::DefinitionName << QContactName::FieldFirst << QVariant("A") << QVariant("Bob") << false << 0 << true << 0 << "a";
    QTest::newRow("string range - no matchflags - 2") << QContactName::DefinitionName << QContactName::FieldFirst << QVariant("A") << QVariant("Bob") << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << 0 << "a";
    QTest::newRow("string range - no matchflags - 3") << QContactName::DefinitionName << QContactName::FieldFirst << QVariant("A") << QVariant("Bob") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << 0 << "a";
    QTest::newRow("string range - no matchflags - 4") << QContactName::DefinitionName << QContactName::FieldFirst << QVariant("A") << QVariant("Bob") << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper) << true << 0 << "ab";
    QTest::newRow("string range - no matchflags - 5") << QContactName::DefinitionName << QContactName::FieldFirst << QVariant("A") << QVariant("Bob") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::IncludeUpper) << true << 0 << "ab";
    QTest::newRow("string range - no matchflags - 6") << QContactName::DefinitionName << QContactName::FieldFirst << QVariant("Bob") << QVariant("C") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::IncludeUpper) << true << 0 << "c";
    QTest::newRow("string range - no matchflags - 7") << QContactName::DefinitionName << QContactName::FieldFirst << QVariant("Bob") << QVariant("C") << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << 0 << "bc";
    QTest::newRow("string range - no matchflags - 8") << QContactName::DefinitionName << QContactName::FieldFirst << QVariant("Bob") << QVariant("C") << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper) << true << 0 << "bc";
    QTest::newRow("string range - no matchflags - 9") << QContactName::DefinitionName << QContactName::FieldFirst << QVariant("Bob") << QVariant("C") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << 0 << "c";
    QTest::newRow("string range - no matchflags - 10") << QContactName::DefinitionName << QContactName::FieldFirst << QVariant("Barry") << QVariant("C") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << 0 << "bc";

    // string range matching - Qt::MatchStartsWith should produce the same results as without matchflags set.
    QTest::newRow("string range - startswith - 1") << QContactName::DefinitionName << QContactName::FieldFirst << QVariant("A") << QVariant("Bob") << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchStartsWith) << "a";
    QTest::newRow("string range - startswith - 2") << QContactName::DefinitionName << QContactName::FieldFirst << QVariant("A") << QVariant("Bob") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchStartsWith) << "a";
    QTest::newRow("string range - startswith - 3") << QContactName::DefinitionName << QContactName::FieldFirst << QVariant("A") << QVariant("Bob") << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper) << true << (int)(Qt::MatchStartsWith) << "ab";
    QTest::newRow("string range - startswith - 4") << QContactName::DefinitionName << QContactName::FieldFirst << QVariant("A") << QVariant("Bob") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::IncludeUpper) << true << (int)(Qt::MatchStartsWith) << "ab";
    QTest::newRow("string range - startswith - 5") << QContactName::DefinitionName << QContactName::FieldFirst << QVariant("Bob") << QVariant("C") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::IncludeUpper) << true << (int)(Qt::MatchStartsWith) << "c";
    QTest::newRow("string range - startswith - 6") << QContactName::DefinitionName << QContactName::FieldFirst << QVariant("Bob") << QVariant("C") << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchStartsWith) << "bc";
    QTest::newRow("string range - startswith - 7") << QContactName::DefinitionName << QContactName::FieldFirst << QVariant("Bob") << QVariant("C") << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper) << true << (int)(Qt::MatchStartsWith) << "bc";
    QTest::newRow("string range - startswith - 8") << QContactName::DefinitionName << QContactName::FieldFirst << QVariant("Bob") << QVariant("C") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchStartsWith) << "c";
    QTest::newRow("string range - startswith - 9") << QContactName::DefinitionName << QContactName::FieldFirst << QVariant("Barry") << QVariant("C") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchStartsWith) << "bc";

    // Qt::MatchContains with range is invalid
    QTest::newRow("string range - contains - 1") << QContactName::DefinitionName << QContactName::FieldFirst << QVariant("A") << QVariant("Bob") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchContains) << es;

    // Check EndsWith with range: A == son, B == sen, C == sun
    QTest::newRow("string range - endswith - 1") << QContactName::DefinitionName << QContactName::FieldLast << QVariant("sen") << QVariant("son") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchEndsWith) << es;
    QTest::newRow("string range - endswith - 2") << QContactName::DefinitionName << QContactName::FieldLast << QVariant("sen") << QVariant("son") << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchEndsWith) << "b";
    QTest::newRow("string range - endswith - 3") << QContactName::DefinitionName << QContactName::FieldLast << QVariant("sen") << QVariant("son") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::IncludeUpper) << true << (int)(Qt::MatchEndsWith) << "a";
    QTest::newRow("string range - endswith - 4") << QContactName::DefinitionName << QContactName::FieldLast << QVariant("sen") << QVariant("son") << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper) << true << (int)(Qt::MatchEndsWith) << "ab";
    QTest::newRow("string range - endswith - 5") << QContactName::DefinitionName << QContactName::FieldLast << QVariant("sen") << QVariant("sun") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::IncludeUpper) << true << (int)(Qt::MatchEndsWith) << "ac";
    QTest::newRow("string range - endswith - 6") << QContactName::DefinitionName << QContactName::FieldLast << QVariant("sen") << QVariant("sun") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchEndsWith) << "a";

    /* A(10), B(20), C(-20) */
    // Now integer range testing
    QTest::newRow("int range - no rangeflags - 1") << "Integer" << "value" << QVariant(9) << QVariant(9) << false << 0 << false << 0 << es;
    QTest::newRow("int range - no rangeflags - 2") << "Integer" << "value" << QVariant(9) << QVariant(10) << false << 0 << false << 0 << es;
    QTest::newRow("int range - no rangeflags - 3") << "Integer" << "value" << QVariant(9) << QVariant(11) << false << 0 << false << 0 << "a";
    QTest::newRow("int range - no rangeflags - 4") << "Integer" << "value" << QVariant(10) << QVariant(10) << false << 0 << false << 0 << es;
    QTest::newRow("int range - rangeflags - 1") << "Integer" << "value" << QVariant(10) << QVariant(10) << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper) << false << 0 << es;
    QTest::newRow("int range - rangeflags - 2") << "Integer" << "value" << QVariant(10) << QVariant(10) << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::ExcludeUpper) << false << 0 << es;
    QTest::newRow("int range - rangeflags - 3") << "Integer" << "value" << QVariant(10) << QVariant(10) << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::IncludeUpper) << false << 0 << es;
    QTest::newRow("int range - rangeflags - 4") << "Integer" << "value" << QVariant(10) << QVariant(10) << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper) << false << 0 << "a";
    QTest::newRow("int range - rangeflags - 5") << "Integer" << "value" << QVariant(10) << QVariant(11) << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper) << false << 0 << "a";
    QTest::newRow("int range - rangeflags - 6") << "Integer" << "value" << QVariant(11) << QVariant(11) << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper) << false << 0 << es;
    QTest::newRow("int range - rangeflags - 7") << "Integer" << "value" << QVariant(-30) << QVariant(-19) << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper) << false << 0 << "c";
    QTest::newRow("int range - rangeflags - 8") << "Integer" << "value" << QVariant(-20) << QVariant(-30) << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper) << false << 0 << es;
    QTest::newRow("int range - rangeflags - variant - 1") << "Integer" << "value" << QVariant(9) << QVariant() << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper) << false << 0 << "ab";
    QTest::newRow("int range - rangeflags - variant - 2") << "Integer" << "value" << QVariant() << QVariant(11) << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper) << false << 0 << "ac";
}

void tst_QContactManagerFiltering::rangeFiltering()
{
    QFETCH(QString, defname);
    QFETCH(QString, fieldname);
    QFETCH(QVariant, minrange);
    QFETCH(QVariant, maxrange);
    QFETCH(bool, setrfs);
    QFETCH(int, rangeflagsi);
    QFETCH(bool, setmfs);
    QFETCH(int, matchflagsi);
    QFETCH(QString, expected);

    QContactDetailRangeFilter::RangeFlags rangeflags = (QContactDetailRangeFilter::RangeFlags)rangeflagsi;
    Qt::MatchFlags matchflags = (Qt::MatchFlags) matchflagsi;

    /* Try the memory database first */
    QContactManager* cm = new QContactManager("memory");

    QList<QUniqueId> contacts = prepareModel(cm);
    QList<QUniqueId> ids;

    QVERIFY(contacts.count() == 4);
    QContact a = cm->contact(contacts.at(0));
    QContact b = cm->contact(contacts.at(1));
    QContact c = cm->contact(contacts.at(2));
    QContact d = cm->contact(contacts.at(3));

    /* Build the range filter */
    QContactDetailRangeFilter drf;
    drf.setDetailDefinitionName(defname, fieldname);
    if (setrfs)
        drf.setRange(minrange, maxrange, rangeflags);
    else
        drf.setRange(minrange, maxrange);
    if (setmfs)
        drf.setMatchFlags(matchflags);

    /* At this point, since we're using memory, assume the filter isn't really supported */
    QVERIFY(cm->information()->filterSupported(drf) == false);

    ids = cm->contacts(drf);

    QCOMPARE(ids.count(), expected.count());
    QVERIFY(expected.count() <= contacts.count());

    /* Expected is of the form "abcd".. */
    for (int i = 0; i < expected.size(); i++) {
        QVERIFY(contacts.at(expected.at(i).toLower().toAscii() -'a').id() == ids.at(i));
    }

    delete cm;
}

void tst_QContactManagerFiltering::groupMembershipFiltering_data()
{
    QTest::addColumn<QString>("expectedone");
    QTest::addColumn<QString>("expectedtwo");

    QString es; // empty string.

    QTest::newRow("1") << "abcd" << es;
    QTest::newRow("2") << "abcd" << "abcd";
    QTest::newRow("3") << "ad" << "ab";
    QTest::newRow("4") << es << "c";
}

void tst_QContactManagerFiltering::groupMembershipFiltering()
{
    QFETCH(QString, expectedone);
    QFETCH(QString, expectedtwo);

    /* Try the memory database first */
    QContactManager* cm = new QContactManager("memory");

    QList<QContact> contacts = prepareModel(cm);
    QList<QUniqueId> idsone, idstwo;

    QVERIFY(contacts.count() == 4);
    QContact a = contacts.at(0);
    QContact b = contacts.at(1);
    QContact c = contacts.at(2);
    QContact d = contacts.at(3);

    QContactGroup g1, g2;
    g1.setName("GroupOne");
    g2.setName("GroupTwo");

    // add the specified members to the specified groups
    for (int i = 0; i < expectedone.size(); i++)
        g1.addMember(contacts.at(expectedone.at(i).toLower().toAscii() - 'a').id());
    for (int i = 0; i < expectedtwo.size(); i++)
        g2.addMember(contacts.at(expectedtwo.at(i).toLower().toAscii() - 'a').id());

    // save them to the manager.
    cm->saveGroup(&g1);
    cm->saveGroup(&g2);

    // build the group membership filters
    QContactGroupMembershipFilter cg1f, cg2f;
    cg1f.setGroupId(g1.id());
    cg2f.setGroupId(g2.id());

    /* At this point, since we're using memory, assume the filter isn't really supported */
    QVERIFY(cm->information()->filterSupported(cg1f) == false);

    idsone = cm->contacts(cg1f);
    idstwo = cm->contacts(cg2f);
    QCOMPARE(idsone.count(), expectedone.count());
    QCOMPARE(idstwo.count(), expectedtwo.count());
    QVERIFY(expectedone.count() <= contacts.count());
    QVERIFY(expectedtwo.count() <= contacts.count());

    /* Expected is of the form "abcd".. */
    for (int i = 0; i < expectedone.size(); i++)
        QVERIFY(contacts.at(expectedone.at(i).toLower().toAscii() -'a').id() == idsone.at(i));
    for (int i = 0; i < expectedtwo.size(); i++)
        QVERIFY(contacts.at(expectedtwo.at(i).toLower().toAscii() -'a').id() == idstwo.at(i));

    delete cm;
}

void tst_QContactManagerFiltering::sorting_data()
{
    QTest::addColumn<QString>("defname");
    QTest::addColumn<QString>("fieldname");
    QTest::addColumn<int>("directioni");
    QTest::addColumn<bool>("setbp");
    QTest::addColumn<int>("blankpolicyi");
    QTest::addColumn<QString>("expected");

    QTest::newRow("first ascending") << QContactName::DefinitionName << QContactName::FieldFirst << (int)(Qt::AscendingOrder) << false << 0 << "abcd";
    QTest::newRow("first descending") << QContactName::DefinitionName << QContactName::FieldFirst << (int)(Qt::DescendingOrder) << false << 0 << "dcba";
    QTest::newRow("last ascending") << QContactName::DefinitionName << QContactName::FieldLast << (int)(Qt::AscendingOrder) << false << 0 << "bacd";
    QTest::newRow("last descending") << QContactName::DefinitionName << QContactName::FieldLast << (int)(Qt::DescendingOrder) << false << 0 << "dcab";
    QTest::newRow("integer ascending, blanks last") << "Integer" << "value" << (int)(Qt::AscendingOrder) << true << (int)(QContactSortOrder::BlanksLast) << "cabd";
    QTest::newRow("integer descending, blanks last") << "Integer" << "value" << (int)(Qt::DescendingOrder) << true << (int)(QContactSortOrder::BlanksLast) << "bacd";
    QTest::newRow("integer ascending, blanks first") << "Integer" << "value" << (int)(Qt::AscendingOrder) << true << (int)(QContactSortOrder::BlanksFirst) << "dcab";
    QTest::newRow("integer descending, blanks first") << "Integer" << "value" << (int)(Qt::DescendingOrder) << true << (int)(QContactSortOrder::BlanksFirst) << "dbac";
}

void tst_QContactManagerFiltering::sorting()
{
    QFETCH(QString, defname);
    QFETCH(QString, fieldname);
    QFETCH(int, directioni);
    QFETCH(bool, setbp);
    QFETCH(int, blankpolicyi);
    QFETCH(QString, expected);

    Qt::SortOrder direction = (Qt::SortOrder)directioni;
    QContactSortOrder::BlankPolicy blankpolicy = (QContactSortOrder::BlankPolicy)blankpolicyi;

    /* Try the memory database first */
    QContactManager* cm = new QContactManager("memory");

    QList<QUniqueId> contacts = prepareModel(cm);
    QList<QUniqueId> ids;

    QVERIFY(contacts.count() == 4);
    QContact a = cm->contact(contacts.at(0));
    QContact b = cm->contact(contacts.at(1));
    QContact c = cm->contact(contacts.at(2));
    QContact d = cm->contact(contacts.at(3));

    /* Build the sort order */
    QContactSortOrder s;
    s.setDetailDefinitionName(defname, fieldname);
    s.setDirection(direction);
    if (setbp)
        s.setBlankPolicy(blankpolicy);

    ids = cm->contacts(s);
    QVERIFY(ids.count() == contacts.count());

    /* Expected is of the form "abcd".. */
    for (int i = 0; i < expected.size(); i++) {
        QVERIFY(contacts.at(expected.at(i).toLower().toAscii() -'a').id() == ids.at(i));
    }

    delete cm;
}

void tst_QContactManagerFiltering::actionPlugins()
{
    QStringList actions = QContactManager::availableActions();
    QVERIFY(actions.contains("Boolean"));
    QVERIFY(actions.contains("Number"));

    /* Ignore the version if the vendor is not set */
    actions = QContactManager::availableActions(QString(), 555);
    QVERIFY(actions.contains("Boolean"));
    QVERIFY(actions.contains("Number"));

    actions = QContactManager::availableActions("NumberCo");
    QVERIFY(actions.contains("Number"));
    QVERIFY(!actions.contains("Boolean"));

    actions = QContactManager::availableActions("IntegerCo");
    QVERIFY(actions.contains("Number"));
    QVERIFY(!actions.contains("Boolean"));

    actions = QContactManager::availableActions("BooleanCo");
    QVERIFY(!actions.contains("Number"));
    QVERIFY(actions.contains("Boolean"));

    actions = QContactManager::availableActions("IntegerCo", 5);
    QVERIFY(actions.contains("Number"));
    QVERIFY(!actions.contains("Boolean"));

    actions = QContactManager::availableActions("IntegerCo", 3);
    QVERIFY(!actions.contains("Number"));
    QVERIFY(!actions.contains("Boolean"));

    actions = QContactManager::availableActions("BooleanCo", 3);
    QVERIFY(!actions.contains("Number"));
    QVERIFY(actions.contains("Boolean"));

    actions = QContactManager::availableActions("BooleanCo", 555);
    QVERIFY(!actions.contains("Number"));
    QVERIFY(!actions.contains("Boolean"));
}

void tst_QContactManagerFiltering::actionFiltering_data()
{
    QTest::addColumn<QString>("actionName");
    QTest::addColumn<QString>("vendorName");
    QTest::addColumn<int>("version");
    QTest::addColumn<QVariant>("value");
    QTest::addColumn<QString>("expected");

    QString es;
    QVariant ev;

    QTest::newRow("empty (any action matches)") << es << es << -1 << ev << "abcd";
    QTest::newRow("bad actionname") << "No such action" << es << -1 << ev << es;
    QTest::newRow("bad vendor") << es << "Vendor missing" << -1 << ev << es;
    /* versions are ignored if vendors are not specified */
    QTest::newRow("ignored version") << es << es << 793434 << ev << "abcd";

    QTest::newRow("Number") << "Number" << es << -1 << ev << "abcd";
    QTest::newRow("Number (IntegerCo)") << "Number" << "IntegerCo" << -1 << ev << "abc";
    QTest::newRow("Number (NumberCo)") << "Number" << "NumberCo" << -1 << ev << "abcd";
    QTest::newRow("Number (BooleanCo)") << "Number" << "BooleanCo" << -1 << ev << es;

    QTest::newRow("Number (IntegerCo, good version)") << "Number" << "IntegerCo" << 5 << ev << "abc";
    QTest::newRow("Number (NumberCo, good version)") << "Number" << "NumberCo" << 42 << ev << "abcd";

    QTest::newRow("Number (IntegerCo, bad version)") << "Number" << "IntegerCo" << 345345 << ev << es;
    QTest::newRow("Number (NumberCo, bad version)") << "Number" << "NumberCo" << 7547544 << ev << es;

    /* versions are ignored if vendors are not specified */
    QTest::newRow("Number (ignored version)") << "Number" << es << 345345 << ev << "abcd";

    /* Vendor specific */
    QTest::newRow("NumberCo") << es << "NumberCo" << -1 << ev << "abcd";
    QTest::newRow("NumberCo (good version)") << es << "NumberCo" << 42 << ev << "abcd";
    QTest::newRow("NumberCo (bad version)") << es << "NumberCo" << 41 << ev << es;

    QTest::newRow("IntegerCo") << es << "IntegerCo" << -1 << ev << "abc";
    QTest::newRow("IntegerCo (good version)") << es << "IntegerCo" << 5 << ev << "abc";
    QTest::newRow("IntegerCo (bad version)") << es << "IntegerCo" << 41 << ev << es;


    /* Boolean testing */
    QTest::newRow("Boolean action") << "Boolean" << es << -1 << ev << "a";
    QTest::newRow("BooleanCo") << es << "BooleanCo" << -1 << ev << "a";
    QTest::newRow("BooleanCo (good version)") << es << "BooleanCo" << 3 << ev << "a";
    QTest::newRow("BooleanCo (bad version)") << es << "BooleanCo" << 3234243 << ev << es;

    /* Value filtering */
    QTest::newRow("Any action matching 20") << es << es << -1 << QVariant(20) << "b";
    QTest::newRow("Any action matching 4.0") << es << es << -1 << QVariant(4.0) << "bc";
    QTest::newRow("NumberCo with 20") << es << "NumberCo" << -1 << QVariant(20) << "b";
    QTest::newRow("NumberCo with 4.0") << es << "NumberCo" << -1 << QVariant(4.0) << "bc";
    QTest::newRow("IntegerCo with 20") << es << "IntegerCo" << -1 << QVariant(20) << "b";
    QTest::newRow("IntegerCo with 4.0") << es << "IntegerCo" << -1 << QVariant(4.0) << es;
    QTest::newRow("Boolean action matching true") << es << "BooleanCo" << -1 << QVariant(true) << "a";
    QTest::newRow("Boolean action matching false") << es << "BooleanCo" << -1 << QVariant(false) << es;
}

void tst_QContactManagerFiltering::actionFiltering()
{
    QContactManager* cm = new QContactManager("memory");

    QFETCH(QString, actionName);
    QFETCH(QString, vendorName);
    QFETCH(int, version);
    QFETCH(QVariant, value);
    QFETCH(QString, expected);

    QList<QUniqueId> contacts = prepareModel(cm);
    QList<QUniqueId> ids;

    QVERIFY(contacts.count() == 4);

    QContactActionFilter af;
    af.setActionName(actionName);
    af.setValue(value);
    af.setVendor(vendorName, version);

    ids = cm->contacts(af);

    QString output = convertIds(contacts, ids);
    QCOMPARE(output, expected);

    delete cm;
}

QList<QUniqueId> tst_QContactManagerFiltering::prepareModel(QContactManager *cm)
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
    time.setValue("value", QTime(16,52,23,0));

    a.saveDetail(&name);
    a.saveDetail(&number);
    a.saveDetail(&integer);
    a.saveDetail(&datetime);
    a.saveDetail(&boool);
    a.saveDetail(&ullong);
    a.saveDetail(&date);
    a.saveDetail(&time);

    name.setFirst("Bob");
    name.setLast("Aaronsen");
    number.setNumber("555-3456");
    integer.setValue("value", 20);
    dubble.setValue("value", 4.0);
    boool.setValue("value", false);
    ullong.setValue("value", (qulonglong) 80000000000LL); // 80B
    uintt.setValue("value", 4000000000u); // 4B
    date.setValue("value", QDate(1492, 5, 5));
    time.setValue("value", QTime(15,52,23,0));
    charr.setValue("value", QVariant(QChar('b')));

    b.saveDetail(&name);
    b.saveDetail(&number);
    b.saveDetail(&integer);
    b.saveDetail(&dubble);
    b.saveDetail(&boool);
    b.saveDetail(&ullong);
    b.saveDetail(&uintt);
    b.saveDetail(&date);
    b.saveDetail(&time);
    b.saveDetail(&charr);

    name.setFirst("Boris");
    name.setLast("Aaronsun");
    integer.setValue("value", -20);
    datetime.setValue("value", QDateTime(QDate(2009, 06, 29), QTime(16, 54, 17, 0)));
    llong.setValue("value", (qlonglong)8000000000LL); // 8B
    charr.setValue("value", QVariant(QChar('c')));

    c.saveDetail(&name);
    c.saveDetail(&integer);
    c.saveDetail(&datetime);
    c.saveDetail(&dubble);
    c.saveDetail(&boool);
    c.saveDetail(&llong);
    c.saveDetail(&ullong);
    c.saveDetail(&charr);

    name.setFirst("Dennis");
    name.setLast("FitzMacintyre");
    dubble.setValue("value", -128.0);
    llong.setValue("value", (qlonglong)-14000000000LL);
    uintt.setValue("value", 3000000000u); // 3B
    date.setValue("value", QDate(1770, 10, 1));

    d.saveDetail(&name);
    d.saveDetail(&dubble);
    d.saveDetail(&llong);
    d.saveDetail(&uintt);
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

    QList<QUniqueId> list;
    if (!a.isEmpty())
        list << a.id();
    if (!b.isEmpty())
        list << b.id();
    if (!c.isEmpty())
        list << c.id();
    if (!d.isEmpty())
        list << d.id();
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

/* Static actions for testing matching */
class QIntegerAction : public QContactAbstractAction
{
    Q_OBJECT

public:
    QIntegerAction() {}
    ~QIntegerAction() {}

    QString actionName() const {return "Number";}
    QVariantMap metadata() const {return QVariantMap();}
    virtual QString vendor() const {return "IntegerCo";}
    virtual int implementationVersion() const {return 5;}

    QContactFilter contactFilter(const QVariant& value) const
    {
        QContactDetailFilter df;
        df.setDetailDefinitionName("Integer", "value");
        df.setValue(value);
        return df;
    }
    bool supportsDetail(const QContactDetail& detail) const
    {
        return detail.definitionName() == "Integer" && !detail.variantValue("value").isNull();
    }

    void performAction(const QContact& contact, const QContactDetail& detail = QContactDetail())
    {
        Q_UNUSED(contact);
        Q_UNUSED(detail);
        // Well, do something
    }
};

class QNumberAction : public QContactAbstractAction
{
    Q_OBJECT

public:
    QNumberAction() {}
    ~QNumberAction() {}

    QString actionName() const {return "Number";}
    QVariantMap metadata() const {return QVariantMap();}
    virtual QString vendor() const {return "NumberCo";}
    virtual int implementationVersion() const {return 42;}

    QContactFilter contactFilter(const QVariant& value) const
    {
        QContactDetailFilter df;
        df.setDetailDefinitionName("Double", "value");
        df.setValue(value);

        QContactDetailFilter df2;
        df2.setDetailDefinitionName("Integer", "value");
        df2.setValue(value);

        /* We like either doubles or integers */
        return df || df2;
    }
    bool supportsDetail(const QContactDetail& detail) const
    {
        if (detail.definitionName() == "Double" && !detail.variantValue("value").isNull())
            return true;
        if (detail.definitionName() == "Integer" && !detail.variantValue("value").isNull())
            return true;
        return false;
    }

    void performAction(const QContact& contact, const QContactDetail& detail = QContactDetail())
    {
        Q_UNUSED(contact);
        Q_UNUSED(detail);
        // Well, do something
    }
};

class QBooleanAction : public QContactAbstractAction
{
    Q_OBJECT

public:
    QBooleanAction() {}
    ~QBooleanAction() {}

    QString actionName() const {return "Boolean";}
    QVariantMap metadata() const {return QVariantMap();}
    virtual QString vendor() const {return "BooleanCo";}
    virtual int implementationVersion() const {return 3;}

    QContactFilter contactFilter(const QVariant& value) const
    {
        if (value.isNull() || (value.type() == QVariant::Bool && value.toBool() == true)) {
            /* This only likes bools that are true */
            QContactDetailFilter df;
            df.setDetailDefinitionName("Bool", "value");
            df.setValue(true);
            return df;
        } else {
            return QContactFilter();
        }
    }
    bool supportsDetail(const QContactDetail& detail) const
    {
        return detail.definitionName() == "Bool" && (detail.value<bool>("value") == true);
    }

    void performAction(const QContact& contact, const QContactDetail& detail = QContactDetail())
    {
        Q_UNUSED(contact);
        Q_UNUSED(detail);
        // Well, do something
    }
};

class FilterActionFactory : public QContactAbstractActionFactory
{
    Q_OBJECT
    Q_INTERFACES(QContactAbstractActionFactory)

public:
    FilterActionFactory() {}
    ~FilterActionFactory() {}

    QString name() {return QString("FilterActionFactory");}
    QStringList actionNames() {return QStringList() << "Number" << "Boolean";}

    QContactAbstractAction* instance(const QString& actionName = QString(), const QString& vendor = QString(), int implementationVersion = -1)
    {
        Q_UNUSED(actionName);
        Q_UNUSED(vendor);
        Q_UNUSED(implementationVersion);
        return 0;
    }

    QList<QContactAbstractAction*> instances(const QString& actionName = QString(), const QString& vendor = QString(), int implementationVersion = -1)
    {
        QList<QContactAbstractAction*> ret;
        // If we're after Number, we add all IntegerAction and NumberAction
        // If we're after Boolean, we add BooleanAction
        if (actionName == "Number") {
            if (vendor.isEmpty()) {
                if (implementationVersion == -1 || implementationVersion == 5)
                    ret.append(new QIntegerAction());
                if (implementationVersion == -1 || implementationVersion == 42) {
                    ret.append(new QNumberAction());
                }
            } else if (vendor == "IntegerCo") {
                if (implementationVersion == -1 || implementationVersion == 5)
                    ret.append(new QIntegerAction);
            } else if (vendor == "NumberCo") {
                if (implementationVersion == -1 || implementationVersion == 42)
                    ret.append(new QNumberAction);
            }
        } else if (actionName == "Boolean") {
            if (implementationVersion == -1 || implementationVersion == 3)
                ret.append(new QBooleanAction);
        } else {
            if (implementationVersion == -1 || implementationVersion == 42)
                ret.append(new QNumberAction);
            if (implementationVersion == -1 || implementationVersion == 5)
                ret.append(new QIntegerAction);
            if (implementationVersion == -1 || implementationVersion == 3)
                ret.append(new QBooleanAction);
        }

        qDebug() << actionName << vendor << implementationVersion << ret;
        return ret;
    }
};

/* Statically import it (and a duplicate copy of it, purely for testing purposes) */
Q_EXPORT_PLUGIN2(contacts_testFilterActionFactory, FilterActionFactory);
Q_IMPORT_PLUGIN(contacts_testFilterActionFactory);

QTEST_MAIN(tst_QContactManagerFiltering)
#include "tst_qcontactmanagerfiltering.moc"
