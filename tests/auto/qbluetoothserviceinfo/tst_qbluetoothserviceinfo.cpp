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

#include <qbluetoothdeviceinfo.h>
#include <qbluetoothserviceinfo.h>
#include <qbluetoothaddress.h>

QTM_USE_NAMESPACE

class tst_QBluetoothServiceInfo : public QObject
{
    Q_OBJECT

public:
    tst_QBluetoothServiceInfo();
    ~tst_QBluetoothServiceInfo();

private slots:
    void initTestCase();

    void tst_construction();
    void tst_assignment();
};

tst_QBluetoothServiceInfo::tst_QBluetoothServiceInfo()
{
}

tst_QBluetoothServiceInfo::~tst_QBluetoothServiceInfo()
{
}

void tst_QBluetoothServiceInfo::initTestCase()
{
}

void tst_QBluetoothServiceInfo::tst_construction()
{
    const QString serviceName("My Service");
    const QBluetoothDeviceInfo deviceInfo(QBluetoothAddress("001122334455"), "Test Device", 0);

    {
        QBluetoothServiceInfo serviceInfo;

        QVERIFY(!serviceInfo.isValid());
    }

    {
        QBluetoothServiceInfo serviceInfo;
        serviceInfo.setServiceName(serviceName);
        serviceInfo.setDevice(deviceInfo);

        QVERIFY(serviceInfo.isValid());

        QCOMPARE(serviceInfo.serviceName(), serviceName);
        QCOMPARE(serviceInfo.device().address(), deviceInfo.address());

        QBluetoothServiceInfo copyInfo(serviceInfo);

        QVERIFY(copyInfo.isValid());

        QCOMPARE(copyInfo.serviceName(), serviceName);
        QCOMPARE(copyInfo.device().address(), deviceInfo.address());
    }
}

void tst_QBluetoothServiceInfo::tst_assignment()
{
    const QString serviceName("My Service");
    const QBluetoothDeviceInfo deviceInfo(QBluetoothAddress("001122334455"), "Test Device", 0);

    QBluetoothServiceInfo serviceInfo;
    serviceInfo.setServiceName(serviceName);
    serviceInfo.setDevice(deviceInfo);

    QVERIFY(serviceInfo.isValid());

    {
        QBluetoothServiceInfo copyInfo = serviceInfo;

        QVERIFY(copyInfo.isValid());

        QCOMPARE(copyInfo.serviceName(), serviceName);
        QCOMPARE(copyInfo.device().address(), deviceInfo.address());
    }

    {
        QBluetoothServiceInfo copyInfo;

        QVERIFY(!copyInfo.isValid());

        copyInfo = serviceInfo;

        QVERIFY(copyInfo.isValid());

        QCOMPARE(copyInfo.serviceName(), serviceName);
        QCOMPARE(copyInfo.device().address(), deviceInfo.address());
    }

    {
        QBluetoothServiceInfo copyInfo1;
        QBluetoothServiceInfo copyInfo2;

        QVERIFY(!copyInfo1.isValid());
        QVERIFY(!copyInfo2.isValid());

        copyInfo1 = copyInfo2 = serviceInfo;

        QVERIFY(copyInfo1.isValid());
        QVERIFY(copyInfo2.isValid());

        QCOMPARE(copyInfo1.serviceName(), serviceName);
        QCOMPARE(copyInfo2.serviceName(), serviceName);
        QCOMPARE(copyInfo1.device().address(), deviceInfo.address());
        QCOMPARE(copyInfo2.device().address(), deviceInfo.address());
    }
}

QTEST_MAIN(tst_QBluetoothServiceInfo)

#include "tst_qbluetoothserviceinfo.moc"
