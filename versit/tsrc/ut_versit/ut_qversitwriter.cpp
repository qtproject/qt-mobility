/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "ut_qversitwriter.h"
#include "qversitwriter.h"
#include "qversitdocument.h"
#include "qversitproperty.h"
#include <QtTest/QtTest>
#include <QByteArray>

void UT_QVersitWriter::init()
{
    mWriter = new QVersitWriter;
}

void UT_QVersitWriter::cleanup()
{
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
    QVERIFY(mWriter->encodeVersitDocument(doc) == vCardSimple);
/*
    const char vCardAgent[] = 
"BEGIN:VCARD\r\n\
VERSION:2.1\r\n\
N:Homer\r\n\
EMAIL;ENCODING=QUOTED-PRINTABLE:homer=40simp=\r\nsons.com\r\n\
AGENT:\r\nBEGIN:VCARD\r\nN:Marge\r\nEND:VCARD\r\n\
END:VCARD\r\n";
    QVersitDocument docAgent;
    docAgent.setVersitType(QVersitDocument::VCard21);
    QVersitProperty propertyAgent;
    propertyAgent.setName(QString("N"));
    propertyAgent.setValue(QByteArray("Homer"));
    docAgent.addProperty(propertyAgent);
    propertyAgent.setName(QString("EMAIL"));
    propertyAgent.setValue(QByteArray("homer@simpsons.com"));
    propertyAgent.addParameter(QString("ENCODING"), 
                               QString("QUOTED-PRINTABLE"));
    docAgent.addProperty(propertyAgent);
    propertyAgent.setName(QString("AGENT"));
    QVersitDocument embeddedDocument;
    QVersitProperty embeddedProperty;
    embeddedProperty.setName(QString("Marge"));
    embeddedDocument.addProperty(embeddedProperty);
    propertyAgent.setEmbeddedDocument(embeddedDocument);
    docAgent.addProperty(propertyAgent);
    QVERIFY(mWriter->encodeVersitDocument(docAgent) == vCardAgent);
*/
}


void UT_QVersitWriter::testEncodeVersitProperty()
{
    // No parameters
    QByteArray expectedResult = "N:Simpson, Homer\r\n"; 
    QVersitProperty property;
    property.setName(QString::fromAscii("N"));
    property.setValue(QByteArray("Simpson, Homer"));
    QCOMPARE(mWriter->encodeVersitProperty(property), expectedResult);
    
    // With parameter(s). No special characters in the value.
    // -> No need to Quoted-Printable encode the value.
    expectedResult = "TEL;HOME:123\r\n"; 
    property.setName(QString::fromAscii("TEL"));
    property.setValue(QByteArray("123"));
    property.addParameter(QByteArray("TYPE"),QByteArray("HOME"));
    QCOMPARE(mWriter->encodeVersitProperty(property), expectedResult);
    
    // With parameter(s). Special characters in the value.
    // -> The value needs to be Quoted-Printable encoded.
    expectedResult = "EMAIL;HOME;ENCODING=QUOTED-PRINTABLE:homer=40simpsons.com\r\n"; 
    property.setName(QString::fromAscii("EMAIL"));
    property.setValue(QByteArray("homer@simpsons.com"));
    QCOMPARE(mWriter->encodeVersitProperty(property), expectedResult);
    
    // AGENT property with parameter
    expectedResult = 
"AGENT;X-WIFE:\r\n\
BEGIN:VCARD\r\n\
VERSION:2.1\r\n\
N:Marge\r\n\
END:VCARD\r\n\
\r\n";
    QVersitProperty agentProperty;
    agentProperty.setName(QString("AGENT"));
    QMultiMap<QString,QString> params;
    params.insert(QString("TYPE"), QString("X-WIFE"));
    agentProperty.setParameters(params);
    QVersitDocument doc;
    QVersitProperty embProperty;
    embProperty.setName(QString("N"));
    embProperty.setValue(QByteArray("Marge"));
    doc.addProperty(embProperty);
    agentProperty.setEmbeddedDocument(doc);
    QCOMPARE(mWriter->encodeVersitProperty(agentProperty), QByteArray(expectedResult));
}

void UT_QVersitWriter::testEncodeParameters()
{
    QString encodingParameterName(QString::fromAscii("ENCODING"));
    
    // No parameters
    QMultiMap<QString,QString> parameters;
    QCOMPARE(mWriter->encodeParameters(parameters), QByteArray());
    
    // One parameter that is not a "TYPE" parameter
    parameters.insertMulti(encodingParameterName,QString::fromAscii("8BIT"));
    QCOMPARE(mWriter->encodeParameters(parameters), QByteArray(";ENCODING=8BIT"));
    
    // One parameter that is a "TYPE" parameter
    parameters.clear();
    parameters.insertMulti(QString::fromAscii("TYPE"),QString::fromAscii("HOME"));
    QCOMPARE(mWriter->encodeParameters(parameters), QByteArray(";HOME"));    
    
    // Two parameters
    parameters.insertMulti(encodingParameterName,QString::fromAscii("7BIT"));    
    QCOMPARE(mWriter->encodeParameters(parameters), QByteArray(";ENCODING=7BIT;HOME"));
    
    // Add ENCODING=QUOTED-PRINTABLE, not present in the parameters yet
    parameters.clear();
    QCOMPARE(mWriter->encodeParameters(parameters,true), 
             QByteArray(";ENCODING=QUOTED-PRINTABLE"));
    
    // ENCODING=QUOTED-PRINTABLE in the parameters already, don't encode it twice
    parameters.insertMulti(encodingParameterName,QString::fromAscii("QUOTED-PRINTABLE"));
    QCOMPARE(mWriter->encodeParameters(parameters,true), 
             QByteArray(";ENCODING=QUOTED-PRINTABLE"));    
}
