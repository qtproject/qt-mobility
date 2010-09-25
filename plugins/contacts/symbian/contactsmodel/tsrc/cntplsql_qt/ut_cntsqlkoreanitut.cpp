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
    mTwelveKeyMap = new C12keyKeyMap();
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
    reference = QString("SELECT contact_id FROM predictivesearch2 WHERE (NOT(NOT(nbr>166633186212708351 AND nbr<171136785840078848) AND NOT(nbr2>166633186212708351 AND nbr2<171136785840078848) AND NOT(nbr3>166633186212708351 AND nbr3<171136785840078848) AND NOT(nbr4>166633186212708351 AND nbr4<171136785840078848))) ORDER BY first_name, last_name ASC;");
    result = mSqlKoreanItuT->createInputSpecificSearch(pattern);
    //qDebug() << pattern << " -> result" << result;
    QVERIFY(!result.compare( reference));    
    
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
    reference = CntSqlKoreanItuT:: ExactMatchFromOneTable;
    result = mSqlKoreanItuT->getSQLQueryType(pattern);
    QCOMPARE( result, reference );
    
    pattern = QString("001");
    reference = CntSqlKoreanItuT:: ExactMatchFromOneTable;
    result = mSqlKoreanItuT->getSQLQueryType(pattern);
    QCOMPARE( result, reference );
    
    pattern = QString("");
    reference = CntSqlKoreanItuT:: NA;
    result = mSqlKoreanItuT->getSQLQueryType(pattern);
    QCOMPARE( result, reference );
    
    TEST_PASSED_LOG("testGetSQLQueryType");
}
