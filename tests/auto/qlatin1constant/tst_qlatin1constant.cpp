/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qtcontactsglobal.h"
#include <QSet>
#include <QMetaType>
#include <QTypeInfo>

//TESTED_CLASS=
//TESTED_FILES=

QTM_USE_NAMESPACE

Q_DEFINE_LATIN1_CONSTANT(a, "a");
Q_DEFINE_LATIN1_CONSTANT(a2, "a");
Q_DEFINE_LATIN1_CONSTANT(b, "b");
Q_DEFINE_LATIN1_CONSTANT(b2, "b");
Q_DEFINE_LATIN1_CONSTANT(bb, "bb");
Q_DEFINE_LATIN1_CONSTANT(bb2, "bb");

Q_DEFINE_LATIN1_CONSTANT(z, "");
Q_DEFINE_LATIN1_CONSTANT(z2, "");

Q_DEFINE_LATIN1_CONSTANT(z3, "\0");
Q_DEFINE_LATIN1_CONSTANT(soup, "alphabet soup"); // but you can't have any

QLatin1String ln(0);
QLatin1String lz("");
QLatin1String la("a");
QLatin1String lb("b");
QLatin1String lbb("bb");
QLatin1String lsoup("alphabet soup");

QString sn;
QString sz("");
QString sa(la);
QString sb(lb);
QString sbb(lbb);
QString ssoup("alphabet soup");

class tst_QLatin1Constant: public QObject
{
Q_OBJECT

public:
    tst_QLatin1Constant();
    virtual ~tst_QLatin1Constant();

    // Overload testers
    int overloaded(const char *) {return 1;}
    //int overloaded(const QLatin1String& ) {return 2;}
    int overloaded(QLatin1String ) {return 3;}
    int overloaded(const QString& ) {return 4;}
    //int overloaded(QString ){return 5;}
    //template<int N> int overloaded(const QLatin1Constant<N>& ) {return 6;}
    template<int N> int overloaded(QLatin1Constant<N> ) {return 7;}
    int overloaded(const QVariant&) {return 8;}

    // More overload testers
    int overloaded2(QLatin1String) {return 3;}
    int overloaded2(const QString&) {return 4;}

    int overloaded3(const char*) {return 1;}
    int overloaded3(QLatin1String) {return 3;}

    int overloaded4(const char*) {return 1;}
    int overloaded4(const QString&) {return 4;}

    // Conversion testers
    bool charfunc(const char* str) {return qstrcmp(str, "alphabet soup") == 0;}
    bool latfunc(QLatin1String lat) {return qstrcmp(lat.latin1(), "alphabet soup") == 0;}
    bool latreffunc(const QLatin1String& lat) {return qstrcmp(lat.latin1(), "alphabet soup") == 0;}
    bool strfunc(QString str) {return str == QString::fromAscii("alphabet soup");}
    bool strreffunc(const QString& str) {return str == QString::fromAscii("alphabet soup");}
    bool varfunc(const QVariant& var) {return (var.type() == QVariant::String) && var.toString() == QString::fromAscii("alphabet soup");}

private slots:
    void hash();
    void conversion();
    void overloads();
    void equals();
    void latinEquals();
    void stringEquals();
    void ordering();
    void latinaccessor();
};

tst_QLatin1Constant::tst_QLatin1Constant()
{
}

tst_QLatin1Constant::~tst_QLatin1Constant()
{
}

void tst_QLatin1Constant::hash()
{
    // Test that if a == b, hash(a) == hash(b)
    // (also for ===)
    QVERIFY(qHash(a) == qHash(a));
    QVERIFY(qHash(a) == qHash(a2));
    QVERIFY(qHash(b) == qHash(b));
    QVERIFY(qHash(b) == qHash(b));
    QVERIFY(qHash(bb) == qHash(bb));
    QVERIFY(qHash(bb) == qHash(bb));

    // As a convenience, make sure that hashing
    // the same string gives the same results
    // no matter the storage
    QVERIFY(qHash(a) == qHash(la));
    QVERIFY(qHash(a) == qHash(sa));
}

void tst_QLatin1Constant::equals()
{
    // Check symmetry and dupes
    QVERIFY(a == a);
    QVERIFY(a == a2);
    QVERIFY(a2 == a);
    QVERIFY(b == b);
    QVERIFY(b == b2);
    QVERIFY(b2 == b2);
    QVERIFY(bb == bb);
    QVERIFY(bb == bb2);
    QVERIFY(bb2 == bb);

    QVERIFY(z == z);
    QVERIFY(z == z2);
    QVERIFY(z2 == z);

    // Now make sure that the length is taken into account
    QVERIFY(b != bb2);
    QVERIFY(bb2 != b);
    QVERIFY(a != z);
    QVERIFY(z != a);

    // and just in case something is really wrong
    QVERIFY(a != b);
    QVERIFY(b != a);
}

void tst_QLatin1Constant::latinaccessor()
{
    QVERIFY(a.chars == a.latin1());
    QVERIFY(z.latin1() == z.chars);
}

void tst_QLatin1Constant::latinEquals()
{
    // Test operator== with latin1 strings
    QVERIFY(a == la);
    QVERIFY(la == a);
    QVERIFY(a2 == la);
    QVERIFY(la == a2);
    QVERIFY(b == lb);
    QVERIFY(lb == b);
    QVERIFY(bb == lbb);
    QVERIFY(lbb == bb);

    QVERIFY(b != lbb);
    QVERIFY(lbb != b);

    QVERIFY(a != lb);
    QVERIFY(lb != a);

    QVERIFY(z == lz);
    QVERIFY((z == ln) == (lz == ln)); // QLatin1String(0) != QLatin1String("")
    QVERIFY(lz == z);
    QVERIFY((ln == z) == (ln == lz));
}

void tst_QLatin1Constant::stringEquals()
{
    // Test operator== with QStrings
    QVERIFY(a == sa);
    QVERIFY(sa == a);
    QVERIFY(a2 == sa);
    QVERIFY(sa == a2);
    QVERIFY(b == sb);
    QVERIFY(sb == b);
    QVERIFY(bb == sbb);
    QVERIFY(sbb == bb);

    QVERIFY(b != sbb);
    QVERIFY(sbb != b);

    QVERIFY(a != sb);
    QVERIFY(sb != a);

    QVERIFY(z == sz);
    QVERIFY((z == sn) == (sz == sn)); // QString(0) != QString("")
    QVERIFY(sz == z);
    QVERIFY((sn == z) == (sn == sz));
}

void tst_QLatin1Constant::conversion()
{
    QVERIFY(charfunc("alphabet soup"));
    QVERIFY(charfunc(soup.chars));
    QVERIFY(charfunc(soup.latin1()));

    QVERIFY(latfunc(lsoup));
    QVERIFY(latreffunc(lsoup));

    QVERIFY(strfunc(ssoup));
    QVERIFY(strreffunc(ssoup));

    // See if soup gets converted appropriately
    QVERIFY(latfunc(soup));
    QVERIFY(strfunc(soup));
    QVERIFY(latreffunc(soup));
    QVERIFY(strreffunc(soup));
    QVERIFY(varfunc(soup));

    // Now we also want to make sure that converting to QLatin1String doesn't copy the string
    QLatin1String lsoup2 = soup; // implicit operator QLatin1String
    QLatin1String lsoup3 = (QLatin1String) soup; // explicit operator QLatin1String
    QLatin1String lsoup4 = QLatin1String(soup); // implicit operator QLatin1String

    QVERIFY(lsoup2.latin1() == soup.latin1());
    QVERIFY(lsoup3.latin1() == soup.latin1());
    QVERIFY(lsoup4.latin1() == soup.latin1());
}

void tst_QLatin1Constant::overloads()
{
    QVERIFY(overloaded("alphabet soup") == 1);
    QVERIFY(overloaded(soup) == 7);
    QVERIFY(overloaded(lsoup) == 2 || overloaded(lsoup) == 3);
    QVERIFY(overloaded(ssoup) == 4 || overloaded(ssoup) == 5);

    QVERIFY(overloaded2(lsoup) == 3);
    QVERIFY(overloaded2(ssoup) == 4);
    QCOMPARE(overloaded2(soup.latin1()), 4); // XXX grr, can't call with just soup [ambiguous], this goes to QString

    QVERIFY(overloaded3(lsoup) == 3);
    QCOMPARE(overloaded3(soup), 3); // XXX this goes with QLatin1String

    QVERIFY(overloaded4(ssoup) == 4);
    QCOMPARE(overloaded4(soup), 4); // XXX this goes with QString
}

void tst_QLatin1Constant::ordering()
{
    QVERIFY(z < a);
    QVERIFY(not (a < z));
    QVERIFY(a < b);
    QVERIFY(not (b < a));
    QVERIFY(a < bb);
    QVERIFY(not (bb < a));
    QVERIFY(b < bb);
    QVERIFY(not (bb < b));

    QVERIFY(not (a < a));
    QVERIFY(not (z < z));
}

QTEST_MAIN(tst_QLatin1Constant)
#include "tst_qlatin1constant.moc"
