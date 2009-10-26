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

#include "ut_sqlfiltering.h"
#include "cntsymbianfiltersqlhelper.h"

#include <qtcontacts.h>
#include <QtTest/QtTest>

void TestSqlFiltering::initTestCase()
{
    mCntMng = new QContactManager("symbian");
    mSqlFilter = new CntSymbianFilterSqlHelper();
    
    // Remove all contacts from the database
    QList<QContactLocalId> cnt_ids = mCntMng->contacts();
    mCntMng->removeContacts(&cnt_ids);
}

void TestSqlFiltering::cleanupTestCase()
{
    delete mCntMng;
    delete mSqlFilter;
}

void TestSqlFiltering::testInvalidFilter()
{
    QList<QContactLocalId> cnt_ids;
    QContactFilter filter;
    QContactManager::Error error;
    
    QBENCHMARK {
        cnt_ids = mSqlFilter->searchContacts(filter, error);
    }
    QVERIFY(error == QContactManager::NotSupportedError);
}

void TestSqlFiltering::testContactDetailFilter()
{
    QList<QContactLocalId> cnt_ids;
    QContactDetailFilter filter;
    QContactManager::Error error;
    
    QBENCHMARK {
        cnt_ids = mSqlFilter->searchContacts(filter, error);
    }
    QVERIFY(error == QContactManager::NoError);
}

void TestSqlFiltering::testContactDetailRangeFilter()
{
    QList<QContactLocalId> cnt_ids;
    QContactDetailRangeFilter filter;
    QContactManager::Error error;
    
    QBENCHMARK {
        cnt_ids = mSqlFilter->searchContacts(filter, error);
    }
    QVERIFY(error == QContactManager::NotSupportedError);
}

void TestSqlFiltering::testChangeLogFilter()
{
    QList<QContactLocalId> cnt_ids;
    QContactChangeLogFilter filter;
    QContactManager::Error error;
    
    QBENCHMARK {
        cnt_ids = mSqlFilter->searchContacts(filter, error);
    }
    QVERIFY(error == QContactManager::NotSupportedError);
}

void TestSqlFiltering::testActionFilter()
{
    QList<QContactLocalId> cnt_ids;
    QContactActionFilter filter;
    QContactManager::Error error;
    
    QBENCHMARK {
        cnt_ids = mSqlFilter->searchContacts(filter, error);
    }    
    QVERIFY(error == QContactManager::NotSupportedError);
}

void TestSqlFiltering::testRelationshipFilter()
{
    QList<QContactLocalId> cnt_ids;
    QContactRelationshipFilter filter;
    QContactManager::Error error;
    
    QBENCHMARK {
        cnt_ids = mSqlFilter->searchContacts(filter, error);
    }
    QVERIFY(error == QContactManager::NotSupportedError);
}

void TestSqlFiltering::testIntersectionFilter()
{
    QList<QContactLocalId> cnt_ids;
    QContactIntersectionFilter filter;
    QContactManager::Error error;
    
    QBENCHMARK {
        cnt_ids = mSqlFilter->searchContacts(filter, error);
    }
    QVERIFY(error == QContactManager::NotSupportedError);
}

void TestSqlFiltering::testUnionFilter()
{
    QList<QContactLocalId> cnt_ids;
    QContactUnionFilter filter;
    QContactManager::Error error;
    
    QBENCHMARK {
        cnt_ids = mSqlFilter->searchContacts(filter, error);
    }
    QVERIFY(error == QContactManager::NotSupportedError);
}

void TestSqlFiltering::testLocalIdFilter()
{
    QList<QContactLocalId> cnt_ids;
    QContactLocalIdFilter filter;
    QContactManager::Error error;
    
    QBENCHMARK {
        cnt_ids = mSqlFilter->searchContacts(filter, error);
    }
    QVERIFY(error == QContactManager::NotSupportedError);
}

void TestSqlFiltering::testDefaultFilter()
{
    QList<QContactLocalId> cnt_ids;
    QContactFilter filter;
    QContactManager::Error error;
    
    QBENCHMARK {
        cnt_ids = mSqlFilter->searchContacts(filter, error);
    }
    QVERIFY(error == QContactManager::NotSupportedError);
}

QTEST_MAIN(TestSqlFiltering);
