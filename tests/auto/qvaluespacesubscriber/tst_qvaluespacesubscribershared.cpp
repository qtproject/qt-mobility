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

#include "tst_qvaluespacesubscribershared.h"

#include <qvaluespacesubscriber.h>
#include <qvaluespaceprovider.h>
#include <qvaluespacemanager_p.h>

#include <QTest>
#include <QSet>
#include <QSignalSpy>
#include <QProcess>
#include <QFile>
#include <QThread>
#include <QTimer>

#include <QDebug>

#ifdef Q_OS_WIN
#define _WIN32_WINNT 0x0500
#include <windows.h>
#endif

#define ERROR_SETVALUE_NOT_SUPPORTED 1

#define QTRY_COMPARE(a,e)                       \
    for (int _i = 0; _i < 10000; _i += 100) {    \
        if ((a) == (e)) break;                  \
        QTest::qWait(100);                      \
    }                                           \
    QCOMPARE(a, e)

#define QTRY_VERIFY(a)                       \
    for (int _i = 0; _i < 10000; _i += 100) {    \
        if (a) break;                  \
        QTest::qWait(100);                      \
    }                                           \
    QVERIFY(a)

class ChangeListener : public QObject
{
    Q_OBJECT

Q_SIGNALS:
    void baseChanged();
    void attributeInterestChanged(const QString&, bool);
};

Q_DECLARE_METATYPE(QValueSpaceSubscriber*)
Q_DECLARE_METATYPE(QAbstractValueSpaceLayer*)
Q_DECLARE_METATYPE(QVariant)
Q_DECLARE_METATYPE(QValueSpace::LayerOptions)
Q_DECLARE_METATYPE(tst_QValueSpaceSubscriber::Type)
Q_DECLARE_METATYPE(QUuid)

void tst_QValueSpaceSubscriber::initTestCase()
{
    qRegisterMetaType<QVariant>("QVariant");
    qRegisterMetaType<tst_QValueSpaceSubscriber::Type>("tst_QValueSpaceSubscriber::Type");
    qRegisterMetaType<QValueSpace::LayerOptions>("QValueSpace::LayerOptions");

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

#if defined(Q_OS_UNIX) && defined(QT_START_VALUESPACE)
    QFile::remove("/tmp/qt-0/valuespace_shmlayer");
#endif

#if defined(QT_START_VALUESPACE)
    QValueSpace::initValueSpaceServer();
#endif

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();
    for (int i = 0; i < layers.count(); ++i) {
        QValueSpaceProvider *root = new QValueSpaceProvider("/", layers.at(i)->id());
        root->setAttribute("/home/user/bool", true);
        root->setAttribute("/home/user/int", 3);
        root->setAttribute("/home/user/QString", QString("testString"));
        QStringList stringList;
        stringList << QString("String 1") << QString("String 2");
        root->setAttribute("/home/user/QStringList", stringList);
        root->setAttribute("/home/user/qint64", qint64(64));
        root->setAttribute("/home/user/QByteArray", QByteArray("testByteArray"));
        root->setAttribute("/home/user/double", 4.56);
        root->setAttribute("/home/user/float", (float)4.56f);
        root->setAttribute("/home/user/QChar", QChar('c'));
        //so far not a lot of data types are supported
        //root->setAttribute("/home/user/QRect", QRect(0,0,5,6));

        root->setAttribute("/home/usercount", 1);

        root->setAttribute("/layer/name", layers.at(i)->name());
        root->setAttribute("/layer/id", layers.at(i)->id().toString());
        root->setAttribute("/layer/options", uint(layers.at(i)->layerOptions()));

        root->sync();

        roots.insert(layers.at(i), root);

        QValueSpaceProvider *busy = new QValueSpaceProvider("/usr", layers.at(i)->id());
        busy->setAttribute("alex/busy", true);
        busy->setAttribute("lorn/busy", false);
        busy->sync();

        busys.insert(layers.at(i), busy);
    }
}

void tst_QValueSpaceSubscriber::cleanupTestCase()
{
    foreach (QAbstractValueSpaceLayer *layer, roots.keys()) {
        QValueSpaceProvider *root = roots.take(layer);

        if (layer->layerOptions() & QValueSpace::PermanentLayer) {
            root->removeAttribute("/home/user/bool");
            root->removeAttribute("/home/user/int");
            root->removeAttribute("/home/user/QString");
            root->removeAttribute("/home/user/QStringList");
            root->removeAttribute("/home/user/qint64");
            root->removeAttribute("/home/user/QByteArray");
            root->removeAttribute("/home/user/double");
            root->removeAttribute("/home/user/float");
            root->removeAttribute("/home/user/QChar");
            root->removeAttribute("/home/user");
            root->removeAttribute("/home/usercount");
            root->removeAttribute("/home");
            root->removeAttribute("/layer/name");
            root->removeAttribute("/layer/id");
            root->removeAttribute("/layer/options");
            root->removeAttribute("/layer");
        }

        delete root;
    }

    foreach (QAbstractValueSpaceLayer *layer, busys.keys()) {
        QValueSpaceProvider *busy = busys.take(layer);

        if (layer->layerOptions() & QValueSpace::PermanentLayer) {
            busy->removeAttribute("alex/busy");
            busy->removeAttribute("alex");
            busy->removeAttribute("lorn/busy");
            busy->removeAttribute("lorn");
            busy->removeAttribute(QString());
        }

        delete busy;
    }
}

void tst_QValueSpaceSubscriber::dataVersatility_data()
{
    QTest::addColumn<QAbstractValueSpaceLayer *>("layer");

    QTest::addColumn< QVariant >("data");
    QTest::addColumn< QString >("typeString");
    QTest::addColumn< int >("typeIdent");

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();
    for (int i = 0; i < layers.count(); ++i) {
        QAbstractValueSpaceLayer *layer = layers.at(i);

        //these types have custom loading/saving operator
        QTest::newRow("Int") << layer
            << QVariant((int)567) << "Int" << (int)QVariant::Int;
        QTest::newRow("Bool") << layer
            << QVariant((bool)true) << "Bool" << (int)QVariant::Bool;
        QTest::newRow("UInt") << layer
            << QVariant((unsigned int)4) << "UInt" << (int)QVariant::UInt;
        QTest::newRow("LongLong") << layer
            << QVariant((long long)5) << "LongLong" << (int)QVariant::LongLong;
        QTest::newRow("ULongLong") << layer
            << QVariant((unsigned long long)6) << "ULongLong" << (int)QVariant::ULongLong;
        QTest::newRow("Double") << layer
            << QVariant((double)4.5) << "Double" << (int)QVariant::Double;
        QTest::newRow("QChar") << layer
            << QVariant(QChar('@')) << "Char" << (int)QVariant::Char;
        QTest::newRow("QString") << layer
            << QVariant(QString("asd")) << "QString" << (int)QVariant::String;
        QTest::newRow("QByteArray") << layer
            << QVariant(QByteArray("bytearray")) << "QByteArray" << (int)QVariant::ByteArray;
        QTest::newRow("QStringList") << layer
            << QVariant(QStringList() << QString("String 1") << QString("String 2"))
            << "QStringList" << (int)QVariant::StringList;

        //other types not specifically covered by valuespace -> uses QVariant based serialization
        QTest::newRow("QRect") << layer
            << QVariant(QRect(4,5,6,7)) << "QRect" << (int)QVariant::Rect;
        QTest::newRow("QDateTime") << layer
            << QVariant(QDateTime::currentDateTime()) << "QDateTime" << (int)QVariant::DateTime;
    }
}

void tst_QValueSpaceSubscriber::dataVersatility()
{
    QFETCH(QAbstractValueSpaceLayer *, layer);

    QFETCH(QVariant, data);
    QFETCH(QString, typeString);
    QFETCH(int, typeIdent);

    QCOMPARE(data.type(), (QVariant::Type)typeIdent);

    QValueSpaceProvider provider("/usr/data", layer->id());
    provider.setAttribute(typeString, data);
    provider.sync();
    QValueSpaceSubscriber subscriber("/usr/data", layer->id());
    QVariant v = subscriber.value(typeString);

    QCOMPARE(v.type(), (QVariant::Type)typeIdent);
    QCOMPARE(v, data);

    if (layer->layerOptions() & QValueSpace::PermanentLayer)
        provider.removeAttribute(typeString);
}

void tst_QValueSpaceSubscriber::testConstructor_data()
{
    QTest::addColumn< QVariant >("testItem");
    QTest::addColumn< QVariant >("value");
    QTest::addColumn< QStringList >("subPaths");
    QTest::addColumn< QString >("path");
    QTest::addColumn< QString >("relItemPath");
    QTest::addColumn< int >("expectedValue");

    QStringList allPaths;
    allPaths << "bool" << "int" << "QString" << "QStringList" << "qint64" << "QByteArray"
             << "double" << "float" << "QChar";

    QStringList rootPaths;
    rootPaths << "home" << "usr" << "layer";

    QStringList homePaths;
    homePaths << "user" << "usercount";

    QVariant data;
    QValueSpaceSubscriber *subscriber;

    // Subscribers with / path.
    subscriber = new QValueSpaceSubscriber(QString(), this);
    qVariantSetValue(data, subscriber);
    QTest::newRow("QValueSpaceSubscriber(QString(), this)")
        << data
        << QVariant()
        << rootPaths
        << QString("/")
        << QString("home/user/int")
        << 3;

    subscriber = new QValueSpaceSubscriber(QString("/"), this);
    qVariantSetValue(data, subscriber);
    QTest::newRow("QValueSpaceSubscriber(QString(\"/\"), this)")
        << data
        << QVariant()
        << rootPaths
        << QString("/")
        << QString("home/user/int")
        << 3;

    subscriber = new QValueSpaceSubscriber("", this);
    qVariantSetValue(data, subscriber);
    QTest::newRow("QValueSpaceSubscriber(\"\", this)")
        << data
        << QVariant()
        << rootPaths
        << QString("/")
        << QString("home/user/int")
        << 3;

    subscriber = new QValueSpaceSubscriber("/", this);
    qVariantSetValue(data, subscriber);
    QTest::newRow("QValueSpaceSubscriber(\"/\", this)")
        << data
        << QVariant()
        << rootPaths
        << QString("/")
        << QString("home/user/int")
        << 3;

    subscriber = new QValueSpaceSubscriber(this);
    qVariantSetValue(data, subscriber);
    QTest::newRow("QValueSpaceSubscriber(this)")
        << data
        << QVariant()
        << rootPaths
        << QString("/")
        << QString("home/user/int")
        << 3;

    // Subscribers with /home path.
    subscriber = new QValueSpaceSubscriber(QString("/home"), this);
    qVariantSetValue(data, subscriber);
    QTest::newRow("QValueSpaceSubscriber(QString(\"/home\"), this)")
        << data
        << QVariant()
        << homePaths
        << QString("/home")
        << QString("user/int")
        << 3;

    // Subscribers with /home/user path.
    subscriber = new QValueSpaceSubscriber(QString("/home/user"), this);
    qVariantSetValue(data, subscriber);
    QTest::newRow("QValueSpaceSubscriber(QString(\"/home/user\"), this)")
        << data
        << QVariant()
        << allPaths
        << QString("/home/user")
        << QString("int")
        << 3;

    // Direct value subscriber with /home/user/int path.
    subscriber = new QValueSpaceSubscriber("/home/user/int", this);
    qVariantSetValue(data, subscriber);
    QTest::newRow("QValueSpaceSubscriber(\"/home/user/int\", this)")
        << data
        << QVariant(3)
        << QStringList()
        << QString("/home/user/int")
        << QString()
        << 3;

    // Subscriber with invalid path.
    subscriber = new QValueSpaceSubscriber(QString("/home/invalidPath"), this);
    qVariantSetValue(data, subscriber);
    QTest::newRow("QValueSpaceSubscriber(QString(\"/home/invalidPath\"), this)")
        << data
        << QVariant()
        << QStringList()
        << QString("/home/invalidPath")
        << QString("user/int")
        << 100; //should fails -> returns default

    // setPath to /
    subscriber = new QValueSpaceSubscriber(this);
    subscriber->setPath("/");
    qVariantSetValue(data, subscriber);
    QTest::newRow("QValueSpaceSubscriber::setPath(\"/\"")
        << data
        << QVariant()
        << rootPaths
        << QString("/")
        << QString("home/user/int")
        << 3;

    // setPath to /home
    subscriber = new QValueSpaceSubscriber(this);
    subscriber->setPath("/home");
    qVariantSetValue(data, subscriber);
    QTest::newRow("QValueSpaceSubscriber::setPath(\"/home\")")
        << data
        << QVariant()
        << homePaths
        << QString("/home")
        << QString("user/int")
        << 3;

    // setPath to /home/user
    subscriber = new QValueSpaceSubscriber(this);
    subscriber->setPath("/home/user");
    qVariantSetValue(data, subscriber);
    QTest::newRow("QValueSpaceSubscriber::setPath(\"/home/user\")")
        << data
        << QVariant()
        << allPaths
        << QString("/home/user")
        << QString("int")
        << 3;

    // setPath to &QValueSpaceSubscriber
    subscriber = new QValueSpaceSubscriber(this);
    {
        QValueSpaceSubscriber user("/home/user");
        subscriber->setPath(&user);
    }
    qVariantSetValue(data, subscriber);
    QTest::newRow("QValueSpaceSubscriber::setPath(&QValueSpaceSubscriber(\"/home/user\")")
        << data
        << QVariant()
        << allPaths
        << QString("/home/user")
        << QString("int")
        << 3;
}

void tst_QValueSpaceSubscriber::testConstructor()
{
    QFETCH(QVariant, testItem);
    QFETCH(QVariant, value);
    QFETCH(QStringList, subPaths);
    QFETCH(QString, path);
    QFETCH(QString, relItemPath);
    QFETCH(int, expectedValue);

    QValueSpaceSubscriber *subscriber = qvariant_cast<QValueSpaceSubscriber*>(testItem);
    QCOMPARE(subscriber->parent(), (QObject*)this);
    QCOMPARE(subscriber->value(), value);
    QCOMPARE(subscriber->subPaths().toSet(), subPaths.toSet());
    QCOMPARE(subscriber->path(), path);
    QCOMPARE(subscriber->value(relItemPath, 100).toInt(), expectedValue);
}

#define ADD(opt, invalid) do {\
    QTest::newRow(QString::number(opt).append(" const char *").toLocal8Bit().constData()) \
        << (QValueSpace::UnspecifiedLayer | opt) << CharStar << invalid; \
    QTest::newRow(QString::number(opt).append(" const QString &").toLocal8Bit().constData()) \
        << (QValueSpace::UnspecifiedLayer | opt) << String << invalid; \
} while (false)

void tst_QValueSpaceSubscriber::testFilterConstructor_data()
{
    QTest::addColumn<QValueSpace::LayerOptions>("options");
    QTest::addColumn<Type>("type");
    QTest::addColumn<bool>("connected");

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();

    for (int i = 0; i < layers.count(); ++i) {
        QAbstractValueSpaceLayer *layer = layers.at(i);

        ADD(layer->layerOptions(), true);
    }

    ADD(QValueSpace::UnspecifiedLayer, true);
    ADD(QValueSpace::PermanentLayer, true);
    ADD(QValueSpace::NonPermanentLayer, true);
    ADD(QValueSpace::PermanentLayer | QValueSpace::NonPermanentLayer, false);
    ADD(QValueSpace::WriteableLayer, true);
    ADD(QValueSpace::NonWriteableLayer, true);
    ADD(QValueSpace::WriteableLayer | QValueSpace::NonWriteableLayer, false);
}

void tst_QValueSpaceSubscriber::testFilterConstructor()
{
    QFETCH(QValueSpace::LayerOptions, options);
    QFETCH(Type, type);
    QFETCH(bool, connected);

    QValueSpaceSubscriber *subscriber;

    switch (type) {
    case CharStar:
        subscriber = new QValueSpaceSubscriber("/layer", options);
        break;
    case String:
        subscriber = new QValueSpaceSubscriber(QString("/layer"), options);
        break;
    default:
        QFAIL("Unexpected type");
        return;
    };

    if (!connected)
        QVERIFY(!subscriber->isConnected());

    if (subscriber->isConnected()) {
        QValueSpace::LayerOptions actualOptions =
            QValueSpace::LayerOptions(subscriber->value("options", 0).toUInt());

        QVERIFY(options == QValueSpace::UnspecifiedLayer || actualOptions & options);
    }
}

void tst_QValueSpaceSubscriber::testPathChanges()
{
    QValueSpaceSubscriber subscriber;

    QStringList rootPaths;
    rootPaths << "layer" << "usr" << "home";

    QStringList homePaths;
    homePaths << "user" << "usercount";

    QStringList homeUserPaths;
    homeUserPaths << "bool" << "int" << "QString" << "QStringList" << "qint64" << "QByteArray"
                  << "double" << "float" << "QChar";

    QCOMPARE(subscriber.path(), QLatin1String("/"));
    QCOMPARE(subscriber.subPaths().toSet(), rootPaths.toSet());

    subscriber.cd("home");
    QCOMPARE(subscriber.path(), QLatin1String("/home"));
    QCOMPARE(subscriber.subPaths().toSet(), homePaths.toSet());

    subscriber.cd("user");
    QCOMPARE(subscriber.path(), QLatin1String("/home/user"));
    QCOMPARE(subscriber.subPaths().toSet(), homeUserPaths.toSet());

    subscriber.cdUp();
    QCOMPARE(subscriber.path(), QLatin1String("/home"));
    QCOMPARE(subscriber.subPaths().toSet(), homePaths.toSet());

    subscriber.cd("/home/user");
    QCOMPARE(subscriber.path(), QLatin1String("/home/user"));
    QCOMPARE(subscriber.subPaths().toSet(), homeUserPaths.toSet());
}

void tst_QValueSpaceSubscriber::contentsChanged_data()
{
    QTest::addColumn<QAbstractValueSpaceLayer *>("layer");

    QTest::addColumn<bool>("implicit");
    QTest::addColumn< QString >("subscriber_path");
    QTest::addColumn< QString >("value_path");
    QTest::addColumn< int >("should_emit_signal");
    QTest::addColumn< bool >("old_value");
    QTest::addColumn< bool >("new_value");

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();
    for (int i = 0; i < layers.count(); ++i) {
        QAbstractValueSpaceLayer *layer = layers.at(i);

        QTest::newRow(layer->name().append(": implicit (empty)").toLocal8Bit().constData())
            << layer
            << true
            << ""
            << "usr/alex/busy"
            << 1
            << false
            << true;

        QTest::newRow(layer->name().append(": explicit (empty)").toLocal8Bit().constData())
            << layer
            << false
            << ""
            << "usr/alex/busy"
            << 1
            << false
            << true;

        QTest::newRow(layer->name().append(": implicit /").toLocal8Bit().constData())
            << layer
            << true
            << "/"
            << "usr/alex/busy"
            << 1
            << false
            << true;

        QTest::newRow(layer->name().append(": explicit /").toLocal8Bit().constData())
            << layer
            << false
            << "/"
            << "usr/alex/busy"
            << 1
            << false
            << true;

        QTest::newRow(layer->name().append(": implicit /usr").toLocal8Bit().constData())
            << layer
            << true
            << "/usr"
            << "alex/busy"
            << 1
            << false
            << true;

        QTest::newRow(layer->name().append(": explicit /usr").toLocal8Bit().constData())
            << layer
            << false
            << "/usr"
            << "alex/busy"
            << 1
            << false
            << true;

        QTest::newRow(layer->name().append(": implicit /usr/alex").toLocal8Bit().constData())
            << layer
            << true
            << "/usr/alex"
            << "busy"
            << 1
            << false
            << true;

        QTest::newRow(layer->name().append(": explicit /usr/alex").toLocal8Bit().constData())
            << layer
            << false
            << "/usr/alex"
            << "busy"
            << 1
            << false
            << true;

        QTest::newRow(layer->name().append(": implicit /usr/alex/busy").toLocal8Bit().constData())
            << layer
            << true
            << "/usr/alex/busy"
            << ""
            << 1
            << false
            << true;

        QTest::newRow(layer->name().append(": explicit /usr/alex/busy").toLocal8Bit().constData())
            << layer
            << false
            << "/usr/alex/busy"
            << ""
            << 1
            << false
            << true;

        QTest::newRow(layer->name().append(": implicit /usr/lorn").toLocal8Bit().constData())
            << layer
            << true
            << "/usr/lorn"
            << "busy"
            << 0
            << false
            << false;

        QTest::newRow(layer->name().append(": explicit /usr/lorn").toLocal8Bit().constData())
            << layer
            << false
            << "/usr/lorn"
            << "busy"
            << 0
            << false
            << false;
    }
}

void tst_QValueSpaceSubscriber::contentsChanged()
{
    QFETCH(QAbstractValueSpaceLayer *, layer);

    QFETCH(bool, implicit);
    QFETCH(QString, subscriber_path);
    QFETCH(QString, value_path);
    QFETCH(int, should_emit_signal);
    QFETCH(bool, old_value);
    QFETCH(bool, new_value);

    QValueSpaceProvider *busy = busys.value(layer);

    busy->setAttribute("alex/busy", old_value);
    busy->sync();

    QValueSpaceSubscriber subscriber(subscriber_path, layer->id());
    QCOMPARE(subscriber.value(value_path,!old_value).toBool(), old_value);

    ChangeListener* listener = 0;
    QSignalSpy *spy;
    if (implicit) {
        listener = new ChangeListener;
        spy = new QSignalSpy(listener, SIGNAL(baseChanged()));
        connect(&subscriber, SIGNAL(contentsChanged()), listener, SIGNAL(baseChanged()));
    } else {
        spy = new QSignalSpy(&subscriber, SIGNAL(contentsChanged()));
        subscriber.property("value");
    }

    QCOMPARE(spy->count(), 0);

    busy->setAttribute("alex/busy", new_value);
    busy->sync();

    QTRY_COMPARE(spy->count(), should_emit_signal);
    QCOMPARE(subscriber.value(value_path,!old_value).toBool(), new_value);

    spy->clear();

    //removing the value triggers signal
    busy->removeAttribute("alex/busy");
    busy->sync();
    QTRY_COMPARE(spy->count(), should_emit_signal);
    QCOMPARE(subscriber.value(value_path,!old_value).toBool(), new_value);

    spy->clear();
    busy->setAttribute("alex/busy", new_value);
    busy->sync();

    QTRY_COMPARE(spy->count(), should_emit_signal);
    QCOMPARE(subscriber.value(value_path,!old_value).toBool(), new_value);

    disconnect(&subscriber, SIGNAL(contentsChanged()), listener, SIGNAL(baseChanged()));

    delete spy;
    delete listener;
}

void tst_QValueSpaceSubscriber::value_data()
{
    QTest::addColumn<QAbstractValueSpaceLayer *>("layer");

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();
    for (int i = 0; i < layers.count(); ++i)
        QTest::newRow(layers.at(i)->name().toLocal8Bit().constData()) << layers.at(i);
}

void tst_QValueSpaceSubscriber::value()
{
    QFETCH(QAbstractValueSpaceLayer *, layer);

    QValueSpaceSubscriber base(QString("/"), layer->id());
    QCOMPARE(base.value("home/usercount", 5).toInt(), 1);
    QCOMPARE(base.value("home/user/QString", "default").toString(), QString("testString"));
    QCOMPARE(base.value("home/user/bool", false).toBool(), true);
    QCOMPARE(base.value("home/user/int", 5).toInt(), 3);
    QCOMPARE(base.value("home/user/QByteArray", QByteArray("invalid")).toByteArray(),
             QByteArray("testByteArray"));
    QCOMPARE(base.value("home/user/double", 4.0).toDouble(), double(4.56));
    //QCOMPARE(base.value("home/user/float", 4.0).toDouble(), (double)4.56);

    QValueSpaceSubscriber base1(QString("/home"), layer->id());
    QCOMPARE(base1.value(QString("usercount"), 5).toInt(), 1);
    QCOMPARE(base1.value(QString("user/QString"), "default").toString(), QString("testString"));
    QCOMPARE(base1.value("user/bool", false).toBool(), true);
    QCOMPARE(base1.value("user/int", 5).toInt(), 3);
    QCOMPARE(base1.value("user/QByteArray", QByteArray("invalid")).toByteArray(),
             QByteArray("testByteArray"));
    QCOMPARE(base1.value("user/double", 4.0).toDouble(), double(4.56));
    //QCOMPARE(base1.value("user/float", 4.0).toDouble(), double(4.56));

    QValueSpaceSubscriber base2(QString("/home/user"), layer->id());
    QCOMPARE(base2.value(QString("usercount"), 5).toInt(), 5);
    QCOMPARE(base2.value(QString("QString"), "default").toString(), QString("testString"));
    QCOMPARE(base2.value("bool", false).toBool(), true);
    QCOMPARE(base2.value("int", 5).toInt(), 3);
    QCOMPARE(base2.value("QByteArray", QByteArray("invalid")).toByteArray(),
             QByteArray("testByteArray"));
    QCOMPARE(base2.value("double", 4.0).toDouble(), double(4.56));
    //QCOMPARE(base2.value("float", 4.0).toDouble(), 4.56);
}

void tst_QValueSpaceSubscriber::ipcTests_data()
{
#ifdef QT_NO_PROCESS
    QSKIP("Qt was compiled with QT_NO_PROCESS", SkipAll);
#else
    QTest::addColumn<QAbstractValueSpaceLayer *>("layer");

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();
    for (int i = 0; i < layers.count(); ++i)
        QTest::newRow(layers.at(i)->name().toLocal8Bit().constData()) << layers.at(i);
#endif
}

void tst_QValueSpaceSubscriber::ipcTests()
{
#if defined(QT_NO_PROCESS)
    QSKIP("Qt was compiled with QT_NO_PROCESS", SkipAll);
#else
    QFETCH(QAbstractValueSpaceLayer *, layer);

    QValueSpaceSubscriber subscriber("/usr/lackey/subdir/value", layer->id());
    ChangeListener listener;
    QSignalSpy spy(&listener, SIGNAL(baseChanged()));
    connect(&subscriber, SIGNAL(contentsChanged()), &listener, SIGNAL(baseChanged()));

    QProcess process;
    process.setProcessChannelMode(QProcess::ForwardedChannels);
    process.start("vsiTestLackey", QStringList() << "-ipcTests" << layer->id().toString());
    QVERIFY(process.waitForStarted());

    //lackey sets value to 100 as part of its startup
    QTRY_COMPARE(spy.count(), 1);
    QCOMPARE(subscriber.value("", 99).toInt(), 100);
    spy.clear();

    //lackey sets value to 101
    QTRY_COMPARE(spy.count(), 1);
    QCOMPARE(subscriber.value("", 99).toInt(), 101);
    spy.clear();

    //value was removed -> returns default
    QTRY_COMPARE(spy.count(), 1);
    QCOMPARE(subscriber.value("", 99).toInt(), 99);
    spy.clear();

    //lackey sets value to 102
    QTRY_COMPARE(spy.count(), 1);
    QCOMPARE(subscriber.value("", 99).toInt(), 102);
    spy.clear();

    //value was removed -> returns default
    QTRY_COMPARE(spy.count(), 1);
    QCOMPARE(subscriber.value("", 99).toInt(), 99);

    QVERIFY(process.waitForFinished(10000));
#endif
}

void tst_QValueSpaceSubscriber::ipcRemoveKey_data()
{
#ifdef QT_NO_PROCESS
    QSKIP("Qt was compiled with QT_NO_PROCESS", SkipAll);
#else
    QTest::addColumn<QAbstractValueSpaceLayer *>("layer");

    bool skip = true;

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();
    for (int i = 0; i < layers.count(); ++i) {
        QAbstractValueSpaceLayer *layer = layers.at(i);

        if (layer->layerOptions() & QValueSpace::PermanentLayer)
            continue;

        skip = false;

        QTest::newRow(layer->name().toLocal8Bit().constData()) << layer;
    }

    if (skip)
        QSKIP("No applicable layers found.", SkipAll);
#endif
}

void tst_QValueSpaceSubscriber::ipcRemoveKey()
{
#if defined(QT_NO_PROCESS)
    QSKIP("Qt was compiled with QT_NO_PROCESS", SkipAll);
#else
    QFETCH(QAbstractValueSpaceLayer *, layer);

    QValueSpaceSubscriber subscriber("/ipcRemoveKey", layer->id());

    ChangeListener listener;
    QSignalSpy changeSpy(&listener, SIGNAL(baseChanged()));
    QObject::connect(&subscriber, SIGNAL(contentsChanged()), &listener, SIGNAL(baseChanged()));

    QProcess process;
    process.setProcessChannelMode(QProcess::ForwardedChannels);
    process.start("vsiTestLackey", QStringList() << "-ipcRemoveKey" << layer->id().toString());
    QVERIFY(process.waitForStarted());

    // Wait for lackey to create "value".
    QTRY_COMPARE(changeSpy.count(), 1);
    QCOMPARE(subscriber.value("value", 5).toInt(), 100);

    // Wait for lackey to delete key "/ipcRemoveKey".
    changeSpy.clear();
    QTRY_COMPARE(changeSpy.count(), 1);

    QVERIFY(subscriber.subPaths().isEmpty());
    QCOMPARE(subscriber.value("value", 6).toInt(), 6);
    QVERIFY(process.waitForFinished(10000));
#endif
}

void tst_QValueSpaceSubscriber::interestNotification_data()
{
    QTest::addColumn<QAbstractValueSpaceLayer *>("layer");

    QTest::addColumn<Type>("type");
    QTest::addColumn<QString>("providerPath");
    QTest::addColumn<QString>("attribute");

    bool foundSupported = false;

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();
    for (int i = 0; i < layers.count(); ++i) {
        QAbstractValueSpaceLayer *layer = layers.at(i);

        if (!layer->supportsInterestNotification())
            continue;

        foundSupported = true;

        QTest::newRow("QValueSpaceSubscriber(char *)")
            << layer << CharStar << "/interestNotification" << "/value";
        QTest::newRow("QValueSpaceSubscriber(QString)")
            << layer << String << "/interestNotification" << "/value";
    }

    if (!foundSupported)
        QSKIP("No layer supporting interest notifications found.", SkipAll);
}

void tst_QValueSpaceSubscriber::interestNotification()
{
    QFETCH(QAbstractValueSpaceLayer *, layer);

    QFETCH(Type, type);
    QFETCH(QString, providerPath);
    QFETCH(QString, attribute);

    QValueSpaceProvider *provider;
    provider = new QValueSpaceProvider(providerPath, layer->id());

    ChangeListener notificationListener;
    connect(provider, SIGNAL(attributeInterestChanged(QString,bool)),
            &notificationListener, SIGNAL(attributeInterestChanged(QString,bool)));

    QSignalSpy notificationSpy(&notificationListener,
                               SIGNAL(attributeInterestChanged(QString,bool)));

    const QString subscriberPath = providerPath + attribute;

    QValueSpaceSubscriber *subscriber;
    switch (type) {
    case CharStar:
        subscriber = new QValueSpaceSubscriber(subscriberPath.toUtf8().constData(), layer->id());
        break;
    case String:
        subscriber = new QValueSpaceSubscriber(subscriberPath, layer->id());
        break;
    default:
        subscriber = 0;
        QFAIL("Invalid type");
    }

    QTRY_COMPARE(notificationSpy.count(), 1);

    QList<QVariant> arguments = notificationSpy.takeFirst();
    QCOMPARE(arguments.at(0).type(), QVariant::String);
    QCOMPARE(arguments.at(0).toString(), attribute);
    QCOMPARE(arguments.at(1).type(), QVariant::Bool);
    QCOMPARE(arguments.at(1).toBool(), true);

    QCOMPARE(subscriber->value(QString(), 10).toInt(), 10);

    provider->setAttribute(attribute, 5);
    provider->sync();

    QCOMPARE(subscriber->value(QString(), 10).toInt(), 5);

    notificationSpy.clear();

    delete subscriber;

    QTRY_COMPARE(notificationSpy.count(), 1);

    arguments = notificationSpy.takeFirst();
    QCOMPARE(arguments.at(0).type(), QVariant::String);
    QCOMPARE(arguments.at(0).toString(), attribute);
    QCOMPARE(arguments.at(1).type(), QVariant::Bool);
    QCOMPARE(arguments.at(1).toBool(), false);

    delete provider;
}

void tst_QValueSpaceSubscriber::ipcInterestNotification_data()
{
#ifdef QT_NO_PROCESS
    QSKIP("Qt was compiled with QT_NO_PROCESS", SkipAll);
#else
    QTest::addColumn<QAbstractValueSpaceLayer *>("layer");

    bool foundSupported = false;

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();
    for (int i = 0; i < layers.count(); ++i) {
        QAbstractValueSpaceLayer *layer = layers.at(i);

        if (!layer->supportsInterestNotification())
            continue;

        foundSupported = true;

        QTest::newRow(layer->name().toLocal8Bit().constData()) << layer;
    }

    if (!foundSupported)
        QSKIP("No layer supporting interest notifications found.", SkipAll);
#endif
}

void tst_QValueSpaceSubscriber::ipcInterestNotification()
{
#if defined(QT_NO_PROCESS)
    QSKIP("Qt was compiled with QT_NO_PROCESS", SkipAll);
#else
    QFETCH(QAbstractValueSpaceLayer *, layer);

    // Test QValueSpaceSubscriber construction before QValueSpaceProvider.

    QValueSpaceSubscriber *subscriber =
        new QValueSpaceSubscriber("/ipcInterestNotification/value", layer->id());

    ChangeListener listener;
    QObject::connect(subscriber, SIGNAL(contentsChanged()), &listener, SIGNAL(baseChanged()));
    QSignalSpy changeSpy(&listener, SIGNAL(baseChanged()));

    // Lackey is not running, so value will not exist.
    QCOMPARE(subscriber->value(QString(), 10).toInt(), 10);

    QProcess process;
    process.setProcessChannelMode(QProcess::ForwardedChannels);
    process.start("vsiTestLackey", QStringList()
        << "-ipcInterestNotification" << layer->id().toString());
    QVERIFY(process.waitForStarted());

    // Lackey will receive attributeInterestChanged from server and set the attribute.
    QTRY_COMPARE(changeSpy.count(), 1);
    changeSpy.clear();

    QCOMPARE(subscriber->value(QString(), 10).toInt(), 5);

    // Lackey will receive attributeInterestChanged and remove attribute.
    delete subscriber;
    QTest::qWait(1000);

    // Test QValueSpaceSubscriber construction after QValueSpaceProvider

    subscriber = new QValueSpaceSubscriber("/ipcInterestNotification/value", layer->id());
    QObject::connect(subscriber, SIGNAL(contentsChanged()), &listener, SIGNAL(baseChanged()));

    QTRY_COMPARE(changeSpy.count(), 1);

    QCOMPARE(subscriber->value(QString(), 10).toInt(), 5);
#endif
}

void tst_QValueSpaceSubscriber::clientServer()
{
#if defined(QT_START_VALUESPACE)
    QVERIFY(QValueSpaceManager::instance()->isServer());
#else
    QVERIFY(!QValueSpaceManager::instance()->isServer());
#endif
}

class WriteThread : public QThread
{
    Q_OBJECT

public:
    WriteThread();

    void setDone();

protected:
    void run();

private:
    QValueSpaceProvider *provider;
    bool done;
};

WriteThread::WriteThread()
:   provider(0), done(false)
{
}

void WriteThread::setDone()
{
    done = true;
}

void WriteThread::run()
{
    QTest::qWait(100);  // give some ReadThreads some time to start.

    QValueSpaceProvider provider("/threads");

    uint value = 0;
    while (!done) {
        provider.setAttribute("value", value);
        provider.sync();
        QTest::qWait(100);
        value += 100;
    }
}

class ReadThread : public QThread
{
    Q_OBJECT

public:
    ReadThread(QValueSpaceSubscriber *subscriber, bool sync);

protected:
    void run();

private:
    QValueSpaceSubscriber *masterSubscriber;
    int iterations;
    bool synchronised;
};

ReadThread::ReadThread(QValueSpaceSubscriber *subscriber, bool sync)
:   masterSubscriber(subscriber), iterations(0), synchronised(sync)
{
}

void ReadThread::run()
{
    while (true) {
        QValueSpaceSubscriber subscriber;
        subscriber.setPath(masterSubscriber);

        if (synchronised) {
            QEventLoop loop;
            connect(&subscriber, SIGNAL(contentsChanged()), &loop, SLOT(quit()));
            QTimer::singleShot(1000, &loop, SLOT(quit()));
            loop.exec();
        }

        ++iterations;

        QVariant value = subscriber.value();
        if (value.isValid() && value.toUInt() > 1000)
            break;
    }
}

void tst_QValueSpaceSubscriber::threads_data()
{
    QTest::addColumn<unsigned int>("threads");
    QTest::addColumn<bool>("synchronised");

    QTest::newRow("1 thread") << uint(1) << true;
    QTest::newRow("2 threads") << uint(2) << true;
#ifdef Q_OS_WINCE
    QTest::newRow("10 threads") << uint(10) << true;
#else
    QTest::newRow("100 threads") << uint(100) << true;
#endif
    QTest::newRow("1 thread, unsynchronised") << uint(1) << false;
    QTest::newRow("2 threads, unsynchronised") << uint(2) << false;
#ifdef Q_OS_WINCE
    QTest::newRow("10 threads") << uint(10) << false;
#else
    QTest::newRow("100 threads, unsynchronised") << uint(100) << false;
#endif
}

void tst_QValueSpaceSubscriber::threads()
{
    QFETCH(unsigned int, threads);
    QFETCH(bool, synchronised);

    QEventLoop writeLoop;
    WriteThread *writeThread = new WriteThread;
    connect(writeThread, SIGNAL(finished()), &writeLoop, SLOT(quit()));
    writeThread->start();

    QValueSpaceSubscriber masterSubscriber("/threads/value");

    QVector<ReadThread *> readThreads(threads);

    for (unsigned int i = 0; i < threads; ++i) {
        readThreads[i] = new ReadThread(&masterSubscriber, synchronised);
        readThreads[i]->start();
    }

    for (unsigned int i = 0; i < threads; ++i) {
        QEventLoop loop;
        connect(readThreads[i], SIGNAL(finished()), &loop, SLOT(quit()));
        if (!readThreads[i]->isFinished())
            loop.exec();

        delete readThreads[i];
    }

    writeThread->setDone();

    if (!writeThread->isFinished())
        writeLoop.exec();

    delete writeThread;
}

#include "tst_qvaluespacesubscribershared.moc"
