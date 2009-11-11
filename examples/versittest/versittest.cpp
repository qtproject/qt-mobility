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
#include <qversitcontactimporter.h>
#include <qversitcontactexporter.h>

VersitTest::VersitTest() :
    QObject(),
    mSaveContacts(false),
    mScaledImageHeight(0),
    mScaledImageWidth(0)
{
}

VersitTest::VersitTest(bool saveContacts,int scaledImageHeight,int scaledImageWidth) :
    QObject(),
    mSaveContacts(saveContacts),
    mScaledImageHeight(scaledImageHeight),
    mScaledImageWidth(scaledImageWidth)
{
    QString homeDir = QDir::homePath();
    if (!homeDir.endsWith(QString::fromAscii("/")))
        homeDir += QString::fromAscii("/");
    QDir dir(homeDir);
    QString testDir("testvcards/");
    mInputDirPath = homeDir + testDir + QString::fromAscii("in");
    mExcludeFieldsFileName = homeDir + testDir + QString::fromAscii("excludefields.txt");
    mOutputDirPath = homeDir + testDir + QString::fromAscii("out");
    mImageAndAudioClipPath = homeDir + testDir + QString::fromAscii("images_and_audio");
}

void VersitTest::scale(const QString& imageFileName, QByteArray& imageData)
{
    if (mScaledImageHeight > 0 && mScaledImageWidth > 0) {
        QImage image(imageFileName);
        image = image.scaled(mScaledImageHeight,mScaledImageWidth);
        QBuffer imageBuffer(&imageData);
        QByteArray fileExtension = imageFileName.section('.',-1).toUpper().toAscii();
        image.save(&imageBuffer,fileExtension.constData());
    }
}

void VersitTest::initTestCase()
{
    QDir dir(mInputDirPath);
    QStringList type("*.vcf");
    mFiles = dir.entryList(type);

    mExcludedFields = new QStringList;
    QFile excludeFieldsFile(mExcludeFieldsFileName);
    if (excludeFieldsFile.open(QIODevice::ReadOnly)) {
        while (!excludeFieldsFile.atEnd()) {
            QString field = QString::fromAscii(excludeFieldsFile.readLine().trimmed());
            mExcludedFields->append(field);
        }
        excludeFieldsFile.close();
    }
    mContactManager = new QContactManager(QString::fromAscii("symbian"));

    if (!dir.exists(mImageAndAudioClipPath)) {
        dir.mkdir(mImageAndAudioClipPath);
    }    
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
    mImporter = new QVersitContactImporter;
    mExporter = new QVersitContactExporter;
}

void VersitTest::cleanup()
{
    delete mImporter;
    delete mExporter;    
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
        if (!dir.exists(mOutputDirPath))
            dir.mkdir(mOutputDirPath);
        QFileInfo fileInfo = in.fileName();
        QFile out(mOutputDirPath+ "/" + fileInfo.fileName());
        out.remove();
        QVERIFY(out.open(QIODevice::ReadWrite));
        QBENCHMARK { executeTest(in,out); }
        in.seek(0);
        out.seek(0);
        VCardComparator comparator(in,out,*mExcludedFields);
        QCOMPARE(QString(),comparator.nonMatchingLines());
        in.close();
        out.close();        
    }
}

void VersitTest::test_data()
{
    QTest::addColumn<QString>("InFile");
    QStringListIterator fileIterator(mFiles);
    while (fileIterator.hasNext()) {
        QString fileName = mInputDirPath + "/" + fileIterator.next();
        QTest::newRow(fileName.toAscii().data()) << fileName;
    }
}

void VersitTest::executeTest(QFile& in, QIODevice& out)
{   
    in.seek(0);
    mReader->setDevice(&in);
    out.reset();
    mWriter->setDevice(&out);

    // Parse the input
    QVERIFY2(mReader->readAll(),in.fileName().toAscii().constData());
    
    // Convert to QContacts
    QList<QContact> contacts;
    QList<QVersitDocument::VersitType> documentTypes;
    foreach (QVersitDocument document, mReader->result()) {
        mImporter->setImagePath(mImageAndAudioClipPath);
        mImporter->setAudioClipPath(mImageAndAudioClipPath);
        QContact contact = mImporter->importContact(document);
        if (mSaveContacts)
            QVERIFY(mContactManager->saveContact(&contact));
        contacts.append(contact);
        documentTypes.append(document.versitType());
    }    
    
    // Convert back to QVersitDocuments
    QList<QVersitDocument> documents;
    connect(mExporter, SIGNAL(scale(const QString&,QByteArray&)),
            this, SLOT(scale(const QString&,QByteArray&)));

    foreach (QContact contact, contacts) {
        QVersitDocument document = mExporter->exportContact(contact);
        if (!documentTypes.isEmpty())
            document.setVersitType(documentTypes.takeFirst());
        documents.append(document);
    }
    
    // Encode and write to output
    foreach (QVersitDocument document, documents) {
        mWriter->setVersitDocument(document);
        QVERIFY2(mWriter->writeAll(),in.fileName().toAscii().constData());
    }
}


