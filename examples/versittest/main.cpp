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
#include "testresultxmlparser.h"
#include "testconfiguration.h"

#include <QtTest/QtTest>

int main(int argc, char** argv)
{   
    QString homeDir = QDir::homePath();
    if (!homeDir.endsWith(QString::fromAscii("/")))
        homeDir += QString::fromAscii("/");
    QString confFileName = homeDir + QString::fromAscii("conf.xml");
    TestConfiguration conf;
    conf.parse(confFileName);

    // overwrite configuration file setting by command line params.
    bool saveContacts = conf.saveContact();
    bool logToFile = conf.logToFile();
    bool performanceTest = conf.performanceTest();
    QString testName = ( conf.testName().length() >0 ) ? conf.testName()
                                                       : QString::fromAscii("versittest");
    int scaledImageHeight = 0;
    int scaledImageWidth = 0;
    for (int i=0; i < argc; i++) {
        QString argStr(QString::fromAscii(argv[i]));        
        if (argStr == QString::fromAscii("save")) {
            // if save argument in command line enable contact saving
            saveContacts = true;
        }else if (argStr == QString::fromAscii("log")) {
            // if log argument in command line enable xml result logging
            logToFile = true;
        }else if (argStr == QString::fromAscii("test")) {
            // if test argument in command line disable performance , so enables the normal test
            performanceTest = false;
        }else {
            // Scaling height and width
            QStringList list = argStr.split('x');
            if (!list.isEmpty())
                scaledImageHeight = list.takeFirst().toInt();
            if (!list.isEmpty())
                scaledImageWidth = list.takeFirst().toInt();
        }
    }
    VersitTest versitTest(performanceTest,saveContacts,scaledImageHeight,scaledImageWidth);
    printf("Running tests...\n");
    QStringList args;    
    if( logToFile){
        QString resultFileName = homeDir + QString::fromAscii("QVersitTestResults.xml");
        args << testName << "-xml" << "-o" << resultFileName;
        QTest::qExec(&versitTest, args);
        TestResultXmlParser parser;
        parser.parseAndPrintResults(resultFileName,true);
    }
    else{
        args << testName;
        QTest::qExec(&versitTest, args);
    }    
    printf("Press any key...\n");
    getchar();
    return 0;   
}



 
