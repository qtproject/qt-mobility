/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tst_qversitreader.h"
#include "qversitreader.h"
#include "qversitreader_p.h"
#include "versitutils_p.h"
#include <QtTest/QtTest>
#include <QSignalSpy>

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

// This says "NOKIA" in Katakana encoded with UTF-8
const QByteArray KATAKANA_NOKIA("\xe3\x83\x8e\xe3\x82\xad\xe3\x82\xa2");

QTM_USE_NAMESPACE

void tst_QVersitReader::init()
{
    mInputDevice = new QBuffer;
    mInputDevice->open(QBuffer::ReadWrite);
    mReader = new QVersitReader;
    mReaderPrivate = new QVersitReaderPrivate;
    mSignalCatcher = new SignalCatcher;
    connect(mReader, SIGNAL(stateChanged(QVersitReader::State)),
            mSignalCatcher, SLOT(stateChanged(QVersitReader::State)));
    connect(mReader, SIGNAL(resultsAvailable()),
            mSignalCatcher, SLOT(resultsAvailable()));
    mAsciiCodec = QTextCodec::codecForName("ISO 8859-1");
}

void tst_QVersitReader::cleanup()
{
    delete mReaderPrivate;
    delete mReader;
    delete mInputDevice;
    delete mSignalCatcher;
}

void tst_QVersitReader::testDevice()
{
    // No device
    QVERIFY(mReader->device() == NULL);

    // Device has been set
    mReader->setDevice(mInputDevice);
    QVERIFY(mReader->device() == mInputDevice);

    delete mInputDevice;
    QVERIFY(mReader->device() == NULL);

    mInputDevice = new QBuffer;
    mInputDevice->open(QBuffer::ReadWrite);

    QVERIFY(mReader->device() == NULL);
    mReader->setDevice(mInputDevice);
    QVERIFY(mReader->device() == mInputDevice);
}

void tst_QVersitReader::testDefaultCodec()
{
    QVERIFY(mReader->defaultCodec() == QTextCodec::codecForName("UTF-8"));
    mReader->setDefaultCodec(QTextCodec::codecForName("UTF-16BE"));
    QVERIFY(mReader->defaultCodec() == QTextCodec::codecForName("UTF-16BE"));
}

void tst_QVersitReader::testReading()
{
    // No I/O device set
    QVERIFY(!mReader->startReading());
    QCOMPARE(mReader->error(), QVersitReader::IOError);

    // Device set, no data
    mReader->setDevice(mInputDevice);
    mInputDevice->open(QBuffer::ReadOnly);
    QVERIFY(mReader->startReading());
    QVERIFY(mReader->waitForFinished());
    QList<QVersitDocument> results(mReader->results());
    QCOMPARE(mReader->state(), QVersitReader::FinishedState);
    QCOMPARE(mReader->error(), QVersitReader::NoError);
    QCOMPARE(results.count(),0);

    // Device set, one document
    const QByteArray& oneDocument =
        "BEGIN:VCARD\r\nVERSION:2.1\r\nFN:John\r\nEND:VCARD\r\n";
    mInputDevice->close();
    mInputDevice->setData(oneDocument);
    mInputDevice->open(QBuffer::ReadOnly);
    mInputDevice->seek(0);
    QVERIFY(mReader->startReading());
    QVERIFY(mReader->waitForFinished());
    results = mReader->results();
    QCOMPARE(mReader->state(), QVersitReader::FinishedState);
    QCOMPARE(mReader->error(), QVersitReader::NoError);
    QCOMPARE(results.count(),1);

    // Wide charset with no byte-order mark
    QTextCodec* codec = QTextCodec::codecForName("UTF-16BE");
    const QByteArray& wideDocument =
            VersitUtils::encode("BEGIN:VCARD\r\nVERSION:2.1\r\nFN:John\r\nEND:VCARD\r\n", codec);
    mInputDevice->close();
    mInputDevice->setData(wideDocument);
    mInputDevice->open(QBuffer::ReadOnly);
    mInputDevice->seek(0);
    mReader->setDefaultCodec(codec);
    QVERIFY(mReader->startReading());
    QVERIFY(mReader->waitForFinished());
    results = mReader->results();
    QCOMPARE(mReader->state(), QVersitReader::FinishedState);
    QCOMPARE(mReader->error(), QVersitReader::NoError);
    QCOMPARE(mReader->results().count(),1);
    mReader->setDefaultCodec(NULL);

    // Two documents
    const QByteArray& twoDocuments =
        " \r\n BEGIN:VCARD\r\nFN:Jenny\r\nEND:VCARD\r\nBEGIN:VCARD\r\nFN:Jake\r\nEND:VCARD\r\n";
    mInputDevice->close();
    mInputDevice->setData(twoDocuments);
    mInputDevice->open(QBuffer::ReadOnly);
    mInputDevice->seek(0);
    QVERIFY(mReader->startReading());
    QVERIFY(mReader->waitForFinished());
    results = mReader->results();
    QCOMPARE(mReader->state(), QVersitReader::FinishedState);
    QCOMPARE(mReader->error(), QVersitReader::NoError);
    QCOMPARE(results.count(),2);

    // Erroneous document (missing property name)
    mInputDevice->close();
    mInputDevice->setData(QByteArray(
            "BEGIN:VCARD\r\nFN:Jenny\r\n;Jenny;;;\r\nEND:VCARD\r\n"
            "BEGIN:VCARD\r\nFN:Jake\r\nEND:VCARD\r\n"));
    mInputDevice->open(QBuffer::ReadOnly);
    mInputDevice->seek(0);
    QVERIFY(mReader->startReading());
    QVERIFY(mReader->waitForFinished());
    results = mReader->results();
    QCOMPARE(mReader->state(), QVersitReader::FinishedState);
    QCOMPARE(mReader->error(), QVersitReader::ParseError);
    QCOMPARE(results.count(), 1);

    // Valid documents and a grouped document between them
    const QByteArray& validDocumentsAndGroupedDocument =
"BEGIN:VCARD\r\nFN:Jenny\r\nEND:VCARD\r\n\
BEGIN:VCARD\r\n\
X-GROUPING:pub gang\r\n\
BEGIN:VCARD\r\nFN:Jeremy\r\nEND:VCARD\r\n\
BEGIN:VCARD\r\nFN:Jeffery\r\nEND:VCARD\r\n\
END:VCARD\r\n\
BEGIN:VCARD\r\nFN:Jake\r\nEND:VCARD\r\n\
BEGIN:VCARD\r\nFN:James\r\nEND:VCARD\r\n\
BEGIN:VCARD\r\nFN:Jane\r\nEND:VCARD\r\n";
    mInputDevice->close();
    mInputDevice->setData(validDocumentsAndGroupedDocument);
    mInputDevice->open(QBuffer::ReadWrite);
    mInputDevice->seek(0);
    QVERIFY(mReader->startReading());
    QVERIFY(mReader->waitForFinished());
    results = mReader->results();
    QCOMPARE(mReader->state(), QVersitReader::FinishedState);
    // An error is logged because one failed, but the rest are readable.
    QCOMPARE(mReader->error(), QVersitReader::ParseError);
    QCOMPARE(results.count(),4);

    // Valid documents and a grouped document between them
    const QByteArray& validDocumentsAndGroupedDocument2 =
"BEGIN:VCARD\r\nFN:Jenny\r\nEND:VCARD\r\n\
BEGIN:VCARD\r\n\
X-GROUPING:pub gang\r\n\
BEGIN:VCARD\r\nFN:Jeremy\r\nEND:VCARD\r\n\
BEGIN:VCARD\r\nFN:Jeffery\r\nEND:VCARD\r\n\
END:VCARD\r\n\
BEGIN:VCARD\r\nFN:Jake\r\nEND:VCARD\r\n\
BEGIN:VCARD\r\nFN:James\r\nEND:VCARD\r\n\
BEGIN:VCARD\r\nFN:Jane\r\nEND:VCARD";
    mInputDevice->close();
    mInputDevice->setData(validDocumentsAndGroupedDocument2);
    mInputDevice->open(QBuffer::ReadWrite);
    mInputDevice->seek(0);
    QVERIFY(mReader->startReading());
    QVERIFY(mReader->waitForFinished());
    results = mReader->results();
    QCOMPARE(mReader->state(), QVersitReader::FinishedState);
    // An error is logged because one failed, but the rest are readable.
    QCOMPARE(mReader->error(), QVersitReader::ParseError);
    QCOMPARE(mReader->results().count(),4);

    // Asynchronous reading
    mInputDevice->close();
    mInputDevice->setData(twoDocuments);
    mInputDevice->open(QBuffer::ReadWrite);
    mInputDevice->seek(0);
    mSignalCatcher->mStateChanges.clear();
    mSignalCatcher->mResultsCount = 0;
    QVERIFY(mReader->startReading());
    QTRY_VERIFY(mSignalCatcher->mStateChanges.count() >= 2);
    QCOMPARE(mSignalCatcher->mStateChanges.at(0), QVersitReader::ActiveState);
    QCOMPARE(mSignalCatcher->mStateChanges.at(1), QVersitReader::FinishedState);
    QVERIFY(mSignalCatcher->mResultsCount >= 2);
    QCOMPARE(mReader->results().size(), 2);
    QCOMPARE(mReader->error(), QVersitReader::NoError);

    // Cancelling
    mInputDevice->close();
    mInputDevice->setData(twoDocuments);
    mInputDevice->open(QBuffer::ReadOnly);
    mInputDevice->seek(0);
    mSignalCatcher->mStateChanges.clear();
    mSignalCatcher->mResultsCount = 0;
    QVERIFY(mReader->startReading());
    mReader->cancel();
    mReader->waitForFinished();
    QTRY_VERIFY(mSignalCatcher->mStateChanges.count() >= 2);
    QCOMPARE(mSignalCatcher->mStateChanges.at(0), QVersitReader::ActiveState);
    QVersitReader::State state(mSignalCatcher->mStateChanges.at(1));
    // It's possible that it finishes before it cancels.
    QVERIFY(state == QVersitReader::CanceledState
            || state == QVersitReader::FinishedState);
}

void tst_QVersitReader::testResult()
{
    QCOMPARE(mReader->results().count(),0);
}

void tst_QVersitReader::testSetVersionFromProperty()
{
    QVersitDocument document;

    // Some other property than VERSION
    QVersitProperty property;
    property.setName(QString::fromAscii("N"));
    QVERIFY(mReaderPrivate->setVersionFromProperty(document,property));

    // VERSION property with 2.1
    property.setName(QString::fromAscii("VERSION"));
    property.setValue(QString::fromAscii("2.1"));
    QVERIFY(mReaderPrivate->setVersionFromProperty(document,property));
    QVERIFY(document.type() == QVersitDocument::VCard21Type);

    // VERSION property with 3.0
    property.setValue(QString::fromAscii("3.0"));
    QVERIFY(mReaderPrivate->setVersionFromProperty(document,property));
    QVERIFY(document.type() == QVersitDocument::VCard30Type);

    // VERSION property with a not supported value
    property.setValue(QString::fromAscii("4.0"));
    QVERIFY(!mReaderPrivate->setVersionFromProperty(document,property));

    // VERSION property with BASE64 encoded supported value
    property.setValue(QString::fromAscii(QByteArray("2.1").toBase64()));
    property.insertParameter(QString::fromAscii("ENCODING"),QString::fromAscii("BASE64"));
    QVERIFY(mReaderPrivate->setVersionFromProperty(document,property));
    QVERIFY(document.type() == QVersitDocument::VCard21Type);

    // VERSION property with BASE64 encoded not supported value
    property.setValue(QString::fromAscii(QByteArray("4.0").toBase64()));
    QVERIFY(!mReaderPrivate->setVersionFromProperty(document,property));
}

void tst_QVersitReader::testParseNextVersitPropertyVCard21()
{
    QVersitDocument::VersitType type = QVersitDocument::VCard21Type;

    // Test a valid vCard 2.1 with properties having separate handling:
    // AGENT property, ENCODING parameters (BASE64 and QUOTED-PRINTABLE) and CHARSET parameter
    QTextCodec* codec = QTextCodec::codecForName("UTF-16BE");
    QByteArray vCard("Begin:vcard\r\n");
    vCard.append("VERSION:2.1\r\n");
    vCard.append("FN:John\r\n");
    // "NOTE:\;\,\:\\"
    vCard.append("NOTE:\\;\\,\\:\\\\\r\n");
    // "N:foo\;bar;foo\,bar;foo\:bar;foo\\bar;foo\\\;bar"
    vCard.append("N:foo\\;bar;foo\\,bar;foo\\:bar;foo\\\\bar;foo\\\\\\;bar\r\n");
    // missing structured value
    vCard.append("ADR:\r\n");
    // "NICKNAME:foo\;bar,foo\,bar,foo\:bar,foo\\bar,foo\\\,bar"
    vCard.append("NICKNAME:foo\\;bar,foo\\,bar,foo\\:bar,foo\\\\bar,foo\\\\\\,bar\r\n");
    // "CATEGORIES:foo\;bar,foo\,bar,foo\:bar,foo\\bar,foo\\\,bar"
    vCard.append("CATEGORIES:foo\\;bar,foo\\,bar,foo\\:bar,foo\\\\bar,foo\\\\\\,bar\r\n");
    vCard.append("ORG;CHARSET=UTF-8:");
    vCard.append(KATAKANA_NOKIA);
    vCard.append("\r\n");
    // "NOKIA" in Katakana, UTF-8 encoded, then base-64 encoded:
    vCard.append("NOTE;ENCODING=BASE64;CHARSET=UTF-8:");
    vCard.append(KATAKANA_NOKIA.toBase64());
    vCard.append("\r\n");
    // The value here is "UXQgaXMgZ3JlYXQh", which is the base64 encoding of "Qt is great!".
    vCard.append("PHOTO;ENCODING=BASE64: U\t XQgaX MgZ\t3Jl YXQh\r\n\r\n");
    // Again, but without the explicit "ENCODING" parameter
    vCard.append("PHOTO;BASE64: U\t XQgaX MgZ\t3Jl YXQh\r\n\r\n");
    vCard.append("HOME.Springfield.EMAIL;Encoding=Quoted-Printable:john.citizen=40exam=\r\nple.com\r\n");
    vCard.append("EMAIL;ENCODING=QUOTED-PRINTABLE;CHARSET=UTF-16BE:");
    vCard.append(codec->fromUnicode(QLatin1String("john.citizen=40exam=\r\nple.com")));
    vCard.append("\r\n");
    vCard.append("AGENT:\r\nBEGIN:VCARD\r\nFN:Jenny\r\nEND:VCARD\r\n\r\n");
    vCard.append("End:VCARD\r\n");
    QBuffer buffer(&vCard);
    buffer.open(QIODevice::ReadOnly);
    LineReader lineReader(&buffer, mAsciiCodec);

    QVersitProperty property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString::fromAscii("BEGIN"));
    QCOMPARE(property.value(),QString::fromAscii("vcard"));

    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString::fromAscii("VERSION"));
    QCOMPARE(property.value(),QString::fromAscii("2.1"));

    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString::fromAscii("FN"));
    QCOMPARE(property.value(),QString::fromAscii("John"));

    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString::fromAscii("NOTE"));
    // Do not Unescape semicolons, commas, colons and backlashes
    // "\;\,\:\\"
    QCOMPARE(property.value(),QString::fromAscii("\\;\\,\\:\\\\"));

    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString::fromAscii("N"));
    QCOMPARE(property.valueType(), QVersitProperty::CompoundType);
    QCOMPARE(property.variantValue().type(), QVariant::StringList);
    QStringList components = property.value<QStringList>();
    QCOMPARE(components.size(), 5);
    QCOMPARE(components.at(0), QLatin1String("foo;bar"));
    QCOMPARE(components.at(1), QLatin1String("foo\\,bar"));
    QCOMPARE(components.at(2), QLatin1String("foo\\:bar"));
    QCOMPARE(components.at(3), QLatin1String("foo\\\\bar"));
    QCOMPARE(components.at(4), QLatin1String("foo\\\\;bar"));

    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString::fromAscii("ADR"));
    QCOMPARE(property.valueType(), QVersitProperty::CompoundType);
    QCOMPARE(property.variantValue().type(), QVariant::StringList);
    components = property.value<QStringList>();
    QCOMPARE(components.size(), 1);
    QVERIFY(components.at(0).isEmpty());

    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString::fromAscii("NICKNAME"));
    QCOMPARE(property.valueType(), QVersitProperty::ListType);
    QCOMPARE(property.variantValue().type(), QVariant::StringList);
    components = property.value<QStringList>();
    QCOMPARE(components.size(), 5);
    QCOMPARE(components.at(0), QLatin1String("foo\\;bar"));
    QCOMPARE(components.at(1), QLatin1String("foo,bar"));
    QCOMPARE(components.at(2), QLatin1String("foo\\:bar"));
    QCOMPARE(components.at(3), QLatin1String("foo\\\\bar"));
    QCOMPARE(components.at(4), QLatin1String("foo\\\\,bar"));

    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString::fromAscii("CATEGORIES"));
    QCOMPARE(property.valueType(), QVersitProperty::ListType);
    QCOMPARE(property.variantValue().type(), QVariant::StringList);
    components = property.value<QStringList>();
    QCOMPARE(components.size(), 5);
    QCOMPARE(components.at(0), QLatin1String("foo\\;bar"));
    QCOMPARE(components.at(1), QLatin1String("foo,bar"));
    QCOMPARE(components.at(2), QLatin1String("foo\\:bar"));
    QCOMPARE(components.at(3), QLatin1String("foo\\\\bar"));
    QCOMPARE(components.at(4), QLatin1String("foo\\\\,bar"));

    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString::fromAscii("ORG"));
    QCOMPARE(property.value(),QString::fromUtf8(KATAKANA_NOKIA));

    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString::fromAscii("NOTE"));
    QCOMPARE(property.value(),QString::fromUtf8(KATAKANA_NOKIA));

    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString::fromAscii("PHOTO"));
    // Linear whitespaces (SPACEs and TABs) removed from the value and base64 decoded:
    QCOMPARE(property.variantValue().type(), QVariant::ByteArray);
    QCOMPARE(property.value<QByteArray>(), QByteArray("Qt is great!"));
    // Ensure that base-64 encoded strings can be retrieved as strings.
    QCOMPARE(property.value(), QLatin1String("Qt is great!"));

    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString::fromAscii("PHOTO"));
    QCOMPARE(property.variantValue().type(), QVariant::ByteArray);
    QCOMPARE(property.value<QByteArray>(), QByteArray("Qt is great!"));
    QCOMPARE(property.value(), QLatin1String("Qt is great!"));

    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QStringList propertyGroup(QString::fromAscii("HOME"));
    propertyGroup.append(QString::fromAscii("Springfield"));
    QCOMPARE(property.groups(),propertyGroup);
    QCOMPARE(property.name(),QString::fromAscii("EMAIL"));
    QCOMPARE(0,property.parameters().count());
    QCOMPARE(property.value(),QString::fromAscii("john.citizen@example.com"));

    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString::fromAscii("EMAIL"));
    // The encoding and charset parameters should be stripped by the reader.
    QCOMPARE(property.parameters().count(), 0);
    QCOMPARE(property.value(),QString::fromAscii("john.citizen@example.com"));

    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString::fromAscii("AGENT"));
    QCOMPARE(property.value(),QString());
    QVERIFY(property.variantValue().userType() == qMetaTypeId<QVersitDocument>());
    QCOMPARE(property.value<QVersitDocument>().properties().count(), 1);

    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString::fromAscii("END"));
    QCOMPARE(property.value(),QString::fromAscii("VCARD"));

    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString());
    QCOMPARE(property.value(),QString());

    // Simulate a situation where the document nesting level is exceeded
    // In practice this would mean a big number of nested AGENT properties
    mReaderPrivate->mDocumentNestingLevel = 20;
    QByteArray agentProperty("AGENT:BEGIN:VCARD\r\nN:Jenny\r\nEND:VCARD\r\n\r\n");
    buffer.close();
    buffer.setData(agentProperty);
    buffer.open(QIODevice::ReadOnly);
    LineReader agentLineReader(&buffer, mAsciiCodec);

    property = mReaderPrivate->parseNextVersitProperty(type, agentLineReader);
    QVERIFY(property.isEmpty());
}

void tst_QVersitReader::testParseNextVersitPropertyVCard30()
{
    QVersitDocument::VersitType type = QVersitDocument::VCard30Type;

    // Test a valid vCard 3.0 with properties having separate handling:
    // AGENT property and some other property
    QByteArray vCard("Begin:vcard\r\n");
    vCard.append("VERSION:3.0\r\n");
    vCard.append("FN:John\r\n");
    // "NOTE:\;\,\:\\"
    vCard.append("NOTE:\\;\\,\\:\\\\\r\n");
    // "N:foo\;bar;foo\,bar;foo\:bar;foo\\bar;foo\\\;bar"
    vCard.append("N:foo\\;bar;foo\\,bar;foo\\:bar;foo\\\\bar;foo\\\\\\;bar\r\n");
    // "NICKNAME:foo\;bar,foo\,bar,foo\:bar,foo\\bar,foo\\\,bar"
    vCard.append("NICKNAME:foo\\;bar,foo\\,bar,foo\\:bar,foo\\\\bar,foo\\\\\\,bar\r\n");
    // "CATEGORIES:foo\;bar,foo\,bar,foo\:bar,foo\\bar,foo\\\,bar"
    vCard.append("CATEGORIES:foo\\;bar,foo\\,bar,foo\\:bar,foo\\\\bar,foo\\\\\\,bar\r\n");
    // "CATEGORIES:foobar\\,foobar\\\\,foo\\\\\,bar"
    vCard.append("CATEGORIES:foobar\\\\,foobar\\\\\\\\,foo\\\\\\\\\\,bar\r\n");
    vCard.append("ORG;CHARSET=UTF-8:");
    vCard.append(KATAKANA_NOKIA);
    vCard.append("\r\n");
    // "NOKIA" in Katakana, UTF-8 encoded, then base-64 encoded:
    vCard.append("NOTE;ENCODING=B;CHARSET=UTF-8:");
    vCard.append(KATAKANA_NOKIA.toBase64());
    vCard.append("\r\n");
    vCard.append("TEL;TYPE=PREF;HOME:123\r\n");
    // The value here is "UXQgaXMgZ3JlYXQh", which is the base64 encoding of "Qt is great!".
    vCard.append("PHOTO;ENCODING=B:UXQgaXMgZ3JlYXQh\r\n");
    // Again, but without the explicity "ENCODING" parameter
    vCard.append("PHOTO;B:UXQgaXMgZ3JlYXQh\r\n");
    vCard.append("EMAIL:john.citizen@example.com\r\n");
    vCard.append("AGENT:BEGIN:VCARD\\nFN:Jenny\\nEND:VCARD\\n\r\n");
    vCard.append("End:VCARD\r\n");
    QBuffer buffer(&vCard);
    buffer.open(QIODevice::ReadOnly);
    LineReader lineReader(&buffer, mAsciiCodec);

    QVersitProperty property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString::fromAscii("BEGIN"));
    QCOMPARE(property.value(),QString::fromAscii("vcard"));

    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString::fromAscii("VERSION"));
    QCOMPARE(property.value(),QString::fromAscii("3.0"));

    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString::fromAscii("FN"));
    QCOMPARE(property.value(),QString::fromAscii("John"));

    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString::fromAscii("NOTE"));
    QCOMPARE(property.valueType(), QVersitProperty::PlainType);
    QCOMPARE(property.variantValue().type(), QVariant::String);
    // Unescape semicolons, commas, colons and backlashes
    QCOMPARE(property.value(), QString::fromAscii(";,:\\"));

    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString::fromAscii("N"));
    QCOMPARE(property.valueType(), QVersitProperty::CompoundType);
    QCOMPARE(property.variantValue().type(), QVariant::StringList);
    QStringList components = property.value<QStringList>();
    QCOMPARE(components.size(), 5);
    QCOMPARE(components.at(0), QLatin1String("foo;bar"));
    QCOMPARE(components.at(1), QLatin1String("foo,bar"));
    QCOMPARE(components.at(2), QLatin1String("foo:bar"));
    QCOMPARE(components.at(3), QLatin1String("foo\\bar"));
    QCOMPARE(components.at(4), QLatin1String("foo\\;bar"));

    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString::fromAscii("NICKNAME"));
    QCOMPARE(property.valueType(), QVersitProperty::ListType);
    QCOMPARE(property.variantValue().type(), QVariant::StringList);
    components = property.value<QStringList>();
    QCOMPARE(components.size(), 5);
    QCOMPARE(components.at(0), QLatin1String("foo;bar"));
    QCOMPARE(components.at(1), QLatin1String("foo,bar"));
    QCOMPARE(components.at(2), QLatin1String("foo:bar"));
    QCOMPARE(components.at(3), QLatin1String("foo\\bar"));
    QCOMPARE(components.at(4), QLatin1String("foo\\,bar"));

    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString::fromAscii("CATEGORIES"));
    QCOMPARE(property.valueType(), QVersitProperty::ListType);
    QCOMPARE(property.variantValue().type(), QVariant::StringList);
    components = property.value<QStringList>();
    QCOMPARE(components.size(), 5);
    QCOMPARE(components.at(0), QLatin1String("foo;bar"));
    QCOMPARE(components.at(1), QLatin1String("foo,bar"));
    QCOMPARE(components.at(2), QLatin1String("foo:bar"));
    QCOMPARE(components.at(3), QLatin1String("foo\\bar"));
    QCOMPARE(components.at(4), QLatin1String("foo\\,bar"));

    // "CATEGORIES:foobar\\,foobar\\\\,foo\\\\\,bar"
    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString::fromAscii("CATEGORIES"));
    QCOMPARE(property.valueType(), QVersitProperty::ListType);
    QCOMPARE(property.variantValue().type(), QVariant::StringList);
    components = property.value<QStringList>();
    QCOMPARE(components.size(), 3);
    QCOMPARE(components.at(0), QLatin1String("foobar\\"));
    QCOMPARE(components.at(1), QLatin1String("foobar\\\\"));
    QCOMPARE(components.at(2), QLatin1String("foo\\\\,bar"));

    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString::fromAscii("ORG"));
    QCOMPARE(property.value(),QString::fromUtf8(KATAKANA_NOKIA));

    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString::fromAscii("NOTE"));
    QCOMPARE(property.value(),QString::fromUtf8(KATAKANA_NOKIA));

    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString::fromAscii("TEL"));
    QCOMPARE(property.value(),QString::fromAscii("123"));
    QCOMPARE(property.parameters().count(), 2);

    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString::fromAscii("PHOTO"));
    QCOMPARE(property.variantValue().type(), QVariant::ByteArray);
    QCOMPARE(property.value<QByteArray>(), QByteArray("Qt is great!"));
    // Ensure that base-64 encoded strings can be retrieved as strings.
    QCOMPARE(property.value(), QLatin1String("Qt is great!"));

    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString::fromAscii("PHOTO"));
    QCOMPARE(property.variantValue().type(), QVariant::ByteArray);
    QCOMPARE(property.value<QByteArray>(), QByteArray("Qt is great!"));
    QCOMPARE(property.value(), QLatin1String("Qt is great!"));

    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString::fromAscii("EMAIL"));
    QCOMPARE(0,property.parameters().count());
    QCOMPARE(property.value(),QString::fromAscii("john.citizen@example.com"));

    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString::fromAscii("AGENT"));
    QVERIFY(property.variantValue().userType() == qMetaTypeId<QVersitDocument>());
    QCOMPARE(property.value<QVersitDocument>().properties().count(), 1);

    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString::fromAscii("END"));
    QCOMPARE(property.value(),QString::fromAscii("VCARD"));

    property = mReaderPrivate->parseNextVersitProperty(type, lineReader);
    QCOMPARE(property.name(),QString());
    QCOMPARE(property.value(),QString());

    // Simulate a situation where the document nesting level is exceeded
    // In practice this would mean a big number of nested AGENT properties
    mReaderPrivate->mDocumentNestingLevel = 20;
    QByteArray agentProperty("AGENT:BEGIN\\:VCARD\\nFN\\:Jenny\\nEND\\:VCARD\\n\r\n");
    buffer.close();
    buffer.setData(agentProperty);
    buffer.open(QIODevice::ReadOnly);
    LineReader agentLineReader(&buffer, mAsciiCodec);

    property = mReaderPrivate->parseNextVersitProperty(type, agentLineReader);
    QVERIFY(property.isEmpty());
}

void tst_QVersitReader::testParseVersitDocument()
{
    QFETCH(QByteArray, vCard);
    QFETCH(bool, expectedSuccess);
    QFETCH(int, expectedProperties);

    QBuffer buffer(&vCard);
    buffer.open(QIODevice::ReadOnly);
    LineReader lineReader(&buffer, QTextCodec::codecForName("UTF-8"));

    QVersitDocument document;
    QCOMPARE(mReaderPrivate->parseVersitDocument(lineReader, document), expectedSuccess);
    QCOMPARE(document.properties().count(), expectedProperties);
    QCOMPARE(mReaderPrivate->mDocumentNestingLevel, 0);
}

void tst_QVersitReader::testParseVersitDocument_data()
{
    QTest::addColumn<QByteArray>("vCard");
    QTest::addColumn<bool>("expectedSuccess");
    QTest::addColumn<int>("expectedProperties");

    QTest::newRow("Basic vCard 2.1")
            << QByteArray(
                    "BEGIN:VCARD\r\n"
                    "VERSION:2.1\r\n"
                    "FN:John\r\n"
                    "END:VCARD\r\n")
            << true
            << 1;

    QTest::newRow("vCard 2.1 with Agent")
            << QByteArray(
                    "BEGIN:VCARD\r\n"
                    "VERSION:2.1\r\n"
                    "FN:John\r\n"
                    "AGENT:BEGIN:VCARD\r\nN:Jenny\r\nEND:VCARD\r\n\r\n"
                    "EMAIL;ENCODING=QUOTED-PRINTABLE:john.citizen=40exam=\r\nple.com\r\n"
                    "END:VCARD\r\n")
            << true
            << 3;

    QTest::newRow("vCard 3.0 with Agent")
            << QByteArray(
                    "BEGIN:VCARD\r\n"
                    "VERSION:3.0\r\n"
                    "FN:John\r\n"
                    "AGENT:BEGIN\\:VCARD\\nN\\:Jenny\\nEND\\:VCARD\\n\r\n"
                    "EMAIL:john.citizen@example.com\r\n"
                    "END:VCARD\r\n")
            << true
            << 3;

    QTest::newRow("No BEGIN found")
            << QByteArray(
                    "VCARD\r\n"
                    "VERSION:2.1\r\n"
                    "FN:Nobody\r\n"
                    "END:VCARD\r\n")
            << false
            << 0;

    QTest::newRow("Wrong card type")
            << QByteArray(
                    "BEGIN:VCAL\r\n"
                    "END:VCAL\r\n")
            << false
            << 0;

    QTest::newRow("Wrong version")
            << QByteArray(
                    "BEGIN:VCARD\r\n"
                    "VERSION:4.0\r\n"
                    "FN:Nobody\r\n"
                    "END:VCARD\r\n")
            << false
            << 0;

    QTest::newRow("No trailing crlf")
            << QByteArray(
                    "BEGIN:VCARD\r\n"
                    "VERSION:2.1\r\n"
                    "FN:Nobody\r\n"
                    "END:VCARD")
            << true
            << 1;

    QTest::newRow("No end")
            << QByteArray(
                    "BEGIN:VCARD\r\n"
                    "VERSION:2.1\r\n"
                    "FN:Nobody\r\n")
            << false
            << 0;

    QTest::newRow("Grouped vCards are not supported. The whole vCard will be discarded.")
            << QByteArray(
                    "BEGIN:VCARD\r\n"
                    "VERSION:2.1\r\n"
                    "X-EXAMPLES:Family vCard\r\n"
                    "BEGIN:VCARD\r\n"
                    "VERSION:2.1\r\n"
                    "N:Citizen;John\r\n"
                    "TEL;CELL:1111\r\n"
                    "EMAIL;ENCODING=QUOTED-PRINTABLE:john.citizen=40example.com\r\n"
                    "END:VCARD\r\n"
                    "BEGIN:VCARD\r\n"
                    "VERSION:2.1\r\n"
                    "N:Citizen;Jenny\r\n"
                    "TEL;CELL:7777\r\n"
                    "END:VCARD\r\n"
                    "END:VCARD")
            << false
            << 0;
}

void tst_QVersitReader::testDecodeQuotedPrintable()
{
    // Soft line breaks
    QString encoded(QLatin1String("This=\r\n is =\r\none line."));
    QString decoded(QLatin1String("This is one line."));
    mReaderPrivate->decodeQuotedPrintable(encoded);
    QCOMPARE(encoded, decoded);

    // Characters recommended to be encoded according to RFC 1521:
    encoded = QLatin1String("To be decoded: =0A=0D=21=22=23=24=3D=40=5B=5C=5D=5E=60=7B=7C=7D=7E");
    decoded = QLatin1String("To be decoded: \n\r!\"#$=@[\\]^`{|}~");
    mReaderPrivate->decodeQuotedPrintable(encoded);
    QCOMPARE(encoded, decoded);

    // Other random characters encoded.
    // Some implementation may encode these too, as it is allowed.
    encoded = QLatin1String("=45=6E=63=6F=64=65=64 =64=61=74=61");
    decoded = QLatin1String("Encoded data");
    mReaderPrivate->decodeQuotedPrintable(encoded);
    QCOMPARE(encoded, decoded);
}
void tst_QVersitReader::testParamName()
{
    // Empty value
    QByteArray param;
    QCOMPARE(mReaderPrivate->paramName(param, mAsciiCodec),QString());

    // Only value present
    param = "WORK";
    QCOMPARE(mReaderPrivate->paramName(param, mAsciiCodec),
             QString::fromAscii("TYPE"));

    // The below tests intentionally use the misspelling TIPE to avoid the default behaviour of
    // returning TYPE when the name can't be parsed.
    // Both name and value, spaces after the name
    param = "TIPE \t =WORK";
    QCOMPARE(mReaderPrivate->paramName(param, mAsciiCodec),
             QString::fromAscii("TIPE"));

    // Both name and value, no spaces after the name
    param = "TIPE=WORK";
    QCOMPARE(mReaderPrivate->paramName(param, mAsciiCodec),
             QString::fromAscii("TIPE"));

    // Test wide character support.
    QTextCodec* codec = QTextCodec::codecForName("UTF-16BE");
    param = codec->fromUnicode(QString::fromAscii("TIPE=WORK"));
    QCOMPARE(mReaderPrivate->paramName(param, codec),
             QString::fromAscii("TIPE"));

}

void tst_QVersitReader::testParamValue()
{
    // Empty value
    QByteArray param;
    QCOMPARE(mReaderPrivate->paramValue(param, mAsciiCodec),QString());

    // Only value present
    param = "WORK";
    QCOMPARE(mReaderPrivate->paramValue(param, mAsciiCodec),
             QString::fromAscii("WORK"));

    // Name and equals sign, but no value
    param = "TYPE=";
    QCOMPARE(mReaderPrivate->paramValue(param, mAsciiCodec),QString());

    // Name and equals sign, but value has only spaces
    param = "TYPE=  \t ";
    QCOMPARE(mReaderPrivate->paramValue(param, mAsciiCodec),QString());

    // Both name and value, spaces before the value
    param = "TYPE= \t  WORK";
    QCOMPARE(mReaderPrivate->paramValue(param, mAsciiCodec),
             QString::fromAscii("WORK"));

    // Both name and value, no spaces before the value
    param = "ENCODING=QUOTED-PRINTABLE";
    QCOMPARE(mReaderPrivate->paramValue(param, mAsciiCodec),
             QString::fromAscii("QUOTED-PRINTABLE"));

    // Test wide character support.
    QTextCodec* codec = QTextCodec::codecForName("UTF-16BE");
    param = codec->fromUnicode(QString::fromAscii("TYPE=WORK"));
    QCOMPARE(mReaderPrivate->paramValue(param, codec),
             QString::fromAscii("WORK"));
}

void tst_QVersitReader::testExtractPart()
{
    QByteArray originalStr;

    // Negative starting position
    QCOMPARE(mReaderPrivate->extractPart(originalStr,-1,1), QByteArray());

    // Empty original string
    QCOMPARE(mReaderPrivate->extractPart(originalStr,0,1), QByteArray());

    // Trimmed substring empty
    originalStr = " \t \t";
    QCOMPARE(mReaderPrivate->extractPart(originalStr,0,4), QByteArray());

    // The given substring length is greater than the original string length
    originalStr = "ENCODING=7BIT";
    QCOMPARE(mReaderPrivate->extractPart(originalStr,0,100), originalStr);

    // Non-empty substring, from the beginning
    originalStr = " TYPE=WORK ; X-PARAM=X-VALUE; ENCODING=8BIT";
    QCOMPARE(mReaderPrivate->extractPart(originalStr,0,11),
             QByteArray("TYPE=WORK"));

    // Non-empty substring, from the middle
    QCOMPARE(mReaderPrivate->extractPart(originalStr,12,16),
             QByteArray("X-PARAM=X-VALUE"));

    // Non-empty substring, from the middle to the end
    QCOMPARE(mReaderPrivate->extractPart(originalStr,29),
             QByteArray("ENCODING=8BIT"));
}

void tst_QVersitReader::testExtractParts()
{
    QList<QByteArray> parts;

    // Empty value
    QByteArray text;
    parts = mReaderPrivate->extractParts(text,";", mAsciiCodec);
    QVERIFY(parts.isEmpty());

    // Only separator
    text = ";";
    parts = mReaderPrivate->extractParts(text,";", mAsciiCodec);
    QVERIFY(parts.isEmpty());

    // One part
    text = "part";
    parts = mReaderPrivate->extractParts(text,";", mAsciiCodec);
    QCOMPARE(parts.count(),1);
    QCOMPARE(QString::fromAscii(parts[0]),QString::fromAscii("part"));

    // Separator in the beginning, one part
    text = ";part";
    parts = mReaderPrivate->extractParts(text,";", mAsciiCodec);
    QCOMPARE(parts.count(),1);
    QCOMPARE(QString::fromAscii(parts[0]),QString::fromAscii("part"));

    // Separator in the end, one part
    text = "part;";
    parts = mReaderPrivate->extractParts(text,";", mAsciiCodec);
    QCOMPARE(parts.count(),1);
    QCOMPARE(QString::fromAscii(parts[0]),QString::fromAscii("part"));

    // One part that contains escaped separator
    text = "part\\;";
    parts = mReaderPrivate->extractParts(text,";", mAsciiCodec);
    QCOMPARE(parts.count(),1);
    QCOMPARE(QString::fromAscii(parts[0]),QString::fromAscii("part\\;"));

    // Two parts
    text = "part1;part2";
    parts = mReaderPrivate->extractParts(text,";", mAsciiCodec);
    QCOMPARE(parts.count(),2);
    QCOMPARE(QString::fromAscii(parts[0]),QString::fromAscii("part1"));
    QCOMPARE(QString::fromAscii(parts[1]),QString::fromAscii("part2"));

    // Two parts that contain escaped separators
    text = "pa\\;rt1;par\\;t2";
    parts = mReaderPrivate->extractParts(text,";", mAsciiCodec);
    QCOMPARE(parts.count(),2);
    QCOMPARE(QString::fromAscii(parts[0]),QString::fromAscii("pa\\;rt1"));
    QCOMPARE(QString::fromAscii(parts[1]),QString::fromAscii("par\\;t2"));

    // Test wide character support
    QTextCodec* codec = QTextCodec::codecForName("UTF-16BE");
    text = codec->fromUnicode(QString::fromAscii("part1;part2"));
    parts = mReaderPrivate->extractParts(text,";", codec);
    QCOMPARE(parts.count(),2);
    QCOMPARE(codec->toUnicode(parts[0]),QString::fromAscii("part1"));
    QCOMPARE(codec->toUnicode(parts[1]),QString::fromAscii("part2"));
}

void tst_QVersitReader::testExtractPropertyGroupsAndName()
{
    QPair<QStringList,QString> groupsAndName;

    // Empty string
    VersitCursor cursor(QByteArray(" "));
    groupsAndName = mReaderPrivate->extractPropertyGroupsAndName(cursor, mAsciiCodec);
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QString());

    // No value -> returns empty string and no groups
    QByteArray property("TEL");
    cursor.setData(property);
    cursor.selection = property.size();
    groupsAndName = mReaderPrivate->extractPropertyGroupsAndName(cursor, mAsciiCodec);
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QString());

    // Simple name and value
    property = "TEL:123";
    cursor.setData(property);
    cursor.selection = property.size();
    groupsAndName = mReaderPrivate->extractPropertyGroupsAndName(cursor, mAsciiCodec);
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QString::fromAscii("TEL"));

    // One whitespace before colon
    property = "TEL :123";
    cursor.setData(property);
    cursor.selection = property.size();
    groupsAndName = mReaderPrivate->extractPropertyGroupsAndName(cursor, mAsciiCodec);
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QString::fromAscii("TEL"));

    // Several whitespaces before colon
    property = "TEL \t  :123";
    cursor.setData(property);
    cursor.selection = property.size();
    groupsAndName = mReaderPrivate->extractPropertyGroupsAndName(cursor, mAsciiCodec);
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QString::fromAscii("TEL"));

    // Name contains a group
    property = "group1.TEL:1234";
    cursor.setData(property);
    cursor.selection = property.size();
    groupsAndName = mReaderPrivate->extractPropertyGroupsAndName(cursor, mAsciiCodec);
    QCOMPARE(groupsAndName.first.count(),1);
    QCOMPARE(groupsAndName.first.takeFirst(),QString::fromAscii("group1"));
    QCOMPARE(groupsAndName.second,QString::fromAscii("TEL"));

    // Name contains more than one group
    property = "group1.group2.TEL:12345";
    cursor.setData(property);
    cursor.selection = property.size();
    groupsAndName = mReaderPrivate->extractPropertyGroupsAndName(cursor, mAsciiCodec);
    QCOMPARE(groupsAndName.first.count(),2);
    QCOMPARE(groupsAndName.first.takeFirst(),QString::fromAscii("group1"));
    QCOMPARE(groupsAndName.first.takeFirst(),QString::fromAscii("group2"));
    QCOMPARE(groupsAndName.second,QString::fromAscii("TEL"));
    QCOMPARE(cursor.position, 17);

    // Property contains one parameter
    property = "TEL;WORK:123";
    cursor.setData(property);
    cursor.selection = property.size();
    groupsAndName = mReaderPrivate->extractPropertyGroupsAndName(cursor, mAsciiCodec);
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QString::fromAscii("TEL"));

    // Property contains several parameters
    property = "EMAIL;INTERNET;ENCODING=QUOTED-PRINTABLE:user=40ovi.com";
    cursor.setData(property);
    cursor.selection = property.size();
    groupsAndName = mReaderPrivate->extractPropertyGroupsAndName(cursor, mAsciiCodec);
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QString::fromAscii("EMAIL"));

    // Name contains an escaped semicolon
    property = "X-proper\\;ty:value";
    cursor.setData(property);
    cursor.selection = property.size();
    groupsAndName = mReaderPrivate->extractPropertyGroupsAndName(cursor, mAsciiCodec);
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QString::fromAscii("X-proper\\;ty"));

    // Test wide character support
    QTextCodec* codec = QTextCodec::codecForName("UTF-16BE");
    property = codec->fromUnicode(QString::fromAscii("group1.group2.TEL;WORK:123"));
    cursor.setData(property);
    cursor.selection = property.size();
    groupsAndName = mReaderPrivate->extractPropertyGroupsAndName(cursor, codec);
    QCOMPARE(groupsAndName.first.count(),2);
    QCOMPARE(groupsAndName.first.takeFirst(),QString::fromAscii("group1"));
    QCOMPARE(groupsAndName.first.takeFirst(),QString::fromAscii("group2"));
    QCOMPARE(groupsAndName.second,QString::fromAscii("TEL"));
    QCOMPARE(cursor.position, 36); // 2 bytes * 17 characters + 2 byte BOM.

}

void tst_QVersitReader::testExtractVCard21PropertyParams()
{
    // No parameters
    VersitCursor cursor(QByteArray(":123"));
    cursor.setSelection(cursor.data.size());
    QCOMPARE(mReaderPrivate->extractVCard21PropertyParams(cursor, mAsciiCodec).count(), 0);

    // "Empty" parameter
    cursor.setData(QByteArray(";:123"));
    cursor.setSelection(cursor.data.size());
    QCOMPARE(mReaderPrivate->extractVCard21PropertyParams(cursor, mAsciiCodec).count(), 0);

    // Semicolon found, but no value for the property
    cursor.setData(QByteArray(";TYPE=X-TYPE"));
    cursor.setSelection(cursor.data.size());
    QCOMPARE(mReaderPrivate->extractVCard21PropertyParams(cursor, mAsciiCodec).count(), 0);

    // The property name contains an escaped semicolon, no parameters
    cursor.setData(QByteArray(":value"));
    cursor.setSelection(cursor.data.size());
    QCOMPARE(mReaderPrivate->extractVCard21PropertyParams(cursor, mAsciiCodec).count(), 0);

    // The property value contains a semicolon, no parameters
    cursor.setData(QByteArray(":va;lue"));
    cursor.setSelection(cursor.data.size());
    QCOMPARE(mReaderPrivate->extractVCard21PropertyParams(cursor, mAsciiCodec).count(), 0);

    // One parameter
    cursor.setData(QByteArray(";HOME:123"));
    cursor.setSelection(cursor.data.size());
    QMultiHash<QString,QString> params = mReaderPrivate->extractVCard21PropertyParams(cursor,
                                                                                   mAsciiCodec);
    QCOMPARE(1, params.count());
    QCOMPARE(1, params.values(QString::fromAscii("TYPE")).count());
    QCOMPARE(params.values(QString::fromAscii("TYPE"))[0],QString::fromAscii("HOME"));

    // Two parameters of the same type
    cursor.setData(QByteArray(";HOME;VOICE:123"));
    cursor.setSelection(cursor.data.size());
    params = mReaderPrivate->extractVCard21PropertyParams(cursor, mAsciiCodec);
    QCOMPARE(2, params.count());
    QCOMPARE(2, params.values(QString::fromAscii("TYPE")).count());
    QCOMPARE(params.values(QString::fromAscii("TYPE"))[0],QString::fromAscii("HOME"));
    QCOMPARE(params.values(QString::fromAscii("TYPE"))[1],QString::fromAscii("VOICE"));

    // Two parameters, several empty parameters (extra semicolons)
    cursor.setData(QByteArray(";;;;HOME;;;;;VOICE;;;:123"));
    cursor.setSelection(cursor.data.size());
    params = mReaderPrivate->extractVCard21PropertyParams(cursor, mAsciiCodec);
    QCOMPARE(2, params.count());
    QCOMPARE(2, params.values(QString::fromAscii("TYPE")).count());
    QCOMPARE(params.values(QString::fromAscii("TYPE"))[0],QString::fromAscii("HOME"));
    QCOMPARE(params.values(QString::fromAscii("TYPE"))[1],QString::fromAscii("VOICE"));

    // Two parameters with different types
    cursor.setData(QByteArray(";INTERNET;ENCODING=QUOTED-PRINTABLE:user=40ovi.com"));
    cursor.setSelection(cursor.data.size());
    params.clear();
    params = mReaderPrivate->extractVCard21PropertyParams(cursor, mAsciiCodec);
    QCOMPARE(2, params.count());
    QList<QString> typeParams = params.values(QString::fromAscii("TYPE"));
    QCOMPARE(1, typeParams.count());
    QCOMPARE(typeParams[0],QString::fromAscii("INTERNET"));
    QList<QString> encodingParams = params.values(QString::fromAscii("ENCODING"));
    QCOMPARE(1, encodingParams.count());
    QCOMPARE(encodingParams[0],QString::fromAscii("QUOTED-PRINTABLE"));

    // Test wide character support.
    QTextCodec* codec = QTextCodec::codecForName("UTF-16BE");
    QByteArray data = VersitUtils::encode(";HOME;CHARSET=UTF-16:123", codec);
    cursor.setData(data);
    cursor.setSelection(cursor.data.size());
    params = mReaderPrivate->extractVCard21PropertyParams(cursor, codec);
    QCOMPARE(2, params.count());
    typeParams = params.values(QString::fromAscii("TYPE"));
    QCOMPARE(1, typeParams.count());
    QCOMPARE(typeParams[0],QString::fromAscii("HOME"));
    encodingParams = params.values(QString::fromAscii("CHARSET"));
    QCOMPARE(1, encodingParams.count());
    QCOMPARE(encodingParams[0],QString::fromAscii("UTF-16"));
}

void tst_QVersitReader::testExtractVCard30PropertyParams()
{
    // No parameters
    VersitCursor cursor(QByteArray(":123"));
    cursor.setSelection(cursor.data.size());
    QCOMPARE(mReaderPrivate->extractVCard30PropertyParams(cursor, mAsciiCodec).count(), 0);

    // One parameter
    cursor.setData(QByteArray(";TYPE=HOME:123"));
    cursor.setSelection(cursor.data.size());
    QMultiHash<QString,QString> params = mReaderPrivate->extractVCard30PropertyParams(cursor,
                                                                                   mAsciiCodec);
    QCOMPARE(params.count(), 1);
    QCOMPARE(params.values(QString::fromAscii("TYPE")).count(), 1);
    QCOMPARE(params.values(QString::fromAscii("TYPE"))[0], QString::fromAscii("HOME"));

    // One parameter with an escaped semicolon
    cursor.setData(QByteArray(";para\\;meter:value"));
    cursor.setSelection(cursor.data.size());
    params = mReaderPrivate->extractVCard30PropertyParams(cursor, mAsciiCodec);
    QCOMPARE(params.count(), 1);
    QCOMPARE(params.values(QString::fromAscii("TYPE")).count(), 1);
    QCOMPARE(params.values(QString::fromAscii("TYPE"))[0], QString::fromAscii("para;meter"));

    // One parameter with and escaped comma in the name and the value
    cursor.setData(QByteArray(";X-PA\\,RAM=VAL\\,UE:123"));
    cursor.setSelection(cursor.data.size());
    params = mReaderPrivate->extractVCard30PropertyParams(cursor, mAsciiCodec);
    QCOMPARE(params.count(), 1);
    QCOMPARE(params.values(QString::fromAscii("X-PA,RAM")).count(), 1);
    QCOMPARE(params.values(QString::fromAscii("X-PA,RAM"))[0], QString::fromAscii("VAL,UE"));

    // Two parameters of the same type
    cursor.setData(QByteArray(";TYPE=HOME,VOICE:123"));
    cursor.setSelection(cursor.data.size());
    params = mReaderPrivate->extractVCard30PropertyParams(cursor, mAsciiCodec);
    QCOMPARE(params.count(), 2);
    QCOMPARE(params.values(QString::fromAscii("TYPE")).count(), 2);
    QVERIFY(params.values(QString::fromAscii("TYPE")).contains(QString::fromAscii("HOME")));
    QVERIFY(params.values(QString::fromAscii("TYPE")).contains(QString::fromAscii("VOICE")));

    // Two parameters of the same type in separate name-values
    cursor.setData(QByteArray(";TYPE=HOME;TYPE=VOICE:123"));
    cursor.setSelection(cursor.data.size());
    params = mReaderPrivate->extractVCard30PropertyParams(cursor, mAsciiCodec);
    QCOMPARE(params.count(), 2);
    QCOMPARE(params.values(QString::fromAscii("TYPE")).count(), 2);
    QVERIFY(params.values(QString::fromAscii("TYPE")).contains(QString::fromAscii("HOME")));
    QVERIFY(params.values(QString::fromAscii("TYPE")).contains(QString::fromAscii("VOICE")));

    // Three parameters of the same type
    cursor.setData(QByteArray(";TYPE=PREF,HOME,VOICE:123"));
    cursor.setSelection(cursor.data.size());
    params = mReaderPrivate->extractVCard30PropertyParams(cursor, mAsciiCodec);
    QCOMPARE(params.count(), 3);
    QCOMPARE(params.values(QString::fromAscii("TYPE")).count(), 3);
    QVERIFY(params.values(QString::fromAscii("TYPE")).contains(QString::fromAscii("PREF")));
    QVERIFY(params.values(QString::fromAscii("TYPE")).contains(QString::fromAscii("HOME")));
    QVERIFY(params.values(QString::fromAscii("TYPE")).contains(QString::fromAscii("VOICE")));

    // Two parameters with different types
    cursor.setData(QByteArray(";TYPE=HOME;X-PARAM=X-VALUE:Home Street 1"));
    cursor.setSelection(cursor.data.size());
    params.clear();
    params = mReaderPrivate->extractVCard30PropertyParams(cursor, mAsciiCodec);
    QCOMPARE(params.count(), 2);
    QList<QString> typeParams = params.values(QString::fromAscii("TYPE"));
    QCOMPARE(typeParams.count(), 1);
    QCOMPARE(typeParams[0],QString::fromAscii("HOME"));
    QList<QString> encodingParams = params.values(QString::fromAscii("X-PARAM"));
    QCOMPARE(encodingParams.count(), 1);
    QCOMPARE(encodingParams[0],QString::fromAscii("X-VALUE"));

    // Test wide character support.
    QTextCodec* codec = QTextCodec::codecForName("UTF-16BE");
    QByteArray data = VersitUtils::encode(";TIPE=HOME,VOICE;CHARSET=UTF-16:123", codec);
    cursor.setData(data);
    cursor.setSelection(cursor.data.size());
    params = mReaderPrivate->extractVCard30PropertyParams(cursor, codec);
    QCOMPARE(params.count(), 3);
    typeParams = params.values(QString::fromAscii("TIPE"));
    QCOMPARE(params.values(QString::fromAscii("TIPE")).count(), 2);
    QVERIFY(params.values(QString::fromAscii("TIPE")).contains(QString::fromAscii("HOME")));
    QVERIFY(params.values(QString::fromAscii("TIPE")).contains(QString::fromAscii("VOICE")));
    encodingParams = params.values(QString::fromAscii("CHARSET"));
    QCOMPARE(1, encodingParams.count());
    QCOMPARE(encodingParams[0],QString::fromAscii("UTF-16"));
}

void tst_QVersitReader::testExtractParams()
{
    VersitCursor cursor;
    QByteArray data = ":123";
    cursor.setData(data);
    cursor.setPosition(0);
    cursor.setSelection(cursor.data.size());
    QList<QByteArray> params = mReaderPrivate->extractParams(cursor, mAsciiCodec);
    QCOMPARE(params.size(), 0);
    QCOMPARE(cursor.position, 1);

    data = "a;b:123";
    cursor.setData(data);
    cursor.setPosition(0);
    cursor.setSelection(cursor.data.size());
    params = mReaderPrivate->extractParams(cursor, mAsciiCodec);
    QCOMPARE(params.size(), 2);
    QCOMPARE(cursor.position, 4);
    QCOMPARE(params.at(0), QByteArray("a"));
    QCOMPARE(params.at(1), QByteArray("b"));

    QTextCodec* codec = QTextCodec::codecForName("UTF-16BE");
    data = VersitUtils::encode(":123", codec);
    cursor.setData(data);
    cursor.setPosition(0);
    cursor.setSelection(cursor.data.size());
    params = mReaderPrivate->extractParams(cursor, codec);
    QCOMPARE(params.size(), 0);
    QCOMPARE(cursor.position, 2);

    data = VersitUtils::encode("a;b:123", codec);
    cursor.setData(data);
    cursor.setPosition(0);
    cursor.setSelection(cursor.data.size());
    params = mReaderPrivate->extractParams(cursor, codec);
    QCOMPARE(params.size(), 2);
    QCOMPARE(cursor.position, 8);

}

Q_DECLARE_METATYPE(QList<QString>)

void tst_QVersitReader::testReadLine()
{
    QFETCH(QByteArray, codecName);
    QFETCH(QString, data);
    QFETCH(QList<QString>, expectedLines);

    QTextCodec* codec = QTextCodec::codecForName(codecName);
    QTextEncoder* encoder = codec->makeEncoder();
    encoder->fromUnicode(QString());

    QByteArray bytes(encoder->fromUnicode(data));

    mInputDevice->close();
    mInputDevice->setData(bytes);
    mInputDevice->open(QIODevice::ReadWrite);

    LineReader lineReader(mInputDevice, codec, 10);

    // Check that all expected lines are read.
    foreach (QString expectedLine, expectedLines) {
        QByteArray expectedBytes(encoder->fromUnicode(expectedLine));
        QVERIFY(!lineReader.atEnd());
        VersitCursor line = lineReader.readLine();
        QVERIFY(line.data.indexOf(expectedBytes) == line.position);
        QCOMPARE(line.selection - line.position, expectedBytes.length());
    }
    // And that there are no more lines
    VersitCursor line = lineReader.readLine();
    QCOMPARE(line.selection, line.position);
    QVERIFY(lineReader.atEnd());

    delete encoder;
}

void tst_QVersitReader::testReadLine_data()
{
    // Note: for this test, we set mLineReader to read 10 bytes at a time.  Lines of multiples of
    // 10 bytes are hence border cases.
    QTest::addColumn<QByteArray>("codecName");
    QTest::addColumn<QString>("data");
    QTest::addColumn<QList<QString> >("expectedLines");

    QList<QByteArray> codecNames;
    codecNames << "UTF-8" << "UTF-16";

    foreach (QByteArray codecName, codecNames) {
        QTest::newRow("empty " + codecName)
                << codecName
                << ""
                << QList<QString>();

        QTest::newRow("one line " + codecName)
                << codecName
                << "line"
                << (QList<QString>() << QLatin1String("line"));

        QTest::newRow("one ten-byte line " + codecName)
                << codecName
                << "tenletters"
                << (QList<QString>() << QLatin1String("tenletters"));

        QTest::newRow("one long line " + codecName)
                << codecName
                << "one line longer than ten characters"
                << (QList<QString>() << QLatin1String("one line longer than ten characters"));

        QTest::newRow("one terminated line " + codecName)
                << codecName
                << "one line longer than ten characters\r\n"
                << (QList<QString>() << QLatin1String("one line longer than ten characters"));

        QTest::newRow("two lines " + codecName)
                << codecName
                << "two\r\nlines"
                << (QList<QString>() << QLatin1String("two") << QLatin1String("lines"));

        QTest::newRow("two terminated lines " + codecName)
                << codecName
                << "two\r\nlines\r\n"
                << (QList<QString>() << QLatin1String("two") << QLatin1String("lines"));

        QTest::newRow("two long lines " + codecName)
                << codecName
                << "one line longer than ten characters\r\nanother line\r\n"
                << (QList<QString>() << QLatin1String("one line longer than ten characters") << QLatin1String("another line"));

        QTest::newRow("two full lines " + codecName)
                << codecName
                << "tenletters\r\n8letters\r\n"
                << (QList<QString>() << QLatin1String("tenletters") << QLatin1String("8letters"));

        QTest::newRow("a nine-byte line " + codecName)
                << codecName
                << "9 letters\r\nanother line\r\n"
                << (QList<QString>() << QLatin1String("9 letters") << QLatin1String("another line"));

        QTest::newRow("a blank line " + codecName)
                << codecName
                << "one\r\n\r\ntwo\r\n"
                << (QList<QString>() << QLatin1String("one") << QLatin1String("two"));

        QTest::newRow("folded lines " + codecName)
                << codecName
                << "folded\r\n  line\r\nsecond line\r\n"
                << (QList<QString>() << QLatin1String("folded line") << QLatin1String("second line"));

        QTest::newRow("multiply folded lines " + codecName)
                << codecName
                << "fo\r\n lded\r\n  line\r\nseco\r\n\tnd l\r\n ine\r\n"
                << (QList<QString>() << QLatin1String("folded line") << QLatin1String("second line"));

        QTest::newRow("fold hidden after a chunk " + codecName)
                << codecName
                << "8letters\r\n  on one line\r\n"
                << (QList<QString>() << QLatin1String("8letters on one line"));

        QTest::newRow("three mac lines " + codecName)
                << codecName
                << "one\rtwo\rthree\r"
                << (QList<QString>() << QLatin1String("one") << QLatin1String("two") << QLatin1String("three"));
    }
}

void tst_QVersitReader::testByteArrayInput()
{
    delete mReader;
    const QByteArray& oneDocument =
        "BEGIN:VCARD\r\nVERSION:2.1\r\nFN:John\r\nEND:VCARD\r\n";

    mReader = new QVersitReader(oneDocument);
    QVERIFY(mReader->device() == 0);
    QVERIFY(mReader->startReading());
    QVERIFY(mReader->waitForFinished());
    QList<QVersitDocument> results = mReader->results();
    QCOMPARE(mReader->state(), QVersitReader::FinishedState);
    QCOMPARE(mReader->error(), QVersitReader::NoError);
    QCOMPARE(results.count(),1);
    QVersitDocument result = results.first();
    QCOMPARE(result.type(), QVersitDocument::VCard21Type);
    QList<QVersitProperty> properties = result.properties();
    QCOMPARE(properties.length(), 1);
    QCOMPARE(properties.first().name(), QLatin1String("FN"));
    QCOMPARE(properties.first().value(), QLatin1String("John"));
}

void tst_QVersitReader::testRemoveBackSlashEscaping()
{
    // Empty string
    QString input;
    QVersitReaderPrivate::removeBackSlashEscaping(input);
    QCOMPARE(input,QString());

    // Nothing to escape in the string
    input = QString::fromAscii("Nothing to escape");
    QVersitReaderPrivate::removeBackSlashEscaping(input);
    QCOMPARE(input,QString::fromAscii("Nothing to escape"));

    // Line break, semicolon, backslash and comma in the string
    input = QString::fromAscii("These should be unescaped \\n \\N \\; \\, \\\\");
    QVersitReaderPrivate::removeBackSlashEscaping(input);
    QCOMPARE(input, QString::fromAscii("These should be unescaped \r\n \r\n ; , \\"));

    // Don't remove escaping within quotes
    input = QString::fromAscii("\"Quoted \\n \\N \\; \\,\"");
    QVersitReaderPrivate::removeBackSlashEscaping(input);
    QCOMPARE(input, QString::fromAscii("\"Quoted \\n \\N \\; \\,\""));
}

QTEST_MAIN(tst_QVersitReader)

