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

#include <qbluetoothaddress.h>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QBluetoothAddress)

class tst_QBluetoothAddress : public QObject
{
    Q_OBJECT

public:
    tst_QBluetoothAddress();
    ~tst_QBluetoothAddress();

private slots:
    void tst_construction_data();
    void tst_construction();

    void tst_assignment();

    void tst_comparison_data();
    void tst_comparison();

    void tst_lessThan_data();
    void tst_lessThan();
};

tst_QBluetoothAddress::tst_QBluetoothAddress()
{
    qRegisterMetaType<QBluetoothAddress>("QBluetoothAddress");
}

tst_QBluetoothAddress::~tst_QBluetoothAddress()
{
}

void tst_QBluetoothAddress::tst_construction_data()
{
    QTest::addColumn<quint64>("addressUInt");
    QTest::addColumn<QString>("addressS12");
    QTest::addColumn<QString>("addressS17");

    QTest::newRow("00:00:00:00:00:00") << Q_UINT64_C(0x000000000000) << QString("000000000000") << QString("00:00:00:00:00:00");
    QTest::newRow("11:22:33:44:55:66") << Q_UINT64_C(0x112233445566) << QString("112233445566") << QString("11:22:33:44:55:66");
    QTest::newRow("AA:BB:CC:DD:EE:FF") << Q_UINT64_C(0xAABBCCDDEEFF) << QString("AABBCCDDEEFF") << QString("AA:BB:CC:DD:EE:FF");
    QTest::newRow("aa:bb:cc:dd:ee:ff") << Q_UINT64_C(0xAABBCCDDEEFF) << QString("aabbccddeeff") << QString("AA:BB:CC:DD:EE:FF");
    QTest::newRow("FF:FF:FF:FF:FF:FF") << Q_UINT64_C(0xFFFFFFFFFFFF) << QString("FFFFFFFFFFFF") << QString("FF:FF:FF:FF:FF:FF");
}

void tst_QBluetoothAddress::tst_construction()
{
    QFETCH(quint64, addressUInt);
    QFETCH(QString, addressS12);
    QFETCH(QString, addressS17);

    {
        QBluetoothAddress address;

        QVERIFY(!address.isValid());
    }

    {
        /* construct from quint64 */
        QBluetoothAddress address(addressUInt);

        QVERIFY(address.isValid());

        QVERIFY(address.toUInt64() == addressUInt);

        QCOMPARE(address.toString(), addressS17);
    }

    {
        /* construct from string without colons */
        QBluetoothAddress address(addressS12);

        QVERIFY(address.isValid());

        QVERIFY(address.toUInt64() == addressUInt);

        QCOMPARE(address.toString(), addressS17);
    }

    {
        /* construct from string with colons */
        QBluetoothAddress address(addressS17);

        QVERIFY(address.isValid());

        QVERIFY(address.toUInt64() == addressUInt);

        QCOMPARE(address.toString(), addressS17);
    }

    {
        QString empty;
        QBluetoothAddress address(empty);

        QVERIFY(!address.isValid());
    }

    {
        QBluetoothAddress address(addressUInt);

        QBluetoothAddress copy(address);

        QVERIFY(address.toUInt64() == copy.toUInt64());
    }
}

void tst_QBluetoothAddress::tst_assignment()
{
    QBluetoothAddress address(Q_UINT64_C(0x112233445566));

    {
        QBluetoothAddress copy = address;

        QCOMPARE(address.toUInt64(), copy.toUInt64());
    }

    {
        QBluetoothAddress copy1;
        QBluetoothAddress copy2;

        QVERIFY(!copy1.isValid());
        QVERIFY(!copy2.isValid());

        copy1 = copy2 = address;

        QVERIFY(copy1.isValid());
        QVERIFY(copy2.isValid());

        QVERIFY(address.toUInt64() == copy1.toUInt64());
        QVERIFY(address.toUInt64() == copy2.toUInt64());
    }
}

void tst_QBluetoothAddress::tst_comparison_data()
{
    QTest::addColumn<QBluetoothAddress>("address1");
    QTest::addColumn<QBluetoothAddress>("address2");
    QTest::addColumn<bool>("result");

    QTest::newRow("invalid == invalid") << QBluetoothAddress() << QBluetoothAddress() << true;
    QTest::newRow("valid != invalid") << QBluetoothAddress(Q_UINT64_C(0x112233445566)) << QBluetoothAddress() << false;
    QTest::newRow("valid == valid") << QBluetoothAddress(Q_UINT64_C(0x112233445566)) << QBluetoothAddress(Q_UINT64_C(0x112233445566)) << true;
}

void tst_QBluetoothAddress::tst_comparison()
{
    QFETCH(QBluetoothAddress, address1);
    QFETCH(QBluetoothAddress, address2);
    QFETCH(bool, result);

    QCOMPARE(address1 == address2, result);
    QCOMPARE(address2 == address1, result);
    QCOMPARE(address1 != address2, !result);
    QCOMPARE(address2 != address1, !result);
}

void tst_QBluetoothAddress::tst_lessThan_data()
{
    QTest::addColumn<QBluetoothAddress>("address1");
    QTest::addColumn<QBluetoothAddress>("address2");
    QTest::addColumn<bool>("result");

    QTest::newRow("invalid < invalid") << QBluetoothAddress() << QBluetoothAddress() << false;
    QTest::newRow("invalid < valid") << QBluetoothAddress() << QBluetoothAddress(Q_UINT64_C(0x112233445566)) << true;
    QTest::newRow("valid < invalid") << QBluetoothAddress(Q_UINT64_C(0x112233445566)) << QBluetoothAddress() << false;
    QTest::newRow("valid < valid") << QBluetoothAddress(Q_UINT64_C(0x112233445566)) << QBluetoothAddress(Q_UINT64_C(0xAABBCCDDEEFF)) << true;
    QTest::newRow("valid < valid") << QBluetoothAddress(Q_UINT64_C(0xAABBCCDDEEFF)) << QBluetoothAddress(Q_UINT64_C(0x112233445566)) << false;
}

void tst_QBluetoothAddress::tst_lessThan()
{
    QFETCH(QBluetoothAddress, address1);
    QFETCH(QBluetoothAddress, address2);
    QFETCH(bool, result);

    QCOMPARE(address1 < address2, result);
}

QTEST_MAIN(tst_QBluetoothAddress)

#include "tst_qbluetoothaddress.moc"

