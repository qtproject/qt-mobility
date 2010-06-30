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

#include "tst_qversitwriter.h"
#include "qversitwriter.h"
#include "qversitdocument.h"
#include "qversitproperty.h"
#include <QtTest/QtTest>
#include <QByteArray>

// Copied from tst_qcontactmanager.cpp
// Waits until __expr is true and fails if it doesn't happen within 5s.
#ifndef QTRY_VERIFY
#define QTRY_VERIFY(__expr) \
        do { \
        const int __step = 50; \
        const int __timeout = 5000; \
        if (!(__expr)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && !(__expr); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QVERIFY(__expr); \
    } while(0)
#endif

QTM_USE_NAMESPACE

void tst_QVersitWriter::init()
{
    mOutputDevice = new QBuffer;
    mWriter = new QVersitWriter;
    mSignalCatcher = new SignalCatcher;
    connect(mWriter, SIGNAL(stateChanged(QVersitWriter::State)),
            mSignalCatcher, SLOT(stateChanged(QVersitWriter::State)));
}

void tst_QVersitWriter::cleanup()
{
    delete mWriter;
    delete mOutputDevice;
    delete mSignalCatcher;
}

void tst_QVersitWriter::testDevice()
{
    // No device
    QVERIFY(mWriter->device() == NULL);

    // Device has been set
    mWriter->setDevice(mOutputDevice);
    QVERIFY(mWriter->device() == mOutputDevice);
}

void tst_QVersitWriter::testDefaultCodec()
{
    QVERIFY(mWriter->defaultCodec() == 0);
    mWriter->setDefaultCodec(QTextCodec::codecForName("UTF-16BE"));
    QVERIFY(mWriter->defaultCodec() == QTextCodec::codecForName("UTF-16BE"));
}

void tst_QVersitWriter::testFold()
{
    // 87 characters long
    QString longString(QLatin1String(
        "4567890123456789012345678901234567890123456789012345678901234567890123456"
        "234567890123456789012345678901234567890123456789012345678901234567890123456"
        "234567890123456789012"));
    QByteArray expected(
            "BEGIN:VCARD\r\n"
            "VERSION:2.1\r\n"
            "FN:4567890123456789012345678901234567890123456789012345678901234567890123456\r\n"
            " 234567890123456789012345678901234567890123456789012345678901234567890123456\r\n"
            " 234567890123456789012\r\n"
            "END:VCARD\r\n");
    QVersitDocument document;
    document.setComponentType(QLatin1String("VCARD"));
    QVersitProperty property;
    property.setName(QLatin1String("FN"));
    property.setValue(longString);
    document.addProperty(property);
    document.setType(QVersitDocument::VCard21Type);
    QList<QVersitDocument> list;
    list.append(document);
    mWriter->setDevice(mOutputDevice);
    mOutputDevice->open(QBuffer::ReadWrite);
    QVERIFY(mWriter->startWriting(list));
    QVERIFY(mWriter->waitForFinished());
    QCOMPARE(mWriter->state(), QVersitWriter::FinishedState);
    QCOMPARE(mWriter->error(), QVersitWriter::NoError);
    mOutputDevice->seek(0);
    QByteArray result(mOutputDevice->readAll());
    QCOMPARE(result, expected);
}

void tst_QVersitWriter::testWriting21()
{
    // vCard 2.1
    QByteArray vCard21(
"BEGIN:VCARD\r\n\
VERSION:2.1\r\n\
FN:John\r\n\
END:VCARD\r\n");
    QVersitDocument document;
    document.setComponentType(QLatin1String("VCARD"));
    QVersitProperty property;
    property.setName(QString(QString::fromAscii("FN")));
    property.setValue(QString::fromAscii("John"));
    document.addProperty(property);
    document.setType(QVersitDocument::VCard21Type);
    QList<QVersitDocument> list;
    list.append(document);

    // Device not set
    QCOMPARE(mWriter->state(), QVersitWriter::InactiveState);
    QCOMPARE(mWriter->error(), QVersitWriter::NoError);
    QVERIFY(!mWriter->startWriting(list));
    QCOMPARE(mWriter->state(), QVersitWriter::InactiveState);
    QCOMPARE(mWriter->error(), QVersitWriter::IOError);
    QVERIFY(!mWriter->waitForFinished());

    // Device not opened
    mWriter->setDevice(mOutputDevice);
    QVERIFY(!mWriter->startWriting(list));
    QCOMPARE(mWriter->state(), QVersitWriter::InactiveState);
    QCOMPARE(mWriter->error(), QVersitWriter::IOError);

    // Now open the device and it should work.
    mOutputDevice->open(QBuffer::ReadWrite);
    QVERIFY(mWriter->startWriting(list));
    QVERIFY(mWriter->waitForFinished());
    QCOMPARE(mWriter->state(), QVersitWriter::FinishedState);
    QCOMPARE(mWriter->error(), QVersitWriter::NoError);
    mOutputDevice->seek(0);
    QByteArray result(mOutputDevice->readAll());
    QCOMPARE(result, vCard21);

    // Try some other codec
    delete mOutputDevice;
    mOutputDevice = new QBuffer;
    mOutputDevice->open(QBuffer::ReadWrite);
    mWriter->setDevice(mOutputDevice);
    QTextCodec* utf16(QTextCodec::codecForName("UTF-16"));
    mWriter->setDefaultCodec(utf16);
    QVERIFY(mWriter->startWriting(list));
    QVERIFY(mWriter->waitForFinished());
    QCOMPARE(mWriter->state(), QVersitWriter::FinishedState);
    QCOMPARE(mWriter->error(), QVersitWriter::NoError);
    mOutputDevice->seek(0);
    result = mOutputDevice->readAll();
    QByteArray expected(utf16->fromUnicode(QLatin1String(vCard21.data())));
    QCOMPARE(result, expected);
}

void tst_QVersitWriter::testWriting30()
{
    // vCard 3.0
    QByteArray vCard30(
"BEGIN:VCARD\r\n\
VERSION:3.0\r\n\
FN:John\r\n\
END:VCARD\r\n");

    QVersitDocument document;
    document.setComponentType(QLatin1String("VCARD"));
    QVersitProperty property;
    property.setName(QString(QString::fromAscii("FN")));
    property.setValue(QString::fromAscii("John"));
    document.addProperty(property);
    document.setType(QVersitDocument::VCard30Type);
    QList<QVersitDocument> list;
    list.append(document);

    // Basic 3.0 test
    mOutputDevice->open(QBuffer::ReadWrite);
    mWriter->setDevice(mOutputDevice);
    QVERIFY(mWriter->startWriting(list));
    QVERIFY(mWriter->waitForFinished());
    QCOMPARE(mWriter->state(), QVersitWriter::FinishedState);
    QCOMPARE(mWriter->error(), QVersitWriter::NoError);
    mOutputDevice->seek(0);
    QByteArray result(mOutputDevice->readAll());
    QCOMPARE(result, vCard30);
    
    qApp->processEvents(); // clean up before we start sniffing signals
    
    // Asynchronous writing
    mOutputDevice->reset();
    mSignalCatcher->mReceived.clear();
    QVERIFY(mWriter->startWriting(list));
    QTRY_VERIFY(mSignalCatcher->mReceived.count() >= 2);
    QCOMPARE(mSignalCatcher->mReceived.at(0), QVersitWriter::ActiveState);
    QCOMPARE(mSignalCatcher->mReceived.at(1), QVersitWriter::FinishedState);

    // Cancelling
    delete mOutputDevice;
    mOutputDevice = new QBuffer;
    mOutputDevice->open(QBuffer::ReadWrite);
    mSignalCatcher->mReceived.clear();
    mWriter->setDevice(mOutputDevice);
    mWriter->startWriting(list);
    mWriter->cancel();
    mWriter->waitForFinished();
    QTRY_VERIFY(mSignalCatcher->mReceived.count() >= 2);
    QCOMPARE(mSignalCatcher->mReceived.at(0), QVersitWriter::ActiveState);
    QVersitWriter::State state(mSignalCatcher->mReceived.at(1));
    // It's possible that it finishes before it cancels.
    QVERIFY(state == QVersitWriter::CanceledState
            || state == QVersitWriter::FinishedState);
}

void tst_QVersitWriter::testByteArrayOutput()
{
    const QByteArray vCard30(
        "BEGIN:VCARD\r\n"
        "VERSION:3.0\r\n"
        "FN:John\r\n"
        "END:VCARD\r\n");

    delete mWriter; // we don't want the init()ed writer.

    QByteArray output;
    mWriter = new QVersitWriter(&output);

    QVERIFY(mWriter->device() == 0);

    QVersitDocument document(QVersitDocument::VCard30Type);
    document.setComponentType(QLatin1String("VCARD"));
    QVersitProperty property;
    property.setName(QString(QString::fromAscii("FN")));
    property.setValue(QString::fromAscii("John"));
    document.addProperty(property);
    QVERIFY(mWriter->startWriting(QList<QVersitDocument>() << document));
    QVERIFY(mWriter->waitForFinished());
    QCOMPARE(output, vCard30);
}

void tst_QVersitWriter::testWritingDocument()
{
    QFETCH(QVersitDocument, document);
    QFETCH(QByteArray, expected);

    mOutputDevice->open(QBuffer::ReadWrite);
    mWriter->setDevice(mOutputDevice);
    QVERIFY(mWriter->startWriting(QList<QVersitDocument>() << document));
    QVERIFY(mWriter->waitForFinished());
    mOutputDevice->seek(0);
    QByteArray result(mOutputDevice->readAll());

    QCOMPARE(result, expected);
}

void tst_QVersitWriter::testWritingDocument_data()
{
    QTest::addColumn<QVersitDocument>("document");
    QTest::addColumn<QByteArray>("expected");

    QVersitDocument document(QVersitDocument::VCard21Type);
    document.setComponentType(QLatin1String("VCARD"));
    QVersitProperty property;
    property.setName(QLatin1String("FN"));
    property.setValue(QLatin1String("Bob"));
    document.addProperty(property);
    QTest::newRow("basic vCard 2.1") << document << QByteArray(
            "BEGIN:VCARD\r\n"
            "VERSION:2.1\r\n"
            "FN:Bob\r\n"
            "END:VCARD\r\n"
            );

    document.setComponentType(QLatin1String("VCARD"));
    document.setType(QVersitDocument::VCard30Type);
    QTest::newRow("basic vCard 3.0") << document << QByteArray(
            "BEGIN:VCARD\r\n"
            "VERSION:3.0\r\n"
            "FN:Bob\r\n"
            "END:VCARD\r\n"
            );

    {
    QVersitDocument document(QVersitDocument::ICalendar20Type);
    document.setComponentType(QLatin1String("VCALENDAR"));
    QVersitProperty property;
    property.setName(QLatin1String("PRODID"));
    property.setValue(QLatin1String("-//hacksw/handcal//NONSGML v1.0//EN"));
    document.addProperty(property);
    QVersitDocument nested(QVersitDocument::ICalendar20Type);
    nested.setComponentType(QLatin1String("VEVENT"));
    property.setName(QLatin1String("DTSTART"));
    property.setValue(QLatin1String("19970714T170000Z"));
    nested.addProperty(property);
    property.setName(QLatin1String("DTEND"));
    property.setValue(QLatin1String("19970715T035959Z"));
    nested.addProperty(property);
    property.setName(QLatin1String("SUMMARY"));
    property.setValue(QLatin1String("Bastille Day Party"));
    nested.addProperty(property);
    document.addSubDocument(nested);
    QTest::newRow("iCalendar 2.0 from spec") << document << QByteArray(
                    "BEGIN:VCALENDAR\r\n"
                    "VERSION:2.0\r\n"
                    "PRODID:-//hacksw/handcal//NONSGML v1.0//EN\r\n"
                    "BEGIN:VEVENT\r\n"
                    "DTSTART:19970714T170000Z\r\n"
                    "DTEND:19970715T035959Z\r\n"
                    "SUMMARY:Bastille Day Party\r\n"
                    "END:VEVENT\r\n"
                    "END:VCALENDAR\r\n");
    }
}

QTEST_MAIN(tst_QVersitWriter)
