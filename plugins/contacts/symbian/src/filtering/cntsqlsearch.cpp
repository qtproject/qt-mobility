/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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
#include <QStringList>

#include "cntsqlsearch.h"

const char KLimitLength = 15;
const int KTwoTokens = 2;
const char KLowerLimitPadding = '0';
const char KUpperLimitPadding = 'F';
const int KMinimumSearchPatternLength = 1;


#define ORDER_BY_FIRSTNAME_LASTNAME " ORDER BY first_name, last_name ASC;"
#define SELECT_CONTACT_ID			"SELECT contact_id FROM "

// TODO: Since the column names are repeated several times, replace them with
// shorter names like w, x, y & z. Also replace contact_id by id etc.

// Predictive search table's columns
const QString KColumn1 = "nbr";
const QString KColumn2 = "nbr2";
const QString KColumn3 = "nbr3";
const QString KColumn4 = "nbr4";


CntSqlSearch::CntSqlSearch()
	{
	}

// Basic cases:
// 1: "0", "5"
// Just one digit. Select all contact ids from the table. No need to compare
// values.
//
// 2: "123", "01", "10", "010", "00"
// No zeros which have non-zeros in their both sides
// One or zero tokens, when pattern is split using '0'.
//
// 3: "101", "1001"
// Same digit of both sides of the zero
// Two tokens, each with length of 1 and tokens are the same.
// The queries of case 3 could also be handled with case 4 query, but that
// would yield a longer SQL statement.
//
// 4: "102", "1002"
// One or more zeros in the middle, just one digit on either side of the zero(s)
// and those digits are not the same.
// Two tokens, each with length of 1 and tokens are different.
//
// 5: "1023", "0102", "1010", "00100200", "10203", "01020304050"
// Two tokens, at least one of them has length > 1.
// If tokens are identical, handle as case 3, otherwise handle as case 4.
// ("10203" -> tokens "1" and "203"
//  "0010023004560" -> tokens "001" and "23004560")
QString CntSqlSearch::CreatePredictiveSearch(const QString &pattern)
	{
	int len = pattern.length();
	// For best performance, handle 1 digit case first
	if (len == KMinimumSearchPatternLength)
        {
		// Case 1
        return SELECT_CONTACT_ID + SelectTable(pattern) + ORDER_BY_FIRSTNAME_LASTNAME;
        }
    if (len <= KLimitLength && len > KMinimumSearchPatternLength)
        {
		return CreateQuery(pattern);
		}

	return QString(""); // Invalid pattern
	}

QString CntSqlSearch::SelectTable(const QString &pattern) const
	{
    QString predictivesearch;
	if (pattern.length() == 0)
		{
		return "";
		}
    switch (pattern.at(0).digitValue())
        {
        case 0:
            {
            predictivesearch = QString("predictivesearch0");
            }
        break;
        case 1:
            {
            predictivesearch = QString("predictivesearch1");
            }
        break;
        case 2:
            {
            predictivesearch = QString("predictivesearch2");
            }
        break;
        case 3:
            {
            predictivesearch = QString("predictivesearch3");
            }
        break;
        case 4:
            {
            predictivesearch = QString("predictivesearch4");
            }
        break;
        case 5:
            {
            predictivesearch = QString("predictivesearch5");
            }
        break;
        case 6:
            {
            predictivesearch = QString("predictivesearch6");
            }
        break;
        case 7:
            {
            predictivesearch = QString("predictivesearch7");
            }
        break;
        case 8:
            {
            predictivesearch = QString("predictivesearch8");
            }
        break;
        case 9:
            {
            predictivesearch = QString("predictivesearch9");
            }
        break;
		default: // error
			predictivesearch = "";
			break;
        }
	return predictivesearch;
	}

// Even if there are over 2 tokens, make 2 tokens.
// If there are two or more tokens, include the leading zeros in the first
// token the and trailing zeros in the second token.
// E.g. "0010230" results tokens "001" and "230" and
// "001230045067800900" tokens "00123" and "45067800900".
QStringList CntSqlSearch::GetTokens(const QString& pattern) const
	{
    const QChar KZero('0');
    QStringList tokens = pattern.split(KZero, QString::SkipEmptyParts);
    if (tokens.count() < KTwoTokens)
        {
        return tokens;
        }

    QStringList twoTokens;
    int i(0);
    while (pattern[i] == KZero) // Skip leading zeros
        {
        ++i;
        }
    while (pattern[i] != KZero) // Skip non-zeros to find where first token ends
        {
        ++i;
        }
    twoTokens.append(pattern.left(i));
    
    while (pattern[i] == KZero) // Skip zeros to find where second token begins
        {
        ++i;
        }
    twoTokens.append(pattern.mid(i));
    return twoTokens;
	}

// pattern length is between KMinimumSearchPatternLength...KLimitLength
QString CntSqlSearch::CreateQuery(const QString& pattern) const
	{
	QStringList tokens = GetTokens(pattern);
	if (tokens.count() < KTwoTokens)
		{
		return ExactMatchSearch(pattern) + Order(tokens);; // Case 2
		}
	else
	    {
		if (tokens.at(0) == tokens.at(1))
            {
            return IdenticalTokensSearch(pattern, tokens); // Case 3
            }
        else
            {
            return IntersectionSearch(pattern, tokens); // Case 4
            }
        }
	}

QString CntSqlSearch::ExactMatchSearch(const QString& pattern) const
	{
	return QString(SELECT_CONTACT_ID + SelectTable(pattern) +
		           " WHERE " + ExactMatch(pattern)); 
	}

// It has been checked that tokens are different, but they might begin with
// the same digit.
QString CntSqlSearch::IntersectionSearch(const QString& pattern,
									     const QStringList& tokens) const
	{
#if defined(SEARCH_FROM_ONE_TABLE)
	return SearchTokensFromOneTable(pattern, tokens);
#else
/* Query for pattern = "205":
SELECT predictivesearch2.contact_id FROM predictivesearch2 WHERE EXISTS
(SELECT contact_id FROM predictivesearch5 WHERE predictivesearch2.contact_id = predictivesearch5.contact_id)
OR
(SELECT contact_id FROM predictivesearch2 
WHERE(predictivesearch2.nbr>145522562959409152 AND predictivesearch2.nbr<145804037936119807) OR
(predictivesearch2.nbr2>145522562959409152 AND predictivesearch2.nbr2<145804037936119807) OR
(predictivesearch2.nbr3>145522562959409152 AND predictivesearch2.nbr3<145804037936119807) OR
(predictivesearch2.nbr4>145522562959409152 AND predictivesearch2.nbr4<145804037936119807))
ORDER BY predictivesearch2.first_name, predictivesearch2.last_name ASC;

This query works if both tokens have just one digit (e.g. "102", but not "1023")
*/	
	if (tokens.at(0).length() == KMinimumSearchPatternLength &&
		tokens.at(1).length() == KMinimumSearchPatternLength) 
		{
		// Case 4
		QString firstTable = SelectTable(tokens.at(0));
		QString secondTable = SelectTable(tokens.at(1));
		QString query =
			"SELECT " + firstTable + ".contact_id FROM " + firstTable +
			" WHERE EXISTS (" + SELECT_CONTACT_ID + secondTable + " WHERE " +
			firstTable + ".contact_id = " + secondTable + ".contact_id) OR (" +
			SELECT_CONTACT_ID + firstTable + " WHERE " + ExactMatch(pattern, firstTable) + ")";
		return query  + Order(tokens);
		}
	if (tokens.at(0).at(0) == tokens.at(1).at(0) ||
	    tokens.at(0).length() > 1 && tokens.at(1).length() > 1)
		{
		// Tokens begin with same digit or both tokens are longer than one digit.
		// Must search from one table.
		return SearchTokensFromOneTable(pattern, tokens);
		}
	return CreateJoinTableSearch(pattern, tokens); // Case 5
#endif
	}

// Find the exact match, or a column whose value is within
// lower..upper(exclusive) and another column whose value is within
// lower2..upper2(exclusive).
// In this case the limits are is different, so there are 12 combinations the
// two values can exist in four columns:
// 
// (column = X  AND column2 = Y) OR
// (column = X  AND column3 = Y) OR
// (column = X  AND column4 = Y) OR
// (column2 = X AND column3 = Y) OR
// (column2 = X AND column4 = Y) OR
// (column3 = X AND column4 = Y) OR
// (column = Y  AND column2 = X) OR
// (column = Y  AND column3 = X) OR
// (column = Y  AND column4 = X) OR
// (column2 = Y AND column3 = X) OR
// (column2 = Y AND column4 = X) OR
// (column3 = Y AND column4 = X)
//
// Where X means: (value > lower-limit AND value < upper-limit)
// and Y means: (value > lower-limit-2 AND value < upper-limit-2)
QString CntSqlSearch::SearchTokensFromOneTable(const QString& pattern,
											   const QStringList& tokens) const
	{
	QString token = tokens.at(0);
    QString lower = LowerLimit(token);
    QString upper = UpperLimit(token);
    QString lower2 = LowerLimit(tokens.at(1));
    QString upper2 = UpperLimit(tokens.at(1));

    QString query =
#if defined(USE_DEMORGAN)
		SELECT_CONTACT_ID + SelectTable(pattern) + " WHERE NOT(NOT" +
	    ExactMatch(pattern) + " AND NOT" +
		CompareTwoColumns(lower, upper, lower2, upper2) + " AND NOT" +
        CompareTwoColumns(lower2, upper2, lower, upper) + ")";
#else
        SELECT_CONTACT_ID + SelectTable(token) + " WHERE (" +
		// exact match (e.g. "102")
        ExactMatch(pattern) + ") OR " +
        CompareTwoColumns(lower, upper, lower2, upper2) + " OR " +
        CompareTwoColumns(lower2, upper2, lower, upper);
#endif
	query += Order(tokens);
	return query;
	}

// Either an exact match is required, or tokens must be found, but not in the
// same column.
// Since tokens are identical, they have same limits, and one call to
// CompareTwoColumns() is enough.
QString CntSqlSearch::IdenticalTokensSearch(const QString& pattern,
										    const QStringList& tokens) const
	{
	QString token = tokens.at(0);
	QString lower = LowerLimit(token);
	QString upper = UpperLimit(token);
#if defined(USE_DEMORGAN)
	QString query(SELECT_CONTACT_ID + SelectTable(pattern) + " WHERE NOT(NOT" +
	    ExactMatch(pattern) +
        " AND NOT" + CompareTwoColumns(lower, upper, lower, upper) + ")");
#else
	QString query(SELECT_CONTACT_ID + SelectTable(pattern) + " WHERE (" +
	    ExactMatch(pattern) +  // exact match (e.g. "101")
        ") OR " + CompareTwoColumns(lower, upper, lower, upper));
#endif
	query += Order(tokens);
	return query;
	}

// Put individual AND / OR operations in such order that in most cases there is
// no need to evaluate all arguments of the AND / OR.
// In case of AND, put the less likely condition on the left side of AND.
// In case of OR, put the more likely condition on the left side of OR.
// Since 2nd column is more likely to be empty, compare it before 1st column.
// Since 1st & 2nd columns are more likely to contain a match, compare them
// before other column combinations (1st & 3rd, 2nd & 3rd etc)
QString CntSqlSearch::CompareTwoColumns(const QString& lower,
										const QString& upper,
										const QString& lower2,
										const QString& upper2) const
	{
#if defined(USE_DEMORGAN)
	// Using De Morgan's law to replace ORs with ANDs:
	// A || B || C || D || E || F --> !(!A && !B && !C && !D && !E && !F)
	//
	// As A (match found in columns 1 and 2) is more likely true than other
	// combinations, then !A is more likely false than other combinations, so
	// it is put first in the AND statement.	
	QString query =
		"(NOT(NOT(" +
		// 2nd & 1st column (='A')
		KColumn2 + ">" + lower2 + " AND " + KColumn2 + "<" + upper2 + " AND " +
		KColumn1 + ">" + lower + " AND " + KColumn1 + "<" + upper +
		") AND NOT(" +
		// 3nd & 1st column (='B')
		KColumn3 + ">" + lower2 + " AND " + KColumn3 + "<" + upper2 + " AND " +
		KColumn1 + ">" + lower + " AND " + KColumn1 + "<" + upper +
		") AND NOT(" +
		// 3rd & 2nd column (='C')
		KColumn3 + ">" + lower2 + " AND " + KColumn3 + "<" + upper2 + " AND " +
		KColumn2 + ">" + lower + " AND " + KColumn2 + "<" + upper +
		") AND NOT(" +
		// 4th & 1st column (='D')
		KColumn4 + ">" + lower2 + " AND " + KColumn4 + "<" + upper2 + " AND " +
		KColumn1 + ">" + lower + " AND " + KColumn1 + "<" + upper +
		") AND NOT(" +
		// 4th & 2nd column (='E')
		KColumn4 + ">" + lower2 + " AND " + KColumn4 + "<" + upper2 + " AND " +
		KColumn2 + ">" + lower + " AND " + KColumn2 + "<" + upper +
		") AND NOT(" +
		// 4th & 3rd column (='F')
		KColumn4 + ">" + lower2 + " AND " + KColumn4 + "<" + upper2 + " AND " +
		KColumn3 + ">" + lower + " AND " + KColumn3 + "<" + upper + ")))";
#else
	QString query =
		"(" +
		// 2nd & 1st column
		KColumn2 + ">" + lower2 + " AND " + KColumn2 + "<" + upper2 + " AND " +
		KColumn1 + ">" + lower + " AND " + KColumn1 + "<" + upper +
		") OR (" +
		// 3nd & 1st column
		KColumn3 + ">" + lower2 + " AND " + KColumn3 + "<" + upper2 + " AND " +
		KColumn1 + ">" + lower + " AND " + KColumn1 + "<" + upper +
		") OR (" +
		// 3rd & 2nd column
		KColumn3 + ">" + lower2 + " AND " + KColumn3 + "<" + upper2 + " AND " +
		KColumn2 + ">" + lower + " AND " + KColumn2 + "<" + upper +
		") OR (" +
		// 4th & 1st column
		KColumn4 + ">" + lower2 + " AND " + KColumn4 + "<" + upper2 + " AND " +
		KColumn1 + ">" + lower + " AND " + KColumn1 + "<" + upper +
		") OR (" +
		// 4th & 2nd column
		KColumn4 + ">" + lower2 + " AND " + KColumn4 + "<" + upper2 + " AND " +
		KColumn2 + ">" + lower + " AND " + KColumn2 + "<" + upper +
		") OR (" +
		// 4th & 3rd column
		KColumn4 + ">" + lower2 + " AND " + KColumn4 + "<" + upper2 + " AND " +
		KColumn3 + ">" + lower + " AND " + KColumn3 + "<" + upper + ")";
#endif
	return query;
	}

QString CntSqlSearch::ExactMatch(const QString& pattern, QString table) const
    {
    QString lower = LowerLimit(pattern);
    QString upper = UpperLimit(pattern);
    
    if (table.length() > 0)
        {
        table += ".";
        }
#if defined(USE_DEMORGAN)
	// Using De Morgan's law to replace ORs with ANDs:
	// column1 || column2 || column3 || column4
	// ->
	// (NOT(NOT(column1) AND NOT(column2) AND NOT(column3) AND NOT(column4))
	//
	// Which means:
	// (NOT(NOT(N>lower  && < N<upper)  AND NOT(N2>lower && < N2<upper) AND
	//      NOT(N3>lower && < N3<upper) AND NOT(N>lower && < N<upper))
	//
	// As KColumn1 is most likely to contain a match, "NOT(KColumn1)" is more
	// likely to be false than "NOT(KColumn2)" etc. So put KColumn1 first in the
	// AND statement.
	return QString("(NOT(NOT(" +
        table + KColumn1 + ">" + lower + " AND " + table + KColumn1 + "<" + upper + ") AND NOT(" +
        table + KColumn2 + ">" + lower + " AND " + table + KColumn2 + "<" + upper + ") AND NOT(" +
        table + KColumn3 + ">" + lower + " AND " + table + KColumn3 + "<" + upper + ") AND NOT(" +
        table + KColumn4 + ">" + lower + " AND " + table + KColumn4 + "<" + upper + ")))");
#else
	// Since first column has always some value, and 2nd column is more likely to
	// have a value than 3rd column etc. Put columns in that order:
	// (column 1 comparison) OR (column 2 comparison) OR (column 3 comparison) OR
	// (column 4 comparison)
	// If 1st column results true, there is no need to compare other columns etc.
    return QString("(" +
        table + KColumn1 + ">" + lower + " AND " + table + KColumn1 + "<" + upper + ") OR (" +
        table + KColumn2 + ">" + lower + " AND " + table + KColumn2 + "<" + upper + ") OR (" +
        table + KColumn3 + ">" + lower + " AND " + table + KColumn3 + "<" + upper + ") OR (" +
        table + KColumn4 + ">" + lower + " AND " + table + KColumn4 + "<" + upper + ")");
#endif
    }


// TODO: if SEARCH_FROM_ONE_TABLE case is slower than the one that searches from
// two tables, then this function is needed for cases where pattern is:
// "1023", "12300450"
//
/* This query makes sub-query into table 5 and searches for a number that begins
with 56606. but it does not support cases where both tokens are longer than one digit.

SELECT predictivesearch5.contact_id FROM predictivesearch5 WHERE EXISTS (SELECT contact_id FROM predictivesearch5 
WHERE (predictivesearch5.nbr>389005014883893248 AND predictivesearch5.nbr<389006114395521023) OR
(predictivesearch5.nbr2>389005014883893248 AND predictivesearch5.nbr2<389006114395521023) OR
(predictivesearch5.nbr3>389005014883893248 AND predictivesearch5.nbr3<389006114395521023) OR
(predictivesearch5.nbr4>389005014883893248 AND predictivesearch5.nbr4<389006114395521023))
OR
(SELECT predictivesearch5.contact_id FROM predictivesearch5 JOIN predictivesearch6 ON
predictivesearch5.contact_id = predictivesearch6.contact_id
WHERE(predictivesearch5.nbr>388998417814126592 AND predictivesearch5.nbr<389279892790837247) OR
(predictivesearch5.nbr2>388998417814126592 AND predictivesearch5.nbr2<389279892790837247) OR
(predictivesearch5.nbr3>388998417814126592 AND predictivesearch5.nbr3<389279892790837247) OR
(predictivesearch5.nbr4>388998417814126592 AND predictivesearch5.nbr4<389279892790837247))
ORDER BY predictivesearch5.first_name, predictivesearch5.last_name ASC;

SELECT contact_id
FROM
(
SELECT predictivesearch5.contact_id, predictivesearch5.first_name, predictivesearch5.last_name  FROM predictivesearch5
WHERE (predictivesearch5.nbr>387415121070129152 AND predictivesearch5.nbr<387432713256173567) OR
(predictivesearch5.nbr2>387415121070129152 AND predictivesearch5.nbr2<387432713256173567) OR
(predictivesearch5.nbr3>387415121070129152 AND predictivesearch5.nbr3<387432713256173567) OR
(predictivesearch5.nbr4>387415121070129152 AND predictivesearch5.nbr4<387432713256173567)
UNION
SELECT predictivesearch5.contact_id, predictivesearch5.first_name, predictivesearch5.last_name  FROM predictivesearch5 JOIN predictivesearch6 ON predictivesearch5.contact_id = predictivesearch6.contact_id
WHERE((predictivesearch5.nbr>387309567953862656 AND predictivesearch5.nbr<391813167581233151) OR (predictivesearch5.nbr2>387309567953862656 AND predictivesearch5.nbr2<391813167581233151) OR (predictivesearch5.nbr3>387309567953862656 AND predictivesearch5.nbr3<391813167581233151) OR (predictivesearch5.nbr4>387309567953862656 AND predictivesearch5.nbr4<391813167581233151)
AND
(predictivesearch6.nbr>387309567953862656 AND predictivesearch6.nbr<391813167581233151) OR (predictivesearch6.nbr2>387309567953862656 AND predictivesearch6.nbr2<391813167581233151) OR (predictivesearch6.nbr3>387309567953862656 AND predictivesearch6.nbr3<391813167581233151) OR (predictivesearch6.nbr4>387309567953862656 AND predictivesearch6.nbr4<391813167581233151))
) AS PR
ORDER BY PR.first_name, PR.last_name ASC;

Here is a De Morgan version

SELECT contact_id
FROM
(
SELECT predictivesearch5.contact_id, predictivesearch5.first_name, predictivesearch5.last_name  FROM predictivesearch5
WHERE NOT((NOT (predictivesearch5.nbr >= 387415121070129152 AND predictivesearch5.nbr <= 387432713256173567)) AND (NOT (predictivesearch5.nbr2 >= 387415121070129152 AND predictivesearch5.nbr2 <= 387432713256173567)) AND (NOT (predictivesearch5.nbr3 >= 387415121070129152 AND predictivesearch5.nbr3 <= 387432713256173567)) AND (NOT (predictivesearch5.nbr4 >= 387415121070129152 AND predictivesearch5.nbr4 <= 387432713256173567)))
UNION
SELECT predictivesearch5.contact_id, predictivesearch5.first_name, predictivesearch5.last_name  FROM predictivesearch5 JOIN predictivesearch6 ON predictivesearch5.contact_id = predictivesearch6.contact_id
WHERE NOT((NOT (predictivesearch5.nbr >= 387309567953862656 AND predictivesearch5.nbr <= 391813167581233151)) AND (NOT (predictivesearch5.nbr2 >= 387309567953862656 AND predictivesearch5.nbr2 <= 391813167581233151)) AND (NOT (predictivesearch5.nbr3 >= 387309567953862656 AND predictivesearch5.nbr3 <= 391813167581233151)) AND (NOT (predictivesearch5.nbr4 >= 387309567953862656 AND predictivesearch5.nbr4 <= 391813167581233151)))
AND
NOT((NOT (predictivesearch6.nbr >= 387309567953862656 AND predictivesearch6.nbr <= 391813167581233151)) AND (NOT (predictivesearch6.nbr2 >= 387309567953862656 AND predictivesearch6.nbr2 <= 391813167581233151)) AND (NOT (predictivesearch6.nbr3 >= 387309567953862656 AND predictivesearch6.nbr3 <= 391813167581233151)) AND (NOT (predictivesearch6.nbr4 >= 387309567953862656 AND predictivesearch6.nbr4 <= 391813167581233151)))
) AS PR
ORDER BY PR.first_name, PR.last_name ASC;

*/
QString CntSqlSearch::CreateJoinTableSearch(QString pattern, QStringList numbers) const
	{
	// It has been checked earlier that tables are not same
	QString firstTable = SelectTable(numbers.at(0));
	QString secondTable = SelectTable(numbers.at(1));

    QString queryString = QString("SELECT contact_id FROM (SELECT "
                                  + firstTable + ".contact_id, " + firstTable + ".first_name, " + firstTable + ".last_name FROM " + firstTable +
                                    " WHERE " + ExactMatch(pattern, firstTable) +
                                    " UNION SELECT " + firstTable + ".contact_id, " + firstTable + ".first_name, " + firstTable + ".last_name FROM " + firstTable + " JOIN " + secondTable + " ON " + firstTable + ".contact_id = " + secondTable +  ".contact_id WHERE" +
                                ExactMatchColumns(numbers) + ") AS PR ORDER BY PR.first_name, PR.last_name ASC;");
	return queryString;
	}

QString CntSqlSearch::ExactMatchColumns(QStringList numbers) const
        {

        QString firstColumn = numbers.at(0);
        QString secondColumn = numbers.at(1);

        if( firstColumn.count() >  1 && secondColumn.count() > 1)
            {
            return "(" + ExactMatch(numbers.at(0), SelectTable(numbers.at(0))) + " AND " + ExactMatch(numbers.at(1), SelectTable(numbers.at(1))) + ")";
            }
        else if(firstColumn.count() > 1)
            {
            return ExactMatch(numbers.at(0), SelectTable(numbers.at(0)));
            }
        else
            {
            return ExactMatch(numbers.at(1), SelectTable(numbers.at(1)));
            }
        }

QString CntSqlSearch::Order(QStringList tokens) const
	{
	if (tokens.count() > 1 )
		{
		QString table = SelectTable(tokens.at(0));
		return QString(" ORDER BY " + table + ".first_name, " + table + ".last_name ASC;");
		}
	return QString(ORDER_BY_FIRSTNAME_LASTNAME);
	}

QString CntSqlSearch::Pad( const QString &pattern, char padChar ) const
    {
    int padCount = KLimitLength - pattern.length();    
    QString result;
    if ( padCount < 0 )
        {
        result = pattern.left(KLimitLength);
        }
    else
        {
        result = pattern;
        for( int i = 0; i < padCount ;i++ )
            {
            result.append(padChar);
            }
        }
    const int KHexadecimalBase = 16;
    bool ok;
    // Use signed int to prevent underflow when replaced is "00...00"
    qint64 value = result.toLongLong(&ok, KHexadecimalBase); 
    if (!ok)
    	{
    	// TODO: handle error (=invalid characters in pattern)
    	}

	// In order to write queries using '>' and '<' instead of '>=' and '<=',
	// expand the limit by one.
	if (padChar == KUpperLimitPadding)
		{
		++value;
		}
	else
		{
		--value;
		}

    return QString::number(value, 10);
    }

QString CntSqlSearch::UpperLimit( const QString &pattern ) const
    {
    return Pad( pattern, KUpperLimitPadding );
    }

QString CntSqlSearch::LowerLimit( const QString &pattern ) const
    {
    return Pad( pattern, KLowerLimitPadding );
    }
