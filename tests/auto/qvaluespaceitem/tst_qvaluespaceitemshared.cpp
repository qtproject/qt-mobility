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

#include "tst_qvaluespaceitemshared.h"

#include <qvaluespaceitem.h>
#include <qvaluespaceobject.h>
#include <qvaluespacemanager_p.h>

#include <QTest>
#include <QSet>
#include <QSignalSpy>
#include <QProcess>

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
    void copyChanged();
    void changeValue(const QByteArray&, const QVariant&);
    void itemRemove(const QByteArray&);
    void itemNotify(const QByteArray&, bool);
};

Q_DECLARE_METATYPE(QValueSpaceItem*)
Q_DECLARE_METATYPE(QAbstractValueSpaceLayer*)
Q_DECLARE_METATYPE(QVariant)
Q_DECLARE_METATYPE(QAbstractValueSpaceLayer::LayerOptions)
Q_DECLARE_METATYPE(tst_QValueSpaceItem::Type)
Q_DECLARE_METATYPE(QUuid)

void tst_QValueSpaceItem::initTestCase()
{
    qRegisterMetaType<QVariant>("QVariant");
    qRegisterMetaType<tst_QValueSpaceItem::Type>("tst_QValueSpaceItem::Type");
    qRegisterMetaType<QAbstractValueSpaceLayer::LayerOptions>("QAbstractValueSpaceLayer::LayerOptions");

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

#if defined(QT_START_VALUESPACE)
    QValueSpace::initValueSpaceManager();
#endif

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();
    for (int i = 0; i < layers.count(); ++i) {
        QValueSpaceObject *root = new QValueSpaceObject("/", layers.at(i)->id());
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

        QValueSpaceObject *busy = new QValueSpaceObject("/usr", layers.at(i)->id());
        busy->setAttribute("alex/busy", true);
        busy->setAttribute("lorn/busy", false);
        busy->sync();

        busys.insert(layers.at(i), busy);
    }
}

void tst_QValueSpaceItem::cleanupTestCase()
{
    foreach (QAbstractValueSpaceLayer *layer, roots.keys()) {
        QValueSpaceObject *root = roots.take(layer);

        if (layer->layerOptions() & QAbstractValueSpaceLayer::PermanentLayer) {
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
        QValueSpaceObject *busy = busys.take(layer);

        if (layer->layerOptions() & QAbstractValueSpaceLayer::PermanentLayer) {
            busy->removeAttribute("alex/busy");
            busy->removeAttribute("alex");
            busy->removeAttribute("lorn/busy");
            busy->removeAttribute("lorn");
            busy->removeAttribute(QByteArray());
        }

        delete busy;
    }
}

void tst_QValueSpaceItem::dataVersatility_data()
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

void tst_QValueSpaceItem::dataVersatility()
{
    QFETCH(QAbstractValueSpaceLayer *, layer);

    QFETCH(QVariant, data);
    QFETCH(QString, typeString);
    QFETCH(int, typeIdent);

    QCOMPARE(data.type(), (QVariant::Type)typeIdent);

    QValueSpaceObject object("/usr/data", layer->id());
    object.setAttribute(typeString, data);
    object.sync();
    QValueSpaceItem item("/usr/data", layer->id());
    item.sync();
    QVariant v = item.value(typeString);

    QCOMPARE(v.type(), (QVariant::Type)typeIdent);
    QCOMPARE(v, data);

    if (layer->layerOptions() & QAbstractValueSpaceLayer::PermanentLayer)
        object.removeAttribute(typeString);
}

void tst_QValueSpaceItem::testConstructor_data()
{
    QTest::addColumn< QVariant >("testItem");
    QTest::addColumn< QVariant >("value");
    QTest::addColumn< QStringList >("subPaths");
    QTest::addColumn< QString >("path");
    QTest::addColumn< QString >("relItemPath");
    QTest::addColumn< int >("expectedValue");

    QStringList allPaths;
    allPaths << "bool" << "int" << "QString" << "QStringList" << "qint64" << "QByteArray" << "double" << "float" << "QChar";

    QStringList rootPaths;
    rootPaths << "home" << "usr" << "layer";

    QStringList homePaths;
    homePaths << "user" << "usercount";

    //valid items based on / path
    QValueSpaceItem* item1 = new QValueSpaceItem(QString(), this);
    QVariant data;
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(QString(), this)") 
        << data
        << QVariant()
        << rootPaths
        << QString("/")
        << QString("home/user/int")
        << 3;

    item1 = new QValueSpaceItem(QString("/"), this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(QString(\"/\"), this)") 
        << data
        << QVariant()
        << rootPaths
        << QString("/")
        << QString("home/user/int")
        << 3;

    item1 = new QValueSpaceItem("", this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(\"\", this)") 
        << data
        << QVariant()
        << rootPaths
        << QString("/")
        << QString("home/user/int")
        << 3;

    item1 = new QValueSpaceItem("/", this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(\"/\", this)") 
        << data
        << QVariant()
        << rootPaths
        << QString("/")
        << QString("home/user/int")
        << 3;

    item1 = new QValueSpaceItem(this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(this)") 
        << data
        << QVariant()
        << rootPaths
        << QString("/")
        << QString("home/user/int")
        << 3;

    //valid items based on /home path
    item1 = new QValueSpaceItem(QString("/home"), this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(QString(\"/home\"), this)") 
        << data
        << QVariant()
        << homePaths
        << QString("/home")
        << QString("user/int")
        << 3;

    //valid items based on /home/user path
    item1 = new QValueSpaceItem(QString("/home/user"), this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(QString(\"/home/user\"), this)") 
        << data
        << QVariant()
        << allPaths
        << QString("/home/user")
        << QString("int")
        << 3;

    // direct value item /home/user/int
    item1 = new QValueSpaceItem("/home/user/int", this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(\"/home/user/int\", this)")
        << data
        << QVariant(3)
        << QStringList()
        << QString("/home/user/int")
        << QString()
        << 3;

    //invalid path
    item1 = new QValueSpaceItem(QString("/home/invalidPath"), this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(QString(\"/home/invalidPath\"), this)")
        << data
        << QVariant()
        << QStringList()
        << QString("/home/invalidPath")
        << QString("user/int")
        << 100; //should fails -> returns default

    // setPath to /
    item1 = new QValueSpaceItem(this);
    item1->setPath(QByteArray("/"));
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem::setPath(QByteArray(\"/\")")
        << data
        << QVariant()
        << rootPaths
        << QString("/")
        << QString("home/user/int")
        << 3;

    // setPath to /home
    item1 = new QValueSpaceItem(this);
    item1->setPath(QByteArray("/home"));
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem::setPath(QByteArray(\"/home\"))")
        << data
        << QVariant()
        << homePaths
        << QString("/home")
        << QString("user/int")
        << 3;

    // setPath to /home/user
    item1 = new QValueSpaceItem(this);
    item1->setPath(QByteArray("/home/user"));
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem::setPath(QByteArray(\"/home/user\"))")
        << data
        << QVariant()
        << allPaths
        << QString("/home/user")
        << QString("int")
        << 3;
} 

void tst_QValueSpaceItem::testConstructor()
{
    QFETCH(QVariant, testItem);
    QFETCH(QVariant, value);
    QFETCH(QStringList, subPaths);
    QFETCH(QString, path);
    QFETCH(QString, relItemPath);
    QFETCH(int, expectedValue);

    QValueSpaceItem* item = qvariant_cast<QValueSpaceItem*>(testItem);
    item->sync();
    QCOMPARE(item->parent(), this);
    QCOMPARE(item->value(), value);
    QCOMPARE(item->subPaths().toSet(), subPaths.toSet());
    QCOMPARE(item->path(), path);
    QCOMPARE(item->value(relItemPath, 100).toInt(), expectedValue);
}

#define ADD(opt, invalid) do {\
    QTest::newRow(QString::number(opt).append(" const char *").toLocal8Bit().constData()) \
        << (QAbstractValueSpaceLayer::UnspecifiedLayer | opt) << CharStar << invalid; \
    QTest::newRow(QString::number(opt).append(" const QString &").toLocal8Bit().constData()) \
        << (QAbstractValueSpaceLayer::UnspecifiedLayer | opt) << String << invalid; \
} while (false)

void tst_QValueSpaceItem::testFilterConstructor_data()
{
    QTest::addColumn<QAbstractValueSpaceLayer::LayerOptions>("options");
    QTest::addColumn<Type>("type");
    QTest::addColumn<bool>("invalid");

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();

    for (int i = 0; i < layers.count(); ++i) {
        QAbstractValueSpaceLayer *layer = layers.at(i);

        ADD(layer->layerOptions(), false);
    }

    ADD(QAbstractValueSpaceLayer::UnspecifiedLayer, false);
    ADD(QAbstractValueSpaceLayer::PermanentLayer, false);
    ADD(QAbstractValueSpaceLayer::NonPermanentLayer, false);
    ADD(QAbstractValueSpaceLayer::PermanentLayer |
        QAbstractValueSpaceLayer::NonPermanentLayer, true);
    ADD(QAbstractValueSpaceLayer::WriteableLayer, false);
    ADD(QAbstractValueSpaceLayer::NonWriteableLayer, false);
    ADD(QAbstractValueSpaceLayer::WriteableLayer |
        QAbstractValueSpaceLayer::NonWriteableLayer, true);
}

void tst_QValueSpaceItem::testFilterConstructor()
{
    QFETCH(QAbstractValueSpaceLayer::LayerOptions, options);
    QFETCH(Type, type);
    QFETCH(bool, invalid);

    QValueSpaceItem *item;

    switch (type) {
    case CharStar:
        item = new QValueSpaceItem("/layer", options);
        break;
    case String:
        item = new QValueSpaceItem(QString("/layer"), options);
        break;
    default:
        QFAIL("Unexpected type");
        return;
    };

    if (invalid)
        QVERIFY(!item->isValid());

    if (item->isValid()) {
        QAbstractValueSpaceLayer::LayerOptions actualOptions =
            QAbstractValueSpaceLayer::LayerOptions(item->value("options", 0).toUInt());

        QVERIFY(options == QAbstractValueSpaceLayer::UnspecifiedLayer || actualOptions & options);
    }
}

void tst_QValueSpaceItem::contentsChanged_data()
{
    QTest::addColumn<QAbstractValueSpaceLayer *>("layer");

    QTest::addColumn<bool>("implicit");
    QTest::addColumn< QString >("item_path");
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

void tst_QValueSpaceItem::contentsChanged()
{
    QFETCH(QAbstractValueSpaceLayer *, layer);

    QFETCH(bool, implicit);
    QFETCH(QString, item_path);
    QFETCH(QString, value_path);
    QFETCH(int, should_emit_signal);
    QFETCH(bool, old_value);
    QFETCH(bool, new_value);

    QValueSpaceObject *busy = busys.value(layer);

    busy->setAttribute("alex/busy", old_value);
    busy->sync();

    QValueSpaceItem item(item_path, layer->id());
    QCOMPARE(item.value(value_path,!old_value).toBool(), old_value);

    ChangeListener* listener = 0;
    QSignalSpy *spy;
    if (implicit) {
        listener = new ChangeListener;
        spy = new QSignalSpy(listener, SIGNAL(baseChanged()));
        connect(&item, SIGNAL(contentsChanged()), listener, SIGNAL(baseChanged()));
    } else {
        spy = new QSignalSpy(&item, SIGNAL(contentsChanged()));
        item.setNotify(true);
    }

    QVERIFY(item.notify());

    QCOMPARE(spy->count(), 0);

    busy->setAttribute("alex/busy", new_value);
    busy->sync();

    QTRY_COMPARE(spy->count(), should_emit_signal);
    QCOMPARE(item.value(value_path,!old_value).toBool(), new_value);

    spy->clear();

    //removing the item triggers signal
    busy->removeAttribute("alex/busy");
    busy->sync();
    QTRY_COMPARE(spy->count(), should_emit_signal);
    QCOMPARE(item.value(value_path,!old_value).toBool(), new_value);

    spy->clear();
    busy->setAttribute("alex/busy", new_value);
    busy->sync();

    QTRY_COMPARE(spy->count(), should_emit_signal);
    QCOMPARE(item.value(value_path,!old_value).toBool(), new_value);

    if (!implicit) {
        item.setNotify(false);
        QVERIFY(!item.notify());

        spy->clear();
        busy->setAttribute("alex/busy", old_value);
        busy->sync();

        QTRY_COMPARE(item.value(value_path, new_value).toBool(), old_value);
        QVERIFY(spy->isEmpty());
    }

    delete spy;
    delete listener;
}

void tst_QValueSpaceItem::value_data()
{
    QTest::addColumn<QAbstractValueSpaceLayer *>("layer");

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();
    for (int i = 0; i < layers.count(); ++i)
        QTest::newRow(layers.at(i)->name().toLocal8Bit().constData()) << layers.at(i);
}

void tst_QValueSpaceItem::value()
{
    QFETCH(QAbstractValueSpaceLayer *, layer);

    QValueSpaceItem *base = new QValueSpaceItem(QString("/"), layer->id(), this);
    QCOMPARE( base->value("home/usercount", 5).toInt(),1) ;
    QCOMPARE( base->value("home/user/QString", "default").toString(), QString("testString") );
    QCOMPARE( base->value("home/user/bool", false).toBool(), true);
    QCOMPARE( base->value("home/user/int", 5).toInt(), 3);
    QCOMPARE( base->value("home/user/QByteArray", QByteArray("invalid")).toByteArray(), QByteArray("testByteArray"));
    QCOMPARE( base->value("home/user/double", 4.0).toDouble(), (double)4.56);
    //QCOMPARE( base->value("home/user/float", 4.0).toDouble(), (double)4.56);

    QValueSpaceItem *base1 = new QValueSpaceItem(QString("/home"), layer->id(), this);
    QCOMPARE( base1->value(QString("usercount"),5).toInt(),1);
    QCOMPARE( base1->value(QByteArray("user/QString"), "default").toString(), QString("testString") );
    QCOMPARE( base1->value("user/bool", false).toBool(), true);
    QCOMPARE( base1->value("user/int", 5).toInt(), 3);
    QCOMPARE( base1->value("user/QByteArray", QByteArray("invalid")).toByteArray(), QByteArray("testByteArray"));
    QCOMPARE( base1->value("user/double", 4.0).toString(),QString("4.56"));
    //QCOMPARE( base1->value("user/float", 4.0).toString(), QString("4.56"));

    QValueSpaceItem *base2 = new QValueSpaceItem(QString("/home/user"), layer->id(), this);
    QCOMPARE( base2->value(QByteArray("usercount"), 5).toInt(), 5);
    QCOMPARE( base2->value(QString("QString"), "default").toString(), QString("testString") );
    QCOMPARE( base2->value("bool", false).toBool(), true);
    QCOMPARE( base2->value("int", 5).toInt(), 3);
    QCOMPARE( base2->value("QByteArray", QByteArray("invalid")).toByteArray(), QByteArray("testByteArray"));
    QCOMPARE( base2->value("double", 4.0).toDouble(), 4.56);
    //QCOMPARE( base2->value("float", 4.0).toDouble(), 4.56);
}

void tst_QValueSpaceItem::ipcTests_data()
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

void tst_QValueSpaceItem::ipcTests()
{
#if defined(QT_NO_PROCESS)
    QSKIP("Qt was compiled with QT_NO_PROCESS", SkipAll);
#else
    QFETCH(QAbstractValueSpaceLayer *, layer);

    QValueSpaceItem item("/usr/lackey/subdir/value", layer->id());
    ChangeListener listener;
    QSignalSpy spy(&listener, SIGNAL(baseChanged()));
    connect(&item, SIGNAL(contentsChanged()), &listener, SIGNAL(baseChanged()));

    QProcess process;
    process.setProcessChannelMode(QProcess::ForwardedChannels);
    process.start("vsiTestLackey", QStringList() << "-ipcTests" << layer->id().toString());
    QVERIFY(process.waitForStarted());

    //lackey sets 100 as part of its startup
    QTRY_COMPARE(spy.count(), 1);
    QCOMPARE(item.value("", 99).toInt(), 100);
    spy.clear();

    //lackey sets 101
    QTRY_COMPARE(spy.count(), 1);
    QCOMPARE(item.value("", 99).toInt(), 101);
    spy.clear();

    //item was removed -> returns default
    QTRY_COMPARE(spy.count(), 1);
    QCOMPARE(item.value("", 99).toInt(), 99);
    spy.clear();

    //lackey sets 102
    QTRY_COMPARE(spy.count(), 1);
    QCOMPARE(item.value("", 99).toInt(), 102);
    spy.clear();

    //item was removed -> returns default
    QTRY_COMPARE(spy.count(), 1);
    QCOMPARE(item.value("", 99).toInt(), 99);

    QVERIFY(process.waitForFinished(10000));
#endif
}

void tst_QValueSpaceItem::setValue_data()
{
    QTest::addColumn<QAbstractValueSpaceLayer *>("layer");

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();
    for (int i = 0; i < layers.count(); ++i)
        QTest::newRow(layers.at(i)->name().toLocal8Bit().constData()) << layers.at(i);
}

void tst_QValueSpaceItem::setValue()
{
    QFETCH(QAbstractValueSpaceLayer *, layer);

    QValueSpaceObject *object = new QValueSpaceObject("/usr/intern/changeRequests", layer->id());
    object->setAttribute("value", 500);
    object->setObjectName("object");
    object->sync();
    QValueSpaceObject *rel_object = new QValueSpaceObject("/usr/intern", layer->id());
    rel_object->setObjectName("rel_object");
    rel_object->sync();

    QValueSpaceItem item("/usr/intern/changeRequests/value", layer->id());
    QCOMPARE(item.value("", 600).toInt(), 500);

    ChangeListener* listener = new ChangeListener();
    connect(object, SIGNAL(itemSetValue(QByteArray, QVariant)),
            listener, SIGNAL(changeValue(QByteArray,QVariant)));
    QSignalSpy spy(listener, SIGNAL(changeValue(QByteArray, QVariant)));

    ChangeListener* rel_listener = new ChangeListener();
    connect(rel_object, SIGNAL(itemSetValue(QByteArray, QVariant)),
            rel_listener, SIGNAL(changeValue(QByteArray,QVariant)));
    QSignalSpy rel_spy(rel_listener, SIGNAL(changeValue(QByteArray, QVariant)));
 
    QList<QVariant> arguments;

    if (item.setValue(501)) {
        item.sync();

        QTRY_COMPARE(spy.count(), 1);
        arguments = spy.takeFirst();

        QCOMPARE(arguments.at(0).type(), QVariant::ByteArray);
        QCOMPARE(arguments.at(0).toByteArray(),QByteArray("/value"));
        QCOMPARE(arguments.at(1).type(),QVariant::UserType);
        QCOMPARE(arguments.at(1).value<QVariant>().toInt(),501);
        QCOMPARE(item.value("", 600).toInt(), 500);
    }

    QValueSpaceItem item2("/usr/intern", layer->id());
    QCOMPARE(item2.value("changeRequests/value", 600).toInt(), 500);

    if (item2.setValue("changeRequests/value", 501)) {
        item2.sync();
        QTRY_COMPARE(spy.count(), 1);

        arguments = spy.takeFirst();
        QCOMPARE(arguments.at(0).type(), QVariant::ByteArray);
        QCOMPARE(arguments.at(0).toByteArray(),QByteArray("/value"));
        QCOMPARE(arguments.at(1).type(),QVariant::UserType);
        QCOMPARE(arguments.at(1).value<QVariant>().toInt(),501);
        QCOMPARE(item2.value("changeRequests/value", 600).toInt(), 500);
    }

    QValueSpaceItem item3("/", layer->id());
    QCOMPARE(item3.value("usr/intern/changeRequests/value", 600).toInt(), 500);

    if (item3.setValue(QString("usr/intern/changeRequests/value"), 501)) {
        item3.sync();
        QTRY_COMPARE(spy.count(), 1);

        arguments = spy.takeFirst();
        QCOMPARE(arguments.at(0).type(), QVariant::ByteArray);
        QCOMPARE(arguments.at(0).toByteArray(),QByteArray("/value"));
        QCOMPARE(arguments.at(1).type(),QVariant::UserType);
        QCOMPARE(arguments.at(1).value<QVariant>().toInt(),501);
        QCOMPARE(item3.value(QString("usr/intern/changeRequests/value"), 600).toInt(), 500);
    }

    QValueSpaceItem item4("/usr/intern/changeRequests", layer->id());
    QCOMPARE(item4.value("value", 600).toInt(), 500);

    if (item4.setValue(QByteArray("value"), 501)) {
        item4.sync();
        QTRY_COMPARE(spy.count(), 1);

        arguments = spy.takeFirst();
        QCOMPARE(arguments.at(0).type(), QVariant::ByteArray);
        QCOMPARE(arguments.at(0).toByteArray(),QByteArray("/value"));
        QCOMPARE(arguments.at(1).type(),QVariant::UserType);
        QCOMPARE(arguments.at(1).value<QVariant>().toInt(),501);
        QCOMPARE(item4.value(QByteArray("value"), 600).toInt(), 500);
    }

    if (layer->layerOptions() & QAbstractValueSpaceLayer::PermanentLayer)
        object->removeAttribute("value");

    delete listener;
    delete rel_listener;
    delete object;
    delete rel_object;
}

void tst_QValueSpaceItem::removeValue_data()
{
    QTest::addColumn<QAbstractValueSpaceLayer *>("layer");

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();
    for (int i = 0; i < layers.count(); ++i)
        QTest::newRow(layers.at(i)->name().toLocal8Bit().constData()) << layers.at(i);
}

void tst_QValueSpaceItem::removeValue()
{
    QFETCH(QAbstractValueSpaceLayer *, layer);

    QValueSpaceObject *object = new QValueSpaceObject("/usr/intern/changeRequests", layer->id());
    object->setAttribute("value", 500);
    object->sync();
    object->setObjectName("object");
    QValueSpaceObject *rel_object = new QValueSpaceObject("/usr/intern", layer->id());
    rel_object->setObjectName("rel_object");

    QValueSpaceItem item("/usr/intern/changeRequests/value", layer->id());
    QCOMPARE(item.value("", 600).toInt(), 500);

    ChangeListener* listener = new ChangeListener();
    connect(object, SIGNAL(itemRemove(QByteArray)),
            listener, SIGNAL(itemRemove(QByteArray)));
    QSignalSpy spy(listener, SIGNAL(itemRemove(QByteArray)));

    ChangeListener* rel_listener = new ChangeListener();
    connect(rel_object, SIGNAL(itemRemove(QByteArray)),
            rel_listener, SIGNAL(itemRemove(QByteArray)));
    QSignalSpy rel_spy(rel_listener, SIGNAL(itemRemove(QByteArray)));

    QList<QVariant> arguments;

    if (item.remove()) {
        item.sync();

        QTRY_COMPARE(spy.count(), 1);
        arguments = spy.takeFirst();

        QCOMPARE(arguments.at(0).type(), QVariant::ByteArray);
        QCOMPARE(arguments.at(0).toByteArray(),QByteArray("/value"));
        QCOMPARE(item.value("", 600).toInt(), 500);
    }

    QValueSpaceItem item2("/usr/intern", layer->id());
    QCOMPARE(item2.value("changeRequests/value", 600).toInt(), 500);

    if (item2.remove("changeRequests/value")) {
        item2.sync();
        QTRY_COMPARE(spy.count(), 1);

        arguments = spy.takeFirst();
        QCOMPARE(arguments.at(0).type(), QVariant::ByteArray);
        QCOMPARE(arguments.at(0).toByteArray(),QByteArray("/value"));
        QCOMPARE(item2.value("changeRequests/value", 600).toInt(), 500);
    }

    QValueSpaceItem item3("/", layer->id());
    QCOMPARE(item3.value("usr/intern/changeRequests/value", 600).toInt(), 500);

    if (item3.remove(QString("usr/intern/changeRequests/value"))) {
        item3.sync();
        QTRY_COMPARE(spy.count(), 1);

        arguments = spy.takeFirst();
        QCOMPARE(arguments.at(0).type(), QVariant::ByteArray);
        QCOMPARE(arguments.at(0).toByteArray(),QByteArray("/value"));
        QCOMPARE(item3.value(QString("usr/intern/changeRequests/value"), 600).toInt(), 500);
    }

    QValueSpaceItem item4("/usr/intern/changeRequests", layer->id());
    QCOMPARE(item4.value("value", 600).toInt(), 500);

    if (item4.remove(QByteArray("value"))) {
        item4.sync();
        QTRY_COMPARE(spy.count(), 1);

        arguments = spy.takeFirst();
        QCOMPARE(arguments.at(0).type(), QVariant::ByteArray);
        QCOMPARE(arguments.at(0).toByteArray(),QByteArray("/value"));
        QCOMPARE(item4.value(QByteArray("value"), 600).toInt(), 500);
    }

    if (layer->layerOptions() & QAbstractValueSpaceLayer::PermanentLayer)
        object->removeAttribute("value");

    delete listener;
    delete rel_listener;
    delete object;
    delete rel_object;
}

void tst_QValueSpaceItem::ipcRemoveKey_data()
{
#ifdef QT_NO_PROCESS
    QSKIP("Qt was compiled with QT_NO_PROCESS", SkipAll);
#else
    QTest::addColumn<QAbstractValueSpaceLayer *>("layer");

    bool skip = true;

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();
    for (int i = 0; i < layers.count(); ++i) {
        QAbstractValueSpaceLayer *layer = layers.at(i);

        if (layer->layerOptions() & QAbstractValueSpaceLayer::PermanentLayer)
            continue;

        skip = false;

        QTest::newRow(layer->name().toLocal8Bit().constData()) << layer;
    }

    if (skip)
        QSKIP("No applicable layers found.", SkipAll);
#endif
}

void tst_QValueSpaceItem::ipcRemoveKey()
{
#if defined(QT_NO_PROCESS)
    QSKIP("Qt was compiled with QT_NO_PROCESS", SkipAll);
#else
    QFETCH(QAbstractValueSpaceLayer *, layer);

    QValueSpaceItem item("/ipcRemoveKey", layer->id());

    ChangeListener listener;
    QSignalSpy changeSpy(&listener, SIGNAL(baseChanged()));
    QObject::connect(&item, SIGNAL(contentsChanged()), &listener, SIGNAL(baseChanged()));

    QProcess process;
    process.setProcessChannelMode(QProcess::ForwardedChannels);
    process.start("vsiTestLackey", QStringList() << "-ipcRemoveKey" << layer->id().toString());
    QVERIFY(process.waitForStarted());

    // Wait for lackey to create "value".
    QTRY_COMPARE(changeSpy.count(), 1);
    QCOMPARE(item.value("value", 5).toInt(), 100);

    // Wait for lackey to delete key "/ipcRemoveKey".
    changeSpy.clear();
    QTRY_COMPARE(changeSpy.count(), 1);

    QStringList paths = item.subPaths();
    if (!paths.isEmpty())
        qDebug() << item.subPaths();
    QVERIFY(item.subPaths().isEmpty());
    QCOMPARE(item.value("value", 6).toInt(), 6);
    QVERIFY(process.waitForFinished(10000));
#endif
}

void tst_QValueSpaceItem::ipcSetValue_data()
{
#ifdef QT_NO_PROCESS
    QSKIP("Qt was compiled with QT_NO_PROCESS", SkipAll);
#else
    QTest::addColumn<QAbstractValueSpaceLayer *>("layer");

    bool skip = true;

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();
    for (int i = 0; i < layers.count(); ++i) {
        QAbstractValueSpaceLayer *layer = layers.at(i);

        if (!layer->supportsRequests())
            continue;

        skip = false;

        QTest::newRow(layer->name().toLocal8Bit().constData()) << layer;
    }

    if (skip)
        QSKIP("No applicable layers found.", SkipAll);
#endif
}

void tst_QValueSpaceItem::ipcSetValue()
{
#if defined(QT_NO_PROCESS)
    QSKIP("Qt was compiled with QT_NO_PROCESS", SkipAll);
#else
    QFETCH(QAbstractValueSpaceLayer *, layer);

    QList<QValueSpaceObject*> objects;
    QList<QSignalSpy*> spies;
    QList<ChangeListener*> listeners;

    objects.append(new QValueSpaceObject("/usr/lackey", layer->id()));  //actual owner
    objects.append(new QValueSpaceObject("/usr/lackey/", layer->id())); //happens to use same path
    objects.append(new QValueSpaceObject("/usr", layer->id())); //parent path
    objects.append(new QValueSpaceObject("/usr/lackey/unrelated", layer->id())); //subpath
    objects.at(0)->setAttribute("changeRequests/value", 500);
    objects.at(0)->sync();

    QValueSpaceItem item("/usr/lackey/changeRequests/value", layer->id());

    const int itemCount = 4;
    for(int j =0; j<itemCount; j++) {
        //objects.at(j)->setObjectName(QString("obj%1").arg(j));
        ChangeListener * listener = new ChangeListener();
        connect(objects.at(j), SIGNAL(itemSetValue(QByteArray, QVariant)),
                listener, SIGNAL(changeValue(QByteArray, QVariant)));
        connect(objects.at(j), SIGNAL(itemRemove(QByteArray)),
                listener, SIGNAL(itemRemove(QByteArray)));
        listeners.append(listener);
        QSignalSpy* changeSpy = new QSignalSpy(listener, SIGNAL(changeValue(QByteArray,QVariant)));
        QSignalSpy* removeSpy = new QSignalSpy(listener, SIGNAL(itemRemove(QByteArray)));
        spies.append(changeSpy);
        spies.append(removeSpy);
    } 

    QCOMPARE(item.value("", 600).toInt(), 500);

    QProcess process;
    process.setProcessChannelMode(QProcess::ForwardedChannels);
    process.start("vsiTestLackey", QStringList() << "-ipcSetValue" << layer->id().toString());
    QVERIFY(process.waitForStarted());

    QTest::qWait(5000);

    if (process.state() == QProcess::NotRunning &&
        process.exitCode() == ERROR_SETVALUE_NOT_SUPPORTED) {
        QFAIL("setValue not supported by underlying layer");
    }

    //QTRY_COMPARE(changeSpy.count(), 3);
    QTRY_COMPARE(spies.at(0)->count(), 3);
    QTRY_COMPARE(spies.at(1)->count(), 5);

    for(int i=0; i<spies.count(); i++) {
        QSignalSpy * spy = spies.at(i);
        int k = 0;
        if (i == 6 || i == 7) { //unrelated subpath item
            QCOMPARE(spy->count(), 0);
        } else if ( i%2 == 0) { //change spies
            while(!spy->isEmpty()) {
                QList<QVariant> arguments = spy->takeFirst();
                QCOMPARE(arguments.at(0).type(), QVariant::ByteArray);
                if (i==4)
                    QCOMPARE(arguments.at(0).toByteArray(),QByteArray("/lackey/changeRequests/value"));
                else
                    QCOMPARE(arguments.at(0).toByteArray(),QByteArray("/changeRequests/value"));
                QCOMPARE(arguments.at(1).type(),QVariant::UserType);
                QCOMPARE(arguments.at(1).value<QVariant>().toInt(),501+k);
                k+=1;
            }
        } else { //remove spies
            QByteArray offset("");
            if (i == 5)
                offset = "/lackey";
            while (!spy->isEmpty()) {
                QList<QVariant> arguments = spy->takeFirst();
                QCOMPARE(arguments.at(0).type(), QVariant::ByteArray);
                if (k == 3)
                    QCOMPARE(arguments.at(0).toByteArray(),offset+QByteArray("/changeRequests/test"));
                else if (k==4)
                    QCOMPARE(arguments.at(0).toByteArray(),offset+QByteArray(""));
                else
                    QCOMPARE(arguments.at(0).toByteArray(),offset+QByteArray("/changeRequests/value"));
                k++;
            }
        }
    }
    QCOMPARE(item.value("", 600).toInt(), 500);

    while(!spies.isEmpty())
        delete spies.takeFirst();
    while(!listeners.isEmpty())
        delete listeners.takeFirst();
    while(!objects.isEmpty())
        delete objects.takeFirst();
    QVERIFY(process.waitForFinished(10000));
#endif
}

void tst_QValueSpaceItem::interestNotification_data()
{
    QTest::addColumn<QAbstractValueSpaceLayer *>("layer");

    QTest::addColumn<Type>("type");
    QTest::addColumn<QString>("objectPath");
    QTest::addColumn<QString>("attribute");

    bool skip = true;

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();
    for (int i = 0; i < layers.count(); ++i) {
        QAbstractValueSpaceLayer *layer = layers.at(i);

        if (!layer->supportsRequests())
            continue;

        skip = false;

        QTest::newRow("QValueSpaceItem(char *)")
            << layer << CharStar << "/interestNotification" << "/value";
        QTest::newRow("QValueSpaceItem(QString)")
            << layer << String << "/interestNotification" << "/value";
    }

    if (skip)
        QSKIP("No applicable layers found.", SkipAll);
}

void tst_QValueSpaceItem::interestNotification()
{
    QFETCH(QAbstractValueSpaceLayer *, layer);

    QFETCH(Type, type);
    QFETCH(QString, objectPath);
    QFETCH(QString, attribute);

    QValueSpaceObject *object;
    object = new QValueSpaceObject(objectPath, layer->id());

    ChangeListener notificationListener;
    connect(object, SIGNAL(itemNotify(QByteArray,bool)),
            &notificationListener, SIGNAL(itemNotify(QByteArray,bool)));

    QSignalSpy notificationSpy(&notificationListener, SIGNAL(itemNotify(QByteArray,bool)));

    const QString itemPath = objectPath + attribute;

    QValueSpaceItem *item;
    switch (type) {
    case CharStar:
        item = new QValueSpaceItem(itemPath.toUtf8().constData(), layer->id());
        break;
    case String:
        item = new QValueSpaceItem(itemPath, layer->id());
        break;
    default:
        item = 0;
        QFAIL("Invalid type");
    }

    QTRY_COMPARE(notificationSpy.count(), 1);

    QList<QVariant> arguments = notificationSpy.takeFirst();
    QCOMPARE(arguments.at(0).type(), QVariant::ByteArray);
    QCOMPARE(arguments.at(0).toByteArray(), attribute.toUtf8());
    QCOMPARE(arguments.at(1).type(), QVariant::Bool);
    QCOMPARE(arguments.at(1).toBool(), true);

    QCOMPARE(item->value(QString(), 10).toInt(), 10);

    object->setAttribute(attribute, 5);
    object->sync();

    QCOMPARE(item->value(QString(), 10).toInt(), 5);

    notificationSpy.clear();

    delete item;

    QTRY_COMPARE(notificationSpy.count(), 1);

    arguments = notificationSpy.takeFirst();
    QCOMPARE(arguments.at(0).type(), QVariant::ByteArray);
    QCOMPARE(arguments.at(0).toByteArray(), attribute.toUtf8());
    QCOMPARE(arguments.at(1).type(), QVariant::Bool);
    QCOMPARE(arguments.at(1).toBool(), false);

    delete object;
}

void tst_QValueSpaceItem::ipcInterestNotification_data()
{
#ifdef QT_NO_PROCESS
    QSKIP("Qt was compiled with QT_NO_PROCESS", SkipAll);
#else
    QTest::addColumn<QAbstractValueSpaceLayer *>("layer");

    bool skip = true;

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();
    for (int i = 0; i < layers.count(); ++i) {
        QAbstractValueSpaceLayer *layer = layers.at(i);

        if (!layer->supportsRequests())
            continue;

        skip = false;

        QTest::newRow(layers.at(i)->name().toLocal8Bit().constData()) << layers.at(i);
    }

    if (skip)
        QSKIP("No applicable layers found.", SkipAll);
#endif
}

void tst_QValueSpaceItem::ipcInterestNotification()
{
#if defined(QT_NO_PROCESS)
    QSKIP("Qt was compiled with QT_NO_PROCESS", SkipAll);
#else
    QFETCH(QAbstractValueSpaceLayer *, layer);

    // Test QValueSpaceItem construction before QValueSpaceObject.

    QValueSpaceItem *item = new QValueSpaceItem("/ipcInterestNotification/value", layer->id());

    ChangeListener listener;
    QObject::connect(item, SIGNAL(contentsChanged()), &listener, SIGNAL(baseChanged()));
    QSignalSpy changeSpy(&listener, SIGNAL(baseChanged()));

    // Lackey is not running, so value will not exist.
    QCOMPARE(item->value(QString(), 10).toInt(), 10);

    QProcess process;
    process.setProcessChannelMode(QProcess::ForwardedChannels);
    process.start("vsiTestLackey", QStringList()
        << "-ipcInterestNotification" << layer->id().toString());
    QVERIFY(process.waitForStarted());

    // Lackey will receive itemNotify from server and  set the attribute.
    QTRY_COMPARE(changeSpy.count(), 1);
    changeSpy.clear();

    QCOMPARE(item->value(QString(), 10).toInt(), 5);

    // Lackey will receive itemNotify and remove attribute.
    delete item;
    QTest::qWait(1000);

    // Test QValueSpaceItem construction after QValueSpaceObject

    item = new QValueSpaceItem("/ipcInterestNotification/value", layer->id());
    QObject::connect(item, SIGNAL(contentsChanged()), &listener, SIGNAL(baseChanged()));

    QTRY_COMPARE(changeSpy.count(), 1);

    QCOMPARE(item->value(QString(), 10).toInt(), 5);
#endif
}

void tst_QValueSpaceItem::clientServer()
{
#if defined(QT_START_VALUESPACE)
    QVERIFY(QValueSpaceManager::instance()->isServer());
#else
    QVERIFY(!QValueSpaceManager::instance()->isServer());
#endif
}

#include "tst_qvaluespaceitemshared.moc"
