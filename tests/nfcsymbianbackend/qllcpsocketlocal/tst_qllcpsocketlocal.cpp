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
Q_DECLARE_METATYPE(QLlcpSocket::State);

class tst_qllcpsocketlocal : public QObject
{
    Q_OBJECT

public:
    tst_qllcpsocketlocal();
private Q_SLOTS:

    void testCase0();

    // ALERT£º Handshake required, do NOT¡¡change the sequence of handshaking testcases.
    void testCase1();   // handshake 1,2
    void testCase2();   // handshake 3
    void testCase3();

    void coverageTest1();

    void negTestCase1();
    void negTestCase2();

    void negTestCase3();
    void negTestCase4();
    void negTestCase5();
    void negTestCase6();

    void dummyTest2();
    void dummyTest3();
    void dummyTest4();

    void waiter();

private:
     QNearFieldManager *m_nfcManager; //own
     QNearFieldTarget *m_target; // not own
     quint8 m_port;
     QLlcpSocket *socket;
};

tst_qllcpsocketlocal::tst_qllcpsocketlocal()
{
    qRegisterMetaType<QNearFieldTarget *>("QNearFieldTarget*");
    qRegisterMetaType<QNearFieldTarget *>("QLlcpSocket::Error");
    qRegisterMetaType<QNearFieldTarget *>("QLlcpSocket::State");
}


/*!
 Description: Init test case for NFC LLCP connection-less mode socket - local peer

 TestScenario:
     Touch a NFC device with LLCP connection-less service actived

 TestExpectedResults:
     Signal of target detected has been found.
*/
void tst_qllcpsocketlocal::testCase0()
{
    m_nfcManager = new QNearFieldManager;
    QSignalSpy targetDetectedSpy(m_nfcManager, SIGNAL(targetDetected(QNearFieldTarget*)));
    m_nfcManager->startTargetDetection(QNearFieldTarget::AnyTarget);

    QString message("Local Wait touch");
    QNfcTestUtil::ShowMessage(message);
    QTRY_VERIFY(!targetDetectedSpy.isEmpty());

    m_target = targetDetectedSpy.at(targetDetectedSpy.count() - 1).at(0).value<QNearFieldTarget *>();
    QVERIFY(m_target!=NULL);
    QVERIFY(m_target->accessMethods() & QNearFieldTarget::LlcpAccess);
    m_port = 35;

    socket = new QLlcpSocket;
}


/*!
 Description: Send the message and Receive the acknowledged identical message

 TestScenario:
               1. Local peer binds to the remote peer
               2. Local peer sends the "testcase1 string" message to the remote peer
               3. Local peer receives the above message sending from the remote peer

 TestExpectedResults:
               1. Local peer binds to local port successfully.
               2. The message has be sent to remote peer.
               3. The message has been received from remote peer.
*/
void tst_qllcpsocketlocal::testCase1()
{
    QString message("testcase1 string");
    //QLlcpSocket socket(this);

    // negative test:  readDatagram must be called before bind
    QByteArray tmpForReadArray;
    tmpForReadArray.resize(127);
    qint64 ret = socket->readDatagram(tmpForReadArray.data(), tmpForReadArray.size());
    QVERIFY(ret == -1);

    QCOMPARE(socket->state(), QLlcpSocket::UnconnectedState);
    QSignalSpy stateChangedSpy(socket, SIGNAL(stateChanged(QLlcpSocket::State)));

    // STEP 1:  bind the local port for current socket
    QSignalSpy readyReadSpy(socket, SIGNAL(readyRead()));
    bool retBool = socket->bind(m_port);
    QVERIFY(retBool);
    QVERIFY(!stateChangedSpy.isEmpty());
    QCOMPARE(socket->state(), QLlcpSocket::BoundState);

    QString messageBox("handshake 1");
    QNfcTestUtil::ShowMessage(messageBox);

    // STEP 2: Local peer sends the  message to the remote peer
    QSignalSpy errorSpy(socket, SIGNAL(error(QLlcpSocket::Error)));
    QSignalSpy bytesWrittenSpy(socket, SIGNAL(bytesWritten(qint64)));

    QByteArray tmpArray(message.toAscii());
    const char* data =  tmpArray.data();
    qint64 strSize = message.size();
    socket->writeDatagram(data,strSize,m_target, m_port);

    QTRY_VERIFY(bytesWrittenSpy.count() == 1);
    QList<QVariant> arguments = bytesWrittenSpy.takeFirst(); // take the first signal
    qint64 writtenSize  = arguments.at(0).value<qint64>();
    QCOMPARE(writtenSize, strSize);

    QString messageBox2("handshake 2");
    QNfcTestUtil::ShowMessage(messageBox2);

    // STEP 3: Receive data from remote peer
    QTRY_VERIFY(!readyReadSpy.isEmpty());
    QByteArray datagram;
    while (socket->hasPendingDatagrams())
    {
       datagram.resize(socket->pendingDatagramSize());
       qint64 readSize = socket->readDatagram(datagram.data(), datagram.size());
       QVERIFY(readSize != -1);
    }

    // verify the echoed string is same as the original string
    QString receivedMessage = datagram.data();
    QVERIFY(message == receivedMessage);

    // make sure the no error signal emitted
    QVERIFY(errorSpy.isEmpty());
}

/*!
 Description: waitForBytesWritten test

 TestScenario:
               1. Spy bytes written signal
               2. Local peer sends the "testcase1 string" message to the remote peer
               3. Local peer receives the above message sending from the remote peer

 TestExpectedResults:
               1. The message is sending to remote peer
               2. Did not spy bytes written signal during the sending
               3. call waitForBytesWritten successfully
*/
void tst_qllcpsocketlocal::testCase2()
{
    QString message("testcase2 string");
    // QLlcpSocket socket(this);

    // STEP1:
    QSignalSpy bytesWrittenSpy(socket, SIGNAL(bytesWritten(qint64)));

    // STEP2:
    QByteArray tmpArray(message.toAscii());
    const char* data =  tmpArray.data();
    qint64 strSize = message.size();
    socket->writeDatagram(data,strSize,m_target, m_port);
    QVERIFY(bytesWrittenSpy.isEmpty());

    // STEP3:
    const int Timeout = 2 * 1000;
    bool ret = socket->waitForBytesWritten(Timeout);

    QString messageBox("handshake 3");
    QNfcTestUtil::ShowMessage(messageBox);

    QVERIFY(ret == true);
}


/*!
 Description:  NFC LLCP connection-less mode socket - queued written buffer

 TestScenario:
                1. Local peer sends the "string1" message to the remote peer
                2. Local peer sends the "string2" message to the remote peer

 TestExpectedResults:
               1. Local peer write datagram successfully twice
*/
void tst_qllcpsocketlocal::testCase3()
{
    QString message("string1");
    QString message2("string2");
    //QLlcpSocket socket(this);

    QByteArray tmpArray(message.toAscii());
    const char* data =  tmpArray.data();
    qint64 strSize = message.size();
    qint64 val = socket->writeDatagram(data,strSize,m_target, m_port);
    QVERIFY(val != -1);

    QByteArray tmpArray2(message2.toAscii());
    const char* data2 =  tmpArray2.data();
    qint64 strSize2 = message2.size();
    qDebug() << "begin write the second time";
    qint64 val2 = socket->writeDatagram(data2,strSize2,m_target, m_port);
    qDebug() << "end write the second time" << val2;
    QVERIFY(val2 != -1);

    QSignalSpy bytesWrittenSpy(socket, SIGNAL(bytesWritten(qint64)));
    QTRY_VERIFY(bytesWrittenSpy.count() == 2);
}

/*!
 Description: negative testcase II - invalid usage of connection-oriented API
*/
void tst_qllcpsocketlocal::coverageTest1()
{
    /*
    QLlcpSocket socket;
    qDebug() << "coverageTest1: " <<m_port;
    bool bindOk = socket->bind(m_port);
    QVERIFY(bindOk == true);
    qDebug() << "coverageTest1 bindOk";
    */
    QSignalSpy errorSpy(socket, SIGNAL(error(QLlcpSocket::Error)));
    socket->connectToService(m_target,"uri");
    QTRY_VERIFY(errorSpy.count() == 1);

    socket->disconnectFromService();
    QTRY_VERIFY(errorSpy.count() == 2);

    QVERIFY(socket->waitForConnected() == false);
    QVERIFY(socket->waitForDisconnected() == false);

    qDebug() << "coverageTest1 writeDatagram";
    QString message = "Oops, must follow a port parameter";
    QByteArray tmpArray(message.toAscii());
    const char* data =  tmpArray.data();
    qint64 strSize = message.size();
    qint64 ret = socket->writeDatagram(data,strSize);

    qDebug() << "coverageTest1 writeDatagram end";
    QVERIFY(ret == -1);
}


/*!
 Description: writeDatagram negative testcase I - invalid port num
*/
void tst_qllcpsocketlocal::negTestCase1()
{
    QString message = "Oops, Invalid port num for writeDatagram";
    QByteArray tmpArray(message.toAscii());
    const char* data =  tmpArray.data();
    qint64 strSize = message.size();
    qint8 invalidPort = -1;
    qint64 ret = socket->writeDatagram(data,strSize,m_target, invalidPort);
    QVERIFY(ret == -1);
}


/*!
 Description: bind negative test -invalid port & double bind
*/
void tst_qllcpsocketlocal::negTestCase2()
{
    // bind again will cause failure

    QLlcpSocket *localSocket = new QLlcpSocket;
    bool ret = localSocket->bind(65);
    QVERIFY(ret == false);
    delete localSocket;

    QTest::qWait(2000);

    bool ret2 = socket->bind(m_port);
    QVERIFY(ret2 == false);

    delete socket;
    socket = NULL;

    /*
    QTest::qWait(2000);
    int invalidPort = -1;
    ret = localSocket->bind(invalidPort);
    QVERIFY(ret == false);
    */
}

void tst_qllcpsocketlocal::negTestCase3()
{
    // bind again will cause failure
    bool ret2 = socket->bind(m_port);
    QVERIFY(ret2 == false);

    delete socket;
    socket = NULL;
}

void tst_qllcpsocketlocal::waiter()
{
    QTest::qWait(2000);
}

void tst_qllcpsocketlocal::negTestCase4()
{

    QLlcpSocket *localSocket = new QLlcpSocket;
    bool ret = localSocket->bind(65);
    QVERIFY(ret == false);

    int invalidPort = -1;
    ret = localSocket->bind(invalidPort);
    QVERIFY(ret == false);

    delete localSocket;
}

void tst_qllcpsocketlocal::negTestCase5()
{
    QLlcpSocket *localSocket = new QLlcpSocket;
    bool ret = localSocket->bind(65);
    QVERIFY(ret == false);
    delete localSocket;
}


void tst_qllcpsocketlocal::negTestCase6()
{
    QLlcpSocket *localSocket = new QLlcpSocket;
    int invalidPort = -1;
    bool ret = localSocket->bind(invalidPort);
    QVERIFY(ret == false);
    delete localSocket;
}

void tst_qllcpsocketlocal::dummyTest2()
{
}

// This is promatic functioanlity, causing program closed
void tst_qllcpsocketlocal::dummyTest3()
{
    delete m_nfcManager;
}

void tst_qllcpsocketlocal::dummyTest4()
{
    delete socket;
}


QTEST_MAIN(tst_qllcpsocketlocal);

#include "tst_qllcpsocketlocal.moc"
