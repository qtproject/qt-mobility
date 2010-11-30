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

class tst_qllcpsocketremote : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCase1();
    void testCase1_data();

private:
     QNearFieldTarget *m_target;
};


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

    QString message("Wait for NFC device touch...");
    QNfcTestUtil::ShowMessage(message);
    QTRY_VERIFY(!targetDetectedSpy.isEmpty());

    m_target = targetDetectedSpy.first().at(0).value<QNearFieldTarget *>();
    QVERIFY(m_target!=NULL);
    QVERIFY(m_target->accessMethods() & QNearFieldTarget::LlcpAccess);
}

void tst_qllcpsocketremote::cleanupTestCase()
{
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
    QFETCH(quint8, port);
    QFETCH(bool, enableWaiterFlag);

    // STEP 1:  bind the local port for current socket
    QLlcpSocket socket(this);
    bool ret = socket.bind(port);
    QVERIFY(ret);

    // STEP 2: Receive data from the peer which send messages to
    QSignalSpy readyReadSpy(&socket, SIGNAL(readyRead()));
    QTRY_VERIFY(readyReadSpy.count() == 1);
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

    socket.writeDatagram(datagram,m_target, port);

    if(enableWaiterFlag)
    {
        const int Timeout = 10 * 1000;
        ret = socket.waitForBytesWritten(Timeout);
        QVERIFY(ret);
    }

    QTRY_VERIFY(bytesWrittenSpy.count() == 1);
    QList<QVariant> arguments = bytesWrittenSpy.takeFirst(); // take the first signal
    qint64 writtenSize  = arguments.at(0).value<qint64>();

    // make sure the no error signal emitted
    QCOMPARE(errorSpy.count(), 0);
}

void tst_qllcpsocketremote::testCase1_data()
{
    QTest::addColumn<quint8>("port");
    QTest::addColumn<bool>("enableWait");
    quint8 port = 35;
    QTest::newRow("row1") << port << false;
    QTest::newRow("row2") << port << true;
}


QTEST_MAIN(tst_qllcpsocketremote);

#include "tst_qllcpsocketremote.moc"
