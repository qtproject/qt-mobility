/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#include <QtTest/QtTest>

#include "ut_cntsqlkoreanitut.h"
#include "cntsqlkoreanitut.h"
#include "c12keykeymap.h"

#define WRITE_LOGS
#define SQL_QT_TEST

#if defined(WRITE_LOGS)
#include <e32debug.h> // RDebug

#define LOG(a)              WriteLog(a, "")
#define LOG2(a, b)          WriteLog(a, b)
#define TEST_BEGIN_LOG(a)   LOG(a##" begins")
#define TEST_PASSED_LOG(a)  LOG(a##" passed")

void WriteLog(const QString a, const QString b);
void WritePart(const TDesC& s);
#else
#define LOG(a)
#define LOG2(a, b)
#define TEST_BEGIN_LOG(a)
#define TEST_PASSED_LOG(a)
#endif


void UT_CntSqlKoreanItuT::initTestCase()
{
}

void UT_CntSqlKoreanItuT::cleanupTestCase()
{
}


void UT_CntSqlKoreanItuT::init()
{   
    QT_TRAP_THROWING(mTwelveKeyMap = C12keyKeyMap::NewL());
    mSqlKoreanItuT = new CntSqlKoreanItuT(mTwelveKeyMap);
}

void UT_CntSqlKoreanItuT::cleanup()
{
    delete mTwelveKeyMap;
    delete mSqlKoreanItuT;    
}
void UT_CntSqlKoreanItuT::testCreateInputSpecificSearch()
{
    TEST_BEGIN_LOG("testCreateInputSpecificSearch");

    QString pattern = "3";
    QString reference = "SELECT contact_id FROM predictivesearch3 ORDER BY first_name, last_name ASC;";
    QString result = mSqlKoreanItuT->createInputSpecificSearch(pattern);
    //qDebug() << pattern << " -> result" << result;
    LOG2(pattern, result);
    QVERIFY( !result.compare( reference) );
    
    pattern = QString("25");
    reference = QString("SELECT contact_id FROM predictivesearch2 WHERE ((nbr>144115188075855871 AND nbr<216172782113783808) AND (nbr2>360287970189639679 AND nbr2<432345564227567616)) OR ((nbr2>144115188075855871 AND nbr2<216172782113783808) AND (nbr3>360287970189639679 AND nbr3<432345564227567616)) OR ((nbr3>144115188075855871 AND nbr3<216172782113783808) AND (nbr4>360287970189639679 AND nbr4<432345564227567616)) OR (nbr4>144115188075855871 AND nbr4<216172782113783808) ORDER BY first_name, last_name ASC;");
    result = mSqlKoreanItuT->createInputSpecificSearch(pattern);
    QVERIFY(!result.compare( reference));   
    
    pattern = QString ("255");
    reference = "SELECT contact_id FROM predictivesearch2 WHERE ((nbr>144115188075855871 AND nbr<216172782113783808) AND (nbr2>360287970189639679 AND nbr2<432345564227567616) AND (nbr3>360287970189639679 AND nbr3<432345564227567616)) OR ((nbr2>144115188075855871 AND nbr2<216172782113783808) AND (nbr3>360287970189639679 AND nbr3<432345564227567616) AND (nbr4>360287970189639679 AND nbr4<432345564227567616)) OR ((nbr3>144115188075855871 AND nbr3<216172782113783808) AND (nbr4>360287970189639679 AND nbr4<432345564227567616)) OR (nbr4>144115188075855871 AND nbr4<216172782113783808) ORDER BY first_name, last_name ASC;"; 
    result = mSqlKoreanItuT->createInputSpecificSearch(pattern);
    QCOMPARE( result, reference );
    
    pattern = QString("");
    reference = QString("");
    result = mSqlKoreanItuT->createInputSpecificSearch(pattern);
    TEST_PASSED_LOG("testCreateInputSpecificSearch");
}

void UT_CntSqlKoreanItuT::testGetSQLQueryType()
{
    TEST_BEGIN_LOG("testGetSQLQueryType");
    QString pattern("0");
    int result;
    int reference = CntSqlKoreanItuT::AllFromOneTable;
    result = mSqlKoreanItuT->getSQLQueryType(pattern);
    QCOMPARE( result, reference );
    
    pattern = QString("*");
    reference = CntSqlKoreanItuT::AllFromOneTable;
    result = mSqlKoreanItuT->getSQLQueryType(pattern);
    QCOMPARE( result, reference );
    
    pattern = QString("12");
    reference = CntSqlKoreanItuT:: KoreanBasicSearch;
    result = mSqlKoreanItuT->getSQLQueryType(pattern);
    QCOMPARE( result, reference );
    
    pattern = QString("255");
    reference = CntSqlKoreanItuT:: KoreanBasicSearch;
    result = mSqlKoreanItuT->getSQLQueryType(pattern);
    QCOMPARE( result, reference );
    
    pattern = QString("25");
    reference = CntSqlKoreanItuT:: KoreanBasicSearch;
    result = mSqlKoreanItuT->getSQLQueryType(pattern);
    QCOMPARE( result, reference );
    
    pattern = QString("101");
    reference = CntSqlKoreanItuT:: KoreanBasicSearch;
    result = mSqlKoreanItuT->getSQLQueryType(pattern);
    QCOMPARE( result, reference );
    
    pattern = QString("");
    reference = CntSqlKoreanItuT:: NA;
    result = mSqlKoreanItuT->getSQLQueryType(pattern);
    QCOMPARE( result, reference );
    
    pattern = QString("999");
    reference = CntSqlKoreanItuT:: ExactMatchFromOneTable;
    result = mSqlKoreanItuT->getSQLQueryType(pattern);
    QCOMPARE( result, reference );
    
    TEST_PASSED_LOG("testGetSQLQueryType");
}

void UT_CntSqlKoreanItuT::testBasicKoreanSearch()
    {
    TEST_BEGIN_LOG("testBasicKoreanSearch");
    QString pattern("255");
    QString result;
    QString reference = "SELECT contact_id FROM predictivesearch2 WHERE ((nbr>144115188075855871 AND nbr<216172782113783808) AND (nbr2>360287970189639679 AND nbr2<432345564227567616) AND (nbr3>360287970189639679 AND nbr3<432345564227567616)) OR ((nbr2>144115188075855871 AND nbr2<216172782113783808) AND (nbr3>360287970189639679 AND nbr3<432345564227567616) AND (nbr4>360287970189639679 AND nbr4<432345564227567616)) OR ((nbr3>144115188075855871 AND nbr3<216172782113783808) AND (nbr4>360287970189639679 AND nbr4<432345564227567616)) OR (nbr4>144115188075855871 AND nbr4<216172782113783808) ORDER BY first_name, last_name ASC;"; 
    result = mSqlKoreanItuT->basicKoreanSearch(pattern);
    QCOMPARE( result, reference );
    pattern = QString("87*"); 
    reference = QString("SELECT contact_id FROM predictivesearch8 WHERE ((nbr>576460752303423487 AND nbr<648518346341351424) AND (nbr2>504403158265495551 AND nbr2<576460752303423488)) OR ((nbr2>576460752303423487 AND nbr2<648518346341351424) AND (nbr3>504403158265495551 AND nbr3<576460752303423488)) OR ((nbr3>576460752303423487 AND nbr3<648518346341351424) AND (nbr4>504403158265495551 AND nbr4<576460752303423488)) OR (nbr4>576460752303423487 AND nbr4<648518346341351424) ORDER BY first_name, last_name ASC;");
    result = mSqlKoreanItuT->basicKoreanSearch(pattern);
    QCOMPARE( result, reference );
    pattern = QString("2"); 
    result = mSqlKoreanItuT->basicKoreanSearch(pattern);
    QVERIFY( result.isEmpty());
    TEST_PASSED_LOG("testBasicKoreanSearch");
    }

void UT_CntSqlKoreanItuT::testCompareColumnsInOrder()
    {
    TEST_BEGIN_LOG("testCompareColumnsInOrder");
    QStringList pattern;
    pattern << "2" << "5";
    QString reference = "(nbr>144115188075855871 AND nbr<216172782113783808) AND (nbr2>360287970189639679 AND nbr2<432345564227567616)";
    //qDebug() << pattern << "-> result" << result;
    QString result = mSqlKoreanItuT->compareColumnsInOrder(pattern);
    QCOMPARE( result, reference );
    
    pattern.clear();
    pattern << "2" << "5" << "5";
    reference = QString("(nbr>144115188075855871 AND nbr<216172782113783808) AND (nbr2>360287970189639679 AND nbr2<432345564227567616) AND (nbr3>360287970189639679 AND nbr3<432345564227567616)");
    result = mSqlKoreanItuT->compareColumnsInOrder(pattern);
    QCOMPARE( result, reference );
    
    pattern.clear();
    pattern << "2" << "5" << "5" << "5";
    reference = QString("(nbr>144115188075855871 AND nbr<216172782113783808) AND (nbr2>360287970189639679 AND nbr2<432345564227567616) AND (nbr3>360287970189639679 AND nbr3<432345564227567616) AND (nbr4>360287970189639679 AND nbr4<432345564227567616)");
    result = mSqlKoreanItuT->compareColumnsInOrder(pattern);
    QCOMPARE( result, reference );
    
    pattern.clear();
    pattern << "2" << "5" << "5" << "5" << "6";
    reference = QString("(nbr>144115188075855871 AND nbr<216172782113783808) AND (nbr2>360287970189639679 AND nbr2<432345564227567616) AND (nbr3>360287970189639679 AND nbr3<432345564227567616) AND (nbr4>360287970189639679 AND nbr4<432345564227567616)");
    result = mSqlKoreanItuT->compareColumnsInOrder(pattern);
    QCOMPARE( result, reference );
    
    pattern.clear();
    pattern << "2";
    result = mSqlKoreanItuT->compareColumnsInOrder(pattern);
    QVERIFY( result.isEmpty());
    
    TEST_PASSED_LOG("testCompareColumnsInOrder");
    }

void UT_CntSqlKoreanItuT::testCompareColumnsFromMidleInOrder()
    {
    TEST_BEGIN_LOG("testCompareColumnsFromMidleInOrder");
    QStringList pattern;
    pattern << "2" << "5";
    QString reference = "((nbr2>144115188075855871 AND nbr2<216172782113783808) AND (nbr3>360287970189639679 AND nbr3<432345564227567616)) OR ((nbr3>144115188075855871 AND nbr3<216172782113783808) AND (nbr4>360287970189639679 AND nbr4<432345564227567616)) OR (nbr4>144115188075855871 AND nbr4<216172782113783808)";
    //qDebug() << pattern << "-> result" << result;
    QString result = mSqlKoreanItuT->compareColumnsFromMidleInOrder(pattern);
    QCOMPARE( result, reference );
    
    pattern.clear();
    pattern << "2" << "5" << "5";
    reference = QString("((nbr2>144115188075855871 AND nbr2<216172782113783808) AND (nbr3>360287970189639679 AND nbr3<432345564227567616) AND (nbr4>360287970189639679 AND nbr4<432345564227567616)) OR ((nbr3>144115188075855871 AND nbr3<216172782113783808) AND (nbr4>360287970189639679 AND nbr4<432345564227567616)) OR (nbr4>144115188075855871 AND nbr4<216172782113783808)");
    //qDebug() << pattern << "-> result" << result;
    result = mSqlKoreanItuT->compareColumnsFromMidleInOrder(pattern);
    QCOMPARE( result, reference );
    
    pattern.clear();
    pattern << "2" << "5" << "5" << "5";
    reference = QString("((nbr2>144115188075855871 AND nbr2<216172782113783808) AND (nbr3>360287970189639679 AND nbr3<432345564227567616) AND (nbr4>360287970189639679 AND nbr4<432345564227567616)) OR ((nbr3>144115188075855871 AND nbr3<216172782113783808) AND (nbr4>360287970189639679 AND nbr4<432345564227567616)) OR (nbr4>144115188075855871 AND nbr4<216172782113783808)");
    //qDebug() << pattern << "-> result" << result;
    result = mSqlKoreanItuT->compareColumnsFromMidleInOrder(pattern);
    QCOMPARE( result, reference );
    
    pattern.clear();
    pattern << "2" << "5" << "5" << "5" << "5";
    reference = QString("((nbr2>144115188075855871 AND nbr2<216172782113783808) AND (nbr3>360287970189639679 AND nbr3<432345564227567616) AND (nbr4>360287970189639679 AND nbr4<432345564227567616)) OR ((nbr3>144115188075855871 AND nbr3<216172782113783808) AND (nbr4>360287970189639679 AND nbr4<432345564227567616)) OR (nbr4>144115188075855871 AND nbr4<216172782113783808)");
    //qDebug() << pattern << "-> result" << result;
    result = mSqlKoreanItuT->compareColumnsFromMidleInOrder(pattern);
    QCOMPARE( result, reference );
    TEST_PASSED_LOG("testCompareColumnsFromMidleInOrder");
    }
void UT_CntSqlKoreanItuT::testGetSearchColumns()
    {
    TEST_BEGIN_LOG("testGetSearchColumns");
    QString pattern("2");
    int position = 0;
    QString result;
    QString reference = "(nbr>144115188075855871 AND nbr<216172782113783808)"; 
    result = mSqlKoreanItuT->getSearchColumns(pattern, position);
    QCOMPARE( result, reference );
    
    pattern = QString("5");
    position = 1;
    reference = QString("(nbr2>360287970189639679 AND nbr2<432345564227567616)");
    result = mSqlKoreanItuT->getSearchColumns(pattern, position);
    QCOMPARE( result, reference );
    
    pattern = QString("5");
    position = 2;
    reference = QString("(nbr3>360287970189639679 AND nbr3<432345564227567616)");
    result = mSqlKoreanItuT->getSearchColumns(pattern, position);
    QCOMPARE( result, reference ); 
    
    pattern = QString("5");
    position = 3;
    reference = QString("(nbr4>360287970189639679 AND nbr4<432345564227567616)");
    result = mSqlKoreanItuT->getSearchColumns(pattern, position);
    QCOMPARE( result, reference ); 
    
    position = 1;
    pattern = QString("7#");
    result = mSqlKoreanItuT->getSearchColumns(pattern, position);
    //qDebug() << pattern << "-> result" << result;
    reference = QString("(nbr2>553942754166571007 AND nbr2<558446353793941504)");
    QCOMPARE( result, reference ); 
    
    position = 1;
    pattern = QString("7**");
    result = mSqlKoreanItuT->getSearchColumns(pattern, position);
    reference = QString("(nbr2>552253904306307071 AND nbr2<552535379283017728)");
    QCOMPARE( result, reference ); 
    
    position = 1;
    pattern = QString("7*");
    result = mSqlKoreanItuT->getSearchColumns(pattern, position);
    reference = QString("(nbr2>549439154539200511 AND nbr2<553942754166571008)");
    QCOMPARE( result, reference ); 
    
    position = 4;
    pattern = QString("5");
    result = mSqlKoreanItuT->getSearchColumns(pattern, position);
    QVERIFY( result.isEmpty()); 
    
    position = 5;
    pattern = QString("5555");
    result = mSqlKoreanItuT->getSearchColumns(pattern, position);
    QVERIFY( result.isEmpty()); 
        
    TEST_PASSED_LOG("testGetSearchColumns");
    }

void UT_CntSqlKoreanItuT::testGetSearchPattern()
    {
    TEST_BEGIN_LOG("testGetSearchPattern");
    //TODO Remove zero spliter after search patter can be tokenize
    QString pattern("20");
    QStringList result;
    QStringList reference;
    reference << "20";
    result = mSqlKoreanItuT->getSearchPattern(pattern);
    //QCOMPARE( result, reference );
    
    reference.clear();
    pattern = QString("2226");
    reference << "2" << "2" << "26";
    result = mSqlKoreanItuT->getSearchPattern(pattern);
    //qDebug() << pattern << "-> result 1" << result.at(0) << "-> result 2" << result.at(1) << "-> result 3" << result.at(2);
    QCOMPARE( result, reference );
    
    reference.clear();
    pattern = QString("2622");
    reference << "2622";
    result = mSqlKoreanItuT->getSearchPattern(pattern);
    //qDebug() << pattern << "-> result 1" << result.at(0) ; //<< "-> result 2" << result.at(1);
    QCOMPARE( result, reference );
    
    reference.clear();
    pattern = QString("1398");
    reference << "138";
    result = mSqlKoreanItuT->getSearchPattern(pattern);
    //qDebug() << pattern << "-> result 1" << result.at(0);
    QCOMPARE( result, reference );
    
    reference.clear();
    pattern = QString("132264");
    reference << "132" << "264" ;
    result = mSqlKoreanItuT->getSearchPattern(pattern);
    //qDebug() << pattern << "-> result 1" << result.at(0);// << " -> result 2" << result.at(1);
    QCOMPARE( result, reference );
    
    reference.clear();
    pattern = QString("13226");
    reference << "132" << "26" ;
    result = mSqlKoreanItuT->getSearchPattern(pattern);
    //qDebug() << pattern << "-> result 1" << result.at(0) << " -> result 2" << result.at(1);
    QCOMPARE( result, reference );
    
    reference.clear();
    pattern = QString("132226");
    reference << "1322" << "26" ;
    result = mSqlKoreanItuT->getSearchPattern(pattern);
    //qDebug() << pattern << "-> result 1" << result.at(0)<< " -> result 2" << result.at(1);
    QCOMPARE( result, reference );
    
    reference.clear();
    pattern = QString("261632");
    reference << "26" << "162" ;
    result = mSqlKoreanItuT->getSearchPattern(pattern);
    //qDebug() << pattern << "-> result 1" << result.at(0) << " -> result 2" << result.at(1);
    QCOMPARE( result, reference );
    
    reference.clear();
    pattern = QString("12457680");
    reference << "1" << "2" << "4" << "5" << "76" << "80";
    result = mSqlKoreanItuT->getSearchPattern(pattern);
    //qDebug() << pattern << "-> result 1" << result.at(0)<< " -> result 2" << result.at(1);
    QCOMPARE( result, reference );
    
    reference.clear();
    pattern = QString("26457580");
    reference << "2645" << "7" << "5" << "80";
    result = mSqlKoreanItuT->getSearchPattern(pattern);
    //qDebug() << pattern << "-> result 1" << result.at(0)<< " -> result 2" << result.at(1) << " -> result 3" << result.at(2) << " -> result 4" << result.at(3);
    QCOMPARE( result, reference );
    
    reference.clear();
    pattern = QString("45726580");
    reference <<  "4" << "5" << "7" << "265" << "80";
    result = mSqlKoreanItuT->getSearchPattern(pattern);
    //qDebug() << pattern << "-> result 1" << result.at(0)<< " -> result 2" << result.at(1);
    QCOMPARE( result, reference );
    
    reference.clear();
    pattern = QString("25");
    reference << "2" << "5";
    result = mSqlKoreanItuT->getSearchPattern(pattern);
    QCOMPARE( result, reference );
    
    reference.clear();
    pattern = QString("255");
    reference << "2" << "5" << "5";
    result = mSqlKoreanItuT->getSearchPattern(pattern);
    QCOMPARE( result, reference );
    
    reference.clear();
    pattern = QString("132465");
    reference << "132" << "465" ;
    result = mSqlKoreanItuT->getSearchPattern(pattern);
    QCOMPARE( result, reference );
    
    reference.clear();
    pattern = QString("369");
    reference << "369";
    result = mSqlKoreanItuT->getSearchPattern(pattern);
    QCOMPARE( result, reference );
    
    reference.clear();
    pattern = QString("3");
    reference << "3";
    result = mSqlKoreanItuT->getSearchPattern(pattern);
    QCOMPARE( result, reference );
    
    reference.clear();
    pattern = QString("2626");
    reference << "26" << "26";
    result = mSqlKoreanItuT->getSearchPattern(pattern);
    QCOMPARE( result, reference );
    
    reference.clear();
    pattern = QString("26226");
    reference << "262" << "26";
    result = mSqlKoreanItuT->getSearchPattern(pattern);
    QCOMPARE( result, reference );
    
    reference.clear();
    pattern = QString("26222");
    reference << "2622" << "2";
    result = mSqlKoreanItuT->getSearchPattern(pattern);
    QCOMPARE( result, reference );
    
    reference.clear();
    pattern = QString("266222");
    reference << "26622" << "2";
    result = mSqlKoreanItuT->getSearchPattern(pattern);
    QCOMPARE( result, reference );
    
    reference.clear();
    pattern = QString("26622266");
    reference << "26622" << "266";
    result = mSqlKoreanItuT->getSearchPattern(pattern);
    QCOMPARE( result, reference );
    
    reference.clear();
    pattern = QString("26622266222");
    reference << "26622" << "26622" << "2";
    result = mSqlKoreanItuT->getSearchPattern(pattern);
    QCOMPARE( result, reference );
    
    reference.clear();
    pattern = QString("26622226622");
    reference << "26622" << "2" << "26622";
    result = mSqlKoreanItuT->getSearchPattern(pattern);
    QCOMPARE( result, reference );
    
    reference.clear();
    pattern = QString("266266");
    reference << "266" << "266";
    result = mSqlKoreanItuT->getSearchPattern(pattern);
    QCOMPARE( result, reference );
    
    reference.clear();
    pattern = QString("26666");
    reference << "26666";
    result = mSqlKoreanItuT->getSearchPattern(pattern);
    QCOMPARE( result, reference );
    
    reference.clear();
    pattern = QString("6666");
    reference << "6666";
    result = mSqlKoreanItuT->getSearchPattern(pattern);
    QCOMPARE( result, reference );
    
    TEST_PASSED_LOG("testGetSearchPattern");
    }

void UT_CntSqlKoreanItuT::testVariationRemoval()
    {
    TEST_BEGIN_LOG("testGetSearchPattern");
    QString pattern("39");
    QString result;
    QString reference("3");
    result = mSqlKoreanItuT->variationRemoval(pattern);
    QCOMPARE( result, reference );
    pattern =QString("1239");
    reference = QString("123");
    result = mSqlKoreanItuT->variationRemoval(pattern);
    QCOMPARE( result, reference );
    pattern =QString("3912");
    reference = QString("312");
    result = mSqlKoreanItuT->variationRemoval(pattern);
    QCOMPARE( result, reference );
    pattern =QString("39");
    reference = QString("3");
    result = mSqlKoreanItuT->variationRemoval(pattern);
    QCOMPARE( result, reference );
    pattern =QString("3393");
    reference = QString("333");
    result = mSqlKoreanItuT->variationRemoval(pattern);
    QCOMPARE( result, reference );
    pattern =QString("639");
    reference = QString("6");
    result = mSqlKoreanItuT->variationRemoval(pattern);
    QCOMPARE( result, reference );
    pattern =QString("639");
    reference = QString("6");
    result = mSqlKoreanItuT->variationRemoval(pattern);
    QCOMPARE( result, reference );
    pattern =QString("639");
    reference = QString("6");
    result = mSqlKoreanItuT->variationRemoval(pattern);
    QCOMPARE( result, reference );
    pattern =QString("6639");
    reference = QString("66");
    result = mSqlKoreanItuT->variationRemoval(pattern);
    QCOMPARE( result, reference );
    pattern =QString("6639");
    reference = QString("66");
    result = mSqlKoreanItuT->variationRemoval(pattern);
    QCOMPARE( result, reference );
    pattern =QString("6639");
    reference = QString("66");
    result = mSqlKoreanItuT->variationRemoval(pattern);
    QCOMPARE( result, reference );
    pattern =QString("3963");
    reference = QString("36");
    result = mSqlKoreanItuT->variationRemoval(pattern);
    QCOMPARE( result, reference );
    pattern =QString("0396312");
    reference = QString("03612");
    result = mSqlKoreanItuT->variationRemoval(pattern);
    QCOMPARE( result, reference );
    pattern =QString("039666312");
    reference = QString("0366612");
    result = mSqlKoreanItuT->variationRemoval(pattern);
    QCOMPARE( result, reference );
    reference.clear();
    pattern = QString("87*");
    reference = QString("87");
    result = mSqlKoreanItuT->variationRemoval(pattern);
    QCOMPARE( result, reference );
    
    reference.clear();
    pattern = QString("8*#7*");
    reference = QString("87");
    result = mSqlKoreanItuT->variationRemoval(pattern);
    QCOMPARE( result, reference );
    TEST_PASSED_LOG("testGetSearchPattern");
    }
