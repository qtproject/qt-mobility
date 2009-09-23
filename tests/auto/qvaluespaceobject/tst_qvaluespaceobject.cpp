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
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <qvaluespaceitem.h>
#include <qvaluespacemanager_p.h>
#include <qvaluespaceobject.h>

#include <QTest>
#include <QDebug>
#include <QSignalSpy>

#ifdef Q_OS_WIN
#define _WIN32_WINNT 0x0500
#include <windows.h>
#endif

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
    void itemNotify(const QByteArray&,bool);
};

class tst_QValueSpaceObject: public QObject
{
    Q_OBJECT

public:
    enum Type { CharStar, String };

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testConstructor_data();
    void testConstructor();
    void testFilterConstructor_data();
    void testFilterConstructor();
    void testBaseConstructor();

    void testSignals_data();
    void testSignals();

    void valuePermanence_data();
    void valuePermanence();

private:
    int variantMetaTypeId;
};

Q_DECLARE_METATYPE(tst_QValueSpaceObject::Type)
Q_DECLARE_METATYPE(QAbstractValueSpaceLayer *)
Q_DECLARE_METATYPE(QUuid)
Q_DECLARE_METATYPE(QVariant)
Q_DECLARE_METATYPE(QValueSpace::LayerOptions)

void tst_QValueSpaceObject::initTestCase()
{
    variantMetaTypeId = qRegisterMetaType<QVariant>("QVariant");

#ifdef Q_OS_WIN
    HKEY key;
    long result = RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Nokia",
                               0, KEY_ALL_ACCESS, &key);
    if (result == ERROR_SUCCESS) {
        result = RegDeleteKey(key, L"QtMobility\\volatileContext");
        result = RegDeleteKey(key, L"QtMobility\\nonVolatileContext");
        result = RegDeleteKey(key, L"QtMobility");

        RegCloseKey(key);
    }
#endif

    QValueSpace::initValueSpaceManager();
}

void tst_QValueSpaceObject::cleanupTestCase()
{
}

#define ADD(layer, id, path, canonical, valid) do {\
    const QString layerName(layer ? layer->name() : 0); \
    QTest::newRow((layerName + ' ' + path + " const char *").toLocal8Bit().constData()) \
        << layer << id << CharStar << path << canonical << valid; \
    QTest::newRow((layerName + ' ' + path + " const QString &").toLocal8Bit().constData()) \
        << layer << id << String << path << canonical << valid; \
} while (false)

void tst_QValueSpaceObject::testConstructor_data()
{
    QTest::addColumn<QAbstractValueSpaceLayer *>("layer");
    QTest::addColumn<QUuid>("uuid");

    QTest::addColumn<Type>("type");
    QTest::addColumn<QString>("path");
    QTest::addColumn<QString>("canonical");
    QTest::addColumn<bool>("valid");

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();

    // add all known layers
    for (int i = 0; i < layers.count(); ++i) {
        QAbstractValueSpaceLayer *layer = layers.at(i);

        ADD(layer, layer->id(), QString(""), QString("/"), true);
        ADD(layer, layer->id(), QString("/"), QString("/"), true);
        ADD(layer, layer->id(), QString("//"), QString("/"), true);
        ADD(layer, layer->id(), QString("/testConstructor"), QString("/testConstructor"), true);
        ADD(layer, layer->id(), QString("/testConstructor/"), QString("/testConstructor"), true);
        ADD(layer, layer->id(), QString("testConstructor"), QString("/testConstructor"), true);
        ADD(layer, layer->id(), QString("/testConstructor/subpath"),\
                                QString("/testConstructor/subpath"), true);
    }

    // unknown uuid
    ADD(reinterpret_cast<QAbstractValueSpaceLayer *>(0),
        QUuid("{9fa51477-7730-48e0-aee1-3eeb5f0c0c5b}"), QString(), QString("/"), false);
}

#undef ADD

void tst_QValueSpaceObject::testConstructor()
{
    QFETCH(QAbstractValueSpaceLayer *, layer);
    QFETCH(QUuid, uuid);

    QFETCH(Type, type);
    QFETCH(QString, path);
    QFETCH(QString, canonical);
    QFETCH(bool, valid);

    QValueSpaceObject *object;

    switch (type) {
    case CharStar:
        object = new QValueSpaceObject(path.toUtf8().constData(), uuid);
        break;
    case String:
        object = new QValueSpaceObject(path, uuid);
        break;
    default:
        QFAIL("Invalid type.");
    };

    QCOMPARE(object->path(), canonical);
    QCOMPARE(object->isValid(), valid);

    if (layer) {
        QAbstractValueSpaceLayer::Handle handle =
            layer->item(QAbstractValueSpaceLayer::InvalidHandle, canonical.toUtf8());

        QVariant data;
        QVERIFY(!layer->value(handle, "/value", &data));

        layer->removeHandle(handle);
    }

    switch (type) {
    case CharStar:
        object->setAttribute("value", 100);
        break;
    case String:
        object->setAttribute(QString("value"), 100);
        break;
    };
    object->sync();

    if (layer) {
        QAbstractValueSpaceLayer::Handle handle =
            layer->item(QAbstractValueSpaceLayer::InvalidHandle, canonical.toUtf8());

        QVariant data;
        QVERIFY(layer->value(handle, "/value", &data));
        QCOMPARE(data.toInt(), 100);

        layer->removeHandle(handle);
    }

    switch (type) {
    case CharStar:
        object->removeAttribute("value");
        break;
    case String:
        object->removeAttribute(QString("value"));
        break;
    };
    object->sync();

    if (layer) {
        QAbstractValueSpaceLayer::Handle handle =
            layer->item(QAbstractValueSpaceLayer::InvalidHandle, canonical.toUtf8());

        QVariant data;
        QVERIFY(!layer->value(handle, "/value", &data));

        layer->removeHandle(handle);
    }

    delete object;

    if (layer && layer->layerOptions() & QValueSpace::PermanentLayer) {
        QValueSpaceObject root("/", uuid);
        while (!canonical.isEmpty()) {
            root.removeAttribute(canonical.mid(1));
            canonical.truncate(canonical.lastIndexOf('/'));
        }
        root.sync();
    }
}

#define ADD(opt, valid) do {\
    QTest::newRow(QString::number(opt).append(" const char *").toLocal8Bit().constData()) \
        << (QValueSpace::UnspecifiedLayer | opt) << CharStar << valid; \
    QTest::newRow(QString::number(opt).append(" const QString &").toLocal8Bit().constData()) \
        << (QValueSpace::UnspecifiedLayer | opt) << String << valid; \
} while (false)

void tst_QValueSpaceObject::testFilterConstructor_data()
{
    QTest::addColumn<QValueSpace::LayerOptions>("options");
    QTest::addColumn<Type>("type");
    QTest::addColumn<bool>("valid");

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();

    for (int i = 0; i < layers.count(); ++i) {
        QAbstractValueSpaceLayer *layer = layers.at(i);

        ADD(layer->layerOptions(), true);
    }

    ADD(QValueSpace::PermanentLayer | QValueSpace::NonPermanentLayer,
        false);
    ADD(QValueSpace::WriteableLayer | QValueSpace::NonWriteableLayer,
        false);
}

void tst_QValueSpaceObject::testFilterConstructor()
{
    QFETCH(QValueSpace::LayerOptions, options);
    QFETCH(Type, type);
    QFETCH(bool, valid);

    QValueSpaceObject *object;

    switch (type) {
    case CharStar:
        object = new QValueSpaceObject("/", options);
        break;
    case String:
        object = new QValueSpaceObject(QString("/"), options);
        break;
    default:
        QFAIL("Invalid type");
        return;
    };

    QCOMPARE(object->isValid(), valid);
}

void tst_QValueSpaceObject::testBaseConstructor()
{
    {
        QValueSpaceObject object("/");
        QVERIFY(object.isValid());
    }

    {
        QValueSpaceObject object(QString("/"));
        QVERIFY(object.isValid());
    }
}

void tst_QValueSpaceObject::testSignals_data()
{
    QTest::addColumn<QAbstractValueSpaceLayer *>("layer");

    QTest::addColumn<QString>("objectPath");
    QTest::addColumn<QByteArray>("objectAttribute");
    QTest::addColumn<QString>("itemPath");
    QTest::addColumn<QString>("itemAttribute");

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();

    bool skip = true;

    for (int i = 0; i < layers.count(); ++i) {
        QAbstractValueSpaceLayer *layer = layers.at(i);

        if (!layer->supportsRequests())
            continue;

        skip = false;

        QTest::newRow("root")
            << layer
            << QString("/")
            << QByteArray()
            << QString("/")
            << QString();

        QTest::newRow("")
            << layer
            << QString("/testSignals")
            << QByteArray("/value")
            << QString("/testSignals")
            << QString("value");
    }

    if (skip)
        QSKIP("No applicable layers found.", SkipAll);
}

void tst_QValueSpaceObject::testSignals()
{
    QFETCH(QAbstractValueSpaceLayer *, layer);

    QFETCH(QString, objectPath);
    QFETCH(QByteArray, objectAttribute);
    QFETCH(QString, itemPath);
    QFETCH(QString, itemAttribute);

    QValueSpaceObject *object = new QValueSpaceObject(objectPath, layer->id());

    ChangeListener listener;
    connect(object, SIGNAL(itemNotify(QByteArray,bool)),
            &listener, SIGNAL(itemNotify(QByteArray,bool)));

    QSignalSpy itemNotifySpy(&listener, SIGNAL(itemNotify(QByteArray,bool)));

    QValueSpaceItem *item = new QValueSpaceItem(itemPath, layer->id());

    QTRY_COMPARE(itemNotifySpy.count(), 1);

    QList<QVariant> arguments = itemNotifySpy.takeFirst();
    QCOMPARE(arguments.count(), 2);
    QCOMPARE(arguments.at(0).type(), QVariant::ByteArray);
    QVERIFY(arguments.at(0).toByteArray().isEmpty());
    QCOMPARE(arguments.at(1).type(), QVariant::Bool);
    QVERIFY(arguments.at(1).toBool());

    delete item;

    QTRY_COMPARE(itemNotifySpy.count(), 1);

    arguments = itemNotifySpy.takeFirst();
    QCOMPARE(arguments.count(), 2);
    QCOMPARE(arguments.at(0).type(), QVariant::ByteArray);
    QVERIFY(arguments.at(0).toByteArray().isEmpty());
    QCOMPARE(arguments.at(1).type(), QVariant::Bool);
    QVERIFY(!arguments.at(1).toBool());
}

void tst_QValueSpaceObject::valuePermanence_data()
{
    QTest::addColumn<QAbstractValueSpaceLayer *>("layer");

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();

    // add all known layers
    for (int i = 0; i < layers.count(); ++i) {
        QAbstractValueSpaceLayer *layer = layers.at(i);

        QTest::newRow(layer->name().toLocal8Bit().constData()) << layer;
    }
}

void tst_QValueSpaceObject::valuePermanence()
{
    QFETCH(QAbstractValueSpaceLayer *, layer);

    QValueSpaceObject *object = new QValueSpaceObject("/valuePermanence", layer->id());

    object->setAttribute("value", 10);

    QValueSpaceItem item("/valuePermanence");
    QCOMPARE(item.value("value", 0).toInt(), 10);

    delete object;

    if (layer->layerOptions() & QValueSpace::PermanentLayer) {
        // Permanent layer, check that value is still available after object is deleted.
        QCOMPARE(item.value("value", 0).toInt(), 10);

        object = new QValueSpaceObject("/valuePermanence", layer->id());

        object->removeAttribute("value");

        QCOMPARE(item.value("value", 0).toInt(), 0);

        object->removeAttribute(QByteArray());

        delete object;
    } else {
        // Non-permanent layer, check that value is not available after object is deleted.
        QCOMPARE(item.value("value", 0).toInt(), 0);
    }
}


QTEST_MAIN(tst_QValueSpaceObject)
#include "tst_qvaluespaceobject.moc"
