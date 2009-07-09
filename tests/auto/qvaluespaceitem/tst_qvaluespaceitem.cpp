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
#include <QtTest/QtTest>
#include <QtCore>
#include <QVariant>
#include "qvaluespace.h"
#include <unistd.h>

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
};



Q_DECLARE_METATYPE(QValueSpaceItem*)
Q_DECLARE_METATYPE(QVariant)
Q_DECLARE_METATYPE(QList<QString>)

class tst_QValueSpaceItem: public QObject
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
    void value();
};

void tst_QValueSpaceItem::initTestCase()
{
    QValueSpace::initValuespaceManager();

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
}

void tst_QValueSpaceItem::init()
{
}

void tst_QValueSpaceItem::cleanupTestCase()
{
    delete root;
}

void tst_QValueSpaceItem::testConstructor_data()
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
    QTest::newRow("QValueSpaceItem(*baseHome, this)") 
        << data
        << QVariant()
        << (QList<QString>() << "user" << "usercount")
        << QString("/home")
        << QString("user/int")
        << 3;

    item1 = new QValueSpaceItem(*baseHome,QByteArray(), this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseHome, this)") 
        << data
        << QVariant()
        << (QList<QString>() << "user" << "usercount")
        << QString("/home")
        << QString("user/int")
        << 3;

    item1 = new QValueSpaceItem(*baseHome,"", this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseHome, this)") 
        << data
        << QVariant()
        << (QList<QString>() << "user" << "usercount")
        << QString("/home")
        << QString("user/int")
        << 3;

    //root base item + empty subpath
    item1 = new QValueSpaceItem(*baseRoot,QString(), this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseRoot, this)") 
        << data
        << QVariant()
        << (QList<QString>() << "home" << "usr")
        << QString("/")
        << QString("home/user/int")
        << 3;

    item1 = new QValueSpaceItem(*baseRoot,QByteArray(), this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseRoot, this)") 
        << data
        << QVariant()
        << (QList<QString>() << "home" << "usr")
        << QString("/")
        << QString("home/user/int")
        << 3;

    item1 = new QValueSpaceItem(*baseRoot,"", this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseRoot, this)") 
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

    //home base item + "/user" subpath -> should not work
    item1 = new QValueSpaceItem(*baseHome,QString("/user"), this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseHome, QString(\"user\"), this)") 
        << data
        << QVariant()
        << QList<QString>()
        << QString("/home//user")
        << QString("int")
        << 100;

    item1 = new QValueSpaceItem(*baseHome,QByteArray("/user"), this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseHome, QByteArray(\"user\"), this)") 
        << data
        << QVariant()
        << QList<QString>()
        << QString("/home//user")
        << QString("int")
        << 100;

    item1 = new QValueSpaceItem(*baseHome,"/user", this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseHome, \"user\",this)") 
        << data
        << QVariant()
        << QList<QString>()
        << QString("/home//user")
        << QString("int")
        << 100;

    //root base item + "/home" subpath -> should not work
    item1 = new QValueSpaceItem(*baseRoot,QString("/home"), this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseRoot, QString(\"/home\"), this)") 
        << data
        << QVariant()
        << QList<QString>()
        << QString("//home")
        << QString("user/int")
        << 100;

    item1 = new QValueSpaceItem(*baseRoot,QByteArray("/home"), this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseRoot, QByteArray(\"/home\"), this)") 
        << data
        << QVariant()
        << QList<QString>()
        << QString("//home")
        << QString("user/int")
        << 100;

    item1 = new QValueSpaceItem(*baseRoot,"/home", this);
    qVariantSetValue(data, item1);
    QTest::newRow("QValueSpaceItem(*baseRoot, \"home\",this)") 
        << data
        << QVariant()
        << QList<QString>()
        << QString("//home")
        << QString("user/int")
        << 100;
}

void tst_QValueSpaceItem::testConstructor()
{
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
    
    if (itemName == "/")
        QEXPECT_FAIL("", "root based value lookup not working", Continue);
    QCOMPARE(item->value(relItemPath, 100).toInt(), expectedValue);

}

void tst_QValueSpaceItem::testAssignmentOperator()
{
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
}

void tst_QValueSpaceItem::contentsChanged_data()
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

void tst_QValueSpaceItem::contentsChanged()
{
    QFETCH(QString, item_path);
    QFETCH(QString, value_path);
    QFETCH(int, should_emit_signal);
    QFETCH(bool, old_value);
    QFETCH(bool, new_value);

    busy->setAttribute("alex/busy", old_value);
    busy->sync();

    QValueSpaceItem item(item_path);
    if (item.itemName() == "/")
        QEXPECT_FAIL("", "root based value lookup not working", Continue);
    QCOMPARE(item.value(value_path,!old_value).toBool(), old_value);

    ChangeListener* listener = new ChangeListener();
    QSignalSpy spy(listener, SIGNAL(baseChanged()));
    connect(&item, SIGNAL(contentsChanged()),listener, SIGNAL(baseChanged()));
    QCOMPARE(spy.count(), 0);

    busy->setAttribute("alex/busy", new_value);
    busy->sync();

    QTRY_COMPARE(spy.count(), should_emit_signal);
    QCOMPARE(item.value(value_path,!old_value).toBool(), new_value);

    delete listener;
}

void tst_QValueSpaceItem::value()
{
    QValueSpaceItem* base = new QValueSpaceItem(QString("/"), this);
    QEXPECT_FAIL("", "root based item not working", Continue);
    QCOMPARE( base->value("home/usercount", 5).toInt(),1) ;
    QEXPECT_FAIL("", "root based item not working", Continue);
    QCOMPARE( base->value("home/user/QString", "default").toString(), QString("testString") );
    QEXPECT_FAIL("", "root based item not working", Continue);
    QCOMPARE( base->value("home/user/bool", false).toBool(), true);
    QEXPECT_FAIL("", "root based item not working", Continue);
    QCOMPARE( base->value("home/user/int", 5).toInt(), 3);
    QEXPECT_FAIL("", "root based item not working", Continue);
    QCOMPARE( base->value("home/user/QByteArray", QByteArray("invalid")).toByteArray(), QByteArray("testByteArray"));
    QEXPECT_FAIL("", "root based item not working", Continue);
    QCOMPARE( base->value("home/user/double", 4.0).toDouble(), (double)4.56);
    QEXPECT_FAIL("", "root based item not working", Continue);
    QCOMPARE( base->value("home/user/float", 4.0).toDouble(), (double)4.56);

    QValueSpaceItem* base1 = new QValueSpaceItem(QString("/home"), this);
    QCOMPARE( base1->value("usercount",5).toInt(),1);
    QCOMPARE( base1->value("user/QString", "default").toString(), QString("testString") );
    QCOMPARE( base1->value("user/bool", false).toBool(), true);
    QCOMPARE( base1->value("user/int", 5).toInt(), 3);
    QCOMPARE( base1->value("user/QByteArray", QByteArray("invalid")).toByteArray(), QByteArray("testByteArray"));
    QCOMPARE( base1->value("user/double", 4.0).toString(),QString("4.56"));
    //QCOMPARE( base1->value("user/float", 4.0).toString(), QString("4.56"));

    QValueSpaceItem* base2 = new QValueSpaceItem(QString("/home/user"), this);
    QCOMPARE( base2->value("usercount", 5).toInt(), 5);
    QCOMPARE( base2->value("QString", "default").toString(), QString("testString") );
    QCOMPARE( base2->value("bool", false).toBool(), true);
    QCOMPARE( base2->value("int", 5).toInt(), 3);
    QCOMPARE( base2->value("QByteArray", QByteArray("invalid")).toByteArray(), QByteArray("testByteArray"));
    QCOMPARE( base2->value("double", 4.0).toDouble(), 4.56);
    //QCOMPARE( base2->value("float", 4.0).toDouble(), 4.56);


}

QTEST_MAIN(tst_QValueSpaceItem)
#include "tst_qvaluespaceitem.moc"
