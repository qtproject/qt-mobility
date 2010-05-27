/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "testrunner.h"
#include "ut_cntsymbianengine.h"
#include "ut_cntfiltering.h"
#include "ut_matchphonenumber.h"
#include "ut_cntrelationship.h"
//#include "ut_cntsymbiandatabase.h"
#include "ut_transformcontactdata.h"
#include "ut_cntsqlsearch.h"

#include <QtTest/QtTest>

int main(int argc, char *argv[]) 
{
    /*bool promptOnExit(true);
    for (int i=0; i<argc; i++) {
        if (QString(argv[i]) == "-noprompt")
            promptOnExit = false;
    }*/
    printf("Running tests...\n");
            
    QApplication app(argc, argv);
    
    TestRunner testRunner("ut_symbian");

    TestSymbianEngine ut_symbainEngine;
    testRunner.runTests(ut_symbainEngine);
    
    TestFiltering ut_filtering;
    testRunner.runTests(ut_filtering);
    
    TestMatchPhoneNumber ut_matchphonenumber;
    testRunner.runTests(ut_matchphonenumber);
    
    TestCntRelationship ut_relationship;
    testRunner.runTests(ut_relationship);
    
    //TODO: fix
    //TestCntSymbianDatabase ut_symbianDatabase;
    //testRunner.runTests(ut_symbianDatabase);
    
    TestCntTransformContactData ut_transformContactData;
    testRunner.runTests(ut_transformContactData);
    
    UT_CntSqlSearch ut_sqlSearch;
    testRunner.runTests(ut_sqlSearch);
    
    testRunner.printResults();

    /*if (promptOnExit) {
        printf("Press any key...\n");
        getchar(); 
    }*/
    return 0;   
}

