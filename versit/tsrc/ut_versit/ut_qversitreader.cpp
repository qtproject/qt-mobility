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

#include "ut_qversitreader.h"
#include "qversitreader.h"
#include <QtTest/QtTest>
#include <QBuffer>

void UT_QVersitReader::init()
{
    m_reader = new QVersitReader;
}

void UT_QVersitReader::cleanup()
{   
    delete m_reader;
}

void UT_QVersitReader::testDevice()
{
    // No device
    QVERIFY(m_reader->device() == NULL);    
    
    // Device has been set
    QBuffer device;
    m_reader->setDevice(&device);
    QVERIFY(m_reader->device() == &device);
}

void UT_QVersitReader::testStart()
{
    QVERIFY(!m_reader->start());
}

void UT_QVersitReader::testResult()
{
    QCOMPARE(m_reader->result().count(),0);
}

void UT_QVersitReader::testParseNextVersitProperty()
{
    const char vCardData[] = 
"BEGIN:VCARD\r\n\
VERSION:2.1\r\n\
N:Homer\r\n\
EMAIL;ENCODING=QUOTED-PRINTABLE:homer=40simp=\r\nsons.com\r\n\
AGENT:\r\nBEGIN:VCARD\r\nN:Marge\r\nEND:VCARD\r\n\
END:VCARD\r\n";
    
    QByteArray vCard(vCardData);
    
    QVersitProperty property = m_reader->parseNextVersitProperty(vCard);
    QCOMPARE(property.name(),QString("BEGIN"));
    QCOMPARE(property.value(),QByteArray("VCARD"));
    
    property = m_reader->parseNextVersitProperty(vCard);
    QCOMPARE(property.name(),QString("VERSION"));
    QCOMPARE(property.value(),QByteArray("2.1"));
    
    property = m_reader->parseNextVersitProperty(vCard);
    QCOMPARE(property.name(),QString("N"));
    QCOMPARE(property.value(),QByteArray("Homer"));
    
    property = m_reader->parseNextVersitProperty(vCard);
    QCOMPARE(property.name(),QString("EMAIL"));
    QCOMPARE(0,property.parameters().count());
    QCOMPARE(property.value(),QByteArray("homer@simpsons.com"));
    
    property = m_reader->parseNextVersitProperty(vCard);
    QCOMPARE(property.name(),QString("AGENT"));
    QCOMPARE(property.value(),QByteArray());
    
    property = m_reader->parseNextVersitProperty(vCard);
    QCOMPARE(property.name(),QString("END"));
    QCOMPARE(property.value(),QByteArray("VCARD"));
    
    property = m_reader->parseNextVersitProperty(vCard);
    QCOMPARE(property.name(),QString());
    QCOMPARE(property.value(),QByteArray());
}

void UT_QVersitReader::testParseVersitDocument()
{
    // Valid card
    const char validCard[] = 
"BEGIN:VCARD\r\n\
VERSION:2.1\r\n\
N:Homer\r\n\
AGENT:BEGIN:VCARD\r\nN:Marge\r\nEND:VCARD\r\n\
EMAIL;ENCODING=QUOTED-PRINTABLE:homer=40simp=\r\nsons.com\r\n\
END:VCARD\r\n";
    QByteArray vCard(validCard);
    QVERIFY(m_reader->parseVersitDocument(vCard));
    
    // No BEGIN found
    const char beginMissing[] = 
"VCARD\r\n\
VERSION:2.1\r\n\
N:Nobody\r\n\
END:VCARD\r\n";
    vCard = beginMissing;
    QVERIFY(!m_reader->parseVersitDocument(vCard));
    
    const char wrongType[] = 
"BEGIN:VCAL\r\n\
END:VCAL\r\n";
    vCard = wrongType;
    QVERIFY(!m_reader->parseVersitDocument(vCard));    
    
    // Wrong version
    const char wrongVersion[] = 
"BEGIN:VCARD\r\n\
VERSION:3.0\r\n\
N:Nobody\r\n\
END:VCARD\r\n";
    vCard = wrongVersion;
    QVERIFY(!m_reader->parseVersitDocument(vCard));    
    
    // No END found
    const char endMissing[] = 
"BEGIN:VCARD\r\n\
VERSION:2.1\r\n\
N:Nobody\r\n\
VCARD\r\n";
    vCard = endMissing;
    QVERIFY(!m_reader->parseVersitDocument(vCard));    
}
