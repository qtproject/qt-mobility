/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

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

    void testRawAccessAndNdefAccess(const QList<QNdefMessage> &messages);

private Q_SLOTS:
    void initTestCase();

    void testSmoke_data();
    void testSmoke();

    void testNdefAccess();

    void testRawAndNdefAccess();

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

    void testCancelNdefOperation();
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

void tst_qnearfieldtagtype2::testCancelNdefOperation()
{
    tester.testCancelNdefOperation();
}

void tst_qnearfieldtagtype2::testRawAccessAndNdefAccess(const QList<QNdefMessage> &messages)
{
    QSignalSpy okSpy(tester.target, SIGNAL(requestCompleted(const QNearFieldTarget::RequestId&)));
    QSignalSpy errSpy(tester.target, SIGNAL(error(QNearFieldTarget::Error, const QNearFieldTarget::RequestId&)));
    QSignalSpy ndefMessageReadSpy(tester.target, SIGNAL(ndefMessageRead(QNdefMessage)));
    QSignalSpy ndefMessageWriteSpy(tester.target, SIGNAL(ndefMessagesWritten()));

    int okCount = 0;
    int errCount = 0;
    int ndefReadCount = 0;
    int ndefWriteCount = 0;

    // write ndef first
    tester.target->writeNdefMessages(messages);
    ++ndefWriteCount;
    QTRY_COMPARE(ndefMessageWriteSpy.count(), ndefWriteCount);

    QNearFieldTarget::RequestId id = tester.target->readBlock(3);
    QVERIFY(tester.target->waitForRequestCompleted(id, 50000));
    ++okCount;
    QCOMPARE(okSpy.count(), okCount);
    // check if NDEF existed
    QByteArray cc = tester.target->requestResponse(id).toByteArray();
    QCOMPARE((char)(cc.at(0)), (char)0xE1);

    // try to find NDEF tlv
    QByteArray blocks;
    int NdefLen = -1;
    for(int i = 4; i < 15; i+=4)
    {
        QNearFieldTarget::RequestId id1 = tester.target->readBlock(i);
        QVERIFY(tester.target->waitForRequestCompleted(id1, 50000));
        QByteArray tlv = tester.target->requestResponse(id1).toByteArray();
        blocks.append(tlv);
        ++okCount;
        QCOMPARE(okSpy.count(), okCount);
    }

    QByteArray ndefContent;
    for (int i = 0; i < blocks.count(); ++i)
    {
        if ((blocks.at(i) == 0x03) && (i < blocks.count() - 1))
        {
            // find ndef tlv
            NdefLen = blocks.at(i+1);
            ndefContent = blocks.mid(i+2, NdefLen);
            break;
        }
    }

    QCOMPARE(ndefContent, messages.at(0).toByteArray());
    // update the ndef meesage with raw command
    QNdefMessage message;
    QNdefNfcTextRecord textRecord;
    textRecord.setText("nfc");
    message.append(textRecord);

    QByteArray newNdefMessage = message.toByteArray();
    NdefLen = newNdefMessage.count();
    qDebug()<<"new Ndef len is "<<NdefLen;
    newNdefMessage.push_front((char)NdefLen);
    newNdefMessage.push_front((char)0x03);

    for(int i = 0; i < 16 - NdefLen; ++i)
    {
        // append padding
        newNdefMessage.append((char)0);
    }

    for(int i = 4; i < 8; ++i)
    {
        QNearFieldTarget::RequestId id2 = tester.target->writeBlock(i, newNdefMessage.left(4));
        QVERIFY(tester.target->waitForRequestCompleted(id2));
        ++okCount;
        QCOMPARE(okSpy.count(), okCount);
        newNdefMessage.remove(0,4);
    }

    // read ndef with ndef access
    tester.target->readNdefMessages();
    ++ndefReadCount;
    QTRY_COMPARE(ndefMessageReadSpy.count(), ndefReadCount);

    const QNdefMessage& ndefMessage_new(ndefMessageReadSpy.first().at(0).value<QNdefMessage>());

    QCOMPARE(message.toByteArray(), ndefMessage_new.toByteArray());
    QCOMPARE(errSpy.count(), errCount);
}

void tst_qnearfieldtagtype2::testRawAndNdefAccess()
{
    tester.touchTarget();
    QNdefMessage message;
    QNdefNfcUriRecord uriRecord;
    uriRecord.setUri(QUrl("http://qt.nokia.com"));
    message.append(uriRecord);

    QList<QNdefMessage> messages;
    messages.append(message);

    testRawAccessAndNdefAccess(messages);
    tester.removeTarget();
}


QTEST_MAIN(tst_qnearfieldtagtype2);

#include "tst_qnearfieldtagtype2.moc"
