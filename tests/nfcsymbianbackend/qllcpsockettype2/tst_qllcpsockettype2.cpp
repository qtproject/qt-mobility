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

#include <qllcpsocket.h>
#include <qnearfieldmanager.h>
#include <qnearfieldtarget.h>
#include "qnfctestcommon.h"
#include "qnfctestutil.h"

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QNearFieldTarget*)
Q_DECLARE_METATYPE(QLlcpSocket::State)
Q_DECLARE_METATYPE(QLlcpSocket::Error)

QString TestUri("urn:nfc:xsn:nokia:symbiantest");
class tst_qllcpsockettype2 : public QObject
{
    Q_OBJECT

public:
    tst_qllcpsockettype2();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    // ALERT Handshake required, do NOT change the following sequence of testcases.

    // basic acceptance testcases
    void echo();   // handshake 1
    void echo_data();
    void echo_wait();  // handshake 2
    void echo_wait_data();
    void api_coverage();  // handshake 3
    void connectTest();   // handshake 4
    void multipleWrite();

    // nagetive testcases
    void negTestCase1();
    void negTestCase1_data();
    void negTestCase2();
    void negTestCase3();

 private:
    QNearFieldTarget *m_target; // not own
};

tst_qllcpsockettype2::tst_qllcpsockettype2()
{
    qRegisterMetaType<QNearFieldTarget*>("QNearFieldTarget*");
    qRegisterMetaType<QLlcpSocket::Error>("QLlcpSocket::Error");
    qRegisterMetaType<QLlcpSocket::State>("QLlcpSocket::State");
}

void tst_qllcpsockettype2::initTestCase()
{
    qDebug()<<"tst_qllcpsockettype2::initTestCase() Begin";
    QString message("Please touch a NFC device with llcp client enabled");
    QNearFieldManager nfcManager;
    QSignalSpy targetDetectedSpy(&nfcManager, SIGNAL(targetDetected(QNearFieldTarget*)));
    nfcManager.startTargetDetection(QNearFieldTarget::AnyTarget);

    QNfcTestUtil::ShowMessage(message);
    QTRY_VERIFY(!targetDetectedSpy.isEmpty());
    if(targetDetectedSpy.count()!=1)
        {
        qDebug()<<"!!Several LLCP target found!!";
        }
    m_target = targetDetectedSpy.at(targetDetectedSpy.count() - 1).at(0).value<QNearFieldTarget*>();
    QVERIFY(m_target!=NULL);
    QVERIFY(m_target->accessMethods() & QNearFieldTarget::LlcpAccess);
    qDebug()<<"tst_qllcpsockettype2::initTestCase()   End";
}


void tst_qllcpsockettype2::cleanupTestCase()
{
}


/*!
 Description: Unit test for NFC LLCP socket async functions

 TestScenario:
               1. Echo client will connect to the Echo server
               2. Echo client will send the "echo" message to the server
               3. Echo client will receive the same message echoed from the server
               4. Echo client will disconnect the connection.

 TestExpectedResults:
               2. The message has be sent to server.
               3. The echoed message has been received from server.
               4. Connection disconnected and NO error signals emitted.

 CounterPart test: tst_QLlcpServer::newConnection() or newConnection_wait()
*/
void tst_qllcpsockettype2::echo()
{
    QFETCH(QString, uri);
    QFETCH(QString, echo);

    QString message("handshake 1");
    QNfcTestUtil::ShowMessage(message);

    QLlcpSocket socket(this);
    QSignalSpy connectedSpy(&socket, SIGNAL(connected()));
    QSignalSpy errorSpy(&socket, SIGNAL(error(QLlcpSocket::Error)));
    QSignalSpy readyReadSpy(&socket, SIGNAL(readyRead()));

    QSignalSpy bytesWrittenSpy(&socket, SIGNAL(bytesWritten(qint64)));

    socket.connectToService(m_target, uri);

    // cocoverage add: WaitForBytesWritten will fail when connecting
    const int secs = 1 * 1000;
    bool waitRet = socket.waitForBytesWritten(secs);
    QVERIFY( waitRet == false);

    QTRY_VERIFY(!connectedSpy.isEmpty());

    // cocoverage add: connect to Service again when already connected will cause fail
    socket.connectToService(m_target, uri);
    QTRY_VERIFY(!errorSpy.isEmpty());
    errorSpy.clear();

    //Send data to server
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << (quint16)0;
    out << echo;

    qDebug("Client-- write quint16 length = %d", sizeof(quint16));
    qDebug("Client-- write echo string = %s", qPrintable(echo));
    qDebug("Client-- write echo string length= %d", echo.length());
    qDebug("Client-- write data length = %d", block.length());
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    qint64 val = socket.write(block);
    QVERIFY( val != -1);

    QTRY_VERIFY(!bytesWrittenSpy.isEmpty());
    qint64 written = bytesWrittenSpy.first().at(0).value<qint64>();

    qDebug()<<"bytesWritten signal return value = "<<written;
    while (written < echo.length())
        {
        QSignalSpy bytesWrittenSpy(&socket, SIGNAL(bytesWritten(qint64)));
        QTRY_VERIFY(!bytesWrittenSpy.isEmpty());
        written += bytesWrittenSpy.first().at(0).value<qint64>();
        }

    //Get the echoed data from server
    QTRY_VERIFY(!readyReadSpy.isEmpty());
    quint16 blockSize = 0;
    QDataStream in(&socket);
    in.setVersion(QDataStream::Qt_4_6);
    while (socket.bytesAvailable() < (int)sizeof(quint16)){
        QSignalSpy readyRead(&socket, SIGNAL(readyRead()));
        QTRY_VERIFY(!readyRead.isEmpty());
    }
    in >> blockSize;
    qDebug() << "Client-- read blockSize=" << blockSize;
    while (socket.bytesAvailable() < blockSize){
        QSignalSpy readyRead(&socket, SIGNAL(readyRead()));
        QTRY_VERIFY(!readyRead.isEmpty());
    }
    QString echoed;
    in >> echoed;

    qDebug() << "Client-- read echoed string =" << echoed;
    //test the echoed string is same as the original string
    QVERIFY(echo == echoed);

    socket.disconnectFromService();

    //Now data has been sent,check the if existing error
    QVERIFY(errorSpy.isEmpty());
}

void tst_qllcpsockettype2::echo_data()
{
    QTest::addColumn<QString>("uri");
    QTest::addColumn<QString>("echo");
    QTest::newRow("0") << TestUri
            << "echo";
}

/*!
 Description: Unit test for NFC LLCP socket sync(waitXXX) functions

 TestScenario: 1. Touch a NFC device with LLCP Echo service actived
               2. Echo client will connect to the Echo server
               3. Echo client will send the "echo" message to the server
               4. Echo client will receive the same message echoed from the server
               5. Echo client will disconnect the connection.

 TestExpectedResults:
               1. The connection successfully set up.
               2. The message has be sent to server.
               3. The echoed message has been received from server.
               4. Connection disconnected and NO error signals emitted.

 CounterPart test: tst_QLlcpServer::newConnection() or newConnection_wait()
*/
void tst_qllcpsockettype2::echo_wait()
    {
    QFETCH(QString, uri);
    QFETCH(QString, echo);

    QString message("handshake 2");
    QNfcTestUtil::ShowMessage(message);

    QLlcpSocket socket(this);
    const int Timeout = 10 * 1000;

    socket.connectToService(m_target, uri);

    bool ret = socket.waitForConnected(Timeout);
    QVERIFY(ret);

    QSignalSpy errorSpy(&socket, SIGNAL(error(QLlcpSocket::Error)));

    //Send data to server
    QSignalSpy bytesWrittenSpy(&socket, SIGNAL(bytesWritten(qint64)));

    //Send data to server
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << (quint16)0;
    out << echo;

    qDebug("Client-- write quint16 length = %d", sizeof(quint16));
    qDebug("Client-- write echo string = %s", qPrintable(echo));
    qDebug("Client-- write echo string length= %d", echo.length());
    qDebug("Client-- write data length = %d", block.length());
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    qint64 val = socket.write(block);
    QVERIFY( val != -1);

    ret = socket.waitForBytesWritten(Timeout);
    QVERIFY(ret);

    QTRY_VERIFY(!bytesWrittenSpy.isEmpty());
    qint64 written = bytesWrittenSpy.first().at(0).value<qint64>();

    while (written < echo.length())
        {
        QSignalSpy bytesWrittenSpy(&socket, SIGNAL(bytesWritten(qint64)));
        ret = socket.waitForBytesWritten(Timeout);
        QVERIFY(ret);
        QTRY_VERIFY(!bytesWrittenSpy.isEmpty());
        written += bytesWrittenSpy.first().at(0).value<qint64>();
        }

    //Get the echoed data from server
    quint16 blockSize = 0;
    QDataStream in(&socket);
    in.setVersion(QDataStream::Qt_4_6);
    while (socket.bytesAvailable() < (int)sizeof(quint16)){
        ret = socket.waitForReadyRead(Timeout);
        QVERIFY(ret);
    }
    in >> blockSize;
    qDebug() << "Client-- read blockSize=" << blockSize;
    while (socket.bytesAvailable() < blockSize){
        ret = socket.waitForReadyRead(Timeout);
        QVERIFY(ret);
    }
    QString echoed;
    in >> echoed;
    qDebug() << "Client-- read echoed string =" << echoed;
    //test the echoed string is same as the original string
    QVERIFY(echo == echoed);

    socket.disconnectFromService();
    ret = socket.waitForDisconnected(Timeout);
    QVERIFY(ret);
    //Now data has been sent,check the if existing error
    QVERIFY(errorSpy.isEmpty());
    }

void tst_qllcpsockettype2::echo_wait_data()
{
    QTest::addColumn<QString>("uri");
    QTest::addColumn<QString>("echo");
    QTest::newRow("0") << TestUri
            << "echo";
}



/*!
 Description: LLCP Socket API & State Machine test (ReadDatagram tested in server side)
 TestScenario:
     Covered API: state(), error(), writeDatagram(const char *data, qint64 size);
                  writeDatagram(const QByteArray &datagram);

 CounterPart test: tst_QLlcpServer::api_coverage()
 */
void tst_qllcpsockettype2::api_coverage()
{

    QString message("handshake 3: api_coverage test");
    QNfcTestUtil::ShowMessage(message);

    QLlcpSocket socket(this);
    QCOMPARE(socket.state(), QLlcpSocket::UnconnectedState);
    QSignalSpy stateChangedSpy(&socket, SIGNAL(stateChanged(QLlcpSocket::State)));

    QSignalSpy connectedSpy(&socket, SIGNAL(connected()));
    socket.connectToService(m_target, TestUri);
    QTRY_VERIFY(!connectedSpy.isEmpty());

    QVERIFY(stateChangedSpy.count() == 2);
    QLlcpSocket::State  state1 = stateChangedSpy.at(0).at(0).value<QLlcpSocket::State>();
    QLlcpSocket::State  state2 = stateChangedSpy.at(1).at(0).value<QLlcpSocket::State>();
    QCOMPARE(state1, QLlcpSocket::ConnectingState);
    QCOMPARE(state2, QLlcpSocket::ConnectedState);

    QSignalSpy bytesWrittenSpy(&socket, SIGNAL(bytesWritten(qint64)));
    message = "Connection oriented write test string";
    QByteArray array;
    array.append(message);
    qint64 ret = socket.writeDatagram(array.constData(),array.size());
    QVERIFY( ret != -1);

    QTRY_VERIFY(bytesWrittenSpy.count() == 1);

    stateChangedSpy.clear();
    socket.disconnectFromService();
    QVERIFY(stateChangedSpy.count() == 1);
    state1 = stateChangedSpy.at(0).at(0).value<QLlcpSocket::State>();
    QCOMPARE(state1, QLlcpSocket::UnconnectedState);

    QCOMPARE(socket.error(),QLlcpSocket::UnknownSocketError);

}


/*!
 Description:  connect to Service testcases
 TestScenario:  1) double connect cause failure
                2) disconnect the connecting socket should not cause failure
                3) readDatagram after disconnection will cause failure.

 CounterPart test: tst_QLlcpServer::newConnection() or newConnection_wait()
 */
void tst_qllcpsockettype2::connectTest()
{
    QString message("handshake 4");
    QNfcTestUtil::ShowMessage(message);

    QLlcpSocket socket(this);
    QCOMPARE(socket.state(), QLlcpSocket::UnconnectedState);
    QSignalSpy errorSpy(&socket, SIGNAL(error(QLlcpSocket::Error)));

    QSignalSpy connectedSpy(&socket, SIGNAL(connected()));
    socket.connectToService(m_target, TestUri);

    //connect to the service again when previous one is connecting
    socket.connectToService(m_target, TestUri);
    QTRY_VERIFY(!errorSpy.isEmpty());

    errorSpy.clear();
    // make sure it is still connecting
    if(connectedSpy.isEmpty())
    {
       socket.disconnectFromService();
    }
    QTRY_VERIFY(errorSpy.isEmpty());

    //double disconnect should not cause error
    socket.disconnectFromService();
    QTRY_VERIFY(errorSpy.isEmpty());

    // readDatagram must be called before successul connection to server
    QByteArray datagram;
    datagram.resize(127);
    qint64 ret = socket.readDatagram(datagram.data(), datagram.size());
    QVERIFY(ret == -1);
}

/*!
 Description: Unit test for NFC LLCP socket write several times

 TestScenario:
               1. Echo client will connect to the Echo server
               2. Echo client will send the "echo" message to the server
               3. Echo client will receive the same message echoed from the server
               4. Echo client will disconnect the connection.

 TestExpectedResults:
               2. The message has be sent to server.
               3. The echoed message has been received from server.
               4. Connection disconnected and NO error signals emitted.

 CounterPart test: tst_QLlcpServer::newConnection() or newConnection_wait()
*/
void tst_qllcpsockettype2::multipleWrite()
    {
    QString message("handshake 5: multipleWrite");
    QNfcTestUtil::ShowMessage(message);
    QLlcpSocket socket(this);

    QSignalSpy connectedSpy(&socket, SIGNAL(connected()));
    socket.connectToService(m_target, TestUri);
    QTRY_VERIFY(!connectedSpy.isEmpty());

    QSignalSpy bytesWrittenSpy(&socket, SIGNAL(bytesWritten(qint64)));
    message = "1234567890";
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << (quint16)0;
    out << message;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    qint64 ret = socket.writeDatagram(block.constData(), block.size()/2);
    QVERIFY( ret != -1);
    ret = socket.writeDatagram(block.constData() + block.size()/2, block.size()/2);
    QVERIFY( ret != -1);

    QTRY_VERIFY(bytesWrittenSpy.count() > 0);
    while (bytesWrittenSpy.count() < 2)
        {
        QTRY_VERIFY(bytesWrittenSpy.count() == 2);
        }
    qint64 written1 = bytesWrittenSpy.at(0).at(0).value<qint64>();
    qint64 written2 = bytesWrittenSpy.at(1).at(0).value<qint64>();
    QCOMPARE(written1 + written2, (qint64)block.size());
    //Get the echoed data from server
    const int Timeout = 10 * 1000;
    quint16 blockSize = 0;
    QDataStream in(&socket);
    in.setVersion(QDataStream::Qt_4_6);
    while (socket.bytesAvailable() < (int)sizeof(quint16)){
        ret = socket.waitForReadyRead(Timeout);
        QVERIFY(ret);
    }
    in >> blockSize;
    qDebug() << "Client-- read blockSize=" << blockSize;
    while (socket.bytesAvailable() < blockSize){
        ret = socket.waitForReadyRead(Timeout);
        QVERIFY(ret);
    }
    QString echoed;
    in >> echoed;
    qDebug() << "Client-- read echoed string =" << echoed;
    //test the echoed string is same as the original string
    QCOMPARE(echoed, message);
    socket.disconnectFromService();
    ret = socket.waitForDisconnected(Timeout);
    QVERIFY(ret);
    }
/*!
 Description:  negative test - connect to an invalid service
 TODO: this test can not be realized,since the port num is hardcode at this time
*/
void tst_qllcpsockettype2::negTestCase1()
{
}

/*!
 invalid service name pool
*/
void tst_qllcpsockettype2::negTestCase1_data()
{
    QTest::addColumn<QString>("URI");
    QString invalidService = "InvalidURI";
    QTest::newRow("row1") << invalidService;
}

/*!
 Description: readDatagram/writeDatagram negative test - read/write without connectToService

 CounterPart test: tst_QLlcpServer::newConnection() or newConnection_wait()
*/
void tst_qllcpsockettype2::negTestCase2()
{
    QLlcpSocket socket(this);
    QByteArray datagram("Connection oriented negTestCase2");
    qint64 ret = socket.writeDatagram(datagram);
    QTRY_VERIFY(ret == -1);
    ret = socket.readDatagram(datagram.data(),datagram.size());
    QTRY_VERIFY(ret == -1);
}


/*!
 Description: negative testcase II - invalid usage of connection-less API
 CounterPart test: tst_QLlcpServer::newConnection() or newConnection_wait()
*/
void tst_qllcpsockettype2::negTestCase3()
{
    QLlcpSocket socket(this);

    QSignalSpy errorSpy(&socket, SIGNAL(error(QLlcpSocket::Error)));
    socket.connectToService(m_target, TestUri);

    bool ret = socket.bind(35);
    QVERIFY(ret == false);

    ret = socket.hasPendingDatagrams();
    QVERIFY(ret == false);

    qint64 size = socket.pendingDatagramSize();
    QVERIFY(size == -1);

    QString message = "Oops, Invalid usage for writeDatagram";
    const char* data = (const char *) message.data();
    qint64 strSize = message.size();
    size = socket.writeDatagram(data,strSize,m_target, 35);
    QVERIFY(size == -1);
    QByteArray datagram(data);
    size = socket.writeDatagram(datagram,m_target, 35);
    QVERIFY(size == -1);

    quint8 port = 35;
    size = socket.readDatagram(datagram.data(),datagram.size(),&m_target, &port);
    QVERIFY(size == -1);

}

QTEST_MAIN(tst_qllcpsockettype2);

#include "tst_qllcpsockettype2.moc"
