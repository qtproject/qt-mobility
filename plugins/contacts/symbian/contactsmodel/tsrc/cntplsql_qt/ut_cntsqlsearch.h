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

#ifndef UT_CNTSQLSEARCH_H
#define UT_CNTSQLSEARCH_H

#include <QObject>

class CntSqlSearch;
class C12keyKeyMap;
class CQwertyKeyMap;

class UT_CntSqlSearch : public QObject                 
{
     Q_OBJECT
    
private slots:

/*
 * In addition, there are four private slots that are not treated as testfunctions. 
 * They will be executed by the testing framework and can be used to initialize and clean up 
 * either the entire test or the current test function.
 * 
 * initTestCase() will be called before the first testfunction is executed.
 * cleanupTestCase() will be called after the last testfunction was executed.
 * init() will be called before each testfunction is executed.
 * cleanup() will be called after every testfunction.
*/
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots: //test methods

    void testCompareTwoColumnsWithModifiedPattern();
    void testPredictiveSearch();
    void testSelectQweryTable();
    void testSelectTableView();
    void testCreateQuery();
    void testCreateQwertyQuery();
    void testExactMatchSearch();
    void testExactMatchSearchQwerty();
    void testIntersectionSearch();
    void testSearchTokensFromOneTable();
    //void testCompareTwoColumnsWithModifiedPattern();   
    void testModifiedMatchColumns();
    void testExactMatch();
    void testExactMatchQwerty();
    void testCreateJoinTableSearch();
    void testCompareTwoQwertyColumns();
    void testChangeStringPadings();
    void testUpperLimit();
    void testLowerLimit();
    void testGetTokens();
    void testTestPattern();
    void testIsQwerty();
    void testQwertyTokens();
    
    
private:
 
    C12keyKeyMap* m12KeyKeyMap;
    CQwertyKeyMap* mQwertyKeyMap;
    CntSqlSearch* mCntSqlSearch; 
};

#endif // UT_CNTSQLSEARCH_H
