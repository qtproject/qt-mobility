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
    void testCase0();   // Intial handshake
    void testCase1();   // handshake 1,2
    void testCase2();   // handshake 3

    void cleanupTest();

private:
     QNearFieldManager *m_nfcManager;
     QNearFieldTarget *m_target;
     QLlcpSocket *m_socket;
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
void tst_qllcpsocketremote::testCase0()
{
    m_nfcManager = new QNearFieldManager;
    QSignalSpy targetDetectedSpy(m_nfcManager, SIGNAL(targetDetected(QNearFieldTarget*)));
    m_nfcManager->startTargetDetection(QNearFieldTarget::AnyTarget);

    QString message("Remote wait touch");
    QNfcTestUtil::ShowMessage(message);
    QTRY_VERIFY(!targetDetectedSpy.isEmpty());

    m_target = targetDetectedSpy.at(targetDetectedSpy.count() - 1).at(0).value<QNearFieldTarget *>();
    QVERIFY(m_target!=NULL);
    QVERIFY(m_target->accessMethods() & QNearFieldTarget::LlcpAccess);

    m_port = 35;
    m_socket = new QLlcpSocket;
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
void tst_qllcpsocketremote::testCase1()
{
    // STEP 1:  bind the local port for current socket
    QSignalSpy readyReadSpy(m_socket, SIGNAL(readyRead()));
    bool ret = m_socket->bind(m_port);

    QString message("handshake 1");
    QNfcTestUtil::ShowMessage(message);

    QTRY_VERIFY(readyReadSpy.count() == 1);
    QVERIFY(ret);

    // STEP 2: Receive data from the peer which send messages to
    QByteArray datagram;
    while (m_socket->hasPendingDatagrams())
    {
       datagram.resize(m_socket->pendingDatagramSize());
       qint64 readSize = m_socket->readDatagram(datagram.data(), datagram.size());
       QVERIFY(readSize != -1);
    }

    // STEP 3: Send the received message back to the intiated device.
    QSignalSpy errorSpy(m_socket, SIGNAL(error(QLlcpSocket::Error)));
    QSignalSpy bytesWrittenSpy(m_socket, SIGNAL(bytesWritten(qint64)));

    m_socket->writeDatagram(datagram,m_target, m_port);

    QTRY_VERIFY(bytesWrittenSpy.count() == 1);
    QList<QVariant> arguments = bytesWrittenSpy.takeFirst(); // take the first signal
    qint64 writtenSize  = arguments.at(0).value<qint64>();

    QString message2("handshake 2");
    QNfcTestUtil::ShowMessage(message2);

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
    QSignalSpy readyReadSpy(m_socket, SIGNAL(readyRead()));
    bool ret = m_socket->bind(m_port);

    QString message("handshake 3");
    QNfcTestUtil::ShowMessage(message);

    qDebug() << "spy count: " << readyReadSpy.count();
    QTRY_VERIFY(readyReadSpy.count() == 1);

    const int Timeout = 10 * 1000;
    ret = m_socket->waitForReadyRead(Timeout);

    QVERIFY(ret);
 }

void tst_qllcpsocketremote::cleanupTest()
{
   delete m_nfcManager;
   delete m_socket;
}

QTEST_MAIN(tst_qllcpsocketremote);

#include "tst_qllcpsocketremote.moc"
