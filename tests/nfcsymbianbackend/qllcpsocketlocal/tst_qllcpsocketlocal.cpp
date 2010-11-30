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

public:
    tst_qllcpsocketlocal();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCase1();
    void testCase1_data();
};

tst_qllcpsocketlocal::tst_qllcpsocketlocal()
{
}

void tst_qllcpsocketlocal::initTestCase()
{
}

void tst_qllcpsocketlocal::cleanupTestCase()
{
}

/*!
 Description: Unit test for NFC LLCP connection-less mode socket - local peer

 TestScenario: 1. Touch a NFC device with LLCP connection-less service actived
               2. Local peer binds to the remote peer
               3. Local peer sends the "connect-less unit test string" message to the remote peer
               4. Local peer receives the above message sending from the remote peer

 TestExpectedResults:
               1. Signal of target detected has been found.
               2. Local peer binds to the remote peer successfully.
               2. The message has be sent to remote peer.
               3. The message has been received from remote peer.
*/
void tst_qllcpsocketlocal::testCase1()
{
    // STEP 1:  Touch a NFC device with LLCP connection-less service actived
    QFETCH(quint8, port);
    QFETCH(QString, hint);
    QFETCH(QString, message);
    QNearFieldManager nfcManager;
    QSignalSpy targetDetectedSpy(&nfcManager, SIGNAL(targetDetected(QNearFieldTarget*)));
    nfcManager.startTargetDetection(QNearFieldTarget::AnyTarget);

    QNfcTestUtil::ShowMessage(hint);
    QTRY_VERIFY(!targetDetectedSpy.isEmpty());

    QNearFieldTarget *target = targetDetectedSpy.first().at(0).value<QNearFieldTarget *>();
    QVERIFY(target!=NULL);
    QVERIFY(target->accessMethods() & QNearFieldTarget::LlcpAccess);

    // STEP 2:  bind the local port for current socket
    QLlcpSocket socket(this);
    bool ret = socket.bind(port);
    QVERIFY(ret);

    // STEP 3: Local peer sends the  message to the remote peer
    QSignalSpy errorSpy(&socket, SIGNAL(error(QLlcpSocket::Error)));
    QSignalSpy bytesWrittenSpy(&socket, SIGNAL(bytesWritten(qint64)));

    const char* data = (const char *) message.data();
    qint64 strSize = message.size();
    socket.writeDatagram(data,strSize,target, port);

    const int Timeout = 10 * 1000;
    ret = socket.waitForBytesWritten(Timeout);
    QVERIFY(ret);

    QTRY_VERIFY(bytesWrittenSpy.count() == 1);
    QList<QVariant> arguments = bytesWrittenSpy.takeFirst(); // take the first signal
    qint64 writtenSize  = arguments.at(0).value<qint64>();
    QCOMPARE(writtenSize, strSize);

    // STEP 4: Receive data from remote peer
    QSignalSpy readyReadSpy(&socket, SIGNAL(readyRead()));
    QTRY_VERIFY(readyReadSpy.count() == 1);
    QByteArray datagram;
    while (socket.hasPendingDatagrams())
    {
       datagram.resize(socket.pendingDatagramSize());
       qint64 readSize = socket.readDatagram(datagram.data(), datagram.size());
       QVERIFY(readSize != -1);
    }

    // verify the echoed string is same as the original string
    QString receivedMessage = datagram.constData();
    QVERIFY(message == receivedMessage);

    // make sure the no error signal emitted
    QCOMPARE(errorSpy.count(), 0);
}

void tst_qllcpsocketlocal::testCase1_data()
{
    quint8 port = 35;
    QString data("connect-less unit test string");

    QTest::addColumn<quint8>("port");
    QTest::addColumn<QString>("hint");
    QTest::addColumn<QString>("message");
    QTest::newRow("row1") << port
            << "Please touch a NFC device with llcp connection-less mode client enabled (local peer): port = 35"
            << data;

}

QTEST_MAIN(tst_qllcpsocketlocal);

#include "tst_qllcpsocketlocal.moc"
