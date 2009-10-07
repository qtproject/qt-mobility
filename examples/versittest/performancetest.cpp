/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Symbian Foundation License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include "performancetest.h"
#include "vcardcomparator.h"
#include <qcontactmanager.h>
#include <qcontact.h>
#include <QDir>
#include <QFile>
#include <QStringList>
#include <QBuffer>
#include <QList>
#include <QtTest/QtTest>
#include <stdio.h>


const QString inputDirPath = "c:\\data\\vcard";
const QString excludeFieldsFileName = "c:\\data\\vcard\\excludefields.txt";

void PerformanceTest::initTestCase()
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
}

void PerformanceTest::cleanupTestCase()
{
    delete mExcludedFields;
    mFiles.clear();
}

void PerformanceTest::init()
{

}

void PerformanceTest::cleanup()
{

}

void PerformanceTest::performanceTest()
{
    // Run test against the data if its availble
    if ( QTest::currentDataTag() ) {
        QFETCH(QString, InFile);
        QFile in(InFile);
        QVERIFY(in.open(QIODevice::ReadOnly));
        QBuffer out;
        QVERIFY(out.open(QIODevice::ReadWrite));
        executeTest(in,out);
        in.close();
        out.close();
    }
}

void PerformanceTest::performanceTest_data()
{
    QTest::addColumn<QString>("InFile");
    QStringListIterator fileIterator(mFiles);
    while (fileIterator.hasNext()) {
        const QString& fileName = inputDirPath + "\\" + fileIterator.next();
        const QString& myDataFeed = "Data feed:" + fileName;
        QTest::newRow( myDataFeed.toAscii().data() ) << fileName;
    }
}

void PerformanceTest::executeTest(QIODevice& in, QIODevice& out)
{   
    QBENCHMARK {
        printf("TODO : Implement import functionality\n");
    }
    QBENCHMARK {
        printf("TODO : Implement export functionality\n");
    }
}
