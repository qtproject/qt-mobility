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
    void initTestCase();
    void cleanupTestCase();

    void queuedWrittenTest();

    // ALERT£º Handshake required, do NOT¡¡change the sequence of handshaking testcases.
    void testCase1();   // handshake 1
    void testCase2();   // handshake 2
    void testCase3();

    void negTestCase1();
    void negTestCase2();
    void negTestCase3();
    void negTestCase4();
    void negTestCase5();


private:
     QNearFieldTarget *m_target; // not own
     quint8 m_port;
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
void tst_qllcpsocketlocal::initTestCase()
{
    QNearFieldManager nfcManager;
    QSignalSpy targetDetectedSpy(&nfcManager, SIGNAL(targetDetected(QNearFieldTarget*)));
    nfcManager.startTargetDetection(QNearFieldTarget::AnyTarget);

    QString message("Local Wait touch");
    QNfcTestUtil::ShowMessage(message);
    QTRY_VERIFY(!targetDetectedSpy.isEmpty());

    m_target = targetDetectedSpy.at(targetDetectedSpy.count() - 1).at(0).value<QNearFieldTarget *>();
    QVERIFY(m_target!=NULL);
    QVERIFY(m_target->accessMethods() & QNearFieldTarget::LlcpAccess);

    m_port = 35;
}

void tst_qllcpsocketlocal::cleanupTestCase()
{
}


/*!
 Description:  NFC LLCP connection-less mode socket - queued written buffer

 TestScenario:
                1. Local peer sends the "string1" message to the remote peer
                2. Local peer sends the "string2" message to the remote peer

 TestExpectedResults:
               1. Local peer write datagram successfully twice
*/
void tst_qllcpsocketlocal::queuedWrittenTest()
{
    QString message("string1");
    QString message2("string2");
    QLlcpSocket socket(this);

    QByteArray tmpArray(message.toAscii());
    const char* data =  tmpArray.data();
    qint64 strSize = message.size();
    qint64 val = socket.writeDatagram(data,strSize,m_target, m_port);
    QVERIFY(val != -1);

    QByteArray tmpArray2(message2.toAscii());
    const char* data2 =  tmpArray2.data();
    qint64 strSize2 = message2.size();
    qDebug() << "begin write the second time";
    qint64 val2 = socket.writeDatagram(data2,strSize2,m_target, m_port);
    qDebug() << "end write the second time" << val2;
    QVERIFY(val2 != -1);

    QSignalSpy bytesWrittenSpy(&socket, SIGNAL(bytesWritten(qint64)));
    QTRY_VERIFY(bytesWrittenSpy.count() == 2);

    QString box("queuedWrittenTest 1");
    QNfcTestUtil::ShowMessage(box);
}

/*!
 Description: testCase 1 for NFC LLCP connection-less mode socket - local peer

 TestScenario:
               1. Local peer binds to the remote peer
               2. Local peer sends the "connect-less unit test string" message to the remote peer
               3. Local peer receives the above message sending from the remote peer

 TestExpectedResults:
               1. Local peer binds to local port successfully.
               2. The message has be sent to remote peer.
               3. The message has been received from remote peer.
*/
void tst_qllcpsocketlocal::testCase1()
{
    QString message("testcase1 string");
    QLlcpSocket socket(this);

    // STEP 1:  bind the local port for current socket
    QSignalSpy readyReadSpy(&socket, SIGNAL(readyRead()));
    bool ret = socket.bind(m_port);
    QVERIFY(ret);

    QString messageBox("handshake 1");
    QNfcTestUtil::ShowMessage(messageBox);

    // STEP 2: Local peer sends the  message to the remote peer
    QSignalSpy errorSpy(&socket, SIGNAL(error(QLlcpSocket::Error)));
    QSignalSpy bytesWrittenSpy(&socket, SIGNAL(bytesWritten(qint64)));

    QByteArray tmpArray(message.toAscii());
    const char* data =  tmpArray.data();
    qint64 strSize = message.size();
    socket.writeDatagram(data,strSize,m_target, m_port);

    QTRY_VERIFY(bytesWrittenSpy.count() == 1);
    QList<QVariant> arguments = bytesWrittenSpy.takeFirst(); // take the first signal
    qint64 writtenSize  = arguments.at(0).value<qint64>();
    QCOMPARE(writtenSize, strSize);

    // STEP 3: Receive data from remote peer
    QTRY_VERIFY(!readyReadSpy.isEmpty());
    QByteArray datagram;
    while (socket.hasPendingDatagrams())
    {
       datagram.resize(socket.pendingDatagramSize());
       qint64 readSize = socket.readDatagram(datagram.data(), datagram.size());
       QVERIFY(readSize != -1);
    }

    // verify the echoed string is same as the original string
    QString receivedMessage = datagram.data();
    QVERIFY(message == receivedMessage);

    // make sure the no error signal emitted
    QVERIFY(errorSpy.isEmpty());
}


void tst_qllcpsocketlocal::testCase2()
{
    QString message("testcase2 string");
    QLlcpSocket socket(this);

    // STEP 1:  bind the local port for current socket
    QSignalSpy readyReadSpy(&socket, SIGNAL(readyRead()));
    bool ret = socket.bind(m_port);
    QVERIFY(ret);

    QString messageBox("handshake 2");
    QNfcTestUtil::ShowMessage(messageBox);

    // STEP 2:
    //qDebug() << "waitForBytesWritten begin: " << val;
    const int Timeout = 2 * 1000;
    ret = socket.waitForBytesWritten(Timeout);
    qDebug() << "waitForBytesWritten after: " << ret;

    QVERIFY(ret == false);
}

/*!
 Description:  connection-less state change checking
*/
void tst_qllcpsocketlocal::testCase3()
{
    QString message("testcase3");
    QNfcTestUtil::ShowMessage(message);
    QLlcpSocket socket(this);
    QCOMPARE(socket.state(), QLlcpSocket::UnconnectedState);
    QSignalSpy stateChangedSpy(&socket, SIGNAL(stateChanged(QLlcpSocket::State)));
    bool ret = socket.bind(m_port);
    QVERIFY(ret);
    QVERIFY(!stateChangedSpy.isEmpty());
    QCOMPARE(socket.state(), QLlcpSocket::BoundState);
}

/*!
 Description: bind negative test -invalid port
*/
void tst_qllcpsocketlocal::negTestCase1()
{
    QLlcpSocket socket(this);
    bool ret = socket.bind(65);
    QVERIFY(ret == false);
    ret = socket.bind(-1);
    QVERIFY(ret == false);
}

/*!
 Description: bind negative test II - double bind
*/
void tst_qllcpsocketlocal::negTestCase2()
{
    QLlcpSocket socket(this);
    bool ret = socket.bind(m_port);
    QVERIFY(ret == true);

    // bind again will cause failure
    ret = socket.bind(m_port);
    QVERIFY(ret == false);
}


/*!
 Description:  readDatagram negative testcase - invalid state machine
*/
void tst_qllcpsocketlocal::negTestCase3()
{
    QLlcpSocket socket(this);
    // readDatagram must be called before bind
    QByteArray datagram;
    datagram.resize(127);
    qint64 ret = socket.readDatagram(datagram.data(), datagram.size());
    QVERIFY(ret == -1);
}

/*!
 Description: writeDatagram negative testcase I - invalid port num
*/
void tst_qllcpsocketlocal::negTestCase4()
{
    QLlcpSocket socket(this);
    QString message = "Oops, Invalid port num for writeDatagram";
    QByteArray tmpArray(message.toAscii());
    const char* data =  tmpArray.data();
    qint64 strSize = message.size();
    qint8 invalidPort = -1;
    qint64 ret = socket.writeDatagram(data,strSize,m_target, invalidPort);
    QVERIFY(ret == -1);
}

/*!
 Description: negative testcase II - invalid usage of connection-oriented API
*/
void tst_qllcpsocketlocal::negTestCase5()
{
    QLlcpSocket socket(this);
    qDebug() << "negTestCase5: " <<m_port;
    bool bindOk = socket.bind(m_port);
    QVERIFY(bindOk == true);
    qDebug() << "negTestCase5 bindOk";

    QSignalSpy errorSpy(&socket, SIGNAL(error(QLlcpSocket::Error)));
    socket.connectToService(m_target,"uri");
    QTRY_VERIFY(errorSpy.count() == 1);

    socket.disconnectFromService();
    QTRY_VERIFY(errorSpy.count() == 2);

    QVERIFY(socket.waitForConnected() == false);
    QVERIFY(socket.waitForDisconnected() == false);

     qDebug() << "negTestCase5 writeDatagram";
    QString message = "Oops, must follow a port parameter";
    QByteArray tmpArray(message.toAscii());
    const char* data =  tmpArray.data();
    qint64 strSize = message.size();
    qint64 ret = socket.writeDatagram(data,strSize);

     qDebug() << "negTestCase5 writeDatagram end";
    QVERIFY(ret == -1);
}

QTEST_MAIN(tst_qllcpsocketlocal);

#include "tst_qllcpsocketlocal.moc"
