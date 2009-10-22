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

#include <QtTest/QtTest>

int main(int argc, char** argv)
{   
    bool saveContacts = false; 
    for (int i=0; i < argc; i++) {
        QString argStr(QString::fromAscii(argv[i]));
        if (argStr == QString::fromAscii("save"))
            saveContacts = true;
    }
    TestResultXmlParser parser;
    QString resultFileName("c:/QVersitTestResults.xml");
    QStringList args;
    printf("Running tests...\n");
    VersitTest versitTest(saveContacts);
    args << "versittest" << "-xml" << "-o" << resultFileName;
    QTest::qExec(&versitTest, args);
    parser.parseAndPrintResults(resultFileName,true);
    printf("Press any key...\n");
    getchar();
    return 0;   
}



 
