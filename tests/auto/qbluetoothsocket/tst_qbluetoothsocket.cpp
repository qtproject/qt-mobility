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

#include <qbluetoothsocket.h>
#include <qbluetoothdeviceinfo.h>
#include <qbluetoothserviceinfo.h>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QBluetoothAddress)
Q_DECLARE_METATYPE(QBluetoothSocket::SocketState)
Q_DECLARE_METATYPE(QBluetoothSocket::SocketType)

// Max time to wait for connection
static const int MaxConnectTime = 60 * 1000;   // 1 minute in ms
static const int MaxReadWriteTime = 60 * 1000; // 1 minute in ms

class tst_QBluetoothSocket : public QObject
{
    Q_OBJECT

public:
    enum ClientConnectionShutdown {
        Error,
        Disconnect,
        Close,
        Abort,
    };

    tst_QBluetoothSocket();
    ~tst_QBluetoothSocket();

private slots:
    void initTestCase();

    void tst_construction_data();
    void tst_construction();

    void tst_clientConnection_data();
    void tst_clientConnection();

    void tst_serviceConnection_data();
    void tst_serviceConnection();

    void tst_clientCommunication_data();
    void tst_clientCommunication();

    void tst_localPeer_data();
    void tst_localPeer();
};

Q_DECLARE_METATYPE(tst_QBluetoothSocket::ClientConnectionShutdown)

tst_QBluetoothSocket::tst_QBluetoothSocket()
{
    qRegisterMetaType<QBluetoothSocket::SocketState>("QBluetoothSocket::SocketState");
    qRegisterMetaType<QBluetoothSocket::SocketError>("QBluetoothSocket::SocketError");
}

tst_QBluetoothSocket::~tst_QBluetoothSocket()
{
}

void tst_QBluetoothSocket::initTestCase()
{
}

void tst_QBluetoothSocket::tst_construction_data()
{
    QTest::addColumn<QBluetoothSocket::SocketType>("socketType");

    QTest::newRow("unknown socket") << QBluetoothSocket::UnknownSocketType;
    QTest::newRow("rfcomm socket") << QBluetoothSocket::RfcommSocket;
    QTest::newRow("l2cap socket") << QBluetoothSocket::L2CapSocket;
}

void tst_QBluetoothSocket::tst_construction()
{
    QFETCH(QBluetoothSocket::SocketType, socketType);

    {
        QBluetoothSocket socket;

        QCOMPARE(socket.socketType(), QBluetoothSocket::UnknownSocketType);
    }

    {
        QBluetoothSocket socket(socketType);

        QCOMPARE(socket.socketType(), socketType);
    }
}

void tst_QBluetoothSocket::tst_clientConnection_data()
{
    QTest::addColumn<ClientConnectionShutdown>("shutdown");
    QTest::addColumn<QBluetoothAddress>("address");
    QTest::addColumn<quint16>("port");
    QTest::addColumn<QByteArray>("data");

    QBluetoothAddress address("001167602023");
    quint16 port = 10;

    QTest::newRow("unavailable, error") << Error << QBluetoothAddress("112233445566") << quint16(10) << QByteArray();
    QTest::newRow("available, disconnect") << Disconnect << address << port << QByteArray();
    QTest::newRow("available, disconnect with data") << Disconnect << address << port << QByteArray("Test message\n");
    QTest::newRow("available, close") << Close << address << port << QByteArray();
    QTest::newRow("available, abort") << Abort << address << port << QByteArray();
    QTest::newRow("available, abort with data") << Abort << address << port << QByteArray("Test message\n");
}

void tst_QBluetoothSocket::tst_clientConnection()
{
    QFETCH(ClientConnectionShutdown, shutdown);
    QFETCH(QBluetoothAddress, address);
    QFETCH(quint16, port);
    QFETCH(QByteArray, data);

    /* Construction */
    QBluetoothSocket *socket = new QBluetoothSocket(QBluetoothSocket::RfcommSocket);

    QSignalSpy stateSpy(socket, SIGNAL(stateChanged(QBluetoothSocket::SocketState)));

    QCOMPARE(socket->socketType(), QBluetoothSocket::RfcommSocket);
    QCOMPARE(socket->state(), QBluetoothSocket::UnconnectedState);

    /* Connection */
    QSignalSpy connectedSpy(socket, SIGNAL(connected()));
    QSignalSpy errorSpy(socket, SIGNAL(error(QBluetoothSocket::SocketError)));

    socket->connectToService(address, port);

    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(stateSpy.takeFirst().at(0).value<QBluetoothSocket::SocketState>(), QBluetoothSocket::ConnectingState);
    QCOMPARE(socket->state(), QBluetoothSocket::ConnectingState);

    stateSpy.clear();

    int connectTime = MaxConnectTime;
    while (connectedSpy.count() == 0 && errorSpy.count() == 0 && connectTime > 0) {
        QTest::qWait(1000);
        connectTime -= 1000;
    }

    if (shutdown == Error) {
        QCOMPARE(connectedSpy.count(), 0);
        QCOMPARE(errorSpy.count(), 1);
        QCOMPARE(stateSpy.count(), 1);
        QCOMPARE(stateSpy.takeFirst().at(0).value<QBluetoothSocket::SocketState>(), QBluetoothSocket::UnconnectedState);
        QCOMPARE(socket->state(), QBluetoothSocket::UnconnectedState);
        return;
    } else {
        if (errorSpy.count() != 0) {
            qDebug() << errorSpy.takeFirst().at(0).toInt();
            QSKIP("Connection error", SkipSingle);
        }
        QCOMPARE(connectedSpy.count(), 1);
        QCOMPARE(stateSpy.count(), 1);
        QCOMPARE(stateSpy.takeFirst().at(0).value<QBluetoothSocket::SocketState>(), QBluetoothSocket::ConnectedState);
        QCOMPARE(socket->state(), QBluetoothSocket::ConnectedState);
    }

    QVERIFY(shutdown != Error);

    stateSpy.clear();

    /* Read / Write */
    QSignalSpy bytesWrittenSpy(socket, SIGNAL(bytesWritten(qint64)));

    if (!data.isEmpty()) {
        // Write data but don't flush.
        socket->write(data);
    }

    /* Disconnection */
    QSignalSpy disconnectedSpy(socket, SIGNAL(disconnected()));

    if (shutdown == Abort) {
        socket->abort();

        if (!data.isEmpty()) {
            // Check that pending write did not complete.
            QCOMPARE(bytesWrittenSpy.count(), 0);
        }

        QCOMPARE(disconnectedSpy.count(), 1);
        QCOMPARE(stateSpy.count(), 1);
        QCOMPARE(stateSpy.takeFirst().at(0).value<QBluetoothSocket::SocketState>(), QBluetoothSocket::UnconnectedState);
        QCOMPARE(socket->state(), QBluetoothSocket::UnconnectedState);
    } else {
        if (shutdown == Disconnect)
            socket->disconnectFromService();
        else if (shutdown == Close)
            socket->close();

        QCOMPARE(stateSpy.count(), 1);
        QCOMPARE(stateSpy.takeFirst().at(0).value<QBluetoothSocket::SocketState>(), QBluetoothSocket::ClosingState);
        QCOMPARE(socket->state(), QBluetoothSocket::ClosingState);

        int disconnectTime = MaxConnectTime;
        while (disconnectedSpy.count() == 0 && disconnectTime > 0) {
            QTest::qWait(1000);
            disconnectTime -= 1000;
        }

        if (!data.isEmpty()) {
            // Check that pending write completed.
            QCOMPARE(bytesWrittenSpy.count(), 1);
            QCOMPARE(bytesWrittenSpy.at(0).at(0).toLongLong(), qint64(data.length()));
        }

        QCOMPARE(disconnectedSpy.count(), 1);
        QCOMPARE(stateSpy.count(), 1);
        QCOMPARE(stateSpy.takeFirst().at(0).value<QBluetoothSocket::SocketState>(), QBluetoothSocket::UnconnectedState);
    }

    delete socket;

    // The remote service needs time to close the connection and resume listening
    QTest::qSleep(100);
}

void tst_QBluetoothSocket::tst_serviceConnection_data()
{
    QTest::addColumn<QBluetoothServiceInfo>("serviceInfo");

    QBluetoothServiceInfo serviceInfo;
    serviceInfo.setDevice(QBluetoothDeviceInfo(QBluetoothAddress("001167602023"), QString(), 0));
    QBluetoothServiceInfo::Sequence protocolDescriptorList;
    QBluetoothServiceInfo::Sequence protocol;
    protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::L2cap));
    protocolDescriptorList.append(QVariant::fromValue(protocol));
    protocol.clear();
    protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::Rfcomm)) << QVariant::fromValue(quint8(10));
    protocolDescriptorList.append(QVariant::fromValue(protocol));
    serviceInfo.setAttribute(QBluetoothServiceInfo::ProtocolDescriptorList, protocolDescriptorList);

    QTest::newRow("service connection") << serviceInfo;
}

void tst_QBluetoothSocket::tst_serviceConnection()
{
    QFETCH(QBluetoothServiceInfo, serviceInfo);

    /* Construction */
    QBluetoothSocket *socket = new QBluetoothSocket;

    QSignalSpy stateSpy(socket, SIGNAL(stateChanged(QBluetoothSocket::SocketState)));

    QCOMPARE(socket->socketType(), QBluetoothSocket::UnknownSocketType);
    QCOMPARE(socket->state(), QBluetoothSocket::UnconnectedState);

    /* Connection */
    QSignalSpy connectedSpy(socket, SIGNAL(connected()));
    QSignalSpy errorSpy(socket, SIGNAL(error(QBluetoothSocket::SocketError)));

    socket->connectToService(serviceInfo);

    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(stateSpy.takeFirst().at(0).value<QBluetoothSocket::SocketState>(), QBluetoothSocket::ConnectingState);
    QCOMPARE(socket->state(), QBluetoothSocket::ConnectingState);

    stateSpy.clear();

    int connectTime = MaxConnectTime;
    while (connectedSpy.count() == 0 && errorSpy.count() == 0 && connectTime > 0) {
        QTest::qWait(1000);
        connectTime -= 1000;
    }

    if (errorSpy.count() != 0) {
        qDebug() << errorSpy.takeFirst().at(0).toInt();
        QSKIP("Connection error", SkipSingle);
    }
    QCOMPARE(connectedSpy.count(), 1);
    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(stateSpy.takeFirst().at(0).value<QBluetoothSocket::SocketState>(), QBluetoothSocket::ConnectedState);
    QCOMPARE(socket->state(), QBluetoothSocket::ConnectedState);

    stateSpy.clear();

    /* Disconnection */
    QSignalSpy disconnectedSpy(socket, SIGNAL(disconnected()));

    socket->disconnectFromService();

    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(stateSpy.takeFirst().at(0).value<QBluetoothSocket::SocketState>(), QBluetoothSocket::ClosingState);
    QCOMPARE(socket->state(), QBluetoothSocket::ClosingState);

    int disconnectTime = MaxConnectTime;
    while (disconnectedSpy.count() == 0 && disconnectTime > 0) {
        QTest::qWait(1000);
        disconnectTime -= 1000;
    }

    QCOMPARE(disconnectedSpy.count(), 1);
    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(stateSpy.takeFirst().at(0).value<QBluetoothSocket::SocketState>(), QBluetoothSocket::UnconnectedState);

    delete socket;

    // The remote service needs time to close the connection and resume listening
    QTest::qSleep(100);
}

void tst_QBluetoothSocket::tst_clientCommunication_data()
{
    QTest::addColumn<QStringList>("data");

    {
        QStringList data;
        data << QLatin1String("Test line one.\n");
        data << QLatin1String("Test line two, with longer data.\n");

        QTest::newRow("two line test") << data;
    }
}

void tst_QBluetoothSocket::tst_clientCommunication()
{
    QFETCH(QStringList, data);

    /* Construction */
    QBluetoothSocket *socket = new QBluetoothSocket(QBluetoothSocket::RfcommSocket);

    QSignalSpy stateSpy(socket, SIGNAL(stateChanged(QBluetoothSocket::SocketState)));

    QCOMPARE(socket->socketType(), QBluetoothSocket::RfcommSocket);
    QCOMPARE(socket->state(), QBluetoothSocket::UnconnectedState);

    /* Connection */
    QSignalSpy connectedSpy(socket, SIGNAL(connected()));

    socket->connectToService(QBluetoothAddress("001167602023"), 10);    // echo service running on device 00:11:67:60:20:23

    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(qvariant_cast<QBluetoothSocket::SocketState>(stateSpy.takeFirst().at(0)), QBluetoothSocket::ConnectingState);
    QCOMPARE(socket->state(), QBluetoothSocket::ConnectingState);

    stateSpy.clear();

    int connectTime = MaxConnectTime;
    while (connectedSpy.count() == 0 && connectTime > 0) {
        QTest::qWait(1000);
        connectTime -= 1000;
    }

    QCOMPARE(connectedSpy.count(), 1);
    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(qvariant_cast<QBluetoothSocket::SocketState>(stateSpy.takeFirst().at(0)), QBluetoothSocket::ConnectedState);
    QCOMPARE(socket->state(), QBluetoothSocket::ConnectedState);

    stateSpy.clear();

    /* Read / Write */
    QCOMPARE(socket->bytesToWrite(), qint64(0));
    QCOMPARE(socket->bytesAvailable(), qint64(0));

    {
        /* Send line by line with event loop */
        foreach (const QString &line, data) {
            QSignalSpy readyReadSpy(socket, SIGNAL(readyRead()));
            QSignalSpy bytesWrittenSpy(socket, SIGNAL(bytesWritten(qint64)));

            socket->write(line.toUtf8());

            QCOMPARE(socket->bytesToWrite(), qint64(line.length()));

            int readWriteTime = MaxReadWriteTime;
            while ((bytesWrittenSpy.count() == 0 || readyReadSpy.count() == 0) && readWriteTime > 0) {
                QTest::qWait(1000);
                readWriteTime -= 1000;
            }

            QCOMPARE(bytesWrittenSpy.count(), 1);
            QCOMPARE(bytesWrittenSpy.at(0).at(0).toLongLong(), qint64(line.length()));
            QCOMPARE(readyReadSpy.count(), 1);

            QCOMPARE(socket->bytesAvailable(), qint64(line.length()));

            QVERIFY(socket->canReadLine());

            QByteArray echoed = socket->readAll();

            QCOMPARE(line.toUtf8(), echoed);
        }
    }

#if 0
    {
        /* Send line by line without event loop */
        QSignalSpy readyReadSpy(socket, SIGNAL(readyRead()));
        QSignalSpy bytesWrittenSpy(socket, SIGNAL(bytesWritten(qint64)));

        qint64 bytesToWrite = 0;
        foreach (const QString &line, data) {
            socket->write(line.toUtf8());
            bytesToWrite += line.toUtf8().length();
            QCOMPARE(socket->bytesToWrite(), bytesToWrite);
        }

        int readWriteTime = MaxReadWriteTime;
        while (socket->bytesToWrite() != 0 && readyReadSpy.count() == 0 && readWriteTime > 0) {
            QTest::qWait(1000);
            readWriteTime -= 1000;
        }

        QCOMPARE(bytesWrittenSpy.count(), 1);
        QCOMPARE(bytesWrittenSpy.at(0).at(0).toLongLong(), bytesToWrite);
        QCOMPARE(readyReadSpy.count(), 1);

        QCOMPARE(socket->bytesAvailable(), bytesToWrite);

        QVERIFY(socket->canReadLine());

        QByteArray echoed = socket->readAll();

        QCOMPARE(data.join(QString()).toUtf8(), echoed);
    }
#endif

    {
        /* Send all at once */
        QSignalSpy readyReadSpy(socket, SIGNAL(readyRead()));
        QSignalSpy bytesWrittenSpy(socket, SIGNAL(bytesWritten(qint64)));

        QString joined = data.join(QString());
        socket->write(joined.toUtf8());

        QCOMPARE(socket->bytesToWrite(), qint64(joined.length()));

        int readWriteTime = MaxReadWriteTime;
        while ((bytesWrittenSpy.count() == 0 || readyReadSpy.count() == 0) && readWriteTime > 0) {
            QTest::qWait(1000);
            readWriteTime -= 1000;
        }

        QCOMPARE(bytesWrittenSpy.count(), 1);
        QCOMPARE(bytesWrittenSpy.at(0).at(0).toLongLong(), qint64(joined.length()));
        QCOMPARE(readyReadSpy.count(), 1);

        QCOMPARE(socket->bytesAvailable(), qint64(joined.length()));

        QVERIFY(socket->canReadLine());

        QByteArray echoed = socket->readAll();

        QCOMPARE(joined.toUtf8(), echoed);
    }

    /* Disconnection */
    QSignalSpy disconnectedSpy(socket, SIGNAL(disconnected()));

    socket->disconnectFromService();

    int disconnectTime = MaxConnectTime;
    while (disconnectedSpy.count() == 0 && disconnectTime > 0) {
        QTest::qWait(1000);
        disconnectTime -= 1000;
    }

    QCOMPARE(disconnectedSpy.count(), 1);
    QCOMPARE(stateSpy.count(), 2);
    QCOMPARE(qvariant_cast<QBluetoothSocket::SocketState>(stateSpy.takeFirst().at(0)), QBluetoothSocket::ClosingState);
    QCOMPARE(qvariant_cast<QBluetoothSocket::SocketState>(stateSpy.takeFirst().at(0)), QBluetoothSocket::UnconnectedState);

    delete socket;
}

void tst_QBluetoothSocket::tst_localPeer_data()
{
    QTest::addColumn<QString>("localName");
    QTest::addColumn<QBluetoothAddress>("localAddress");
    QTest::addColumn<quint16>("localPort");
    QTest::addColumn<QString>("peerName");
    QTest::addColumn<QBluetoothAddress>("peerAddress");
    QTest::addColumn<quint16>("peerPort");

    QTest::newRow("test") << QString("00:11:B1:08:AD:B8") << QBluetoothAddress("0011B108ADB8") << quint16(10)
                          << QString("00:11:67:60:20:23") << QBluetoothAddress("001167602023") << quint16(10);
}

void tst_QBluetoothSocket::tst_localPeer()
{
    QFETCH(QString, localName);
    QFETCH(QBluetoothAddress, localAddress);
    QFETCH(quint16, localPort);
    QFETCH(QString, peerName);
    QFETCH(QBluetoothAddress, peerAddress);
    QFETCH(quint16, peerPort);

    /* Construction */
    QBluetoothSocket *socket = new QBluetoothSocket(QBluetoothSocket::RfcommSocket);

    QSignalSpy stateSpy(socket, SIGNAL(stateChanged(QBluetoothSocket::SocketState)));

    QCOMPARE(socket->socketType(), QBluetoothSocket::RfcommSocket);
    QCOMPARE(socket->state(), QBluetoothSocket::UnconnectedState);

    /* Connection */
    QSignalSpy connectedSpy(socket, SIGNAL(connected()));

    socket->connectToService(QBluetoothAddress("001167602023"), 10);    // echo service running on device 00:11:67:60:20:23

    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(qvariant_cast<QBluetoothSocket::SocketState>(stateSpy.takeFirst().at(0)), QBluetoothSocket::ConnectingState);
    QCOMPARE(socket->state(), QBluetoothSocket::ConnectingState);

    stateSpy.clear();

    int connectTime = MaxConnectTime;
    while (connectedSpy.count() == 0 && connectTime > 0) {
        QTest::qWait(1000);
        connectTime -= 1000;
    }

    QCOMPARE(connectedSpy.count(), 1);
    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(qvariant_cast<QBluetoothSocket::SocketState>(stateSpy.takeFirst().at(0)), QBluetoothSocket::ConnectedState);
    QCOMPARE(socket->state(), QBluetoothSocket::ConnectedState);

    QCOMPARE(socket->localName(), localName);
    QCOMPARE(socket->localAddress(), localAddress);
    QCOMPARE(socket->localPort(), localPort);
    QCOMPARE(socket->peerName(), peerName);
    QCOMPARE(socket->peerAddress(), peerAddress);
    QCOMPARE(socket->peerPort(), peerPort);

    /* Disconnection */
    QSignalSpy disconnectedSpy(socket, SIGNAL(disconnected()));

    socket->disconnectFromService();

    int disconnectTime = MaxConnectTime;
    while (disconnectedSpy.count() == 0 && disconnectTime > 0) {
        QTest::qWait(1000);
        disconnectTime -= 1000;
    }

    QCOMPARE(disconnectedSpy.count(), 1);
    QCOMPARE(stateSpy.count(), 2);
    QCOMPARE(qvariant_cast<QBluetoothSocket::SocketState>(stateSpy.takeFirst().at(0)), QBluetoothSocket::ClosingState);
    QCOMPARE(qvariant_cast<QBluetoothSocket::SocketState>(stateSpy.takeFirst().at(0)), QBluetoothSocket::UnconnectedState);

    delete socket;
}

QTEST_MAIN(tst_QBluetoothSocket)

#include "tst_qbluetoothsocket.moc"

