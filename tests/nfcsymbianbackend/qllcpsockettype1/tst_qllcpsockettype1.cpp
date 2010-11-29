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

class tst_qllcpsockettype1 : public QObject
{
    Q_OBJECT

public:
    tst_qllcpsockettype1();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCase1();
    void testCase1_data();
};

tst_qllcpsockettype1::tst_qllcpsockettype1()
{
}

void tst_qllcpsockettype1::initTestCase()
{
}

void tst_qllcpsockettype1::cleanupTestCase()
{
}

/*!
 Description: Unit test for NFC LLCP connection-less mode socket

 TestScenario: 1. Touch a NFC device with LLCP connection-less service actived
               2. Local peer binds to the remote peer
               3. Local peer sends the "connect-less unit test string" message to the remote peer
               4. Local peer receives the message sending from the remote peer

 TestExpectedResults:
               1. Signal of target detected has been found.
               2. Local peer binds to the remote peer successfully.
               2. The message has be sent to remote peer.
               3. The message has been received from remote peer.
*/
void tst_qllcpsockettype1::testCase1()
{
    // Step 1:  Touch a NFC device with LLCP connection-less service actived
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

    // Step 2:  Local peer binds to the remote peer
    QLlcpSocket socket(this);

    //const quint8 KInterestingSsap = 35;
    //quint8 port = KInterestingSsap;
    bool ret = socket.bind(port);

    QVERIFY(ret);

    // Step 3: Local peer sends the  message to the remote peer
    QSignalSpy errorSpy(&socket, SIGNAL(error(QLlcpSocket::Error)));

    //Send data to server
    QSignalSpy bytesWrittenSpy(&socket, SIGNAL(bytesWritten(qint64)));


    const char* data = "connect-less unit test string";
    qint64 strSize = QString(data).size();
    socket.writeDatagram(data,strSize);


    //socket.write(block);

    const int Timeout = 10 * 1000;
    ret = socket.waitForBytesWritten(Timeout);
    QVERIFY(ret);

}

void tst_qllcpsockettype1::testCase1_data()
{
    QTest::addColumn<QString>("data");
    QTest::newRow("0") << QString();

    QTest::addColumn<QString>("");
    QTest::addColumn<QString>("hint");
    QTest::addColumn<QString>("message");
    QTest::newRow("0") << "urn:nfc:sn:test"
            << "Please touch a NFC device with llcp client enabled: uri = urn:nfc:sn:test"
            << "echo";

}

QTEST_MAIN(tst_qllcpsockettype1);

#include "tst_qllcpsockettype1.moc"
