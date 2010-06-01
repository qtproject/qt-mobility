/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>
#include <QStringList>

#include "ut_cntsqlsearch.h"
#include "cntsqlsearch.h"


void UT_CntSqlSearch::initTestCase()
{
}

void UT_CntSqlSearch::cleanupTestCase()
{
}


void UT_CntSqlSearch::init()
{
mCntSqlSearch = new CntSqlSearch();
}

void UT_CntSqlSearch::cleanup()
{
    delete mCntSqlSearch;
    mCntSqlSearch = 0;
    
}
void UT_CntSqlSearch::testPredictiveSearch()
{
    QString pattern = QString("209");
    QString result;
    QString reference("SELECT contact_id FROM view2 WHERE (first_name_as_number LIKE % 209%) OR (last_name_as_number LIKE % 209%) OR (last_name_as_number LIKE % 2%) OR ((first_name_as_number LIKE % 2%) AND (first_name_as_number LIKE %_ 2%)) OR ((last_name_as_number LIKE % 9%) AND (last_name_as_number LIKE %_ 9%)) ORDER BY first_name_as_number ASC");
   /* result = mCntSqlSearch->CreatePredictiveSearch(pattern);
    QVERIFY( !result.compare( reference) );*/

    pattern = QString("3");
    reference = QString("SELECT contact_id FROM view3 ORDER BY first_name_as_number ASC;");
    result = mCntSqlSearch->CreatePredictiveSearch(pattern);
    QVERIFY( !result.compare( reference) );

    pattern = QString("33");
    reference = QString("SELECT contact_id FROM view3 WHERE (first_name_as_number LIKE % 33%) OR (last_name_as_number LIKE % 33%) ORDER BY first_name_as_number ASC;");
    result = mCntSqlSearch->CreatePredictiveSearch(pattern);
    QVERIFY( !result.compare( reference) );

    pattern = QString("001100");
    reference = QString("SELECT contact_id FROM view0 WHERE (first_name_as_number LIKE % 001100%) OR (last_name_as_number LIKE % 001100%) ORDER BY first_name_as_number ASC;");
    result = mCntSqlSearch->CreatePredictiveSearch(pattern);
    QVERIFY( !result.compare( reference) );

    pattern = QString("1");
    reference = QString("SELECT contact_id FROM view1 ORDER BY first_name_as_number ASC;");
    result = mCntSqlSearch->CreatePredictiveSearch(pattern);
    QVERIFY( !result.compare( reference) ); pattern = QString("1");
}

void UT_CntSqlSearch::testSelectTableView()
{
    QString pattern = QString("0");
    QString reference = QString("view0");;
    QString result = mCntSqlSearch->SelectTableView(pattern);
    QVERIFY( !result.compare( reference) );

    pattern = QString("1");
    reference = QString("view1");;
    result = mCntSqlSearch->SelectTableView(pattern);
    QVERIFY( !result.compare( reference) );

    pattern = QString("2");
    reference = QString("view2");;
    result = mCntSqlSearch->SelectTableView(pattern);
    QVERIFY( !result.compare( reference) );

    pattern = QString("3");
    reference = QString("view3");;
    result = mCntSqlSearch->SelectTableView(pattern);
    QVERIFY( !result.compare( reference) );

    pattern = QString("4");
    reference = QString("view4");;
    result = mCntSqlSearch->SelectTableView(pattern);
    QVERIFY( !result.compare( reference) );

    pattern = QString("5");
    reference = QString("view5");;
    result = mCntSqlSearch->SelectTableView(pattern);
    QVERIFY( !result.compare( reference) );

    pattern = QString("6");
    reference = QString("view6");;
    result = mCntSqlSearch->SelectTableView(pattern);
    QVERIFY( !result.compare( reference) );

    pattern = QString("7");
    reference = QString("view7");;
    result = mCntSqlSearch->SelectTableView(pattern);
    QVERIFY( !result.compare( reference) );

    pattern = QString("8");
    reference = QString("view8");;
    result = mCntSqlSearch->SelectTableView(pattern);
    QVERIFY( !result.compare( reference) );

    pattern = QString("9");
    reference = QString("view9");;
    result = mCntSqlSearch->SelectTableView(pattern);
    QVERIFY( !result.compare( reference) );
}

void UT_CntSqlSearch::testIsSubStringSearch()
{
    QString pattern("102");
    QCOMPARE(mCntSqlSearch->IsSubStringSearch(pattern), true);
    pattern = QString("12");
    QCOMPARE(mCntSqlSearch->IsSubStringSearch(pattern), false);
    pattern = QString("1");
    QCOMPARE(mCntSqlSearch->IsSubStringSearch(pattern), false);
    pattern = QString("01");
    QCOMPARE(mCntSqlSearch->IsSubStringSearch(pattern), true);
    pattern = QString("012");
    QCOMPARE(mCntSqlSearch->IsSubStringSearch(pattern), true);
    pattern = QString("00012");
    QCOMPARE(mCntSqlSearch->IsSubStringSearch(pattern), true);
    pattern = QString("000100002");
    QCOMPARE(mCntSqlSearch->IsSubStringSearch(pattern), true);
    pattern = QString("000");
    QCOMPARE(mCntSqlSearch->IsSubStringSearch(pattern), false);
    pattern = QString("0");
    QCOMPARE(mCntSqlSearch->IsSubStringSearch(pattern), false);
}

void UT_CntSqlSearch::testGetNumber()
{   
    QStringList numbers;

    QString pattern("102");
    numbers = mCntSqlSearch->GetNumber(pattern);
    QCOMPARE(numbers.at(0), QString("1"));
    QCOMPARE(numbers.at(1), QString("2"));

    pattern = QString("12");
    numbers = mCntSqlSearch->GetNumber(pattern);
    QCOMPARE(numbers.at(0), QString("12"));

    pattern = QString("01");
    numbers = mCntSqlSearch->GetNumber(pattern);
    QCOMPARE(numbers.at(0), QString("1"));

    pattern = QString("012");
    numbers = mCntSqlSearch->GetNumber(pattern);
    QCOMPARE(numbers.at(0), QString("12"));

    pattern = QString("00012");
    numbers = mCntSqlSearch->GetNumber(pattern);
    QCOMPARE(numbers.at(0), QString("12"));

    pattern = QString("00012000");
    numbers = mCntSqlSearch->GetNumber(pattern);
    QCOMPARE(numbers.at(0), QString("12"));

    pattern = QString("000100002");
    numbers = mCntSqlSearch->GetNumber(pattern);
    QCOMPARE(numbers.at(0), QString("1"));
    QCOMPARE(numbers.at(1), QString("2"));

    pattern = QString("000100002000");
    numbers = mCntSqlSearch->GetNumber(pattern);
    QCOMPARE(numbers.at(0), QString("1"));
    QCOMPARE(numbers.at(1), QString("2"));
}

void UT_CntSqlSearch::testCreateSubStringSearch()
{
    QString pattern = QString("102");
    QString result;
    QString reference("(first_name_as_number LIKE % 102%) OR (last_name_as_number LIKE % 102%) OR ((first_name_as_number LIKE % 1%) AND (first_name_as_number LIKE %_ 1%)) OR ((last_name_as_number LIKE % 1%) AND (last_name_as_number LIKE %_ 1%)) OR ((first_name_as_number LIKE % 2%) AND (first_name_as_number LIKE %_ 2%)) OR ((last_name_as_number LIKE % 2%) AND (last_name_as_number LIKE %_ 2%))");
    result = mCntSqlSearch->CreateSubStringSearch(pattern);
    QVERIFY( !result.compare( reference) );

    pattern = QString("00102");
    reference = QString("(first_name_as_number LIKE % 00102%) OR (last_name_as_number LIKE % 00102%) OR ((first_name_as_number LIKE % 1%) AND (first_name_as_number LIKE %_ 1%)) OR ((last_name_as_number LIKE % 1%) AND (last_name_as_number LIKE %_ 1%)) OR ((first_name_as_number LIKE % 2%) AND (first_name_as_number LIKE %_ 2%)) OR ((last_name_as_number LIKE % 2%) AND (last_name_as_number LIKE %_ 2%))");
    result = mCntSqlSearch->CreateSubStringSearch(pattern);
    QVERIFY( !result.compare( reference) );

    pattern = QString("0010200");
    reference = QString("(first_name_as_number LIKE % 0010200%) OR (last_name_as_number LIKE % 0010200%) OR ((first_name_as_number LIKE % 1%) AND (first_name_as_number LIKE %_ 1%)) OR ((last_name_as_number LIKE % 1%) AND (last_name_as_number LIKE %_ 1%)) OR ((first_name_as_number LIKE % 2%) AND (first_name_as_number LIKE %_ 2%)) OR ((last_name_as_number LIKE % 2%) AND (last_name_as_number LIKE %_ 2%))");
    result = mCntSqlSearch->CreateSubStringSearch(pattern);
    QVERIFY( !result.compare( reference) );

    pattern = QString("12");
    reference = QString("(first_name_as_number LIKE % 12%) OR (last_name_as_number LIKE % 12%)");
    result = mCntSqlSearch->CreateSubStringSearch(pattern);
    QVERIFY( !result.compare( reference) );

    pattern = QString("000");
    reference = QString("(first_name_as_number LIKE % 000%) OR (last_name_as_number LIKE % 000%)");
    result = mCntSqlSearch->CreateSubStringSearch(pattern);
    QVERIFY( !result.compare( reference) );
}

void UT_CntSqlSearch::testCreateStringSearch()
{
    QString numbers("102");

    QString result;
    result = mCntSqlSearch->CreateStringSearch(numbers);
    QVERIFY( result == "(first_name_as_number LIKE % 102%) OR (last_name_as_number LIKE % 102%)" );

    numbers = QString("00012");
    result = mCntSqlSearch->CreateStringSearch(numbers);
    QVERIFY( result == "(first_name_as_number LIKE % 00012%) OR (last_name_as_number LIKE % 00012%)" );

    numbers = QString("000120012");
    result = mCntSqlSearch->CreateStringSearch(numbers);
    QVERIFY( result == "(first_name_as_number LIKE % 000120012%) OR (last_name_as_number LIKE % 000120012%)" );

    numbers = QString("12");
    result = mCntSqlSearch->CreateStringSearch(numbers);
    QVERIFY( result == "(first_name_as_number LIKE % 12%) OR (last_name_as_number LIKE % 12%)" );

    numbers = QString("00012001200");
    result = mCntSqlSearch->CreateStringSearch(numbers);
    QVERIFY( result == "(first_name_as_number LIKE % 00012001200%) OR (last_name_as_number LIKE % 00012001200%)" );
}

void UT_CntSqlSearch::testCreateSpaceStringSearch()
{
    QStringList numbers;
    numbers << "1";
    QString result;
    result = mCntSqlSearch->CreateSpaceStringSearch(numbers);
    QVERIFY( result == " OR ((first_name_as_number LIKE % 1%) AND (first_name_as_number LIKE %_ 1%)) OR ((last_name_as_number LIKE % 1%) AND (last_name_as_number LIKE %_ 1%))" );

    numbers.clear();
    numbers << "1" << "2";
    QString reference(" OR ((first_name_as_number LIKE % 1%) AND (first_name_as_number LIKE %_ 1%)) OR ((last_name_as_number LIKE % 1%) AND (last_name_as_number LIKE %_ 1%)) OR ((first_name_as_number LIKE % 2%) AND (first_name_as_number LIKE %_ 2%)) OR ((last_name_as_number LIKE % 2%) AND (last_name_as_number LIKE %_ 2%))");
    result = mCntSqlSearch->CreateSpaceStringSearch(numbers);
    QVERIFY( !result.compare( reference) );

    numbers.clear();
    numbers << "1" << "21";
    reference = QString(" OR ((first_name_as_number LIKE % 1%) AND (first_name_as_number LIKE %_ 1%)) OR ((last_name_as_number LIKE % 1%) AND (last_name_as_number LIKE %_ 1%)) OR ((first_name_as_number LIKE % 21%) AND (first_name_as_number LIKE %_ 21%)) OR ((last_name_as_number LIKE % 21%) AND (last_name_as_number LIKE %_ 21%))");
    result = mCntSqlSearch->CreateSpaceStringSearch(numbers);
    QVERIFY( !result.compare( reference));
}

void UT_CntSqlSearch::testCreateSpaceString()
{
    QString pattern("1");
    QString result;
    QString reference("((first_name_as_number LIKE % 1%) AND (first_name_as_number LIKE %_ 1%)) OR ((last_name_as_number LIKE % 1%) AND (last_name_as_number LIKE %_ 1%))");
    result = mCntSqlSearch->CreateSpaceString(pattern);
    QVERIFY(!result.compare(reference));
}

