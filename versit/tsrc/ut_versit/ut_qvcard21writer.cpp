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

#include "ut_qvcard21writer.h"
#include "qvcard21writer.h"
#include "qversitproperty.h"
#include <QtTest/QtTest>
#include <QByteArray>

void UT_QVCard21Writer::init()
{
    mWriter = new QVCard21Writer;
}

void UT_QVCard21Writer::cleanup()
{
    delete mWriter;
}

void UT_QVCard21Writer::testEncodeVersitProperty()
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
    QMultiHash<QString,QString> params;
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

void UT_QVCard21Writer::testEncodeParameter()
{
    // Empty name and value
    QString name;
    QString value;
    QByteArray result;
    QCOMPARE(
        QString::fromAscii(mWriter->encodeParameter(name,value)),
        QString::fromAscii(result));

    // TYPE parameter -> name not encoded
    name = QString::fromAscii("TYPE");
    value = QString::fromAscii("HOME");
    result = "HOME";
    QCOMPARE(
        QString::fromAscii(mWriter->encodeParameter(name,value)),
        QString::fromAscii(result));

    // Other that TYPE parameter -> name encoded
    name = QString::fromAscii("ENCODING");
    value = QString::fromAscii("BASE64");
    result = "ENCODING=BASE64";
    QCOMPARE(
        QString::fromAscii(mWriter->encodeParameter(name,value)),
        QString::fromAscii(result));
}

void UT_QVCard21Writer::testQuotedPrintableEncode()
{
    QByteArray encodedValue;
    
    // The property doesn't contain ENCODING parameter, 
    // no special characters in the encodedValue -> no need to use Quoted-Printable encode
    QVersitProperty property;
    property.setName(QString::fromAscii("N"));
    property.setValue(QByteArray("Simpson;Homer"));
    QVERIFY(!mWriter->quotedPrintableEncode(property,encodedValue));
    QVERIFY(encodedValue == property.value());
    
    // The property doesn't contain ENCODING parameter,
    // special characters in the encodedValue -> needs to be Quoted-Printable encoded
    property.setName(QString::fromAscii("EMAIL"));
    property.setValue(QByteArray("homer@simpsons.com"));
    QVERIFY(mWriter->quotedPrintableEncode(property,encodedValue));
    QCOMPARE(QString::fromAscii(encodedValue), QString::fromAscii("homer=40simpsons.com"));
    
    // The property contains ENCODING parameter
    // -> Value should not be Quoted-Printable encoded
    property.setName(QString::fromAscii("PHOTO"));
    property.setValue(QByteArray("the data").toBase64());
    property.addParameter(QString::fromAscii("ENCODING"),QString::fromAscii("BASE64"));
    QVERIFY(!mWriter->quotedPrintableEncode(property,encodedValue));
    QVERIFY(encodedValue == property.value());
}
