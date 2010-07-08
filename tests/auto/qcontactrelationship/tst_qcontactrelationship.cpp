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

#include "qtcontacts.h"

//TESTED_CLASS=
//TESTED_FILES=

QTM_USE_NAMESPACE

class tst_QContactRelationship: public QObject
{
Q_OBJECT

public:
    tst_QContactRelationship();
    virtual ~tst_QContactRelationship();

private slots:
    void operations();
    void emptiness();
    void hash();
    void datastream();
};

tst_QContactRelationship::tst_QContactRelationship()
{
}

tst_QContactRelationship::~tst_QContactRelationship()
{
}

void tst_QContactRelationship::operations()
{
    QContactRelationship r1;

    QContactId id1, id2;
    id1.setLocalId(1);
    id1.setManagerUri("test");
    id2.setLocalId(2);
    id2.setManagerUri("test");

    QVERIFY(r1.first() == QContactId());
    QVERIFY(r1.second() == QContactId());
    QVERIFY(r1.relationshipType() == QString());

    r1.setFirst(id1);
    QVERIFY(r1.first() == id1);
    r1.setSecond(id2);
    QVERIFY(r1.second() == id2);
    QVERIFY(r1.first() == id1);
    QVERIFY(r1.relationshipType() == QString());

    r1.setRelationshipType(QContactRelationship::HasSpouse);
    QVERIFY(r1.relationshipType() == QString(QLatin1String(QContactRelationship::HasSpouse)));
}

void tst_QContactRelationship::emptiness()
{
    QContactRelationship r1, r2, r3;

    QContactId id1, id2, id3;
    id1.setLocalId(1);
    id1.setManagerUri("test");
    id2.setLocalId(2);
    id2.setManagerUri("test");
    id3.setLocalId(3);
    id3.setManagerUri("test");

    QVERIFY(r1.first() == QContactId());
    QVERIFY(r1.second() == QContactId());
    QVERIFY(r1.relationshipType() == QString());

    r1.setFirst(id1);
    QVERIFY(r1.first() == id1);
    r1.setSecond(id2);
    QVERIFY(r1.second() == id2);
    QVERIFY(r1.first() == id1);
    QVERIFY(r1.relationshipType() == QString());

    r1.setRelationshipType(QContactRelationship::HasSpouse);
    QVERIFY(r1.relationshipType() == QString(QLatin1String(QContactRelationship::HasSpouse)));

    r2 = r1;
    QVERIFY(r1 == r2);
    QVERIFY(r1 != r3);
    QVERIFY(r2 != r3);

    r3.setFirst(id3);
    r3.setSecond(id2);
    r3.setRelationshipType(QContactRelationship::HasAssistant);

    r2.setFirst(id3);

    QVERIFY(r1 != r2);
    QVERIFY(r2 != r3);
    QVERIFY(r3 != r1);
}

void tst_QContactRelationship::hash()
{
    QContactRelationship r1;
    QContactId id1;
    id1.setManagerUri("a");
    id1.setLocalId(1);
    r1.setFirst(id1);
    QContactId id2;
    id2.setManagerUri("b");
    id2.setLocalId(2);
    r1.setSecond(id2);
    r1.setRelationshipType(QContactRelationship::HasMember);

    QContactRelationship r2;
    r2.setFirst(id1);
    r2.setSecond(id2);
    r2.setRelationshipType(QContactRelationship::HasMember);

    QContactRelationship r3;
    r3.setFirst(id1);
    QContactId id3;
    id3.setManagerUri("c");
    id3.setLocalId(3);
    r3.setSecond(id3);
    r3.setRelationshipType(QContactRelationship::HasMember);

    QVERIFY(qHash(r1) == qHash(r2));
    QVERIFY(qHash(r1) != qHash(r3));

}

void tst_QContactRelationship::datastream()
{
    QByteArray buffer;
    QDataStream stream1(&buffer, QIODevice::WriteOnly);
    QContactRelationship relationshipIn;
    QContactId id1;
    id1.setManagerUri("a");
    id1.setLocalId(1);
    relationshipIn.setFirst(id1);
    QContactId id2;
    id2.setManagerUri("b");
    id2.setLocalId(2);
    relationshipIn.setSecond(id2);
    relationshipIn.setRelationshipType(QContactRelationship::HasMember);
    stream1 << relationshipIn;

    QVERIFY(buffer.size() > 0);

    QDataStream stream2(buffer);
    QContactRelationship relationshipOut;
    stream2 >> relationshipOut;
    QCOMPARE(relationshipOut, relationshipIn);
}

QTEST_MAIN(tst_QContactRelationship)
#include "tst_qcontactrelationship.moc"
