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

#include "ut_cntsqlsearchbase.h"
#include "cntsqlsearchbase.h"
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


void UT_CntSqlSearchBase::initTestCase()
{
}

void UT_CntSqlSearchBase::cleanupTestCase()
{
}


void UT_CntSqlSearchBase::init()
{   
    mTwelveKeyMap = new C12keyKeyMap();
    mSqlSearchBase = new CntSqlSearchBase(mTwelveKeyMap);
}

void UT_CntSqlSearchBase::cleanup()
{
    delete mTwelveKeyMap;
    delete mSqlSearchBase;    
}

void UT_CntSqlSearchBase::testExactMatch()
    {
    TEST_BEGIN_LOG("testExactMatch");
    QString pattern("25");
    QString reference("(NOT(NOT(nbr>166633186212708351 AND nbr<171136785840078848) AND NOT(nbr2>166633186212708351 AND nbr2<171136785840078848) AND NOT(nbr3>166633186212708351 AND nbr3<171136785840078848) AND NOT(nbr4>166633186212708351 AND nbr4<171136785840078848)))");
    QString result = mSqlSearchBase->exactMatch(pattern);
    QVERIFY(!result.compare(reference));    
    TEST_PASSED_LOG("testExactMatch");
    }
   
void UT_CntSqlSearchBase::testExactMatchSearch()
    {
    TEST_BEGIN_LOG("testExactMatchSearch");
    QString pattern = QString("25");
    QString reference = QString("SELECT contact_id FROM predictivesearch2 WHERE (NOT(NOT(nbr>166633186212708351 AND nbr<171136785840078848) AND NOT(nbr2>166633186212708351 AND nbr2<171136785840078848) AND NOT(nbr3>166633186212708351 AND nbr3<171136785840078848) AND NOT(nbr4>166633186212708351 AND nbr4<171136785840078848))) ORDER BY first_name, last_name ASC;");
    QString result = mSqlSearchBase->exactMatchSearch(pattern);
    QVERIFY(!result.compare( reference));    
    TEST_PASSED_LOG("testExactMatchSearch");
    }
   

void UT_CntSqlSearchBase::testReturnAllcontacts()
{
    TEST_BEGIN_LOG("testReturnAllcontacts");
    QString pattern = "3";
    QString reference = "SELECT contact_id FROM predictivesearch3 ORDER BY first_name, last_name ASC;";
    QString result = mSqlSearchBase->returnAllcontacts(pattern);
    qDebug() << pattern << " -> result" << result;
    LOG2(pattern, result);
    QVERIFY( !result.compare( reference) );
    TEST_PASSED_LOG("testReturnAllcontacts");
}

void UT_CntSqlSearchBase::testSelectTable()
{
    TEST_BEGIN_LOG("testSelectTable");

    TEST_PASSED_LOG("testSelectTable");
}
