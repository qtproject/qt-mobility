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
    QByteArray simpleProperty = "BEGIN:VCARD\r\n"; 
    QVersitProperty property;
    property.setName(QString("BEGIN"));
    property.setValue(QByteArray("VCARD"));
    QCOMPARE(mWriter->encodeVersitProperty(property), simpleProperty);
    
    QByteArray  oneParamValueProperty = 
        "LABEL;ENCODING=QUOTED-PRINTABLE:123\r\n";    
    QVersitProperty property1;
    property1.setName(QString("LABEL"));
    property1.setValue(QByteArray("123"));
    QMultiMap<QString,QString> params1;
    params1.insert(QString("ENCODING"), QString("QUOTED-PRINTABLE"));
    property1.setParameters(params1);
    QCOMPARE(mWriter->encodeVersitProperty(property1), oneParamValueProperty);

    QByteArray oneParamProperty = 
        "ADR;HOME:14 Bridge Street, FL1850, USA\r\n";
    QVersitProperty property2;
    property2.setName(QString("ADR"));
    property2.setValue(QByteArray("14 Bridge Street, FL1850, USA"));
    QMultiMap<QString,QString> params2;
    params2.insert(QString("TYPE"), QString("HOME"));
    property2.setParameters(params2);
    QCOMPARE(mWriter->encodeVersitProperty(property2), oneParamProperty);
    
    QByteArray moreParamsProperty = 
        "ADR;HOME;DOM:2009 Santa Claus Street, Lapland, Finland\r\n";
    QVersitProperty property3;
    property3.setName(QString("ADR"));
    property3.setValue(QByteArray("2009 Santa Claus Street, Lapland, Finland"));
    QMultiMap<QString,QString> params3;
    params3.insert(QString("TYPE"), QString("HOME"));
    params3.insert(QString("TYPE"), QString("DOM"));
    property3.setParameters(params3);
    QCOMPARE(mWriter->encodeVersitProperty(property3), moreParamsProperty);
}
