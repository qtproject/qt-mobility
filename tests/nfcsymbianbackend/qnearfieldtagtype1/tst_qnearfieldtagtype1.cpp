#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include "qnfctagtestcommon.h"
#include <qnearfieldtagtype1.h>
#include "qnfctestcommon.h"

class tst_qnearfieldtagtype1 : public QObject
{
    Q_OBJECT

public:
    tst_qnearfieldtagtype1();

private Q_SLOTS:
    void init();
    void cleanup();
    void testNdefAccess();
    void testMixRawCommandAndNdefAccess();
    //void testSendAsyncRequestInSlot();
    //void testWaitAsyncRequestInSlot();
    void testRemoveTagBeforeAsyncRequestCompleted();
private:
    QNfcTagTestCommon tester;
    // Not Own
    QNearFieldTagType1 * tagType1;
};

tst_qnearfieldtagtype1::tst_qnearfieldtagtype1()
{
    tagType1 = 0;
}

void tst_qnearfieldtagtype1::init()
{
}

void tst_qnearfieldtagtype1::cleanup()
{
}

void tst_qnearfieldtagtype1::testNdefAccess()
{
    tester.touchTarget(QNearFieldTarget::NfcTagType1);
    tagType1 = qobject_cast<QNearFieldTagType1 *>(tester.getTarget());
    QVERIFY(tagType1);
    QCOMPARE(tagType1->type(), QNearFieldTarget::NfcTagType1);

    tester.NdefCheck();
    tester.removeTarget();
}

void tst_qnearfieldtagtype1::testMixRawCommandAndNdefAccess()
{
    tester.touchTarget(QNearFieldTarget::NfcTagType1);
    tagType1 = qobject_cast<QNearFieldTagType1 *>(tester.getTarget());
    QVERIFY(tagType1);

    QSignalSpy readAllSpy(tagType1, SIGNAL(error(QNearFieldTarget::Error)));
    QSignalSpy ndefMessageReadSpy(tagType1, SIGNAL(ndefMessageRead(const QNdefMessage&)));

    qDebug()<<"send readAll async request"<<endl;
    QNearFieldTarget::RequestId id1 = tagType1->readAll();
    qDebug()<<"send readIdentification request"<<endl;
    QNearFieldTarget::RequestId id2 = tagType1->readIdentification();
    qDebug()<<"read ndef message async request"<<endl;
    tagType1->readNdefMessages();
    qDebug()<<"wait readIdentification request"<<endl;
    QVERIFY(tagType1->waitForRequestCompleted(id2));

    qDebug()<<"check signals"<<endl;
    QTRY_VERIFY(!readAllSpy.isEmpty());
    QTRY_VERIFY(!ndefMessageReadSpy.isEmpty());

    qDebug()<<"send readAll async request and wait"<<endl;
    QNearFieldTarget::RequestId id3 = tagType1->readAll();
    QVERIFY(tagType1->waitForRequestCompleted(id3));
    tester.removeTarget();
}

void tst_qnearfieldtagtype1::testRemoveTagBeforeAsyncRequestCompleted()
{
    tester.touchTarget(QNearFieldTarget::NfcTagType1);
    tagType1 = qobject_cast<QNearFieldTagType1 *>(tester.getTarget());
    QVERIFY(tagType1);

    QSignalSpy readAllSpy(tagType1, SIGNAL(error(QNearFieldTarget::Error)));
    QSignalSpy ndefMessageReadSpy(tagType1, SIGNAL(ndefMessageRead(const QNdefMessage&)));

    qDebug()<<"send readAll async request"<<endl;
    QNearFieldTarget::RequestId id1 = tagType1->readAll();

    qDebug()<<"send readIdentification request"<<endl;
    QNearFieldTarget::RequestId id2 = tagType1->readIdentification();

    qDebug()<<"read ndef message async request"<<endl;
    tagType1->readNdefMessages();

    tester.removeTarget();

    qDebug()<<"wait readIdentification request"<<endl;
    QVERIFY(!tagType1->waitForRequestCompleted(id2));

    qDebug()<<"check signals"<<endl;
    QTRY_VERIFY(!readAllSpy.isEmpty());
    QTRY_VERIFY(ndefMessageReadSpy.isEmpty());
}

QTEST_MAIN(tst_qnearfieldtagtype1);

#include "tst_qnearfieldtagtype1.moc"
