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

#include "qtcontacts.h"
#include "qcontactmanagerdataholder.h" //QContactManagerDataHolder
#include <QSet>

//TESTED_CLASS=
//TESTED_FILES=

QTM_USE_NAMESPACE
class tst_QContactDetail : public QObject
{
Q_OBJECT

public:
    tst_QContactDetail();
    virtual ~tst_QContactDetail();
private:
    QContactManagerDataHolder managerDataHolder;

public slots:
    void init();
    void cleanup();

private slots:
    void classHierarchy();
    void assignment();
    void templates();
    void contexts();
    void values();
    void hash();
    void traits();
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
    m1.setFirstName("Bob");
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

    p2.setNumber("5678"); // NOTE: implicitly shared, this has caused a detach so p1 != 2
    QVERIFY(p1 != p2);
    QVERIFY(p1 == f2);
    QVERIFY(p2 != f2);
    QCOMPARE(p2.number(), QString("5678"));
    QCOMPARE(p1.number(), QString("123456"));

    /* Bad assignment */
    p2 = m1; // assign a name to a phone number
    QVERIFY(p2 != m1);
    QVERIFY(p2.definitionName() == QContactPhoneNumber::DefinitionName); // should still be a phone number though
    QVERIFY(p2.isEmpty());

    /* copy ctor */
    QContactName m2(m1);
    QVERIFY(m2 == m1);

    /* another bad assignment */
    m2 = p2; // phone number to a name
    QVERIFY(m2 != m1);
    QVERIFY(m2.definitionName() == QContactName::DefinitionName);
    QVERIFY(m2.isEmpty());

    /* Check contexts are considered for equality */
    p2 = QContactPhoneNumber(); // new id / detach
    p2.setNumber(p1.number());
    p2.setContexts(QContactDetail::ContextHome);
    QVERIFY(p1 != p2);
    p2.removeValue(QContactDetail::FieldContext); // note, context is a value.
    QVERIFY(p1 == p2); // different ids but same values should be equal

    /* Copy ctor from valid type */
    QContactDetail f3(p2);
    QVERIFY(f3 == p2);
    QVERIFY(f3.definitionName() == QContactPhoneNumber::DefinitionName);

    /* Copy ctor from invalid type */
    QContactPhoneNumber p3(m1);
    QVERIFY(p3 != m1);
    QVERIFY(p3.definitionName() == QContactPhoneNumber::DefinitionName);
    QVERIFY(p3.isEmpty());

    /* Copy ctore from invalid type, through base type */
    f3 = m1;
    QContactPhoneNumber p4(f3);
    QVERIFY(p4 != f3);
    QVERIFY(p4.definitionName() == QContactPhoneNumber::DefinitionName);
    QVERIFY(p4.isEmpty());

    /* Try a reference */
    p1.setNumber("123456");
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

void tst_QContactDetail::assignment()
{
    QContactPhoneNumber p1, p2;
    p1.setNumber("12345");
    p2.setNumber("54321");
    QVERIFY(p1 != p2);

    p1 = p2;
    QVERIFY(p1 == p2);

    QContactEmailAddress e1;
    e1.setEmailAddress("test@nokia.com");
    QVERIFY(e1 != p1);
    e1 = p1;
    QVERIFY(e1 != p1); // assignment across types shouldn't work
    QVERIFY(e1.emailAddress() == QString()); // should reset the detail
    QCOMPARE(e1, QContactEmailAddress());
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

void tst_QContactDetail::contexts()
{
    QContactDetail d;
    QVERIFY(d.contexts().count() == 0);

    // test set contexts
    d.setContexts(QContactDetail::ContextWork);
    QVERIFY(d.contexts().count() == 1);
    QVERIFY(d.contexts().contains(QContactDetail::ContextWork));
    QVERIFY(!d.contexts().contains(QContactDetail::ContextOther));
    QVERIFY(!d.contexts().contains(QContactDetail::ContextHome));

    QStringList contexts;
    contexts.append(QContactDetail::ContextHome);
    contexts.append(QContactDetail::ContextOther);
    contexts.append("CustomContext"); // note: won't (necessarily) validate but will work in QCD.
    d.setContexts(contexts);
    QVERIFY(d.contexts().count() == 3);
    QVERIFY(!d.contexts().contains(QContactDetail::ContextWork));
    QVERIFY(d.contexts().contains(QContactDetail::ContextOther));
    QVERIFY(d.contexts().contains(QContactDetail::ContextHome));
    QVERIFY(d.contexts().contains("CustomContext"));
    QCOMPARE(d.contexts(), contexts);

    // test that contexts are values.
    QCOMPARE(d.value<QStringList>(QContactDetail::FieldContext), d.contexts());
}

void tst_QContactDetail::values()
{
    QContactDetail p;

    QCOMPARE(p.variantValues(), QVariantMap());

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
    QVERIFY(!p.hasValue("non existent field"));

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
    QVariantMap emptyValues;
    QVariantMap values = p.variantValues();
    QStringList keys = values.keys();
    foreach (const QString& key, keys)
        QVERIFY(p.setValue(key, QVariant()));

    QCOMPARE(p.variantValues(), emptyValues);
    QVERIFY(p.variantValues().count() == 0);
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

    /* Set values */
    keys = values.keys();
    foreach (const QString& key, keys)
        QVERIFY(p.setValue(key, values.value(key)));

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
    values = p.variantValues();
    keys = values.keys();
    foreach (const QString& key, keys)
        QVERIFY(p.setValue(key, QVariant()));
    QCOMPARE(p.variantValues(), emptyValues);

    /* Check that we can add a null variant */
    //QVERIFY(p.setValue("nullvariant", QVariant()));
    //QVERIFY(p.hasValue("nullvariant"));
    //QCOMPARE(p.value("nullvariant"), QString());
    //QCOMPARE(p.variantValue("nullvariant"), QVariant());
    //QVERIFY(p.removeValue("nullvariant"));
    //QVERIFY(p.values().count() == 0);

    /* Check that adding a value, then setting it to null updates it */
    //QVERIFY(p.setValue("string", QString("string value")));
    //QCOMPARE(p.values().count(), 1);
    //QCOMPARE(p.value("string"), QString("string value"));
    //QVERIFY(p.setValue("string", QVariant()));
    //QCOMPARE(p.values().count(), 1);
    //QVERIFY(p.hasValue("string"));
    //QVERIFY(p.removeValue("string"));
    //QCOMPARE(p.values().count(), 0);

    /* See if adding a null QString triggers the same behaviour */
    //QVERIFY(p.setValue("string", QString("string value")));
    //QCOMPARE(p.values().count(), 1);
    //QCOMPARE(p.value("string"), QString("string value"));
    //QVERIFY(p.setValue("string", QString()));
    //QCOMPARE(p.values().count(), 1);
    //QVERIFY(p.hasValue("string"));
    //QVERIFY(p.removeValue("string"));
    //QCOMPARE(p.values().count(), 0);

    /* Check adding a null value removes the field */
    p.setValue("string", "stringvalue");
    QVERIFY(p.variantValues().contains("string"));
    QVERIFY(p.value("string") == QString("stringvalue"));
    p.setValue("string", QVariant());
    QVERIFY(!p.variantValues().contains("string"));

    /* Check adding a field whose value is an empty string */
    p.setValue("string", "");
    QVERIFY(p.variantValues().contains("string"));
    QVERIFY(p.value("string") == QString(""));

    /* Check accessing a missing value */
    QCOMPARE(p.value("nonexistent"), QString());
    QVERIFY(p.setValue("nonexistent", "changed my mind"));
    QCOMPARE(p.value("nonexistent"), QString("changed my mind"));

    /* Check removing a missing value */
    QVERIFY(!p.removeValue("does not exist"));
}

void tst_QContactDetail::hash()
{
    QContactDetail detail1("definition");
    detail1.setValue("key", "value");
    QContactDetail detail2("definition");
    detail2.setValue("key", "value");
    QContactDetail detail3("definition");
    detail3.setValue("key", "different value");
    QVERIFY(qHash(detail1) == qHash(detail2));
    QVERIFY(qHash(detail1) != qHash(detail3));
    QSet<QContactDetail> set;
    set.insert(detail1);
    set.insert(detail2);
    set.insert(detail3);
    QCOMPARE(set.size(), 2);
}

void tst_QContactDetail::traits()
{
    // QContactDetail has a vtable and a dpointer, so we can't really make claims about the size
    // QCOMPARE(sizeof(QContactDetail), sizeof(void *));
    QTypeInfo<QTM_PREPEND_NAMESPACE(QContactDetail)> ti;
    QVERIFY(ti.isComplex);
    QVERIFY(!ti.isStatic);
    QVERIFY(ti.isLarge); // virtual table + d pointer
    QVERIFY(!ti.isPointer);
    QVERIFY(!ti.isDummy);
}

QTEST_MAIN(tst_QContactDetail)
#include "tst_qcontactdetail.moc"
