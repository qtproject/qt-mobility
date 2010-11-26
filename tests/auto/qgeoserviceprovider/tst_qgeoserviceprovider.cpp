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

//TESTED_COMPONENT=src/location

#include <QtTest/QtTest>
#include <qgeoserviceprovider.h>

Q_IMPORT_PLUGIN( qtgeoservices_staticplugin1)
Q_IMPORT_PLUGIN( qtgeoservices_staticplugin2)

QTM_USE_NAMESPACE

class tst_QGeoServiceProvider: public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void availableServiceProviders();
    void constructStaticPlugins();
    void constructDynamicPlugin();
    void constructInvalidPlugin();
    void loadSearchManager_data();
    void loadSearchManager();
    void loadMappingManager_data();
    void loadMappingManager();
    void loadRoutingManager_data();
    void loadRoutingManager();
};

void tst_QGeoServiceProvider::initTestCase()
{
}

void tst_QGeoServiceProvider::cleanupTestCase()
{
}

void tst_QGeoServiceProvider::init()
{
}

void tst_QGeoServiceProvider::cleanup()
{
}

void tst_QGeoServiceProvider::availableServiceProviders()
{
    QStringList providers = QGeoServiceProvider::availableServiceProviders();
    QVERIFY(providers.count() > 2);
    QVERIFY(providers.contains("dynamic.test.plugin.1"));
    QVERIFY(providers.contains("static.test.plugin.1"));
    QVERIFY(providers.contains("static.test.plugin.2"));
}

void tst_QGeoServiceProvider::constructStaticPlugins()
{
    QGeoServiceProvider* m_serviceProvider = new QGeoServiceProvider("static.test.plugin.1");
    QVERIFY(m_serviceProvider);
    QCOMPARE(m_serviceProvider->error(), QGeoServiceProvider::NoError);
    delete m_serviceProvider;

    m_serviceProvider = new QGeoServiceProvider("static.test.plugin.2");
    QVERIFY(m_serviceProvider);
    QCOMPARE(m_serviceProvider->error(), QGeoServiceProvider::NoError);
    delete m_serviceProvider;
}

void tst_QGeoServiceProvider::constructDynamicPlugin()
{
    QGeoServiceProvider* m_serviceProvider = new QGeoServiceProvider("dynamic.test.plugin.1");
    QVERIFY(m_serviceProvider);
    QCOMPARE(m_serviceProvider->error(), QGeoServiceProvider::NoError);
    delete m_serviceProvider;
}

void tst_QGeoServiceProvider::constructInvalidPlugin()
{
    QGeoServiceProvider* m_serviceProvider = new QGeoServiceProvider(
        "test.to.load.invalid.plugin.that.will.not.be.found");
    QCOMPARE(m_serviceProvider->error(), QGeoServiceProvider::NotSupportedError);
    QVERIFY(m_serviceProvider->errorString().length());
    delete m_serviceProvider;
}

void tst_QGeoServiceProvider::loadSearchManager_data()
{
    QTest::addColumn<QString>("provider");
    QTest::addColumn<bool>("supported");
    QTest::newRow("not supported") << "static.test.plugin.1" << false;
    QTest::newRow("supported") << "static.test.plugin.2" << true;
}

void tst_QGeoServiceProvider::loadSearchManager()
{
    QFETCH(QString, provider);
    QFETCH(bool, supported);

    QGeoServiceProvider* m_serviceProvider = new QGeoServiceProvider(provider);
    if (supported) {
        QVERIFY(m_serviceProvider->searchManager());
    }
    else {
        QVERIFY(!m_serviceProvider->searchManager());
        QCOMPARE(m_serviceProvider->error(), QGeoServiceProvider::NotSupportedError);
        QVERIFY(m_serviceProvider->errorString().length());
    }

    delete m_serviceProvider;
}

void tst_QGeoServiceProvider::loadMappingManager_data()
{
    QTest::addColumn<QString>("provider");
    QTest::addColumn<bool>("supported");
    QTest::newRow("not supported") << "static.test.plugin.1" << false;
    QTest::newRow("supported") << "static.test.plugin.2" << true;
}

void tst_QGeoServiceProvider::loadMappingManager()
{
    QFETCH(QString, provider);
    QFETCH(bool, supported);

    QGeoServiceProvider* m_serviceProvider = new QGeoServiceProvider(provider);
    if (supported) {
        QVERIFY(m_serviceProvider->mappingManager());
    }
    else {
        QVERIFY(!m_serviceProvider->mappingManager());
        QCOMPARE(m_serviceProvider->error(), QGeoServiceProvider::NotSupportedError);
        QVERIFY(m_serviceProvider->errorString().length());
    }

    delete m_serviceProvider;
}

void tst_QGeoServiceProvider::loadRoutingManager_data()
{
    QTest::addColumn<QString>("provider");
    QTest::addColumn<bool>("supported");
    QTest::newRow("not supported") << "static.test.plugin.1" << false;
    QTest::newRow("supported") << "static.test.plugin.2" << true;
}

void tst_QGeoServiceProvider::loadRoutingManager()
{
    QFETCH(QString, provider);
    QFETCH(bool, supported);

    QGeoServiceProvider* m_serviceProvider = new QGeoServiceProvider(provider);
    if (supported) {
        QVERIFY(m_serviceProvider->routingManager());
    }
    else {
        QVERIFY(!m_serviceProvider->routingManager());
        QCOMPARE(m_serviceProvider->error(), QGeoServiceProvider::NotSupportedError);
        QVERIFY(m_serviceProvider->errorString().length());
    }

    delete m_serviceProvider;
}

QTEST_MAIN( tst_QGeoServiceProvider)
#include "tst_qgeoserviceprovider.moc"
