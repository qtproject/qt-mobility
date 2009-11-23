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

#include <qvaluespacesubscriber.h>
#include <qvaluespacemanager_p.h>
#include <qvaluespaceprovider.h>

#include <QThread>
#include <QVector>

#include <QTest>
#include <QDebug>
#include <QSignalSpy>
#include <QFile>

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

QTM_USE_NAMESPACE
class ChangeListener : public QObject
{
    Q_OBJECT
Q_SIGNALS:
    void attributeInterestChanged(const QString&, bool);
};

class tst_QValueSpaceProvider: public QObject
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

    void threads_data();
    void threads();

private:
    int variantMetaTypeId;
};

Q_DECLARE_METATYPE(tst_QValueSpaceProvider::Type)
Q_DECLARE_METATYPE(QAbstractValueSpaceLayer *)
Q_DECLARE_METATYPE(QUuid)
Q_DECLARE_METATYPE(QVariant)
Q_DECLARE_METATYPE(QValueSpace::LayerOptions)

void tst_QValueSpaceProvider::initTestCase()
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

#ifdef Q_OS_UNIX
    QFile::remove("/tmp/qt-0/valuespace_shmlayer");
#endif

    QValueSpace::initValueSpaceServer();
}

void tst_QValueSpaceProvider::cleanupTestCase()
{
}

#define ADD(layer, id, path, canonical, valid) do {\
    const QString layerName(layer ? layer->name() : 0); \
    QTest::newRow((layerName + ' ' + path + " const char *").toLocal8Bit().constData()) \
        << layer << id << CharStar << path << canonical << valid; \
    QTest::newRow((layerName + ' ' + path + " const QString &").toLocal8Bit().constData()) \
        << layer << id << String << path << canonical << valid; \
} while (false)

void tst_QValueSpaceProvider::testConstructor_data()
{
    QTest::addColumn<QAbstractValueSpaceLayer *>("layer");
    QTest::addColumn<QUuid>("uuid");

    QTest::addColumn<Type>("type");
    QTest::addColumn<QString>("path");
    QTest::addColumn<QString>("canonical");
    QTest::addColumn<bool>("connected");

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

void tst_QValueSpaceProvider::testConstructor()
{
    QFETCH(QAbstractValueSpaceLayer *, layer);
    QFETCH(QUuid, uuid);

    QFETCH(Type, type);
    QFETCH(QString, path);
    QFETCH(QString, canonical);
    QFETCH(bool, connected);

    QValueSpaceProvider *provider;

    switch (type) {
    case CharStar:
        provider = new QValueSpaceProvider(path.toUtf8().constData(), uuid);
        break;
    case String:
        provider = new QValueSpaceProvider(path, uuid);
        break;
    default:
        QFAIL("Invalid type.");
    };

    QCOMPARE(provider->path(), canonical);
    QCOMPARE(provider->isConnected(), connected);

    if (layer) {
        QAbstractValueSpaceLayer::Handle handle =
            layer->item(QAbstractValueSpaceLayer::InvalidHandle, canonical.toUtf8());

        QVariant data;
        QVERIFY(!layer->value(handle, "/value", &data));

        layer->removeHandle(handle);
    }

    switch (type) {
    case CharStar:
        provider->setAttribute("value", 100);
        break;
    case String:
        provider->setAttribute(QString("value"), 100);
        break;
    };
    provider->sync();

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
        provider->removeAttribute("value");
        break;
    case String:
        provider->removeAttribute(QString("value"));
        break;
    };
    QValueSpaceProvider::syncAll();

    if (layer) {
        QAbstractValueSpaceLayer::Handle handle =
            layer->item(QAbstractValueSpaceLayer::InvalidHandle, canonical.toUtf8());

        QVariant data;
        QVERIFY(!layer->value(handle, "/value", &data));

        layer->removeHandle(handle);
    }

    delete provider;

    if (layer && layer->layerOptions() & QValueSpace::PermanentLayer) {
        QValueSpaceProvider root("/", uuid);
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

void tst_QValueSpaceProvider::testFilterConstructor_data()
{
    QTest::addColumn<QValueSpace::LayerOptions>("options");
    QTest::addColumn<Type>("type");
    QTest::addColumn<bool>("connected");

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();

    for (int i = 0; i < layers.count(); ++i) {
        QAbstractValueSpaceLayer *layer = layers.at(i);

        ADD(layer->layerOptions(), true);
    }

    ADD(QValueSpace::PermanentLayer | QValueSpace::NonPermanentLayer,
        false);
    ADD(QValueSpace::WritableLayer | QValueSpace::NonWritableLayer,
        false);
}

void tst_QValueSpaceProvider::testFilterConstructor()
{
    QFETCH(QValueSpace::LayerOptions, options);
    QFETCH(Type, type);
    QFETCH(bool, connected);

    QValueSpaceProvider *provider;

    switch (type) {
    case CharStar:
        provider = new QValueSpaceProvider("/", options);
        break;
    case String:
        provider = new QValueSpaceProvider(QString("/"), options);
        break;
    default:
        QFAIL("Invalid type");
        return;
    };

    QCOMPARE(provider->isConnected(), connected);
}

void tst_QValueSpaceProvider::testBaseConstructor()
{
    {
        QValueSpaceProvider provider("/");
        QVERIFY(provider.isConnected());
    }

    {
        QValueSpaceProvider provider(QString("/"));
        QVERIFY(provider.isConnected());
    }
}

void tst_QValueSpaceProvider::testSignals_data()
{
    QTest::addColumn<QAbstractValueSpaceLayer *>("layer");

    QTest::addColumn<QString>("providerPath");
    QTest::addColumn<QString>("subscriberPath");
    QTest::addColumn<QString>("attribute");

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();

    bool foundSupported = false;

    for (int i = 0; i < layers.count(); ++i) {
        QAbstractValueSpaceLayer *layer = layers.at(i);

        if (!layer->supportsInterestNotification())
            continue;

        foundSupported = true;

        QTest::newRow("/ /")
            << layer
            << QString("/")
            << QString("/")
            << QString();

        QTest::newRow("/ /testSignals")
            << layer
            << QString("/")
            << QString("/testSignals")
            << QString("testSignals");

        QTest::newRow("/testSignals /testSignals")
            << layer
            << QString("/testSignals")
            << QString("/testSignals")
            << QString();
    }

    if (!foundSupported)
        QSKIP("No layer supporting interest notifications found.", SkipAll);
}

void tst_QValueSpaceProvider::testSignals()
{
    QFETCH(QAbstractValueSpaceLayer *, layer);

    QFETCH(QString, providerPath);
    QFETCH(QString, subscriberPath);
    QFETCH(QString, attribute);

    QValueSpaceProvider *provider = new QValueSpaceProvider(providerPath, layer->id());

    ChangeListener listener;
    connect(provider, SIGNAL(attributeInterestChanged(QString,bool)),
            &listener, SIGNAL(attributeInterestChanged(QString,bool)));

    QSignalSpy interestChangedSpy(&listener, SIGNAL(attributeInterestChanged(QString,bool)));

    QValueSpaceSubscriber *subscriber = new QValueSpaceSubscriber(subscriberPath, layer->id());

    QTRY_COMPARE(interestChangedSpy.count(), 1);

    QList<QVariant> arguments = interestChangedSpy.takeFirst();
    QCOMPARE(arguments.count(), 2);
    QCOMPARE(arguments.at(0).type(), QVariant::String);
    QCOMPARE(arguments.at(0).toString(), attribute);
    QCOMPARE(arguments.at(1).type(), QVariant::Bool);
    QVERIFY(arguments.at(1).toBool());

    delete subscriber;

    QTRY_COMPARE(interestChangedSpy.count(), 1);

    arguments = interestChangedSpy.takeFirst();
    QCOMPARE(arguments.count(), 2);
    QCOMPARE(arguments.at(0).type(), QVariant::String);
    QCOMPARE(arguments.at(0).toString(), attribute);
    QCOMPARE(arguments.at(1).type(), QVariant::Bool);
    QVERIFY(!arguments.at(1).toBool());

    delete provider;
}

void tst_QValueSpaceProvider::valuePermanence_data()
{
    QTest::addColumn<QAbstractValueSpaceLayer *>("layer");

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();

    // add all known layers
    for (int i = 0; i < layers.count(); ++i) {
        QAbstractValueSpaceLayer *layer = layers.at(i);

        QTest::newRow(layer->name().toLocal8Bit().constData()) << layer;
    }
}

void tst_QValueSpaceProvider::valuePermanence()
{
    QFETCH(QAbstractValueSpaceLayer *, layer);

    QValueSpaceProvider *provider = new QValueSpaceProvider("/valuePermanence", layer->id());

    provider->setAttribute("value", 10);

    QValueSpaceSubscriber subscriber("/valuePermanence");
    QCOMPARE(subscriber.value("value", 0).toInt(), 10);

    delete provider;

    if (layer->layerOptions() & QValueSpace::PermanentLayer) {
        // Permanent layer, check that value is still available after provider is deleted.
        QCOMPARE(subscriber.value("value", 0).toInt(), 10);

        provider = new QValueSpaceProvider("/valuePermanence", layer->id());

        provider->removeAttribute("value");

        QCOMPARE(subscriber.value("value", 0).toInt(), 0);

        provider->removeAttribute(QString());

        delete provider;
    } else {
        // Non-permanent layer, check that value is not available after provider is deleted.
        QCOMPARE(subscriber.value("value", 0).toInt(), 0);
    }
}

class WriteThread : public QThread
{
    Q_OBJECT

public:
    WriteThread(const QString &path, const QUuid &uuid, unsigned int count);

    void runSequential() { run(); }

protected:
    void run();

private:
    QString path;
    unsigned int count;
    QValueSpaceProvider *provider;
};

WriteThread::WriteThread(const QString &path, const QUuid &uuid, unsigned int count)
:   path(path), count(count)
{
    provider = new QValueSpaceProvider(path, uuid, this);
}

void WriteThread::run()
{
    const QString key("key%1");
    const QString value("value%1");

    for (unsigned int i = 0; i < count; ++i)
        provider->setAttribute(key.arg(i), value.arg(i));

    provider->sync();
}

void tst_QValueSpaceProvider::threads_data()
{
    QTest::addColumn<QUuid>("uuid");

    QTest::addColumn<unsigned int>("threads");
    QTest::addColumn<unsigned int>("count");
    QTest::addColumn<bool>("sequential");

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();

    for (int i = 0; i < layers.count(); ++i) {
        QAbstractValueSpaceLayer *layer = layers.at(i);

        if (layer->id() == QVALUESPACE_NONVOLATILEREGISTRY_LAYER)
            continue;

#ifdef Q_OS_WINCE
        // Limit number of items on Windows CE to prevent out of disk space errors.
        if (layer->id() == QVALUESPACE_VOLATILEREGISTRY_LAYER) {
            QTest::newRow("1 thread, 10 items, sequential")
                << layer->id() << uint(1) << uint(10) << true;
            QTest::newRow("1 thread, 3000 items, sequential")
                << layer->id() << uint(1) << uint(3000) << true;
            QTest::newRow("2 threads, 10 items, sequential")
                << layer->id() << uint(2) << uint(10) << true;
            QTest::newRow("2 threads, 1500 items, sequential")
                << layer->id() << uint(2) << uint(1500) << true;
            QTest::newRow("4 threads, 750 items, sequential")
                << layer->id() << uint(4) << uint(750) << true;
            QTest::newRow("10 threads, 300 items, sequential")
                << layer->id() << uint(10) << uint(300) << true;

            QTest::newRow("1 thread, 10 items")
                << layer->id() << uint(1) << uint(10) << false;
            QTest::newRow("1 thread, 3000 items")
                << layer->id() << uint(1) << uint(3000) << false;
            QTest::newRow("2 threads, 10 items")
                << layer->id() << uint(2) << uint(10) << false;
            QTest::newRow("2 threads, 1500 items")
                << layer->id() << uint(2) << uint(1500) << false;
            QTest::newRow("4 threads, 750 items")
                << layer->id() << uint(4) << uint(750) << false;
            QTest::newRow("10 threads, 300 items")
                << layer->id() << uint(10) << uint(300) << false;
        } else
#endif

        // The Shared Memory layer can hold a maximum of 8191 nodes.
        if (layer->id() == QVALUESPACE_SHAREDMEMORY_LAYER) {
            QTest::newRow("1 thread, 10 items, sequential")
                << layer->id() << uint(1) << uint(10) << true;
            QTest::newRow("1 thread, 8000 items, sequential")
                << layer->id() << uint(1) << uint(8000) << true;
            QTest::newRow("2 threads, 10 items, sequential")
                << layer->id() << uint(2) << uint(10) << true;
            QTest::newRow("2 threads, 4000 items, sequential")
                << layer->id() << uint(2) << uint(4000) << true;
            QTest::newRow("4 threads, 2000 items, sequential")
                << layer->id() << uint(4) << uint(2000) << true;
            QTest::newRow("10 threads, 800 items, sequential")
                << layer->id() << uint(10) << uint(800) << true;
            QTest::newRow("100 threads, 80 items, sequential")
                << layer->id() << uint(100) << uint(80) << true;

            QTest::newRow("1 thread, 10 items")
                << layer->id() << uint(1) << uint(10) << false;
            QTest::newRow("1 thread, 8000 items")
                << layer->id() << uint(1) << uint(8000) << false;
            QTest::newRow("2 threads, 10 items")
                << layer->id() << uint(2) << uint(10) << false;
            QTest::newRow("2 threads, 4000 items")
                << layer->id() << uint(2) << uint(4000) << false;
            QTest::newRow("4 threads, 2000 items")
                << layer->id() << uint(4) << uint(2000) << false;
            QTest::newRow("10 threads, 800 items")
                << layer->id() << uint(10) << uint(800) << false;
            QTest::newRow("100 threads, 80 items")
                << layer->id() << uint(100) << uint(80) << false;
        } else {
            // Assume no limits on all other layers.
            QTest::newRow("1 thread, 10 items, sequential")
                << layer->id() << uint(1) << uint(10) << true;
            QTest::newRow("1 thread, 10000 items, sequential")
                << layer->id() << uint(1) << uint(10000) << true;
            QTest::newRow("2 threads, 10 items, sequential")
                << layer->id() << uint(2) << uint(10) << true;
            QTest::newRow("2 threads, 5000 items, sequential")
                << layer->id() << uint(2) << uint(5000) << true;
            QTest::newRow("100 threads, 100 items, sequential")
                << layer->id() << uint(100) << uint(100) << true;

            QTest::newRow("1 thread, 10 items")
                << layer->id() << uint(1) << uint(10) << false;
            QTest::newRow("1 thread, 10000 items")
                << layer->id() << uint(1) << uint(10000) << false;
            QTest::newRow("2 threads, 10 items")
                << layer->id() << uint(2) << uint(10) << false;
            QTest::newRow("2 threads, 5000 items")
                << layer->id() << uint(2) << uint(5000) << false;
            QTest::newRow("100 threads, 100 items")
                << layer->id() << uint(100) << uint(100) << false;
        }
    }
}

void tst_QValueSpaceProvider::threads()
{
    QFETCH(QUuid, uuid);
    QFETCH(unsigned int, threads);
    QFETCH(unsigned int, count);
    QFETCH(bool, sequential);

    QStringList expectedPaths;
    for (unsigned int i = 0; i < threads; ++i)
        expectedPaths.append(QString("thread%1").arg(i));

    QHash<QString, QString> expectedValues;
    for (unsigned int i = 0; i < count; ++i)
        expectedValues.insert(QString("key%1").arg(i), QString("value%1").arg(i));

    QValueSpaceSubscriber *subscriber = new QValueSpaceSubscriber("/threads", uuid);

    QVERIFY(subscriber->subPaths().isEmpty());

    QVector<WriteThread *> writeThreads(threads);

    // Create and start writer threads.
    for (unsigned int i = 0; i < threads; ++i) {
        writeThreads[i] =
            new WriteThread(QString("/threads/%1").arg(expectedPaths.at(i)), uuid, count);

        if (sequential)
            writeThreads[i]->runSequential();
        else
            writeThreads[i]->start();
    }

    if (!sequential) {
        // Wait for writer threads to finish.
        for (unsigned int i = 0; i < threads; ++i)
            writeThreads[i]->wait();
    }

    qDebug() << "Published" << count << "items in" << threads
             << "theads, totaling" << (threads * count);

    // Verify Value Space
    QStringList subPaths = subscriber->subPaths();
    if (subPaths.toSet() != expectedPaths.toSet()) {
        qDebug() << "Expected Paths:" << expectedPaths;
        qDebug() << "Actual Paths:" << subPaths;
    }
    QVERIFY(subPaths.toSet() == expectedPaths.toSet());

    while (!subPaths.isEmpty()) {
        QValueSpaceSubscriber threadItem;
        threadItem.setPath(subscriber);
        threadItem.cd(subPaths.takeFirst());

        QStringList keys = threadItem.subPaths();

        if (keys.toSet() != expectedValues.keys().toSet()) {
            qDebug() << "Expected value keys:" << expectedValues.keys();
            qDebug() << "Actual value keys:" << keys;
        }
        QVERIFY(keys.toSet() == expectedValues.keys().toSet());

        while (!keys.isEmpty()) {
            const QString key = keys.takeFirst();

            QCOMPARE(threadItem.value(key).toString(), expectedValues.value(key));
        }
    }

    delete subscriber;

    // Delete writer threads.
    for (unsigned int i = 0; i < threads; ++i)
        delete writeThreads[i];
}

QTEST_MAIN(tst_QValueSpaceProvider)
#include "tst_qvaluespaceprovider.moc"
