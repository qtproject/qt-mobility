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
#include <qvaluespaceobject.h>

#include <QtTest/QtTest>
#include <QtCore>
#include <QVariant>
#include <QProcess>
#include <QFile>
#include <QVariant>

#define ERROR_SETVALUE_NOT_SUPPORTED 1

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
    void baseChanged();
    void copyChanged();
    void changeValue(const QByteArray&, const QVariant&);
    void itemRemove(const QByteArray&);
    void itemNotify(const QByteArray&, bool);
};



Q_DECLARE_METATYPE(QValueSpaceItem*)
Q_DECLARE_METATYPE(QVariant)
Q_DECLARE_METATYPE(QList<QString>)

class tst_QValueSpaceItem_oop: public QObject
{
    Q_OBJECT

private:
    QValueSpaceObject* root;
    QValueSpaceObject* busy;

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();

    void testConstructor();
    void testConstructor_data();
    void testAssignmentOperator();
    void contentsChanged_data();
    void contentsChanged();
    void dataVersatility_data();
    void dataVersatility();
    void value();
    void ipcTests();
    void setValue();
    void ipcSetValue();
    void interestNotification_data();
    void interestNotification();
    void ipcInterestNotification();
};

void tst_QValueSpaceItem_oop::initTestCase()
{
#if defined(QT_NO_PROCESS)
    QSKIP("Qt was compiled with QT_NO_PROCESS", SkipAll);
#else
    root = new QValueSpaceObject("/");
    root->setAttribute("/home/user/bool", true);
    root->setAttribute("/home/user/int", 3);
    root->setAttribute("/home/user/QString", QString("testString"));
    root->setAttribute("/home/user/QByteArray", QString("testByteArray"));
    root->setAttribute("/home/user/double", 4.56);
    root->setAttribute("/home/user/float", (float)4.56f);
    root->setAttribute("/home/user/QChar", QChar('c'));
    //so far not a lot of data types are supported
    //root->setAttribute("/home/user/QRect", QRect(0,0,5,6));
    root->setAttribute("/home/usercount", 1);
    root->sync();

    busy = new QValueSpaceObject("/usr");
    busy->setAttribute("alex/busy", true);
    busy->setAttribute("lorn/busy", false);
    busy->sync();
#endif
}

void tst_QValueSpaceItem_oop::init()
{
}

void tst_QValueSpaceItem_oop::cleanupTestCase()
{
#if defined(QT_NO_PROCESS)
    QSKIP("Qt was compiled with QT_NO_PROCESS", SkipAll);
#else
    delete root;
    delete busy;
#endif
}

void tst_QValueSpaceItem_oop::dataVersatility_data()
{
    QTest::addColumn< QVariant >("data");
    QTest::addColumn< QString >("typeString");
    QTest::addColumn< int >("typeIdent");

    //these types have custom loading/saving operator
    QTest::newRow("Int") << QVariant((int)567) << "Int" << (int)QVariant::Int;
    QTest::newRow("Bool") << QVariant((bool)true) << "Bool" << (int)QVariant::Bool;
    QTest::newRow("UInt") << QVariant((unsigned int)4) << "UInt" << (int)QVariant::UInt;
    QTest::newRow("LongLong") << QVariant((long long)5) << "LongLong" << (int)QVariant::LongLong;
    QTest::newRow("ULongLong") << QVariant((unsigned long long)6) << "ULongLong" << (int)QVariant::ULongLong;
    QTest::newRow("Double") << QVariant((double)4.5) << "Double" << (int)QVariant::Double;
    QTest::newRow("QChar") << QVariant(QChar('@')) << "Char" << (int)QVariant::Char;
    QTest::newRow("QString") << QVariant(QString("asd")) << "QString" << (int)QVariant::String;
    QTest::newRow("QByteArray") << QVariant(QByteArray("bytearray")) << "QByteArray" << (int)QVariant::ByteArray;

    //other types not specifically covered by valuespace -> uses QVariant based serialization
    QTest::newRow("QRect") << QVariant(QRect(4,5,6,7)) << "QRect" << (int)QVariant::Rect;
    QMap<QString,QVariant> map;
    map.insert("key", QVariant(QRect(4,5,8,9)));
    QTest::newRow("QDateTime") << QVariant(QDateTime::currentDateTime()) << "QDateTime" << (int)QVariant::DateTime;
}

void tst_QValueSpaceItem_oop::dataVersatility()
{
#if defined(QT_NO_PROCESS)
    QSKIP("Qt was compiled with QT_NO_PROCESS", SkipAll);
#else
    QFETCH(QVariant, data);
    QFETCH(QString, typeString);
    QFETCH(int, typeIdent);

    QCOMPARE(data.type(), (QVariant::Type)typeIdent);

    QValueSpaceObject object("/usr/data");
    object.setAttribute(typeString, data);
    object.sync();
    QValueSpaceItem item("/usr/data");
    item.sync();
    QVariant v = item.value(typeString);

    QCOMPARE(v.type(), (QVariant::Type)typeIdent);
    QCOMPARE(v, data);
#endif
}

void tst_QValueSpaceItem_oop::testConstructor_data()
{
    QTest::addColumn< QVariant >("testItem");
    QTest::addColumn< QVariant >("value");
    QTest::addColumn< QList<QString> >("subPaths");
    QTest::addColumn< QString >("itemName");
    QTest::addColumn< QString >("relItemPath");
    QTest::addColumn< int >("expectedValue");

    QList<QString> allPaths;
    //allPaths << "bool" << "int" << "QString" << "QByteArray" << "double" << "float" << "QChar" << "QRect";
    allPaths << "bool" << "int" << "QString" << "QByteArray" << "double" << "float" << "QChar";

    //valid items based on / path
    QValueSpaceItem* item1 = new QValueSpaceItem(QString(), this);
    QVariant data;
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(QString(), this)") 
        << data
        << QVariant()
        << (QList<QString>() << "home" << "usr")
        << QString("/")
        << QString("home/user/int")
        << 3;

    item1 = new QValueSpaceItem(QString("/"), this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(QString(\"/\"), this)") 
        << data
        << QVariant()
        << (QList<QString>() << "home"<< "usr")
        << QString("/")
        << QString("home/user/int")
        << 3;

    item1 = new QValueSpaceItem(QByteArray(), this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(QByteArray(), this)") 
        << data
        << QVariant()
        << (QList<QString>() << "home" << "usr")
        << QString("/")
        << QString("home/user/int")
        << 3;

    item1 = new QValueSpaceItem(QByteArray("/"), this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(QByteArray(\"/\", this)") 
        << data
        << QVariant()
        << (QList<QString>() << "home" << "usr")
        << QString("/")
        << QString("home/user/int")
        << 3;

    item1 = new QValueSpaceItem("", this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(\"\", this)") 
        << data
        << QVariant()
        << (QList<QString>() << "home" << "usr")
        << QString("/")
        << QString("home/user/int")
        << 3;

    item1 = new QValueSpaceItem("/", this);
    QValueSpaceItem* baseRoot = item1;
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(\"/\", this)") 
        << data
        << QVariant()
        << (QList<QString>() << "home" << "usr")
        << QString("/")
        << QString("home/user/int")
        << 3;

    item1 = new QValueSpaceItem(this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(this)") 
        << data
        << QVariant()
        << (QList<QString>() << "home" <<"usr")
        << QString("/")
        << QString("home/user/int")
        << 3;

    //valid items based on /home path
    item1 = new QValueSpaceItem(QString("/home"), this);
    QValueSpaceItem* baseHome = item1;
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(QString(\"/home\"), this)") 
        << data
        << QVariant()
        << (QList<QString>() << "user" << "usercount")
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

    //invalid path
    item1 = new QValueSpaceItem(QByteArray("/home/invalidPath"), this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(QByteArray(\"/home/invalidPath\"), this)") 
        << data
        << QVariant()
        << QList<QString>()
        << QString("/home/invalidPath")
        << QString("user/int")
        << 100; //should fails -> returns default

    //items based on home base item without subpath
    item1 = new QValueSpaceItem(*baseHome, this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseHome, this)") 
        << data
        << QVariant()
        << (QList<QString>() << "user" << "usercount")
        << QString("/home")
        << QString("user/int")
        << 3;

    //items based on root base item without subpath
    item1 = new QValueSpaceItem(*baseRoot, this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseRoot, this)") 
        << data
        << QVariant()
        << (QList<QString>() << "home" << "usr")
        << QString("/")
        << QString("home/user/int")
        << 3;

    //home base item + empty subpath
    item1 = new QValueSpaceItem(*baseHome,QString(), this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseHome, QString(), this)")
        << data
        << QVariant()
        << (QList<QString>() << "user" << "usercount")
        << QString("/home")
        << QString("user/int")
        << 3;

    item1 = new QValueSpaceItem(*baseHome,QByteArray(), this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseHome, QByteArray(), this)")
        << data
        << QVariant()
        << (QList<QString>() << "user" << "usercount")
        << QString("/home")
        << QString("user/int")
        << 3;

    item1 = new QValueSpaceItem(*baseHome,"", this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseHome, \"\", this)")
        << data
        << QVariant()
        << (QList<QString>() << "user" << "usercount")
        << QString("/home")
        << QString("user/int")
        << 3;

    //root base item + empty subpath
    item1 = new QValueSpaceItem(*baseRoot,QString(), this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseRoot, QString(), this)")
        << data
        << QVariant()
        << (QList<QString>() << "home" << "usr")
        << QString("/")
        << QString("home/user/int")
        << 3;

    item1 = new QValueSpaceItem(*baseRoot,QByteArray(), this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseRoot, QByteArray(), this)")
        << data
        << QVariant()
        << (QList<QString>() << "home" << "usr")
        << QString("/")
        << QString("home/user/int")
        << 3;

    item1 = new QValueSpaceItem(*baseRoot,"", this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseRoot, \"\", this)")
        << data
        << QVariant()
        << (QList<QString>() << "home" << "usr")
        << QString("/")
        << QString("home/user/int")
        << 3;

    //home base item + "/" subpath
    item1 = new QValueSpaceItem(*baseHome,QString("/"), this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseHome, QString(\"/\"), this)") 
        << data
        << QVariant()
        << (QList<QString>() << "user" << "usercount")
        << QString("/home")
        << QString("user/int")
        << 3;

    item1 = new QValueSpaceItem(*baseHome,QByteArray("/"), this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseHome, QByteArray(\"/\"), this)") 
        << data
        << QVariant()
        << (QList<QString>() << "user" << "usercount")
        << QString("/home")
        << QString("user/int")
        << 3;

    item1 = new QValueSpaceItem(*baseHome,"/", this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseHome, \"/\",this)") 
        << data
        << QVariant()
        << (QList<QString>() << "user" << "usercount")
        << QString("/home")
        << QString("user/int")
        << 3;

    //root base item + "/" subpath
    item1 = new QValueSpaceItem(*baseRoot,QString("/"), this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseRoot, QString(\"/\"), this)") 
        << data
        << QVariant()
        << (QList<QString>() << "home" << "usr")
        << QString("/")
        << QString("home/user/int")
        << 3;

    item1 = new QValueSpaceItem(*baseRoot,QByteArray("/"), this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseRoot, QByteArray(\"/\"), this)") 
        << data
        << QVariant()
        << (QList<QString>() << "home" << "usr")
        << QString("/")
        << QString("home/user/int")
        << 3;

    item1 = new QValueSpaceItem(*baseRoot,"/", this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseRoot, \"/\",this)") 
        << data
        << QVariant()
        << (QList<QString>() << "home" << "usr")
        << QString("/")
        << QString("home/user/int")
        << 3;

    //home base item + "user" subpath
    item1 = new QValueSpaceItem(*baseHome,QString("user"), this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseHome, QString(\"user\"), this)") 
        << data
        << QVariant()
        << allPaths
        << QString("/home/user")
        << QString("int")
        << 3;

    item1 = new QValueSpaceItem(*baseHome,QByteArray("user"), this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseHome, QByteArray(\"user\"), this)") 
        << data
        << QVariant()
        << allPaths
        << QString("/home/user")
        << QString("int")
        << 3;

    item1 = new QValueSpaceItem(*baseHome,"user", this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseHome, \"user\",this)") 
        << data
        << QVariant()
        << allPaths
        << QString("/home/user")
        << QString("int")
        << 3;

    //root base item + "home" subpath
    item1 = new QValueSpaceItem(*baseRoot,QString("home"), this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseRoot, QString(\"home\"), this)") 
        << data
        << QVariant()
        << (QList<QString>() << "user" << "usercount")
        << QString("/home")
        << QString("user/int")
        << 3;

    item1 = new QValueSpaceItem(*baseRoot,QByteArray("home"), this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseRoot, QByteArray(\"home\"), this)") 
        << data
        << QVariant()
        << (QList<QString>() << "user" << "usercount")
        << QString("/home")
        << QString("user/int")
        << 3;

    item1 = new QValueSpaceItem(*baseRoot,"home", this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseRoot, \"home\",this)") 
        << data
        << QVariant()
        << (QList<QString>() << "user" << "usercount")
        << QString("/home")
        << QString("user/int")
        << 3;

    item1 = new QValueSpaceItem(*baseHome,QString("/user"), this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseHome, QString(\"/user\"), this)")
        << data
        << QVariant()
        << allPaths
        << QString("/home/user")
        << QString("int")
        << 3;

    item1 = new QValueSpaceItem(*baseHome,QByteArray("/user"), this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseHome, QByteArray(\"/user\"), this)")
        << data
        << QVariant()
        << allPaths
        << QString("/home/user")
        << QString("int")
        << 3;

    item1 = new QValueSpaceItem(*baseHome,"/user", this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseHome, \"/user\",this)")
        << data
        << QVariant()
        << allPaths
        << QString("/home/user")
        << QString("int")
        << 3;

    item1 = new QValueSpaceItem(*baseRoot,QString("/home"), this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseRoot, QString(\"/home\"), this)") 
        << data
        << QVariant()
        << (QList<QString>() << "user" << "usercount")
        << QString("/home")
        << QString("user/int")
        << 3;

    item1 = new QValueSpaceItem(*baseRoot,QByteArray("/home"), this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseRoot, QByteArray(\"/home\"), this)") 
        << data
        << QVariant()
        << (QList<QString>() << "user" << "usercount")
        << QString("/home")
        << QString("user/int")
        << 3;

    item1 = new QValueSpaceItem(*baseRoot,"/home", this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseRoot, \"/home\",this)")
        << data
        << QVariant()
        << (QList<QString>() << "user" << "usercount")
        << QString("/home")
        << QString("user/int")
        << 3;
} 

void tst_QValueSpaceItem_oop::testConstructor()
{
#if defined(QT_NO_PROCESS)
    QSKIP("Qt was compiled with QT_NO_PROCESS", SkipAll);
#else
    QFETCH(QVariant, testItem);
    QFETCH(QVariant, value);
    QFETCH(QList<QString>, subPaths);
    QFETCH(QString, itemName);
    QFETCH(QString, relItemPath);
    QFETCH(int, expectedValue);

    QValueSpaceItem* item = qvariant_cast<QValueSpaceItem*>(testItem);
    item->sync();
    QCOMPARE(item->parent(), this);
    QCOMPARE(item->value(), value);
    QCOMPARE(item->subPaths().toSet(), subPaths.toSet());
    QCOMPARE(item->itemName(), itemName);
    QCOMPARE(item->value(relItemPath, 100).toInt(), expectedValue);
#endif
}

void tst_QValueSpaceItem_oop::testAssignmentOperator()
{
#if defined(QT_NO_PROCESS)
    QSKIP("Qt was compiled with QT_NO_PROCESS", SkipAll);
#else
    QValueSpaceObject* object = new QValueSpaceObject("/changes");
    object->setAttribute("subchange/value", 34);
    object->sync();
    QValueSpaceItem* item = new QValueSpaceItem("/changes", this);
    QCOMPARE(item->value("subchange/value").toInt(), 34);
    QValueSpaceItem* copy = new QValueSpaceItem("/misc");

    //we cannot directly connect to spy since contentsChanged() is done 
    //via QObject::connectNotify() -> use proxy object
    ChangeListener listener;
    QObject::connect(item, SIGNAL(contentsChanged()), &listener, SIGNAL(baseChanged()));
    QObject::connect(copy, SIGNAL(contentsChanged()), &listener, SIGNAL(copyChanged()));

    QSignalSpy base_spy(&listener, SIGNAL(baseChanged()));
    QSignalSpy copy_spy(&listener, SIGNAL(copyChanged()));
    object->setAttribute("subchange/value", 35);
    object->sync();
    
    QTRY_COMPARE(base_spy.count(), 1);
    QCOMPARE(item->value("subchange/value").toInt(), 35);
    QCOMPARE(copy_spy.count(), 0);
    QCOMPARE(item->value("subchanges/value", 45).toInt(), 45);

    base_spy.clear();
    QCOMPARE(base_spy.count(), 0);
    (*copy) = *item;

    object->setAttribute("subchange/value", 55);
    object->sync();

    QTRY_COMPARE(base_spy.count(), 1);
    QTRY_COMPARE(copy_spy.count(), 1);
    QCOMPARE(item->value("subchange/value").toInt(), 55);
    QCOMPARE(copy->value("subchange/value").toInt(), 55);

    delete item;
    delete copy;
    delete object;
#endif
}

void tst_QValueSpaceItem_oop::contentsChanged_data()
{
    QTest::addColumn< QString >("item_path");
    QTest::addColumn< QString >("value_path");
    QTest::addColumn< int >("should_emit_signal");
    QTest::addColumn< bool >("old_value");
    QTest::addColumn< bool >("new_value");

    QTest::newRow("(empty)")
        << ""
        << "usr/alex/busy"
        << 1
        << false
        << true;
    
    QTest::newRow("/")
        << "/"
        << "usr/alex/busy"
        << 1
        << false
        << true;

    QTest::newRow("/usr")
        << "/usr"
        << "alex/busy"
        << 1
        << false
        << true;

    QTest::newRow("/usr/alex")
        << "/usr/alex"
        << "busy"
        << 1
        << false
        << true;

    QTest::newRow("/usr/alex/busy")
        << "/usr/alex/busy"
        << ""
        << 1
        << false
        << true;

    QTest::newRow("/usr/lorn")
        << "/usr/lorn"
        << "busy"
        << 0
        << false
        << false;
}

void tst_QValueSpaceItem_oop::contentsChanged()
{
#if defined(QT_NO_PROCESS)
    QSKIP("Qt was compiled with QT_NO_PROCESS", SkipAll);
#else
    QFETCH(QString, item_path);
    QFETCH(QString, value_path);
    QFETCH(int, should_emit_signal);
    QFETCH(bool, old_value);
    QFETCH(bool, new_value);

    busy->setAttribute("alex/busy", old_value);
    busy->sync();

    QValueSpaceItem item(item_path);
    QCOMPARE(item.value(value_path,!old_value).toBool(), old_value);

    ChangeListener* listener = new ChangeListener();
    QSignalSpy spy(listener, SIGNAL(baseChanged()));
    connect(&item, SIGNAL(contentsChanged()),listener, SIGNAL(baseChanged()));
    QCOMPARE(spy.count(), 0);

    busy->setAttribute("alex/busy", new_value);
    busy->sync();

    QTRY_COMPARE(spy.count(), should_emit_signal);
    QCOMPARE(item.value(value_path,!old_value).toBool(), new_value);

    spy.clear();

    //removing the item triggers signal
    busy->removeAttribute("alex/busy");
    busy->sync();
    QTRY_COMPARE(spy.count(), should_emit_signal);
    QCOMPARE(item.value(value_path,!old_value).toBool(), new_value);

    spy.clear();
    busy->setAttribute("alex/busy", new_value);
    busy->sync();

    QTRY_COMPARE(spy.count(), should_emit_signal);
    QCOMPARE(item.value(value_path,!old_value).toBool(), new_value);

    delete listener;
#endif
}

void tst_QValueSpaceItem_oop::value()
{
#if defined(QT_NO_PROCESS)
    QSKIP("Qt was compiled with QT_NO_PROCESS", SkipAll);
#else
    QValueSpaceItem* base = new QValueSpaceItem(QString("/"), this);
    QCOMPARE( base->value("home/usercount", 5).toInt(),1) ;
    QCOMPARE( base->value("home/user/QString", "default").toString(), QString("testString") );
    QCOMPARE( base->value("home/user/bool", false).toBool(), true);
    QCOMPARE( base->value("home/user/int", 5).toInt(), 3);
    QCOMPARE( base->value("home/user/QByteArray", QByteArray("invalid")).toByteArray(), QByteArray("testByteArray"));
    QCOMPARE( base->value("home/user/double", 4.0).toDouble(), (double)4.56);
    //QCOMPARE( base->value("home/user/float", 4.0).toDouble(), (double)4.56);

    QValueSpaceItem* base1 = new QValueSpaceItem(QString("/home"), this);
    QCOMPARE( base1->value(QString("usercount"),5).toInt(),1);
    QCOMPARE( base1->value(QByteArray("user/QString"), "default").toString(), QString("testString") );
    QCOMPARE( base1->value("user/bool", false).toBool(), true);
    QCOMPARE( base1->value("user/int", 5).toInt(), 3);
    QCOMPARE( base1->value("user/QByteArray", QByteArray("invalid")).toByteArray(), QByteArray("testByteArray"));
    QCOMPARE( base1->value("user/double", 4.0).toString(),QString("4.56"));
    //QCOMPARE( base1->value("user/float", 4.0).toString(), QString("4.56"));

    QValueSpaceItem* base2 = new QValueSpaceItem(QString("/home/user"), this);
    QCOMPARE( base2->value(QByteArray("usercount"), 5).toInt(), 5);
    QCOMPARE( base2->value(QString("QString"), "default").toString(), QString("testString") );
    QCOMPARE( base2->value("bool", false).toBool(), true);
    QCOMPARE( base2->value("int", 5).toInt(), 3);
    QCOMPARE( base2->value("QByteArray", QByteArray("invalid")).toByteArray(), QByteArray("testByteArray"));
    QCOMPARE( base2->value("double", 4.0).toDouble(), 4.56);
    //QCOMPARE( base2->value("float", 4.0).toDouble(), 4.56);
#endif

}

void tst_QValueSpaceItem_oop::ipcTests()
{
#if defined(QT_NO_PROCESS)
    QSKIP("Qt was compiled with QT_NO_PROCESS", SkipAll);
#else
    QValueSpaceItem item ("/usr/lackey/subdir/value");
    ChangeListener* listener = new ChangeListener();
    QSignalSpy spy(listener, SIGNAL(baseChanged()));
    connect(&item, SIGNAL(contentsChanged()),listener, SIGNAL(baseChanged()));

    QProcess process;
    process.setProcessChannelMode(QProcess::ForwardedChannels);
    process.start("vsiTestLackey_oop");
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
#endif
}

void tst_QValueSpaceItem_oop::setValue()
{
#if defined(QT_NO_PROCESS)
    QSKIP("Qt was compiled with QT_NO_PROCESS", SkipAll);
#else
    QValueSpaceObject* object = new QValueSpaceObject("/usr/intern/changeRequests");
    object->setAttribute("value", 500);
    object->setObjectName("object");
    object->sync();
    QValueSpaceObject* rel_object = new QValueSpaceObject("/usr/intern");
    rel_object->setObjectName("rel_object");
    rel_object->sync();


    QValueSpaceItem item("/usr/intern/changeRequests/value");
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

    QValueSpaceItem item2("/usr/intern");
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

    QValueSpaceItem item3("/");
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

    QValueSpaceItem item4("/usr/intern/changeRequests");
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

    delete listener;
    delete rel_listener;
    delete object;
    delete rel_object;
#endif
}

void tst_QValueSpaceItem_oop::ipcSetValue()
{
#if defined(QT_NO_PROCESS)
    QSKIP("Qt was compiled with QT_NO_PROCESS", SkipAll);
#else
    QList<QValueSpaceObject*> objects;
    QList<QSignalSpy*> spies;
    QList<ChangeListener*> listeners;

    objects.append( new QValueSpaceObject("/usr/lackey"));  //actual owner
    objects.append( new QValueSpaceObject("/usr/lackey/")); //happens to use same path
    objects.append( new QValueSpaceObject("/usr") ); //parent path
    objects.append( new QValueSpaceObject("/usr/lackey/unrelated")); //subpath
    objects.at(0)->setAttribute("changeRequests/value", 500);
    objects.at(0)->sync();

    QValueSpaceItem item("/usr/lackey/changeRequests/value");

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
    process.start("vsiTestLackey_oop",QStringList()<< "-ipcSetValue" );
    QVERIFY(process.waitForStarted());

    QTest::qWait(5000);

    if (process.state() == QProcess::NotRunning &&
        process.exitCode() == ERROR_SETVALUE_NOT_SUPPORTED) {
        QSKIP("setValue not supported by underlying layer", SkipSingle);
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
#endif
}

enum Type { Copy, CharStar, String, ByteArray };
Q_DECLARE_METATYPE(Type);
void tst_QValueSpaceItem_oop::interestNotification_data()
{
#if defined(QT_NO_PROCESS)
    QSKIP("Qt was compiled with QT_NO_PROCESS", SkipAll);
#else
    qRegisterMetaType<Type>("Type");

    QTest::addColumn<Type>("type");
    QTest::addColumn<QString>("basePath");
    QTest::addColumn<QString>("objectPath");
    QTest::addColumn<QString>("attribute");

    QTest::newRow("QValueSpaceItem(char *)")
        << CharStar << QString() << "/interestNotification" << "/value";
    QTest::newRow("QValueSpaceItem(QString)")
        << String << QString() << "/interestNotification" << "/value";
    QTest::newRow("QValueSpaceItem(QByteArray)")
        << ByteArray << QString() << "/interestNotification" << "/value";
    QTest::newRow("QValueSpaceItem(QValueSpaceItem)")
        << Copy << QString() << "/interestNotification" << "/value";
    QTest::newRow("QValueSpaceItem(QValueSpaceItem, char *)")
        << CharStar << "/interestNotification" << "subpath" << "/value";
    QTest::newRow("QValueSpaceItem(QValueSpaceItem, QString)")
        << String << "/interestNotification" << "subpath" << "/value";
    QTest::newRow("QValueSpaceItem(QValueSpaceItem, QByteArray)")
        << ByteArray << "/interestNotification" << "subpath" << "/value";
#endif
}

void tst_QValueSpaceItem_oop::interestNotification()
{
#if defined(QT_NO_PROCESS)
    QSKIP("Qt was compiled with QT_NO_PROCESS", SkipAll);
#else
    if (!root->supportsRequests())
        QSKIP("Underlying layer does not support requests.", SkipSingle);

    QFETCH(Type, type);
    QFETCH(QString, basePath);
    QFETCH(QString, objectPath);
    QFETCH(QString, attribute);

    QValueSpaceObject *object;
    if (basePath.isEmpty())
        object = new QValueSpaceObject(objectPath);
    else
        object = new QValueSpaceObject(basePath + '/' + objectPath);

    ChangeListener notificationListener;
    connect(object, SIGNAL(itemNotify(QByteArray,bool)),
            &notificationListener, SIGNAL(itemNotify(QByteArray,bool)));

    QSignalSpy notificationSpy(&notificationListener, SIGNAL(itemNotify(QByteArray,bool)));

    const QString itemPath = objectPath + attribute;

    QValueSpaceItem *baseItem = 0;
    if (type == Copy) {
        baseItem = new QValueSpaceItem(itemPath);

        QTRY_COMPARE(notificationSpy.count(), 1);
        notificationSpy.clear();
    } else if (!basePath.isEmpty()) {
        baseItem = new QValueSpaceItem(basePath);

        QTest::qWait(100);
        QCOMPARE(notificationSpy.count(), 0);
    }

    QValueSpaceItem *item;
    switch (type) {
    case Copy:
        item = new QValueSpaceItem(*baseItem);
        break;
    case CharStar:
        if (baseItem)
            item = new QValueSpaceItem(*baseItem, itemPath.toUtf8().constData());
        else
            item = new QValueSpaceItem(itemPath.toUtf8().constData());
        break;
    case String:
        if (baseItem)
            item = new QValueSpaceItem(*baseItem, itemPath);
        else
            item = new QValueSpaceItem(itemPath);
        break;
    case ByteArray:
        if (baseItem)
            item = new QValueSpaceItem(*baseItem, itemPath.toUtf8());
        else
            item = new QValueSpaceItem(itemPath.toUtf8());
        break;
    default:
        item = 0;
        QFAIL("Invalid type");
    }

    if (type == Copy) {
        // Copies of QValueSpaceItem share the same interest notification.
        QTest::qWait(100);
        QCOMPARE(notificationSpy.count(), 0);
    } else {
        QTRY_COMPARE(notificationSpy.count(), 1);

        QList<QVariant> arguments = notificationSpy.takeFirst();
        QCOMPARE(arguments.at(0).type(), QVariant::ByteArray);
        QCOMPARE(arguments.at(0).toByteArray(), attribute.toUtf8());
        QCOMPARE(arguments.at(1).type(), QVariant::Bool);
        QCOMPARE(arguments.at(1).toBool(), true);
    }

    QCOMPARE(item->value(QString(), 10).toInt(), 10);

    object->setAttribute(attribute, 5);
    object->sync();

    QCOMPARE(item->value(QString(), 10).toInt(), 5);

    notificationSpy.clear();

    delete item;

    if (type == Copy) {
        // Copies of QValueSpaceItem share the same interest notification.
        QTest::qWait(100);
        QCOMPARE(notificationSpy.count(), 0);
    } else {
        QTRY_COMPARE(notificationSpy.count(), 1);

        QList<QVariant> arguments = notificationSpy.takeFirst();
        QCOMPARE(arguments.at(0).type(), QVariant::ByteArray);
        QCOMPARE(arguments.at(0).toByteArray(), attribute.toUtf8());
        QCOMPARE(arguments.at(1).type(), QVariant::Bool);
        QCOMPARE(arguments.at(1).toBool(), false);
    }

    if (baseItem) {
        notificationSpy.clear();
        delete baseItem;

        if (type == Copy) {
            QTRY_COMPARE(notificationSpy.count(), 1);
        } else {
            QTest::qWait(100);
            QCOMPARE(notificationSpy.count(), 0);
        }
    }

    delete object;
#endif
}

void tst_QValueSpaceItem_oop::ipcInterestNotification()
{
#if defined(QT_NO_PROCESS)
    QSKIP("Qt was compiled with QT_NO_PROCESS", SkipAll);
#else
    if (!root->supportsRequests())
        QSKIP("Underlying layer does not support requests.", SkipSingle);

    // Test QValueSpaceItem construction before QValueSpaceObject.

    QValueSpaceItem *item = new QValueSpaceItem("/ipcInterestNotification/value");

    ChangeListener listener;
    QObject::connect(item, SIGNAL(contentsChanged()), &listener, SIGNAL(baseChanged()));
    QSignalSpy changeSpy(&listener, SIGNAL(baseChanged()));

    // Lackey is not running, so value will not exist.
    QCOMPARE(item->value(QString(), 10).toInt(), 10);

    QProcess process;
    process.setProcessChannelMode(QProcess::ForwardedChannels);
    process.start("vsiTestLackey_oop", QStringList() << "-ipcInterestNotification");
    QVERIFY(process.waitForStarted());

    // Lackey will receive itemNotify from server and  set the attribute.
    QTRY_COMPARE(changeSpy.count(), 1);
    changeSpy.clear();

    QCOMPARE(item->value(QString(), 10).toInt(), 5);

    // Lackey will receive itemNotify and remove attribute.
    delete item;


    // Test QValueSpaceItem construction after QValueSpaceObject

    item = new QValueSpaceItem("/ipcInterestNotification/value");
    QObject::connect(item, SIGNAL(contentsChanged()), &listener, SIGNAL(baseChanged()));

    QTRY_COMPARE(changeSpy.count(), 1);

    QCOMPARE(item->value(QString(), 10).toInt(), 5);
#endif
}

class ShutdownControl : public QObject
{
    Q_OBJECT
public:
    ShutdownControl(QProcess* process)
    {
        connect(process,SIGNAL(finished(int,QProcess::ExitStatus)),
               this, SLOT(shutDown(int, QProcess::ExitStatus))); 
    }
private slots:
    void shutDown(int, QProcess::ExitStatus)
    {
        qApp->quit();
    }

};

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    QStringList args = app.arguments();

#if defined(QT_NO_PROCESS)
        tst_QValueSpaceItem_oop test;
        return QTest::qExec(&test, argc-1, argv);
#else
    if (args.contains("-vsClientMode")) {
        tst_QValueSpaceItem_oop test;
        return QTest::qExec(&test, argc-1, argv);
    } else {
        QValueSpace::initValuespaceManager();
        QProcess process;
        ShutdownControl control(&process);
        process.setProcessChannelMode(QProcess::ForwardedChannels);
        args.removeAt(0); //don't pass the binary name
        process.start("tst_qvaluespaceitem_oop", args << "-vsClientMode");
        return app.exec();
    }
#endif
}

//QTEST_MAIN(tst_QValueSpaceItem_oop)
#include "tst_qvaluespaceitem_oop.moc"
