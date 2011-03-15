/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtTest/QtTest>

#include "../qconnectivitytestcommon.h"
#include "qnearfieldmanager_emulator_p.h"

#include <qnearfieldmanager.h>
#include <qndefmessage.h>
#include <qnearfieldtagtype1.h>
#include <qndefnfctextrecord.h>

Q_DECLARE_METATYPE(QNearFieldTarget*)

QTM_USE_NAMESPACE

class tst_QNearFieldTagType1 : public QObject
{
    Q_OBJECT

public:
    tst_QNearFieldTagType1();

private slots:
    void init();
    void cleanup();

    void staticMemoryModel();
    void dynamicMemoryModel();

    void ndefMessages();

private:
    void waitForMatchingTarget();

    QNearFieldManagerPrivateImpl *emulatorBackend;
    QNearFieldManager *manager;
    QNearFieldTagType1 *target;
};

tst_QNearFieldTagType1::tst_QNearFieldTagType1()
:   emulatorBackend(0), manager(0), target(0)
{
#ifndef Q_OS_SYMBIAN
    QDir::setCurrent(QLatin1String(SRCDIR));
#endif

    qRegisterMetaType<QNdefMessage>("QNdefMessage");
    qRegisterMetaType<QNearFieldTarget *>("QNearFieldTarget*");
}

void tst_QNearFieldTagType1::init()
{
    emulatorBackend = new QNearFieldManagerPrivateImpl;
    manager = new QNearFieldManager(emulatorBackend, 0);
}

void tst_QNearFieldTagType1::cleanup()
{
    emulatorBackend->reset();

    delete manager;
    manager = 0;
    emulatorBackend = 0;
    target = 0;
}

void tst_QNearFieldTagType1::waitForMatchingTarget()
{
    QSignalSpy targetDetectedSpy(manager, SIGNAL(targetDetected(QNearFieldTarget*)));

    manager->startTargetDetection(QNearFieldTarget::NfcTagType1);

    QTRY_VERIFY(!targetDetectedSpy.isEmpty());

    target =
        qobject_cast<QNearFieldTagType1 *>(targetDetectedSpy.first().at(0).value<QNearFieldTarget *>());

    manager->stopTargetDetection();

    QVERIFY(target);

    QCOMPARE(target->type(), QNearFieldTarget::NfcTagType1);
}

void tst_QNearFieldTagType1::staticMemoryModel()
{
    waitForMatchingTarget();

    QVERIFY(target->accessMethods() & QNearFieldTarget::TagTypeSpecificAccess);

    QCOMPARE(target->version(), quint8(0x10));

    // readIdentification()
    {
        QNearFieldTarget::RequestId id = target->readIdentification();
        QVERIFY(target->waitForRequestCompleted(id));

        const QByteArray data = target->requestResponse(id).toByteArray();

        QCOMPARE(data.length(), 6);

        quint8 hr0 = data.at(0);
        quint8 hr1 = data.at(1);
        const QByteArray uid4 = data.mid(2, 4);

        // verify NfcTagType1
        QVERIFY(hr0 & 0x10);

        QCOMPARE(uid4, target->uid().left(4));
    }

    // readAll()
    {
        // read all reads the first 120 bytes, prepended with HR0 and HR1.
        QNearFieldTarget::RequestId id = target->readAll();
        QVERIFY(target->waitForRequestCompleted(id));

        const QByteArray data = target->requestResponse(id).toByteArray();
        QCOMPARE(data.length(), 122);

        // verify NfcTagType1.
        QVERIFY(data.at(0) & 0x10);

        // NFC Magic Number means NDEF message present.
        QCOMPARE(quint8(data.at(10)) == 0xe1, target->hasNdefMessage());
    }

    // readByte()
    {
        QNearFieldTarget::RequestId id = target->readAll();
        QVERIFY(target->waitForRequestCompleted(id));

        const QByteArray data = target->requestResponse(id).toByteArray();

        for (int i = 0; i < 120; ++i) {
            id = target->readByte(i);
            QVERIFY(target->waitForRequestCompleted(id));

            quint8 byte = target->requestResponse(id).toUInt();

            QCOMPARE(byte, quint8(data.at(i + 2)));
        }
    }

    // writeByte()
    {
        for (int i = 0; i < 8; ++i) {
            QNearFieldTarget::RequestId id = target->readByte(i);
            QVERIFY(target->waitForRequestCompleted(id));

            quint8 byte = target->requestResponse(id).toUInt();

            id = target->writeByte(i, 0x55);
            QVERIFY(!target->waitForRequestCompleted(id));

            QVERIFY(!target->requestResponse(id).isValid());

            id = target->readByte(i);
            QVERIFY(target->waitForRequestCompleted(id));

            quint8 readByte = target->requestResponse(id).toUInt();
            QCOMPARE(readByte, byte);
        }

        for (int i = 8; i < 104; ++i) {
            // Write 0x55
            QNearFieldTarget::RequestId id = target->writeByte(i, 0x55);
            QVERIFY(target->waitForRequestCompleted(id));
            QVERIFY(target->requestResponse(id).toBool());

            id = target->readByte(i);
            QVERIFY(target->waitForRequestCompleted(id));
            quint8 readByte = target->requestResponse(id).toUInt();
            QCOMPARE(readByte, quint8(0x55));

            // Write 0xaa
            id = target->writeByte(i, 0xaa);
            QVERIFY(target->waitForRequestCompleted(id));
            QVERIFY(target->requestResponse(id).toBool());

            id = target->readByte(i);
            QVERIFY(target->waitForRequestCompleted(id));
            readByte = target->requestResponse(id).toUInt();
            QCOMPARE(readByte, quint8(0xaa));

            // Write 0x55 without erase, result should be 0xff
            id = target->writeByte(i, 0x55, QNearFieldTagType1::WriteOnly);
            QVERIFY(target->waitForRequestCompleted(id));
            QVERIFY(target->requestResponse(id).toBool());

            id = target->readByte(i);
            QVERIFY(target->waitForRequestCompleted(id));
            readByte = target->requestResponse(id).toUInt();
            QCOMPARE(readByte, quint8(0xff));
        }

        for (int i = 104; i < 120; ++i) {
            QNearFieldTarget::RequestId id = target->readByte(i);
            QVERIFY(target->waitForRequestCompleted(id));

            quint8 byte = target->requestResponse(id).toUInt();

            id = target->writeByte(i, 0x55);
            QVERIFY(!target->waitForRequestCompleted(id));

            QVERIFY(!target->requestResponse(id).isValid());

            id = target->readByte(i);
            QVERIFY(target->waitForRequestCompleted(id));

            quint8 readByte = target->requestResponse(id).toUInt();
            QCOMPARE(readByte, byte);
        }
    }
}

void tst_QNearFieldTagType1::dynamicMemoryModel()
{
    bool testedStatic = false;
    bool testedDynamic = false;

    QList<QByteArray> seenIds;
    forever {
        waitForMatchingTarget();

        QNearFieldTarget::RequestId id = target->readIdentification();
        QVERIFY(target->waitForRequestCompleted(id));

        const QByteArray data = target->requestResponse(id).toByteArray();
        if (seenIds.contains(data))
            break;
        else
            seenIds.append(data);

        quint8 hr0 = data.at(0);
        bool dynamic = (((hr0 & 0xf0) == 0x10) && ((hr0 & 0x0f) != 0x01));

        if (dynamic) {
            testedDynamic = true;

            // block 0, UID is locked
            {
                QNearFieldTarget::RequestId id = target->readBlock(0x00);
                QVERIFY(target->waitForRequestCompleted(id));

                const QByteArray block = target->requestResponse(id).toByteArray();

                QCOMPARE(target->uid(), block.left(7));
                QCOMPARE(quint8(block.at(7)), quint8(0x00));

                id = target->writeBlock(0x00, QByteArray(8, quint8(0x55)));
                QVERIFY(!target->waitForRequestCompleted(id));
                QVERIFY(!target->requestResponse(id).isValid());

                QCOMPARE(target->uid(), block.left(7));
                QCOMPARE(quint8(block.at(7)), quint8(0x00));
            }

            // static data area
            QNearFieldTarget::RequestId id = target->readSegment(0);
            QVERIFY(target->waitForRequestCompleted(id));
            QByteArray segment = target->requestResponse(id).toByteArray();
            for (int i = 1; i < 0x0d; ++i) {
                // Write 0x55
                id = target->writeBlock(i, QByteArray(8, quint8(0x55)));
                QVERIFY(target->waitForRequestCompleted(id));
                QVERIFY(target->requestResponse(id).toBool());

                id = target->readBlock(i);
                QVERIFY(target->waitForRequestCompleted(id));
                QCOMPARE(target->requestResponse(id).toByteArray(), QByteArray(8, quint8(0x55)));

                segment.replace(i * 8, 8, QByteArray(8, quint8(0x55)));

                id = target->readSegment(0);
                QVERIFY(target->waitForRequestCompleted(id));
                QCOMPARE(target->requestResponse(id).toByteArray(), segment);

                // Write 0xaa
                id = target->writeBlock(i, QByteArray(8, quint8(0xaa)));
                QVERIFY(target->waitForRequestCompleted(id));
                QVERIFY(target->requestResponse(id).toBool());

                id = target->readBlock(i);
                QVERIFY(target->waitForRequestCompleted(id));
                QCOMPARE(target->requestResponse(id).toByteArray(), QByteArray(8, quint8(0xaa)));

                segment.replace(i * 8, 8, QByteArray(8, quint8(0xaa)));

                id = target->readSegment(0);
                QVERIFY(target->waitForRequestCompleted(id));
                QCOMPARE(target->requestResponse(id).toByteArray(), segment);

                // Write 0x55 without erase, result should be 0xff
                id = target->writeBlock(i, QByteArray(8, quint8(0x55)),
                                        QNearFieldTagType1::WriteOnly);
                QVERIFY(target->waitForRequestCompleted(id));
                QVERIFY(target->requestResponse(id).toBool());

                id = target->readBlock(i);
                QVERIFY(target->waitForRequestCompleted(id));
                QCOMPARE(target->requestResponse(id).toByteArray(), QByteArray(8, quint8(0xff)));

                segment.replace(i * 8, 8, QByteArray(8, quint8(0xff)));

                id = target->readSegment(0);
                QVERIFY(target->waitForRequestCompleted(id));
                QCOMPARE(target->requestResponse(id).toByteArray(), segment);
            }

            // static / dynamic reserved lock area
            for (int i = 0x0d; i < 0x10; ++i) {
                id = target->readBlock(i);
                QVERIFY(target->waitForRequestCompleted(id));
                QByteArray block = target->requestResponse(id).toByteArray();

                id = target->writeBlock(i, QByteArray(8, quint8(0x55)));
                QVERIFY(!target->waitForRequestCompleted(id));
                QVERIFY(!target->requestResponse(id).isValid());

                id = target->readBlock(i);
                QVERIFY(target->waitForRequestCompleted(id));
                QCOMPARE(target->requestResponse(id).toByteArray(), block);
            }
        } else {
            testedStatic = true;

            for (int i = 0; i < 256; ++i) {
                QNearFieldTarget::RequestId id = target->readBlock(i);
                QVERIFY(!target->waitForRequestCompleted(id));

                id = target->writeBlock(i, QByteArray(8, quint8(0x55)));
                QVERIFY(!target->waitForRequestCompleted(id));
            }
            for (int i = 0; i < 16; ++i) {
                QNearFieldTarget::RequestId id = target->readSegment(i);
                QVERIFY(!target->waitForRequestCompleted(id));
            }
        }
    }

    QVERIFY(testedStatic);
    QVERIFY(testedDynamic);
}

void tst_QNearFieldTagType1::ndefMessages()
{
    QByteArray firstId;
    forever {
        waitForMatchingTarget();

        QNearFieldTarget::RequestId id = target->readIdentification();
        QVERIFY(target->waitForRequestCompleted(id));

        const QByteArray uid = target->requestResponse(id).toByteArray();
        if (firstId.isEmpty())
            firstId = uid;
        else if (firstId == uid)
            break;

        QVERIFY(target->hasNdefMessage());

        QSignalSpy ndefMessageReadSpy(target, SIGNAL(ndefMessageRead(QNdefMessage)));

        target->readNdefMessages();

        QTRY_VERIFY(!ndefMessageReadSpy.isEmpty());

        QList<QNdefMessage> ndefMessages;
        for (int i = 0; i < ndefMessageReadSpy.count(); ++i)
            ndefMessages.append(ndefMessageReadSpy.at(i).first().value<QNdefMessage>());

        QList<QNdefMessage> messages;
        QNdefNfcTextRecord textRecord;
        textRecord.setText(QLatin1String("tst_QNearFieldTagType1::ndefMessages"));

        QNdefMessage message;
        message.append(textRecord);

        if (target->memorySize() > 120) {
            QNdefRecord record;
            record.setTypeNameFormat(QNdefRecord::ExternalRtd);
            record.setType("com.nokia.qt:ndefMessagesTest");
            record.setPayload(QByteArray(120, quint8(0x55)));
            message.append(record);
        }

        messages.append(message);

        QSignalSpy ndefMessageWriteSpy(target, SIGNAL(ndefMessagesWritten()));
        target->writeNdefMessages(messages);

        QTRY_VERIFY(!ndefMessageWriteSpy.isEmpty());

        QVERIFY(target->hasNdefMessage());

        ndefMessageReadSpy.clear();

        target->readNdefMessages();

        QTRY_VERIFY(!ndefMessageReadSpy.isEmpty());

        QList<QNdefMessage> storedMessages;
        for (int i = 0; i < ndefMessageReadSpy.count(); ++i)
            storedMessages.append(ndefMessageReadSpy.at(i).first().value<QNdefMessage>());

        QVERIFY(ndefMessages != storedMessages);

        QVERIFY(messages == storedMessages);
    }
}

QTEST_MAIN(tst_QNearFieldTagType1);

#include "tst_qnearfieldtagtype1.moc"
