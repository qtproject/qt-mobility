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
#include <qnearfieldtagtype1.h>
#include "qnfctestcommon.h"

QTM_USE_NAMESPACE
class tst_qnearfieldtagtype1 : public QObject
{
    Q_OBJECT

public:
    tst_qnearfieldtagtype1();
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
    QNfcTagTestCommon<QtMobility::QNearFieldTagType1> tester;
    QMap<QString, QPair<QVariant, QVariant> > dataPool;
};


tst_qnearfieldtagtype1::tst_qnearfieldtagtype1()
{
}


void tst_qnearfieldtagtype1::initTestCase()
{
    tester.touchTarget();
    QByteArray uid = tester.target->uid();
    QVERIFY(!uid.isEmpty());

    tester.removeTarget();

    // prepare data
    {
        QString name = "READ";
        QByteArray command;

        command.append(char(0x01));     // READ
        command.append(char(0x67));  // Address
        command.append(char(0x00));     // Data (unused)
        command.append(uid.left(4));  // 4 bytes of UID

        dataPool.insert(name, qMakePair(QVariant(command), QVariant((quint8)0x33)));
    }

    {
        QString name = "WRITE_E";
        QByteArray command;
        command.append(0x53);
        command.append(char(0x67)); // block c, byte 7
        command.append(char(0x33));
        command.append(uid.left(4)); // UID
        dataPool.insert(name, qMakePair(QVariant(command), QVariant(true)));
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

void tst_qnearfieldtagtype1::testSmoke_data()
{
    QTest::addColumn<QStringList>("dsp");
    QTest::addColumn<QVariantList>("cmd");
    QTest::addColumn<QVariantList>("rsp");

    QStringList dsp;
    QVariantList cmd;
    QVariantList rsp;

    dsp<<"RID"<<"READ ALL"<<"INVALID";
    cmd.append(dataPool["WRITE_E"].first);
    cmd.append(dataPool["READ"].first);
    cmd.append(dataPool["INVALID"].first);
    rsp.append(dataPool["WRITE_E"].second);
    rsp.append(dataPool["READ"].second);
    rsp.append(dataPool["INVALID"].second);
    QTest::newRow("data 1")<<dsp<<cmd<<rsp;
}

void tst_qnearfieldtagtype1::testSmoke()
{
    tester.touchTarget();
    testCommandSet();

    QFETCH(QStringList, dsp);
    QFETCH(QVariantList, cmd);
    QFETCH(QVariantList, rsp);
    tester.testSmoke(dsp, cmd, rsp);
    tester.removeTarget();
}

void tst_qnearfieldtagtype1::testCommandSet()
{
    QSignalSpy okSpy(tester.target, SIGNAL(requestCompleted(const QNearFieldTarget::RequestId&)));
    QSignalSpy errSpy(tester.target, SIGNAL(error(QNearFieldTarget::Error, const QNearFieldTarget::RequestId&)));

    int okCount = 0;
    int errCount = 0;

    QNearFieldTarget::RequestId id1 = tester.target->readIdentification();
    QVERIFY(id1.isValid());
    QVERIFY(!tester.target->waitForRequestCompleted(id1));
    ++errCount;

    QNearFieldTarget::RequestId id2 = tester.target->readAll();
    QVERIFY(id2.isValid());
    QVERIFY(!tester.target->waitForRequestCompleted(id2));
    ++errCount;

    QNearFieldTarget::RequestId id3 = tester.target->readByte(0x81);
    QVERIFY(!id3.isValid());
    QNearFieldTarget::RequestId id4 = tester.target->readByte(0x02);
    QVERIFY(!tester.target->waitForRequestCompleted(id4));
    ++errCount;

    QNearFieldTarget::RequestId id5 = tester.target->writeByte(0x81, 0, QtMobility::QNearFieldTagType1::EraseAndWrite);
    QVERIFY(!id5.isValid());
    QNearFieldTarget::RequestId id6 = tester.target->writeByte(0x02, 0, QtMobility::QNearFieldTagType1::EraseAndWrite);
    QVERIFY(!tester.target->waitForRequestCompleted(id6));
    ++errCount;
    QNearFieldTarget::RequestId id7 = tester.target->writeByte(0x02, 0, QtMobility::QNearFieldTagType1::WriteOnly);
    QVERIFY(!tester.target->waitForRequestCompleted(id7));
    ++errCount;

    QNearFieldTarget::RequestId id9 = tester.target->readSegment(0xf1);
    QVERIFY(!id9.isValid());
    QNearFieldTarget::RequestId id10 = tester.target->readSegment(0x01);
    QVERIFY(!tester.target->waitForRequestCompleted(id4));
    ++errCount;

    QNearFieldTarget::RequestId id11 = tester.target->readBlock(0x13);
    QVERIFY(!tester.target->waitForRequestCompleted(id11));
    ++errCount;

    QByteArray data;
    for(int i = 0; i < 7; ++i)
    {
        data.append((char)i);
    }
    QNearFieldTarget::RequestId id12 = tester.target->writeBlock(0x13, data, QtMobility::QNearFieldTagType1::EraseAndWrite);
    QVERIFY(!id12.isValid());
    data.append((char)8);
    QNearFieldTarget::RequestId id13 = tester.target->writeBlock(0x13, data, QtMobility::QNearFieldTagType1::EraseAndWrite);
    QVERIFY(!tester.target->waitForRequestCompleted(id13));
    ++errCount;
    QNearFieldTarget::RequestId id14 = tester.target->writeBlock(0x13, data, QtMobility::QNearFieldTagType1::WriteOnly);
    QVERIFY(!tester.target->waitForRequestCompleted(id14));
    ++errCount;

    QTRY_COMPARE(okSpy.count(), okCount);
    QTRY_COMPARE(errSpy.count(), errCount);
}

void tst_qnearfieldtagtype1::testNdefAccess()
{
    tester.testNdefAccess();
}

void tst_qnearfieldtagtype1::testRawCommand_data()
{
    QTest::addColumn<QStringList>("dsp");
    QTest::addColumn<QVariantList>("cmd");
    QTest::addColumn<QVariantList>("rsp");

    QStringList dsp;
    QVariantList cmd;
    QVariantList rsp;

    dsp<<"RID"<<"READ ALL"<<"INVALID";
    cmd.append(dataPool["WRITE_E"].first);
    cmd.append(dataPool["READ"].first);
    cmd.append(dataPool["INVALID"].first);
    rsp.append(dataPool["WRITE_E"].second);
    rsp.append(dataPool["READ"].second);
    rsp.append(dataPool["INVALID"].second);
    QTest::newRow("data 1")<<dsp<<cmd<<rsp;
}
void tst_qnearfieldtagtype1::testRawCommand()
{
    QFETCH(QStringList, dsp);
    QFETCH(QVariantList, cmd);
    QFETCH(QVariantList, rsp);
    tester.testRawCommand(dsp, cmd, rsp);
}

void tst_qnearfieldtagtype1::testWaitRawCommand_data()
{
    QTest::addColumn<QStringList>("dsp");
    QTest::addColumn<QVariantList>("cmd");
    QTest::addColumn<QVariantList>("rsp");

    QStringList dsp;
    QVariantList cmd;
    QVariantList rsp;

    dsp<<"RID"<<"READ ALL"<<"INVALID";
    cmd.append(dataPool["WRITE_E"].first);
    cmd.append(dataPool["READ"].first);
    cmd.append(dataPool["INVALID"].first);
    rsp.append(dataPool["WRITE_E"].second);
    rsp.append(dataPool["READ"].second);
    rsp.append(dataPool["INVALID"].second);
    QTest::newRow("data 1")<<dsp<<cmd<<rsp;
}

void tst_qnearfieldtagtype1::testWaitRawCommand()
{
    QFETCH(QStringList, dsp);
    QFETCH(QVariantList, cmd);
    QFETCH(QVariantList, rsp);
    tester.testWaitRawCommand(dsp, cmd, rsp);
}

void tst_qnearfieldtagtype1::testMixRawCommandAndNdefAccess_data()
{
    QTest::addColumn<QStringList>("dsp");
    QTest::addColumn<QVariantList>("cmd");
    QTest::addColumn<QVariantList>("rsp");

    QStringList dsp;
    QVariantList cmd;
    QVariantList rsp;

    dsp<<"RID"<<"READ ALL"<<"INVALID";
    cmd.append(dataPool["WRITE_E"].first);
    cmd.append(dataPool["READ"].first);
    cmd.append(dataPool["INVALID"].first);
    rsp.append(dataPool["WRITE_E"].second);
    rsp.append(dataPool["READ"].second);
    rsp.append(dataPool["INVALID"].second);
    QTest::newRow("data 1")<<dsp<<cmd<<rsp;
}
void tst_qnearfieldtagtype1::testMixRawCommandAndNdefAccess()
{
    QFETCH(QStringList, dsp);
    QFETCH(QVariantList, cmd);
    QFETCH(QVariantList, rsp);
    tester.testMixRawCommandAndNdefAccess(dsp, cmd, rsp);
}

void tst_qnearfieldtagtype1::testWaitMixRawCommandAndNdefAccess_data()
{
    QTest::addColumn<QStringList>("dsp");
    QTest::addColumn<QVariantList>("cmd");
    QTest::addColumn<QVariantList>("rsp");

    QStringList dsp;
    QVariantList cmd;
    QVariantList rsp;

    dsp<<"RID"<<"READ ALL"<<"INVALID";
    cmd.append(dataPool["WRITE_E"].first);
    cmd.append(dataPool["READ"].first);
    cmd.append(dataPool["INVALID"].first);
    rsp.append(dataPool["WRITE_E"].second);
    rsp.append(dataPool["READ"].second);
    rsp.append(dataPool["INVALID"].second);
    QTest::newRow("data 1")<<dsp<<cmd<<rsp;
}

void tst_qnearfieldtagtype1::testWaitMixRawCommandAndNdefAccess()
{
    QFETCH(QStringList, dsp);
    QFETCH(QVariantList, cmd);
    QFETCH(QVariantList, rsp);
    tester.testWaitMixRawCommandAndNdefAccess(dsp, cmd, rsp);
}

void tst_qnearfieldtagtype1::testWaitInSlot_data()
{
    QTest::addColumn<QStringList>("dsp");
    QTest::addColumn<QVariantList>("cmd");
    QTest::addColumn<QVariantList>("rsp");

    QStringList dsp;
    QVariantList cmd;
    QVariantList rsp;

    dsp<<"RID"<<"READ ALL"<<"INVALID";
    cmd.append(dataPool["WRITE_E"].first);
    cmd.append(dataPool["READ"].first);
    cmd.append(dataPool["INVALID"].first);
    rsp.append(dataPool["WRITE_E"].second);
    rsp.append(dataPool["READ"].second);
    rsp.append(dataPool["INVALID"].second);
    QTest::newRow("data 1")<<dsp<<cmd<<rsp;
}
void tst_qnearfieldtagtype1::testWaitInSlot()
{
    QFETCH(QStringList, dsp);
    QFETCH(QVariantList, cmd);
    QFETCH(QVariantList, rsp);
    tester.testWaitInSlot(dsp, cmd, rsp);
}

void tst_qnearfieldtagtype1::testDeleteTargetBeforeAsyncRequestComplete_data()
{
    QTest::addColumn<QStringList>("dsp");
    QTest::addColumn<QVariantList>("cmd");
    QTest::addColumn<QVariantList>("rsp");

    QStringList dsp;
    QVariantList cmd;
    QVariantList rsp;

    dsp<<"RID"<<"READ ALL"<<"INVALID";
    cmd.append(dataPool["WRITE_E"].first);
    cmd.append(dataPool["READ"].first);
    cmd.append(dataPool["INVALID"].first);
    rsp.append(dataPool["WRITE_E"].second);
    rsp.append(dataPool["READ"].second);
    rsp.append(dataPool["INVALID"].second);
    QTest::newRow("data 1")<<dsp<<cmd<<rsp;
}
void tst_qnearfieldtagtype1::testDeleteTargetBeforeAsyncRequestComplete()
{
    QFETCH(QStringList, dsp);
    QFETCH(QVariantList, cmd);
    QFETCH(QVariantList, rsp);
    tester.testDeleteTargetBeforeAsyncRequestComplete(dsp, cmd, rsp);
}

void tst_qnearfieldtagtype1::testRemoveTargetBeforeAsyncRequestComplete_data()
{
    QTest::addColumn<QStringList>("dsp");
    QTest::addColumn<QVariantList>("cmd");
    QTest::addColumn<QVariantList>("rsp");

    QStringList dsp;
    QVariantList cmd;
    QVariantList rsp;

    dsp<<"RID"<<"READ ALL"<<"INVALID";
    cmd.append(dataPool["WRITE_E"].first);
    cmd.append(dataPool["READ"].first);
    cmd.append(dataPool["INVALID"].first);
    rsp.append(dataPool["WRITE_E"].second);
    rsp.append(dataPool["READ"].second);
    rsp.append(dataPool["INVALID"].second);
    QTest::newRow("data 1")<<dsp<<cmd<<rsp;
}
void tst_qnearfieldtagtype1::testRemoveTargetBeforeAsyncRequestComplete()
{
    QFETCH(QStringList, dsp);
    QFETCH(QVariantList, cmd);
    QFETCH(QVariantList, rsp);
    tester.testRemoveTargetBeforeAsyncRequestComplete(dsp, cmd, rsp);
}

void tst_qnearfieldtagtype1::testCancelNdefOperation()
{
    tester.testCancelNdefOperation();
}

void tst_qnearfieldtagtype1::testRawAccessAndNdefAccess(const QList<QNdefMessage> &messages)
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

    // has Ndef message check
    QVERIFY(tester.target->hasNdefMessage());

    QNearFieldTarget::RequestId id = tester.target->readAll();
    QVERIFY(tester.target->waitForRequestCompleted(id));
    QByteArray allBlocks = tester.target->requestResponse(id).toByteArray();
    ++okCount;
    QCOMPARE(okSpy.count(), okCount);

    // Verify readByte
    QNearFieldTarget::RequestId id1 = tester.target->readByte(0x08);
    QVERIFY(tester.target->waitForRequestCompleted(id1));
    quint8 cc0 = tester.target->requestResponse(id1).value<quint8>();
    QCOMPARE(cc0, quint8(0xE1));
    ++okCount;
    QCOMPARE(okSpy.count(), okCount);

    QNearFieldTarget::RequestId id2 = tester.target->readByte(0x0d);
    QVERIFY(tester.target->waitForRequestCompleted(id2));
    quint8 len = tester.target->requestResponse(id2).value<quint8>();
    QCOMPARE(int(len), messages.at(0).toByteArray().count());
    ++okCount;
    QCOMPARE(okSpy.count(), okCount);

    QCOMPARE(allBlocks.mid(16, len), messages.at(0).toByteArray());

    // Write NDEF with raw command
    QNdefMessage message;
    QNdefNfcTextRecord textRecord;
    textRecord.setText("nfc");

    message.append(textRecord);

    QByteArray newNdefMessageContent = message.toByteArray();
    quint8 ndefMessageContentLen = newNdefMessageContent.count();

    QNearFieldTarget::RequestId id3 = tester.target->writeByte(0x0d, ndefMessageContentLen);
    QVERIFY(tester.target->waitForRequestCompleted(id3));
    QVERIFY(tester.target->requestResponse(id3).toBool());
    ++okCount;
    QCOMPARE(okSpy.count(), okCount);

    quint8 block = 1;
    quint8 byte = 6;
    for (int i = 0; i < ndefMessageContentLen; ++i)
    {
        quint8 addr = (block & 0x0F);
        addr <<= 3;
        addr |= (byte & 0x07);
        QNearFieldTarget::RequestId tempId = tester.target->writeByte(addr, newNdefMessageContent.at(i));
        QVERIFY(tester.target->waitForRequestCompleted(tempId));
        QVERIFY(tester.target->requestResponse(tempId).toBool());
        byte = (7 == byte) ? 0 : (byte+1);
        block = (0 == byte) ? (block+1) : block;
        ++okCount;
        QCOMPARE(okSpy.count(), okCount);
    }

    // read ndef with ndef access
    tester.target->readNdefMessages();
    ++ndefReadCount;
    QTRY_COMPARE(ndefMessageReadSpy.count(), ndefReadCount);

    const QNdefMessage& ndefMessage_new(ndefMessageReadSpy.first().at(0).value<QNdefMessage>());

    QCOMPARE(newNdefMessageContent, ndefMessage_new.toByteArray());
    QCOMPARE(errSpy.count(), errCount);
}

void tst_qnearfieldtagtype1::testRawAndNdefAccess()
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

QTEST_MAIN(tst_qnearfieldtagtype1);

#include "tst_qnearfieldtagtype1.moc"
