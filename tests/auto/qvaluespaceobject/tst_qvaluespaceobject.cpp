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
** contained in Technology Preview License Agreement accompanying
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

#include <qvaluespace.h>
#include <qvaluespacemanager_p.h>
#include <qvaluespaceobject.h>

#include <QTest>
#include <QDebug>
#include <QSignalSpy>

#define QTRY_COMPARE(a,e)                       \
    for (int _i = 0; _i < 5000; _i += 100) {    \
        if ((a) == (e)) break;                  \
        QTest::qWait(100);                      \
    }                                           \
    QCOMPARE(a, e)

#define QTRY_VERIFY(a)                       \
    for (int _i = 0; _i < 5000; _i += 100) {    \
        if (a) break;                  \
        QTest::qWait(100);                      \
    }                                           \
    QVERIFY(a)

class ChangeListener : public QObject
{
    Q_OBJECT
Q_SIGNALS:
    void changeValue(const QByteArray&, const QVariant&);
    void itemRemove(const QByteArray&);
    void itemNotify(const QByteArray&,bool);
};

class tst_QValueSpaceObject: public QObject
{
    Q_OBJECT

public:
    enum Type { CharStar, String, ByteArray };

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testConstructor_data();
    void testConstructor();

    void testSignals_data();
    void testSignals();

private:
    int variantMetaTypeId;
};

Q_DECLARE_METATYPE(tst_QValueSpaceObject::Type)
Q_DECLARE_METATYPE(QAbstractValueSpaceLayer *)
Q_DECLARE_METATYPE(QUuid)
Q_DECLARE_METATYPE(QVariant)

void tst_QValueSpaceObject::initTestCase()
{
    variantMetaTypeId = qRegisterMetaType<QVariant>("QVariant");

    QValueSpace::initValuespaceManager();
}

void tst_QValueSpaceObject::cleanupTestCase()
{
}

#define ADD(layer, path, canonical, id, valid) do {\
    const QString layerName(layer ? layer->name() : 0); \
    QTest::newRow((layerName + ' ' + path + " const char *").toLocal8Bit().constData()) \
        << layer << CharStar << path << canonical << id << valid; \
    QTest::newRow((layerName + ' ' + path + " const QString &").toLocal8Bit().constData()) \
        << layer << String << path << canonical << id << valid; \
    QTest::newRow((layerName + ' ' + path + " const QByteArray &").toLocal8Bit().constData()) \
        << layer << ByteArray << path << canonical << id << valid; \
} while (false)

void tst_QValueSpaceObject::testConstructor_data()
{
    QTest::addColumn<QAbstractValueSpaceLayer *>("layer");
    QTest::addColumn<Type>("type");
    QTest::addColumn<QString>("path");
    QTest::addColumn<QString>("canonical");
    QTest::addColumn<QUuid>("uuid");
    QTest::addColumn<bool>("valid");

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();

    // add all known layers
    for (int i = 0; i < layers.count(); ++i) {
        QAbstractValueSpaceLayer *layer = layers.at(i);

        ADD(layer, QString(""), QString("/"), layer->id(), true);
        ADD(layer, QString("/"), QString("/"), layer->id(), true);
        ADD(layer, QString("//"), QString("/"), layer->id(), true);
        ADD(layer, QString("/testConstructor"), QString("/testConstructor"), layer->id(), true);
        ADD(layer, QString("/testConstructor/"), QString("/testConstructor"), layer->id(), true);
        ADD(layer, QString("testConstructor"), QString("/testConstructor"), layer->id(), true);
        ADD(layer, QString("/testConstructor/subpath"), QString("/testConstructor/subpath"), layer->id(), true);
    }

    // auto selection
    ADD(reinterpret_cast<QAbstractValueSpaceLayer *>(0), QString(), QString("/"), QUuid(), !layers.isEmpty());

    // unknown uuid
    ADD(reinterpret_cast<QAbstractValueSpaceLayer *>(0), QString(), QString("/"), QUuid("{9fa51477-7730-48e0-aee1-3eeb5f0c0c5b}"), false);
}

void tst_QValueSpaceObject::testConstructor()
{
    QFETCH(QAbstractValueSpaceLayer *, layer);
    QFETCH(Type, type);
    QFETCH(QString, path);
    QFETCH(QString, canonical);
    QFETCH(QUuid, uuid);
    QFETCH(bool, valid);

    QValueSpaceObject *object;

    switch (type) {
    case CharStar:
        object = new QValueSpaceObject(path.toUtf8().constData(), uuid);
        break;
    case String:
        object = new QValueSpaceObject(path, uuid);
        break;
    case ByteArray:
        object = new QValueSpaceObject(path.toUtf8(), uuid);
        break;
    default:
        QFAIL("Invalid type.");
    };

    QCOMPARE(object->objectPath(), canonical);
    QCOMPARE(object->isValid(), valid);

    if (layer) {
        QAbstractValueSpaceLayer::Handle handle =
            layer->item(QAbstractValueSpaceLayer::InvalidHandle, canonical.toUtf8());

        QVariant data;
        QVERIFY(!layer->value(handle, "/value", &data));

        layer->removeHandle(handle);
    }

    object->setAttribute("value", 100);
    object->sync();

    if (layer) {
        QAbstractValueSpaceLayer::Handle handle =
            layer->item(QAbstractValueSpaceLayer::InvalidHandle, canonical.toUtf8());

        QVariant data;
        QVERIFY(layer->value(handle, "/value", &data));
        QCOMPARE(data.toInt(), 100);

        layer->removeHandle(handle);
    }

    object->removeAttribute("value");
    object->sync();

    if (layer) {
        QAbstractValueSpaceLayer::Handle handle =
            layer->item(QAbstractValueSpaceLayer::InvalidHandle, canonical.toUtf8());

        QVariant data;
        QVERIFY(!layer->value(handle, "/value", &data));

        layer->removeHandle(handle);
    }

    delete object;
}

void tst_QValueSpaceObject::testSignals_data()
{
    QTest::addColumn<QString>("objectPath");
    QTest::addColumn<QByteArray>("objectAttribute");
    QTest::addColumn<QString>("itemPath");
    QTest::addColumn<QString>("itemAttribute");

    QTest::newRow("root") << QString("/") << QByteArray() << QString("/") << QString();
    QTest::newRow("") << QString("/testSignals") << QByteArray("/value") << QString("/testSignals") << QString("value");
}

void tst_QValueSpaceObject::testSignals()
{
    QFETCH(QString, objectPath);
    QFETCH(QByteArray, objectAttribute);
    QFETCH(QString, itemPath);
    QFETCH(QString, itemAttribute);

    QValueSpaceObject *object = new QValueSpaceObject(objectPath);

    if (!object->supportsRequests()) {
        delete object;
        QSKIP("Underlying layer does not support requests.", SkipAll);
    }

    ChangeListener listener;
    connect(object, SIGNAL(itemSetValue(QByteArray,QVariant)),
            &listener, SIGNAL(changeValue(QByteArray,QVariant)));
    connect(object, SIGNAL(itemRemove(QByteArray)),
            &listener, SIGNAL(itemRemove(QByteArray)));
    connect(object, SIGNAL(itemNotify(QByteArray,bool)),
            &listener, SIGNAL(itemNotify(QByteArray,bool)));

    QSignalSpy setValueSpy(&listener, SIGNAL(changeValue(QByteArray,QVariant)));
    QSignalSpy removeSpy(&listener, SIGNAL(itemRemove(QByteArray)));
    QSignalSpy itemNotifySpy(&listener, SIGNAL(itemNotify(QByteArray,bool)));

    QValueSpaceItem *item = new QValueSpaceItem(itemPath);

    QTRY_COMPARE(itemNotifySpy.count(), 1);
    QVERIFY(setValueSpy.isEmpty());
    QVERIFY(removeSpy.isEmpty());

    QList<QVariant> arguments = itemNotifySpy.takeFirst();
    QCOMPARE(arguments.count(), 2);
    QCOMPARE(arguments.at(0).type(), QVariant::ByteArray);
    QVERIFY(arguments.at(0).toByteArray().isEmpty());
    QCOMPARE(arguments.at(1).type(), QVariant::Bool);
    QVERIFY(arguments.at(1).toBool());

    if (itemAttribute.isEmpty())
        item->setValue(10);
    else
        item->setValue(itemAttribute, 10);
    item->sync();

    QTRY_COMPARE(setValueSpy.count(), 1);
    QVERIFY(removeSpy.isEmpty());
    QVERIFY(itemNotifySpy.isEmpty());

    arguments = setValueSpy.takeFirst();
    QCOMPARE(arguments.count(), 2);
    QCOMPARE(arguments.at(0).type(), QVariant::ByteArray);
    QCOMPARE(arguments.at(0).toByteArray(), objectAttribute);
    QCOMPARE(arguments.at(1).userType(), variantMetaTypeId);
    QCOMPARE(arguments.at(1).value<QVariant>().type(), QVariant::Int);
    QCOMPARE(arguments.at(1).value<QVariant>().toInt(), 10);

    item->remove(itemAttribute);
    item->sync();

    QTRY_COMPARE(removeSpy.count(), 1);
    QVERIFY(setValueSpy.isEmpty());
    QVERIFY(itemNotifySpy.isEmpty());

    arguments = removeSpy.takeFirst();
    QCOMPARE(arguments.count(), 1);
    QCOMPARE(arguments.at(0).type(), QVariant::ByteArray);
    QCOMPARE(arguments.at(0).toByteArray(), objectAttribute);

    delete item;

    QTRY_COMPARE(itemNotifySpy.count(), 1);
    QVERIFY(setValueSpy.isEmpty());
    QVERIFY(removeSpy.isEmpty());

    arguments = itemNotifySpy.takeFirst();
    QCOMPARE(arguments.count(), 2);
    QCOMPARE(arguments.at(0).type(), QVariant::ByteArray);
    QVERIFY(arguments.at(0).toByteArray().isEmpty());
    QCOMPARE(arguments.at(1).type(), QVariant::Bool);
    QVERIFY(!arguments.at(1).toBool());
}

QTEST_MAIN(tst_QValueSpaceObject)
#include "tst_qvaluespaceobject.moc"
