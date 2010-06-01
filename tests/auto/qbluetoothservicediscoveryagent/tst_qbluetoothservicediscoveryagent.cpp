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
#include <qbluetoothservicediscoveryagent.h>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QBluetoothDeviceInfo)
Q_DECLARE_METATYPE(QList<QBluetoothUuid>)

// Maximum time to for bluetooth device scan
const int MaxScanTime = 5 * 60 * 1000;  // 5 minutes in ms

class tst_QBluetoothServiceDiscoveryAgent : public QObject
{
    Q_OBJECT

public:
    tst_QBluetoothServiceDiscoveryAgent();
    ~tst_QBluetoothServiceDiscoveryAgent();

public slots:
    void deviceDiscoveryDebug(const QBluetoothDeviceInfo &info);
    void serviceDiscoveryDebug(const QBluetoothServiceInfo &info);

private slots:
    void initTestCase();

    void tst_serviceDiscovery_data();
    void tst_serviceDiscovery();

private:
    QList<QBluetoothDeviceInfo> devices;
};

tst_QBluetoothServiceDiscoveryAgent::tst_QBluetoothServiceDiscoveryAgent()
{
    qRegisterMetaType<QBluetoothDeviceInfo>("QBluetoothDeviceInfo");
    qRegisterMetaType<QBluetoothServiceInfo>("QBluetoothServiceInfo");
    qRegisterMetaType<QList<QBluetoothUuid> >("QList<QBluetoothUuid>");
    qRegisterMetaType<QBluetoothServiceDiscoveryAgent::Error>("QBluetoothServiceDiscoveryAgent::Error");
}

tst_QBluetoothServiceDiscoveryAgent::~tst_QBluetoothServiceDiscoveryAgent()
{
}

void tst_QBluetoothServiceDiscoveryAgent::deviceDiscoveryDebug(const QBluetoothDeviceInfo &info)
{
    qDebug() << "Discovered device:" << info.address().toString() << info.name();
}

void tst_QBluetoothServiceDiscoveryAgent::initTestCase()
{
#if 0
    QBluetoothDeviceDiscoveryAgent discoveryAgent;

    QSignalSpy finishedSpy(&discoveryAgent, SIGNAL(finished()));
    QSignalSpy errorSpy(&discoveryAgent, SIGNAL(error(QBluetoothServiceDiscoveryAgent::Error)));
    QSignalSpy discoveredSpy(&discoveryAgent, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo&)));
    connect(&discoveryAgent, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo&)),
            this, SLOT(deviceDiscoveryDebug(const QBluetoothDeviceInfo&)));

    discoveryAgent.start();

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

    devices = discoveryAgent.discoveredDevices();
#else
    devices.append(QBluetoothDeviceInfo(QBluetoothAddress(Q_UINT64_C(0x001e3a81ba69)), "Yuna", 0));
#endif
}

void tst_QBluetoothServiceDiscoveryAgent::serviceDiscoveryDebug(const QBluetoothServiceInfo &info)
{
    qDebug() << "Discovered service on"
             << info.device().name() << info.device().address().toString();
    qDebug() << "\tService name:" << info.serviceName();
    qDebug() << "\tDescription:"
             << info.attribute(QBluetoothServiceInfo::ServiceDescription).toString();
    qDebug() << "\tProvider:" << info.attribute(QBluetoothServiceInfo::ServiceProvider).toString();
    qDebug() << "\tL2CAP protocol service multiplexer:" << info.protocolServiceMultiplexer();
    qDebug() << "\tRFCOMM server channel:" << info.serverChannel();
}

void tst_QBluetoothServiceDiscoveryAgent::tst_serviceDiscovery_data()
{
    if (devices.isEmpty())
            QSKIP("This test requires an in-range bluetooth device", SkipAll);

    QTest::addColumn<QBluetoothDeviceInfo>("deviceInfo");
    QTest::addColumn<QList<QBluetoothUuid> >("uuidFilter");

    foreach (const QBluetoothDeviceInfo &info, devices) {
        QTest::newRow("default filter") << info << QList<QBluetoothUuid>();
        //QTest::newRow("public browse group") << info << (QList<QBluetoothUuid>() << QBluetoothUuid::PublicBrowseGroup);
        //QTest::newRow("l2cap") << info << (QList<QBluetoothUuid>() << QBluetoothUuid::L2cap);
    }
    QTest::newRow("all devices") << QBluetoothDeviceInfo() << QList<QBluetoothUuid>();
}

static void dumpAttributeVariant(const QVariant &var, const QString indent)
{
    if (!var.isValid()) {
        qDebug("%sEmpty", indent.toLocal8Bit().constData());
        return;
    }

    if (var.userType() == qMetaTypeId<QBluetoothServiceInfo::Sequence>()) {
        qDebug("%sSequence", indent.toLocal8Bit().constData());
        const QBluetoothServiceInfo::Sequence *sequence = static_cast<const QBluetoothServiceInfo::Sequence *>(var.data());
        foreach (const QVariant &v, *sequence)
            dumpAttributeVariant(v, indent + '\t');
    } else if (var.userType() == qMetaTypeId<QBluetoothServiceInfo::Alternative>()) {
        qDebug("%sAlternative", indent.toLocal8Bit().constData());
        const QBluetoothServiceInfo::Alternative *alternative = static_cast<const QBluetoothServiceInfo::Alternative *>(var.data());
        foreach (const QVariant &v, *alternative)
            dumpAttributeVariant(v, indent + '\t');
    } else if (var.userType() == qMetaTypeId<QBluetoothUuid>()) {
        QBluetoothUuid uuid = var.value<QBluetoothUuid>();
        switch (uuid.minimumSize()) {
        case 0:
            qDebug("%suuid NULL", indent.toLocal8Bit().constData());
            break;
        case 2:
            qDebug("%suuid %04x", indent.toLocal8Bit().constData(), uuid.toUInt16());
            break;
        case 4:
            qDebug("%suuid %08x", indent.toLocal8Bit().constData(), uuid.toUInt32());
            break;
        case 16: {
            qDebug("%suuid %s", indent.toLocal8Bit().constData(), QByteArray(reinterpret_cast<const char *>(uuid.toUInt128().data), 16).toHex().constData());
            break;
        }
        default:
            qDebug("%suuid ???", indent.toLocal8Bit().constData());
        }
    } else {
        switch (var.userType()) {
        case QVariant::UInt:
            qDebug("%suint %u", indent.toLocal8Bit().constData(), var.toUInt());
            break;
        case QVariant::Int:
            qDebug("%sint %d", indent.toLocal8Bit().constData(), var.toInt());
            break;
        case QVariant::String:
            qDebug("%sstring %s", indent.toLocal8Bit().constData(), var.toString().toLocal8Bit().constData());
            break;
        case QVariant::Bool:
            qDebug("%sbool %d", indent.toLocal8Bit().constData(), var.toBool());
            break;
        case QVariant::Url:
            qDebug("%surl %s", indent.toLocal8Bit().constData(), var.toUrl().toString().toLocal8Bit().constData());
            break;
        default:
            qDebug("%sunknown", indent.toLocal8Bit().constData());
        }
    }
}

static void dumpServiceInfoAttributes(const QBluetoothServiceInfo &info)
{
    foreach (quint16 id, info.attributes()) {
        dumpAttributeVariant(info.attribute(id), QString("\t"));
    }
}

void tst_QBluetoothServiceDiscoveryAgent::tst_serviceDiscovery()
{
    if (devices.isEmpty())
        QSKIP("This test requires an in-range bluetooth device", SkipAll);

    QFETCH(QBluetoothDeviceInfo, deviceInfo);
    QFETCH(QList<QBluetoothUuid>, uuidFilter);

    qDebug() << "Discovering services of" << deviceInfo.address().toString();

    QBluetoothServiceDiscoveryAgent discoveryAgent(deviceInfo.address());

    QVERIFY(!discoveryAgent.isActive());

    QVERIFY(discoveryAgent.discoveredServices().isEmpty());

    QVERIFY(discoveryAgent.uuidFilter().isEmpty());

    discoveryAgent.setUuidFilter(uuidFilter);

    QVERIFY(discoveryAgent.uuidFilter() == uuidFilter);

    QSignalSpy finishedSpy(&discoveryAgent, SIGNAL(finished()));
    QSignalSpy errorSpy(&discoveryAgent, SIGNAL(error(QBluetoothServiceDiscoveryAgent::Error)));
    QSignalSpy discoveredSpy(&discoveryAgent, SIGNAL(serviceDiscovered(QBluetoothServiceInfo)));
    connect(&discoveryAgent, SIGNAL(serviceDiscovered(QBluetoothServiceInfo)),
            this, SLOT(serviceDiscoveryDebug(QBluetoothServiceInfo)));

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

    // All returned QBluetoothServiceInfo should be valid.
    while (!discoveredSpy.isEmpty()) {
        const QVariant v = discoveredSpy.takeFirst().at(0);

        // Work around limitation in QMetaType and moc.
        // QBluetoothServiceInfo is registered with metatype as QtMobility::QBluetoothServiceInfo
        // moc sees it as the unqualified QBluetoothServiceInfo.
        if (qstrcmp(v.typeName(), "QBluetoothServiceInfo") == 0) {
            const QBluetoothServiceInfo info =
                *reinterpret_cast<const QBluetoothServiceInfo*>(v.constData());

            QVERIFY(info.isValid());

#if 0
            qDebug() << info.device().name() << info.device().address().toString();
            qDebug() << "\tService name:" << info.serviceName();
            if (info.protocolServiceMultiplexer() >= 0)
                qDebug() << "\tL2CAP protocol service multiplexer:" << info.protocolServiceMultiplexer();
            if (info.serverChannel() >= 0)
                qDebug() << "\tRFCOMM server channel:" << info.serverChannel();
            //dumpServiceInfoAttributes(info);
#endif
        }
    }
}

QTEST_MAIN(tst_QBluetoothServiceDiscoveryAgent)

#include "tst_qbluetoothservicediscoveryagent.moc"
