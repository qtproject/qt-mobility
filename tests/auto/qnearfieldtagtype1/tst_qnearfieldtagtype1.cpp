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
    QDir::setCurrent(QLatin1String(SRCDIR));

    qRegisterMetaType<QNdefMessage>("QNdefMessage");
    qRegisterMetaType<QNearFieldTarget *>("QNearFieldTarget*");
}

class MessageListener : public QObject
{
    Q_OBJECT

signals:
    void matchedNdefMessage(const QNdefMessage &message, QNearFieldTarget *target);
};

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
    MessageListener listener;
    QSignalSpy messageSpy(&listener, SIGNAL(matchedNdefMessage(QNdefMessage,QNearFieldTarget*)));

    int id = manager->registerTargetDetectedHandler(QNearFieldTarget::NfcTagType1, &listener,
                                                    SIGNAL(matchedNdefMessage(QNdefMessage,QNearFieldTarget*)));

    QVERIFY(id != -1);

    QTRY_VERIFY(!messageSpy.isEmpty());

    target =
        qobject_cast<QNearFieldTagType1 *>(messageSpy.first().at(1).value<QNearFieldTarget *>());

    QVERIFY(target);

    QCOMPARE(target->type(), QNearFieldTarget::NfcTagType1);
}

void tst_QNearFieldTagType1::staticMemoryModel()
{
    waitForMatchingTarget();

    // readIdentification()
    {
        const QByteArray id = target->readIdentification();

        QCOMPARE(id.length(), 6);

        quint8 hr0 = id.at(0);
        quint8 hr1 = id.at(1);
        const QByteArray uid4 = id.mid(2, 4);

        // verify NfcTagType1
        QVERIFY(hr0 & 0x10);

        QCOMPARE(uid4, target->uid().left(4));
    }

    // readAll()
    {
        // read all reads the first 120 bytes, prepended with HR0 and HR1.
        const QByteArray data = target->readAll();
        QCOMPARE(data.length(), 122);

        // verify NfcTagType1.
        QVERIFY(data.at(0) & 0x10);

        // NFC Magic Number means NDEF message present.
        QCOMPARE(quint8(data.at(10)) == 0xe1, target->hasNdefMessage());
    }

    // readByte()
    {
        const QByteArray data = target->readAll();

        for (int i = 0; i < 120; ++i) {
            quint8 byte = target->readByte(i);

            QCOMPARE(byte, quint8(data.at(i + 2)));
        }
    }

    // writeByte()
    {
        for (int i = 0; i < 8; ++i) {
            quint8 byte = target->readByte(i);
            QVERIFY(!target->writeByte(i, 0x55));
            QCOMPARE(target->readByte(i), byte);
        }

        for (int i = 8; i < 104; ++i) {
            QVERIFY(target->writeByte(i, 0x55));
            QCOMPARE(target->readByte(i), quint8(0x55));

            QVERIFY(target->writeByte(i, 0xaa));
            QCOMPARE(target->readByte(i), quint8(0xaa));

            QVERIFY(target->writeByte(i, 0x55, QNearFieldTagType1::WriteOnly));
            QCOMPARE(target->readByte(i), quint8(0xff));
        }

        for (int i = 104; i < 120; ++i) {
            quint8 byte = target->readByte(i);
            QVERIFY(!target->writeByte(i, 0x55));
            QCOMPARE(target->readByte(i), byte);
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

        const QByteArray id = target->readIdentification();
        if (seenIds.contains(id))
            break;
        else
            seenIds.append(id);

        quint8 hr0 = id.at(0);
        bool dynamic = (((hr0 & 0xf0) == 0x10) && ((hr0 & 0x0f) != 0x01));

        if (dynamic) {
            testedDynamic = true;

            // block 0, UID is locked
            {
                QByteArray block = target->readBlock(0x00);
                QCOMPARE(target->uid(), block.left(7));
                QCOMPARE(quint8(block.at(7)), quint8(0x00));
                QVERIFY(!target->writeBlock(0x00, QByteArray(8, quint8(0x55))));
                QCOMPARE(target->uid(), block.left(7));
                QCOMPARE(quint8(block.at(7)), quint8(0x00));
            }

            // static data area
            QByteArray segment = target->readSegment(0);
            for (int i = 1; i < 0x0d; ++i) {
                QVERIFY(target->writeBlock(i, QByteArray(8, quint8(0x55))));
                QCOMPARE(target->readBlock(i), QByteArray(8, quint8(0x55)));
                segment.replace(i * 8, 8, QByteArray(8, quint8(0x55)));
                QCOMPARE(target->readSegment(0), segment);

                QVERIFY(target->writeBlock(i, QByteArray(8, quint8(0xaa))));
                QCOMPARE(target->readBlock(i), QByteArray(8, quint8(0xaa)));
                segment.replace(i * 8, 8, QByteArray(8, quint8(0xaa)));
                QCOMPARE(target->readSegment(0), segment);

                QVERIFY(target->writeBlock(i, QByteArray(8, quint8(0x55)), QNearFieldTagType1::WriteOnly));
                QCOMPARE(target->readBlock(i), QByteArray(8, quint8(0xff)));
                segment.replace(i * 8, 8, QByteArray(8, quint8(0xff)));
                QCOMPARE(target->readSegment(0), segment);
            }

            // static / dynamic reserved lock area
            for (int i = 0x0d; i < 0x10; ++i) {
                QByteArray block = target->readBlock(i);
                QVERIFY(!target->writeBlock(i, QByteArray(8, quint8(0x55))));
                QCOMPARE(target->readBlock(i), block);
            }
        } else {
            testedStatic = true;

            for (int i = 0; i < 256; ++i) {
                QVERIFY(target->readBlock(i).isEmpty());
                QVERIFY(!target->writeBlock(i, QByteArray(8, quint8(0x55))));
            }
            for (int i = 0; i < 16; ++i) {
                QVERIFY(target->readSegment(i).isEmpty());
            }
        }
    }
}

void tst_QNearFieldTagType1::ndefMessages()
{
    QByteArray firstId;
    forever {
        waitForMatchingTarget();

        QByteArray id = target->readIdentification();
        if (firstId.isEmpty())
            firstId = id;
        else if (firstId == id)
            break;

        QVERIFY(target->hasNdefMessage());

        QList<QNdefMessage> ndefMessages = target->ndefMessages();

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

        target->setNdefMessages(messages);

        QVERIFY(target->hasNdefMessage());

        QList<QNdefMessage> storedMessages = target->ndefMessages();

        QVERIFY(ndefMessages != storedMessages);

        QVERIFY(messages == storedMessages);
    }
}

QTEST_MAIN(tst_QNearFieldTagType1);

#include "tst_qnearfieldtagtype1.moc"
