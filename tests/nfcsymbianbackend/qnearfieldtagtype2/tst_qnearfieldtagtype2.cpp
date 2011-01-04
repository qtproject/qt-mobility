#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QVariant>
#include <QVariantList>
#include "qnfctagtestcommon.h"
#include <qnearfieldtagtype2.h>
#include "qnfctestcommon.h"

QTM_USE_NAMESPACE
class tst_qnearfieldtagtype2: public QObject
{
    Q_OBJECT

public:
    tst_qnearfieldtagtype2();
    void testCommandSet();

private Q_SLOTS:
    void initTestCase();
    
    void testSmoke_data();
    void testSmoke();

    void testNdefAccess();

    void testRawCommand_data();
    void testRawCommand();
    
    void testWaitRawCommand_data();
    void testWaitRawCommand();
    
    void testMixRawCommandAndNdefAccess_data();
    void testMixRawCommandAndNdefAccess();
    
    void testWaitMixRawCommandAndNdefAccess_data();
    void testWaitMixRawCommandAndNdefAccess();
    
    void testWaitInSlot_data();
    void testWaitInSlot();

    void testDeleteTargetBeforeAsyncRequestComplete_data();
    void testDeleteTargetBeforeAsyncRequestComplete();

    void testRemoveTargetBeforeAsyncRequestComplete_data();
    void testRemoveTargetBeforeAsyncRequestComplete();

    void cleanupTestCase(){}
private:
    QNfcTagTestCommon<QtMobility::QNearFieldTagType2> tester;
    QMap<QString, QPair<QVariant, QVariant> > dataPool;
};


tst_qnearfieldtagtype2::tst_qnearfieldtagtype2()
{
}

void tst_qnearfieldtagtype2::initTestCase()
{
    tester.touchTarget();
    QByteArray uid = tester.target->uid();
    QVERIFY(!uid.isEmpty());

    tester.removeTarget();

    // prepare data
    {
        QString name = "RID";
        QByteArray command;
        command.append(char(0x78));     // RID
        command.append(char(0x00));     // Address (unused)
        command.append(char(0x00));     // Data (unused)
        command.append(uid.left(4));  // 4 bytes of UID
        // Hardware will append CRC bytes. The CRC value appended 
        // to the command will be ignored.
        command.append(char(0x00)); // CRC1
        command.append(char(0x00)); // CRC2
        
        dataPool.insert(name, qMakePair(QVariant(command), QVariant()));
    }

    {
        QString name = "READ ALL";
        QByteArray command;
        command.append(char(0x00)); // RALL
        command.append(char(0x00));
        command.append(char(0x00));
        command.append(uid.left(4)); // UID
        // Hardware will append CRC bytes. The CRC value appended 
        // to the command will be ignored.
        command.append(char(0x00)); // CRC1
        command.append(char(0x00)); // CRC2

        dataPool.insert(name, qMakePair(QVariant(command), QVariant()));
    }

    {
        QString name = "INVALID";
        QByteArray command;
        command.append(1);
        command.append(1);
        command.append(1);
        command.append(1);
        command.append(1);
        command.append(1);
        command.append(1);
        command.append(1);
        command.append(1);

        QVariantList cmdList;
        cmdList.append(command);
        cmdList.append(command);
        QVariantList rspList;
        rspList.append(QVariant());
        rspList.append(QVariant());

        dataPool.insert(name, qMakePair(QVariant(cmdList), QVariant(rspList)));
    }
}

void tst_qnearfieldtagtype2::testSmoke_data()
{
    QTest::addColumn<QStringList>("dsp");
    QTest::addColumn<QVariantList>("cmd");
    QTest::addColumn<QVariantList>("rsp");
    
    QStringList dsp;
    QVariantList cmd;
    QVariantList rsp;
    
    dsp<<"RID"<<"READ ALL"<<"INVALID";
    cmd.append(dataPool["RID"].first);
    cmd.append(dataPool["READ ALL"].first);
    cmd.append(dataPool["INVALID"].first);
    rsp.append(dataPool["RID"].second);
    rsp.append(dataPool["READ ALL"].second);
    rsp.append(dataPool["INVALID"].second);
    QTest::newRow("data 1")<<dsp<<cmd<<rsp;
}

void tst_qnearfieldtagtype2::testSmoke()
{
    tester.touchTarget();
    
    testCommandSet(); 

    QFETCH(QStringList, dsp);
    QFETCH(QVariantList, cmd);
    QFETCH(QVariantList, rsp);
    tester.testSmoke(dsp, cmd, rsp);
    tester.removeTarget();
}

void tst_qnearfieldtagtype2::testCommandSet()
{
    QSignalSpy okSpy(tester.target, SIGNAL(requestCompleted(const QNearFieldTarget::RequestId&)));
    QSignalSpy errSpy(tester.target, SIGNAL(error(QNearFieldTarget::Error, const QNearFieldTarget::RequestId&)));
    
    int okCount = 0;
    int errCount = 0;

    QNearFieldTarget::RequestId id1 = tester.target->readBlock(0);
    QVERIFY(id1.isValid());
    QVERIFY(!tester.target->waitForRequestCompleted(id1));
    ++errCount;

    QByteArray data;
    for(int i = 0; i < 3; ++i)
    {
        data.append((char)i);
    }
    QNearFieldTarget::RequestId id2 = tester.target->writeBlock(0x13, data);
    QVERIFY(!id2.isValid());
    data.append((char)3);
    QNearFieldTarget::RequestId id3 = tester.target->writeBlock(0x13, data);
    QVERIFY(!tester.target->waitForRequestCompleted(id3));
    ++errCount;

    QNearFieldTarget::RequestId id4 = tester.target->selectSector(2); 
    QVERIFY(!id4.isValid());

    QTRY_COMPARE(okSpy.count(), okCount);
    QTRY_COMPARE(errSpy.count(), errCount);
}

void tst_qnearfieldtagtype2::testNdefAccess()
{
    tester.testNdefAccess();
}

void tst_qnearfieldtagtype2::testRawCommand_data()
{
    QTest::addColumn<QStringList>("dsp");
    QTest::addColumn<QVariantList>("cmd");
    QTest::addColumn<QVariantList>("rsp");
    
    QStringList dsp;
    QVariantList cmd;
    QVariantList rsp;
    
    dsp<<"RID"<<"READ ALL"<<"INVALID";
    cmd.append(dataPool["RID"].first);
    cmd.append(dataPool["READ ALL"].first);
    cmd.append(dataPool["INVALID"].first);
    rsp.append(dataPool["RID"].second);
    rsp.append(dataPool["READ ALL"].second);
    rsp.append(dataPool["INVALID"].second);
    QTest::newRow("data 1")<<dsp<<cmd<<rsp;
}
void tst_qnearfieldtagtype2::testRawCommand()
{
    QFETCH(QStringList, dsp);
    QFETCH(QVariantList, cmd);
    QFETCH(QVariantList, rsp);
    tester.testRawCommand(dsp, cmd, rsp); 
}

void tst_qnearfieldtagtype2::testWaitRawCommand_data()
{
    QTest::addColumn<QStringList>("dsp");
    QTest::addColumn<QVariantList>("cmd");
    QTest::addColumn<QVariantList>("rsp");
    
    QStringList dsp;
    QVariantList cmd;
    QVariantList rsp;
    
    dsp<<"RID"<<"READ ALL"<<"INVALID";
    cmd.append(dataPool["RID"].first);
    cmd.append(dataPool["READ ALL"].first);
    cmd.append(dataPool["INVALID"].first);
    rsp.append(dataPool["RID"].second);
    rsp.append(dataPool["READ ALL"].second);
    rsp.append(dataPool["INVALID"].second);
    QTest::newRow("data 1")<<dsp<<cmd<<rsp;
}

void tst_qnearfieldtagtype2::testWaitRawCommand()
{
    QFETCH(QStringList, dsp);
    QFETCH(QVariantList, cmd);
    QFETCH(QVariantList, rsp);
    tester.testWaitRawCommand(dsp, cmd, rsp);
}

void tst_qnearfieldtagtype2::testMixRawCommandAndNdefAccess_data()
{
    QTest::addColumn<QStringList>("dsp");
    QTest::addColumn<QVariantList>("cmd");
    QTest::addColumn<QVariantList>("rsp");
    
    QStringList dsp;
    QVariantList cmd;
    QVariantList rsp;
    
    dsp<<"RID"<<"READ ALL"<<"INVALID";
    cmd.append(dataPool["RID"].first);
    cmd.append(dataPool["READ ALL"].first);
    cmd.append(dataPool["INVALID"].first);
    rsp.append(dataPool["RID"].second);
    rsp.append(dataPool["READ ALL"].second);
    rsp.append(dataPool["INVALID"].second);
    QTest::newRow("data 1")<<dsp<<cmd<<rsp;
}
void tst_qnearfieldtagtype2::testMixRawCommandAndNdefAccess()
{
    QFETCH(QStringList, dsp);
    QFETCH(QVariantList, cmd);
    QFETCH(QVariantList, rsp);
    tester.testMixRawCommandAndNdefAccess(dsp, cmd, rsp);
}

void tst_qnearfieldtagtype2::testWaitMixRawCommandAndNdefAccess_data()
{
    QTest::addColumn<QStringList>("dsp");
    QTest::addColumn<QVariantList>("cmd");
    QTest::addColumn<QVariantList>("rsp");
    
    QStringList dsp;
    QVariantList cmd;
    QVariantList rsp;
    
    dsp<<"RID"<<"READ ALL"<<"INVALID";
    cmd.append(dataPool["RID"].first);
    cmd.append(dataPool["READ ALL"].first);
    cmd.append(dataPool["INVALID"].first);
    rsp.append(dataPool["RID"].second);
    rsp.append(dataPool["READ ALL"].second);
    rsp.append(dataPool["INVALID"].second);
    QTest::newRow("data 1")<<dsp<<cmd<<rsp;
}

void tst_qnearfieldtagtype2::testWaitMixRawCommandAndNdefAccess()
{
    QFETCH(QStringList, dsp);
    QFETCH(QVariantList, cmd);
    QFETCH(QVariantList, rsp);
    tester.testWaitMixRawCommandAndNdefAccess(dsp, cmd, rsp);
}

void tst_qnearfieldtagtype2::testWaitInSlot_data()
{
    QTest::addColumn<QStringList>("dsp");
    QTest::addColumn<QVariantList>("cmd");
    QTest::addColumn<QVariantList>("rsp");
    
    QStringList dsp;
    QVariantList cmd;
    QVariantList rsp;
    
    dsp<<"RID"<<"READ ALL"<<"INVALID";
    cmd.append(dataPool["RID"].first);
    cmd.append(dataPool["READ ALL"].first);
    cmd.append(dataPool["INVALID"].first);
    rsp.append(dataPool["RID"].second);
    rsp.append(dataPool["READ ALL"].second);
    rsp.append(dataPool["INVALID"].second);
    QTest::newRow("data 1")<<dsp<<cmd<<rsp;
}
void tst_qnearfieldtagtype2::testWaitInSlot()
{
    QFETCH(QStringList, dsp);
    QFETCH(QVariantList, cmd);
    QFETCH(QVariantList, rsp);
    tester.testWaitInSlot(dsp, cmd, rsp);
}

void tst_qnearfieldtagtype2::testDeleteTargetBeforeAsyncRequestComplete_data()
{
    QTest::addColumn<QStringList>("dsp");
    QTest::addColumn<QVariantList>("cmd");
    QTest::addColumn<QVariantList>("rsp");
    
    QStringList dsp;
    QVariantList cmd;
    QVariantList rsp;
    
    dsp<<"RID"<<"READ ALL"<<"INVALID";
    cmd.append(dataPool["RID"].first);
    cmd.append(dataPool["READ ALL"].first);
    cmd.append(dataPool["INVALID"].first);
    rsp.append(dataPool["RID"].second);
    rsp.append(dataPool["READ ALL"].second);
    rsp.append(dataPool["INVALID"].second);
    QTest::newRow("data 1")<<dsp<<cmd<<rsp;
}
void tst_qnearfieldtagtype2::testDeleteTargetBeforeAsyncRequestComplete()
{
    QFETCH(QStringList, dsp);
    QFETCH(QVariantList, cmd);
    QFETCH(QVariantList, rsp);
    tester.testDeleteTargetBeforeAsyncRequestComplete(dsp, cmd, rsp);
}

void tst_qnearfieldtagtype2::testRemoveTargetBeforeAsyncRequestComplete_data()
{
    QTest::addColumn<QStringList>("dsp");
    QTest::addColumn<QVariantList>("cmd");
    QTest::addColumn<QVariantList>("rsp");
    
    QStringList dsp;
    QVariantList cmd;
    QVariantList rsp;
    
    dsp<<"RID"<<"READ ALL"<<"INVALID";
    cmd.append(dataPool["RID"].first);
    cmd.append(dataPool["READ ALL"].first);
    cmd.append(dataPool["INVALID"].first);
    rsp.append(dataPool["RID"].second);
    rsp.append(dataPool["READ ALL"].second);
    rsp.append(dataPool["INVALID"].second);
    QTest::newRow("data 1")<<dsp<<cmd<<rsp;
}
void tst_qnearfieldtagtype2::testRemoveTargetBeforeAsyncRequestComplete()
{
    QFETCH(QStringList, dsp);
    QFETCH(QVariantList, cmd);
    QFETCH(QVariantList, rsp);
    tester.testRemoveTargetBeforeAsyncRequestComplete(dsp, cmd, rsp);
}

QTEST_MAIN(tst_qnearfieldtagtype2);

#include "tst_qnearfieldtagtype2.moc"
