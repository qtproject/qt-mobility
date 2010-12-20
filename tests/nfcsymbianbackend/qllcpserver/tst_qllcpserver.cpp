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

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <qllcpserver.h>
#include <qllcpsocket.h>

#include "qnfctestcommon.h"
#include "qnfctestutil.h"
QTM_USE_NAMESPACE

QString TestUri("urn:nfc:xsn:nokia:symbiantest");

Q_DECLARE_METATYPE(QLlcpSocket::Error)
class tst_QLlcpServer : public QObject
{
    Q_OBJECT

public:
    tst_QLlcpServer();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    // ALERT Handshake required, do NOT change the following sequence of testcases.

    // basic acceptance test
    void newConnection(); //  handshake 1
    void newConnection_data();
    void newConnection_wait(); // handshake 2
    void newConnection_wait_data();
    void api_coverage();  // handshake 3

    // nagetive testcases
    void negTestCase1();  // handshake 4
};

tst_QLlcpServer::tst_QLlcpServer()
{
}
void tst_QLlcpServer::initTestCase()
{
	qRegisterMetaType<QLlcpSocket::Error>("QLlcpSocket::Error");
}

void tst_QLlcpServer::cleanupTestCase()
{
}
void tst_QLlcpServer::newConnection_data()
{
    QString hint = "handshake 1";
    QTest::addColumn<QString>("uri");
    QTest::addColumn<QString>("hint");
    QTest::newRow("0") << TestUri << hint;
}

/*!
 Description: Unit test for NFC LLCP server async functions

 TestScenario: 1. Server will listen to a pre-defined URI
               2. Wait client to connect.
               3. Read message from client.
               4. Echo the same message back to client
               5. Wait client disconnect event.

 TestExpectedResults:
               1. The listen successfully set up.
               2. The message has be received from client.
               3. The echoed message has been sent to client.
               4. Connection disconnected and NO error signals emitted.
*/
void tst_QLlcpServer::newConnection()
{
    QFETCH(QString, uri);
    QFETCH(QString, hint);

    QLlcpServer server;
    qDebug() << "Create QLlcpServer completed";
    qDebug() << "Start listening...";
    QSignalSpy connectionSpy(&server, SIGNAL(newConnection()));

    bool ret = server.listen(uri);
    QVERIFY(ret);
    qDebug() << "Listen() return ok";

    QNfcTestUtil::ShowMessage(hint);

    QTRY_VERIFY(!connectionSpy.isEmpty());
    qDebug() << "try to call nextPendingConnection()";
    QLlcpSocket *socket = server.nextPendingConnection();
    QVERIFY(socket != NULL);
    QSignalSpy readyReadSpy(socket, SIGNAL(readyRead()));
    QSignalSpy errorSpy(socket, SIGNAL(error(QLlcpSocket::Error)));

    //Get data from client
    QTRY_VERIFY(!readyReadSpy.isEmpty());

    qDebug() << "Bytes available = " << socket->bytesAvailable();
    quint16 blockSize = 0;
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_4_6);
    while (socket->bytesAvailable() < (int)sizeof(quint16)){
        QSignalSpy readyRead(socket, SIGNAL(readyRead()));
        QTRY_VERIFY(!readyRead.isEmpty());
    }
    in >> blockSize;
    qDebug()<<"Read blockSize from client: " << blockSize;
    while (socket ->bytesAvailable() < blockSize){
        QSignalSpy readyRead(socket, SIGNAL(readyRead()));
        QTRY_VERIFY(!readyRead.isEmpty());
    }
    QString echo;
    in >> echo;
    qDebug() << "Read data from client:" << echo;
    //Send data to client
    QSignalSpy bytesWrittenSpy(socket, SIGNAL(bytesWritten(qint64)));
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << (quint16)0;
    out << echo;
    qDebug()<<"Write echoed data back to client";
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    qint64 val = socket->write(block);
    qDebug("Write() return value = %d", val);
    QVERIFY(val != -1);

    QTRY_VERIFY(!bytesWrittenSpy.isEmpty());
    qint64 written = bytesWrittenSpy.first().at(0).value<qint64>();
    qDebug()<<"Server::bytesWritten signal return value = " << written;
    while (written < echo.length())
        {
        QSignalSpy bytesWrittenSpy(socket, SIGNAL(bytesWritten(qint64)));
        QTRY_VERIFY(!bytesWrittenSpy.isEmpty());
        written += bytesWrittenSpy.first().at(0).value<qint64>();
        }
    //Now data has been sent,check the if existing error
    QVERIFY(errorSpy.isEmpty());

    server.close();
}

/*!
 Description: Unit test for NFC LLCP server sync(waitXXX) functions

 TestScenario: 1. Server will listen to a pre-defined URI
               2. Wait client to connect.
               3. Read message from client.
               4. Echo the same message back to client
               5. Wait client disconnect event.

 TestExpectedResults:
               1. The listen successfully set up.
               2. The message has be received from client.
               3. The echoed message has been sent to client.
               4. Connection disconnected and NO error signals emitted.
*/
void tst_QLlcpServer::newConnection_wait()
    {
    QFETCH(QString, uri);
    QFETCH(QString, hint);

    QLlcpServer server;
    qDebug() << "Create QLlcpServer completed";
    qDebug() << "Start listening...";
    bool ret = server.listen(uri);
    QVERIFY(ret);
    qDebug() << "Listen() return ok";
    QSignalSpy connectionSpy(&server, SIGNAL(newConnection()));
    QNfcTestUtil::ShowMessage(hint);

    QTRY_VERIFY(!connectionSpy.isEmpty());
    qDebug() << "try to call nextPendingConnection()";
    QLlcpSocket *socket = server.nextPendingConnection();
    QVERIFY(socket != NULL);

    QSignalSpy errorSpy(socket, SIGNAL(error(QLlcpSocket::Error)));
    //Get data from client
    const int Timeout = 10 * 1000;

    quint16 blockSize = 0;
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_4_6);
    while (socket->bytesAvailable() < (int)sizeof(quint16)) {
        bool ret = socket->waitForReadyRead(Timeout);
        QVERIFY(ret);
    }

    in >> blockSize;
    qDebug()<<"Read blockSize from client: " << blockSize;
    while (socket ->bytesAvailable() < blockSize){
        bool ret = socket->waitForReadyRead(Timeout);
        QVERIFY(ret);
    }
    QString echo;
    in >> echo;
    qDebug() << "Read data from client:" << echo;
    //Send data to client
    QSignalSpy bytesWrittenSpy(socket, SIGNAL(bytesWritten(qint64)));

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << (quint16)0;
    out << echo;
    qDebug()<<"Write echoed data back to client";
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    qint64 val = socket->write(block);
    qDebug("Write() return value = %d", val);
    QVERIFY(val != -1);

    ret = socket->waitForBytesWritten(Timeout);
    QVERIFY(ret);

    QTRY_VERIFY(!bytesWrittenSpy.isEmpty());
    qint64 written = bytesWrittenSpy.first().at(0).value<qint64>();

    while (written < echo.length())
        {
        QSignalSpy bytesWrittenSpy(socket, SIGNAL(bytesWritten(qint64)));
        bool ret = socket->waitForBytesWritten(Timeout);
        QVERIFY(ret);
        QTRY_VERIFY(!bytesWrittenSpy.isEmpty());
        written += bytesWrittenSpy.first().at(0).value<qint64>();
        }
    //Now data has been sent,check the if existing error
    if (!errorSpy.isEmpty())
        {
            QLlcpSocket::Error error = errorSpy.first().at(0).value<QLlcpSocket::Error>();
            qDebug("QLlcpSocket::Error =%d", error);
        }
    QVERIFY(errorSpy.isEmpty());

    server.close();
    }

void tst_QLlcpServer::newConnection_wait_data()
    {
    QTest::addColumn<QString>("uri");
    QTest::addColumn<QString>("hint");
    QTest::newRow("0") << TestUri
            << "handshake 2";
    }



/*!
 Description: LLCP Server API test & Socket readDatagram API test
 TestScenario:
     1) Read two datagrams from llcp client device
     2) Covered API: readDatagram(), serviceUri(), servicePort(), isListening(), serverError()
 */
void tst_QLlcpServer::api_coverage()
{
    QString uri = TestUri;
    QLlcpServer server;
    QSignalSpy connectionSpy(&server, SIGNAL(newConnection()));
    bool ret = server.listen(uri);
    QVERIFY(ret);

    QString message("handshake 3: api_coverage test");
    QNfcTestUtil::ShowMessage(message);

    QTRY_VERIFY(!connectionSpy.isEmpty());

    bool hasPending = server.hasPendingConnections();
    QVERIFY(hasPending);
    QLlcpSocket *socket = server.nextPendingConnection();
    QVERIFY(socket != NULL);
    QSignalSpy readyReadSpy(socket, SIGNAL(readyRead()));
    //Get data from client
    QTRY_VERIFY(!readyReadSpy.isEmpty());

    qint64 size = socket->bytesAvailable();
    QTRY_VERIFY(size > 0);
    QByteArray datagram;
    datagram.resize(size);
    qint64 readSize = socket->readDatagram(datagram.data(), datagram.size());
    QVERIFY(readSize != -1);

    qDebug()<<"Server Uri = " << server.serviceUri();
    QCOMPARE(uri,server.serviceUri());

    quint8 unsupportedPort = 0;
    QCOMPARE(unsupportedPort,server.serverPort());

    QVERIFY(server.isListening() == true);
    QVERIFY(server.serverError() == QLlcpServer::UnknownSocketError);
    server.close();
}


/*!
 Description: listen negative test - listen twice
*/
void tst_QLlcpServer::negTestCase1()
{
    QString message("handshake 4: negTestCase1 test");
    QNfcTestUtil::ShowMessage(message);

    QString uri = TestUri;
    QLlcpServer server;
    QSignalSpy connectionSpy(&server, SIGNAL(newConnection()));
    bool ret = server.listen(uri);
    QVERIFY(ret);

    ret = server.listen(uri);
    QVERIFY(ret == false);

    server.close();
}


QTEST_MAIN(tst_QLlcpServer);

#include "tst_qllcpserver.moc"
