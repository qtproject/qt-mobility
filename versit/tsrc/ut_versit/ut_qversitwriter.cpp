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
    
    // Device set, document not set
    QBuffer buffer;
    mWriter->setDevice(&buffer);
    buffer.open(QBuffer::ReadWrite);
    QVERIFY(!mWriter->start());

    // Document set, device not set
    mWriter->setDevice(0);
    QVersitDocument document;
    QVersitProperty property;
    property.setName(QString("FN"));
    property.setValue(QByteArray("Homer"));
    document.addProperty(property);
    mWriter->setVersitDocument(document);
    QVERIFY(!mWriter->start());

    // vCard 2.1
    const char vCard21[] =
"BEGIN:VCARD\r\n\
VERSION:2.1\r\n\
FN:Homer\r\n\
END:VCARD\r\n";
    document.setVersitType(QVersitDocument::VCard21);
    mWriter->setVersitDocument(document);
    buffer.open(QBuffer::ReadWrite);
    mWriter->setDevice(&buffer);
    QVERIFY(mWriter->start());
    buffer.seek(0);
    QByteArray result(buffer.readAll());
    QCOMPARE(QString::fromAscii(result),QString::fromAscii(vCard21));

    // vCard 3.0
    const char vCard30[] =
"BEGIN:VCARD\r\n\
VERSION:3.0\r\n\
FN:Homer\r\n\
END:VCARD\r\n";
    document.setVersitType(QVersitDocument::VCard30);
    mWriter->setVersitDocument(document);
    buffer.reset();
    mWriter->setDevice(&buffer);
    QVERIFY(mWriter->start());
    buffer.seek(0);
    result = buffer.readAll();
    QCOMPARE(QString::fromAscii(result),QString::fromAscii(vCard30));

}

void UT_QVersitWriter::testEncodeGroupsAndName()
{
    QVersitProperty property;

    // No groups
    property.setName(QString::fromAscii("name"));
    QByteArray result("NAME");
    QCOMPARE(mWriterPrivate->encodeGroupsAndName(property),result);

    // One group
    property.setGroups(QStringList(QString::fromAscii("group")));
    result = "group.NAME";
    QCOMPARE(mWriterPrivate->encodeGroupsAndName(property),result);

    // Two groups
    QStringList groups(QString::fromAscii("group1"));
    groups.append(QString::fromAscii("group2"));
    property.setGroups(groups);
    result = "group1.group2.NAME";
    QCOMPARE(mWriterPrivate->encodeGroupsAndName(property),result);
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
