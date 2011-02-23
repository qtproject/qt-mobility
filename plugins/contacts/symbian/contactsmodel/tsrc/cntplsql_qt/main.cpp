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

#include "ut_cntsqlsearch.h"
#include "ut_cntsqlfactory.h"
#include "ut_cntsqlkoreanitut.h"
#include "ut_cntsqlsearchbase.h"

#include <QtTest/QtTest>

int main(int argc, char *argv[]) 
{
    bool promptOnExit(true);
    for (int i=0; i<argc; i++) {
        if (QString(argv[i]) == "-noprompt")
            promptOnExit = false;
    }
    printf("Running tests...\n");

    QString resultFileName = "c:/ut_sqlSearch.xml";
    QStringList args_sqlSearch( "UT_CntSqlSearch");
    args_sqlSearch << "-o" << resultFileName;
    UT_CntSqlSearch ut_sqlSearch;
    QTest::qExec(&ut_sqlSearch, args_sqlSearch);
    
    resultFileName = QString("c:/ut_sqlsearchbase.xml");
    QStringList args_sqlsearchbase( "UT_CntSqlSearchBase");
    args_sqlsearchbase << "-o" << resultFileName;
    UT_CntSqlSearchBase ut_sqlsearchbase;
    QTest::qExec(&ut_sqlsearchbase, args_sqlsearchbase);
  
    resultFileName = QString("c:/ut_sqlkoreanitut.xml");
    QStringList args_sqlkoreanitut( "UT_CntSqlKoreanItuT");
    args_sqlkoreanitut << "-o" << resultFileName;
    UT_CntSqlKoreanItuT ut_sqlkoreanitut;
    QTest::qExec(&ut_sqlkoreanitut, args_sqlkoreanitut);
    
    resultFileName = "c:/ut_sqlfactory.xml";
    QStringList args_sqlfactory( "UT_CntSqlSearch");
    args_sqlfactory << "-o" << resultFileName;
    UT_CntSqlFactory ut_sqlfactory;
    QTest::qExec(&ut_sqlfactory, args_sqlfactory);
    

    /*if (promptOnExit) {
        printf("Press any key...\n");
        getchar(); 
    }*/
    return 0;   
}

