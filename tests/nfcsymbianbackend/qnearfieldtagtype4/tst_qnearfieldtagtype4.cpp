#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QVariant>
#include <QVariantList>
#include "qnfctagtestcommon.h"
#include <qnearfieldtagtype4.h>
#include "qnfctestcommon.h"
#include <QtEndian>

QTM_USE_NAMESPACE
class tst_qnearfieldtagtype4 : public QObject
{
    Q_OBJECT

public:
    tst_qnearfieldtagtype4();

private Q_SLOTS:
    void initTestCase();
    
    void testSmoke_data();
    void testSmoke();

    void testCommandSet();
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
    QNfcTagTestCommon<QtMobility::QNearFieldTagType4> tester;
    QMap<QString, QPair<QVariant, QVariant> > dataPool;
};


tst_qnearfieldtagtype4::tst_qnearfieldtagtype4()
{
}

void tst_qnearfieldtagtype4::initTestCase()
{
    // prepare data
    {
        QString name = "NDEF Select";
        QByteArray command;
        command.append(char(0x00));     // CLA
        command.append(char(0xA4));     // INS
        command.append(char(0x04));     // P1
        command.append(char(0x00));     // P2
        command.append(char(0x07));     // Lc
        
        command.append(char(0xD2));
        command.append(char(0x76));
        command.append(char(0x00));
        command.append(char(0x00));
        command.append(char(0x85));
        command.append(char(0x01));
        command.append(char(0x00));
        
        QByteArray resp;
        resp.append(char(0x90));
        resp.append(char(0x00));
        
        dataPool.insert(name, qMakePair(QVariant(command), QVariant(resp)));
    }
    
    {
        QString name = "NDEF Select v2";
        QByteArray command;
        command.append(char(0x00));     // CLA
        command.append(char(0xA4));     // INS
        command.append(char(0x04));     // P1
        command.append(char(0x0c));     // P2
        command.append(char(0x07));     // Lc
        
        command.append(char(0xD2));
        command.append(char(0x76));
        command.append(char(0x00));
        command.append(char(0x00));
        command.append(char(0x85));
        command.append(char(0x01));
        command.append(char(0x01));
        
        QByteArray resp;
        resp.append(char(0x6a));
        resp.append(char(0x86));
        
        dataPool.insert(name, qMakePair(QVariant(command), QVariant(resp)));
    }

    {
        QString name = "CC select";
        QByteArray command;
        command.append(char(0x00)); // CLA
        command.append(char(0xA4)); // INS
        command.append(char(0x00)); // P1
        command.append(char(0x00)); // P2
        command.append(char(0x02)); // Lc
        quint16 data = 0xe103;
        quint16 temp = qToBigEndian<quint16>(data);
        command.append(reinterpret_cast<const char*>(&temp), 
                       sizeof(quint16)); // P1/P2 offset
        
        QByteArray resp;
        resp.append(char(0x90));
        resp.append(char(0x00));
        
        dataPool.insert(name, qMakePair(QVariant(command), QVariant(resp)));
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
        
        QByteArray resp;
        resp.append(char(0x67));
        resp.append(char(0x00));

        dataPool.insert(name, qMakePair(QVariant(command), QVariant(resp)));
    }
    
    {
        QString name = "Combine";
        QByteArray command;
        command.append(char(0x00));     // CLA
        command.append(char(0xA4));     // INS
        command.append(char(0x04));     // P1
        command.append(char(0x00));     // P2
        command.append(char(0x07));     // Lc
        
        command.append(char(0xD2));
        command.append(char(0x76));
        command.append(char(0x00));
        command.append(char(0x00));
        command.append(char(0x85));
        command.append(char(0x01));
        command.append(char(0x00));
        
        QByteArray resp;
        resp.append(char(0x90));
        resp.append(char(0x00));
        
        QVariantList cmdList;
        QVariantList respList;
        
        cmdList.append(command);
        respList.append(resp);
        
        command.clear();
        command.append(char(0x00)); // CLA
        command.append(char(0xA4)); // INS
        command.append(char(0x00)); // P1
        command.append(char(0x00)); // P2
        command.append(char(0x02)); // Lc
        quint16 data = 0xe103;
        quint16 temp = qToBigEndian<quint16>(data);
        command.append(reinterpret_cast<const char*>(&temp), 
                       sizeof(quint16)); // P1/P2 offset
        
        cmdList.append(command);
        respList.append(resp);
        
        dataPool.insert(name, qMakePair(QVariant(cmdList), QVariant(respList)));
    }
}


void tst_qnearfieldtagtype4::testSmoke_data()
{
    QTest::addColumn<QStringList>("dsp");
    QTest::addColumn<QVariantList>("cmd");
    QTest::addColumn<QVariantList>("rsp");
    
    QStringList dsp;
    QVariantList cmd;
    QVariantList rsp;
    
    dsp<<"NDEF Select"<<"CC select"<<"Combine";
    cmd.append(dataPool["NDEF Select"].first);
    cmd.append(dataPool["CC select"].first);
    cmd.append(dataPool["Combine"].first);
    rsp.append(dataPool["NDEF Select"].second);
    rsp.append(dataPool["CC select"].second);
    rsp.append(dataPool["Combine"].second);
    QTest::newRow("data 1")<<dsp<<cmd<<rsp;
}

void tst_qnearfieldtagtype4::testSmoke()
{
    tester.touchTarget();
    
    QSignalSpy okSpy(tester.target, SIGNAL(requestCompleted(const QNearFieldTarget::RequestId&)));
    QSignalSpy errSpy(tester.target, SIGNAL(error(QNearFieldTarget::Error, const QNearFieldTarget::RequestId&)));
    
    int okCount = 0;
    int errCount = 0;
    
    QByteArray resp;
    resp.append(char(0x90));
    resp.append(char(0x00));
    
    // select ndef tag
    qDebug()<<"select ndef tag";
    QByteArray command;
    command.append(char(0xD2));
    command.append(char(0x76));
    command.append(char(0x00));
    command.append(char(0x00));
    command.append(char(0x85));
    command.append(char(0x01));
    command.append(char(0x00));
    QNearFieldTarget::RequestId id1 = tester.target->select(command);
    QVERIFY(tester.target->waitForRequestCompleted(id1));
    ++okCount;
    QCOMPARE(tester.target->requestResponse(id1).toByteArray(), resp);
    
    // select cc
    qDebug()<<"select cc";
    QNearFieldTarget::RequestId id2 = tester.target->select(0xe103);
    QVERIFY(tester.target->waitForRequestCompleted(id2));
    ++okCount;
    QCOMPARE(tester.target->requestResponse(id2).toByteArray(), resp);
    
    // read cc
    qDebug()<<"read cc";
    QNearFieldTarget::RequestId id3 = tester.target->read(0x0008,0x0007);
    QVERIFY(tester.target->waitForRequestCompleted(id3));
    QByteArray readResp = tester.target->requestResponse(id3).toByteArray(); // 04 06 E1 04 0E E0 00 00 
    ++okCount;
    QCOMPARE(readResp.at(readResp.count()-1), char(0x00));
    QCOMPARE(readResp.at(readResp.count()-2), char(0x90));
    
    quint8 temp = readResp.at(2);
    quint16 fileId = 0;
    fileId |= temp;
    fileId<<=8;
    
    temp = readResp.at(3);
    fileId |= temp;
    
    // select NDEF
    qDebug()<<"select ndef";
    QNearFieldTarget::RequestId id4 = tester.target->select(fileId);
    QVERIFY(tester.target->waitForRequestCompleted(id4));
    ++okCount;
    QCOMPARE(tester.target->requestResponse(id4).toByteArray(), resp);
    
    // read NDEF
    qDebug()<<"read ndef";
    QNearFieldTarget::RequestId id5 = tester.target->read(0x0000,0x0002);
    QVERIFY(tester.target->waitForRequestCompleted(id5));
    ++okCount;
    QByteArray readNdefResp = tester.target->requestResponse(id5).toByteArray(); 
    ++okCount;
    QCOMPARE(readNdefResp.at(readNdefResp.count()-1), char(0x00));
    QCOMPARE(readNdefResp.at(readNdefResp.count()-2), char(0x90));
    
    QByteArray data;
    data.append(readNdefResp.at(0));
    data.append(readNdefResp.at(1));
    
    // write NDEF
    qDebug()<<"write ndef";
    QNearFieldTarget::RequestId id6 = tester.target->write(data, 0x0000);
    QVERIFY(tester.target->waitForRequestCompleted(id6));
    ++okCount;
    QCOMPARE(tester.target->requestResponse(id6).toByteArray(), resp);
    
    // read again
    qDebug()<<"read ndef";
    QNearFieldTarget::RequestId id7 = tester.target->read(0x0000,0x0002);
    QVERIFY(tester.target->waitForRequestCompleted(id7));
    ++okCount;
    data.append(resp);
    QCOMPARE(tester.target->requestResponse(id7).toByteArray(), data);
    
    // wrong param
    data.clear();
    QNearFieldTarget::RequestId id8 = tester.target->write(data, 0x0000);
    QVERIFY(!id8.isValid());
    for(int i = 0; i < 500; ++i)
    {
        data.append(char(i));
    }
    QNearFieldTarget::RequestId id9 = tester.target->write(data, 0x0000);
    QVERIFY(!id9.isValid());
    
    QTRY_COMPARE(okSpy.count(), okCount);
    QTRY_COMPARE(errSpy.count(), errCount);
    
    QFETCH(QStringList, dsp);
    QFETCH(QVariantList, cmd);
    QFETCH(QVariantList, rsp);
    tester.testWaitRawCommand(dsp, cmd, rsp);
    
    tester.removeTarget();
}

void tst_qnearfieldtagtype4::testCommandSet()
{
    tester.touchTarget();
    
    QSignalSpy okSpy(tester.target, SIGNAL(requestCompleted(const QNearFieldTarget::RequestId&)));
    QSignalSpy errSpy(tester.target, SIGNAL(error(QNearFieldTarget::Error, const QNearFieldTarget::RequestId&)));
    
    int okCount = 0;
    int errCount = 0;

    QByteArray resp;
    resp.append(char(0x90));
    resp.append(char(0x00));
    
    // select ndef tag
    qDebug()<<"select ndef tag";
    QByteArray command;
    command.append(char(0xD2));
    command.append(char(0x76));
    command.append(char(0x00));
    command.append(char(0x00));
    command.append(char(0x85));
    command.append(char(0x01));
    command.append(char(0x00));
    QNearFieldTarget::RequestId id1 = tester.target->select(command);
    QVERIFY(tester.target->waitForRequestCompleted(id1));
    ++okCount;
    QCOMPARE(tester.target->requestResponse(id1).toByteArray(), resp);
    
    // select cc
    qDebug()<<"select cc";
    QNearFieldTarget::RequestId id2 = tester.target->select(0xe103);
    QVERIFY(tester.target->waitForRequestCompleted(id2));
    ++okCount;
    QCOMPARE(tester.target->requestResponse(id2).toByteArray(), resp);
    
    // read cc
    qDebug()<<"read cc";
    QNearFieldTarget::RequestId id3 = tester.target->read(0x0008,0x0007);
    QVERIFY(tester.target->waitForRequestCompleted(id3));
    QByteArray readResp = tester.target->requestResponse(id3).toByteArray(); // 04 06 E1 04 0E E0 00 00 
    ++okCount;
    QCOMPARE(readResp.at(readResp.count()-1), char(0x00));
    QCOMPARE(readResp.at(readResp.count()-2), char(0x90));
    
    quint8 temp = readResp.at(2);
    quint16 fileId = 0;
    fileId |= temp;
    fileId<<=8;
    
    temp = readResp.at(3);
    fileId |= temp;
    
    // select NDEF
    qDebug()<<"select ndef";
    QNearFieldTarget::RequestId id4 = tester.target->select(fileId);
    QVERIFY(tester.target->waitForRequestCompleted(id4));
    ++okCount;
    QCOMPARE(tester.target->requestResponse(id4).toByteArray(), resp);
    
    // read NDEF
    qDebug()<<"read ndef";
    QNearFieldTarget::RequestId id5 = tester.target->read(0x0000,0x0002);
    QVERIFY(tester.target->waitForRequestCompleted(id5));
    ++okCount;
    QByteArray readNdefResp = tester.target->requestResponse(id5).toByteArray(); 
    ++okCount;
    QCOMPARE(readNdefResp.at(readNdefResp.count()-1), char(0x00));
    QCOMPARE(readNdefResp.at(readNdefResp.count()-2), char(0x90));
    
    QByteArray data;
    data.append(readNdefResp.at(0));
    data.append(readNdefResp.at(1));
    
    // write NDEF
    qDebug()<<"write ndef";
    QNearFieldTarget::RequestId id6 = tester.target->write(data, 0x0000);
    QVERIFY(tester.target->waitForRequestCompleted(id6));
    ++okCount;
    QCOMPARE(tester.target->requestResponse(id6).toByteArray(), resp);
    
    // read again
    qDebug()<<"read ndef";
    QNearFieldTarget::RequestId id7 = tester.target->read(0x0000,0x0002);
    QVERIFY(tester.target->waitForRequestCompleted(id7));
    ++okCount;
    data.append(resp);
    QCOMPARE(tester.target->requestResponse(id7).toByteArray(), data);
    
    // wrong param
    data.clear();
    QNearFieldTarget::RequestId id8 = tester.target->write(data, 0x0000);
    QVERIFY(!id8.isValid());
    for(int i = 0; i < 500; ++i)
    {
        data.append(char(i));
    }
    QNearFieldTarget::RequestId id9 = tester.target->write(data, 0x0000);
    QVERIFY(!id9.isValid());
    
    QTRY_COMPARE(okSpy.count(), okCount);
    QTRY_COMPARE(errSpy.count(), errCount);
    tester.removeTarget();
}
    
void tst_qnearfieldtagtype4::testNdefAccess()
{
    tester.testNdefAccess();
}

void tst_qnearfieldtagtype4::testRawCommand_data()
{
    QTest::addColumn<QStringList>("dsp");
    QTest::addColumn<QVariantList>("cmd");
    QTest::addColumn<QVariantList>("rsp");
    
    QStringList dsp;
    QVariantList cmd;
    QVariantList rsp;
    
    dsp<<"NDEF Select"<<"CC select"<<"Combine";
    cmd.append(dataPool["NDEF Select"].first);
    cmd.append(dataPool["CC select"].first);
    cmd.append(dataPool["Combine"].first);
    rsp.append(dataPool["NDEF Select"].second);
    rsp.append(dataPool["CC select"].second);
    rsp.append(dataPool["Combine"].second);
    QTest::newRow("data 1")<<dsp<<cmd<<rsp;
}
void tst_qnearfieldtagtype4::testRawCommand()
{
    QFETCH(QStringList, dsp);
    QFETCH(QVariantList, cmd);
    QFETCH(QVariantList, rsp);
    tester.testRawCommand(dsp, cmd, rsp); 
}

void tst_qnearfieldtagtype4::testWaitRawCommand_data()
{
    QTest::addColumn<QStringList>("dsp");
    QTest::addColumn<QVariantList>("cmd");
    QTest::addColumn<QVariantList>("rsp");
    
    QStringList dsp;
    QVariantList cmd;
    QVariantList rsp;
    
    dsp<<"NDEF Select"<<"CC select"<<"Combine";
    cmd.append(dataPool["NDEF Select"].first);
    cmd.append(dataPool["CC select"].first);
    cmd.append(dataPool["Combine"].first);
    rsp.append(dataPool["NDEF Select"].second);
    rsp.append(dataPool["CC select"].second);
    rsp.append(dataPool["Combine"].second);
    QTest::newRow("data 1")<<dsp<<cmd<<rsp;
}

void tst_qnearfieldtagtype4::testWaitRawCommand()
{
    QFETCH(QStringList, dsp);
    QFETCH(QVariantList, cmd);
    QFETCH(QVariantList, rsp);
    tester.testWaitRawCommand(dsp, cmd, rsp);
}

void tst_qnearfieldtagtype4::testMixRawCommandAndNdefAccess_data()
{
    QTest::addColumn<QStringList>("dsp");
    QTest::addColumn<QVariantList>("cmd");
    QTest::addColumn<QVariantList>("rsp");
    
    QStringList dsp;
    QVariantList cmd;
    QVariantList rsp;
    
    dsp<<"NDEF Select"<<"CC select"<<"INVALID";
    cmd.append(dataPool["NDEF Select"].first);
    cmd.append(dataPool["CC select"].first);
    cmd.append(dataPool["Combine"].first);
    rsp.append(dataPool["NDEF Select"].second);
    rsp.append(dataPool["CC select"].second);
    rsp.append(dataPool["Combine"].second);
    QTest::newRow("data 1")<<dsp<<cmd<<rsp;
}
void tst_qnearfieldtagtype4::testMixRawCommandAndNdefAccess()
{
    QFETCH(QStringList, dsp);
    QFETCH(QVariantList, cmd);
    QFETCH(QVariantList, rsp);
    tester.testMixRawCommandAndNdefAccess(dsp, cmd, rsp);
}

void tst_qnearfieldtagtype4::testWaitMixRawCommandAndNdefAccess_data()
{
    QTest::addColumn<QStringList>("dsp");
    QTest::addColumn<QVariantList>("cmd");
    QTest::addColumn<QVariantList>("rsp");
    
    QStringList dsp;
    QVariantList cmd;
    QVariantList rsp;
    
    dsp<<"NDEF Select"<<"CC select"<<"Combine";
    cmd.append(dataPool["NDEF Select"].first);
    cmd.append(dataPool["CC select"].first);
    cmd.append(dataPool["Combine"].first);
    rsp.append(dataPool["NDEF Select"].second);
    rsp.append(dataPool["CC select"].second);
    rsp.append(dataPool["Combine"].second);
    QTest::newRow("data 1")<<dsp<<cmd<<rsp;
}

void tst_qnearfieldtagtype4::testWaitMixRawCommandAndNdefAccess()
{
    QFETCH(QStringList, dsp);
    QFETCH(QVariantList, cmd);
    QFETCH(QVariantList, rsp);
    tester.testWaitMixRawCommandAndNdefAccess(dsp, cmd, rsp);
}

void tst_qnearfieldtagtype4::testWaitInSlot_data()
{
    QTest::addColumn<QStringList>("dsp");
    QTest::addColumn<QVariantList>("cmd");
    QTest::addColumn<QVariantList>("rsp");
    
    QStringList dsp;
    QVariantList cmd;
    QVariantList rsp;
    
    dsp<<"NDEF Select"<<"CC select"<<"Combine";
    cmd.append(dataPool["NDEF Select"].first);
    cmd.append(dataPool["CC select"].first);
    cmd.append(dataPool["Combine"].first);
    rsp.append(dataPool["NDEF Select"].second);
    rsp.append(dataPool["CC select"].second);
    rsp.append(dataPool["Combine"].second);
    QTest::newRow("data 1")<<dsp<<cmd<<rsp;
}
void tst_qnearfieldtagtype4::testWaitInSlot()
{
    QFETCH(QStringList, dsp);
    QFETCH(QVariantList, cmd);
    QFETCH(QVariantList, rsp);
    tester.testWaitInSlot(dsp, cmd, rsp);
}

void tst_qnearfieldtagtype4::testDeleteTargetBeforeAsyncRequestComplete_data()
{
    QTest::addColumn<QStringList>("dsp");
    QTest::addColumn<QVariantList>("cmd");
    QTest::addColumn<QVariantList>("rsp");
    
    QStringList dsp;
    QVariantList cmd;
    QVariantList rsp;
    
    dsp<<"NDEF Select"<<"CC select"<<"Combine";
    cmd.append(dataPool["NDEF Select"].first);
    cmd.append(dataPool["CC select"].first);
    cmd.append(dataPool["Combine"].first);
    rsp.append(dataPool["NDEF Select"].second);
    rsp.append(dataPool["CC select"].second);
    rsp.append(dataPool["Combine"].second);
    QTest::newRow("data 1")<<dsp<<cmd<<rsp;
}
void tst_qnearfieldtagtype4::testDeleteTargetBeforeAsyncRequestComplete()
{
    QFETCH(QStringList, dsp);
    QFETCH(QVariantList, cmd);
    QFETCH(QVariantList, rsp);
    tester.testDeleteTargetBeforeAsyncRequestComplete(dsp, cmd, rsp);
}

void tst_qnearfieldtagtype4::testRemoveTargetBeforeAsyncRequestComplete_data()
{
    QTest::addColumn<QStringList>("dsp");
    QTest::addColumn<QVariantList>("cmd");
    QTest::addColumn<QVariantList>("rsp");
    
    QStringList dsp;
    QVariantList cmd;
    QVariantList rsp;
    
    dsp<<"NDEF Select"<<"CC select"<<"Combine";
    cmd.append(dataPool["NDEF Select"].first);
    cmd.append(dataPool["CC select"].first);
    cmd.append(dataPool["Combine"].first);
    rsp.append(dataPool["NDEF Select"].second);
    rsp.append(dataPool["CC select"].second);
    rsp.append(dataPool["Combine"].second);
    QTest::newRow("data 1")<<dsp<<cmd<<rsp;
}
void tst_qnearfieldtagtype4::testRemoveTargetBeforeAsyncRequestComplete()
{
    QFETCH(QStringList, dsp);
    QFETCH(QVariantList, cmd);
    QFETCH(QVariantList, rsp);
    tester.testRemoveTargetBeforeAsyncRequestComplete(dsp, cmd, rsp);
}

QTEST_MAIN(tst_qnearfieldtagtype4);

#include "tst_qnearfieldtagtype4.moc"
