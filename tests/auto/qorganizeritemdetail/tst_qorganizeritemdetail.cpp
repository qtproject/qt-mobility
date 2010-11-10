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
#include <QSet>

//TESTED_COMPONENT=src/organizer
//TESTED_CLASS=
//TESTED_FILES=

QTM_USE_NAMESPACE
class tst_QOrganizerItemDetail : public QObject
{
Q_OBJECT

public:
    tst_QOrganizerItemDetail();
    virtual ~tst_QOrganizerItemDetail();

public slots:
    void init();
    void cleanup();

private slots:
    void classHierarchy();
    void assignment();
    void templates();
    void values();
    void hash();
    void datastream();
    void traits();
    void keys();
};

tst_QOrganizerItemDetail::tst_QOrganizerItemDetail()
{
}

tst_QOrganizerItemDetail::~tst_QOrganizerItemDetail()
{
}

void tst_QOrganizerItemDetail::init()
{
}

void tst_QOrganizerItemDetail::cleanup()
{
}

/* Test class that doesn't do the right thing */
class NonMacroCustomDetail : public QOrganizerItemDetail
{
public:
    NonMacroCustomDetail() : QOrganizerItemDetail("malicious") {}
    void doAssign(const QOrganizerItemDetail& other) {assign(other, "malicious");}
    NonMacroCustomDetail(const QOrganizerItemDetail& other)
        : QOrganizerItemDetail(other, "malicious") {}
};

class NonMacroCustomDetail2 : public QOrganizerItemDetail
{
public:
    NonMacroCustomDetail2() : QOrganizerItemDetail(QString("malicious")) {}
    void doAssign(const QOrganizerItemDetail& other) {assign(other, QString("malicious"));}
    NonMacroCustomDetail2(const QOrganizerItemDetail& other)
        : QOrganizerItemDetail(other, QString("malicious")) {}
};

void tst_QOrganizerItemDetail::classHierarchy()
{
    QOrganizerItemDetail f1;
    QOrganizerItemDetail f2;

    QVERIFY(f1.isEmpty());
    QVERIFY(f2.isEmpty());

    QOrganizerItemPriority p1;
    p1.setPriority(QOrganizerItemPriority::VeryHighPriority);
    QVERIFY(!p1.isEmpty());
    QVERIFY(p1.definitionName() == QOrganizerItemPriority::DefinitionName);

    QOrganizerItemComment m1;
    m1.setComment("Bob");
    QVERIFY(!m1.isEmpty());
    QVERIFY(m1.definitionName() == QOrganizerItemComment::DefinitionName);

    QVERIFY(p1 != m1);
    QVERIFY(f1 == f2);

    f1 = p1; // f1 is a priority
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

    f2 = f1; // f2 = f1 = priority
    QVERIFY(f1 == f2);
    QVERIFY(f2 == f1);
    QVERIFY(f2 == p1);
    QVERIFY(f1 == p1);

    f1 = m1; // f1 = name, f2 = priority
    QVERIFY(f1 == m1);
    QVERIFY(f1 != f2);
    QVERIFY(f2 == p1);

    QOrganizerItemPriority p2(f2); // p2 = f2 = priority
    QVERIFY(p1 == p2);
    QVERIFY(p1 == f2);
    QCOMPARE(p2.priority(), p1.priority());
    QCOMPARE(p2.priority(), QOrganizerItemPriority::VeryHighPriority);

    p2 = p1; // priority to priority
    QVERIFY(p1 == p2);
    QVERIFY(p1 == f2);
    QCOMPARE(p2.priority(), p1.priority());
    QCOMPARE(p2.priority(), QOrganizerItemPriority::VeryHighPriority);

    p2.setPriority(QOrganizerItemPriority::VeryLowPriority); // NOTE: implicitly shared, this has caused a detach so p1 != 2
    QVERIFY(p1 != p2);
    QVERIFY(p1 == f2);
    QVERIFY(p2 != f2);
    QCOMPARE(p2.priority(), QOrganizerItemPriority::VeryLowPriority);
    QCOMPARE(p1.priority(), QOrganizerItemPriority::VeryHighPriority);

    /* Bad assignment */
    p2 = m1; // assign a comment to a priority
    QVERIFY(p2 != m1);
    QVERIFY(p2.definitionName() == QOrganizerItemPriority::DefinitionName); // should still be a phone number though
    QVERIFY(p2.isEmpty());

    /* copy ctor */
    QOrganizerItemComment m2(m1);
    QVERIFY(m2 == m1);

    /* another bad assignment */
    m2 = p2; // priority to a comment
    QVERIFY(m2 != m1);
    QVERIFY(m2.definitionName() == QOrganizerItemComment::DefinitionName);
    QVERIFY(m2.isEmpty());

    /* Copy ctor from valid type */
    QOrganizerItemDetail f3(p2);
    QVERIFY(f3 == p2);
    QVERIFY(f3.definitionName() == QOrganizerItemPriority::DefinitionName);

    /* Copy ctor from invalid type */
    QOrganizerItemPriority p3(m1);
    QVERIFY(p3 != m1);
    QVERIFY(p3.definitionName() == QOrganizerItemPriority::DefinitionName);
    QVERIFY(p3.isEmpty());

    /* Copy ctore from invalid type, through base type */
    f3 = m1;
    QOrganizerItemPriority p4(f3);
    QVERIFY(p4 != f3);
    QVERIFY(p4.definitionName() == QOrganizerItemPriority::DefinitionName);
    QVERIFY(p4.isEmpty());

    /* Try a reference */
    p1.setPriority(QOrganizerItemPriority::VeryLowPriority);
    QOrganizerItemDetail& ref = p1;
    QVERIFY(p1.priority() == QOrganizerItemPriority::VeryLowPriority);
    QVERIFY(p1.value(QOrganizerItemPriority::FieldPriority).toInt() == static_cast<int>(QOrganizerItemPriority::VeryLowPriority));
    QVERIFY(ref.value(QOrganizerItemPriority::FieldPriority).toInt() == static_cast<int>(QOrganizerItemPriority::VeryLowPriority));
    QVERIFY(p1 == ref);
    QVERIFY(ref == p1);

    /* Try changing the original */
    p1.setPriority(QOrganizerItemPriority::MediumPriority);
    QVERIFY(p1.priority() == QOrganizerItemPriority::MediumPriority);
    QVERIFY(p1.value(QOrganizerItemPriority::FieldPriority).toInt() == static_cast<int>(QOrganizerItemPriority::MediumPriority));
    QVERIFY(ref.value(QOrganizerItemPriority::FieldPriority).toInt() == static_cast<int>(QOrganizerItemPriority::MediumPriority));
    QVERIFY(p1 == ref);
    QVERIFY(ref == p1);

    /* Try changing the reference */
    ref.setValue(QOrganizerItemPriority::FieldPriority, QOrganizerItemPriority::LowPriority);
    QVERIFY(p1.priority() == QOrganizerItemPriority::LowPriority);
    QVERIFY(p1.value(QOrganizerItemPriority::FieldPriority).toInt() == static_cast<int>(QOrganizerItemPriority::LowPriority));
    QVERIFY(ref.value(QOrganizerItemPriority::FieldPriority).toInt() == static_cast<int>(QOrganizerItemPriority::LowPriority));
    QVERIFY(p1 == ref);
    QVERIFY(ref == p1);

    /* Random other test */
    NonMacroCustomDetail md;
    QVERIFY(md.definitionName() == "malicious");
    QVERIFY(md.setValue("key", "value"));
    QVERIFY(!md.isEmpty());
    md.doAssign(md); // self assignment
    QVERIFY(!md.isEmpty());
    QVERIFY(md.value("key") == "value");

    QOrganizerItemDetail mdv;
    mdv = md;
    QVERIFY(mdv.definitionName() == "malicious");
    QVERIFY(mdv.value("key") == "value");

    md = mdv;
    QVERIFY(md.definitionName() == "malicious");
    QVERIFY(md.value("key") == "value");

    NonMacroCustomDetail2 md2;
    QVERIFY(md2.setValue("key", "value"));
    QVERIFY(md2.definitionName() == "malicious");
    QVERIFY(md2.value("key") == "value");
    md2.doAssign(md);
    QVERIFY(md2 == md);
    md2 = md;
    QVERIFY(md.definitionName() == "malicious");
    QVERIFY(md.value("key") == "value");

    // Self assignment
    md2.doAssign(md2);
    QVERIFY(md2.definitionName() == "malicious");
    QVERIFY(md2.value("key") == "value");

    md.doAssign(md2);
    QVERIFY(md == md2);

    // Assigning something else
    QOrganizerItemPriority pn;
    pn.setPriority(QOrganizerItemPriority::LowestPriority);
    md2.doAssign(pn);
    QVERIFY(md2.isEmpty());
    QVERIFY(md2.definitionName() == "malicious");

    NonMacroCustomDetail mdb(pn);
    QVERIFY(mdb.isEmpty());
    QVERIFY(mdb.definitionName() == "malicious");

    NonMacroCustomDetail2 md2b(pn);
    QVERIFY(md2b.isEmpty());
    QVERIFY(md2b.definitionName() == "malicious");
}

void tst_QOrganizerItemDetail::assignment()
{
    QOrganizerItemPriority p1, p2;
    p1.setPriority(QOrganizerItemPriority::LowPriority);
    p2.setPriority(QOrganizerItemPriority::HighPriority);
    QVERIFY(p1 != p2);

    p1 = p2;
    QVERIFY(p1 == p2);

    QOrganizerItemComment c1;
    c1.setComment("test comment");
    QVERIFY(c1 != p1);
    c1 = p1;
    QVERIFY(c1 != p1); // assignment across types shouldn't work
    QVERIFY(c1.comment() == QString()); // should reset the detail
    QCOMPARE(c1, QOrganizerItemComment());

    QOrganizerManagerEngine::setDetailAccessConstraints(&p2, QOrganizerItemDetail::Irremovable);
    QVERIFY(p1 != p2);
}

void tst_QOrganizerItemDetail::templates()
{
    QOrganizerItem c;
    QOrganizerItemPriority p1, p2;
    p1.setPriority(QOrganizerItemPriority::HighPriority);
    p2.setPriority(QOrganizerItemPriority::HighestPriority);
    QVERIFY(c.saveDetail(&p1));
    QVERIFY(c.saveDetail(&p2));

    QList<QOrganizerItemDetail> l = c.details(QOrganizerItemPriority::DefinitionName);

    QCOMPARE(l.count(), 2);
    QCOMPARE(QOrganizerItemPriority(l.at(0)), p1);
    QCOMPARE(QOrganizerItemPriority(l.at(1)), p2);

    QList<QOrganizerItemPriority> l2 = c.details<QOrganizerItemPriority>();
    QCOMPARE(l2.count(), 2);
    QCOMPARE(l2.at(0), p1);
    QCOMPARE(l2.at(1), p2);
}

void tst_QOrganizerItemDetail::values()
{
    QOrganizerItemDetail p;

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

    // Test the setter that takes a QString
    p.setValue(QLatin1String("stringint"), "123");

    // and the setter that takes a QL1C
    p.setValue(QOrganizerItemPriority::FieldPriority, QVariant::fromValue(static_cast<int>(QOrganizerItemPriority::ExtremelyHighPriority)));

    /* Presence test (const char * version) */
    QVERIFY(p.hasValue("string"));
    QVERIFY(p.hasValue("date"));
    QVERIFY(p.hasValue("datetime"));
    QVERIFY(p.hasValue("int"));
    QVERIFY(p.hasValue("stringdate"));
    QVERIFY(p.hasValue("stringdatetime"));
    QVERIFY(p.hasValue("stringint"));
    QVERIFY(p.hasValue(QOrganizerItemPriority::FieldPriority.latin1()));
    QVERIFY(!p.hasValue("non existent field"));

    /* QLatin1Constant version */
    QVERIFY(p.hasValue(QOrganizerItemPriority::FieldPriority));
    QVERIFY(!p.hasValue(QOrganizerItemComment::FieldComment));

    /* Again with QString version */
    QVERIFY(p.hasValue(QLatin1String("string")));
    QVERIFY(p.hasValue(QLatin1String("date")));
    QVERIFY(p.hasValue(QLatin1String("datetime")));
    QVERIFY(p.hasValue(QLatin1String("int")));
    QVERIFY(p.hasValue(QLatin1String("stringdate")));
    QVERIFY(p.hasValue(QLatin1String("stringdatetime")));
    QVERIFY(p.hasValue(QLatin1String("stringint")));
    QVERIFY(p.hasValue(QString(QLatin1String(QOrganizerItemPriority::FieldPriority))));
    QVERIFY(!p.hasValue(QLatin1String("non existent field")));

    /* string accessors with const char* key */
    QCOMPARE(p.value("string"), QString("This is a string"));
    QCOMPARE(p.value("date"), d.toString(Qt::ISODate));
    QCOMPARE(p.value("datetime"), dt.toString(Qt::ISODate));
    QCOMPARE(p.value("int"), QString("6"));
    QCOMPARE(p.value("stringdate"), d.toString(Qt::ISODate));
    QCOMPARE(p.value("stringdatetime"), dt.toString(Qt::ISODate));
    QCOMPARE(p.value("stringint"), QString("123"));
    QCOMPARE(p.value(QOrganizerItemPriority::FieldPriority.latin1()), QString::number(static_cast<int>(QOrganizerItemPriority::ExtremelyHighPriority)));

    /* string accessor with QL1C key */
    QCOMPARE(p.variantValue(QOrganizerItemPriority::FieldPriority).toInt(), static_cast<int>(QOrganizerItemPriority::ExtremelyHighPriority));

    /* string accessors with QString key */
    QCOMPARE(p.value(QLatin1String("string")), QString("This is a string"));
    QCOMPARE(p.value(QLatin1String("date")), d.toString(Qt::ISODate));
    QCOMPARE(p.value(QLatin1String("datetime")), dt.toString(Qt::ISODate));
    QCOMPARE(p.value(QLatin1String("int")), QString("6"));
    QCOMPARE(p.value(QLatin1String("stringdate")), d.toString(Qt::ISODate));
    QCOMPARE(p.value(QLatin1String("stringdatetime")), dt.toString(Qt::ISODate));
    QCOMPARE(p.value(QLatin1String("stringint")), QString("123"));
    QCOMPARE(p.value(QString(QLatin1String(QOrganizerItemPriority::FieldPriority))), QString::number(static_cast<int>(QOrganizerItemPriority::ExtremelyHighPriority)));

    /* Variant accessor with const char * key */
    QCOMPARE(p.variantValue("string"), QVariant(QString("This is a string")));
    QCOMPARE(p.variantValue("date"), QVariant(d));
    QCOMPARE(p.variantValue("datetime"), QVariant(dt));
    QCOMPARE(p.variantValue("int"), QVariant((int)6));
    QCOMPARE(p.variantValue("stringdate"), QVariant(d.toString(Qt::ISODate)));
    QCOMPARE(p.variantValue("stringdatetime"), QVariant(dt.toString(Qt::ISODate)));
    QCOMPARE(p.variantValue("stringint"), QVariant(QString("123")));
    QCOMPARE(p.variantValue(QOrganizerItemPriority::FieldPriority.latin1()).toInt(), static_cast<int>(QOrganizerItemPriority::ExtremelyHighPriority));

    /* Variant accessor with QL1C key */
    QCOMPARE(p.variantValue(QOrganizerItemPriority::FieldPriority).toInt(), static_cast<int>(QOrganizerItemPriority::ExtremelyHighPriority));

    /* Variant accessor with QString key */
    QCOMPARE(p.variantValue(QLatin1String("string")), QVariant(QString("This is a string")));
    QCOMPARE(p.variantValue(QLatin1String("date")), QVariant(d));
    QCOMPARE(p.variantValue(QLatin1String("datetime")), QVariant(dt));
    QCOMPARE(p.variantValue(QLatin1String("int")), QVariant((int)6));
    QCOMPARE(p.variantValue(QLatin1String("stringdate")), QVariant(d.toString(Qt::ISODate)));
    QCOMPARE(p.variantValue(QLatin1String("stringdatetime")), QVariant(dt.toString(Qt::ISODate)));
    QCOMPARE(p.variantValue(QLatin1String("stringint")), QVariant(QString("123")));
    QCOMPARE(p.variantValue(QLatin1String(QOrganizerItemPriority::FieldPriority)).toInt(), static_cast<int>(QOrganizerItemPriority::ExtremelyHighPriority));

    /* Typed accessors, string first, const char* key */
    QCOMPARE(p.value<QString>("string"), QString("This is a string"));
    QCOMPARE(p.value<QString>("date"), d.toString(Qt::ISODate));
    QCOMPARE(p.value<QString>("datetime"), dt.toString(Qt::ISODate));
    QCOMPARE(p.value<QString>("int"), QString("6"));
    QCOMPARE(p.value<QString>("stringdate"), d.toString(Qt::ISODate));
    QCOMPARE(p.value<QString>("stringdatetime"), dt.toString(Qt::ISODate));
    QCOMPARE(p.value<QString>("stringint"), QString("123"));
    QCOMPARE(p.value<int>(QOrganizerItemPriority::FieldPriority.latin1()), static_cast<int>(QOrganizerItemPriority::ExtremelyHighPriority));

    /* Now individual original types */
    QCOMPARE(p.value<QDate>("date"), d);
    QCOMPARE(p.value<QDateTime>("datetime"), dt);
    QCOMPARE(p.value<int>("int"), 6);

    /* now latin constant keys */
    QCOMPARE(p.value<int>(QOrganizerItemPriority::FieldPriority), static_cast<int>(QOrganizerItemPriority::ExtremelyHighPriority));

    /* Typed accessors, string first, QString key */
    QCOMPARE(p.value<QString>(QLatin1String("string")), QString("This is a string"));
    QCOMPARE(p.value<QString>(QLatin1String("date")), d.toString(Qt::ISODate));
    QCOMPARE(p.value<QString>(QLatin1String("datetime")), dt.toString(Qt::ISODate));
    QCOMPARE(p.value<QString>(QLatin1String("int")), QString("6"));
    QCOMPARE(p.value<QString>(QLatin1String("stringdate")), d.toString(Qt::ISODate));
    QCOMPARE(p.value<QString>(QLatin1String("stringdatetime")), dt.toString(Qt::ISODate));
    QCOMPARE(p.value<QString>(QLatin1String("stringint")), QString("123"));
    QCOMPARE(p.value<int>(QLatin1String(QOrganizerItemPriority::FieldPriority)), static_cast<int>(QOrganizerItemPriority::ExtremelyHighPriority));
    QCOMPARE(p.value<QDate>(QLatin1String("date")), d);
    QCOMPARE(p.value<QDateTime>(QLatin1String("datetime")), dt);
    QCOMPARE(p.value<int>(QLatin1String("int")), 6);

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
    QVERIFY(!p.removeValue(QLatin1String("does not exist")));
    QVERIFY(!p.removeValue(QOrganizerItemComment::FieldComment));

    p.setValue("stringint", "555");
    p.setValue(QOrganizerItemPriority::FieldPriority, "1234");

    /* Check removing a real value */
    QVERIFY(p.removeValue("string"));
    QVERIFY(p.removeValue(QLatin1String("stringint")));
    QVERIFY(p.removeValue(QOrganizerItemPriority::FieldPriority));
}

void tst_QOrganizerItemDetail::hash()
{
    QOrganizerItemDetail detail1("definition");
    detail1.setValue("key", "value");
    QOrganizerItemDetail detail2("definition");
    detail2.setValue("key", "value");
    QOrganizerItemDetail detail3("definition");
    detail3.setValue("key", "different value");
    QVERIFY(qHash(detail1) == qHash(detail2));
    QVERIFY(qHash(detail1) != qHash(detail3));
    QSet<QOrganizerItemDetail> set;
    set.insert(detail1);
    set.insert(detail2);
    set.insert(detail3);
    QCOMPARE(set.size(), 2);
}

void tst_QOrganizerItemDetail::datastream()
{
    QByteArray buffer;
    QDataStream stream1(&buffer, QIODevice::WriteOnly);
    QOrganizerItemDetail detailIn("definition");
    detailIn.setValue("key1", "value1");
    detailIn.setValue("key2", "value2");
    stream1 << detailIn;

    QVERIFY(buffer.size() > 0);

    QDataStream stream2(buffer);
    QOrganizerItemDetail detailOut;
    stream2 >> detailOut;
    QCOMPARE(detailOut, detailIn);
}

void tst_QOrganizerItemDetail::traits()
{
    QCOMPARE(sizeof(QOrganizerItemDetail), sizeof(void *));
    QTypeInfo<QTM_PREPEND_NAMESPACE(QOrganizerItemDetail)> ti;
    QVERIFY(ti.isComplex);
    QVERIFY(!ti.isStatic);
    QVERIFY(!ti.isLarge);
    QVERIFY(!ti.isPointer);
    QVERIFY(!ti.isDummy);
}

void tst_QOrganizerItemDetail::keys()
{
    QOrganizerItemDetail d;
    QOrganizerItemDetail d2;
    QVERIFY(d.key() != d2.key());

    d = d2;
    QVERIFY(d.key() == d2.key());
    d.resetKey();
    QVERIFY(d.key() != d2.key());
}

QTEST_MAIN(tst_QOrganizerItemDetail)
#include "tst_qorganizeritemdetail.moc"
