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

class tst_QContactDetail : public QObject
{
Q_OBJECT

public:
    tst_QContactDetail();
    virtual ~tst_QContactDetail();

public slots:
    void init();
    void cleanup();

private slots:
    void classHierarchy();
    void templates();
    void attributes();
    void values();
};

tst_QContactDetail::tst_QContactDetail()
{
}

tst_QContactDetail::~tst_QContactDetail()
{
}

void tst_QContactDetail::init()
{
}

void tst_QContactDetail::cleanup()
{
}

/* Test class that doesn't do the right thing */
class MaliciousDetail : public QContactDetail
{
public:
    MaliciousDetail() : QContactDetail("malicious") {}
    void doAssign(const QContactDetail& other) {assign(other, "malicious");}
};

void tst_QContactDetail::classHierarchy()
{
    QContactDetail f1;
    QContactDetail f2;

    QVERIFY(f1.isEmpty());
    QVERIFY(f2.isEmpty());

    QContactPhoneNumber p1;
    p1.setNumber("123456");
    QVERIFY(!p1.isEmpty());
    QVERIFY(p1.definitionName() == QContactPhoneNumber::DefinitionName);

    QContactName m1;
    m1.setFirst("Bob");
    QVERIFY(!m1.isEmpty());
    QVERIFY(m1.definitionName() == QContactName::DefinitionName);

    QVERIFY(p1 != m1);
    QVERIFY(f1 == f2);

    f1 = p1; // f1 is a phonenumber
    QVERIFY(f1 == p1);

    f1 = f1; // assign to itself
    QVERIFY(f1 == f1);
    QVERIFY(f1 == p1);
    QVERIFY(f1 != f2);
    QVERIFY(p1 != f2);

    QCOMPARE(f1.error(), QContactDetail::NoError);


    p1 = p1; // assign leaf class to itself
    QVERIFY(p1 == p1);
    QVERIFY(f1 == p1);
    QVERIFY(p1 == f1);

    f2 = f1; // f2 = f1 = phonenumber
    QVERIFY(f1 == f2);
    QVERIFY(f2 == f1);
    QVERIFY(f2 == p1);
    QVERIFY(f1 == p1);

    f1 = m1; // f1 = name, f2 = phonenumber
    QVERIFY(f1 == m1);
    QVERIFY(f1 != f2);
    QVERIFY(f2 == p1);

    QCOMPARE(f2.error(), QContactDetail::NoError);

    QContactPhoneNumber p2(f2); // p2 = f2 = phonenumber
    QVERIFY(p1 == p2);
    QVERIFY(p1 == f2);
    QCOMPARE(p2.number(), p1.number());
    QCOMPARE(p2.number(), QString("123456"));

    p2 = p1; // phone number to phone number
    QVERIFY(p1 == p2);
    QVERIFY(p1 == f2);
    QCOMPARE(p2.number(), p1.number());
    QCOMPARE(p2.number(), QString("123456"));

    p2.setNumber("5678");
    QVERIFY(p1 != p2);
    QVERIFY(p1 == f2);
    QVERIFY(p2 != f2);

    /* Bad assignment */
    p2 = m1; // assign a name to a phone number
    QVERIFY(p2 != m1);
    QCOMPARE(p2.error(), QContactDetail::IncompatibleAssignmentError);
    QVERIFY(p2.definitionName() == QContactPhoneNumber::DefinitionName); // should still be a phone number though
    QVERIFY(p2.isEmpty());

    /* copy ctor */
    QContactName m2(m1);
    QVERIFY(m2 == m1);

    /* another bad assignment */
    m2 = p2; // phone number to a name
    QVERIFY(m2 != m1);
    QCOMPARE(m2.error(), QContactDetail::IncompatibleAssignmentError);
    QVERIFY(m2.definitionName() == QContactName::DefinitionName);
    QVERIFY(m2.isEmpty());

    /* Check attributes are considered for equality */
    p2 = p1;
    p2.setAttribute(QContactPhoneNumber::AttributeContext, QContactPhoneNumber::AttributeContextHome);
    QVERIFY(p1 != p2);
    p2.removeAttribute(QContactPhoneNumber::AttributeContext);
    QVERIFY(p1 == p2);

    /* Copy ctor from valid type */
    QContactDetail f3(p2);
    QVERIFY(f3 == p2);
    QCOMPARE(f3.error(), QContactDetail::NoError);
    QVERIFY(f3.definitionName() == QContactPhoneNumber::DefinitionName);

    /* Copy ctor from invalid type */
    QContactPhoneNumber p3(m1);
    QVERIFY(p3 != m1);
    QCOMPARE(p3.error(), QContactDetail::IncompatibleAssignmentError);
    QVERIFY(p3.definitionName() == QContactPhoneNumber::DefinitionName);
    QVERIFY(p3.isEmpty());

    /* Copy ctore from invalid type, through base type */
    f3 = m1;
    QContactPhoneNumber p4(f3);
    QVERIFY(p4 != f3);
    QCOMPARE(p4.error(), QContactDetail::IncompatibleAssignmentError);
    QVERIFY(p4.definitionName() == QContactPhoneNumber::DefinitionName);
    QVERIFY(p4.isEmpty());

    // Make sure errors don't move from object to object

    /* Get some objects with no errors */
    QContactDetail goodbase("something different");
    QCOMPARE(goodbase.error(), QContactDetail::NoError);

    QContactPhoneNumber goodleaf;
    QCOMPARE(goodleaf.error(), QContactDetail::NoError);

    QContactDetail goodleafbase(goodleaf);
    QCOMPARE(goodleaf.error(), QContactDetail::NoError);
    QCOMPARE(goodleafbase.error(), QContactDetail::NoError);
    QVERIFY(goodleaf == goodleafbase);

    QContactName goodleaf2;
    QCOMPARE(goodleaf2.error(), QContactDetail::NoError);

    /* Get a base class with an error */
    QContactDetail errorbase(goodbase);
    QCOMPARE(errorbase.value("nonexistent"), QString());
    QCOMPARE(errorbase.error(), QContactDetail::MissingValueError);

    /* and a leaf class with an error */
    QContactPhoneNumber errorleaf;
    QCOMPARE(errorleaf.value("nonexistent"), QString());
    QCOMPARE(errorleaf.error(), QContactDetail::MissingValueError);

    QContactName errorleaf2;
    QCOMPARE(errorleaf2.value("nonexistent"), QString());
    QCOMPARE(errorleaf2.error(), QContactDetail::MissingValueError);

    /* and a leaf class as a base with an error */
    QContactDetail errorleafbase(errorleaf);
    QCOMPARE(errorleafbase.value("nonexistent"), QString());
    QCOMPARE(errorleafbase.error(), QContactDetail::MissingValueError);

    /* Make sure nothing changed */
    QCOMPARE(errorbase.error(), QContactDetail::MissingValueError);
    QCOMPARE(errorleaf.error(), QContactDetail::MissingValueError);
    QCOMPARE(errorleaf2.error(), QContactDetail::MissingValueError);
    QCOMPARE(errorleafbase.error(), QContactDetail::MissingValueError);

    QCOMPARE(goodbase.error(), QContactDetail::NoError);
    QCOMPARE(goodleaf.error(), QContactDetail::NoError);
    QCOMPARE(goodleaf2.error(), QContactDetail::NoError);
    QCOMPARE(goodleafbase.error(), QContactDetail::NoError);

    /* === Base and Base of same type === */
    /* Base copy ctor with no error */
    QContactDetail gb(goodbase);
    QCOMPARE(gb.error(), QContactDetail::NoError);
    QCOMPARE(goodbase.error(), QContactDetail::NoError);

    /* Base op = with no error */
    QContactDetail gb2;
    gb2 = goodbase;
    QCOMPARE(gb2.error(), QContactDetail::NoError);

    /* base copy ctor with error */
    QContactDetail eb(errorbase);
    QCOMPARE(eb.error(), QContactDetail::NoError);

    /* base op = with error */
    QContactDetail eb2;
    eb2 = errorbase;
    QCOMPARE(eb2.error(), QContactDetail::NoError);

    /* === Base and base of another type (operator =) === */

    /* You can assign base contact details, but not leaf classes... */
    QContactDetail gbb(goodbase);
    gbb = goodleafbase;
    QCOMPARE(gbb.error(), QContactDetail::NoError);

    gbb = errorleafbase;
    QCOMPARE(gbb.error(), QContactDetail::NoError);

    /* === Base and  Leaf === */
    /* base copy ctor of leaf with no error */
    QContactDetail gb3(goodleaf);
    QCOMPARE(gb3.error(), QContactDetail::NoError);

    /* base op = of leaf with no error */
    QContactDetail gb4;
    gb4 = goodleaf;
    QCOMPARE(gb4.error(), QContactDetail::NoError);

    /* base copy ctor of leaf with error */
    QContactDetail eb3(errorleaf);
    QCOMPARE(eb3.error(), QContactDetail::NoError);

    /* base op = of leaf with error */
    QContactDetail eb4;
    eb4 = errorleaf;
    QCOMPARE(eb4.error(), QContactDetail::NoError);

    /* === Base of leaf type and leaf (operator =) === */
    QContactDetail leafbase(goodleafbase);
    leafbase = goodleaf;
    QCOMPARE(leafbase.error(), QContactDetail::NoError);

    leafbase = errorleaf;
    QCOMPARE(leafbase.error(), QContactDetail::NoError);

    /* === Leaf and base === */
    /* copy ctor, no error */
    QContactPhoneNumber gl(goodbase);
    QCOMPARE(gl.error(), QContactDetail::IncompatibleAssignmentError);

    QContactPhoneNumber gl2;
    gl2 = goodbase;
    QCOMPARE(gl2.error(), QContactDetail::IncompatibleAssignmentError);

    QContactPhoneNumber el(errorbase);
    QCOMPARE(el.error(), QContactDetail::IncompatibleAssignmentError);

    QContactPhoneNumber el2;
    el2 = errorbase;
    QCOMPARE(el2.error(), QContactDetail::IncompatibleAssignmentError);

    /* === Leaf and base of leaf with same type === */
    QContactPhoneNumber gbl(goodleafbase);
    QCOMPARE(gbl.error(), QContactDetail::NoError);

    QContactPhoneNumber gbl2;
    gbl2 = goodleafbase;
    QCOMPARE(gbl2.error(), QContactDetail::NoError);

    QContactPhoneNumber ebl(errorleafbase);
    QCOMPARE(ebl.error(), QContactDetail::NoError);

    QContactPhoneNumber ebl2;
    ebl2 = errorleafbase;
    QCOMPARE(ebl2.error(), QContactDetail::NoError);

    /* === Leaf and Leaf === */
    QContactPhoneNumber gl3(goodleaf);
    QCOMPARE(gl3.error(), QContactDetail::NoError);

    QContactPhoneNumber gl4;
    gl4 = goodleaf;
    QCOMPARE(gl4.error(), QContactDetail::NoError);

    QContactPhoneNumber el3(errorleaf);
    QCOMPARE(el3.error(), QContactDetail::NoError);

    QContactPhoneNumber el4;
    el4 = errorleaf;
    QCOMPARE(el4.error(), QContactDetail::NoError);

    /* === Leaf and wrong leaf === */
    QContactPhoneNumber gl5(goodleaf2);
    QCOMPARE(gl5.error(), QContactDetail::IncompatibleAssignmentError);

    QContactPhoneNumber gl6;
    gl6 = goodleaf2;
    QCOMPARE(gl6.error(), QContactDetail::IncompatibleAssignmentError);

    QContactPhoneNumber el5(errorleaf2);
    QCOMPARE(el5.error(), QContactDetail::IncompatibleAssignmentError);

    QContactPhoneNumber el6;
    el6 = errorleaf2;
    QCOMPARE(el6.error(), QContactDetail::IncompatibleAssignmentError);

    /* === Leaf and wrong base leaf === */
    QContactName gbl5(goodleafbase);
    QCOMPARE(gbl5.error(), QContactDetail::IncompatibleAssignmentError);

    QContactName gbl6;
    gbl6 = goodleafbase;
    QCOMPARE(gbl6.error(), QContactDetail::IncompatibleAssignmentError);

    QContactName ebl5(errorleafbase);
    QCOMPARE(ebl5.error(), QContactDetail::IncompatibleAssignmentError);

    QContactName ebl6;
    ebl6 = errorleafbase;
    QCOMPARE(ebl6.error(), QContactDetail::IncompatibleAssignmentError);

    /* Make sure nothing changed again */
    QCOMPARE(errorbase.error(), QContactDetail::MissingValueError);
    QCOMPARE(errorleaf.error(), QContactDetail::MissingValueError);
    QCOMPARE(errorleaf2.error(), QContactDetail::MissingValueError);
    QCOMPARE(errorleafbase.error(), QContactDetail::MissingValueError);

    QCOMPARE(goodbase.error(), QContactDetail::NoError);
    QCOMPARE(goodleaf.error(), QContactDetail::NoError);
    QCOMPARE(goodleaf2.error(), QContactDetail::NoError);
    QCOMPARE(goodleafbase.error(), QContactDetail::NoError);

    /* Try a reference */
    QContactDetail& ref = p1;
    QVERIFY(p1.number() == "123456");
    QVERIFY(p1.value(QContactPhoneNumber::FieldNumber) == "123456");
    QVERIFY(ref.value(QContactPhoneNumber::FieldNumber) == "123456");
    QVERIFY(p1 == ref);
    QVERIFY(ref == p1);

    /* Try changing the original */
    p1.setNumber("56789");
    QVERIFY(p1.number() == "56789");
    QVERIFY(p1.value(QContactPhoneNumber::FieldNumber) == "56789");
    QVERIFY(ref.value(QContactPhoneNumber::FieldNumber) == "56789");
    QVERIFY(p1 == ref);
    QVERIFY(ref == p1);

    /* Try changing the reference */
    ref.setValue(QContactPhoneNumber::FieldNumber, "654321");
    QVERIFY(p1.number() == "654321");
    QVERIFY(p1.value(QContactPhoneNumber::FieldNumber) == "654321");
    QVERIFY(ref.value(QContactPhoneNumber::FieldNumber) == "654321");
    QVERIFY(p1 == ref);
    QVERIFY(ref == p1);

    /* Random other test */
    MaliciousDetail md;
    QVERIFY(md.setValue("key", "value"));
    QVERIFY(!md.isEmpty());
    md.doAssign(md);
    QVERIFY(!md.isEmpty());
    QVERIFY(md.value("key") == "value");

}

void tst_QContactDetail::templates()
{
    QContact c;
    QContactPhoneNumber p1, p2;
    p1.setNumber("1234");
    p2.setNumber("5678");
    QVERIFY(c.saveDetail(&p1));
    QVERIFY(c.saveDetail(&p2));

    QList<QContactDetail> l = c.details("PhoneNumber");

    QCOMPARE(l.count(), 2);
    QCOMPARE(QContactPhoneNumber(l.at(0)), p1);
    QCOMPARE(QContactPhoneNumber(l.at(1)), p2);

    QList<QContactPhoneNumber> l2 = c.details<QContactPhoneNumber>();
    QCOMPARE(l2.count(), 2);
    QCOMPARE(l2.at(0), p1);
    QCOMPARE(l2.at(1), p2);
}

void tst_QContactDetail::attributes()
{
    QContactDetail d;
    QVERIFY(d.attributes().count() == 0);

    QMap<QString, QString> mine;
    mine.insert("string", "string");
    mine.insert("one", "1");
    mine.insert("two", "2");

    QVERIFY(d.setAttributes(QMap<QString, QString>()));
    QVERIFY(d.attributes().count() == 0);

    QVERIFY(d.setAttribute("exists", "value"));
    QCOMPARE(d.attributes().count(), 1);
    QCOMPARE(d.attribute("exists"), QString("value"));

    QVERIFY(d.setAttributes(mine));
    QCOMPARE(mine, d.attributes());

    QCOMPARE(d.attribute("string"), QString("string"));
    QCOMPARE(d.attribute("one"), QString("1"));
    QCOMPARE(d.attribute("two"), QString("2"));
    QCOMPARE(d.attribute("nonexistent"), QString());
    QCOMPARE(d.attribute(QString()), QString());
    QCOMPARE(d.attribute("exists"), QString()); // should not exist any more

    QVERIFY(d.setAttribute("three", "3"));
    QCOMPARE(d.attribute("three"), QString("3"));

    QVERIFY(d.removeAttribute("three"));
    QCOMPARE(d.attribute("three"), QString());

    QCOMPARE(d.attributes(), mine);
}

void tst_QContactDetail::values()
{
    QContactDetail p;

    QCOMPARE(p.values(), QVariantMap());

    QDateTime dt = QDateTime::currentDateTime();
    QTime t = dt.time();
    t.setHMS(t.hour(), t.minute(), t.second(), 0); // milliseconds don't round trip through ISODate
    dt.setTime(t);
    QDate d = dt.date();

    QDateTime ddt(d); // DateTime version of a Date (QTime())

    p.setValue("string", "This is a string");
    p.setValue("date", d);
    p.setValue("datetime", dt);
    p.setValue("int", (int)6);

    p.setValue("stringdate", d.toString(Qt::ISODate));
    p.setValue("stringdatetime", dt.toString(Qt::ISODate));
    p.setValue("stringint", "123");

    /* Presence test */
    QVERIFY(p.hasValue("string"));
    QVERIFY(p.hasValue("date"));
    QVERIFY(p.hasValue("datetime"));
    QVERIFY(p.hasValue("int"));
    QVERIFY(p.hasValue("stringdate"));
    QVERIFY(p.hasValue("stringdatetime"));
    QVERIFY(p.hasValue("stringint"));
    QCOMPARE(p.error(), QContactDetail::NoError);
    QVERIFY(!p.hasValue("non existent field"));
    QCOMPARE(p.error(), QContactDetail::MissingValueError);

    /* String accessors */
    QCOMPARE(p.value("string"), QString("This is a string"));
    QCOMPARE(p.value("date"), d.toString(Qt::ISODate));
    QCOMPARE(p.value("datetime"), dt.toString(Qt::ISODate));
    QCOMPARE(p.value("int"), QString("6"));
    QCOMPARE(p.value("stringdate"), d.toString(Qt::ISODate));
    QCOMPARE(p.value("stringdatetime"), dt.toString(Qt::ISODate));
    QCOMPARE(p.value("stringint"), QString("123"));

    /* Variant accessor */
    QCOMPARE(p.variantValue("string"), QVariant(QString("This is a string")));
    QCOMPARE(p.variantValue("date"), QVariant(d));
    QCOMPARE(p.variantValue("datetime"), QVariant(dt));
    QCOMPARE(p.variantValue("int"), QVariant((int)6));
    QCOMPARE(p.variantValue("stringdate"), QVariant(d.toString(Qt::ISODate)));
    QCOMPARE(p.variantValue("stringdatetime"), QVariant(dt.toString(Qt::ISODate)));
    QCOMPARE(p.variantValue("stringint"), QVariant(QString("123")));

    /* Typed accessors, string first */
    QCOMPARE(p.value<QString>("string"), QString("This is a string"));
    QCOMPARE(p.value<QString>("date"), d.toString(Qt::ISODate));
    QCOMPARE(p.value<QString>("datetime"), dt.toString(Qt::ISODate));
    QCOMPARE(p.value<QString>("int"), QString("6"));
    QCOMPARE(p.value<QString>("stringdate"), d.toString(Qt::ISODate));
    QCOMPARE(p.value<QString>("stringdatetime"), dt.toString(Qt::ISODate));
    QCOMPARE(p.value<QString>("stringint"), QString("123"));

    /* Now individual original types */
    QCOMPARE(p.value<QDate>("date"), d);
    QCOMPARE(p.value<QDateTime>("datetime"), dt);
    QCOMPARE(p.value<int>("int"), 6);

    /* Now cross types that should fail */
    QDate id;
    QDateTime idt;
    QCOMPARE(p.value<QDate>("string"), id);
    QCOMPARE(p.value<QDate>("int"), id);
    QCOMPARE(p.value<QDate>("stringint"), id);
    QCOMPARE(p.value<QDateTime>("string"), idt);
    QCOMPARE(p.value<QDateTime>("int"), idt);
    QCOMPARE(p.value<QDateTime>("stringint"), idt);
    QCOMPARE(p.value<int>("date"), 0);
    QCOMPARE(p.value<int>("datetime"), 0);
    QCOMPARE(p.value<int>("string"), 0);
    QCOMPARE(p.value<int>("stringdate"), 0);
    QCOMPARE(p.value<int>("stringdatetime"), 0);

    /* Cross types that should work.. */
    QCOMPARE(p.value<int>("stringint"), 123);
    QCOMPARE(p.value<QDate>("stringdate"), d);
    QCOMPARE(p.value<QDateTime>("stringdatetime"), dt);
    QCOMPARE(p.value<QDate>("datetime"), d);
    QCOMPARE(p.value<QDate>("stringdatetime"), d);
    QCOMPARE(p.value<QDateTime>("date"), ddt);
    QCOMPARE(p.value<QDateTime>("stringdate"), ddt);

    /* Now set everything again */
    QVariantMap values;
    QVERIFY(p.setValues(values));

    QCOMPARE(p.values(), values);
    QVERIFY(p.values().count() == 0);
    QVERIFY(!p.hasValue("string"));
    QVERIFY(!p.hasValue("date"));
    QVERIFY(!p.hasValue("datetime"));
    QVERIFY(!p.hasValue("int"));
    QVERIFY(!p.hasValue("stringdate"));
    QVERIFY(!p.hasValue("stringdatetime"));
    QVERIFY(!p.hasValue("stringint"));
    QVERIFY(!p.hasValue("non existent field"));

    QVERIFY(p.value("string") == QString());
    QVERIFY(p.variantValue("string") == QVariant());

    values.insert("string", "This is a string");
    values.insert("date", d);
    values.insert("datetime", dt);
    values.insert("int", (int)6);

    values.insert("stringdate", d.toString(Qt::ISODate));
    values.insert("stringdatetime", dt.toString(Qt::ISODate));
    values.insert("stringint", "123");
    values.insert("string", QString("This is a string"));

    QVERIFY(p.setValues(values));
    QCOMPARE(p.values(), values);

    /* Now repeat the tests with our bulk set map */
    QVERIFY(p.hasValue("string"));
    QVERIFY(p.hasValue("date"));
    QVERIFY(p.hasValue("datetime"));
    QVERIFY(p.hasValue("int"));
    QVERIFY(p.hasValue("stringdate"));
    QVERIFY(p.hasValue("stringdatetime"));
    QVERIFY(p.hasValue("stringint"));
    QVERIFY(!p.hasValue("non existent field"));

    /* String accessors */
    QCOMPARE(p.value("string"), QString("This is a string"));
    QCOMPARE(p.value("date"), d.toString(Qt::ISODate));
    QCOMPARE(p.value("datetime"), dt.toString(Qt::ISODate));
    QCOMPARE(p.value("int"), QString("6"));
    QCOMPARE(p.value("stringdate"), d.toString(Qt::ISODate));
    QCOMPARE(p.value("stringdatetime"), dt.toString(Qt::ISODate));
    QCOMPARE(p.value("stringint"), QString("123"));

    /* Typed accessors, string first */
    QCOMPARE(p.value<QString>("string"), QString("This is a string"));
    QCOMPARE(p.value<QString>("date"), d.toString(Qt::ISODate));
    QCOMPARE(p.value<QString>("datetime"), dt.toString(Qt::ISODate));
    QCOMPARE(p.value<QString>("int"), QString("6"));
    QCOMPARE(p.value<QString>("stringdate"), d.toString(Qt::ISODate));
    QCOMPARE(p.value<QString>("stringdatetime"), dt.toString(Qt::ISODate));
    QCOMPARE(p.value<QString>("stringint"), QString("123"));

    /* Now individual original types */
    QCOMPARE(p.value<QDate>("date"), d);
    QCOMPARE(p.value<QDateTime>("datetime"), dt);
    QCOMPARE(p.value<int>("int"), 6);

    /* Now cross types that should fail */
    QCOMPARE(p.value<QDate>("string"), id);
    QCOMPARE(p.value<QDate>("int"), id);
    QCOMPARE(p.value<QDate>("stringint"), id);
    QCOMPARE(p.value<QDateTime>("string"), idt);
    QCOMPARE(p.value<QDateTime>("int"), idt);
    QCOMPARE(p.value<QDateTime>("stringint"), idt);
    QCOMPARE(p.value<int>("date"), 0);
    QCOMPARE(p.value<int>("datetime"), 0);
    QCOMPARE(p.value<int>("string"), 0);
    QCOMPARE(p.value<int>("stringdate"), 0);
    QCOMPARE(p.value<int>("stringdatetime"), 0);

    /* Cross types that should work.. */
    QCOMPARE(p.value<int>("stringint"), 123);
    QCOMPARE(p.value<QDate>("stringdate"), d);
    QCOMPARE(p.value<QDateTime>("stringdatetime"), dt);
    QCOMPARE(p.value<QDate>("datetime"), d);
    QCOMPARE(p.value<QDate>("stringdatetime"), d);
    QCOMPARE(p.value<QDateTime>("date"), ddt);
    QCOMPARE(p.value<QDateTime>("stringdate"), ddt);

    /* Reset again */
    QVERIFY(p.setValues(QVariantMap()));

    /* Check that we can add a null variant */
    QVERIFY(p.setValue("nullvariant", QVariant()));
    QVERIFY(p.hasValue("nullvariant"));
    QCOMPARE(p.value("nullvariant"), QString());
    QCOMPARE(p.variantValue("nullvariant"), QVariant());
    QVERIFY(p.removeValue("nullvariant"));
    QVERIFY(p.values().count() == 0);

    /* Check that adding a value, then setting it to null updates it */
    QVERIFY(p.setValue("string", QString("string value")));
    QCOMPARE(p.values().count(), 1);
    QCOMPARE(p.value("string"), QString("string value"));
    QVERIFY(p.setValue("string", QVariant()));
    QCOMPARE(p.values().count(), 1);
    QVERIFY(p.hasValue("string"));
    QVERIFY(p.removeValue("string"));
    QCOMPARE(p.values().count(), 0);

    /* See if adding a null QString triggers the same behaviour */
    QVERIFY(p.setValue("string", QString("string value")));
    QCOMPARE(p.values().count(), 1);
    QCOMPARE(p.value("string"), QString("string value"));
    QVERIFY(p.setValue("string", QString()));
    QCOMPARE(p.values().count(), 1);
    QVERIFY(p.hasValue("string"));
    QVERIFY(p.removeValue("string"));
    QCOMPARE(p.values().count(), 0);

    /* Check accessing a missing value */
    QCOMPARE(p.value("nonexistent"), QString());
    QCOMPARE(p.error(), QContactDetail::MissingValueError);
    QVERIFY(p.setValue("nonexistent", "changed my mind"));
    QCOMPARE(p.error(), QContactDetail::NoError);
    QCOMPARE(p.value("nonexistent"), QString("changed my mind"));
    QCOMPARE(p.error(), QContactDetail::NoError);

    /* Check removing a missing value */
    QVERIFY(!p.removeValue("does not exist"));
    QCOMPARE(p.error(), QContactDetail::MissingValueError);
}


QTEST_MAIN(tst_QContactDetail)
#include "tst_qcontactdetail.moc"
