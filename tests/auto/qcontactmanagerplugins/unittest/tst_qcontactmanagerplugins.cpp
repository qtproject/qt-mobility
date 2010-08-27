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


/* Force a static plugin */
#define QT_STATICPLUGIN

#include <QtTest/QtTest>

#include "qtcontacts.h"

#include <QApplication>
QTM_USE_NAMESPACE

class tst_QContactManagerPlugins : public QObject
{
Q_OBJECT

public:
    tst_QContactManagerPlugins();
    virtual ~tst_QContactManagerPlugins();

public slots:
    void init();
    void cleanup();
private slots:
    void testDummy();
};

/* Test a static factory as well */
class DummyStaticEngineFactory : public QObject, public QContactManagerEngineFactory
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QContactManagerEngineFactory)
    public:
        QContactManagerEngine* engine(const QMap<QString, QString>& parameters, QContactManager::Error* error);
        QString managerName() const {return "teststaticdummy";}
};

QContactManagerEngine* DummyStaticEngineFactory::engine(const QMap<QString, QString>& parameters, QContactManager::Error* error)
{
    Q_UNUSED(parameters);
    *error = QContactManager::LockedError; // random unlikely error
    return 0; // always fail, haha
}

Q_EXPORT_PLUGIN2(contacts_teststaticdummy, DummyStaticEngineFactory)
Q_IMPORT_PLUGIN(contacts_teststaticdummy)

/* And a copy */
Q_EXPORT_PLUGIN2(contacts_teststaticdummycopy, DummyStaticEngineFactory)
Q_IMPORT_PLUGIN(contacts_teststaticdummycopy)

/* And test an impostor as well */
class ImpostorEngineFactory : public QObject, public QContactManagerEngineFactory
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QContactManagerEngineFactory)
    public:
        QContactManagerEngine* engine(const QMap<QString, QString>& , QContactManager::Error* ) {return 0;}
        QString managerName() const {return "memory";}
};

Q_EXPORT_PLUGIN2(contacts_testimpostordummy, ImpostorEngineFactory)
Q_IMPORT_PLUGIN(contacts_testimpostordummy)

/* And test another impostor as well */
class ImpostorEngineFactory2 : public QObject, public QContactManagerEngineFactory
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QContactManagerEngineFactory)
    public:
        QContactManagerEngine* engine(const QMap<QString, QString>& , QContactManager::Error* ) {return 0;}
        QString managerName() const {return "invalid";}
};


Q_EXPORT_PLUGIN2(contacts_testimpostordummy2, ImpostorEngineFactory2)
Q_IMPORT_PLUGIN(contacts_testimpostordummy2)

/* An empty interface name */
class EmptyEngineFactory : public QObject, public QContactManagerEngineFactory
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QContactManagerEngineFactory)
    public:
        QContactManagerEngine* engine(const QMap<QString, QString>& , QContactManager::Error* ) {return 0;}
        QString managerName() const {return QString();}
};

Q_EXPORT_PLUGIN2(contacts_teststaticemptydummy, EmptyEngineFactory)
Q_IMPORT_PLUGIN(contacts_teststaticemptydummy)

/* And a different interface one too */

class BoringInterface
{
    public:
        void doNothing() {}

};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(BoringInterface, "REALLYBORING!")
QT_END_NAMESPACE

class BoringFactory : public QObject, public BoringInterface
{
    Q_OBJECT
    Q_INTERFACES(BoringInterface)
};

Q_EXPORT_PLUGIN2(contacts_testboring, BoringFactory)
Q_IMPORT_PLUGIN(contacts_testboring)


tst_QContactManagerPlugins::tst_QContactManagerPlugins()
{
}

tst_QContactManagerPlugins::~tst_QContactManagerPlugins()
{
}

void tst_QContactManagerPlugins::init()
{
    /* Add a path to our plugin path */
    QString path = QApplication::applicationDirPath() + "/dummyplugin/plugins";
    QApplication::addLibraryPath(path);
    QApplication::addLibraryPath(path); // Test the plugin path deduplication code
    path = QApplication::applicationDirPath() + "/dummyplugin";
    QApplication::addLibraryPath(path);
    QApplication::addLibraryPath("/"); // strictly to test a cdUp :/
}

void tst_QContactManagerPlugins::cleanup()
{
}

void tst_QContactManagerPlugins::testDummy()
{
    QVERIFY(QContactManager::availableManagers().contains("testdummy"));
    QVERIFY(QContactManager::availableManagers().contains("teststaticdummy"));

    QContactManager m1("teststaticdummy"); // should fail
    QVERIFY(m1.managerName() == "invalid");
    QVERIFY(m1.error() == QContactManager::LockedError);

    QContactManager m2("testdummy");
    QVERIFY(m2.managerName() == "testdummy");
}

QTEST_MAIN(tst_QContactManagerPlugins)
#include "tst_qcontactmanagerplugins.moc"
