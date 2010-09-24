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
    mSqlKoreanItuT = new CntSqlKoreanItuT();
}

void UT_CntSqlKoreanItuT::cleanup()
{
    delete mSqlKoreanItuT;    
}
void UT_CntSqlKoreanItuT::testCreateInputSpecificSearch()
{
    TEST_BEGIN_LOG("testCreateInputSpecificSearch");
    
    TEST_PASSED_LOG("testCreateInputSpecificSearch");
}


