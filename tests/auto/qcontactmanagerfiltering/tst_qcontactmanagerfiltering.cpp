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

    QList<QContactManager*> cleanupStack;

private slots:
    void rangeFiltering(); // XXX should take all managers
    void rangeFiltering_data();

    void detailStringFiltering(); // XXX should take all managers
    void detailStringFiltering_data();

    void actionPlugins();
    void actionFiltering();
    void actionFiltering_data();

    void detailVariantFiltering();
    void detailVariantFiltering_data();

    void groupMembershipFiltering();
    void groupMembershipFiltering_data();

    void intersectionFiltering();
    void intersectionFiltering_data();

    void unionFiltering();
    void unionFiltering_data();

    void changelogFiltering();
    void changelogFiltering_data();

    void idListFiltering();
    void idListFiltering_data();

    void sorting(); // XXX should take all managers
    void sorting_data();

    void multiSorting();
    void multiSorting_data();

    void invalidFiltering();
    void allFiltering();
};

tst_QContactManagerFiltering::tst_QContactManagerFiltering()
{
}

tst_QContactManagerFiltering::~tst_QContactManagerFiltering()
{
    // Cleanup whatever is left over
    qDeleteAll(cleanupStack);
    cleanupStack.clear();
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

void tst_QContactManagerFiltering::detailStringFiltering_data()
{
    QTest::addColumn<QString>("defname");
    QTest::addColumn<QString>("fieldname");
    QTest::addColumn<QVariant>("value");
    QTest::addColumn<int>("matchflags");
    QTest::addColumn<QString>("expected");

    QVariant ev; // empty variant
    QString es; // empty string

    QString name = QContactName::DefinitionName;
    QString firstname = QContactName::FieldFirst;

    QTest::newRow("Name == Aaron") << name << firstname << QVariant("Aaron") << 0 << "a";
    QTest::newRow("Name == aaron") << name << firstname << QVariant("aaron") << 0 << "a";
    QTest::newRow("Name == Aaron, case sensitive") << name << firstname << QVariant("Aaron") << (int)(Qt::MatchCaseSensitive) << "a";
    QTest::newRow("Name == aaron, case sensitive") << name << firstname << QVariant("aaron") << (int)(Qt::MatchCaseSensitive) << es;

    QTest::newRow("Name == Aaron, begins") << name << firstname << QVariant("Aaron") << (int)(Qt::MatchStartsWith) << "a";
    QTest::newRow("Name == aaron, begins") << name << firstname << QVariant("aaron") << (int)(Qt::MatchStartsWith) << "a";
    QTest::newRow("Name == Aaron, begins, case sensitive") << name << firstname << QVariant("Aaron") << (int)(Qt::MatchStartsWith | Qt::MatchCaseSensitive) << "a";
    QTest::newRow("Name == aaron, begins, case sensitive") << name << firstname << QVariant("aaron") << (int)(Qt::MatchStartsWith | Qt::MatchCaseSensitive) << es;

    QTest::newRow("Name == Aar, begins") << name << firstname << QVariant("Aar") << (int)(Qt::MatchStartsWith) << "a";
    QTest::newRow("Name == aar, begins") << name << firstname << QVariant("aar") << (int)(Qt::MatchStartsWith) << "a";
    QTest::newRow("Name == Aar, begins, case sensitive") << name << firstname << QVariant("Aar") << (int)(Qt::MatchStartsWith | Qt::MatchCaseSensitive) << "a";
    QTest::newRow("Name == aar, begins, case sensitive") << name << firstname << QVariant("aar") << (int)(Qt::MatchStartsWith | Qt::MatchCaseSensitive) << es;

    QTest::newRow("Name == aro, contains") << name << firstname << QVariant("aro") << (int)(Qt::MatchContains) << "a";
    QTest::newRow("Name == ARO, contains") << name << firstname << QVariant("ARO") << (int)(Qt::MatchContains) << "a";
    QTest::newRow("Name == aro, contains, case sensitive") << name << firstname << QVariant("aro") << (int)(Qt::MatchContains | Qt::MatchCaseSensitive) << "a";
    QTest::newRow("Name == ARO, contains, case sensitive") << name << firstname << QVariant("ARO") << (int)(Qt::MatchContains | Qt::MatchCaseSensitive) << es;

    QTest::newRow("Name == ron, ends") << name << firstname << QVariant("ron") << (int)(Qt::MatchEndsWith) << "a";
    QTest::newRow("Name == ARON, ends") << name << firstname << QVariant("ARON") << (int)(Qt::MatchEndsWith) << "a";
    QTest::newRow("Name == aron, ends, case sensitive") << name << firstname << QVariant("aron") << (int)(Qt::MatchEndsWith | Qt::MatchCaseSensitive) << "a";
    QTest::newRow("Name == ARON, ends, case sensitive") << name << firstname << QVariant("ARON") << (int)(Qt::MatchEndsWith | Qt::MatchCaseSensitive) << es;

    QTest::newRow("Name == Aaron, fixed") << name << firstname << QVariant("Aaron") << (int)(Qt::MatchFixedString) << "a";
    QTest::newRow("Name == aaron, fixed") << name << firstname << QVariant("aaron") << (int)(Qt::MatchFixedString) << "a";
    QTest::newRow("Name == Aaron, fixed, case sensitive") << name << firstname << QVariant("Aaron") << (int)(Qt::MatchFixedString | Qt::MatchCaseSensitive) << "a";
    QTest::newRow("Name == aaron, fixed, case sensitive") << name << firstname << QVariant("aaron") << (int)(Qt::MatchFixedString | Qt::MatchCaseSensitive) << es;

    /* Converting other types to strings */
    QTest::newRow("integer == 20") << "Integer" << "value" << QVariant("20") << 0 << es;
    QTest::newRow("integer == 20, as string") << "Integer" << "value" << QVariant("20") << (int)(Qt::MatchFixedString) << "b";
    QTest::newRow("integer == 20, begins with, string") << "Integer" << "value" << QVariant("20") << (int)(Qt::MatchFixedString | Qt::MatchStartsWith) << "b";
    QTest::newRow("integer == 2, begins with, string") << "Integer" << "value" << QVariant("2") << (int)(Qt::MatchFixedString | Qt::MatchStartsWith) << "b";
    QTest::newRow("integer == 20, ends with, string") << "Integer" << "value" << QVariant("20") << (int)(Qt::MatchFixedString | Qt::MatchEndsWith) << "bc";
    QTest::newRow("integer == 0, ends with, string") << "Integer" << "value" << QVariant("0") << (int)(Qt::MatchFixedString | Qt::MatchEndsWith) << "abc";
    QTest::newRow("integer == 20, contains, string") << "Integer" << "value" << QVariant("20") << (int)(Qt::MatchFixedString | Qt::MatchContains) << "bc";
    QTest::newRow("integer == 0, contains, string") << "Integer" << "value" << QVariant("0") << (int)(Qt::MatchFixedString | Qt::MatchContains) << "abc";
}

void tst_QContactManagerFiltering::detailStringFiltering()
{
    QFETCH(QString, defname);
    QFETCH(QString, fieldname);
    QFETCH(QVariant, value);
    QFETCH(QString, expected);
    QFETCH(int, matchflags);

    /* Try the memory database first */
    QContactManager* cm = new QContactManager("memory");

    QList<QUniqueId> contacts = prepareModel(cm);
    QList<QUniqueId> ids;

    QVERIFY(contacts.count() == 4);

    QContactDetailFilter df;
    df.setDetailDefinitionName(defname, fieldname);
    df.setValue(value);
    df.setMatchFlags(Qt::MatchFlags(matchflags));

    /* At this point, since we're using memory, assume the filter isn't really supported */
    QVERIFY(cm->information()->filterSupported(df) == false);

    ids = cm->contacts(df);

    QString output = convertIds(contacts, ids);
    QEXPECT_FAIL("integer == 20", "Not sure if this should pass or fail", Continue);
    QCOMPARE(output, expected);

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

    /* Strings (name) */
    QTest::newRow("first name presence") << "Name" << QString(QLatin1String(QContactName::FieldFirst)) << false << ev << "abcd";
    QTest::newRow("first name == Aaron") << "Name" << QString(QLatin1String(QContactName::FieldFirst)) << true << QVariant("Aaron") << "a";

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

    QString namedef = QContactName::DefinitionName;
    QString firstname = QContactName::FieldFirst;
    QString lastname = QContactName::FieldLast;

    QString phonedef = QContactPhoneNumber::DefinitionName;
    QString phonenum = QContactPhoneNumber::FieldNumber;

    int csflag = (int)Qt::MatchCaseSensitive;

    /* First, cover the "empty defname / fieldname / ranges" cases */
    QTest::newRow("invalid defname") << es << firstname << QVariant("A") << QVariant("Bob") << false << 0 << true << 0 << es;
    QTest::newRow("defn presence test") << namedef << es << QVariant("A") << QVariant("Bob") << false << 0 << true << 0 << "abcd";
    QTest::newRow("field presence test") << phonedef << phonenum << QVariant() << QVariant() << false << 0 << true << 0 << "ab";
    QTest::newRow("good def, bad field") << namedef << "Bongo" << QVariant("A") << QVariant("Bob") << false << 0 << true << 0 << es;
    QTest::newRow("bad def") << "Bongo" << es << QVariant("A") << QVariant("Bob") << false << 0 << true << 0 << es;

    /* Presence for fields that aren't there */
    QTest::newRow("defn presence test negative") << "Burgers" << es << ev << ev << false << 0 << false << 0 << es;
    QTest::newRow("field presence test negative") << "Burgers" << "Beef" << ev << ev << false << 0 << false << 0 << es;
    QTest::newRow("defn yes, field no presence test negative") << namedef << "Burger" << ev << ev << false << 0 << false << 0 << es;

    QTest::newRow("no max, all results") << namedef << firstname << QVariant("a") << QVariant() << false << 0 << true << 0 << "abcd";
    QTest::newRow("no max, some results") << namedef << firstname << QVariant("BOB") << QVariant() << false << 0 << true << 0 << "bcd";
    QTest::newRow("no max, no results") << namedef << firstname << QVariant("ZamBeZI") << QVariant() << false << 0 << true << 0 << es;
    QTest::newRow("no min, all results") << namedef << firstname << QVariant() << QVariant("zambezi") << false << 0 << true << 0 << "abcd";
    QTest::newRow("no min, some results") << namedef << firstname << QVariant() << QVariant("bOb") << false << 0 << true << 0 << "a";
    QTest::newRow("no min, no results") << namedef << firstname << QVariant() << QVariant("aardvark") << false << 0 << true << 0 << es;

    /* now case sensitive */
    QTest::newRow("no max, cs, all results") << namedef << firstname << QVariant("A") << QVariant() << false << 0 << true << csflag << "abcd";
    QTest::newRow("no max, cs, some results") << namedef << firstname << QVariant("Bob") << QVariant() << false << 0 << true << csflag << "bcd";
    QTest::newRow("no max, cs, no results") << namedef << firstname << QVariant("Zambezi") << QVariant() << false << 0 << true << csflag << es;
    QTest::newRow("no min, cs, all results") << namedef << firstname << QVariant() << QVariant("Zambezi") << false << 0 << true << csflag << "abcd";
    QTest::newRow("no min, cs, some results") << namedef << firstname << QVariant() << QVariant("Bob") << false << 0 << true << csflag << "a";
    QTest::newRow("no min, cs, no results") << namedef << firstname << QVariant() << QVariant("Aardvark") << false << 0 << true << csflag << es;

    /* due to ascii sorting, most lower case parameters give all results, which is boring */
    QTest::newRow("no max, cs, badcase, all results") << namedef << firstname << QVariant("A") << QVariant() << false << 0 << true << csflag << "abcd";
    QTest::newRow("no max, cs, badcase, some results") << namedef << firstname << QVariant("BOB") << QVariant() << false << 0 << true << csflag << "bcd";
    QTest::newRow("no max, cs, badcase, no results") << namedef << firstname << QVariant("ZAMBEZI") << QVariant() << false << 0 << true << csflag << es;
    QTest::newRow("no min, cs, badcase, all results") << namedef << firstname << QVariant() << QVariant("ZAMBEZI") << false << 0 << true << csflag << "abcd";
    QTest::newRow("no min, cs, badcase, some results") << namedef << firstname << QVariant() << QVariant("BOB") << false << 0 << true << csflag << "a";
    QTest::newRow("no min, cs, badcase, no results") << namedef << firstname << QVariant() << QVariant("AARDVARK") << false << 0 << true << csflag << es;

    /* 'a' has phone number ("555-1212") */
    QTest::newRow("range1") << phonedef << phonenum << QVariant("555-1200") << QVariant("555-1220") << false << 0 << false << 0 << "a";

    /* A(Aaron Aaronson), B(Bob Aaronsen), C(Boris Aaronsun), D(Dennis FitzMacyntire) */
    // string range matching - no matchflags set.
    QTest::newRow("string range - no matchflags - 1") << namedef << firstname << QVariant("A") << QVariant("Bob") << false << 0 << true << 0 << "a";
    QTest::newRow("string range - no matchflags - 2") << namedef << firstname << QVariant("A") << QVariant("Bob") << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << 0 << "a";
    QTest::newRow("string range - no matchflags - 3") << namedef << firstname << QVariant("A") << QVariant("Bob") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << 0 << "a";
    QTest::newRow("string range - no matchflags - 4") << namedef << firstname << QVariant("A") << QVariant("Bob") << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper) << true << 0 << "ab";
    QTest::newRow("string range - no matchflags - 5") << namedef << firstname << QVariant("A") << QVariant("Bob") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::IncludeUpper) << true << 0 << "ab";
    QTest::newRow("string range - no matchflags - 6") << namedef << firstname << QVariant("Bob") << QVariant("C") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::IncludeUpper) << true << 0 << "c";
    QTest::newRow("string range - no matchflags - 7") << namedef << firstname << QVariant("Bob") << QVariant("C") << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << 0 << "bc";
    QTest::newRow("string range - no matchflags - 8") << namedef << firstname << QVariant("Bob") << QVariant("C") << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper) << true << 0 << "bc";
    QTest::newRow("string range - no matchflags - 9") << namedef << firstname << QVariant("Bob") << QVariant("C") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << 0 << "c";
    QTest::newRow("string range - no matchflags - 10") << namedef << firstname << QVariant("Barry") << QVariant("C") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << 0 << "bc";

    // string range matching - Qt::MatchStartsWith should produce the same results as without matchflags set.
    QTest::newRow("string range - startswith - 1") << namedef << firstname << QVariant("A") << QVariant("Bob") << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchStartsWith) << "a";
    QTest::newRow("string range - startswith - 2") << namedef << firstname << QVariant("A") << QVariant("Bob") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchStartsWith) << "a";
    QTest::newRow("string range - startswith - 3") << namedef << firstname << QVariant("A") << QVariant("Bob") << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper) << true << (int)(Qt::MatchStartsWith) << "ab";
    QTest::newRow("string range - startswith - 4") << namedef << firstname << QVariant("A") << QVariant("Bob") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::IncludeUpper) << true << (int)(Qt::MatchStartsWith) << "ab";
    QTest::newRow("string range - startswith - 5") << namedef << firstname << QVariant("Bob") << QVariant("C") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::IncludeUpper) << true << (int)(Qt::MatchStartsWith) << "c";
    QTest::newRow("string range - startswith - 6") << namedef << firstname << QVariant("Bob") << QVariant("C") << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchStartsWith) << "bc";
    QTest::newRow("string range - startswith - 7") << namedef << firstname << QVariant("Bob") << QVariant("C") << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper) << true << (int)(Qt::MatchStartsWith) << "bc";
    QTest::newRow("string range - startswith - 8") << namedef << firstname << QVariant("Bob") << QVariant("C") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchStartsWith) << "c";
    QTest::newRow("string range - startswith - 9") << namedef << firstname << QVariant("Barry") << QVariant("C") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchStartsWith) << "bc";

    // Open ended starts with
    QTest::newRow("string range - startswith open top - 1") << namedef << firstname << QVariant("A") << ev << true << (int)(QContactDetailRangeFilter::IncludeLower) << true << (int)(Qt::MatchStartsWith) << "abcd";
    QTest::newRow("string range - startswith open top - 2") << namedef << firstname << QVariant("A") << ev << true << (int)(QContactDetailRangeFilter::ExcludeLower) << true << (int)(Qt::MatchStartsWith) << "abcd";
    QTest::newRow("string range - startswith open top - 3") << namedef << firstname << QVariant("Aaron") << ev << true << (int)(QContactDetailRangeFilter::IncludeLower) << true << (int)(Qt::MatchStartsWith) << "abcd";
    QTest::newRow("string range - startswith open top - 4") << namedef << firstname << QVariant("Aaron") << ev << true << (int)(QContactDetailRangeFilter::ExcludeLower) << true << (int)(Qt::MatchStartsWith) << "bcd";
    QTest::newRow("string range - startswith open bottom - 1") << namedef << firstname << ev << QVariant("Borit") << true << (int)(QContactDetailRangeFilter::IncludeUpper) << true << (int)(Qt::MatchStartsWith) << "abc";
    QTest::newRow("string range - startswith open bottom - 2") << namedef << firstname << ev << QVariant("Borit") << true << (int)(QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchStartsWith) << "abc";
    QTest::newRow("string range - startswith open bottom - 3") << namedef << firstname << ev << QVariant("Boris") << true << (int)(QContactDetailRangeFilter::IncludeUpper) << true << (int)(Qt::MatchStartsWith) << "abc";
    QTest::newRow("string range - startswith open bottom - 4") << namedef << firstname << ev << QVariant("Boris") << true << (int)(QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchStartsWith) << "ab";

    // Qt::MatchContains with range is invalid
    QTest::newRow("string range - contains - 1") << namedef << firstname << QVariant("A") << QVariant("Bob") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchContains) << es;

    // Check EndsWith with range: A == son, B == sen, C == sun
    QTest::newRow("string range - endswith - 1") << namedef << lastname << QVariant("sen") << QVariant("son") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchEndsWith) << es;
    QTest::newRow("string range - endswith - 2") << namedef << lastname << QVariant("sen") << QVariant("son") << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchEndsWith) << "b";
    QTest::newRow("string range - endswith - 3") << namedef << lastname << QVariant("sen") << QVariant("son") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::IncludeUpper) << true << (int)(Qt::MatchEndsWith) << "a";
    QTest::newRow("string range - endswith - 4") << namedef << lastname << QVariant("sen") << QVariant("son") << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper) << true << (int)(Qt::MatchEndsWith) << "ab";
    QTest::newRow("string range - endswith - 5") << namedef << lastname << QVariant("sen") << QVariant("sun") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::IncludeUpper) << true << (int)(Qt::MatchEndsWith) << "ac";
    QTest::newRow("string range - endswith - 6") << namedef << lastname << QVariant("sen") << QVariant("sun") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchEndsWith) << "a";

    // Endswith with open ends
    QTest::newRow("string range - endswith no max - 1") << namedef << lastname << QVariant("sen") << ev << true << (int)(QContactDetailRangeFilter::ExcludeLower) << true << (int)(Qt::MatchEndsWith) << "acd";
    QTest::newRow("string range - endswith no max - 2") << namedef << lastname << QVariant("sen") << ev << true << (int)(QContactDetailRangeFilter::IncludeLower) << true << (int)(Qt::MatchEndsWith) << "abcd";
    QTest::newRow("string range - endswith no max - 3") << namedef << lastname << QVariant("sem") << ev << true << (int)(QContactDetailRangeFilter::ExcludeLower) << true << (int)(Qt::MatchEndsWith) << "abcd";
    QTest::newRow("string range - endswith no max - 4") << namedef << lastname << QVariant("sem") << ev << true << (int)(QContactDetailRangeFilter::IncludeLower) << true << (int)(Qt::MatchEndsWith) << "abcd";
    QTest::newRow("string range - endswith no min - 1") << namedef << lastname << ev << QVariant("sen") << true << (int)(QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchEndsWith) << es;
    QTest::newRow("string range - endswith no min - 2") << namedef << lastname << ev << QVariant("sen") << true << (int)(QContactDetailRangeFilter::IncludeUpper) << true << (int)(Qt::MatchEndsWith) << "b";
    QTest::newRow("string range - endswith no min - 3") << namedef << lastname << ev << QVariant("seo") << true << (int)(QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchEndsWith) << "b";
    QTest::newRow("string range - endswith no min - 4") << namedef << lastname << ev << QVariant("seo") << true << (int)(QContactDetailRangeFilter::IncludeUpper) << true << (int)(Qt::MatchEndsWith) << "b";

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

    QString output = convertIds(contacts, ids);
    QCOMPARE(output, expected);

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

    QList<QUniqueId> contacts = prepareModel(cm);
    QList<QUniqueId> idsone, idstwo;

    QVERIFY(contacts.count() == 4);

    QContactGroup g1, g2;
    g1.setName("GroupOne");
    g2.setName("GroupTwo");

    // add the specified members to the specified groups
    for (int i = 0; i < expectedone.size(); i++)
        g1.addMember(contacts.at(expectedone.at(i).toLower().toAscii() - 'a'));
    for (int i = 0; i < expectedtwo.size(); i++)
        g2.addMember(contacts.at(expectedtwo.at(i).toLower().toAscii() - 'a'));

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

    QString output = convertIds(contacts, idsone);
    QCOMPARE(output, expectedone);

    output = convertIds(contacts, idstwo);
    QCOMPARE(output, expectedtwo);

    delete cm;
}

void tst_QContactManagerFiltering::intersectionFiltering_data()
{
    QTest::addColumn<bool>("firstfilter");
    QTest::addColumn<int>("fftype"); // 1 = detail, 2 = detailrange, 3 = groupmembership, 4 = union, 5 = intersection
    QTest::addColumn<QString>("ffdefname");
    QTest::addColumn<QString>("fffieldname");
    QTest::addColumn<bool>("ffsetvalue");
    QTest::addColumn<QVariant>("ffvalue");
    QTest::addColumn<QVariant>("ffminrange");
    QTest::addColumn<QVariant>("ffmaxrange");
    QTest::addColumn<bool>("secondfilter");
    QTest::addColumn<int>("sftype");
    QTest::addColumn<QString>("sfdefname");
    QTest::addColumn<QString>("sffieldname");
    QTest::addColumn<bool>("sfsetvalue");
    QTest::addColumn<QVariant>("sfvalue");
    QTest::addColumn<QVariant>("sfminrange");
    QTest::addColumn<QVariant>("sfmaxrange");
    QTest::addColumn<QString>("order");
    QTest::addColumn<QString>("expected");

    QString es; // empty string.

    // for the following tests, terminology:
    // X will be an (empty) intersection filter created in the test
    // Y will be the first filter defined here
    // Z will be the second filter defined here

    // WITH Y AND Z AS DETAIL FILTERS (with no overlap between Y and Z results)
    // For these tests, Y matches "bc" and Z matches "a"
    // X && Y - X empty so es
    QTest::newRow("A1") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << false << 1 << "Integer" << "value" << false << QVariant(10) << QVariant() << QVariant()
                        << "XY" << es;
    // Y && X - X empty so es
    QTest::newRow("A2") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << false << 1 << "Integer" << "value" << false << QVariant(10) << QVariant() << QVariant()
                        << "YX" << es;
    // Y && Z  - matches "a" and "bc" - so intersected = es
    QTest::newRow("A3") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << true << 1 << "Integer" << "value" << true << QVariant(10) << QVariant() << QVariant()
                        << "YZ" << es;
    // Z && Y - matches "bc" and "a" - so intersected = es
    QTest::newRow("A4") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << true << 1 << "Integer" << "value" << true << QVariant(10) << QVariant() << QVariant()
                        << "ZY" << es;
    // X && Z - X empty so es
    QTest::newRow("A5") << false << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << true << 1 << "Integer" << "value" << true << QVariant(10) << QVariant() << QVariant()
                        << "XZ" << es;
    // Z && X - X empty so es
    QTest::newRow("A6") << false << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << true << 1 << "Integer" << "value" << true << QVariant(10) << QVariant() << QVariant()
                        << "ZX" << es;
    // X && Y && Z - X empty so es
    QTest::newRow("A7") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << true << 1 << "Integer" << "value" << true << QVariant(10) << QVariant() << QVariant()
                        << "XYZ" << es;
    // X && Z && Y - X empty so es
    QTest::newRow("A8") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << true << 1 << "Integer" << "value" << true << QVariant(10) << QVariant() << QVariant()
                        << "XZY" << es;
    // Y && X && Z - X empty so es
    QTest::newRow("A9") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << true << 1 << "Integer" << "value" << true << QVariant(10) << QVariant() << QVariant()
                        << "YXZ" << es;
    // Z && X && Y - X empty so es
    QTest::newRow("A10") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                         << true << 1 << "Integer" << "value" << true << QVariant(10) << QVariant() << QVariant()
                         << "ZXY" << es;
    // Y && Z && X - X empty so es
    QTest::newRow("A11") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                         << true << 1 << "Integer" << "value" << true << QVariant(10) << QVariant() << QVariant()
                         << "YZX" << es;
    // Z && Y && X - X empty so es
    QTest::newRow("A12") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                         << true << 1 << "Integer" << "value" << true << QVariant(10) << QVariant() << QVariant()
                         << "ZYX" << es;

    // WITH Y AND Z AS DETAIL FILTERS (with some overlap between Y and Z results)
    // For these tests, Y matches "bc", Z matches "b"
    // X && Y - X empty so es
    QTest::newRow("B1") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << false << 1 << "Integer" << "value" << false << QVariant(20) << QVariant() << QVariant()
                        << "XY" << es;
    // Y && X - X empty so es
    QTest::newRow("B2") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << false << 1 << "Integer" << "value" << false << QVariant(20) << QVariant() << QVariant()
                        << "YX" << es;
    // Y && Z  - matches "b" and "bc" - so intersected = "b"
    QTest::newRow("B3") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << true << 1 << "Integer" << "value" << true << QVariant(20) << QVariant() << QVariant()
                        << "YZ" << "b";
    // Z && Y - matches "bc" and "b" - so intersected = "b"
    QTest::newRow("B4") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << true << 1 << "Integer" << "value" << true << QVariant(20) << QVariant() << QVariant()
                        << "ZY" << "b";
    // X && Z - X empty so es
    QTest::newRow("B5") << false << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << true << 1 << "Integer" << "value" << true << QVariant(20) << QVariant() << QVariant()
                        << "XZ" << es;
    // Z && X - X empty so es
    QTest::newRow("B6") << false << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << true << 1 << "Integer" << "value" << true << QVariant(20) << QVariant() << QVariant()
                        << "ZX" << es;
    // X && Y && Z - X empty so es
    QTest::newRow("B7") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << true << 1 << "Integer" << "value" << true << QVariant(20) << QVariant() << QVariant()
                        << "XYZ" << es;
    // X && Z && Y - X empty so es
    QTest::newRow("B8") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << true << 1 << "Integer" << "value" << true << QVariant(20) << QVariant() << QVariant()
                        << "XZY" << es;
    // Y && X && Z - X empty so es
    QTest::newRow("B9") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << true << 1 << "Integer" << "value" << true << QVariant(20) << QVariant() << QVariant()
                        << "YXZ" << es;
    // Z && X && Y - X empty so es
    QTest::newRow("B10") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                         << true << 1 << "Integer" << "value" << true << QVariant(20) << QVariant() << QVariant()
                         << "ZXY" << es;
    // Y && Z && X - X empty so es
    QTest::newRow("B11") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                         << true << 1 << "Integer" << "value" << true << QVariant(20) << QVariant() << QVariant()
                         << "YZX" << es;
    // Z && Y && X - X empty so es
    QTest::newRow("B12") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                         << true << 1 << "Integer" << "value" << true << QVariant(20) << QVariant() << QVariant()
                         << "ZYX" << es;

    //---------------------------

    // WITH Y AND Z AS RANGE FILTERS (with no overlap between Y and Z results)
    // For these tests, Y matches "a", Z matches "b"
    // X && Y - X empty so es
    QTest::newRow("C1") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(15)
                        << false << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "XY" << es;
    // Y && X - X empty so es
    QTest::newRow("C2") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(15)
                        << false << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "YX" << es;
    // Y && Z - no overlap so es
    QTest::newRow("C3") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(15)
                        << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "YZ" << es;
    // Z && Y - no overlap so es
    QTest::newRow("C4") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(15)
                        << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "ZY" << es;
    // X && Z - X empty so es
    QTest::newRow("C5") << false << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(15)
                        << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "XZ" << es;
    // Z && X - X empty so es
    QTest::newRow("C6") << false << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(15)
                        << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "ZX" << es;
    // X && Y && Z - X empty so es
    QTest::newRow("C7") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(15)
                        << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "XYZ" << es;
    // X && Z && Y - X empty so es
    QTest::newRow("C8") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(15)
                        << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "XZY" << es;
    // Y && X && Z - X empty so es
    QTest::newRow("C9") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(15)
                        << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "YXZ" << es;
    // Z && X && Y - X empty so es
    QTest::newRow("C10") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(15)
                         << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                         << "ZXY" << es;
    // Y && Z && X - X empty so es
    QTest::newRow("C11") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(15)
                         << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                         << "YZX" << es;
    // Z && Y && X - X empty so es
    QTest::newRow("C12") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(15)
                         << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                         << "ZYX" << es;

    // WITH Y AND Z AS RANGE FILTERS (with some overlap between Y and Z results)
    // For these tests, Y matches "ab", Z matches "b"
    // X && Y - X empty so es
    QTest::newRow("D1") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(25)
                        << false << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "XY" << es;
    // Y && X - X empty so es
    QTest::newRow("D2") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(25)
                        << false << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "YX" << es;
    // Y && Z - Y matches "ab", Z matches "b", intersection = "b"
    QTest::newRow("D3") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(25)
                        << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "YZ" << "b";
    // Z && Y - Y matches "ab", Z matches "b", intersection = "b"
    QTest::newRow("D4") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(25)
                        << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "ZY" << "b";
    // X && Z - X empty so es
    QTest::newRow("D5") << false << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(25)
                        << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "XZ" << es;
    // Z && X - X empty so es
    QTest::newRow("D6") << false << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(25)
                        << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "ZX" << es;
    // X && Y && Z - X empty so es
    QTest::newRow("D7") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(25)
                        << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "XYZ" << es;
    // X && Z && Y - X empty so es
    QTest::newRow("D8") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(25)
                        << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "XZY" << es;
    // Y && X && Z - X empty so es
    QTest::newRow("D9") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(25)
                        << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "YXZ" << es;
    // Z && X && Y - X empty so es
    QTest::newRow("D10") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(25)
                         << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                         << "ZXY" << es;
    // Y && Z && X - X empty so es
    QTest::newRow("D11") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(25)
                         << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                         << "YZX" << es;
    // Z && Y && X - X empty so es
    QTest::newRow("D12") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(25)
                         << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                         << "ZYX" << es;
}

void tst_QContactManagerFiltering::intersectionFiltering()
{
    QFETCH(bool, firstfilter);
    QFETCH(int, fftype); // 1 = detail, 2 = detailrange, 3 = groupmembership, 4 = union, 5 = intersection
    QFETCH(QString, ffdefname);
    QFETCH(QString, fffieldname);
    QFETCH(bool, ffsetvalue);
    QFETCH(QVariant, ffvalue);
    QFETCH(QVariant, ffminrange);
    QFETCH(QVariant, ffmaxrange);
    QFETCH(bool, secondfilter);
    QFETCH(int, sftype);
    QFETCH(QString, sfdefname);
    QFETCH(QString, sffieldname);
    QFETCH(bool, sfsetvalue);
    QFETCH(QVariant, sfvalue);
    QFETCH(QVariant, sfminrange);
    QFETCH(QVariant, sfmaxrange);
    QFETCH(QString, order);
    QFETCH(QString, expected);

    QContactFilter *x = new QContactIntersectionFilter();
    QContactFilter *y = 0, *z = 0;

    if (firstfilter) {
        switch (fftype) {
            case 1: // detail filter
                y = new QContactDetailFilter();
                static_cast<QContactDetailFilter*>(y)->setDetailDefinitionName(ffdefname, fffieldname);
                if (ffsetvalue)
                    static_cast<QContactDetailFilter*>(y)->setValue(ffvalue);
                break;
            case 2: // range filter
                y = new QContactDetailRangeFilter();
                static_cast<QContactDetailRangeFilter*>(y)->setDetailDefinitionName(ffdefname, fffieldname);
                static_cast<QContactDetailRangeFilter*>(y)->setRange(ffminrange, ffmaxrange);
                break;
            case 3: // group membership filter
            case 4: // union filter
            case 5: // intersection filter
                break;

            default:
                QVERIFY(false); // force fail.
            break;
        }
    }

    if (secondfilter) {
        switch (sftype) {
            case 1: // detail filter
                z = new QContactDetailFilter();
                static_cast<QContactDetailFilter*>(z)->setDetailDefinitionName(sfdefname, sffieldname);
                if (sfsetvalue)
                    static_cast<QContactDetailFilter*>(z)->setValue(sfvalue);
                break;
            case 2: // range filter
                z = new QContactDetailRangeFilter();
                static_cast<QContactDetailRangeFilter*>(z)->setDetailDefinitionName(sfdefname, sffieldname);
                static_cast<QContactDetailRangeFilter*>(z)->setRange(sfminrange, sfmaxrange);
                break;
            case 3: // group membership filter
            case 4: // union filter
            case 5: // intersection filter
                break;

            default:
                QVERIFY(false); // force fail.
            break;
        }
    }

    // control variables - order: starts, ends, mids
    bool sX = false;
    bool sY = false;
    bool sZ = false;
    bool eX = false;
    bool eY = false;
    bool eZ = false;
    bool mX = false;
    bool mY = false;
    bool mZ = false;

    if (order.startsWith("X"))
        sX = true;
    if (order.startsWith("Y"))
        sY = true;
    if (order.startsWith("Z"))
        sZ = true;
    if (order.endsWith("X"))
        eX = true;
    if (order.endsWith("Y"))
        eY = true;
    if (order.endsWith("Z"))
        eZ = true;
    if (order.size() > 2) {
        if (order.at(1) == 'X')
            mX = true;
        if (order.at(1) == 'Y')
            mY = true;
        if (order.at(1) == 'Z')
            mZ = true;
    }

    // now perform the filtering.
    QContactIntersectionFilter resultFilter;
    if (sX) {
        if (mY && eZ)
            resultFilter = *x && *y && *z;
        else if (mZ && eY)
            resultFilter = *x && *z && *y;
        else if (eY)
            resultFilter = *x && *y;
        else if (eZ)
            resultFilter = *x && *z;
    } else if (sY) {
        if (mX && eZ)
            resultFilter = *y && *x && *z;
        else if (mZ && eX)
            resultFilter = *y && *z && *x;
        else if (eX)
            resultFilter = *y && *x;
        else if (eZ)
            resultFilter = *y && *z;
    } else if (sZ) {
        if (mX && eY)
            resultFilter = *z && *x && *y;
        else if (mY && eX)
            resultFilter = *z && *y && *x;
        else if (eX)
            resultFilter = *z && *x;
        else if (eY)
            resultFilter = *z && *y;
    }

    /* Try the memory database first */
    QContactManager* cm = new QContactManager("memory");

    QList<QUniqueId> contacts = prepareModel(cm);
    QList<QUniqueId> ids;

    QVERIFY(contacts.count() == 4);

    ids = cm->contacts(resultFilter);

    QString output = convertIds(contacts, ids);
    QCOMPARE(output, expected);

    delete x;
    if (y) delete y;
    if (z) delete z;
    delete cm;
}

void tst_QContactManagerFiltering::unionFiltering_data()
{
    QTest::addColumn<bool>("firstfilter");
    QTest::addColumn<int>("fftype"); // 1 = detail, 2 = detailrange, 3 = groupmembership, 4 = union, 5 = intersection
    QTest::addColumn<QString>("ffdefname");
    QTest::addColumn<QString>("fffieldname");
    QTest::addColumn<bool>("ffsetvalue");
    QTest::addColumn<QVariant>("ffvalue");
    QTest::addColumn<QVariant>("ffminrange");
    QTest::addColumn<QVariant>("ffmaxrange");
    QTest::addColumn<bool>("secondfilter");
    QTest::addColumn<int>("sftype");
    QTest::addColumn<QString>("sfdefname");
    QTest::addColumn<QString>("sffieldname");
    QTest::addColumn<bool>("sfsetvalue");
    QTest::addColumn<QVariant>("sfvalue");
    QTest::addColumn<QVariant>("sfminrange");
    QTest::addColumn<QVariant>("sfmaxrange");
    QTest::addColumn<QString>("order");
    QTest::addColumn<QString>("expected");

    QString es; // empty string.

    // for the following tests, terminology:
    // X will be an (empty) union filter created in the test
    // Y will be the first filter defined here
    // Z will be the second filter defined here

    // WITH Y AND Z AS DETAIL FILTERS (with no overlap between Y and Z results)
    // For these tests, Y matches "bc" and Z matches "a"
    // X || Y - X empty, Y matches "bc" so union = "bc"
    QTest::newRow("A1") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << false << 1 << "Integer" << "value" << false << QVariant(10) << QVariant() << QVariant()
                        << "XY" << "bc";
    // Y || X - Y matches "bc", X empty so union = "bc"
    QTest::newRow("A2") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << false << 1 << "Integer" << "value" << false << QVariant(10) << QVariant() << QVariant()
                        << "YX" << "bc";
    // Y || Z  - Y matches "bc" and Z matches "a" - so union = "abc"
    QTest::newRow("A3") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << true << 1 << "Integer" << "value" << true << QVariant(10) << QVariant() << QVariant()
                        << "YZ" << "abc";
    // Z || Y - Y matches "bc" and Z matches "a" - so union = "abc"
    QTest::newRow("A4") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << true << 1 << "Integer" << "value" << true << QVariant(10) << QVariant() << QVariant()
                        << "ZY" << "abc";
    // X || Z - X empty, Z matches "a" so "a"
    QTest::newRow("A5") << false << 1 << "Bool" << "value" << false << QVariant(false) << QVariant() << QVariant()
                        << true << 1 << "Integer" << "value" << true << QVariant(10) << QVariant() << QVariant()
                        << "XZ" << "a";
    // Z || X - X empty, Z matches "a" so "a"
    QTest::newRow("A6") << false << 1 << "Bool" << "value" << false << QVariant(false) << QVariant() << QVariant()
                        << true << 1 << "Integer" << "value" << true << QVariant(10) << QVariant() << QVariant()
                        << "ZX" << "a";
    // X || Y || Z - X empty, Y matches "bc", Z matches "a" so "abc"
    QTest::newRow("A7") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << true << 1 << "Integer" << "value" << true << QVariant(10) << QVariant() << QVariant()
                        << "XYZ" << "abc";
    // X || Z || Y - X empty, Y matches "bc", Z matches "a" so "abc"
    QTest::newRow("A8") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << true << 1 << "Integer" << "value" << true << QVariant(10) << QVariant() << QVariant()
                        << "XZY" << "abc";
    // Y || X || Z - X empty, Y matches "bc", Z matches "a" so "abc"
    QTest::newRow("A9") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << true << 1 << "Integer" << "value" << true << QVariant(10) << QVariant() << QVariant()
                        << "YXZ" << "abc";
    // Z || X || Y - X empty, Y matches "bc", Z matches "a" so "abc"
    QTest::newRow("A10") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                         << true << 1 << "Integer" << "value" << true << QVariant(10) << QVariant() << QVariant()
                         << "ZXY" << "abc";
    // Y || Z || X - X empty, Y matches "bc", Z matches "a" so "abc"
    QTest::newRow("A11") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                         << true << 1 << "Integer" << "value" << true << QVariant(10) << QVariant() << QVariant()
                         << "YZX" << "abc";
    // Z || Y || X - X empty, Y matches "bc", Z matches "a" so "abc"
    QTest::newRow("A12") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                         << true << 1 << "Integer" << "value" << true << QVariant(10) << QVariant() << QVariant()
                         << "ZYX" << "abc";

    // WITH Y AND Z AS DETAIL FILTERS (with some overlap between Y and Z results)
    // For these tests, Y matches "bc", Z matches "b"
    // X || Y - X empty, Y matches "b", so "bc"
    QTest::newRow("B1") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << false << 1 << "Integer" << "value" << false << QVariant(20) << QVariant() << QVariant()
                        << "XY" << "bc";
    // Y || X - X empty, Y matches "bc", so "bc"
    QTest::newRow("B2") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << false << 1 << "Integer" << "value" << false << QVariant(20) << QVariant() << QVariant()
                        << "YX" << "bc";
    // Y || Z  - X empty, Y matches "bc", Z matches "b" so "bc"
    QTest::newRow("B3") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << true << 1 << "Integer" << "value" << true << QVariant(20) << QVariant() << QVariant()
                        << "YZ" << "bc";
    // Z || Y - X empty, Y matches "bc", Z matches "b" so "bc"
    QTest::newRow("B4") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << true << 1 << "Integer" << "value" << true << QVariant(20) << QVariant() << QVariant()
                        << "ZY" << "bc";
    // X || Z - X empty, Z matches "b" so "b"
    QTest::newRow("B5") << false << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << true << 1 << "Integer" << "value" << true << QVariant(20) << QVariant() << QVariant()
                        << "XZ" << "b";
    // Z || X - X empty, Z matches "b" so "b"
    QTest::newRow("B6") << false << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << true << 1 << "Integer" << "value" << true << QVariant(20) << QVariant() << QVariant()
                        << "ZX" << "b";
    // X || Y || Z - X empty, Y matches "bc", Z matches "b" so "bc"
    QTest::newRow("B7") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << true << 1 << "Integer" << "value" << true << QVariant(20) << QVariant() << QVariant()
                        << "XYZ" << "bc";
    // X || Z || Y - X empty, Y matches "bc", Z matches "b" so "bc"
    QTest::newRow("B8") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << true << 1 << "Integer" << "value" << true << QVariant(20) << QVariant() << QVariant()
                        << "XZY" << "bc";
    // Y || X || Z - X empty, Y matches "bc", Z matches "b" so "bc"
    QTest::newRow("B9") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                        << true << 1 << "Integer" << "value" << true << QVariant(20) << QVariant() << QVariant()
                        << "YXZ" << "bc";
    // Z || X || Y - X empty, Y matches "bc", Z matches "b" so "bc"
    QTest::newRow("B10") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                         << true << 1 << "Integer" << "value" << true << QVariant(20) << QVariant() << QVariant()
                         << "ZXY" << "bc";
    // Y || Z || X - X empty, Y matches "bc", Z matches "b" so "bc"
    QTest::newRow("B11") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                         << true << 1 << "Integer" << "value" << true << QVariant(20) << QVariant() << QVariant()
                         << "YZX" << "bc";
    // Z || Y || X - X empty, Y matches "bc", Z matches "b" so "bc"
    QTest::newRow("B12") << true << 1 << "Bool" << "value" << true << QVariant(false) << QVariant() << QVariant()
                         << true << 1 << "Integer" << "value" << true << QVariant(20) << QVariant() << QVariant()
                         << "ZYX" << "bc";

    //---------------------------

    // WITH Y AND Z AS RANGE FILTERS (with no overlap between Y and Z results)
    // For these tests, Y matches "a", Z matches "b"
    // X || Y - X empty, Y matches "a" so "a"
    QTest::newRow("C1") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(15)
                        << false << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "XY" << "a";
    // Y || X - X empty, Y matches "a" so "a"
    QTest::newRow("C2") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(15)
                        << false << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "YX" << "a";
    // Y || Z - Y matches "a", Z matches "b" so "ab"
    QTest::newRow("C3") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(15)
                        << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "YZ" << "ab";
    // Z || Y - Y matches "a", Z matches "b" so "ab"
    QTest::newRow("C4") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(15)
                        << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "ZY" << "ab";
    // X || Z - X empty, Z matches "b" so "b"
    QTest::newRow("C5") << false << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(15)
                        << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "XZ" << "b";
    // Z || X - X empty, Z matches "b" so "b"
    QTest::newRow("C6") << false << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(15)
                        << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "ZX" << "b";
    // X || Y || Z - X empty, Y matches "a", Z matches "b" so "ab"
    QTest::newRow("C7") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(15)
                        << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "XYZ" << "ab";
    // X || Z || Y - X empty, Y matches "a", Z matches "b" so "ab"
    QTest::newRow("C8") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(15)
                        << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "XZY" << "ab";
    // Y || X || Z - X empty, Y matches "a", Z matches "b" so "ab"
    QTest::newRow("C9") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(15)
                        << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "YXZ" << "ab";
    // Z || X || Y - X empty, Y matches "a", Z matches "b" so "ab"
    QTest::newRow("C10") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(15)
                         << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                         << "ZXY" << "ab";
    // Y || Z || X - X empty, Y matches "a", Z matches "b" so "ab"
    QTest::newRow("C11") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(15)
                         << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                         << "YZX" << "ab";
    // Z || Y || X - X empty, Y matches "a", Z matches "b" so "ab"
    QTest::newRow("C12") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(15)
                         << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                         << "ZYX" << "ab";

    // WITH Y AND Z AS RANGE FILTERS (with some overlap between Y and Z results)
    // For these tests, Y matches "ab", Z matches "b"
    // X || Y - X empty, Y matches "ab" so "ab"
    QTest::newRow("D1") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(25)
                        << false << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "XY" << "ab";
    // Y || X - X empty, Y matches "ab" so "ab"
    QTest::newRow("D2") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(25)
                        << false << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "YX" << "ab";
    // Y || Z - Y matches "ab", Z matches "b", union = "ab"
    QTest::newRow("D3") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(25)
                        << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "YZ" << "ab";
    // Z || Y - Y matches "ab", Z matches "b", union = "ab"
    QTest::newRow("D4") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(25)
                        << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "ZY" << "ab";
    // X || Z - X empty, Z matches "b" so "b"
    QTest::newRow("D5") << false << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(25)
                        << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "XZ" << "b";
    // Z || X - X empty, Z matches "b" so "b"
    QTest::newRow("D6") << false << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(25)
                        << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "ZX" << "b";
    // X || Y || Z - X empty, Y matches "ab", Z matches "b" so "ab"
    QTest::newRow("D7") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(25)
                        << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "XYZ" << "ab";
    // X || Z || Y - X empty, Y matches "ab", Z matches "b" so "ab"
    QTest::newRow("D8") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(25)
                        << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "XZY" << "ab";
    // Y || X || Z - X empty, Y matches "ab", Z matches "b" so "ab"
    QTest::newRow("D9") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(25)
                        << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                        << "YXZ" << "ab";
    // Z || X || Y - X empty, Y matches "ab", Z matches "b" so "ab"
    QTest::newRow("D10") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(25)
                         << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                         << "ZXY" << "ab";
    // Y || Z || X - X empty, Y matches "ab", Z matches "b" so "ab"
    QTest::newRow("D11") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(25)
                         << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                         << "YZX" << "ab";
    // Z || Y || X - X empty, Y matches "ab", Z matches "b" so "ab"
    QTest::newRow("D12") << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(5) << QVariant(25)
                         << true << 2 << "Integer" << "value" << false << QVariant(0) << QVariant(15) << QVariant(25)
                         << "ZYX" << "ab";
}

void tst_QContactManagerFiltering::unionFiltering()
{
    QFETCH(bool, firstfilter);
    QFETCH(int, fftype); // 1 = detail, 2 = detailrange, 3 = groupmembership, 4 = union, 5 = intersection
    QFETCH(QString, ffdefname);
    QFETCH(QString, fffieldname);
    QFETCH(bool, ffsetvalue);
    QFETCH(QVariant, ffvalue);
    QFETCH(QVariant, ffminrange);
    QFETCH(QVariant, ffmaxrange);
    QFETCH(bool, secondfilter);
    QFETCH(int, sftype);
    QFETCH(QString, sfdefname);
    QFETCH(QString, sffieldname);
    QFETCH(bool, sfsetvalue);
    QFETCH(QVariant, sfvalue);
    QFETCH(QVariant, sfminrange);
    QFETCH(QVariant, sfmaxrange);
    QFETCH(QString, order);
    QFETCH(QString, expected);

    QContactFilter *x = new QContactUnionFilter();
    QContactFilter *y = 0, *z = 0;

    if (firstfilter) {
        switch (fftype) {
            case 1: // detail filter
                y = new QContactDetailFilter();
                static_cast<QContactDetailFilter*>(y)->setDetailDefinitionName(ffdefname, fffieldname);
                if (ffsetvalue)
                    static_cast<QContactDetailFilter*>(y)->setValue(ffvalue);
                break;
            case 2: // range filter
                y = new QContactDetailRangeFilter();
                static_cast<QContactDetailRangeFilter*>(y)->setDetailDefinitionName(ffdefname, fffieldname);
                static_cast<QContactDetailRangeFilter*>(y)->setRange(ffminrange, ffmaxrange);
                break;
            case 3: // group membership filter
            case 4: // union filter
            case 5: // intersection filter
                break;

            default:
                QVERIFY(false); // force fail.
            break;
        }
    }

    if (secondfilter) {
        switch (sftype) {
            case 1: // detail filter
                z = new QContactDetailFilter();
                static_cast<QContactDetailFilter*>(z)->setDetailDefinitionName(sfdefname, sffieldname);
                if (sfsetvalue)
                    static_cast<QContactDetailFilter*>(z)->setValue(sfvalue);
                break;
            case 2: // range filter
                z = new QContactDetailRangeFilter();
                static_cast<QContactDetailRangeFilter*>(z)->setDetailDefinitionName(sfdefname, sffieldname);
                static_cast<QContactDetailRangeFilter*>(z)->setRange(sfminrange, sfmaxrange);
                break;
            case 3: // group membership filter
            case 4: // union filter
            case 5: // intersection filter
                break;

            default:
                QVERIFY(false); // force fail.
            break;
        }
    }

    // control variables - order: starts, ends, mids
    bool sX = false;
    bool sY = false;
    bool sZ = false;
    bool eX = false;
    bool eY = false;
    bool eZ = false;
    bool mX = false;
    bool mY = false;
    bool mZ = false;

    if (order.startsWith("X"))
        sX = true;
    if (order.startsWith("Y"))
        sY = true;
    if (order.startsWith("Z"))
        sZ = true;
    if (order.endsWith("X"))
        eX = true;
    if (order.endsWith("Y"))
        eY = true;
    if (order.endsWith("Z"))
        eZ = true;
    if (order.size() > 2) {
        if (order.at(1) == 'X')
            mX = true;
        if (order.at(1) == 'Y')
            mY = true;
        if (order.at(1) == 'Z')
            mZ = true;
    }

    // now perform the filtering.
    QContactUnionFilter resultFilter;
    if (sX) {
        if (mY && eZ)
            resultFilter = *x || *y || *z;
        else if (mZ && eY)
            resultFilter = *x || *z || *y;
        else if (eY)
            resultFilter = *x || *y;
        else if (eZ)
            resultFilter = *x || *z;
    } else if (sY) {
        if (mX && eZ)
            resultFilter = *y || *x || *z;
        else if (mZ && eX)
            resultFilter = *y || *z || *x;
        else if (eX)
            resultFilter = *y || *x;
        else if (eZ)
            resultFilter = *y || *z;
    } else if (sZ) {
        if (mX && eY)
            resultFilter = *z || *x || *y;
        else if (mY && eX)
            resultFilter = *z || *y || *x;
        else if (eX)
            resultFilter = *z || *x;
        else if (eY)
            resultFilter = *z || *y;
    }

    /* Try the memory database first */
    QContactManager* cm = new QContactManager("memory");

    QList<QUniqueId> contacts = prepareModel(cm);
    QList<QUniqueId> ids;

    QVERIFY(contacts.count() == 4);

    ids = cm->contacts(resultFilter);

    QString output = convertIds(contacts, ids);
    QCOMPARE(output, expected);

    delete x;
    if (y) delete y;
    if (z) delete z;
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

    QString firstname = QContactName::FieldFirst;
    QString lastname = QContactName::FieldLast;
    QString namedef = QContactName::DefinitionName;
    QString urldef = QContactUrl::DefinitionName;
    QString urlfield = QContactUrl::FieldUrl;

    QTest::newRow("first ascending") << namedef << firstname << (int)(Qt::AscendingOrder) << false << 0 << "abcd";
    QTest::newRow("first descending") << namedef << firstname << (int)(Qt::DescendingOrder) << false << 0 << "dcba";
    QTest::newRow("last ascending") << namedef << lastname << (int)(Qt::AscendingOrder) << false << 0 << "bacd";
    QTest::newRow("last descending") << namedef << lastname << (int)(Qt::DescendingOrder) << false << 0 << "dcab";
    QTest::newRow("integer ascending, blanks last") << "Integer" << "value" << (int)(Qt::AscendingOrder) << true << (int)(QContactSortOrder::BlanksLast) << "cabd";
    QTest::newRow("integer descending, blanks last") << "Integer" << "value" << (int)(Qt::DescendingOrder) << true << (int)(QContactSortOrder::BlanksLast) << "bacd";
    QTest::newRow("integer ascending, blanks first") << "Integer" << "value" << (int)(Qt::AscendingOrder) << true << (int)(QContactSortOrder::BlanksFirst) << "dcab";
    QTest::newRow("integer descending, blanks first") << "Integer" << "value" << (int)(Qt::DescendingOrder) << true << (int)(QContactSortOrder::BlanksFirst) << "dbac";
    QTest::newRow("url ascending (null value), blanks first") << urldef << urlfield << (int)(Qt::AscendingOrder) << true << (int)(QContactSortOrder::BlanksFirst) << "abcd";
    QTest::newRow("url ascending (null value), blanks last") << urldef << urlfield << (int)(Qt::AscendingOrder) << true << (int)(QContactSortOrder::BlanksLast) << "dcba";
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

    /* Build the sort order */
    QContactSortOrder s;
    s.setDetailDefinitionName(defname, fieldname);
    s.setDirection(direction);
    if (setbp)
        s.setBlankPolicy(blankpolicy);

    ids = cm->contacts(s);
    QString output = convertIds(contacts, ids);
    QCOMPARE(output, expected);

    /* Now do a check with a filter involved; the filter should not affect the sort order */
    QContactDetailFilter presenceName;
    presenceName.setDetailDefinitionName(QContactName::DefinitionName);
    ids = cm->contacts(presenceName, s);
    output = convertIds(contacts, ids);
    QCOMPARE(output, expected);


    delete cm;
}

void tst_QContactManagerFiltering::multiSorting_data()
{
    QTest::addColumn<bool>("firstsort");
    QTest::addColumn<QString>("fsdefname");
    QTest::addColumn<QString>("fsfieldname");
    QTest::addColumn<int>("fsdirectioni");

    QTest::addColumn<bool>("secondsort");
    QTest::addColumn<QString>("ssdefname");
    QTest::addColumn<QString>("ssfieldname");
    QTest::addColumn<int>("ssdirectioni");

    QTest::addColumn<QString>("expected");


    QString es;

    QString firstname = QContactName::FieldFirst;
    QString lastname = QContactName::FieldLast;
    QString namedef = QContactName::DefinitionName;
    QString urldef = QContactUrl::DefinitionName;
    QString urlfield = QContactUrl::FieldUrl;
    QString phonedef = QContactPhoneNumber::DefinitionName;
    QString numberfield = QContactPhoneNumber::FieldNumber;

    QTest::newRow("1") << true << namedef << firstname << (int)(Qt::AscendingOrder)
                       << true << namedef << lastname << (int)(Qt::AscendingOrder)
                       << "abcdefg";
    QTest::newRow("2") << true << namedef << firstname << (int)(Qt::AscendingOrder)
                       << true << namedef << lastname << (int)(Qt::DescendingOrder)
                       << "abcdgfe";
    QTest::newRow("3") << true << namedef << firstname << (int)(Qt::DescendingOrder)
                       << true << namedef << lastname << (int)(Qt::AscendingOrder)
                       << "efgdcba";
    QTest::newRow("4") << true << namedef << firstname << (int)(Qt::DescendingOrder)
                       << true << namedef << lastname << (int)(Qt::DescendingOrder)
                       << "gfedcba";

    QTest::newRow("5") << true << namedef << firstname << (int)(Qt::AscendingOrder)
                       << false << namedef << lastname << (int)(Qt::AscendingOrder)
                       << "abcdefg";

    QTest::newRow("5b") << true << namedef << firstname << (int)(Qt::AscendingOrder)
                       << true << es << es << (int)(Qt::AscendingOrder)
                       << "abcdefg";

    QTest::newRow("6") << false << namedef << firstname << (int)(Qt::AscendingOrder)
                       << true << namedef << lastname << (int)(Qt::AscendingOrder)
                       << "bacdefg";

    QTest::newRow("7") << false << namedef << firstname << (int)(Qt::AscendingOrder)
                       << false << namedef << lastname << (int)(Qt::AscendingOrder)
                       << "abcdefg";

    QTest::newRow("8") << true << urldef << urlfield << (int)(Qt::AscendingOrder)
                       << false << urldef << urlfield << (int)(Qt::DescendingOrder)
                       << "gfedcba";

    QTest::newRow("8b") << true << urldef << urlfield << (int)(Qt::AscendingOrder)
                       << false << es << es << (int)(Qt::DescendingOrder)
                       << "gfedcba";

    QTest::newRow("9") << true << phonedef << numberfield << (int)(Qt::AscendingOrder)
                       << true << namedef << lastname << (int)(Qt::DescendingOrder)
                       << "abgfedc";

    QTest::newRow("10") << true << namedef << firstname << (int)(Qt::AscendingOrder)
                        << true << namedef << firstname << (int)(Qt::DescendingOrder)
                        << "abcdefg";


}

void tst_QContactManagerFiltering::multiSorting()
{
    QFETCH(bool, firstsort);
    QFETCH(QString, fsdefname);
    QFETCH(QString, fsfieldname);
    QFETCH(int, fsdirectioni);
    QFETCH(bool, secondsort);
    QFETCH(QString, ssdefname);
    QFETCH(QString, ssfieldname);
    QFETCH(int, ssdirectioni);
    QFETCH(QString, expected);

    Qt::SortOrder fsdirection = (Qt::SortOrder)fsdirectioni;
    Qt::SortOrder ssdirection = (Qt::SortOrder)ssdirectioni;

    /* Try the memory database first */
    QContactManager* cm = new QContactManager("memory");

    QContact e,f,g;
    QContactName n;
    n.setFirst("John");
    n.setLast("Smithee");
    e.saveDetail(&n);
    n.setLast("Smithey");
    f.saveDetail(&n);
    n.setLast("Smithy");
    g.saveDetail(&n);

    QList<QUniqueId> contacts = prepareModel(cm);
    QList<QUniqueId> ids;

    Q_ASSERT(cm->saveContact(&e));
    Q_ASSERT(cm->saveContact(&f));
    Q_ASSERT(cm->saveContact(&g));

    contacts = cm->contacts();
    QCOMPARE(contacts.count(), 7);

    /* Build the sort orders */
    QContactSortOrder fs;
    fs.setDetailDefinitionName(fsdefname, fsfieldname);
    fs.setDirection(fsdirection);
    QContactSortOrder ss;
    ss.setDetailDefinitionName(ssdefname, ssfieldname);
    ss.setDirection(ssdirection);
    QList<QContactSortOrder> sortOrders;
    if (firstsort)
        sortOrders.append(fs);
    if (secondsort)
        sortOrders.append(ss);

    ids = cm->contacts(sortOrders);
    QString output = convertIds(contacts, ids);
    QCOMPARE(output, expected);

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

    /* Recursive filtering */
    QTest::newRow("Recursive action 1") << "IntersectionRecursive" << es << -1 << QVariant(false) << es;
    QTest::newRow("Recursive action 2") << "UnionRecursive" << es << -1 << QVariant(false) << es;
    QTest::newRow("Recursive action 3") << "PairRecursive" << es << -1 << QVariant(false) << es;
    QTest::newRow("Recursive action 4") << "AnotherPairRecursive" << es << -1 << QVariant(false) << es;
    QTest::newRow("Recursive action 5") << "Recursive" << es << -1 << QVariant(false) << es;
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

void tst_QContactManagerFiltering::idListFiltering_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("expected");

    QString es;
    QTest::newRow("empty") << es << es;
    QTest::newRow("a") << "a" << "a";
    QTest::newRow("ab") << "ab" << "ab";
    QTest::newRow("aa") << "aa" << "a";
    QTest::newRow("ba") << "ba" << "ab";
    QTest::newRow("abcd") << "abcd" << "abcd";
    QTest::newRow("abcdefg") << "abcdefg" << "abcd";
}

void tst_QContactManagerFiltering::idListFiltering()
{
    QContactManager* cm = new QContactManager("memory");

    QFETCH(QString, input);
    QFETCH(QString, expected);

    QList<QUniqueId> contacts = prepareModel(cm);
    QList<QUniqueId> ids;

    QVERIFY(contacts.count() == 4);

    // 3 extra ids that (hopefully) won't exist
    QUniqueId e = 0x54555657;
    QUniqueId f = 0x96969696;
    QUniqueId g = 0x44335566;

    /* Convert the input to a list of ids */
    foreach(QChar c, input) {
        if (c == 'a')
            ids << contacts.at(0);
        else if (c == 'b')
            ids << contacts.at(1);
        else if (c == 'c')
            ids << contacts.at(2);
        else if (c == 'd')
            ids << contacts.at(3);
        else if (c == 'e')
            ids << e;
        else if (c == 'f')
            ids << f;
        else if (c == 'g')
            ids << g;
    }

    /* And do the search */
    QContactIdListFilter idf;
    idf.setIds(ids);

    // now reuse ids
    ids = cm->contacts(idf);

    QString output = convertIds(contacts, ids);
    QCOMPARE(output, expected);

    delete cm;
}

void tst_QContactManagerFiltering::invalidFiltering()
{
    QContactManager* cm = new QContactManager("memory");

    QList<QUniqueId> contacts = prepareModel(cm);
    QList<QUniqueId> ids;

    QVERIFY(contacts.count() == 4);

    QContactInvalidFilter f; // invalid

    ids = cm->contacts(f);

    QVERIFY(ids.count() == 0);

    // Try unions/intersections of invalids too
    ids = cm->contacts(f || f);
    QVERIFY(ids.count() == 0);

    ids = cm->contacts(f && f);
    QVERIFY(ids.count() == 0);

    delete cm;
}

void tst_QContactManagerFiltering::allFiltering()
{
    QContactManager* cm = new QContactManager("memory");

    QList<QUniqueId> contacts = prepareModel(cm);
    QList<QUniqueId> ids;

    QVERIFY(contacts.count() == 4);

    QContactFilter f; // default = permissive

    ids = cm->contacts(f);

    QVERIFY(ids.count() == 4);

    // Try unions/intersections of defaults
    ids = cm->contacts(f || f);
    QVERIFY(ids.count() == 4);

    ids = cm->contacts(f && f);
    QVERIFY(ids.count() == 4);

    delete cm;
}

Q_DECLARE_METATYPE(QContactManager*);
Q_DECLARE_METATYPE(QList<QUniqueId>);
void tst_QContactManagerFiltering::changelogFiltering_data()
{
    // Clean out the cleanup stack
    qDeleteAll(cleanupStack);
    cleanupStack.clear();

    QTest::addColumn<QContactManager *>("cm");
    QTest::addColumn<QList<QUniqueId> >("contacts");
    QTest::addColumn<int>("changeType");
    QTest::addColumn<QDateTime>("since");
    QTest::addColumn<QString>("expected");

    int added = (int)QContactChangeLogFilter::Added;
    int changed = (int)QContactChangeLogFilter::Changed;
    int removed = (int)QContactChangeLogFilter::Removed;

    QContactManager *manager = new QContactManager("memory");
    QList<QUniqueId> contacts;

    // Manually create contacts with the appropriate changes
    QContact a,b,c,d;
    a.setDisplayLabel("Alfred");
    b.setDisplayLabel("Bob");
    c.setDisplayLabel("Carol");
    d.setDisplayLabel("David");

    qDebug() << "Generating contacts with different timestamps, please wait..";
    manager->saveContact(&a);
    QTest::qSleep(2000);
    manager->saveContact(&b);
    QTest::qSleep(2000);
    manager->saveContact(&c);
    QTest::qSleep(2000);
    manager->saveContact(&d);

    // now update c
    QTest::qSleep(2000);
    c.setDisplayLabel("Clarence");
    manager->saveContact(&c);
    // b
    QTest::qSleep(2000);
    b.setDisplayLabel("Boris");
    manager->saveContact(&b);
    // a
    QTest::qSleep(2000);
    a.setDisplayLabel("Albert");
    manager->saveContact(&a);
    qDebug() << "Done!";

    contacts << a.id() << b.id() << c.id() << d.id();

    QDateTime ac = a.detail<QContactTimestamp>().created();
    QDateTime bc = b.detail<QContactTimestamp>().created();
    QDateTime cc = c.detail<QContactTimestamp>().created();
    QDateTime dc = d.detail<QContactTimestamp>().created();

    QDateTime am = a.detail<QContactTimestamp>().lastModified();
    QDateTime bm = b.detail<QContactTimestamp>().lastModified();
    QDateTime cm = c.detail<QContactTimestamp>().lastModified();
    QDateTime dm = d.detail<QContactTimestamp>().lastModified();

    QTest::newRow("Added since before start") << manager << contacts << added << ac.addSecs(-1) << "abcd";
    QTest::newRow("Added since first") << manager << contacts << added << ac << "abcd";
    QTest::newRow("Added since second") << manager << contacts << added << bc << "bcd";
    QTest::newRow("Added since third") << manager << contacts << added << cc << "cd";
    QTest::newRow("Added since fourth") << manager << contacts << added << dc << "d";
    QTest::newRow("Added since after fourth") << manager << contacts << added << dc.addSecs(1) << "";
    QTest::newRow("Added since first changed") << manager << contacts << added << am << "";
    QTest::newRow("Added since second changed") << manager << contacts << added << bm << "";
    QTest::newRow("Added since third changed") << manager << contacts << added << cm << "";
    QTest::newRow("Added since fourth changed") << manager << contacts << added << cm << "";

    QTest::newRow("Changed since before start") << manager << contacts << changed << ac.addSecs(-1) << "abcd";
    QTest::newRow("Changed since first") << manager << contacts << changed << ac << "abcd";
    QTest::newRow("Changed since second") << manager << contacts << changed << bc << "abcd";
    QTest::newRow("Changed since third") << manager << contacts << changed << cc << "abcd";
    QTest::newRow("Changed since fourth") << manager << contacts << changed << dc << "abcd";
    QTest::newRow("Changed since after fourth") << manager << contacts << changed << dc.addSecs(1) << "abc";
    QTest::newRow("Changed since first changed") << manager << contacts << changed << am << "a";
    QTest::newRow("Changed since second changed") << manager << contacts << changed << bm << "ab";
    QTest::newRow("Changed since third changed") << manager << contacts << changed << cm << "abc";
    QTest::newRow("Changed since fourth changed") << manager << contacts << changed << dm << "abcd";

    // These are currently useless..
    QTest::newRow("Removed since before start") << manager << contacts << removed << ac.addSecs(-1) << "";
    QTest::newRow("Removed since first") << manager << contacts << removed << ac << "";
    QTest::newRow("Removed since second") << manager << contacts << removed << bc << "";
    QTest::newRow("Removed since third") << manager << contacts << removed << cc << "";
    QTest::newRow("Removed since fourth") << manager << contacts << removed << dc << "";
    QTest::newRow("Removed since after fourth") << manager << contacts << removed << dc.addSecs(1) << "";

    cleanupStack << manager;
}

void tst_QContactManagerFiltering::changelogFiltering()
{
    QFETCH(int, changeType);
    QFETCH(QDateTime, since);
    QFETCH(QString, expected);
    QFETCH(QContactManager*, cm);
    QFETCH(QList<QUniqueId>, contacts);

    QList<QUniqueId> ids;

    QVERIFY(contacts.count() == 4);

    QContactChangeLogFilter clf((QContactChangeLogFilter::ChangeType)changeType);
    clf.setSince(since);

    ids = cm->contacts(clf);

    QString output = convertIds(contacts, ids);
    QCOMPARE(output, expected);
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
    def.setName("Integer");
    fields["value"].dataType = QVariant::Int;
    def.setFields(fields);
    Q_ASSERT(cm->saveDetailDefinition(def));

    /* Date time detail */
    def.setName("DateTime");
    fields["value"].dataType = QVariant::DateTime;
    def.setFields(fields);
    Q_ASSERT(cm->saveDetailDefinition(def));

    /* double detail */
    def.setName("Double");
    fields["value"].dataType = QVariant::Double;
    def.setFields(fields);
    Q_ASSERT(cm->saveDetailDefinition(def));

    /* bool */
    def.setName("Bool");
    fields["value"].dataType = QVariant::Bool;
    def.setFields(fields);
    Q_ASSERT(cm->saveDetailDefinition(def));

    /* long long */
    def.setName("LongLong");
    fields["value"].dataType = QVariant::LongLong;
    def.setFields(fields);
    Q_ASSERT(cm->saveDetailDefinition(def));

    /* unsigned long long */
    def.setName("ULongLong");
    fields["value"].dataType = QVariant::ULongLong;
    def.setFields(fields);
    Q_ASSERT(cm->saveDetailDefinition(def));

    /* date */
    def.setName("Date");
    fields["value"].dataType = QVariant::Date;
    def.setFields(fields);
    Q_ASSERT(cm->saveDetailDefinition(def));

    /* time */
    def.setName("Time");
    fields["value"].dataType = QVariant::Time;
    def.setFields(fields);
    Q_ASSERT(cm->saveDetailDefinition(def));

    /* uint */
    def.setName("UInt");
    fields["value"].dataType = QVariant::UInt;
    def.setFields(fields);
    Q_ASSERT(cm->saveDetailDefinition(def));

    /* char */
    def.setName("Char");
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
class QIntegerAction : public QContactAction
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

class QNumberAction : public QContactAction
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

class QBooleanAction : public QContactAction
{
    Q_OBJECT

public:
    QBooleanAction() {}
    ~QBooleanAction() {}

    QString actionName() const {return "Boolean";}
    QVariantMap metadata() const {return QVariantMap();}
    QString vendor() const {return "BooleanCo";}
    int implementationVersion() const {return 3;}

    QContactFilter contactFilter(const QVariant& value) const
    {
        if (value.isNull() || (value.type() == QVariant::Bool && value.toBool() == true)) {
            /* This only likes bools that are true */
            QContactDetailFilter df;
            df.setDetailDefinitionName("Bool", "value");
            df.setValue(true);
            return df;
        } else {
            return QContactInvalidFilter();
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

class RecursiveAction : public QContactAction
{
    Q_OBJECT

public:
    RecursiveAction() {}
    ~RecursiveAction() {}

    QString actionName() const {return "Recursive";}
    QVariantMap metadata() const {return QVariantMap();}
    QString vendor() const {return "RecursiveCo";}
    int implementationVersion() const {return 3;}

    QContactFilter contactFilter(const QVariant& value) const
    {
        /* Return a filter that selects us again.. */
        QContactActionFilter af;
        af.setActionName("Recursive");
        af.setVendor("RecursiveCo", 3);
        af.setValue(value);
        return af;
    }
    bool supportsDetail(const QContactDetail&) const
    {
        return false;
    }
    void performAction(const QContact&, const QContactDetail&)
    {

    }
};

class AnotherRecursiveAction : public RecursiveAction {
    Q_OBJECT

public:
    int implementationVersion() const {return 4;}
    QContactFilter contactFilter(const QVariant& value) const
    {
        Q_UNUSED(value);
        /* Slightly looser filter */
        QContactActionFilter af;
        af.setActionName("Recursive");
        return af;
    }
};

/* A pair that reference each other */
class PairRecursiveAction : public RecursiveAction {
    Q_OBJECT

public:
    QString actionName() const {return "PairRecursive";}
    QContactFilter contactFilter(const QVariant& value) const
    {
        Q_UNUSED(value);
        /* Slightly looser filter */
        QContactActionFilter af;
        af.setActionName("AnotherPairRecursive");
        return af;
    }
};

class AnotherPairRecursiveAction : public RecursiveAction {
    Q_OBJECT

public:
    QString actionName() const {return "AnotherPairRecursive";}
    QContactFilter contactFilter(const QVariant& value) const
    {
        Q_UNUSED(value);
        /* Slightly looser filter */
        QContactActionFilter af;
        af.setActionName("PairRecursive");
        return af;
    }
};

class IntersectionRecursiveAction : public RecursiveAction {
    Q_OBJECT

public:
    QString actionName() const {return "IntersectionRecursive";}
    QContactFilter contactFilter(const QVariant& value) const
    {
        Q_UNUSED(value);
        /* Slightly looser filter */
        QContactActionFilter af;
        af.setActionName("PairRecursive");
        return af && QContactFilter() && af;
    }
};

class UnionRecursiveAction : public RecursiveAction {
    Q_OBJECT

public:
    QString actionName() const {return "UnionRecursive";}
    QContactFilter contactFilter(const QVariant& value) const
    {
        Q_UNUSED(value);
        /* Slightly looser filter */
        QContactActionFilter af;
        af.setActionName("PairRecursive");
        return af || QContactInvalidFilter() || af;
    }
};

class FilterActionFactory : public QContactActionFactory
{
    Q_OBJECT
    Q_INTERFACES(QContactActionFactory)

public:
    FilterActionFactory() {}
    ~FilterActionFactory() {}

    QString name() const
    {
        return QString("FilterActionFactory");
    }

    QList<QContactActionFactory::ActionDescriptor> actionDescriptors() const
    {
        QList<QContactActionFactory::ActionDescriptor> ret;

        ret << ActionDescriptor("Number", "NumberCo", 42)
                << ActionDescriptor("Number", "IntegerCo", 5)
                << ActionDescriptor("Boolean", "BooleanCo", 3)
                << ActionDescriptor("Recursive", "RecursiveCo", 3)
                << ActionDescriptor("Recursive", "RecursiveCo", 4)
                << ActionDescriptor("PairRecursive", "RecursiveCo", 3)
                << ActionDescriptor("AnotherPairRecursive", "RecursiveCo", 3)
                << ActionDescriptor("IntersectionRecursive", "RecursiveCo", 3)
                << ActionDescriptor("UnionRecursive", "RecursiveCo", 3);

        return ret;
    }

    QContactAction* instance(const QContactActionFactory::ActionDescriptor& descriptor) const
    {
        if (descriptor.actionName == "Number") {
            if (descriptor.vendorName == "IntegerCo")
                return new QIntegerAction;
            else
                return new QNumberAction;
        } else if (descriptor.actionName == "Boolean") {
            return new QBooleanAction;
        } else if (descriptor.actionName == "Recursive") {
            if (descriptor.vendorVersion == 3)
                return new RecursiveAction;
            else
                return new AnotherRecursiveAction;
        } else if (descriptor.actionName == "PairRecursive") {
            return new PairRecursiveAction;
        } else if (descriptor.actionName == "AnotherPairRecursive") {
            return new AnotherPairRecursiveAction;
        } else if (descriptor.actionName == "IntersectionRecursive") {
            return new IntersectionRecursiveAction;
        } else {
            return new UnionRecursiveAction;
        }
    }
};

/* Statically import it (and a duplicate copy of it, purely for testing purposes) */
Q_EXPORT_PLUGIN2(contacts_testFilterActionFactory, FilterActionFactory);
Q_IMPORT_PLUGIN(contacts_testFilterActionFactory);

QTEST_MAIN(tst_QContactManagerFiltering)
#include "tst_qcontactmanagerfiltering.moc"
