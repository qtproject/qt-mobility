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

//TESTED_COMPONENT=src/organizer
//TESTED_CLASS=
//TESTED_FILES=

QTM_USE_NAMESPACE
class tst_QOrganizerItemDetailDefinition: public QObject
{
Q_OBJECT

public:
    tst_QOrganizerItemDetailDefinition();
    virtual ~tst_QOrganizerItemDetailDefinition();

public slots:
    void init();
    void cleanup();
private slots:
    void testCtor();
    void testGetSet();
    void testEquality();
    void testEmpty();
    void traits();
    void fieldTraits();
};

tst_QOrganizerItemDetailDefinition::tst_QOrganizerItemDetailDefinition()
{
}

tst_QOrganizerItemDetailDefinition::~tst_QOrganizerItemDetailDefinition()
{
}

void tst_QOrganizerItemDetailDefinition::init()
{
}

void tst_QOrganizerItemDetailDefinition::cleanup()
{
}


void tst_QOrganizerItemDetailDefinition::testCtor()
{
    QOrganizerItemDetailDefinition def;

    /* Check the ctor sets sane things */
    QVERIFY(def.isEmpty());
    QVERIFY(def.name().isEmpty());
    QVERIFY(def.fields().isEmpty());
    QVERIFY(def.isUnique() == false);

    /* Set a few things */
    QMap<QString, QOrganizerItemDetailFieldDefinition> map;
    QOrganizerItemDetailFieldDefinition currField;
    currField.setDataType(QVariant::String);
    map.insert("string", currField);
    currField.setDataType(QVariant::DateTime);
    map.insert("datetime", currField);

    def.setName("Test ID");
    def.setUnique(true);
    def.setFields(map);

    QVERIFY(def.name() == "Test ID");
    QVERIFY(def.isUnique());
    QVERIFY(def.fields() == map);

    QOrganizerItemDetailDefinition def2(def);

    QVERIFY(def2.name() == "Test ID");
    QVERIFY(def2.isUnique());
    QVERIFY(def2.fields() == map);

    QOrganizerItemDetailDefinition def3;
    def3 = def2;

    QVERIFY(def3.name() == "Test ID");
    QVERIFY(def3.isUnique());
    QVERIFY(def3.fields() == map);

    /* Make sure they aren't improperly shared */
    def.setName("id one");
    QVERIFY(def2.name() != def.name());
    QVERIFY(def3.name() != def.name());

    def2.setName("id two");
    QVERIFY(def2.name() != def3.name());
}

void tst_QOrganizerItemDetailDefinition::testGetSet()
{
    QOrganizerItemDetailDefinition def;

    /* Id */
    def.setName("this is the id");
    QVERIFY(def.name() == "this is the id");

    def.setName(QString());
    QVERIFY(def.name() == QString());

    /* Uniqueness */
    def.setUnique(true);
    QVERIFY(def.isUnique() == true);

    def.setUnique(false);
    QVERIFY(def.isUnique() == false);

    /* Type map */
    QMap<QString, QOrganizerItemDetailFieldDefinition> map;
    QOrganizerItemDetailFieldDefinition currField;
    currField.setDataType(QVariant::String);
    map.insert("string", currField);
    currField.setDataType(QVariant::DateTime);
    map.insert("datetime", currField);

    def.setFields(map);
    QVERIFY(def.fields() == map);

    def.setFields(QMap<QString, QOrganizerItemDetailFieldDefinition>());
    QVERIFY(def.fields().isEmpty());
}

void tst_QOrganizerItemDetailDefinition::testEmpty()
{
    QOrganizerItemDetailDefinition def;

    QVERIFY(def.isEmpty());

    def.setName("Name");
    QVERIFY(!def.isEmpty());
    def.setName(QString());
    QVERIFY(def.isEmpty());
    QMap<QString, QOrganizerItemDetailFieldDefinition> fields;
    QOrganizerItemDetailFieldDefinition f;
    f.setDataType(QVariant::String);
    fields.insert("Field", f);
    def.setFields(fields);
    QVERIFY(!def.isEmpty());

    def.setName("Name");
    QVERIFY(!def.isEmpty());

    fields.clear();
    def.setFields(fields);
    QVERIFY(!def.isEmpty());

    def.setName(QString());
    QVERIFY(def.isEmpty());
}

void tst_QOrganizerItemDetailDefinition::testEquality()
{
    /* Create a few */
    QOrganizerItemDetailDefinition def1, def2;

    /* Both empty, should be equal */
    QVERIFY(def1 == def2);
    QVERIFY(def2 == def1);

    /* Change id first */
    def1.setName("id");
    QVERIFY(def1 != def2);
    QVERIFY(def2 != def1);

    def2.setName("id");
    QVERIFY(def1 == def2);
    QVERIFY(def2 == def1);

    /* Then uniqueness */
    def1.setUnique(true);
    QVERIFY(def1 != def2);
    QVERIFY(def2 != def1);

    def2.setUnique(true);
    QVERIFY(def1 == def2);
    QVERIFY(def2 == def1);

    /* Test Fields */
    QOrganizerItemDetailFieldDefinition f1, f2;
    QVERIFY(f1 == f2);
    QVERIFY(f1.allowableValues().count() == 0);
    QVERIFY(f1.dataType() == QVariant::Invalid);

    f1.setDataType(QVariant::String);
    QVERIFY(f1 != f2);
    f1.setDataType(QVariant::Invalid);
    QVERIFY(f1 == f2);
    QVariantList vlist;
    vlist << "string" << 56;
    f1.setAllowableValues(vlist);
    QVERIFY(f1 != f2);
    f2.setAllowableValues(vlist);
    QVERIFY(f1 == f2);

    /* Field map */
    QMap<QString, QOrganizerItemDetailFieldDefinition> fields;
    QOrganizerItemDetailFieldDefinition currField;
    currField.setDataType(QVariant::String);
    fields.insert("string", currField);
    currField.setDataType(QVariant::DateTime);
    fields.insert("datetime", currField);

    def1.setFields(fields);
    QVERIFY(def1 != def2);
    QVERIFY(def2 != def1);
    QVERIFY(def1.fields() != def2.fields());

    def2.setFields(fields);
    QVERIFY(def1 == def2);
    QVERIFY(def2 == def1);
    QVERIFY(def1.fields() == def2.fields());

    /* Same map done a different way */
    fields.clear();
    fields.insert("datetime", currField);
    currField.setDataType(QVariant::String);
    fields.insert("string", currField);
    def2.setFields(fields);
    QVERIFY(def1 == def2);
    QVERIFY(def2 == def1);
}

void tst_QOrganizerItemDetailDefinition::traits()
{
    QCOMPARE(sizeof(QOrganizerItemDetailDefinition), sizeof(void *));
    QTypeInfo<QTM_PREPEND_NAMESPACE(QOrganizerItemDetailDefinition)> ti;
    QVERIFY(ti.isComplex);
    QVERIFY(!ti.isStatic);
    QVERIFY(!ti.isLarge);
    QVERIFY(!ti.isPointer);
    QVERIFY(!ti.isDummy);
}

void tst_QOrganizerItemDetailDefinition::fieldTraits()
{
    QCOMPARE(sizeof(QOrganizerItemDetailFieldDefinition), sizeof(void *));
    QTypeInfo<QTM_PREPEND_NAMESPACE(QOrganizerItemDetailFieldDefinition)> ti;
    QVERIFY(ti.isComplex);
    QVERIFY(!ti.isStatic);
    QVERIFY(!ti.isLarge);
    QVERIFY(!ti.isPointer);
    QVERIFY(!ti.isDummy);
}


QTEST_MAIN(tst_QOrganizerItemDetailDefinition)
#include "tst_qorganizeritemdetaildefinition.moc"
