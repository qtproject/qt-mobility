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

#include <qllcpserver.h>
#include <qllcpsocket.h>

#include <QtCore/QDebug>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QLlcpSocket::SocketState)
Q_DECLARE_METATYPE(QLlcpSocket::SocketError)

class tst_QLlcpServer : public QObject
{
    Q_OBJECT

public:
    enum ServerConnectionShutdown {
        ServerDisconnect,
        ServerClose,
        ClientDisconnect,
        ClientClose
    };

    tst_QLlcpServer();
    ~tst_QLlcpServer();

private slots:
    void tst_construction();

    void tst_serverConnection_data();
    void tst_serverConnection();
};

Q_DECLARE_METATYPE(tst_QLlcpServer::ServerConnectionShutdown)

tst_QLlcpServer::tst_QLlcpServer()
{
    qRegisterMetaType<QLlcpSocket::SocketState>("QLlcpSocket::SocketState");
    qRegisterMetaType<QLlcpSocket::SocketError>("QLlcpSocket::SocketError");
}

tst_QLlcpServer::~tst_QLlcpServer()
{
}

void tst_QLlcpServer::tst_construction()
{
    QLlcpServer server;

    QVERIFY(!server.isListening());

    QVERIFY(server.serviceUri().isEmpty());
    QCOMPARE(server.serverPort(), quint8(0));

    QVERIFY(!server.hasPendingConnections());
    QVERIFY(!server.nextPendingConnection());

    QCOMPARE(server.serverError(), QLlcpSocket::UnknownSocketError);
}

void tst_QLlcpServer::tst_serverConnection_data()
{
    QTest::addColumn<ServerConnectionShutdown>("shutdown");
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

void tst_QLlcpServer::tst_serverConnection()
{
    QFETCH(ServerConnectionShutdown, shutdown);
    QFETCH(bool, stream);

    QString service = QLatin1String("urn:nfc:sn:com.nokia.qtmobility.helloserver") +
                      (stream ? QLatin1String(".stream") : QLatin1String(".datagram"));

    /* Construction */
    QLlcpServer *server = new QLlcpServer;

    QSignalSpy newConnectionSpy(server, SIGNAL(newConnection()));

    QVERIFY(server->listen(service));

    QVERIFY(server->isListening());
    QCOMPARE(server->serviceUri(), service);

    QTRY_VERIFY_TIMEOUT(30000, !newConnectionSpy.isEmpty());

    QVERIFY(server->hasPendingConnections());

    QLlcpSocket *socket = server->nextPendingConnection();

    QVERIFY(socket);

    QCOMPARE(socket->state(), QLlcpSocket::ConnectedState);
    QVERIFY(socket->isOpen());

    QSignalSpy readyReadSpy(socket, SIGNAL(readyRead()));
    QSignalSpy disconnectedSpy(socket, SIGNAL(disconnected()));
    QSignalSpy stateSpy(socket, SIGNAL(stateChanged(QLlcpSocket::SocketState)));
    QSignalSpy errorSpy(socket, SIGNAL(error(QLlcpSocket::SocketError)));

    QTRY_VERIFY(!readyReadSpy.isEmpty());
    readyReadSpy.clear();

    /* Read Hello */
    if (stream) {
        const QByteArray line = socket->readLine().trimmed();

        QVERIFY(!line.isEmpty());

        QList<QByteArray> fields = line.split(' ');

        QCOMPARE(fields.count(), 2);

        QCOMPARE(fields.at(0), QByteArray("HELLO"));
        QCOMPARE(QString::fromUtf8(fields.at(1).constData()), service);
    } else {
        QVERIFY(socket->hasPendingDatagrams());

        qint64 size = socket->pendingDatagramSize();
        QVERIFY(size > 0);

        QByteArray data;
        data.resize(size);
        QCOMPARE(size, socket->readDatagram(data.data(), data.size()));

        QList<QByteArray> fields = data.split(' ');

        QCOMPARE(fields.count(), 2);

        QCOMPARE(fields.at(0), QByteArray("HELLO"));
        QCOMPARE(QString::fromUtf8(fields.at(1).constData()), service);
    }

    disconnectedSpy.clear();

    /* Write shutdown */
    switch (shutdown) {
    case ServerDisconnect:
        socket->disconnectFromService();
        break;
    case ServerClose:
        socket->close();
        break;
    case ClientDisconnect:
        if (stream)
            socket->write("DISCONNECT\n");
        else
            socket->writeDatagram("DISCONNECT", 10);
        break;
    case ClientClose:
        if (stream)
            socket->write("CLOSE\n");
        else
            socket->writeDatagram("CLOSE", 5);
        break;
    }

    /* Verify shutdown */
    {
        QTRY_VERIFY(!disconnectedSpy.isEmpty());

        QCOMPARE(disconnectedSpy.count(), 1);
        QCOMPARE(stateSpy.count(), 2);
        QCOMPARE(stateSpy.takeFirst().at(0).value<QLlcpSocket::SocketState>(),
                 QLlcpSocket::ClosingState);
        QCOMPARE(stateSpy.takeFirst().at(0).value<QLlcpSocket::SocketState>(),
                 QLlcpSocket::UnconnectedState);
        QVERIFY(!socket->isOpen());

        if (shutdown == ClientDisconnect || shutdown == ClientClose) {
            QTRY_VERIFY(!errorSpy.isEmpty());

            QCOMPARE(errorSpy.count(), 1);
            QCOMPARE(errorSpy.takeFirst().at(0).value<QLlcpSocket::SocketError>(),
                     QLlcpSocket::RemoteHostClosedError);
            QCOMPARE(socket->error(), QLlcpSocket::RemoteHostClosedError);
        }
    }

    delete socket;

    server->close();

    QVERIFY(!server->isListening());

    delete server;
}

QTEST_MAIN(tst_QLlcpServer)

#include "tst_qllcpserver.moc"

