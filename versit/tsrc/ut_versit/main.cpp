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

#include "testresultxmlparser.h"
#include "ut_qversitcontactimporter.h"
#include "ut_qversitcontactexporter.h"
#include "ut_qversitproperty.h"
#include "ut_qversitdocument.h"
#include "ut_versitutils.h"
#include "ut_qversitreader.h"
#include "ut_qversitwriter.h"
#include "ut_qvcard21writer.h"
#include "ut_qvcard30writer.h"

#include <QDir>
#include <QtTest/QtTest>

int main(int /*argc*/, char** /*argv[]*/)
{
    printf("Running tests...\n");
    TestResultXmlParser parser;
    QStringList args(QString::fromAscii("ut_versit"));
    args << QString::fromAscii("-xml") << QString::fromAscii("-o");
    QString homeDir = QDir::homePath();
    if (!homeDir.endsWith(QString::fromAscii("/")))
        homeDir += QString::fromAscii("/");

    UT_QVersitContactImporter ut_versitContactImporter;
    QString resultFileName = homeDir + QString::fromAscii("ut_versitContactImporter.xml");
    args << resultFileName;
    QTest::qExec(&ut_versitContactImporter, args);
    parser.parseAndPrintResults(resultFileName);

    UT_QVersitContactExporter ut_versitContactExporter;
    resultFileName = homeDir + QString::fromAscii("ut_versitContactExporter.xml");
    args.replace(args.count()-1,resultFileName);
    QTest::qExec(&ut_versitContactExporter, args);
    parser.parseAndPrintResults(resultFileName);
	
    UT_QVersitProperty ut_qVersitProperty;
    resultFileName = homeDir + QString::fromAscii("ut_versitProperty.xml");
    args.replace(args.count()-1,resultFileName);
    QTest::qExec(&ut_qVersitProperty, args);
    parser.parseAndPrintResults(resultFileName);
	
    UT_QVersitDocument ut_qVersitDocument;
    resultFileName = homeDir + QString::fromAscii("ut_versitDocument.xml");
    args.replace(args.count()-1,resultFileName);
    QTest::qExec(&ut_qVersitDocument, args);
    parser.parseAndPrintResults(resultFileName);
    
    UT_VersitUtils ut_versitUtils;
    resultFileName = homeDir + QString::fromAscii("ut_versitUtils.xml");
    args.replace(args.count()-1,resultFileName);    
    QTest::qExec(&ut_versitUtils, args);
    parser.parseAndPrintResults(resultFileName);
    
    UT_QVersitReader ut_versitReader;
    resultFileName = homeDir + QString::fromAscii("ut_versitReader.xml");
    args.replace(args.count()-1,resultFileName);    
    QTest::qExec(&ut_versitReader, args);
    parser.parseAndPrintResults(resultFileName);
   
    UT_QVersitWriter ut_versitWriter;
    resultFileName = homeDir + QString::fromAscii("ut_versitWriter.xml");
    args.replace(args.count()-1,resultFileName);    
    QTest::qExec(&ut_versitWriter, args);
    parser.parseAndPrintResults(resultFileName);
    
    UT_QVCard21Writer ut_vcard21Writer;
    resultFileName = homeDir + QString::fromAscii("ut_vcard21Writer.xml");
    args.replace(args.count()-1,resultFileName);
    QTest::qExec(&ut_vcard21Writer, args);
    parser.parseAndPrintResults(resultFileName);

    UT_QVCard30Writer ut_vcard30Writer;
    resultFileName = homeDir + QString::fromAscii("ut_vcard30Writer.xml");
    args.replace(args.count()-1,resultFileName);
    QTest::qExec(&ut_vcard30Writer, args);
    parser.parseAndPrintResults(resultFileName,true);

    printf("Press any key...\n");
    getchar(); 

    return 0;   
}



 
