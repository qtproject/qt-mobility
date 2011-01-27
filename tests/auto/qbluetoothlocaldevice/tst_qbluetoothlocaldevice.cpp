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

#include <QtTest/QtTest>

#include <QDebug>
#include <QVariant>

#include <qbluetoothaddress.h>
#include <qbluetoothlocaldevice.h>
QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QBluetoothLocalDevice::Pairing)

#define WAIT_FOR_CONDITION(a,e)            \
    for (int _i = 0; _i < 5000; _i += 1) {  \
        if ((a) == (e)) break;             \
        QTest::qWait(100);}

class tst_QBluetoothLocalDevice : public QObject
{
    Q_OBJECT

public:
    tst_QBluetoothLocalDevice();
    ~tst_QBluetoothLocalDevice();

private slots:
    void initTestCase();

    void tst_powerOn();
    void tst_powerOff();
    void tst_hostModes();
    void tst_hostModes_data();
    void tst_address();
    void tst_isValid();
    void tst_allDevices();
    void tst_construction();
    void tst_pairingStatus_data();
    void tst_pairingStatus();
    void tst_pairDevice_data();
    void tst_pairDevice();
};

tst_QBluetoothLocalDevice::tst_QBluetoothLocalDevice()
{
    qRegisterMetaType<QBluetoothLocalDevice::Pairing>("QBluetoothLocalDevice::Pairing");
    qRegisterMetaType<QBluetoothAddress>("QBluetoothAddress");
    // start with host powered off
    QBluetoothLocalDevice *device = new QBluetoothLocalDevice();
    device->setHostMode(QBluetoothLocalDevice::HostPoweredOff);
    delete device;
}

tst_QBluetoothLocalDevice::~tst_QBluetoothLocalDevice()
{
}

void tst_QBluetoothLocalDevice::initTestCase()
{
    qRegisterMetaType<QBluetoothLocalDevice::Pairing>("QBluetoothLocalDevice::Pairing");
}

void tst_QBluetoothLocalDevice::tst_hostModes_data()
{
    QTest::addColumn<QBluetoothLocalDevice::HostMode>("hostModeExpected");

    QTest::newRow("HostDiscoverable") << QBluetoothLocalDevice::HostDiscoverable;
    QTest::newRow("HostPoweredOff") << QBluetoothLocalDevice::HostPoweredOff;
    QTest::newRow("HostConnectable") << QBluetoothLocalDevice::HostConnectable;
    QTest::newRow("HostPoweredOff") << QBluetoothLocalDevice::HostPoweredOff;
    QTest::newRow("HostDiscoverable") << QBluetoothLocalDevice::HostDiscoverable;

}
void tst_QBluetoothLocalDevice::tst_pairDevice_data()
{
    QTest::addColumn<QBluetoothAddress>("deviceAddress");
    QTest::addColumn<QBluetoothLocalDevice::Pairing>("pairingExpected");

    QTest::newRow("UnPaired Device: DUMMY") << QBluetoothAddress("11:00:00:00:00:00") << QBluetoothLocalDevice::Unpaired;
    //QTest::newRow("UnPaired Device: DUMMY") << QBluetoothAddress("11:00:00:00:00:00") << QBluetoothLocalDevice::Paired;
#ifdef Q_OS_SYMBIAN
    //QTest::newRow("UNPAIR Device: BH-604") << QBluetoothAddress("00:0d:3c:b0:77:1c") << QBluetoothLocalDevice::Unpaired;
    QTest::newRow("PAIR Device: BH-604") << QBluetoothAddress("00:0d:3c:b0:77:1c") << QBluetoothLocalDevice::Paired;
#endif // Q_OS_SYMBIAN
}

void tst_QBluetoothLocalDevice::tst_pairingStatus_data()
{
    QTest::addColumn<QBluetoothAddress>("deviceAddress");
    QTest::addColumn<QBluetoothLocalDevice::Pairing>("pairingExpected");

    QTest::newRow("UnPaired Device: DUMMY") << QBluetoothAddress("11:00:00:00:00:00") << QBluetoothLocalDevice::Unpaired;
#ifdef Q_OS_SYMBIAN
    QTest::newRow("Paired Device: BH-604") << QBluetoothAddress("00:0d:3c:b0:77:1c") << QBluetoothLocalDevice::Paired;
#endif // Q_OS_SYMBIAN
}

void tst_QBluetoothLocalDevice::tst_powerOn()
{
    {
    QBluetoothLocalDevice localDevice;

    QSignalSpy hostModeSpy(&localDevice, SIGNAL(hostModeStateChanged(QBluetoothLocalDevice::HostMode)));
    // there should be no changes yet
    QVERIFY(hostModeSpy.isEmpty());

    localDevice.powerOn();
    // async, wait for it
    WAIT_FOR_CONDITION(hostModeSpy.count(),1);
    QVERIFY(hostModeSpy.count() > 0);
    qDebug()<< "Current Host Mode after spyEvent="<<localDevice.hostMode();
    // we should not be powered off
    QVERIFY(localDevice.hostMode() == QBluetoothLocalDevice::HostConnectable
         || localDevice.hostMode() == QBluetoothLocalDevice::HostDiscoverable);
    }

}
void tst_QBluetoothLocalDevice::tst_powerOff()
{
    {
    QBluetoothLocalDevice localDevice;
    localDevice.powerOn();
    }
    QBluetoothLocalDevice localDevice;
    QSignalSpy hostModeSpy(&localDevice, SIGNAL(hostModeStateChanged(QBluetoothLocalDevice::HostMode)));
    // there should be no changes yet
    QVERIFY(hostModeSpy.isEmpty());

    localDevice.setHostMode(QBluetoothLocalDevice::HostPoweredOff);
    // async, wait for it
    WAIT_FOR_CONDITION(hostModeSpy.count(),1);
    QVERIFY(hostModeSpy.count() > 0);
    // we should not be powered off
    QVERIFY(localDevice.hostMode() == QBluetoothLocalDevice::HostPoweredOff);

}

void tst_QBluetoothLocalDevice::tst_hostModes()
{
    QFETCH(QBluetoothLocalDevice::HostMode, hostModeExpected);

    QBluetoothLocalDevice localDevice;
    QSignalSpy hostModeSpy(&localDevice, SIGNAL(hostModeStateChanged(QBluetoothLocalDevice::HostMode)));
    // there should be no changes yet
    QVERIFY(hostModeSpy.isEmpty());

    localDevice.setHostMode(hostModeExpected);
    if (hostModeExpected != localDevice.hostMode()) {
        WAIT_FOR_CONDITION(hostModeSpy.count(),1);
        QVERIFY(hostModeSpy.count() > 0);
    }
    QCOMPARE(hostModeExpected, localDevice.hostMode());
}

void tst_QBluetoothLocalDevice::tst_address()
{
    QBluetoothLocalDevice localDevice;
    QVERIFY(!localDevice.address().toString().isEmpty());
}
void tst_QBluetoothLocalDevice::tst_isValid()
{
    QBluetoothLocalDevice localDevice;
    QVERIFY(localDevice.isValid());
}
void tst_QBluetoothLocalDevice::tst_allDevices()
{
    // we should have one local bluetooth device
    QVERIFY(QBluetoothLocalDevice::allDevices().count() == 1);
}
void tst_QBluetoothLocalDevice::tst_construction()
{
    QBluetoothLocalDevice localDevice;
    QVERIFY(localDevice.isValid());

    QBluetoothLocalDevice anotherDevice = new QBluetoothLocalDevice(QBluetoothAddress(000000000000));
    QVERIFY(anotherDevice.isValid());
    QVERIFY(anotherDevice.address().toUInt64() != 0);

}

void tst_QBluetoothLocalDevice::tst_pairDevice()
{
    QFETCH(QBluetoothAddress, deviceAddress);
    QFETCH(QBluetoothLocalDevice::Pairing, pairingExpected);

    QBluetoothLocalDevice localDevice;
    //powerOn if not already
    localDevice.powerOn();

    QSignalSpy pairingSpy(&localDevice, SIGNAL(pairingFinished(const QBluetoothAddress &,QBluetoothLocalDevice::Pairing)) );
    // there should be no signals yet
    QVERIFY(pairingSpy.isEmpty());

    localDevice.requestPairing(deviceAddress, pairingExpected);
    // async, wait for it
    WAIT_FOR_CONDITION(pairingSpy.count(),1);
    QVERIFY(pairingSpy.count() > 0);
    QCOMPARE(pairingExpected, localDevice.pairingStatus(deviceAddress));

}

void tst_QBluetoothLocalDevice::tst_pairingStatus()
{
    QFETCH(QBluetoothAddress, deviceAddress);
    QFETCH(QBluetoothLocalDevice::Pairing, pairingExpected);

    QBluetoothLocalDevice localDevice;
    QCOMPARE(pairingExpected, localDevice.pairingStatus(deviceAddress));
}
QTEST_MAIN(tst_QBluetoothLocalDevice)

#include "tst_qbluetoothlocaldevice.moc"
