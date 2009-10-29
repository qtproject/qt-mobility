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

#include <QtTest/QtTest>
#include "../qbearertestcommon.h"
#include "qnetworkconfiguration.h"
#include "qnetworkconfigmanager.h"

class tst_QNetworkConfiguration : public QObject
{
    Q_OBJECT

private slots:
    void invalidPoint();
    void comparison();
    void children();
    void roamingAvailable();
};

void tst_QNetworkConfiguration::invalidPoint()
{
    QNetworkConfiguration pt;

    QVERIFY(pt.name().isEmpty());
    QVERIFY(!pt.isValid());
    QVERIFY(pt.type() == QNetworkConfiguration::Invalid);
    QVERIFY(!(pt.state() & QNetworkConfiguration::Defined));
    QVERIFY(!(pt.state() & QNetworkConfiguration::Discovered));
    QVERIFY(!(pt.state() & QNetworkConfiguration::Active));
    QVERIFY(!pt.roamingAvailable());

    QNetworkConfiguration pt2(pt);
    QVERIFY(pt2.name().isEmpty());
    QVERIFY(!pt2.isValid());
    QVERIFY(pt2.type() == QNetworkConfiguration::Invalid);
    QVERIFY(!(pt2.state() & QNetworkConfiguration::Defined));
    QVERIFY(!(pt2.state() & QNetworkConfiguration::Discovered));
    QVERIFY(!(pt2.state() & QNetworkConfiguration::Active));
    QVERIFY(!pt2.roamingAvailable());

}

void tst_QNetworkConfiguration::comparison()
{
    //test copy constructor and assignment operator
    //compare invalid connection points 
    QNetworkConfiguration pt1;
    QVERIFY(!pt1.isValid());
    QVERIFY(pt1.type() == QNetworkConfiguration::Invalid);

    QNetworkConfiguration pt2(pt1);
    QVERIFY(pt1==pt2);
    QVERIFY(!(pt1!=pt2));
    QVERIFY(pt1.name() == pt2.name());
    QVERIFY(pt1.isValid() == pt2.isValid());
    QVERIFY(pt1.type() == pt2.type());
    QVERIFY(pt1.state() == pt2.state());
    QVERIFY(pt1.purpose() == pt2.purpose());

    
    QNetworkConfiguration pt3;
    pt3 = pt1;
    QVERIFY(pt1==pt3);
    QVERIFY(!(pt1!=pt3));
    QVERIFY(pt1.name() == pt3.name());
    QVERIFY(pt1.isValid() == pt3.isValid());
    QVERIFY(pt1.type() == pt3.type());
    QVERIFY(pt1.state() == pt3.state());
    QVERIFY(pt1.purpose() == pt3.purpose());

    //test case must run on machine that has valid connection points
    QNetworkConfigurationManager manager;
    QList<QNetworkConfiguration> preScanConfigs = manager.allConfigurations();

    QSignalSpy spy(&manager, SIGNAL(updateCompleted()));
    manager.updateConfigurations(); //initiate scans
    QTRY_VERIFY(spy.count() == 1); //wait for scan to complete

    QList<QNetworkConfiguration> configs = manager.allConfigurations(QNetworkConfiguration::Discovered);
    QVERIFY(configs.count());
    QNetworkConfiguration defaultConfig = manager.defaultConfiguration();
    QVERIFY(defaultConfig.isValid());
    QVERIFY(defaultConfig.type() != QNetworkConfiguration::Invalid);
    QVERIFY(!defaultConfig.name().isEmpty());

    pt3 = defaultConfig;
    QVERIFY(defaultConfig==pt3);
    QVERIFY(!(defaultConfig!=pt3));
    QVERIFY(defaultConfig.name() == pt3.name());
    QVERIFY(defaultConfig.isValid() == pt3.isValid());
    QVERIFY(defaultConfig.type() == pt3.type());
    QVERIFY(defaultConfig.state() == pt3.state());
    QVERIFY(defaultConfig.purpose() == pt3.purpose());
}

void tst_QNetworkConfiguration::children()
{
    QNetworkConfigurationManager manager;
    QList<QNetworkConfiguration> configs = manager.allConfigurations();

    foreach(QNetworkConfiguration c, configs)
    {
        if ( c.type() == QNetworkConfiguration::ServiceNetwork ) {
            qDebug() << "found service network" << c.name() << c.children().count();
            QVERIFY(c.isValid());
            QList<QNetworkConfiguration> members = c.children();
            foreach(QNetworkConfiguration child, members) {
                QVERIFY(child.isValid());
                QVERIFY(configs.contains(child));
                qDebug() << "\t" << child.name();
            }
        }
    }
}

void tst_QNetworkConfiguration::roamingAvailable()
{
    QNetworkConfigurationManager manager;
    QList<QNetworkConfiguration> configs = manager.allConfigurations();
    
    //force update to get maximum list
    QSignalSpy spy(&manager, SIGNAL(updateCompleted()));
    manager.updateConfigurations(); //initiate scans
    QTRY_VERIFY(spy.count() == 1); //wait for scan to complete
    
    foreach(QNetworkConfiguration c, configs)
    {
        QVERIFY(QNetworkConfiguration::UserChoice != c.type());
        QVERIFY(QNetworkConfiguration::Invalid != c.type());
        if ( c.type() == QNetworkConfiguration::ServiceNetwork ) {
            //cannot test flag as some SNAPs may not support roaming anyway
            //QVERIFY(c.roamingavailable())
            if ( c.children().count() <= 1 )
                QVERIFY(!c.roamingAvailable());
            foreach(QNetworkConfiguration child, c.children()) {
                QVERIFY(QNetworkConfiguration::InternetAccessPoint == child.type());
                QCOMPARE(child.children().count(), 0);
            }
        } else {
            QVERIFY(!c.roamingAvailable());
        }
    }
}

QTEST_MAIN(tst_QNetworkConfiguration)
#include "tst_qnetworkconfiguration.moc"
