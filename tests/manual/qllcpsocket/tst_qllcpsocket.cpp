/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "../../auto/qconnectivitytestcommon.h"

#include <qllcpsocket.h>

#include <QtCore/QDebug>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QLlcpSocket::SocketState)
Q_DECLARE_METATYPE(QLlcpSocket::SocketError)

class tst_QLlcpSocket : public QObject
{
    Q_OBJECT

public:
    enum ClientConnectionShutdown {
        ServerDisconnect,
        ServerClose,
        ClientDisconnect,
        ClientClose
    };

    tst_QLlcpSocket();
    ~tst_QLlcpSocket();

private slots:
    void tst_construction();

    void tst_clientConnection_data();
    void tst_clientConnection();

    void tst_waitFor_data();
    void tst_waitFor();

    void tst_invalidServiceUri();
};

Q_DECLARE_METATYPE(tst_QLlcpSocket::ClientConnectionShutdown)

tst_QLlcpSocket::tst_QLlcpSocket()
{
    qRegisterMetaType<QLlcpSocket::SocketState>("QLlcpSocket::SocketState");
    qRegisterMetaType<QLlcpSocket::SocketError>("QLlcpSocket::SocketError");
}

tst_QLlcpSocket::~tst_QLlcpSocket()
{
}

void tst_QLlcpSocket::tst_construction()
{
    QLlcpSocket socket;

    QVERIFY(!socket.isOpen());
    QCOMPARE(socket.state(), QLlcpSocket::UnconnectedState);
    QCOMPARE(socket.error(), QLlcpSocket::UnknownSocketError);

    QVERIFY(!socket.hasPendingDatagrams());
    QCOMPARE(socket.pendingDatagramSize(), Q_INT64_C(-1));

    QCOMPARE(socket.bytesAvailable(), Q_INT64_C(0));
    QVERIFY(!socket.canReadLine());

    QVERIFY(socket.isSequential());
}

void tst_QLlcpSocket::tst_clientConnection_data()
{
    QTest::addColumn<ClientConnectionShutdown>("shutdown");
    QTest::addColumn<bool>("stream");

    QTest::newRow("server disconnect stream") << ServerDisconnect << true;
    QTest::newRow("server disconnect datagram") << ServerDisconnect << false;
    QTest::newRow("server close stream") << ServerClose << true;
    QTest::newRow("server close datagram") << ServerClose << false;
    QTest::newRow("client disconnect stream") << ClientDisconnect << true;
    QTest::newRow("client disconnect datagram") << ClientDisconnect << false;
    QTest::newRow("client close stream") << ClientClose << true;
    QTest::newRow("client close datagram") << ClientClose << false;
}

void tst_QLlcpSocket::tst_clientConnection()
{
    QFETCH(ClientConnectionShutdown, shutdown);
    QFETCH(bool, stream);

    QString service = QLatin1String("urn:nfc:sn:com.nokia.qtmobility.commandserver") +
                      (stream ? QLatin1String(".stream") : QLatin1String(".datagram"));

    /* Construction */
    QLlcpSocket *socket = new QLlcpSocket;

    QSignalSpy stateSpy(socket, SIGNAL(stateChanged(QLlcpSocket::SocketState)));

    QCOMPARE(socket->state(), QLlcpSocket::UnconnectedState);

    /* Connection */
    QSignalSpy connectedSpy(socket, SIGNAL(connected()));
    QSignalSpy errorSpy(socket, SIGNAL(error(QLlcpSocket::SocketError)));

    socket->connectToService(0, service);

    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(stateSpy.takeFirst().at(0).value<QLlcpSocket::SocketState>(),
             QLlcpSocket::ConnectingState);
    QCOMPARE(socket->state(), QLlcpSocket::ConnectingState);

    stateSpy.clear();

    QTRY_VERIFY_TIMEOUT(30000, !connectedSpy.isEmpty() || !errorSpy.isEmpty());

    if (errorSpy.count() != 0) {
        qDebug() << errorSpy.takeFirst().at(0).toInt();
        QSKIP("Connection error", SkipSingle);
    }

    QCOMPARE(connectedSpy.count(), 1);
    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(stateSpy.takeFirst().at(0).value<QLlcpSocket::SocketState>(),
             QLlcpSocket::ConnectedState);
    QCOMPARE(socket->state(), QLlcpSocket::ConnectedState);

    stateSpy.clear();

    QSignalSpy bytesWrittenSpy(socket, SIGNAL(bytesWritten(qint64)));
    QSignalSpy readyReadSpy(socket, SIGNAL(readyRead()));

    /* Verify connected to correct service */
    if (stream) {
        socket->write("URI\n");

        QCOMPARE(bytesWrittenSpy.count(), 1);
        QCOMPARE(bytesWrittenSpy.takeFirst().at(0).value<qint64>(), qint64(4));

        QTRY_VERIFY(!readyReadSpy.isEmpty() && socket->canReadLine());

        const QByteArray line = socket->readLine().trimmed();

        QCOMPARE(line, service.toLatin1());
    } else {
        socket->writeDatagram("URI");

        QCOMPARE(bytesWrittenSpy.count(), 1);
        QCOMPARE(bytesWrittenSpy.takeFirst().at(0).value<qint64>(), qint64(3));

        QTRY_VERIFY(!readyReadSpy.isEmpty() && socket->hasPendingDatagrams());

        QByteArray datagram;
        datagram.resize(socket->pendingDatagramSize());

        socket->readDatagram(datagram.data(), datagram.size());

        QCOMPARE(datagram, service.toLatin1());
    }

    bytesWrittenSpy.clear();
    readyReadSpy.clear();

    /* Read / Write */
    if (stream) {
        QByteArray data("ECHO Test data\n");
        socket->write(data);

        QCOMPARE(bytesWrittenSpy.count(), 1);
        QCOMPARE(bytesWrittenSpy.takeFirst().at(0).value<qint64>(), qint64(data.size()));

        QTRY_VERIFY(!readyReadSpy.isEmpty());

        const QByteArray line = socket->readLine().trimmed();

        QCOMPARE(line.constData(), "Test data");
    } else {
        socket->writeDatagram("ECHO Test data");

        QCOMPARE(bytesWrittenSpy.count(), 1);
        QCOMPARE(bytesWrittenSpy.takeFirst().at(0).value<qint64>(), qint64(14));

        QTRY_VERIFY(!readyReadSpy.isEmpty());

        QByteArray datagram;
        datagram.resize(socket->pendingDatagramSize());

        socket->readDatagram(datagram.data(), datagram.size());

        QCOMPARE(datagram.constData(), "Test data");
    }

    bytesWrittenSpy.clear();
    readyReadSpy.clear();

    QSignalSpy disconnectedSpy(socket, SIGNAL(disconnected()));
    errorSpy.clear();

    /* Shutdown */
    switch (shutdown) {
    case ServerDisconnect:
        if (stream)
            socket->write("DISCONNECT\n");
        else
            socket->writeDatagram("DISCONNECT");
        break;
    case ServerClose:
        if (stream)
            socket->write("CLOSE\n");
        else
            socket->writeDatagram("CLOSE");
        break;
    case ClientDisconnect:
        socket->disconnectFromService();
        break;
    case ClientClose:
        socket->close();
        break;
    }

    QTRY_VERIFY(!disconnectedSpy.isEmpty());

    QCOMPARE(disconnectedSpy.count(), 1);
    QCOMPARE(stateSpy.count(), 2);
    QCOMPARE(stateSpy.takeFirst().at(0).value<QLlcpSocket::SocketState>(),
             QLlcpSocket::ClosingState);
    QCOMPARE(stateSpy.takeFirst().at(0).value<QLlcpSocket::SocketState>(),
             QLlcpSocket::UnconnectedState);
    QVERIFY(!socket->isOpen());

    if (shutdown == ServerDisconnect || shutdown == ServerClose) {
        QTRY_VERIFY(!errorSpy.isEmpty());

        QCOMPARE(errorSpy.count(), 1);
        QCOMPARE(errorSpy.takeFirst().at(0).value<QLlcpSocket::SocketError>(),
                 QLlcpSocket::RemoteHostClosedError);
        QCOMPARE(socket->error(), QLlcpSocket::RemoteHostClosedError);
    }

    delete socket;
}

void tst_QLlcpSocket::tst_waitFor_data()
{
    QTest::addColumn<ClientConnectionShutdown>("shutdown");

    QTest::newRow("server disconnect") << ServerDisconnect;
    QTest::newRow("server close") << ServerClose;
    QTest::newRow("client disconnect") << ClientDisconnect;
    QTest::newRow("client close") << ClientClose;
}

void tst_QLlcpSocket::tst_waitFor()
{
    QFETCH(ClientConnectionShutdown, shutdown);

    QString service = QLatin1String("urn:nfc:sn:com.nokia.qtmobility.commandserver.stream");

    /* Construction */
    QLlcpSocket *socket = new QLlcpSocket;

    QCOMPARE(socket->state(), QLlcpSocket::UnconnectedState);

    /* Connection */
    socket->connectToService(0, service);
    QCOMPARE(socket->state(), QLlcpSocket::ConnectingState);
    QVERIFY(socket->waitForConnected());
    QCOMPARE(socket->state(), QLlcpSocket::ConnectedState);

    /* Verify connected to correct service */
    {
        socket->write("URI\n");
        QVERIFY(socket->waitForBytesWritten());

        QVERIFY(socket->waitForReadyRead());
        const QByteArray line = socket->readLine().trimmed();

        QCOMPARE(line, service.toLatin1());
    }

    /* Shutdown */
    switch (shutdown) {
    case ServerDisconnect:
        socket->write("DISCONNECT\n");
        break;
    case ServerClose:
        socket->write("CLOSE\n");
        break;
    case ClientDisconnect:
        socket->disconnectFromService();
        QCOMPARE(socket->state(), QLlcpSocket::ClosingState);
        break;
    case ClientClose:
        socket->close();
        QCOMPARE(socket->state(), QLlcpSocket::ClosingState);
        break;
    }

    QVERIFY(socket->waitForDisconnected());
    QVERIFY(!socket->isOpen());
    QCOMPARE(socket->state(), QLlcpSocket::UnconnectedState);

    if (shutdown == ServerDisconnect || shutdown == ServerClose)
        QCOMPARE(socket->error(), QLlcpSocket::RemoteHostClosedError);

    delete socket;
}

void tst_QLlcpSocket::tst_invalidServiceUri()
{
    QLatin1String invalidServiceUri("invalid");

    QLlcpSocket *socket = new QLlcpSocket;

    QSignalSpy stateSpy(socket, SIGNAL(stateChanged(QLlcpSocket::SocketState)));
    QSignalSpy errorSpy(socket, SIGNAL(error(QLlcpSocket::SocketError)));

    socket->connectToService(0, invalidServiceUri);

    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(stateSpy.takeFirst().at(0).value<QLlcpSocket::SocketState>(),
             QLlcpSocket::ConnectingState);
    QCOMPARE(socket->state(), QLlcpSocket::ConnectingState);

    QTRY_VERIFY(!errorSpy.isEmpty());

    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(stateSpy.takeFirst().at(0).value<QLlcpSocket::SocketState>(),
             QLlcpSocket::UnconnectedState);
    QCOMPARE(socket->state(), QLlcpSocket::UnconnectedState);

    QCOMPARE(errorSpy.takeFirst().at(0).value<QLlcpSocket::SocketError>(),
             QLlcpSocket::SocketAccessError);
    QCOMPARE(socket->error(), QLlcpSocket::SocketAccessError);
    QVERIFY(!socket->errorString().isEmpty());

    delete socket;
}

QTEST_MAIN(tst_QLlcpSocket)

#include "tst_qllcpsocket.moc"

