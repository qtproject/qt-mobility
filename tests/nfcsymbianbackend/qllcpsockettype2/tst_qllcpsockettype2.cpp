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

QString TestUri("urn:nfc:xsn:nokia:symbiantest");
class tst_qllcpsockettype2 : public QObject
{
    Q_OBJECT

public:
    tst_qllcpsockettype2();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void echo();
    void echo_data();
    void echo_wait();
    void echo_wait_data();
};

tst_qllcpsockettype2::tst_qllcpsockettype2()
{
    qRegisterMetaType<QNearFieldTarget*>("QNearFieldTarget*");
}

void tst_qllcpsockettype2::initTestCase()
{
}

void tst_qllcpsockettype2::cleanupTestCase()
{
}

/*!
 Description: Unit test for NFC LLCP socket async functions

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
*/
void tst_qllcpsockettype2::echo()
{
    QFETCH(QString, uri);
    QFETCH(QString, hint);
    QFETCH(QString, echo);
    QNearFieldManager nfcManager;
    QSignalSpy targetDetectedSpy(&nfcManager, SIGNAL(targetDetected(QNearFieldTarget*)));
    nfcManager.startTargetDetection(QNearFieldTarget::AnyTarget);

    QNfcTestUtil::ShowMessage(hint);
    QTRY_VERIFY(!targetDetectedSpy.isEmpty());

    QNearFieldTarget *target = targetDetectedSpy.first().at(0).value<QNearFieldTarget*>();
    QVERIFY(target!=NULL);
    QVERIFY(target->accessMethods() & QNearFieldTarget::LlcpAccess);

    QLlcpSocket socket(this);

    socket.connectToService(target, uri);
    QSignalSpy connectedSpy(&socket, SIGNAL(connected()));
    QSignalSpy errorSpy(&socket, SIGNAL(error(QLlcpSocket::Error)));
    QTRY_VERIFY(!connectedSpy.isEmpty());

    QSignalSpy readyReadSpy(&socket, SIGNAL(readyRead()));
    //Send data to server
    QSignalSpy bytesWrittenSpy(&socket, SIGNAL(bytesWritten(qint64)));

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << (quint16)echo.length();
    out << echo;

    socket.write(block);

    QTRY_VERIFY(!bytesWrittenSpy.isEmpty());
    qint64 written = bytesWrittenSpy.first().at(0).value<qint64>();

    while (written < echo.length())
        {
        QSignalSpy bytesWrittenSpy(&socket, SIGNAL(bytesWritten(qint64)));
        QTRY_VERIFY(!bytesWrittenSpy.isEmpty());
        written = bytesWrittenSpy.first().at(0).value<qint64>();
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

    while (socket.bytesAvailable() < blockSize){
        QSignalSpy readyRead(&socket, SIGNAL(readyRead()));
        QTRY_VERIFY(!readyRead.isEmpty());
    }
    QString echoed;
    in >> echoed;

    //test the echoed string is same as the original string
    QVERIFY(echo == echoed);

    socket.disconnectFromService();

    //Now data has been sent,check the if existing error
    QVERIFY(errorSpy.isEmpty());
}

void tst_qllcpsockettype2::echo_data()
{
    QTest::addColumn<QString>("uri");
    QTest::addColumn<QString>("hint");
    QTest::addColumn<QString>("echo");
    QTest::newRow("0") << TestUri
            << "Client: uri = echo"
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
*/
void tst_qllcpsockettype2::echo_wait()
    {
    QFETCH(QString, uri);
    QFETCH(QString, hint);
    QFETCH(QString, echo);
    QNearFieldManager nfcManager;
    QSignalSpy targetDetectedSpy(&nfcManager, SIGNAL(targetDetected(QNearFieldTarget*)));
    nfcManager.startTargetDetection(QNearFieldTarget::AnyTarget);

    QNfcTestUtil::ShowMessage(hint);
    QTRY_VERIFY(!targetDetectedSpy.isEmpty());

    QNearFieldTarget *target = targetDetectedSpy.first().at(0).value<QNearFieldTarget *>();
    QVERIFY(target!=NULL);
    QVERIFY(target->accessMethods() & QNearFieldTarget::LlcpAccess);

    QLlcpSocket socket(this);

    const int Timeout = 10 * 1000;

    socket.connectToService(target, uri);

    bool ret = socket.waitForConnected(Timeout);
    QVERIFY(ret);

    QSignalSpy errorSpy(&socket, SIGNAL(error(QLlcpSocket::Error)));

    //Send data to server
    QSignalSpy bytesWrittenSpy(&socket, SIGNAL(bytesWritten(qint64)));

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << (quint16)echo.length();
    out << echo;

    socket.write(block);

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
        written = bytesWrittenSpy.first().at(0).value<qint64>();
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

    while (socket.bytesAvailable() < blockSize){
        ret = socket.waitForReadyRead(Timeout);
        QVERIFY(ret);
    }
    QString echoed;
    in >> echoed;

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
    QTest::addColumn<QString>("hint");
    QTest::addColumn<QString>("echo");
    QTest::newRow("0") << TestUri
            << "Please touch a NFC device with llcp client enabled: uri = " + TestUri
            << "echo";
}

QTEST_MAIN(tst_qllcpsockettype2);

#include "tst_qllcpsockettype2.moc"
