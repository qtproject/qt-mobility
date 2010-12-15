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

    // ALERT£º Handshake required, do NOT¡¡change the sequence of handshaking testcases.
    void initTestCase();
    void cleanupTestCase();
    void queuedWrittenTest();
    void testCase1();   // handshake 1
    void testCase2(); // handshake 2

private:
     QNearFieldTarget *m_target;
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
    QNearFieldManager *nfcManager = new QNearFieldManager;
    QSignalSpy targetDetectedSpy(nfcManager, SIGNAL(targetDetected(QNearFieldTarget*)));
    nfcManager->startTargetDetection(QNearFieldTarget::AnyTarget);

    QString message("Remote wait touch");
    QNfcTestUtil::ShowMessage(message);
    QTRY_VERIFY(!targetDetectedSpy.isEmpty());

    m_target = targetDetectedSpy.first().at(0).value<QNearFieldTarget *>();
    QVERIFY(m_target!=NULL);
    QVERIFY(m_target->accessMethods() & QNearFieldTarget::LlcpAccess);

    m_port = 35;
}

void tst_qllcpsocketremote::cleanupTestCase()
{
}

/*!
 Description: testCase 1 for NFC LLCP connection-less mode socket - local peer

 TestScenario:
               1. Local peer binds to the remote peer
               2. Local peer receives the datagram  sending from the remote peer

 TestExpectedResults:
               1. Local peer binds to local port successfully.
               2. Local peer receives the  datagram successfully
*/
void tst_qllcpsocketremote::queuedWrittenTest()
{
    QString message("string1");
    QString message2("string2");
    QLlcpSocket socket(this);

    QSignalSpy readyReadSpy(&socket, SIGNAL(readyRead()));
    bool ret = socket.bind(m_port);

    QString box("handshake 1");
    QNfcTestUtil::ShowMessage(box);

    qDebug() << "readspy count: " << readyReadSpy.count();
    QTRY_VERIFY(readyReadSpy.count() == 2);
    QVERIFY(ret);

    // STEP 2: Receive data from the peer which send messages to
    QByteArray datagram;
    while (socket.hasPendingDatagrams())
    {
       datagram.resize(socket.pendingDatagramSize());
       qint64 readSize = socket.readDatagram(datagram.data(), datagram.size());
       qDebug() << "readStr:" << datagram.data();
       QVERIFY(readSize != -1);
    }

}

/*!
 Description: Unit test for NFC LLCP connection-less mode socket - remote peer (passive)

 TestScenario: 1. Remote peer binds to local port
               2. Remote peer receives the message sending from the local peer
               3. Remote peer sends the echoed message to the local peer

 TestExpectedResults:
               1. Remote peer binds to local port successfully.
               2. The message has been received from remote peer.
               3. The message has be sent to l peer.
*/
void tst_qllcpsocketremote::testCase1()
{  
    // STEP 1:  bind the local port for current socket
    QLlcpSocket socket(this);
    QSignalSpy readyReadSpy(&socket, SIGNAL(readyRead()));
    bool ret = socket.bind(m_port);

    QString message("handshake 1");
    QNfcTestUtil::ShowMessage(message);

    QTRY_VERIFY(readyReadSpy.count() == 1);
    QVERIFY(ret);

    // STEP 2: Receive data from the peer which send messages to
    QByteArray datagram;
    while (socket.hasPendingDatagrams())
    {
       datagram.resize(socket.pendingDatagramSize());
       qint64 readSize = socket.readDatagram(datagram.data(), datagram.size());
       QVERIFY(readSize != -1);
    }

    // STEP 3: Send the received message back to the intiated device.
    QSignalSpy errorSpy(&socket, SIGNAL(error(QLlcpSocket::Error)));
    QSignalSpy bytesWrittenSpy(&socket, SIGNAL(bytesWritten(qint64)));

    socket.writeDatagram(datagram,m_target, m_port);

    QTRY_VERIFY(bytesWrittenSpy.count() == 1);
    QList<QVariant> arguments = bytesWrittenSpy.takeFirst(); // take the first signal
    qint64 writtenSize  = arguments.at(0).value<qint64>();

    // make sure the no error signal emitted
    QCOMPARE(errorSpy.count(), 0);
}


/*!
 Description: Unit test for NFC LLCP connection-less mode socket - remote peer (passive)

 TestScenario: 1. Remote peer binds to local port
               2. Remote peer waitForReadyRead

 TestExpectedResults:
               1. Remote peer binds to local port successfully.
               2. waitForReadyRead return true as long as readyReadSpy not empty
*/
void tst_qllcpsocketremote::testCase2()
{
    // STEP 1:  bind the local port for current socket
    QLlcpSocket socket(this);
    QSignalSpy readyReadSpy(&socket, SIGNAL(readyRead()));
    bool ret = socket.bind(m_port);

    QString message("handshake 2");
    QNfcTestUtil::ShowMessage(message);

    QTRY_VERIFY(readyReadSpy.count() == 1);

    const int Timeout = 10 * 1000;
    ret = socket.waitForReadyRead(Timeout);

    QString message2("handshake 3");
    QNfcTestUtil::ShowMessage(message2);
    QVERIFY(ret);
 }

QTEST_MAIN(tst_qllcpsocketremote);

#include "tst_qllcpsocketremote.moc"
