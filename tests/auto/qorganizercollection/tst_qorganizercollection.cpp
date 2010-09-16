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
#include <QSet>

//TESTED_COMPONENT=src/organizer
//TESTED_CLASS=
//TESTED_FILES=

QTM_USE_NAMESPACE
class tst_QOrganizerCollection: public QObject
{
Q_OBJECT

public:
    tst_QOrganizerCollection();
    virtual ~tst_QOrganizerCollection();

private slots:
    void metaData();
    void idLessThan();
    void idHash();
    void hash();
    void datastream();
    void traits();
    void idTraits();
    void localIdTraits();
};

tst_QOrganizerCollection::tst_QOrganizerCollection()
{
}

tst_QOrganizerCollection::~tst_QOrganizerCollection()
{
}

void tst_QOrganizerCollection::metaData()
{
    QOrganizerCollection c;
    QVERIFY(c.metaData().isEmpty());
    c.setMetaData("test", 5);
    QVERIFY(c.metaData().contains("test"));

    QVariantMap mdm;
    mdm.insert("test2", 6);
    c.setMetaData(mdm);
    QCOMPARE(c.metaData(), mdm);
}

void tst_QOrganizerCollection::idLessThan()
{
    QOrganizerCollectionId id1;
    id1.setManagerUri("a");
    id1.setLocalId(1);
    QOrganizerCollectionId id2;
    id2.setManagerUri("a");
    id2.setLocalId(1);
    QVERIFY(!(id1 < id2));
    QVERIFY(!(id2 < id1));
    QOrganizerCollectionId id3;
    id3.setManagerUri("a");
    id3.setLocalId(2);
    QOrganizerCollectionId id4;
    id4.setManagerUri("b");
    id4.setLocalId(1);
    QOrganizerCollectionId id5; // no URI
    id5.setLocalId(2);
    QVERIFY(id1 < id3);
    QVERIFY(!(id3 < id1));
    QVERIFY(id1 < id4);
    QVERIFY(!(id4 < id1));
    QVERIFY(id3 < id4);
    QVERIFY(!(id4 < id3));
    QVERIFY(id5 < id1);
    QVERIFY(!(id1 < id5));
}

void tst_QOrganizerCollection::idHash()
{
    QOrganizerCollectionId id1;
    id1.setManagerUri("a");
    id1.setLocalId(1);
    QOrganizerCollectionId id2;
    id2.setManagerUri("a");
    id2.setLocalId(1);
    QOrganizerCollectionId id3;
    id3.setManagerUri("b");
    id3.setLocalId(1);
    QVERIFY(qHash(id1) == qHash(id2));
    QVERIFY(qHash(id1) != qHash(id3));
    QSet<QOrganizerCollectionId> set;
    set.insert(id1);
    set.insert(id2);
    set.insert(id3);
    QCOMPARE(set.size(), 2);
}

void tst_QOrganizerCollection::hash()
{
    QOrganizerCollectionId id;
    id.setManagerUri("a");
    id.setLocalId(1);
    QOrganizerCollection c1;
    c1.setId(id);
    c1.setMetaData("key", "value");
    QOrganizerCollection c2;
    c2.setId(id);
    c2.setMetaData("key", "value");
    QOrganizerCollection c3;
    c3.setId(id);
    c3.setMetaData("key", "another value");
    QOrganizerCollection c4; // no details
    c4.setId(id);
    QOrganizerCollection c5;
    c5.setId(id);
    c5.setMetaData("key", "value");
    QVERIFY(qHash(c1) == qHash(c2));
    QVERIFY(qHash(c1) != qHash(c3));
    QVERIFY(qHash(c1) != qHash(c4));
    QVERIFY(qHash(c1) == qHash(c5));
}

void tst_QOrganizerCollection::datastream()
{
    QByteArray buffer;
    QDataStream stream1(&buffer, QIODevice::WriteOnly);
    QOrganizerCollection contactIn;
    QOrganizerCollectionId id;
    id.setManagerUri("manager");
    id.setLocalId(1234);
    contactIn.setId(id);
    contactIn.setMetaData("key", "value");
    stream1 << contactIn;

    QVERIFY(buffer.size() > 0);

    QDataStream stream2(buffer);
    QOrganizerCollection contactOut;
    stream2 >> contactOut;
    QCOMPARE(contactOut, contactIn);
}

void tst_QOrganizerCollection::traits()
{
    QVERIFY(sizeof(QOrganizerCollection) == sizeof(void *));
    QTypeInfo<QTM_PREPEND_NAMESPACE(QOrganizerCollection)> ti;
    QVERIFY(ti.isComplex);
    QVERIFY(!ti.isStatic);
    QVERIFY(!ti.isLarge);
    QVERIFY(!ti.isPointer);
    QVERIFY(!ti.isDummy);
}

void tst_QOrganizerCollection::idTraits()
{
    QVERIFY(sizeof(QOrganizerCollectionId) == sizeof(void *));
    QTypeInfo<QTM_PREPEND_NAMESPACE(QOrganizerCollectionId)> ti;
    QVERIFY(ti.isComplex);
    QVERIFY(!ti.isStatic);
    QVERIFY(!ti.isLarge);
    QVERIFY(!ti.isPointer);
    QVERIFY(!ti.isDummy);
}

void tst_QOrganizerCollection::localIdTraits()
{
    QVERIFY(sizeof(QOrganizerCollectionId) == sizeof(void *));
    QTypeInfo<QTM_PREPEND_NAMESPACE(QOrganizerCollectionLocalId)> ti;
    QVERIFY(!ti.isComplex);
    QVERIFY(!ti.isStatic);
    QVERIFY(!ti.isLarge);
    QVERIFY(!ti.isPointer);
    QVERIFY(!ti.isDummy);
}

QTEST_MAIN(tst_QOrganizerCollection)
#include "tst_qorganizercollection.moc"
