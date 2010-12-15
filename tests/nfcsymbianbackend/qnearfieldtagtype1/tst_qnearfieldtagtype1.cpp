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
    tester.touchTarget(QNearFieldTarget::NfcTagType1);
}

void tst_qnearfieldtagtype1::cleanup()
{
    tester.removeTarget();
}

void tst_qnearfieldtagtype1::testNdefAccess()
{
    tagType1 = qobject_cast<QNearFieldTagType1 *>(tester.getTarget());
    QVERIFY(tagType1);
    QCOMPARE(tagType1->type(), QNearFieldTarget::NfcTagType1);

    tester.NdefCheck();
}

void tst_qnearfieldtagtype1::testMixRawCommandAndNdefAccess()
{
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
}

QTEST_MAIN(tst_qnearfieldtagtype1);

#include "tst_qnearfieldtagtype1.moc"
