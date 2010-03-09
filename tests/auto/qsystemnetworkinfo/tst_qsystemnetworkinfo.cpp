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
#include "qsysteminfo.h"

QTM_USE_NAMESPACE
Q_DECLARE_METATYPE(QSystemNetworkInfo::NetworkStatus);
Q_DECLARE_METATYPE(QSystemNetworkInfo::NetworkMode);


class tst_QSystemNetworkInfo : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase();
    void tst_networkStatus();

    void tst_networkSignalStrength_data();
    void tst_networkSignalStrength();
    void tst_cellId();
    void tst_locationAreaCode();

    void tst_currentMobileCountryCode(); // Mobile Country Code
    void tst_currentMobileNetworkCode(); // Mobile Network Code

    void tst_homeMobileCountryCode();
    void tst_homeMobileNetworkCode();

    void tst_networkName();

    void tst_macAddress_data();
    void tst_macAddress();

    void tst_interfaceForMode();
    void tst_currentMode();
};
//signal todo:
//    void networkStatusChanged(QSystemNetworkInfo::NetworkMode netmode, QSystemNetworkInfo::CellNetworkStatus netStatus);

void tst_QSystemNetworkInfo::initTestCase()
{
    qRegisterMetaType<QSystemNetworkInfo::NetworkStatus>("QSystemNetworkInfo::NetworkStatus");
    qRegisterMetaType<QSystemNetworkInfo::NetworkMode>("QSystemNetworkInfo::NetworkMode");
}


void tst_QSystemNetworkInfo::tst_networkStatus()
{
    QSystemNetworkInfo ni;
    QList<QSystemNetworkInfo::NetworkMode> modeList;
    modeList << QSystemNetworkInfo::GsmMode;
    modeList << QSystemNetworkInfo::CdmaMode;
    modeList << QSystemNetworkInfo::WcdmaMode;
    modeList << QSystemNetworkInfo::WlanMode;
    modeList << QSystemNetworkInfo::EthernetMode;
    modeList << QSystemNetworkInfo::BluetoothMode;
    modeList << QSystemNetworkInfo::WimaxMode;

    foreach(QSystemNetworkInfo::NetworkMode mode, modeList) {
        QSystemNetworkInfo::NetworkStatus status = ni.networkStatus(mode);

        QVERIFY( status == QSystemNetworkInfo::UndefinedStatus
                 || status == QSystemNetworkInfo::NoNetworkAvailable
                 || status == QSystemNetworkInfo::EmergencyOnly
                 || status == QSystemNetworkInfo::Searching
                 || status == QSystemNetworkInfo::Busy
                 || status == QSystemNetworkInfo::Connected
                 || status == QSystemNetworkInfo::HomeNetwork
                 || status == QSystemNetworkInfo::Denied
                 || status == QSystemNetworkInfo::Roaming);
    }

}

void tst_QSystemNetworkInfo::tst_networkSignalStrength_data()
{
    QTest::addColumn<QSystemNetworkInfo::NetworkMode>("mode");

    QTest::newRow("GsmMode") << QSystemNetworkInfo::GsmMode;
    QTest::newRow("CdmaMode") << QSystemNetworkInfo::CdmaMode;
    QTest::newRow("WcdmaMode") << QSystemNetworkInfo::WcdmaMode;
    QTest::newRow("WlanMode") << QSystemNetworkInfo::WlanMode;
    QTest::newRow("EthernetMode") << QSystemNetworkInfo::EthernetMode;
    QTest::newRow("BluetoothMode") << QSystemNetworkInfo::BluetoothMode;
    QTest::newRow("WimaxMode") << QSystemNetworkInfo::WimaxMode;
}

void tst_QSystemNetworkInfo::tst_networkSignalStrength()
{
    {
        QFETCH(QSystemNetworkInfo::NetworkMode, mode);
        QSystemNetworkInfo ni;
        qint32 strength = ni.networkSignalStrength(mode);
        QVERIFY( strength > -2
                && strength < 101);
    }
}

void  tst_QSystemNetworkInfo::tst_cellId()
{
    QSystemNetworkInfo ni;
    qint32 id = ni.cellId();
    QVERIFY(id > -2);
}

void  tst_QSystemNetworkInfo::tst_locationAreaCode()
{
    QSystemNetworkInfo ni;
    qint32 lac = ni.locationAreaCode();
    QVERIFY(lac > -2);
}


void  tst_QSystemNetworkInfo::tst_currentMobileCountryCode()
{
    QSystemNetworkInfo ni;
    if(QSystemNetworkInfo::Connected == ni.networkStatus(QSystemNetworkInfo::GsmMode)
        || QSystemNetworkInfo::Connected == ni.networkStatus(QSystemNetworkInfo::CdmaMode)
        || QSystemNetworkInfo::Connected == ni.networkStatus(QSystemNetworkInfo::WcdmaMode)) {
        QVERIFY(!ni.currentMobileCountryCode().isEmpty());
    } else {
        QVERIFY(ni.currentMobileCountryCode().isEmpty());
    }
}

void  tst_QSystemNetworkInfo::tst_currentMobileNetworkCode()
{
    QSystemNetworkInfo ni;
    if(QSystemNetworkInfo::Connected == ni.networkStatus(QSystemNetworkInfo::GsmMode)
        || QSystemNetworkInfo::Connected == ni.networkStatus(QSystemNetworkInfo::CdmaMode)
        || QSystemNetworkInfo::Connected == ni.networkStatus(QSystemNetworkInfo::WcdmaMode)) {
        QVERIFY(!ni.currentMobileNetworkCode().isEmpty());
    } else {
        QVERIFY(ni.currentMobileNetworkCode().isEmpty());
    }
}


void  tst_QSystemNetworkInfo::tst_homeMobileCountryCode()
{
    QSystemNetworkInfo ni;
    if(QSystemNetworkInfo::Connected == ni.networkStatus(QSystemNetworkInfo::GsmMode)
        || QSystemNetworkInfo::Connected == ni.networkStatus(QSystemNetworkInfo::CdmaMode)
        || QSystemNetworkInfo::Connected == ni.networkStatus(QSystemNetworkInfo::WcdmaMode)) {
        QVERIFY(!ni.homeMobileCountryCode().isEmpty());
    } else {
        QVERIFY(ni.homeMobileCountryCode().isEmpty());
    }
}

void  tst_QSystemNetworkInfo::tst_homeMobileNetworkCode()
{
    QSystemNetworkInfo ni;
    if(QSystemNetworkInfo::Connected == ni.networkStatus(QSystemNetworkInfo::GsmMode)
        || QSystemNetworkInfo::Connected == ni.networkStatus(QSystemNetworkInfo::CdmaMode)
        || QSystemNetworkInfo::Connected == ni.networkStatus(QSystemNetworkInfo::WcdmaMode)) {
        QVERIFY(!ni.homeMobileNetworkCode().isEmpty());
    } else {
        QVERIFY(ni.homeMobileNetworkCode().isEmpty());
    }
}

void  tst_QSystemNetworkInfo::tst_networkName()
{
    QSystemNetworkInfo ni;
    QList<QSystemNetworkInfo::NetworkMode> modeList;
    modeList << QSystemNetworkInfo::GsmMode;
    modeList << QSystemNetworkInfo::CdmaMode;
    modeList << QSystemNetworkInfo::WcdmaMode;
    modeList << QSystemNetworkInfo::WlanMode;
    modeList << QSystemNetworkInfo::EthernetMode;
    modeList << QSystemNetworkInfo::BluetoothMode;
    modeList << QSystemNetworkInfo::WimaxMode;

    foreach(QSystemNetworkInfo::NetworkMode mode, modeList) {
        QVERIFY(!ni.networkName(mode).isEmpty()
                ||ni.networkName(mode).isEmpty());
    }
}


void tst_QSystemNetworkInfo::tst_macAddress_data()
{
   tst_networkSignalStrength_data();
}

void tst_QSystemNetworkInfo::tst_macAddress()
{
   QFETCH(QSystemNetworkInfo::NetworkMode, mode);
   QSystemNetworkInfo ni;
   QString mac = ni.macAddress(mode);
   if (!mac.isEmpty()) {
      QVERIFY(mac.length() == 17);
      QVERIFY(mac.contains(":"));
   }
}

void tst_QSystemNetworkInfo::tst_interfaceForMode()
{
    QSystemNetworkInfo ni;
    QList<QSystemNetworkInfo::NetworkMode> modeList;
    modeList << QSystemNetworkInfo::GsmMode;
    modeList << QSystemNetworkInfo::CdmaMode;
    modeList << QSystemNetworkInfo::WcdmaMode;
    modeList << QSystemNetworkInfo::WlanMode;
    modeList << QSystemNetworkInfo::EthernetMode;
    modeList << QSystemNetworkInfo::BluetoothMode;
    modeList << QSystemNetworkInfo::WimaxMode;

    foreach(QSystemNetworkInfo::NetworkMode mode, modeList) {
        QVERIFY(!ni.interfaceForMode(mode).name().isEmpty()
            || !ni.interfaceForMode(mode).isValid());
    }

}

void tst_QSystemNetworkInfo::tst_currentMode()
{
    QSystemNetworkInfo ni;
    QSystemNetworkInfo::NetworkMode mode = ni.currentMode();

    QVERIFY( mode == QSystemNetworkInfo::UnknownMode
             || mode == QSystemNetworkInfo::GsmMode
             || mode == QSystemNetworkInfo::CdmaMode
             || mode == QSystemNetworkInfo::WcdmaMode
             || mode == QSystemNetworkInfo::WlanMode
             || mode == QSystemNetworkInfo::EthernetMode
             || mode == QSystemNetworkInfo::BluetoothMode
             || mode == QSystemNetworkInfo::WimaxMode );
}


QTEST_MAIN(tst_QSystemNetworkInfo)
#include "tst_qsystemnetworkinfo.moc"
