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
#include "qorganizercollectionengineid.h"
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
    void idStringFunctions();
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
    QCOMPARE(c.metaData(QString("test")).toInt(), 5);

    QVariantMap mdm;
    mdm.insert("test2", 6);
    c.setMetaData(mdm);
    QCOMPARE(c.metaData(), mdm);
    QCOMPARE(c.metaData(QString("test2")).toInt(), 6);
}

class BasicCollectionLocalId : public QOrganizerCollectionEngineId
{
public:
    BasicCollectionLocalId(const QString& managerUri, uint id) : m_managerUri(managerUri), m_id(id) {}
    bool isEqualTo(const QOrganizerCollectionEngineId* other) const {
        if (m_managerUri == static_cast<const BasicCollectionLocalId*>(other)->m_managerUri)
            return m_id == static_cast<const BasicCollectionLocalId*>(other)->m_id;
        return false;
    }
    bool isLessThan(const QOrganizerCollectionEngineId* other) const {
        if (m_managerUri == static_cast<const BasicCollectionLocalId*>(other)->m_managerUri)
            return m_id < static_cast<const BasicCollectionLocalId*>(other)->m_id;
        return m_managerUri < static_cast<const BasicCollectionLocalId*>(other)->m_managerUri;
    }
    QString managerUri() const {
        static const QString uri(QLatin1String("qtorganizer:basic:"));
        return uri;
    }
    QOrganizerCollectionEngineId* clone() const {
        BasicCollectionLocalId* cloned = new BasicCollectionLocalId(m_managerUri, m_id);
        return cloned;
    }
    QDebug& debugStreamOut(QDebug& dbg) const {
        return dbg << m_managerUri << m_id;
    }
    QString toString() const {
        return m_managerUri + QString("::") + QString::number(m_id);
    }
    uint hash() const {
        return m_id;
    }

private:
    QString m_managerUri;
    uint m_id;
};

QOrganizerCollectionId makeId(const QString& managerUri, uint id)
{
    return QOrganizerCollectionId(new BasicCollectionLocalId(managerUri, id));
}


void tst_QOrganizerCollection::idLessThan()
{
    // TODO: review tests
    QOrganizerCollectionId id1(makeId("a", 1));
    QOrganizerCollectionId id2(makeId("a", 1));
    QVERIFY(!(id1 < id2));
    QVERIFY(!(id2 < id1));
    QVERIFY(id1 == id2);
    QOrganizerCollectionId id3(makeId("a", 2));
    QOrganizerCollectionId id4(makeId("b", 1));
    QOrganizerCollectionId id5(makeId(QString(), 2));
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
    // TODO: review tests
    QOrganizerCollectionId id1(makeId(QString(), 1));
    QOrganizerCollectionId id2(makeId(QString(), 1));
    QOrganizerCollectionId id3(makeId(QString(), 2));
    QOrganizerCollectionId id4(makeId("a", 1));
    QVERIFY(qHash(id1) == qHash(id2));
    QVERIFY(qHash(id1) != qHash(id3));
    // note that the hash function is dependent on the id type
    // in BasicCollectionLocalId, the hash function ignores the managerUri.

    QSet<QOrganizerCollectionId> set;
    set.insert(id1);
    set.insert(id2);
    set.insert(id3);
    set.insert(id4);
    QCOMPARE(set.size(), 3);
}

void tst_QOrganizerCollection::idStringFunctions()
{
    // TODO: review test
    QOrganizerCollectionId id1(makeId("a", 1));
    QOrganizerCollectionId id2(makeId("a", 1));
    QOrganizerCollectionId id3(makeId("b", 1));
    QOrganizerCollectionId id4(makeId("a", 2));
    QVERIFY(qHash(id1) == qHash(id2));
    QVERIFY(qHash(id1) != qHash(id4));

    // note that the toString and fromString functions are
    // engine and id specific.  This test merely checks that
    // the API is hooked up correctly.

    QVERIFY(id1.toString() == id2.toString());
    QVERIFY(id1.toString() != id3.toString());
    QVERIFY(id1.toString() != id4.toString());
    QVERIFY(id3.toString() != id4.toString());

    // this should "work" -- string of the correct format
    QString prebuiltidstring = QString("qtorganizer") + QString(":") + QString("a") + QString("::") + QString::number(2);
    QOrganizerCollectionId rebuiltid = QOrganizerCollectionId::fromString(prebuiltidstring);
    // QVERIFY(rebuiltid == id4); // -- this requires a working backend.

    // this string has the right format and one parameter, but requires a working backend
    prebuiltidstring = QString("qtorganizer") + QString(":") + QString("a") + QString(":") + QString("key=value") + QString(":") + QString::number(2);
    rebuiltid = QOrganizerCollectionId::fromString(prebuiltidstring);
    // QVERIFY(rebuiltid == id4); // -- this requires a working backend.

    // this string has the right format and some parameters, but requires a working backend
    prebuiltidstring = QString("qtorganizer") + QString(":") + QString("a") + QString(":") + QString("key=value&key2=value2") + QString(":") + QString::number(2);
    rebuiltid = QOrganizerCollectionId::fromString(prebuiltidstring);
    // QVERIFY(rebuiltid == id4); // -- this requires a working backend.

    // this string has the right format but misses the value for a parameter
    prebuiltidstring = QString("qtorganizer") + QString(":") + QString("a") + QString(":") + QString("key=value&key2=") + QString(":") + QString::number(2);
    rebuiltid = QOrganizerCollectionId::fromString(prebuiltidstring);
    // QVERIFY(rebuiltid == id4); // -- this requires a working backend.

    // this string misses a field (the parameters)
    prebuiltidstring = QString("qtorganizer") + QString(":") + QString("a") + QString(":") + QString::number(2);
    rebuiltid = QOrganizerCollectionId::fromString(prebuiltidstring);
    QVERIFY(rebuiltid == QOrganizerCollectionId()); // invalid so should be null.

    // this string misses two fields (params plus manager uri)
    prebuiltidstring = QString("qtorganizer") + QString(":") + QString::number(2);
    rebuiltid = QOrganizerCollectionId::fromString(prebuiltidstring);
    QVERIFY(rebuiltid == QOrganizerCollectionId()); // invalid so should be null.

    // this string misses the prefix (qtorganizer)
    prebuiltidstring = QString("notorganizer") + QString(":") + QString("a") + QString("::") + QString::number(2);
    rebuiltid = QOrganizerCollectionId::fromString(prebuiltidstring);
    QVERIFY(rebuiltid == QOrganizerCollectionId()); // invalid so should be null.

    // this string misses the manager uri
    prebuiltidstring = QString("notorganizer") + QString(":::") + QString::number(2);
    rebuiltid = QOrganizerCollectionId::fromString(prebuiltidstring);
    QVERIFY(rebuiltid == QOrganizerCollectionId()); // invalid so should be null.
}

void tst_QOrganizerCollection::hash()
{
    // TODO: review tests
    QOrganizerCollectionId id(makeId("a", 1));
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
        QOrganizerManager om("memory");
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
        collectionIn.setId(QOrganizerCollectionId());
        stream1 << collectionIn;
        QVERIFY(buffer.size() > 0);
        QDataStream stream2(buffer);
        stream2 >> collectionOut;
        QCOMPARE(collectionOut, collectionIn); // can use QCOMPARE for collections, since no detail keys.
    }

    /* TODO: Review tests
    // third, stream an item with an id with the mgr uri null, local id set
    {
        QDataStream stream1(&buffer, QIODevice::WriteOnly);
        QOrganizerCollectionId modifiedId = originalId;
        modifiedId.setManagerUri(QString()); // this will clear the local id!
        modifiedId.setId(originalId.localId()); // so reset it and make sure nothing bad happens.
        collectionIn.setId(modifiedId);
        stream1 << collectionIn;
        QVERIFY(buffer.size() > 0);
        QDataStream stream2(buffer);
        stream2 >> collectionOut;
        QVERIFY(collectionOut.metaData() == collectionIn.metaData());
        QVERIFY(collectionOut.id() != collectionIn.id()); // no manager uri of input :. won't be serialized.
    }*/

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
        QString serializedId = inputId.toString();
        outputId = QOrganizerCollectionId::fromString(serializedId);
        QCOMPARE(inputId, outputId);

        inputId = originalId;
        buffer.clear();
        QDataStream stream1(&buffer, QIODevice::WriteOnly);
        stream1 << inputId;
        QVERIFY(buffer.size() > 0);
        QDataStream stream2(buffer);
        stream2 >> outputId;
        QCOMPARE(inputId, outputId);
    }

    /* TODO: review test
    // second, stream a partial id (mgr uri null, local id set)
    {
        inputId.setManagerUri(QString());
        inputId.setId(originalId.localId());
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
        inputId.setId(QOrganizerCollectionId());
        buffer.clear();
        QDataStream stream1(&buffer, QIODevice::WriteOnly);
        stream1 << inputId;
        QVERIFY(buffer.size() > 0);
        QDataStream stream2(buffer);
        stream2 >> outputId;
        QCOMPARE(inputId, outputId);
    }*/

    // fourth, stream a null id
    {
        inputId = QOrganizerCollectionId();
        QString serializedId = inputId.toString();
        outputId = QOrganizerCollectionId::fromString(serializedId);
        QCOMPARE(inputId, outputId);

        inputId = QOrganizerCollectionId();
        buffer.clear();
        QDataStream stream1(&buffer, QIODevice::WriteOnly);
        stream1 << inputId;
        QVERIFY(buffer.size() > 0);
        QDataStream stream2(buffer);
        stream2 >> outputId;
        QCOMPARE(inputId, outputId);
    }

    /* TODO: review test
    // fifth, stream an id after changing it's manager uri.
    {
        inputId.setManagerUri(originalId.managerUri());
        inputId.setId(originalId.localId());
        inputId.setManagerUri("test manager uri"); // should clear the local id.
        QVERIFY(inputId.localId() == QOrganizerCollectionId());
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
        inputId.setId(originalId.localId());
        buffer.clear();
        QDataStream stream1(&buffer, QIODevice::WriteOnly);
        stream1 << inputId;
        QVERIFY(buffer.size() > 0);
        QDataStream stream2(buffer);
        stream2 >> outputId;
        QVERIFY(outputId.isNull());
    } */
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
    QTypeInfo<QTM_PREPEND_NAMESPACE(QOrganizerCollectionId)> ti;
    QVERIFY(ti.isComplex); // unlike QContactLocalId (int typedef), we have a ctor
    QVERIFY(!ti.isStatic);
    QVERIFY(!ti.isLarge);
    QVERIFY(!ti.isPointer);
    QVERIFY(!ti.isDummy);
}

QTEST_MAIN(tst_QOrganizerCollection)
#include "tst_qorganizercollection.moc"
