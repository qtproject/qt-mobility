/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QtTest/QtTest>
#include "../qbearertestcommon.h"
#include "qnetworkconfiguration.h"
#include "qnetworkconfigmanager.h"

#ifdef MAEMO
#include <iapconf.h>
#endif

class tst_QNetworkConfigurationManager : public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void allConfigurations();
    void defaultConfiguration();
    void configurationFromIdentifier();

private:
#ifdef MAEMO
    Maemo::IAPConf *iapconf;
    Maemo::IAPConf *gprsiap;
    QProcess *icd_stub;
#endif
};

void tst_QNetworkConfigurationManager::initTestCase()
{
}


void tst_QNetworkConfigurationManager::cleanupTestCase()
{
}

void tst_QNetworkConfigurationManager::init()
{
#ifdef MAEMO
    iapconf = new Maemo::IAPConf("007");
    iapconf->clear();
    iapconf->setValue("ipv4_type", "AUTO");
    iapconf->setValue("wlan_wepkey1", "connt");
    iapconf->setValue("wlan_wepdefkey", 1);
    iapconf->setValue("wlan_ssid", QByteArray("JamesBond"));
    iapconf->setValue("name", "James Bond");
    iapconf->setValue("type", "WLAN_INFRA");

    gprsiap = new Maemo::IAPConf("This-is-GPRS-IAP");
    gprsiap->clear();
    gprsiap->setValue("ask_password", false);
    gprsiap->setValue("gprs_accesspointname", "internet");
    gprsiap->setValue("gprs_password", "");
    gprsiap->setValue("gprs_username", "");
    gprsiap->setValue("ipv4_autodns", true);
    gprsiap->setValue("ipv4_type", "AUTO");
    gprsiap->setValue("sim_imsi", "244070123456789");
    gprsiap->setValue("name", "MI6");
    gprsiap->setValue("type", "GPRS");

    icd_stub = new QProcess(this);
    icd_stub->start("/usr/bin/icd2_stub.py");
    QTest::qWait(1000);

    // Add a known network to scan list that icd2 stub returns
    QProcess dbus_send;
    dbus_send.start("dbus-send --type=method_call --system "
		    "--dest=com.nokia.icd2 /com/nokia/icd2 "
		    "com.nokia.icd2.testing.add_available_network "
		    "string:'' uint32:0 string:'' "
		    "string:WLAN_INFRA uint32:5000011 array:byte:48,48,55");
    dbus_send.waitForFinished();
#endif
}

void tst_QNetworkConfigurationManager::cleanup()
{
#ifdef MAEMO
    iapconf->clear();
    delete iapconf;
    gprsiap->clear();
    delete gprsiap;

    icd_stub->terminate();
    icd_stub->waitForFinished();
#endif
}

void printConfigurationDetails(const QNetworkConfiguration& p)
{
    qDebug() << p.name() <<":  isvalid->" <<p.isValid() << " type->"<< p.type() << 
                " roaming->" << p.roamingAvailable() << "identifier->" << p.identifier() <<
                " purpose->" << p.purpose() << " state->" << p.state();
}

void tst_QNetworkConfigurationManager::allConfigurations()
{
    QNetworkConfigurationManager manager;
    QList<QNetworkConfiguration> preScanConfigs = manager.allConfigurations();

    QSignalSpy spy(&manager, SIGNAL(updateCompleted()));
    manager.updateConfigurations(); //initiate scans
    QTRY_VERIFY(spy.count() == 1); //wait for scan to complete

    QList<QNetworkConfiguration> configs = manager.allConfigurations();

    int all = configs.count();
    qDebug() << "All configurations:" << all;
    QVERIFY(all);
    foreach(QNetworkConfiguration p, configs) {
        QVERIFY(p.isValid());
        printConfigurationDetails(p);
        QVERIFY(p.type() != QNetworkConfiguration::Invalid);
    }

    configs = manager.allConfigurations(QNetworkConfiguration::Undefined);
    int undefined = configs.count();
    QVERIFY(undefined <= all);
    qDebug() << "Undefined configurations:" << undefined;
    foreach( const QNetworkConfiguration p, configs) {
        printConfigurationDetails(p);
        QVERIFY(p.state() & QNetworkConfiguration::Undefined);
        QVERIFY(!(p.state() & QNetworkConfiguration::Defined));
    }

    //get defined configs only (same as all)
    configs = manager.allConfigurations(QNetworkConfiguration::Defined);
    int defined = configs.count();
    qDebug() << "Defined configurations:" << defined;
    QVERIFY(defined <= all);
    foreach( const QNetworkConfiguration p, configs) {
        printConfigurationDetails(p);
        QVERIFY(p.state() & QNetworkConfiguration::Defined);
        QVERIFY(!(p.state() & QNetworkConfiguration::Undefined));
    }

    //get discovered configurations only
    configs = manager.allConfigurations(QNetworkConfiguration::Discovered);
    int discovered = configs.count();
    //QVERIFY(discovered);
    qDebug() << "Discovered configurations:" << discovered;
    foreach(const QNetworkConfiguration p, configs) {
        printConfigurationDetails(p);
        QVERIFY(p.isValid());
        QVERIFY(!(p.state() & QNetworkConfiguration::Undefined));
        QVERIFY(p.state() & QNetworkConfiguration::Defined);
        QVERIFY(p.state() & QNetworkConfiguration::Discovered);
    }

    //getactive configurations only
    configs = manager.allConfigurations(QNetworkConfiguration::Active);
    int active = configs.count();
    //QVERIFY(active);
    qDebug() << "Active configurations:" << active;
    foreach(const QNetworkConfiguration p, configs) {
        printConfigurationDetails(p);
        QVERIFY(p.isValid());
        QVERIFY(!(p.state() & QNetworkConfiguration::Undefined));
        QVERIFY(p.state() & QNetworkConfiguration::Active);
        QVERIFY(p.state() & QNetworkConfiguration::Discovered);
        QVERIFY(p.state() & QNetworkConfiguration::Defined);
    }

    QVERIFY(all >= discovered);
    QVERIFY(discovered >= active);
}


void tst_QNetworkConfigurationManager::defaultConfiguration()
{
    QNetworkConfigurationManager manager;
    QList<QNetworkConfiguration> configs = manager.allConfigurations();
    QNetworkConfiguration defaultConfig = manager.defaultConfiguration();

    bool confirm = configs.contains(defaultConfig);
#ifdef MAEMO
    confirm = !confirm; // In maemo the default configuration will never be in allConfiguration because it is a pseudo config
#endif
    QVERIFY(confirm || !defaultConfig.isValid());
    QVERIFY(!(confirm && !defaultConfig.isValid()));
}

void tst_QNetworkConfigurationManager::configurationFromIdentifier()
{
    QNetworkConfigurationManager manager;
    QSet<QString> allIdentifier;
    QList<QNetworkConfiguration> configs = manager.allConfigurations();
    foreach(QNetworkConfiguration c, configs) {
        QVERIFY(!allIdentifier.contains(c.identifier()));
        allIdentifier.insert(c.identifier());

        QNetworkConfiguration direct = manager.configurationFromIdentifier(c.identifier());
        QVERIFY(direct.isValid());
        QVERIFY(direct == c);
    }

    //assume that there is no item with identifier 'FooBar'
    QVERIFY(!allIdentifier.contains("FooBar"));
    QNetworkConfiguration invalid = manager.configurationFromIdentifier("FooBar");
    QVERIFY(!invalid.isValid());
}


QTEST_MAIN(tst_QNetworkConfigurationManager)
#include "tst_qnetworkconfigmanager.moc"
