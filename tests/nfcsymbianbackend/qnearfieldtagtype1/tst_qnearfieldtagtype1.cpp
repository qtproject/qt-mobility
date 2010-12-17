#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include "qnfctagtestcommon.h"
#include <qnearfieldtagtype1.h>
#include "qnfctestcommon.h"
#include "qdummyslot.h"

class tst_qnearfieldtagtype1 : public QObject
{
    Q_OBJECT

public:
    tst_qnearfieldtagtype1();

private Q_SLOTS:
    void testNdefAccess();
    void testMixRawCommandAndNdefAccess();
    void testRemoveTagBeforeAsyncRequestCompleted();
    void testDeleteTagBeforeAsyncRequestCompleted();
    void testWaitCommandInSlot();
private:
    QNfcTagTestCommon tester;
    // Not Own
    QNearFieldTagType1 * tagType1;
};

tst_qnearfieldtagtype1::tst_qnearfieldtagtype1()
{
    tagType1 = 0;
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

    QSignalSpy commandSpy(tagType1, SIGNAL(error(QNearFieldTarget::Error)));
    QSignalSpy ndefMessageReadSpy(tagType1, SIGNAL(ndefMessageRead(const QNdefMessage&)));

    qDebug()<<"send readAll async request"<<endl;
    QNearFieldTarget::RequestId id1 = tagType1->readAll();

    qDebug()<<"send readIdentification request"<<endl;
    QNearFieldTarget::RequestId id2 = tagType1->readIdentification();

    qDebug()<<"read ndef message async request"<<endl;
    tagType1->readNdefMessages();

    tester.removeTarget();

    qDebug()<<"command signals count is "<<commandSpy.count()<<endl;
    qDebug()<<"ndef message signals count is "<<ndefMessageReadSpy.count()<<endl;
}

void tst_qnearfieldtagtype1::testDeleteTagBeforeAsyncRequestCompleted()
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
    
    qDebug()<<"delete tag, should be no panic"<<endl;
    delete tagType1;
    tester.getTarget() = 0;
    
    QTRY_VERIFY(readAllSpy.isEmpty());
    QTRY_VERIFY(ndefMessageReadSpy.isEmpty());
    
    QNfcTestUtil::ShowMessage("please remove the tag");
}

void tst_qnearfieldtagtype1::testWaitCommandInSlot()
{
    tester.touchTarget(QNearFieldTarget::NfcTagType1);
    tagType1 = qobject_cast<QNearFieldTagType1 *>(tester.getTarget());
    QVERIFY(tagType1);
    
    QDummySlot dummySlot;
    dummySlot.tag = tagType1;
    QObject::connect(tagType1, SIGNAL(error(QNearFieldTarget::Error)), &dummySlot, SLOT(errorHandling(QNearFieldTarget::Error)));
    
    qDebug()<<"send readAll async request"<<endl;
    QNearFieldTarget::RequestId id1 = tagType1->readAll();
    
    qDebug()<<"send readIdentification request"<<endl;
    QNearFieldTarget::RequestId id2 = tagType1->readIdentification();
    
    QSignalSpy ndefMessageReadSpy(tagType1, SIGNAL(ndefMessageRead(const QNdefMessage&)));
    qDebug()<<"read ndef message async request"<<endl;
    tagType1->readNdefMessages();
    
    qDebug()<<"send readAll async request"<<endl;
    QNearFieldTarget::RequestId id3 = tagType1->readAll();
    
    dummySlot.id = id3;
    
    QVERIFY(tagType1->waitForRequestCompleted(id2));
    QTRY_VERIFY(!ndefMessageReadSpy.isEmpty());
    
    tester.removeTarget();
}

QTEST_MAIN(tst_qnearfieldtagtype1);

#include "tst_qnearfieldtagtype1.moc"
