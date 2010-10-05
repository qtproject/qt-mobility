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
#include "qorganizercollectionenginelocalid.h"
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

class BasicCollectionLocalId : public QOrganizerCollectionEngineLocalId
{
public:
    BasicCollectionLocalId(uint id) : m_id(id) {}
    bool isEqualTo(const QOrganizerCollectionEngineLocalId* other) const {
        return m_id == static_cast<const BasicCollectionLocalId*>(other)->m_id;
    }
    bool isLessThan(const QOrganizerCollectionEngineLocalId* other) const {
        return m_id < static_cast<const BasicCollectionLocalId*>(other)->m_id;
    }
    uint engineLocalIdType() const {
        return 0;
    }
    QOrganizerCollectionEngineLocalId* clone() const {
        BasicCollectionLocalId* cloned = new BasicCollectionLocalId(m_id);
        return cloned;
    }
    QDebug debugStreamOut(QDebug dbg) {
        return dbg << m_id;
    }
    QDataStream& dataStreamOut(QDataStream& out) {
        return out << static_cast<quint32>(m_id);
    }
    QDataStream& dataStreamIn(QDataStream& in) {
        quint32 id;
        in >> id;
        m_id = id;
        return in;
    }
    uint hash() const {
        return m_id;
    }

private:
    uint m_id;
};

QOrganizerCollectionLocalId makeId(uint id)
{
    return QOrganizerCollectionLocalId(new BasicCollectionLocalId(id));
}


void tst_QOrganizerCollection::idLessThan()
{
    QOrganizerCollectionId id1;
    id1.setManagerUri("a");
    id1.setLocalId(makeId(1));
    QOrganizerCollectionId id2;
    id2.setManagerUri("a");
    id2.setLocalId(makeId(1));
    QVERIFY(!(id1 < id2));
    QVERIFY(!(id2 < id1));
    QOrganizerCollectionId id3;
    id3.setManagerUri("a");
    id3.setLocalId(makeId(2));
    QOrganizerCollectionId id4;
    id4.setManagerUri("b");
    id4.setLocalId(makeId(1));
    QOrganizerCollectionId id5; // no URI
    id5.setLocalId(makeId(2));
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
    id1.setLocalId(makeId(1));
    QOrganizerCollectionId id2;
    id2.setManagerUri("a");
    id2.setLocalId(makeId(1));
    QOrganizerCollectionId id3;
    id3.setManagerUri("b");
    id3.setLocalId(makeId(1));
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
    id.setLocalId(makeId(1));
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
    // collection datastreaming
    QByteArray buffer;
    QOrganizerCollection collectionIn;
    collectionIn.setMetaData("key", "value");
    QOrganizerCollection collectionOut;
    QOrganizerCollectionId originalId;

    // first, stream an item with a complete id
    {
        QDataStream stream1(&buffer, QIODevice::WriteOnly);
        QOrganizerItemManager om("memory");
        QVERIFY(om.saveCollection(&collectionIn)); // fill in its ID
        originalId = collectionIn.id();
        stream1 << collectionIn;
        QVERIFY(buffer.size() > 0);
        QDataStream stream2(buffer);
        stream2 >> collectionOut;
        QCOMPARE(collectionOut, collectionIn); // can use QCOMPARE for collections, since no detail keys.
    }

    // second, stream an item with an id with the mgr uri set, local id null
    {
        QDataStream stream1(&buffer, QIODevice::WriteOnly);
        QOrganizerCollectionId modifiedId = originalId;
        modifiedId.setLocalId(QOrganizerCollectionLocalId());
        collectionIn.setId(modifiedId);
        stream1 << collectionIn;
        QVERIFY(buffer.size() > 0);
        QDataStream stream2(buffer);
        stream2 >> collectionOut;
        QCOMPARE(collectionOut, collectionIn); // can use QCOMPARE for collections, since no detail keys.
    }

    // third, stream an item with an id with the mgr uri null, local id set
    {
        QDataStream stream1(&buffer, QIODevice::WriteOnly);
        QOrganizerCollectionId modifiedId = originalId;
        modifiedId.setManagerUri(QString()); // this will clear the local id!
        modifiedId.setLocalId(originalId.localId()); // so reset it and make sure nothing bad happens.
        collectionIn.setId(modifiedId);
        stream1 << collectionIn;
        QVERIFY(buffer.size() > 0);
        QDataStream stream2(buffer);
        stream2 >> collectionOut;
        QVERIFY(collectionOut.metaData() == collectionIn.metaData());
        QVERIFY(collectionOut.id() != collectionIn.id()); // no manager uri of input :. won't be serialized.
    }

    // fourth, stream an item with a null id
    {
        QDataStream stream1(&buffer, QIODevice::WriteOnly);
        collectionIn.setId(QOrganizerCollectionId());
        stream1 << collectionIn;
        QVERIFY(buffer.size() > 0);
        QDataStream stream2(buffer);
        stream2 >> collectionOut;
        QVERIFY(collectionOut.metaData() == collectionIn.metaData());
        QVERIFY(collectionOut.id() == collectionIn.id()); // should both be null ids.
    }

    // id datastreaming
    buffer.clear();
    QOrganizerCollectionId inputId;
    QOrganizerCollectionId outputId;

    // first, stream the whole id (mgr uri set, local id set)
    {
        inputId = originalId;
        buffer.clear();
        QDataStream stream1(&buffer, QIODevice::WriteOnly);
        stream1 << inputId;
        QVERIFY(buffer.size() > 0);
        QDataStream stream2(buffer);
        stream2 >> outputId;
        QCOMPARE(inputId, outputId);
    }

    // second, stream a partial id (mgr uri null, local id set)
    {
        inputId.setManagerUri(QString());
        inputId.setLocalId(originalId.localId());
        buffer.clear();
        QDataStream stream1(&buffer, QIODevice::WriteOnly);
        stream1 << inputId;
        QVERIFY(buffer.size() > 0);
        QDataStream stream2(buffer);
        stream2 >> outputId;

        // because the manager uri is null, we cannot stream it in.
        QVERIFY(outputId.isNull());
        QVERIFY(!inputId.isNull());
    }

    // third, stream a partial id (mgr uri set, local id null).
    {
        inputId.setManagerUri(originalId.managerUri());
        inputId.setLocalId(QOrganizerCollectionLocalId());
        buffer.clear();
        QDataStream stream1(&buffer, QIODevice::WriteOnly);
        stream1 << inputId;
        QVERIFY(buffer.size() > 0);
        QDataStream stream2(buffer);
        stream2 >> outputId;
        QCOMPARE(inputId, outputId);
    }

    // fourth, stream a null id
    {
        inputId = QOrganizerCollectionId();
        buffer.clear();
        QDataStream stream1(&buffer, QIODevice::WriteOnly);
        stream1 << inputId;
        QVERIFY(buffer.size() > 0);
        QDataStream stream2(buffer);
        stream2 >> outputId;
        QCOMPARE(inputId, outputId);
    }

    // fifth, stream an id after changing it's manager uri.
    {
        inputId.setManagerUri(originalId.managerUri());
        inputId.setLocalId(originalId.localId());
        inputId.setManagerUri("test manager uri"); // should clear the local id.
        QVERIFY(inputId.localId() == QOrganizerCollectionLocalId());
        buffer.clear();
        QDataStream stream1(&buffer, QIODevice::WriteOnly);
        stream1 << inputId;
        QVERIFY(buffer.size() > 0);
        QDataStream stream2(buffer);
        stream2 >> outputId;
        QCOMPARE(inputId, outputId);
    }

    // sixth, stream an id after changing it's manager uri and resetting the local id.
    // this should cause great problems, because the manager doesn't exist so it shouldn't
    // be able to deserialize.  Make sure it's handled gracefully.
    {
        inputId.setManagerUri(originalId.managerUri());
        inputId.setManagerUri("test manager uri"); // should clear the local id.
        inputId.setLocalId(originalId.localId());
        buffer.clear();
        QDataStream stream1(&buffer, QIODevice::WriteOnly);
        stream1 << inputId;
        QVERIFY(buffer.size() > 0);
        QDataStream stream2(buffer);
        stream2 >> outputId;
        QVERIFY(outputId.isNull());
    }
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
    QVERIFY(sizeof(QOrganizerCollectionLocalId) == sizeof(void *));
    QTypeInfo<QTM_PREPEND_NAMESPACE(QOrganizerCollectionLocalId)> ti;
    QVERIFY(ti.isComplex); // unlike QContactLocalId (int typedef), we have a ctor
    QVERIFY(!ti.isStatic);
    QVERIFY(!ti.isLarge);
    QVERIFY(!ti.isPointer);
    QVERIFY(!ti.isDummy);
}

QTEST_MAIN(tst_QOrganizerCollection)
#include "tst_qorganizercollection.moc"
