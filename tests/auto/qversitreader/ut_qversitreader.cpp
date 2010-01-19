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

#include "ut_qversitreader.h"
#include "qversitreader.h"
#include "qversitreader_p.h"
#include "versitutils_p.h"
#include <QtTest/QtTest>


// This says "NOKIA" in Katakana
const QByteArray KATAKANA_NOKIA("\xe3\x83\x8e\xe3\x82\xad\xe3\x82\xa2");

QTM_USE_NAMESPACE

void UT_QVersitReader::init()
{
    mReadingDoneCalled = false;
    mExpectedDocumentCount = 0;
    mInputDevice = new QBuffer;
    mInputDevice->open(QBuffer::ReadWrite);
    mReader = new QVersitReader;
    connect(mReader,SIGNAL(finished()),this,SLOT(finished()),Qt::DirectConnection);
    mReaderPrivate = new QVersitReaderPrivate;
}

void UT_QVersitReader::cleanup()
{   
    delete mReaderPrivate;
    delete mReader;
    delete mInputDevice;
}

void UT_QVersitReader::finished()
{
    mReadingDoneCalled = true;
    QCOMPARE(mReader->state(), QVersitReader::FinishedState);
    QCOMPARE(mReader->error(), mExpectedError);
    QCOMPARE(mReader->results().count(),mExpectedDocumentCount);
}

void UT_QVersitReader::testDevice()
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
}

void UT_QVersitReader::testReading()
{
    // No I/O device set
    QCOMPARE(mReader->readAll(), QList<QVersitDocument>());
    QCOMPARE(mReader->error(), QVersitReader::IOError);
    
    // Device set, no data
    mReader->setDevice(mInputDevice);
    mInputDevice->open(QBuffer::ReadWrite);
    QList<QVersitDocument> results(mReader->readAll());
    QCOMPARE(mReader->state(), QVersitReader::FinishedState);
    QCOMPARE(mReader->error(), QVersitReader::NoError);
    QCOMPARE(results.count(),0);

    // Device set, one document
    const QByteArray& oneDocument = 
        "BEGIN:VCARD\r\nVERSION:2.1\r\nFN:John\r\nEND:VCARD\r\n";
    mInputDevice->write(oneDocument);
    mInputDevice->seek(0);
    results = mReader->readAll();
    QCOMPARE(mReader->state(), QVersitReader::FinishedState);
    QCOMPARE(mReader->error(), QVersitReader::NoError);
    QCOMPARE(results.count(),1);

    // Wide charset with no byte-order mark
    QTextCodec* codec = QTextCodec::codecForName("UTF-16BE");
    const QByteArray& wideDocument =
            VersitUtils::encode("BEGIN:VCARD\r\nVERSION:2.1\r\nFN:John\r\nEND:VCARD\r\n", codec);
    delete mInputDevice;
    mInputDevice = new QBuffer;
    mInputDevice->open(QBuffer::ReadWrite);
    mInputDevice->write(wideDocument);
    mInputDevice->seek(0);
    mReader->setDevice(mInputDevice);
    mReader->setDefaultCodec(codec);
    results = mReader->readAll();
    QCOMPARE(mReader->state(), QVersitReader::FinishedState);
    QCOMPARE(mReader->error(), QVersitReader::NoError);
    QCOMPARE(mReader->results().count(),1);
    mReader->setDefaultCodec(NULL);

    // Two documents
    const QByteArray& twoDocuments =
        " \r\n BEGIN:VCARD\r\nFN:Jenny\r\nEND:VCARD\r\nBEGIN:VCARD\r\nFN:Jake\r\nEND:VCARD\r\n";
    delete mInputDevice;
    mInputDevice = 0;
    mInputDevice = new QBuffer;
    mInputDevice->open(QBuffer::ReadWrite);
    mInputDevice->write(twoDocuments);
    mInputDevice->seek(0);
    mReader->setDevice(mInputDevice);
    results = mReader->readAll();
    QCOMPARE(mReader->state(), QVersitReader::FinishedState);
    QCOMPARE(mReader->error(), QVersitReader::NoError);
    QCOMPARE(results.count(),2);

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
    delete mInputDevice;
    mInputDevice = 0;
    mInputDevice = new QBuffer;
    mInputDevice->open(QBuffer::ReadWrite);
    mInputDevice->write(validDocumentsAndGroupedDocument);
    mInputDevice->seek(0);
    mReader->setDevice(mInputDevice);
    results = mReader->readAll();
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
    delete mInputDevice;
    mInputDevice = 0;
    mInputDevice = new QBuffer;
    mInputDevice->open(QBuffer::ReadWrite);
    mInputDevice->write(validDocumentsAndGroupedDocument2);
    mInputDevice->seek(0);
    mReader->setDevice(mInputDevice);
    results = mReader->readAll();
    QCOMPARE(mReader->state(), QVersitReader::FinishedState);
    // An error is logged because one failed, but the rest are readable.
    QCOMPARE(mReader->error(), QVersitReader::ParseError);
    QCOMPARE(mReader->results().count(),4);


    // Asynchronous reading
    QVERIFY(!mReadingDoneCalled);
    delete mInputDevice;
    mInputDevice = 0;
    mInputDevice = new QBuffer;
    mInputDevice->open(QBuffer::ReadWrite);
    mInputDevice->write(oneDocument);
    mInputDevice->seek(0);
    mReader->setDevice(mInputDevice);
    mExpectedDocumentCount = 1;
    mExpectedError = QVersitReader::NoError;
    QVERIFY(mReader->startReading());
    delete mReader; // waits for the thread to finish
    mReader = 0;
    QVERIFY(mReadingDoneCalled);
}

void UT_QVersitReader::testResult()
{
    QCOMPARE(mReader->results().count(),0);
}

void UT_QVersitReader::testSetVersionFromProperty()
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
    property.addParameter(QString::fromAscii("ENCODING"),QString::fromAscii("BASE64"));
    QVERIFY(mReaderPrivate->setVersionFromProperty(document,property));
    QVERIFY(document.type() == QVersitDocument::VCard21Type);
    
    // VERSION property with BASE64 encoded not supported value
    property.setValue(QString::fromAscii(QByteArray("4.0").toBase64()));
    QVERIFY(!mReaderPrivate->setVersionFromProperty(document,property));
}

void UT_QVersitReader::testParseNextVersitPropertyVCard21()
{
    QVersitDocument::VersitType type = QVersitDocument::VCard21Type;

    // Test a valid vCard 2.1 with properties having separate handling:
    // AGENT property, ENCODING parameters (BASE64 and QUOTED-PRINTABLE) and CHARSET parameter
    QTextCodec* codec = QTextCodec::codecForName("UTF-16BE");
    QByteArray vCard("Begin:vcard\r\n");
    vCard.append("VERSION:2.1\r\n");
    vCard.append("FN:John\r\n");
    vCard.append("ORG;CHARSET=UTF-8:");
    vCard.append(KATAKANA_NOKIA);
    vCard.append("\r\n");
    // "NOKIA" in Katakana, UTF-8 encoded, then base-64 encoded:
    vCard.append("NOTE;ENCODING=BASE64;CHARSET=UTF-8:");
    vCard.append(KATAKANA_NOKIA.toBase64());
    vCard.append("\r\n");
    // The value here is "UXQgaXMgZ3JlYXQh", which is the base64 encoding of "Qt is great!".
    vCard.append("PHOTO;ENCODING=BASE64: U\t XQgaX MgZ\t3Jl YXQh\r\n\r\n");
    vCard.append("HOME.Springfield.EMAIL;Encoding=Quoted-Printable:john.citizen=40exam=\r\nple.com\r\n");
    vCard.append("EMAIL;ENCODING=QUOTED-PRINTABLE;CHARSET=UTF-16BE:");
    vCard.append(codec->fromUnicode(QLatin1String("john.citizen=40exam=\r\nple.com")));
    vCard.append("\r\n");
    vCard.append("AGENT:\r\nBEGIN:VCARD\r\nFN:Jenny\r\nEND:VCARD\r\n\r\n");
    vCard.append("End:VCARD\r\n");

    VersitCursor cursor(vCard);

    QVersitProperty property = mReaderPrivate->parseNextVersitProperty(type, cursor);
    QCOMPARE(property.name(),QString::fromAscii("BEGIN"));
    QCOMPARE(property.value(),QString::fromAscii("vcard"));
    QCOMPARE(cursor.position, 11); // pointing to the  \r\n
    
    property = mReaderPrivate->parseNextVersitProperty(type, cursor);
    QCOMPARE(property.name(),QString::fromAscii("VERSION"));
    QCOMPARE(property.value(),QString::fromAscii("2.1"));
    QCOMPARE(cursor.position, 24);

    property = mReaderPrivate->parseNextVersitProperty(type, cursor);
    QCOMPARE(property.name(),QString::fromAscii("FN"));
    QCOMPARE(property.value(),QString::fromAscii("John"));

    property = mReaderPrivate->parseNextVersitProperty(type, cursor);
    QCOMPARE(property.name(),QString::fromAscii("ORG"));
    QCOMPARE(property.value(),QString::fromUtf8(KATAKANA_NOKIA));

    property = mReaderPrivate->parseNextVersitProperty(type, cursor);
    QCOMPARE(property.name(),QString::fromAscii("NOTE"));
    QCOMPARE(property.value(),QString::fromUtf8(KATAKANA_NOKIA));
    
    property = mReaderPrivate->parseNextVersitProperty(type, cursor);
    QCOMPARE(property.name(),QString::fromAscii("PHOTO"));
    // Linear whitespaces (SPACEs and TABs) removed from the value and base64 decoded:
    QCOMPARE(property.variantValue().type(), QVariant::ByteArray);
    QCOMPARE(property.value<QByteArray>(), QByteArray("Qt is great!"));
    // Ensure that base-64 encoded strings can be retrieved as strings.
    QCOMPARE(property.value(), QLatin1String("Qt is great!"));

    property = mReaderPrivate->parseNextVersitProperty(type, cursor);
    QStringList propertyGroup(QString::fromAscii("HOME"));
    propertyGroup.append(QString::fromAscii("Springfield"));
    QCOMPARE(property.groups(),propertyGroup);
    QCOMPARE(property.name(),QString::fromAscii("EMAIL"));
    QCOMPARE(0,property.parameters().count());
    QCOMPARE(property.value(),QString::fromAscii("john.citizen@example.com"));

    property = mReaderPrivate->parseNextVersitProperty(type, cursor);
    QCOMPARE(property.name(),QString::fromAscii("EMAIL"));
    // The base64 parameter should be stripped by the reader.
    QCOMPARE(property.parameters().count(), 0);
    QCOMPARE(property.value(),QString::fromAscii("john.citizen@example.com"));

    property = mReaderPrivate->parseNextVersitProperty(type, cursor);
    QCOMPARE(property.name(),QString::fromAscii("AGENT"));
    QCOMPARE(property.value(),QString());
    QVERIFY(property.variantValue().userType() == qMetaTypeId<QVersitDocument>());
    QCOMPARE(property.value<QVersitDocument>().properties().count(), 1);
    
    property = mReaderPrivate->parseNextVersitProperty(type, cursor);
    QCOMPARE(property.name(),QString::fromAscii("END"));
    QCOMPARE(property.value(),QString::fromAscii("VCARD"));
    
    property = mReaderPrivate->parseNextVersitProperty(type, cursor);
    QCOMPARE(property.name(),QString());
    QCOMPARE(property.value(),QString());
    
    // Simulate a situation where the document nesting level is exceeded
    // In practice this would mean a big number of nested AGENT properties
    mReaderPrivate->mDocumentNestingLevel = 20;
    QByteArray agentProperty("AGENT:BEGIN:VCARD\r\nN:Jenny\r\nEND:VCARD\r\n\r\n");
    VersitCursor agentCursor(agentProperty);

    property = mReaderPrivate->parseNextVersitProperty(type, agentCursor);
    QVERIFY(property.isEmpty());
}

void UT_QVersitReader::testParseNextVersitPropertyVCard30()
{
    QVersitDocument::VersitType type = QVersitDocument::VCard30Type;

    // Test a valid vCard 3.0 with properties having separate handling:
    // AGENT property and some other property
    QByteArray vCard("Begin:vcard\r\n");
    vCard.append("VERSION:3.0\r\n");
    vCard.append("FN:John\r\n");
    vCard.append("ORG;CHARSET=UTF-8:");
    vCard.append(KATAKANA_NOKIA);
    vCard.append("\r\n");
    // "NOKIA" in Katakana, UTF-8 encoded, then base-64 encoded:
    vCard.append("NOTE;ENCODING=BASE64;CHARSET=UTF-8:");
    vCard.append(KATAKANA_NOKIA.toBase64());
    vCard.append("\r\n");
    vCard.append("TEL;TYPE=PREF;HOME:123\r\n");
    // The value here is "UXQgaXMgZ3JlYXQh", which is the base64 encoding of "Qt is great!".
    vCard.append("PHOTO;ENCODING=B:UXQgaXMgZ3JlYXQh\r\n");
    vCard.append("EMAIL:john.citizen@example.com\r\n");
    vCard.append("AGENT:BEGIN:VCARD\\nFN:Jenny\\nEND:VCARD\\n\r\n");
    vCard.append("End:VCARD\r\n");

    VersitCursor cursor(vCard);

    QVersitProperty property = mReaderPrivate->parseNextVersitProperty(type, cursor);
    QCOMPARE(property.name(),QString::fromAscii("BEGIN"));
    QCOMPARE(property.value(),QString::fromAscii("vcard"));

    property = mReaderPrivate->parseNextVersitProperty(type, cursor);
    QCOMPARE(property.name(),QString::fromAscii("VERSION"));
    QCOMPARE(property.value(),QString::fromAscii("3.0"));

    property = mReaderPrivate->parseNextVersitProperty(type, cursor);
    QCOMPARE(property.name(),QString::fromAscii("FN"));
    QCOMPARE(property.value(),QString::fromAscii("John"));

    property = mReaderPrivate->parseNextVersitProperty(type, cursor);
    QCOMPARE(property.name(),QString::fromAscii("ORG"));
    QCOMPARE(property.value(),QString::fromUtf8(KATAKANA_NOKIA));

    property = mReaderPrivate->parseNextVersitProperty(type, cursor);
    QCOMPARE(property.name(),QString::fromAscii("NOTE"));
    QCOMPARE(property.value(),QString::fromUtf8(KATAKANA_NOKIA));

    property = mReaderPrivate->parseNextVersitProperty(type, cursor);
    QCOMPARE(property.name(),QString::fromAscii("TEL"));
    QCOMPARE(property.value(),QString::fromAscii("123"));
    QCOMPARE(property.parameters().count(), 2);

    property = mReaderPrivate->parseNextVersitProperty(type, cursor);
    QCOMPARE(property.name(),QString::fromAscii("PHOTO"));
    QCOMPARE(property.variantValue().type(), QVariant::ByteArray);
    QCOMPARE(property.value<QByteArray>(), QByteArray("Qt is great!"));
    // Ensure that base-64 encoded strings can be retrieved as strings.
    QCOMPARE(property.value(), QLatin1String("Qt is great!"));

    property = mReaderPrivate->parseNextVersitProperty(type, cursor);
    QCOMPARE(property.name(),QString::fromAscii("EMAIL"));
    QCOMPARE(0,property.parameters().count());
    QCOMPARE(property.value(),QString::fromAscii("john.citizen@example.com"));

    property = mReaderPrivate->parseNextVersitProperty(type, cursor);
    QCOMPARE(property.name(),QString::fromAscii("AGENT"));
    QVERIFY(property.variantValue().userType() == qMetaTypeId<QVersitDocument>());
    QCOMPARE(property.value<QVersitDocument>().properties().count(), 1);

    property = mReaderPrivate->parseNextVersitProperty(type, cursor);
    QCOMPARE(property.name(),QString::fromAscii("END"));
    QCOMPARE(property.value(),QString::fromAscii("VCARD"));

    property = mReaderPrivate->parseNextVersitProperty(type, cursor);
    QCOMPARE(property.name(),QString());
    QCOMPARE(property.value(),QString());

    // Simulate a situation where the document nesting level is exceeded
    // In practice this would mean a big number of nested AGENT properties
    mReaderPrivate->mDocumentNestingLevel = 20;
    QByteArray agentProperty("AGENT:BEGIN\\:VCARD\\nFN\\:Jenny\\nEND\\:VCARD\\n\r\n");

    VersitCursor agentCursor(agentProperty);
    property = mReaderPrivate->parseNextVersitProperty(type, agentCursor);
    QVERIFY(property.isEmpty());
}

void UT_QVersitReader::testParseVersitDocument()
{
    // Valid vCard 2.1
    const char validCard21[] =
"BEGIN:VCARD\r\n\
VERSION:2.1\r\n\
FN:John\r\n\
AGENT:BEGIN:VCARD\r\nN:Jenny\r\nEND:VCARD\r\n\r\n\
EMAIL;ENCODING=QUOTED-PRINTABLE:john.citizen=40exam=\r\nple.com\r\n\
END:VCARD\r\n";
    QByteArray vCard(validCard21);
    VersitCursor cursor(vCard);
    QVersitDocument document;
    QVERIFY(mReaderPrivate->parseVersitDocument(cursor, document));
    QCOMPARE(document.properties().count(),3);
    QCOMPARE(mReaderPrivate->mDocumentNestingLevel,0);

    // Valid vCard 3.0
    const char validCard30[] =
"BEGIN:VCARD\r\n\
VERSION:3.0\r\n\
FN:John\r\n\
AGENT:BEGIN\\:VCARD\\nN\\:Jenny\\nEND\\:VCARD\\n\r\n\
EMAIL:john.citizen@example.com\r\n\
END:VCARD\r\n";
    vCard = validCard30;
    cursor.setData(vCard);
    document = QVersitDocument();
    QVERIFY(mReaderPrivate->parseVersitDocument(cursor, document));
    QCOMPARE(document.properties().count(),3);
    QCOMPARE(mReaderPrivate->mDocumentNestingLevel,0);
    
    // No BEGIN found
    const char beginMissing[] = 
"VCARD\r\n\
VERSION:2.1\r\n\
FN:Nobody\r\n\
END:VCARD\r\n";
    vCard = beginMissing;
    cursor.setData(vCard);
    document = QVersitDocument();
    QVERIFY(!mReaderPrivate->parseVersitDocument(cursor, document));
    QCOMPARE(document.properties().count(),0);
    QCOMPARE(mReaderPrivate->mDocumentNestingLevel,0);
    
    // Wrong card type
    const char wrongType[] = 
"BEGIN:VCAL\r\n\
END:VCAL\r\n";
    vCard = wrongType;
    cursor.setData(vCard);
    document = QVersitDocument();
    QVERIFY(!mReaderPrivate->parseVersitDocument(cursor, document));
    QCOMPARE(document.properties().count(),0);
    QCOMPARE(mReaderPrivate->mDocumentNestingLevel,0);
    
    // Wrong version
    const char wrongVersion[] = 
"BEGIN:VCARD\r\n\
VERSION:4.0\r\n\
FN:Nobody\r\n\
END:VCARD\r\n";
    vCard = wrongVersion;
    cursor.setData(vCard);
    document = QVersitDocument();
    QVERIFY(!mReaderPrivate->parseVersitDocument(cursor, document));
    QCOMPARE(document.properties().count(),0);
    QCOMPARE(mReaderPrivate->mDocumentNestingLevel,0);

    // Grouped vCards are not supported. The whole vCard will be discarded.
    const char groupedCard[] =
"BEGIN:VCARD\r\n\
VERSION:2.1\r\n\
X-EXAMPLES:Family vCard\r\n\
BEGIN:VCARD\r\n\
VERSION:2.1\r\n\
N:Citizen;John\r\n\
TEL;CELL:1111\r\n\
EMAIL;ENCODING=QUOTED-PRINTABLE:john.citizen=40example.com\r\n\
END:VCARD\r\n\
BEGIN:VCARD\r\n\
VERSION:2.1\r\n\
N:Citizen;Jenny\r\n\
TEL;CELL:7777\r\n\
END:VCARD\r\n\
END:VCARD";
    vCard = groupedCard;
    cursor.setData(vCard);
    document = QVersitDocument();
    QVERIFY(!mReaderPrivate->parseVersitDocument(cursor, document));
    QCOMPARE(mReaderPrivate->mDocumentNestingLevel, 0);
    QCOMPARE(mReaderPrivate->mVersitDocuments.count(), 0);
}

QTEST_MAIN(UT_QVersitReader)

