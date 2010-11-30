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
class tst_QLlcpServer : public QObject
{
    Q_OBJECT

public:
    tst_QLlcpServer();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void newConnection();
    void newConnection_data();
    void newConnection_wait();
    void newConnection_wait_data();
};

tst_QLlcpServer::tst_QLlcpServer()
{
}
void tst_QLlcpServer::initTestCase()
{
}

void tst_QLlcpServer::cleanupTestCase()
{
}
void tst_QLlcpServer::newConnection_data()
{
    QTest::addColumn<QString>("uri");
    QTest::addColumn<QString>("hint");
    QTest::newRow("0") << TestUri
            << "Please touch a NFC device with llcp client enabled: uri = " + TestUri;

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
    bool ret = server.listen(uri);
    QVERIFY(ret);
    qDebug() << "Listen() return ok";
    QSignalSpy connectionSpy(&server, SIGNAL(newConnection()));

    QNfcTestUtil::ShowMessage(hint);

    QTRY_VERIFY(!connectionSpy.isEmpty());
    qDebug() << "try to call nextPendingConnection()";
    QLlcpSocket *socket = server.nextPendingConnection();
    QVERIFY(socket != NULL);

    QSignalSpy readyReadSpy(socket, SIGNAL(readyRead()));
    QSignalSpy errorSpy(socket, SIGNAL(error(QLlcpSocket::Error)));
    //Get data from client
    QTRY_VERIFY(!readyReadSpy.isEmpty());

    quint16 blockSize = 0;
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_4_6);
    while (socket->bytesAvailable() < (int)sizeof(quint16)){
        QSignalSpy readyRead(socket, SIGNAL(readyRead()));
        QTRY_VERIFY(!readyRead.isEmpty());
    }
    in >> blockSize;

    while (socket ->bytesAvailable() < blockSize){
        QSignalSpy readyRead(socket, SIGNAL(readyRead()));
        QTRY_VERIFY(!readyRead.isEmpty());
    }
    QString echo;
    in >> echo;
    //Send data to client
    QSignalSpy bytesWrittenSpy(socket, SIGNAL(bytesWritten(qint64)));
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << (quint16)echo.length();
    out << echo;

    socket->write(block);

    QTRY_VERIFY(!bytesWrittenSpy.isEmpty());
    qint64 written = bytesWrittenSpy.first().at(0).value<qint64>();

    while (written < echo.length())
        {
        QSignalSpy bytesWrittenSpy(socket, SIGNAL(bytesWritten(qint64)));
        QTRY_VERIFY(!bytesWrittenSpy.isEmpty());
        written = bytesWrittenSpy.first().at(0).value<qint64>();
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
    bool ret = server.listen(uri);
    QVERIFY(ret);

    QSignalSpy connectionSpy(&server, SIGNAL(newConnection()));

    QNfcTestUtil::ShowMessage(hint);

    QTRY_VERIFY(!connectionSpy.isEmpty());

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

    while (socket ->bytesAvailable() < blockSize){
        bool ret = socket->waitForReadyRead(Timeout);
        QVERIFY(ret);
    }
    QString echo;
    in >> echo;
    //Send data to client
    QSignalSpy bytesWrittenSpy(socket, SIGNAL(bytesWritten(qint64)));

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << (quint16)echo.length();
    out << echo;

    socket->write(block);
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
        written = bytesWrittenSpy.first().at(0).value<qint64>();
        }
    //Now data has been sent,check the if existing error
    QVERIFY(errorSpy.isEmpty());

    server.close();
    }

void tst_QLlcpServer::newConnection_wait_data()
    {
    QTest::addColumn<QString>("uri");
    QTest::addColumn<QString>("hint");
    QTest::newRow("0") << TestUri
            << "Please touch a NFC device with llcp client enabled: uri = " + TestUri;
    }
QTEST_MAIN(tst_QLlcpServer);

#include "tst_qllcpserver.moc"
