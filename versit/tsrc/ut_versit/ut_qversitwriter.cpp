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

#include "ut_qversitwriter.h"
#include "qversitwriter.h"
#include "qvcard21writer.h"
#include "qversitdocument.h"
#include "qversitproperty.h"
#include <QtTest/QtTest>
#include <QByteArray>

void UT_QVersitWriter::init()
{
    mWriter = new QVersitWriter;
    mWriterPrivate = new QVCard21Writer;
}

void UT_QVersitWriter::cleanup()
{
    delete mWriterPrivate;
    delete mWriter;
}

void UT_QVersitWriter::testVersitDocument()
{
    QVersitDocument doc;
    mWriter->setVersitDocument(doc);
    QCOMPARE(doc.versitType(), QVersitDocument::VCard21);
}

void UT_QVersitWriter::testDevice()
{
    // No device
    QVERIFY(mWriter->device() == NULL);    
    
    // Device has been set
    QBuffer device;
    mWriter->setDevice(&device);
    QVERIFY(mWriter->device() == &device);
}

void UT_QVersitWriter::testStart()
{
    QVERIFY(!mWriter->start());
    
    //device set, document not set
    QBuffer buffer;
    mWriter->setDevice(&buffer);
    buffer.open(QBuffer::ReadWrite);
    QVERIFY(!mWriter->start());

    //document set, device not set
    mWriter->setDevice(0);
    QVersitDocument doc;
    QVersitProperty prop;
    prop.setName(QString("FN"));
    prop.setValue("John Smith");
    doc.addProperty(prop);
    doc.addProperty(prop);
    mWriter->setVersitDocument(doc);
    QVERIFY(!mWriter->start());

    //both document and device are set
    mWriter->setDevice(&buffer);
    buffer.open(QBuffer::ReadWrite);
    QVERIFY(mWriter->start());
}

void UT_QVersitWriter::testEncodeVersitDocument()
{
    const char vCardSimple[] = 
"BEGIN:VCARD\r\n\
VERSION:2.1\r\n\
N:Homer\r\n\
TEL:12347\r\n\
END:VCARD\r\n";
    QVersitDocument doc;
    doc.setVersitType(QVersitDocument::VCard21);
    QVersitProperty property;
    property.setName(QString("N"));
    property.setValue(QByteArray("Homer"));
    doc.addProperty(property);
    property.setName(QString("TEL"));
    property.setValue(QByteArray("12347"));
    doc.addProperty(property);
    QVERIFY(mWriterPrivate->encodeVersitDocument(doc) == vCardSimple);

    const char vCardAgent[] = 
"BEGIN:VCARD\r\n\
VERSION:2.1\r\n\
N:Homer\r\n\
EMAIL;ENCODING=QUOTED-PRINTABLE:homer=40simpsons.com\r\n\
AGENT:\r\nBEGIN:VCARD\r\nVERSION:2.1\r\nN:Marge\r\nEND:VCARD\r\n\r\n\
END:VCARD\r\n";
    QVersitDocument docAgent;
    docAgent.setVersitType(QVersitDocument::VCard21);
    property.setName(QString::fromAscii("N"));
    property.setValue(QByteArray("Homer"));
    docAgent.addProperty(property);
    property.setName(QString::fromAscii("EMAIL"));
    property.setValue(QByteArray("homer@simpsons.com"));
    docAgent.addProperty(property);
    property.setName(QString("AGENT"));
    property.setValue(QByteArray());
    QVersitDocument embeddedDocument;
    QVersitProperty embeddedProperty;
    embeddedProperty.setName(QString::fromAscii("N"));
    embeddedProperty.setValue(QByteArray("Marge"));
    embeddedDocument.addProperty(embeddedProperty);
    property.setEmbeddedDocument(embeddedDocument);
    docAgent.addProperty(property);
    QCOMPARE(
        QString::fromAscii(mWriterPrivate->encodeVersitDocument(docAgent)),
        QString::fromAscii(vCardAgent));
}

void UT_QVersitWriter::testEncodeParameters()
{
    QString encodingParameterName(QString::fromAscii("ENCODING"));
    
    // No parameters
    QMultiHash<QString,QString> parameters;
    QCOMPARE(mWriterPrivate->encodeParameters(parameters), QByteArray());
    
    // One parameter
    parameters.insert(encodingParameterName,QString::fromAscii("8BIT"));
    QCOMPARE(mWriterPrivate->encodeParameters(parameters), QByteArray(";ENCODING=8BIT"));   
    
    // Two parameters
    parameters.clear();
    parameters.insert(QString::fromAscii("X-PARAM"),QString::fromAscii("value"));
    parameters.insert(encodingParameterName,QString::fromAscii("7BIT"));    
    QCOMPARE(mWriterPrivate->encodeParameters(parameters), QByteArray(";X-PARAM=value;ENCODING=7BIT"));
}
