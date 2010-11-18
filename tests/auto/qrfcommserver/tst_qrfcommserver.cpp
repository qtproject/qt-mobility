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

#include <qrfcommserver.h>
#include <qbluetoothsocket.h>

QTM_USE_NAMESPACE

// Max time to wait for connection
static const int MaxConnectTime = 60 * 1000;   // 1 minute in ms

class tst_QRfcommServer : public QObject
{
    Q_OBJECT

public:
    tst_QRfcommServer();
    ~tst_QRfcommServer();

private slots:
    void initTestCase();

    void tst_construction();

    void tst_listen_data();
    void tst_listen();

    void tst_pendingConnections_data();
    void tst_pendingConnections();

    void tst_receive_data();
    void tst_receive();
};

tst_QRfcommServer::tst_QRfcommServer()
{
}

tst_QRfcommServer::~tst_QRfcommServer()
{
}

void tst_QRfcommServer::initTestCase()
{
}

void tst_QRfcommServer::tst_construction()
{
    {
        QRfcommServer server;

        QVERIFY(!server.isListening());
        QCOMPARE(server.maxPendingConnections(), 1);
        QVERIFY(!server.hasPendingConnections());
        QVERIFY(server.nextPendingConnection() == 0);
        QVERIFY(server.serverAddress().isNull());
        QCOMPARE(server.serverPort(), quint16(0));
    }
}

void tst_QRfcommServer::tst_listen_data()
{
    QTest::addColumn<QBluetoothAddress>("address");
    QTest::addColumn<quint16>("port");

    QTest::newRow("default") << QBluetoothAddress() << quint16(0);
    QTest::newRow("specified address") << QBluetoothAddress("00:11:B1:08:AD:B8") << quint16(0);
    QTest::newRow("specified port") << QBluetoothAddress() << quint16(10);
    QTest::newRow("specified address/port") << QBluetoothAddress("00:11:B1:08:AD:B8") << quint16(10);
}

void tst_QRfcommServer::tst_listen()
{
    QFETCH(QBluetoothAddress, address);
    QFETCH(quint16, port);

    {
        QRfcommServer server;

        bool result = server.listen(address, port);

        QVERIFY(result);
        QVERIFY(server.isListening());

        if (!address.isNull())
            QCOMPARE(server.serverAddress(), address);
        else
            QVERIFY(!server.serverAddress().isNull());

        if (port != 0)
            QCOMPARE(server.serverPort(), port);
        else
            QVERIFY(server.serverPort() != 0);

        QCOMPARE(server.maxPendingConnections(), 1);

        QVERIFY(!server.hasPendingConnections());
        QVERIFY(server.nextPendingConnection() == 0);

        server.close();

        QVERIFY(!server.isListening());

        QVERIFY(server.serverAddress().isNull());
        QVERIFY(server.serverPort() == 0);

        QVERIFY(!server.hasPendingConnections());
        QVERIFY(server.nextPendingConnection() == 0);
    }
}

void tst_QRfcommServer::tst_pendingConnections_data()
{
    QTest::addColumn<int>("maxConnections");

    QTest::newRow("1 connection") << 1;
    //QTest::newRow("2 connections") << 2;
}

void tst_QRfcommServer::tst_pendingConnections()
{
    QFETCH(int, maxConnections);

    {
        QRfcommServer server;

        server.setMaxPendingConnections(maxConnections);

        bool result = server.listen();

        QVERIFY(result);
        QVERIFY(server.isListening());

        qDebug() << "Listening on RFCOMM channel:" << server.serverPort();

        QCOMPARE(server.maxPendingConnections(), maxConnections);

        QVERIFY(!server.hasPendingConnections());
        QVERIFY(server.nextPendingConnection() == 0);

        {
            /* wait for maxConnections simultaneous connections */
            qDebug() << "Waiting for" << maxConnections << "simultaneous connections.";

            QSignalSpy connectionSpy(&server, SIGNAL(newConnection()));

            int connectTime = MaxConnectTime;
            while (connectionSpy.count() < maxConnections && connectTime > 0) {
                QTest::qWait(1000);
                connectTime -= 1000;
            }

            QList<QBluetoothSocket *> sockets;
            while (server.hasPendingConnections())
                sockets.append(server.nextPendingConnection());

            QCOMPARE(connectionSpy.count(), maxConnections);
            QCOMPARE(sockets.count(), maxConnections);

            foreach (QBluetoothSocket *socket, sockets) {
                qDebug() << socket->state();
                QVERIFY(socket->state() == QBluetoothSocket::ConnectedState);
                QVERIFY(socket->openMode() == QIODevice::ReadWrite);
            }

            QVERIFY(!server.hasPendingConnections());
            QVERIFY(server.nextPendingConnection() == 0);

            while (!sockets.isEmpty()) {
                QBluetoothSocket *socket = sockets.takeFirst();
                socket->close();
                delete socket;
            }
        }

        server.close();
    }
}

void tst_QRfcommServer::tst_receive_data()
{
    QTest::addColumn<QByteArray>("expected");

    QTest::newRow("test") << QByteArray("hello\r\n");
}

void tst_QRfcommServer::tst_receive()
{
    QFETCH(QByteArray, expected);

    QRfcommServer server;

    bool result = server.listen();

    QVERIFY(result);

    qDebug() << "Listening on RFCOMM channel:" << server.serverPort();

    int connectTime = MaxConnectTime;
    while (!server.hasPendingConnections() && connectTime > 0) {
        QTest::qWait(1000);
        connectTime -= 1000;
    }

    QVERIFY(server.hasPendingConnections());

    QBluetoothSocket *socket = server.nextPendingConnection();

    QVERIFY(socket->state() == QBluetoothSocket::ConnectedState);
    QVERIFY(socket->openMode() == QIODevice::ReadWrite);

    QSignalSpy readyReadSpy(socket, SIGNAL(readyRead()));

    int readyReadTime = 60000;
    while (readyReadSpy.isEmpty() && readyReadTime > 0) {
        QTest::qWait(1000);
        readyReadTime -= 1000;
    }

    QVERIFY(!readyReadSpy.isEmpty());

    const QByteArray data = socket->readAll();

    QCOMPARE(data, expected);
}

QTEST_MAIN(tst_QRfcommServer)

#include "tst_qrfcommserver.moc"
