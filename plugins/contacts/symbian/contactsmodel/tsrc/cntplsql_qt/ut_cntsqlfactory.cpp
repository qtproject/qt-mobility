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
#include <QStringList>
#include <QLocale>
#include <sqldb.h>

#include "ut_cntsqlfactory.h"
#include "cntsqlfactory.h"
#include "cntsqlsearchinterface.h"
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


void UT_CntSqlFactory::initTestCase()
{
}

void UT_CntSqlFactory::cleanupTestCase()
{
}


void UT_CntSqlFactory::init()
{   
    m12KeyKeyMap = C12keyKeyMap::NewL();
    mCntSqlFactory = new CntSqlFactory(m12KeyKeyMap);
}

void UT_CntSqlFactory::cleanup()
{
    delete m12KeyKeyMap;
    delete mCntSqlFactory;    
}
void UT_CntSqlFactory::testGetSqlIinstance()
{
    TEST_BEGIN_LOG("testGetSqlIinstance");
    
    QLocale language = QLocale ( QLocale::Korean, QLocale::RepublicOfKorea);
    CntSqlSearchInterface* interface = mCntSqlFactory->getSqlIinstance(language);
    //qDebug() << " -> result" << result;
    QVERIFY(interface->getQueryType() == CntSqlSearchInterface::KoreaITUT);
    delete interface;
    language = QLocale ( QLocale::English, QLocale::UnitedKingdom);
    interface = mCntSqlFactory->getSqlIinstance(language);
    QVERIFY(interface == NULL);
    TEST_PASSED_LOG("testGetSqlIinstance");
}


