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

#include "versittest.h"
#include "vcardcomparator.h" 

#include <QDir>
#include <QFile>
#include <QStringList>
#include <QBuffer>
#include <QList>
#include <QtTest/QtTest>
#include <qcontactmanager.h>
#include <qversitreader.h>
#include <qversitwriter.h>
#include <qversitdocument.h>
#include <qversitcontactgenerator.h>
#include <qversitcontactconverter.h>

const QString inputDirPath = "c:\\data\\testvcards\\in";
const QString excludeFieldsFileName = "c:\\data\\testvcards\\excludefields.txt";
const QString outputDirPath = "c:\\data\\testvcards\\out";

VersitTest::VersitTest() 
    : QObject(), mSaveContacts(false)
{   
}

VersitTest::VersitTest(bool saveContacts) 
    : QObject(), mSaveContacts(saveContacts)
{
}

void VersitTest::initTestCase()
{
    QDir dir(inputDirPath);
    QStringList type("*.vcf");
    mFiles = dir.entryList(type);

    mExcludedFields = new QStringList;
    QFile excludeFieldsFile(excludeFieldsFileName);
    if (excludeFieldsFile.open(QIODevice::ReadOnly)) {
        while (!excludeFieldsFile.atEnd()) {
            QString field(excludeFieldsFile.readLine().trimmed());
            mExcludedFields->append(field);
        }
        excludeFieldsFile.close();
    }
    mContactManager = new QContactManager(QString::fromAscii("symbian"));
}

void VersitTest::cleanupTestCase()
{
    delete mContactManager;
    delete mExcludedFields;
    mFiles.clear();
}

void VersitTest::init()
{
    mReader = new QVersitReader;
    mWriter = new QVersitWriter;
}

void VersitTest::cleanup()
{
    delete mWriter;
    delete mReader;
}

void VersitTest::test()
{
    // Run test against the data if its availble
    if ( QTest::currentDataTag() ) {
        QFETCH(QString, InFile);
        QFile in(InFile);
        QVERIFY(in.open(QIODevice::ReadOnly));        
        QDir dir;
        if (!dir.exists(outputDirPath))
            dir.mkdir(outputDirPath);
        QFileInfo fileInfo = in.fileName();
        QFile out(outputDirPath+ "\\" + fileInfo.fileName());
        QVERIFY(out.open(QIODevice::ReadWrite));
        QBENCHMARK { executeTest(in,out); }
        in.close();
        out.close();        
    }
}

void VersitTest::test_data()
{
    QTest::addColumn<QString>("InFile");
    QStringListIterator fileIterator(mFiles);
    while (fileIterator.hasNext()) {
        const QString& fileName = inputDirPath + "\\" + fileIterator.next();
        const QString& myDataFeed = "Data feed:" + fileName; 
        QTest::newRow( myDataFeed.toAscii().data() ) << fileName;
    }
}

void VersitTest::executeTest(QFile& in, QIODevice& out)
{    
    mReader->setDevice(&in);
    mWriter->setDevice(&out);
    
    // Parse the input
    QVERIFY2(mReader->start(), in.fileName().toAscii().constData());
    
    // Convert to QContacts
    QList<QContact> contacts;
    QVersitContactGenerator generator;
    foreach (QVersitDocument document, mReader->result()) {
        QContact contact = generator.generateContact(document);
        if (mSaveContacts)
            QVERIFY(mContactManager->saveContact(&contact));
        contacts.append(contact);
    }    
    
    // Convert back to QVersitDocuments
    QList<QVersitDocument> documents;
    QVersitContactConverter converter;
    foreach (QContact contact, contacts) {
        documents.append(converter.convertContact(contact));
    }
    
    // Encode and write to output
    foreach (QVersitDocument document, documents) {
        mWriter->setVersitDocument(document);
        QVERIFY2(mWriter->start(), in.fileName().toAscii().constData());
    }
    
    // Compare the input and output
    in.seek(0);
    out.seek(0);    
    VCardComparator comparator(in,out,*mExcludedFields);
    QCOMPARE(QString(),comparator.nonMatchingLines());
}


