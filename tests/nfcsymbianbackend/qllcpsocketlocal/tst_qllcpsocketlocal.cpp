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

class tst_qllcpsocketlocal : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCase1();
    void testCase1_data();
    void testCase2();

    void negTestCase1();
    void negTestCase1_data();
    void negTestCase2();
    void negTestCase3();
    void negTestCase4();
    void negTestCase5();

private:
     QNearFieldTarget *m_target; // not own
     quint8 m_port;
};

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

    QString message("Please touch a NFC device with llcp connection-less mode client enabled: port = 35");
    QNfcTestUtil::ShowMessage(message);
    QTRY_VERIFY(!targetDetectedSpy.isEmpty());

    m_target = targetDetectedSpy.first().at(0).value<QNearFieldTarget *>();
    QVERIFY(m_target!=NULL);
    QVERIFY(m_target->accessMethods() & QNearFieldTarget::LlcpAccess);

    m_port = 35;
}

void tst_qllcpsocketlocal::cleanupTestCase()
{
}

/*!
 Description: testCase 1 for NFC LLCP connection-less mode socket - local peer

 TestScenario:
               1. Local peer sends the "connect-less unit test string" message to the remote peer
               2. Local peer binds to the remote peer
               3. Local peer receives the above message sending from the remote peer

 TestExpectedResults:
               1. The message has be sent to remote peer.
               2. Local peer binds to local port successfully.
               3. The message has been received from remote peer.
*/
void tst_qllcpsocketlocal::testCase1()
{
    QFETCH(quint8, port);
    QFETCH(QString, message);
    QFETCH(bool, enableWaiterFlag);
    QLlcpSocket socket(this);

    // STEP 1: Local peer sends the  message to the remote peer
    QSignalSpy errorSpy(&socket, SIGNAL(error(QLlcpSocket::Error)));
    QSignalSpy bytesWrittenSpy(&socket, SIGNAL(bytesWritten(qint64)));

    const char* data = (const char *) message.data();
    qint64 strSize = message.size();
    socket.writeDatagram(data,strSize,m_target, port);

    if (enableWaiterFlag)
    {
        const int Timeout = 10 * 1000;
        bool ret = socket.waitForBytesWritten(Timeout);
        QVERIFY(ret);
    }

    QTRY_VERIFY(bytesWrittenSpy.count() == 1);
    QList<QVariant> arguments = bytesWrittenSpy.takeFirst(); // take the first signal
    qint64 writtenSize  = arguments.at(0).value<qint64>();
    QCOMPARE(writtenSize, strSize);

    // STEP 2:  bind the local port for current socket
    QSignalSpy readyReadSpy(&socket, SIGNAL(readyRead()));
    bool ret = socket.bind(port);
    QVERIFY(ret);

    // STEP 3: Receive data from remote peer
    QTRY_VERIFY(readyReadSpy.count() == 1);
    QByteArray datagram;
    while (socket.hasPendingDatagrams())
    {
       datagram.resize(socket.pendingDatagramSize());
       qint64 readSize = socket.readDatagram(datagram.data(), datagram.size());
       QVERIFY(readSize != -1);
    }

    if (enableWaiterFlag)
    {
        const int Timeout = 10 * 1000;
        ret = socket.waitForReadyRead(Timeout);
        QVERIFY(ret == false);
    }

    // verify the echoed string is same as the original string
    QString receivedMessage = datagram.constData();
    QVERIFY(message == receivedMessage);

    // make sure the no error signal emitted
    QVERIFY(errorSpy.isEmpty());
}

void tst_qllcpsocketlocal::testCase1_data()
{
    QTest::addColumn<quint8>("port");
    QTest::addColumn<QString>("socketContent");
    QTest::addColumn<bool>("enableWait");
    quint8 port = 35;
    QTest::newRow("row1") << port << "llcp conenction mode test string 1" << false;
    QTest::newRow("row2") << port << "llcp conenction mode test string 2" << true;
}

/*!
 Description:  connection-less state change checking
*/
void tst_qllcpsocketlocal::testCase2()
{
    QLlcpSocket socket(this);
    QCOMPARE(socket.state(), QLlcpSocket::UnconnectedState);
    QSignalSpy stateChangedSpy(&socket, SIGNAL(stateChanged()));
    socket.bind(m_port);
    QVERIFY(!stateChangedSpy.isEmpty());
    QCOMPARE(socket.state(), QLlcpSocket::BoundState);
}


/*!
 Description: bind negative test -invalid port
*/
void tst_qllcpsocketlocal::negTestCase1()
{
    QLlcpSocket socket(this);
    bool ret = socket.bind(m_port);
    QVERIFY(ret == true);
    // bind again will cause failure
    ret = socket.bind(m_port);
    QVERIFY(ret == false);
    // bind invalid port cause failure
    quint8 unknownPort = -1;
}

void tst_qllcpsocketlocal::negTestCase1_data()
{
    QTest::addColumn<quint8>("port");

    quint8 sdpPort = 14;
    quint8 tooBigPort = 65;
    quint8 invalidPort = -1;

    QTest::newRow("row1") << sdpPort;
    QTest::newRow("row2") << tooBigPort;
    QTest::newRow("row3") << invalidPort;
}

/*!
 Description: bind negative test II - double bind
*/
void tst_qllcpsocketlocal::negTestCase2()
{
    QLlcpSocket socket(this);
    bool ret = socket.bind(m_port);
    QVERIFY(ret == true);

    QSignalSpy errorSpy(&socket, SIGNAL(error(QLlcpSocket::Error)));
    // bind again will cause failure
    ret = socket.bind(m_port);
    QVERIFY(ret == false);
    QVERIFY(!errorSpy.isEmpty());
}


/*!
 Description:  readDatagram negative testcase - invalid state machine
*/
void tst_qllcpsocketlocal::negTestCase3()
{
    QLlcpSocket socket(this);
    // readDatagram must be called before bind
    QSignalSpy errorSpy(&socket, SIGNAL(error(QLlcpSocket::Error)));
    QByteArray datagram;
    datagram.resize(127);
    qint64 ret = socket.readDatagram(datagram.data(), datagram.size());
    QVERIFY(ret == -1);
    QVERIFY(!errorSpy.isEmpty());
}

/*!
 Description: writeDatagram negative testcase I - invalid port num
*/
void tst_qllcpsocketlocal::negTestCase4()
{
    QLlcpSocket socket(this);
    QString message = "Oops, Invalid port num for writeDatagram";
    const char* data = (const char *) message.data();
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

    QSignalSpy errorSpy(&socket, SIGNAL(error(QLlcpSocket::Error)));
    socket.connectToService(m_target,"uri");
    QVERIFY(errorSpy.count() == 1);

    socket.disconnectFromService();
    QVERIFY(errorSpy.count() == 2);

    QVERIFY(socket.waitForConnected() == false);
    QVERIFY(socket.waitForDisconnected() == false);

    QString message = "Oops, must follow a port parameter";
    const char* data = (const char *) message.data();
    qint64 strSize = message.size();
    qint64 ret = socket.writeDatagram(data,strSize);
    QVERIFY(ret == -1);
}

QTEST_MAIN(tst_qllcpsocketlocal);

#include "tst_qllcpsocketlocal.moc"
