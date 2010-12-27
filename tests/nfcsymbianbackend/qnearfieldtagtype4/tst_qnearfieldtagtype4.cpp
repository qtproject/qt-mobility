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
        QString name = "CC select";
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
        
        dataPool.insert(name, qMakePair(QVariant(command), QVariant(QByteArray())));
    }
    
    {
        QString name = "NDEF Select";
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
        
        dataPool.insert(name, qMakePair(QVariant(command), QVariant(QByteArray())));
    }

    {
        QString name = "CC select old";
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
        
        dataPool.insert(name, qMakePair(QVariant(command), QVariant(QByteArray())));
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

        dataPool.insert(name, qMakePair(QVariant(command), QVariant(QByteArray())));
    }
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
    
    dsp<<"NDEF Select"<<"CC select"<<"INVALID";
    cmd.append(dataPool["NDEF Select"].first);
    cmd.append(dataPool["CC select"].first);
    cmd.append(dataPool["INVALID"].first);
    rsp.append(dataPool["NDEF Select"].second);
    rsp.append(dataPool["CC select"].second);
    rsp.append(dataPool["INVALID"].second);
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
    
    dsp<<"NDEF Select"<<"CC select"<<"INVALID";
    cmd.append(dataPool["NDEF Select"].first);
    cmd.append(dataPool["CC select"].first);
    cmd.append(dataPool["INVALID"].first);
    rsp.append(dataPool["NDEF Select"].second);
    rsp.append(dataPool["CC select"].second);
    rsp.append(dataPool["INVALID"].second);
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
    cmd.append(dataPool["INVALID"].first);
    rsp.append(dataPool["NDEF Select"].second);
    rsp.append(dataPool["CC select"].second);
    rsp.append(dataPool["INVALID"].second);
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
    
    dsp<<"NDEF Select"<<"CC select"<<"INVALID";
    cmd.append(dataPool["NDEF Select"].first);
    cmd.append(dataPool["CC select"].first);
    cmd.append(dataPool["INVALID"].first);
    rsp.append(dataPool["NDEF Select"].second);
    rsp.append(dataPool["CC select"].second);
    rsp.append(dataPool["INVALID"].second);
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
    
    dsp<<"NDEF Select"<<"CC select"<<"INVALID";
    cmd.append(dataPool["NDEF Select"].first);
    cmd.append(dataPool["CC select"].first);
    cmd.append(dataPool["INVALID"].first);
    rsp.append(dataPool["NDEF Select"].second);
    rsp.append(dataPool["CC select"].second);
    rsp.append(dataPool["INVALID"].second);
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
    
    dsp<<"NDEF Select"<<"CC select"<<"INVALID";
    cmd.append(dataPool["NDEF Select"].first);
    cmd.append(dataPool["CC select"].first);
    cmd.append(dataPool["INVALID"].first);
    rsp.append(dataPool["NDEF Select"].second);
    rsp.append(dataPool["CC select"].second);
    rsp.append(dataPool["INVALID"].second);
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
    
    dsp<<"NDEF Select"<<"CC select"<<"INVALID";
    cmd.append(dataPool["NDEF Select"].first);
    cmd.append(dataPool["CC select"].first);
    cmd.append(dataPool["INVALID"].first);
    rsp.append(dataPool["NDEF Select"].second);
    rsp.append(dataPool["CC select"].second);
    rsp.append(dataPool["INVALID"].second);
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
