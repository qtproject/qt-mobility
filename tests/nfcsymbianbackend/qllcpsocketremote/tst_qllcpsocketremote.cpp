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
Q_DECLARE_METATYPE(QLlcpSocket::Error);

class tst_qllcpsocketremote : public QObject
{
    Q_OBJECT

public:
    tst_qllcpsocketremote();

private Q_SLOTS:

    // ALERT: Handshake required, do NOT change the sequence of handshaking testcases.
    void echoServer();   // handshake 1,2  - work with  tst_qllcpsocketlocal echoClient
    void testCase2();   // handshake 3   - work with  tst_qllcpsocketlocal testCase2

    void initTestCase();
    void cleanupTest();

private:
     QNearFieldTarget *m_target; // Not own
     quint8 m_port;
};

tst_qllcpsocketremote::tst_qllcpsocketremote()
{
    qRegisterMetaType<QNearFieldTarget *>("QNearFieldTarget*");
    qRegisterMetaType<QNearFieldTarget *>("QLlcpSocket::Error");
}

/*!
 Description: Init test case for NFC LLCP connection-less mode socket - local peer

 TestScenario:
     Touch a NFC device with LLCP connection-less service actived

 TestExpectedResults:
     Signal of target detected has been found.
*/
void tst_qllcpsocketremote::initTestCase()
{
    QNearFieldManager nfcManager;
    QSignalSpy targetDetectedSpy(&nfcManager, SIGNAL(targetDetected(QNearFieldTarget*)));
    nfcManager.startTargetDetection(QNearFieldTarget::NfcForumDevice);

    QString message("Please touch a NFC device with llcp client enabled");
    QNfcTestUtil::ShowAutoMsg(message, &targetDetectedSpy, 1);
    QTRY_VERIFY(!targetDetectedSpy.isEmpty());

    m_target = targetDetectedSpy.at(targetDetectedSpy.count() - 1).at(0).value<QNearFieldTarget *>();
    QVERIFY(m_target!=NULL);
    QVERIFY(m_target->accessMethods() & QNearFieldTarget::LlcpAccess);
    QVERIFY(m_target->uid() == QByteArray());
    QVERIFY(m_target->type() == QNearFieldTarget::NfcForumDevice);

    m_port = 35;
}

/*!
 Description:  Description: Receive the message and send the acknowledged identical message

 TestScenario: 1. Remote peer binds to local port
               2. Remote peer receives the message sending from the local peer
               3. Remote peer sends the echoed message to the local peer

 TestExpectedResults:
               1. Remote peer binds to local port successfully.
               2. The message has been received from remote peer.
               3. The message has be sent to local peer.
*/
void tst_qllcpsocketremote::echoServer()
{
    QLlcpSocket remoteSocket;
    // STEP 1:  bind the local port for current socket
    QSignalSpy readyReadSpy(&remoteSocket, SIGNAL(readyRead()));
    bool ret = remoteSocket.bind(m_port);
    QVERIFY(ret);

    QString message("Test 1: Wait client msg comming");
    QNfcTestUtil::ShowAutoMsg(message, &readyReadSpy);

    // STEP 2: Receive data from the peer which send messages to
    QByteArray inPayload;
    while(remoteSocket.hasPendingDatagrams()) {
        QByteArray tempBuffer;
        tempBuffer.resize(remoteSocket.pendingDatagramSize());
        quint8 remotePort = 0;
        QSignalSpy readyRead(&remoteSocket, SIGNAL(readyRead()));
        qint64 readSize = remoteSocket.readDatagram(tempBuffer.data()
                                                   , tempBuffer.size()
                                                   , &m_target, &remotePort);
        QVERIFY(readSize != -1);
        QVERIFY(remotePort > 0);
        inPayload.append(tempBuffer);
        qDebug() << "Server-- read inPayload size=" << inPayload.size();
        qDebug() << "Server-- read remotePort=" << remotePort;
        if (inPayload.size() >= (int)sizeof(quint16)) {
            break;
        }
        else {
            qDebug() << "Server-- not enough header";
            QTRY_VERIFY(!readyRead.isEmpty());
        }
    }

    QDataStream in(inPayload);
    quint16 headerSize = 0; // size of real echo payload
    in >> headerSize;
    qDebug() << "Server-- read headerSize=" << headerSize;
    while (inPayload.size() < headerSize + (int)sizeof(quint16)){
        QSignalSpy readyRead(&remoteSocket, SIGNAL(readyRead()));
        QTRY_VERIFY(!readyRead.isEmpty());
        if(remoteSocket.hasPendingDatagrams()) {
            QByteArray tempBuffer;
            tempBuffer.resize(remoteSocket.pendingDatagramSize());
            quint8 remotePort = 0;
            qint64 readSize = remoteSocket.readDatagram(tempBuffer.data()
                                                       , tempBuffer.size()
                                                       , &m_target, &remotePort);
            QVERIFY(readSize != -1);
            QVERIFY(remotePort > 0);
            inPayload.append(tempBuffer);
            qDebug() << "Server-- read long blocksize=" << inPayload.size();
        }
    }

    // STEP 3: Send the received message back to the intiated device.
    QSignalSpy errorSpy(&remoteSocket, SIGNAL(error(QLlcpSocket::Error)));
    QSignalSpy bytesWrittenSpy(&remoteSocket, SIGNAL(bytesWritten(qint64)));

    qDebug("Server-- write payload length = %d", inPayload.length());
    qint64 val = remoteSocket.writeDatagram(inPayload, m_target, m_port);
    QVERIFY(val != -1);

    QTRY_VERIFY(!bytesWrittenSpy.isEmpty());

    qint64 written = 0;
    qint32 lastSignalCount = 0;
    while(true)
    {
        for(int i = lastSignalCount; i < bytesWrittenSpy.count(); i++) {
            written += bytesWrittenSpy.at(i).at(0).value<qint64>();
        }
        lastSignalCount = bytesWrittenSpy.count();
        qDebug() << "current signal count = " << lastSignalCount;
        qDebug() << "written payload size = " << written;
        if (written < inPayload.size()) {
            QTRY_VERIFY(bytesWrittenSpy.count() > lastSignalCount);
        }
        else {
            break;
        }
    }

    qDebug() << "Overall bytesWritten = " << written;
    qDebug() << "Overall block size = " << inPayload.size();
    QVERIFY(written == inPayload.size());
    // make sure the no error signal emitted
    QCOMPARE(errorSpy.count(), 0);
}

/*!
 Description: Unit test for NFC LLCP connection-less mode socket - remote peer (passive)

 TestScenario: 1. Remote peer binds to local port ( already done in testCase1)
               2. Remote peer  receive datagram twice
               3. Remote peer waitForReadyRead

 TestExpectedResults:
               1. Remote peer binds to local port successfully.
               2. Remote peer  receive datagram twice successfully
               3. waitForReadyRead return true as long as readyReadSpy not empty
*/
void tst_qllcpsocketremote::testCase2()
{
    QLlcpSocket remoteSocket;
    quint8 localPort = 38;
    QSignalSpy readyReadSpy(&remoteSocket, SIGNAL(readyRead()));
    bool ret = remoteSocket.bind(localPort);
    QVERIFY(ret);

    // STEP 1:  bind the local port for current socket
    QString expectedMessage1("testcase2 string str1");
    QString expectedMessage2("testcase2 string str2");

    QString boxMessage("handshake 3");
    QNfcTestUtil::ShowAutoMsg(boxMessage,&readyReadSpy);

    QTRY_VERIFY(readyReadSpy.count() == 1);

    QByteArray datagram;
    if (remoteSocket.hasPendingDatagrams())
    {
       datagram.resize(remoteSocket.pendingDatagramSize());
       qint64 readSize = remoteSocket.readDatagram(datagram.data(), datagram.size());
       QVERIFY(readSize != -1);
    }

    QString receivedMessage1 = datagram.data();
    qDebug() << "receivedMessage1: " << receivedMessage1;
    QVERIFY(expectedMessage1 == receivedMessage1);

    QTRY_VERIFY(readyReadSpy.count() == 2);

     QByteArray datagram2;
    if (remoteSocket.hasPendingDatagrams())
    {
       datagram2.resize(remoteSocket.pendingDatagramSize());
       qint64 readSize = remoteSocket.readDatagram(datagram2.data(), datagram2.size());
       QVERIFY(readSize != -1);
    }
    QString receivedMessage2 = datagram2.data();
    qDebug() << "receivedMessage2: " << receivedMessage2;
    QVERIFY(expectedMessage2 == receivedMessage2);

    const int Timeout = 10 * 1000;
    ret = remoteSocket.waitForReadyRead(Timeout);

    QVERIFY(ret == false);
 }

void tst_qllcpsocketremote::cleanupTest()
{
}

QTEST_MAIN(tst_qllcpsocketremote);

#include "tst_qllcpsocketremote.moc"
