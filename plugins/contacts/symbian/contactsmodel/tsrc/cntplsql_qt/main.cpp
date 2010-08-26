/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Retrieves the character map for each of the numeric keys.
*/

#include "testrunner.h"
#include "ut_cntsqlsearch.h"

#include <QtTest/QtTest>

int main(int argc, char *argv[]) 
{
    bool promptOnExit(true);
    for (int i=0; i<argc; i++) {
        if (QString(argv[i]) == "-noprompt")
            promptOnExit = false;
    }
    printf("Running tests...\n");
            
    QApplication app(argc, argv);
    
    TestRunner testRunner("ut_symbian");
    
    UT_CntSqlSearch ut_sqlSearch;
    testRunner.runTests(ut_sqlSearch);
    
    testRunner.printResults();

    /*if (promptOnExit) {
        printf("Press any key...\n");
        getchar(); 
    }*/
    return 0;   
}

