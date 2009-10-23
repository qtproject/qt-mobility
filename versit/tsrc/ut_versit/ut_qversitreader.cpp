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
#include <QtTest/QtTest>
#include <QBuffer>

void UT_QVersitReader::init()
{
    mReader = new QVersitReader;
    mReaderPrivate = new QVersitReaderPrivate;
}

void UT_QVersitReader::cleanup()
{   
    delete mReaderPrivate;
    delete mReader;
}

void UT_QVersitReader::testDevice()
{
    // No device
    QVERIFY(mReader->device() == NULL);    
    
    // Device has been set
    QBuffer device;
    mReader->setDevice(&device);
    QVERIFY(mReader->device() == &device);
}

void UT_QVersitReader::testStart()
{
    // No I/O device set
    QVERIFY(!mReader->start());
    
    // Device set, not open
    QBuffer buffer;
    mReader->setDevice(&buffer);
    QVERIFY(!mReader->start());
    
    // Device set, opened, no data
    buffer.open(QBuffer::ReadWrite);
    QVERIFY(!mReader->start());
    
    // Device ready, invalid data
    const QByteArray& invalidData = 
        "BEGIN:VCARD\r\nVERSION:2.1\r\nEND:VCARD\r\n";
    buffer.write(invalidData);
    buffer.seek(0);
    QVERIFY(!mReader->start());

    // Device set, one document
    const QByteArray& oneDocument = 
        "Begin:VCARD\r\nVERSION:2.1\r\nN:Homer\r\nenD:VCARD\r\n";
    buffer.write(oneDocument);
    buffer.seek(0);
    QVERIFY(mReader->start());
    QCOMPARE(mReader->result().count(),1);
    
    // Two documents
    const QByteArray& twoDocuments = 
        "begin:VCARD\r\nN:Marge\r\nEND:VCARD\r\nBEGIN:VCARD\r\nN:Bart\r\nend:VCARD\r\n";
    buffer.reset();
    buffer.write(twoDocuments);
    buffer.seek(0);
    QVERIFY(mReader->start());
    QCOMPARE(mReader->result().count(),3);
}

void UT_QVersitReader::testResult()
{
    QCOMPARE(mReader->result().count(),0);
}

void UT_QVersitReader::testContainsSupportedVersion()
{
    // Some other property than VERSION
    QVersitProperty property;
    property.setName(QString::fromAscii("N"));
    QVERIFY(mReaderPrivate->containsSupportedVersion(property));
    
    // VERSION property with a supported value
    property.setName(QString::fromAscii("VERSION"));
    property.setValue(QByteArray("2.1"));
    QVERIFY(mReaderPrivate->containsSupportedVersion(property));    
    
    // VERSION property with a not supported value
    property.setValue(QByteArray("3.0"));
    QVERIFY(!mReaderPrivate->containsSupportedVersion(property));     
    
    // VERSION property with BASE64 encoded supported value
    property.setValue(QByteArray("2.1").toBase64());
    property.addParameter(QString::fromAscii("ENCODING"),QString::fromAscii("BASE64"));
    QVERIFY(mReaderPrivate->containsSupportedVersion(property));    
    
    // VERSION property with BASE64 encoded not supported value
    property.setValue(QByteArray("3.0").toBase64());
    QVERIFY(!mReaderPrivate->containsSupportedVersion(property));     
}

void UT_QVersitReader::testParseNextVersitProperty()
{
    // Test a valid card with properties having separate handling: 
    // AGENT property, some property with parameter ENCODING=QUOTED-PRINTABLE 
    // and some other property without this parameter
    QByteArray vCard("Begin:vcard\r\n");
    vCard.append("VERSION:2.1\r\n");
    vCard.append("N:Homer\r\n");
    vCard.append("PHOTO;ENCODING=BASE64: U XQgaX MgZ 3Jl YXQh\r\n\r\n");
    vCard.append("EMAIL;Encoding=Quoted-Printable:homer=40simp=\r\nsons.com\r\n");
    vCard.append("AGENT:\r\nBEGIN:VCARD\r\nN:Marge\r\nEND:VCARD\r\n\r\n");
    vCard.append("End:VCARD\r\n");
    
    QVersitProperty property = mReaderPrivate->parseNextVersitProperty(vCard);
    QCOMPARE(property.name(),QString::fromAscii("BEGIN"));
    QCOMPARE(property.value(),QByteArray("vcard"));
    
    property = mReaderPrivate->parseNextVersitProperty(vCard);
    QCOMPARE(property.name(),QString::fromAscii("VERSION"));
    QCOMPARE(property.value(),QByteArray("2.1"));
    
    property = mReaderPrivate->parseNextVersitProperty(vCard);
    QCOMPARE(property.name(),QString::fromAscii("N"));
    QCOMPARE(property.value(),QByteArray("Homer"));
    
    property = mReaderPrivate->parseNextVersitProperty(vCard);
    QCOMPARE(property.name(),QString::fromAscii("PHOTO"));
    QCOMPARE(1,property.parameters().count());
    QCOMPARE(property.value(),QByteArray("UXQgaXMgZ3JlYXQh"));

    property = mReaderPrivate->parseNextVersitProperty(vCard);
    QCOMPARE(property.name(),QString::fromAscii("EMAIL"));
    QCOMPARE(0,property.parameters().count());
    QCOMPARE(property.value(),QByteArray("homer@simpsons.com"));
    
    property = mReaderPrivate->parseNextVersitProperty(vCard);
    QCOMPARE(property.name(),QString::fromAscii("AGENT"));
    QCOMPARE(property.value(),QByteArray());
    QCOMPARE(property.embeddedDocument().properties().count(),1);
    
    property = mReaderPrivate->parseNextVersitProperty(vCard);
    QCOMPARE(property.name(),QString::fromAscii("END"));
    QCOMPARE(property.value(),QByteArray("VCARD"));
    
    property = mReaderPrivate->parseNextVersitProperty(vCard);
    QCOMPARE(property.name(),QString());
    QCOMPARE(property.value(),QByteArray());
    
    // Simulate a situation where the document nesting level is exceeded
    // In practice this would mean a big number of nested AGENT properties
    mReaderPrivate->mDocumentNestingLevel = 20;
    QByteArray agentProperty("AGENT:BEGIN:VCARD\r\nN:Marge\r\nEND:VCARD\r\n\r\n");
    property = mReaderPrivate->parseNextVersitProperty(agentProperty);
    QCOMPARE(property.name(),QString::fromAscii("AGENT"));
    QCOMPARE(property.embeddedDocument().properties().count(),0);
    QCOMPARE(property.value(),QByteArray());
}

void UT_QVersitReader::testParseVersitDocument()
{
    // Valid card
    const char validCard[] = 
"BEGIN:VCARD\r\n\
VERSION:2.1\r\n\
N:Homer\r\n\
AGENT:BEGIN:VCARD\r\nN:Marge\r\nEND:VCARD\r\n\r\n\
EMAIL;ENCODING=QUOTED-PRINTABLE:homer=40simp=\r\nsons.com\r\n\
END:VCARD\r\n";
    QByteArray vCard(validCard);
    QVersitDocument document = mReaderPrivate->parseVersitDocument(vCard);
    QCOMPARE(document.properties().count(),3);
    QCOMPARE(mReaderPrivate->mDocumentNestingLevel,0);
    
    // No BEGIN found
    const char beginMissing[] = 
"VCARD\r\n\
VERSION:2.1\r\n\
N:Nobody\r\n\
END:VCARD\r\n";
    vCard = beginMissing;
    document = mReaderPrivate->parseVersitDocument(vCard);
    QCOMPARE(document.properties().count(),0);
    QCOMPARE(mReaderPrivate->mDocumentNestingLevel,0);
    
    // Wrong card type
    const char wrongType[] = 
"BEGIN:VCAL\r\n\
END:VCAL\r\n";
    vCard = wrongType;
    document = mReaderPrivate->parseVersitDocument(vCard);
    QCOMPARE(document.properties().count(),0);
    QCOMPARE(mReaderPrivate->mDocumentNestingLevel,0);
    
    // Wrong version
    const char wrongVersion[] = 
"BEGIN:VCARD\r\n\
VERSION:3.0\r\n\
N:Nobody\r\n\
END:VCARD\r\n";
    vCard = wrongVersion;
    document = mReaderPrivate->parseVersitDocument(vCard);
    QCOMPARE(document.properties().count(),0);
    QCOMPARE(mReaderPrivate->mDocumentNestingLevel,0);
}
