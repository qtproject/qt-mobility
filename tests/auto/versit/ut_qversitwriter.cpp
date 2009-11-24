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
    mWritingDoneCalled = false;
    mOutputDevice = new QBuffer;
    mWriter = new QVersitWriter;
    connect(mWriter,SIGNAL(writingDone()),this,SLOT(writingDone()),Qt::DirectConnection);
    mWriterPrivate = new QVCard21Writer;
}

void UT_QVersitWriter::cleanup()
{
    delete mWriterPrivate;
    delete mWriter;
    delete mOutputDevice;
}

void UT_QVersitWriter::writingDone()
{
    mWritingDoneCalled = true;
}

void UT_QVersitWriter::testDevice()
{
    // No device
    QVERIFY(mWriter->device() == NULL);    
    
    // Device has been set
    mWriter->setDevice(mOutputDevice);
    QVERIFY(mWriter->device() == mOutputDevice);
}

void UT_QVersitWriter::testWriting()
{
    // Device not set
    QVERIFY(!mWriter->writeAll());

    // Device set, but not opened
    mWriter->setDevice(mOutputDevice);
    QVERIFY(!mWriter->writeAll());

    // vCard 2.1
    const char vCard21[] =
"BEGIN:VCARD\r\n\
VERSION:2.1\r\n\
FN:Homer\r\n\
END:VCARD\r\n";
    mOutputDevice->open(QBuffer::ReadWrite);
    QVersitDocument document;
    QVersitProperty property;
    property.setName(QString(QString::fromAscii("FN")));
    property.setValue(QByteArray("Homer"));
    document.addProperty(property);
    document.setVersitType(QVersitDocument::VCard21);
    mWriter->setVersitDocument(document);
    QVERIFY(mWriter->writeAll());
    mOutputDevice->seek(0);
    QByteArray result(mOutputDevice->readAll());
    QCOMPARE(QString::fromAscii(result),QString::fromAscii(vCard21));

    // vCard 3.0
    const char vCard30[] =
"BEGIN:VCARD\r\n\
VERSION:3.0\r\n\
FN:Homer\r\n\
END:VCARD\r\n";
    document.setVersitType(QVersitDocument::VCard30);
    mWriter->setVersitDocument(document);
    mOutputDevice->reset();
    QVERIFY(mWriter->writeAll());
    mOutputDevice->seek(0);
    result = mOutputDevice->readAll();
    QCOMPARE(QString::fromAscii(result),QString::fromAscii(vCard30));

    // Asynchronous writing
    QVERIFY(!mWritingDoneCalled);
    mOutputDevice->reset();
    QVERIFY(mWriter->startWriting());
    delete mWriter; // waits for the thread to finish
    mWriter = 0;
    QVERIFY(mWritingDoneCalled);

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

