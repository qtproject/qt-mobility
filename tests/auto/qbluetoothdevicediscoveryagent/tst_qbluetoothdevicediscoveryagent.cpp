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
#include <qbluetoothdevicediscoveryagent.h>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QBluetoothDeviceInfo)
Q_DECLARE_METATYPE(QBluetoothDeviceDiscoveryAgent::InquiryType)

// Maximum time to for bluetooth device scan
const int MaxScanTime = 5 * 60 * 1000;  // 5 minutes in ms

class tst_QBluetoothDeviceDiscoveryAgent : public QObject
{
    Q_OBJECT

public:
    tst_QBluetoothDeviceDiscoveryAgent();
    ~tst_QBluetoothDeviceDiscoveryAgent();

public slots:
    void deviceDiscoveryDebug(const QBluetoothDeviceInfo &info);

private slots:
    void initTestCase();

    void tst_properties();

    void tst_deviceDiscovery_data();
    void tst_deviceDiscovery();
};

tst_QBluetoothDeviceDiscoveryAgent::tst_QBluetoothDeviceDiscoveryAgent()
{
    qRegisterMetaType<QBluetoothDeviceDiscoveryAgent::Error>("QBluetoothDeviceDiscoveryAgent::Error");
}

tst_QBluetoothDeviceDiscoveryAgent::~tst_QBluetoothDeviceDiscoveryAgent()
{
}

void tst_QBluetoothDeviceDiscoveryAgent::initTestCase()
{
    qRegisterMetaType<QBluetoothDeviceInfo>("QBluetoothDeviceInfo");
    qRegisterMetaType<QBluetoothDeviceDiscoveryAgent::InquiryType>("QBluetoothDeviceDiscoveryAgent::InquiryType");
}

void tst_QBluetoothDeviceDiscoveryAgent::tst_properties()
{
    {
        QBluetoothDeviceDiscoveryAgent discoveryAgent;

        QCOMPARE(discoveryAgent.inquiryType(), QBluetoothDeviceDiscoveryAgent::GeneralUnlimitedInquiry);
        discoveryAgent.setInquiryType(QBluetoothDeviceDiscoveryAgent::LimitedInquiry);
        QCOMPARE(discoveryAgent.inquiryType(), QBluetoothDeviceDiscoveryAgent::LimitedInquiry);
        discoveryAgent.setInquiryType(QBluetoothDeviceDiscoveryAgent::GeneralUnlimitedInquiry);
        QCOMPARE(discoveryAgent.inquiryType(), QBluetoothDeviceDiscoveryAgent::GeneralUnlimitedInquiry);
    }
}

void tst_QBluetoothDeviceDiscoveryAgent::deviceDiscoveryDebug(const QBluetoothDeviceInfo &info)
{
    qDebug() << "Discovered device:" << info.address().toString() << info.name();
}

void tst_QBluetoothDeviceDiscoveryAgent::tst_deviceDiscovery_data()
{
    QTest::addColumn<QBluetoothDeviceDiscoveryAgent::InquiryType>("inquiryType");

    QTest::newRow("general unlimited inquiry") << QBluetoothDeviceDiscoveryAgent::GeneralUnlimitedInquiry;
    QTest::newRow("limited inquiry") << QBluetoothDeviceDiscoveryAgent::LimitedInquiry;
}

void tst_QBluetoothDeviceDiscoveryAgent::tst_deviceDiscovery()
{
    {
        QFETCH(QBluetoothDeviceDiscoveryAgent::InquiryType, inquiryType);

        QBluetoothDeviceDiscoveryAgent discoveryAgent;

        QVERIFY(!discoveryAgent.isActive());

        QVERIFY(discoveryAgent.discoveredDevices().isEmpty());

        QSignalSpy finishedSpy(&discoveryAgent, SIGNAL(finished()));
        QSignalSpy errorSpy(&discoveryAgent, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)));
        QSignalSpy discoveredSpy(&discoveryAgent, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo&)));
        connect(&discoveryAgent, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo&)),
                this, SLOT(deviceDiscoveryDebug(const QBluetoothDeviceInfo&)));

        discoveryAgent.setInquiryType(inquiryType);
        discoveryAgent.start();

        QVERIFY(discoveryAgent.isActive());

        // Wait for up to MaxScanTime for the scan to finish
        int scanTime = MaxScanTime;
        while (finishedSpy.count() == 0 && scanTime > 0) {
            QTest::qWait(1000);
            scanTime -= 1000;
        }
        qDebug() << "Scan time left:" << scanTime;

        // Expect finished signal with no error
        QVERIFY(finishedSpy.count() == 1);
        QVERIFY(errorSpy.isEmpty());

        // All returned QBluetoothDeviceInfo should be valid.
        while (!discoveredSpy.isEmpty()) {
            const QBluetoothDeviceInfo info =
                qvariant_cast<QBluetoothDeviceInfo>(discoveredSpy.takeFirst().at(0));
            QVERIFY(info.isValid());
        }
    }
}

QTEST_MAIN(tst_QBluetoothDeviceDiscoveryAgent)

#include "tst_qbluetoothdevicediscoveryagent.moc"
