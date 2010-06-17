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
#include "cqwertykeymap.h"
#include "c12keykeymap.h"
#include <QHash>
#include <QLocale>

const char KLimitLength = 15;
const int KTwoTokens = 2;
const int KOneToken = 1;
const char KLowerLimitPadding = '0';
const char KUpperLimitPadding = 'F';
const int KMinimumSearchPatternLength = 1;
const int KHexadecimalBase = 16;


#define ORDER_BY_FIRSTNAME_LASTNAME " ORDER BY first_name, last_name ASC;"
#define SELECT_CONTACT_ID			"SELECT contact_id FROM "

// TODO: Since the column names are repeated several times, replace them with
// shorter names like w, x, y & z. Also replace contact_id by id etc.

// Predictive search table's columns
const QString KColumn1 = "nbr";
const QString KColumn2 = "nbr2";
const QString KColumn3 = "nbr3";
const QString KColumn4 = "nbr4";

// Predictive search qwert table's columns
const QString KQm1 = "n";
const QString KQm2 = "n2";
const QString KQm3 = "n3";
const QString KQm4 = "n4";
const QString KQm5 = "n5";
const QString KQm6 = "n6";
const QString KQm7 = "n7";

// Special handling for characters that originate from * and # keys
const QChar KStarChar('*');
const QChar KPlusChar('+');
const QChar KPChar('p');
const QChar KWChar('w');
const QChar KHashChar('#');

//Predictive search table
const QString QwertyTableName = "qm";

CntSqlSearch::CntSqlSearch( const CPcsKeyMap& twelveKeyKeyMap,
                            const CPcsKeyMap& qertyKeyMap ) 
        
    : mkeyKeyMap( static_cast<const C12keyKeyMap*>(&twelveKeyKeyMap) ),
      mQertyKeyMap( static_cast<const CQwertyKeyMap*>(&qertyKeyMap) )
    {
    }

CntSqlSearch::~CntSqlSearch()
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
//
// 6: "10", "1000"
// One token, ends with zero.
// In this case, query should look-up first toke and number ("10", "1000").

QString CntSqlSearch::CreatePredictiveSearch(const QString &pattern)
	{
	int len = pattern.length();
	QString newPattern;
	if (isQwerty(pattern))
	    {
        return CreateQwertyQuery(pattern);
	    }
	else
	    {
        newPattern = ChangeStringPadings(pattern);
        // For best performance, handle 1 digit case first
        if (len == KMinimumSearchPatternLength)
            {
            // Case 1
            return SELECT_CONTACT_ID + SelectTable(newPattern) + ORDER_BY_FIRSTNAME_LASTNAME;
            }
        if (len <= KLimitLength && len > KMinimumSearchPatternLength)
            {
            return CreateQuery(newPattern);
            }
        return QString(""); // Invalid pattern
        }
	}
QString CntSqlSearch::selectQweryTable(const QString &pattern) const
    {
    QString tableNumber; 
    if(pattern.length() > 0)
        {
        return QwertyTableName + tableNumber.setNum(mQertyKeyMap->MapKeyNameToValue(pattern[0]));
        }
    else
        {
        return QString("");
        }
    }
QString CntSqlSearch::SelectTable(const QString &pattern) const
	{
        QString predictivesearch;
        QStringList tokens = GetTokens(pattern);
        bool ok;
        if (pattern.length() == 0)
                {
                return "";
                }
        QString firstNumber(pattern.at(0));
        uint hex = firstNumber.toUInt(&ok, 16);
        if (!ok)
            {
            // TODO: handle error (=invalid characters in pattern)
            }
        switch (hex)
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
            case 10:
                {
                predictivesearch = QString("predictivesearch10");
                }
            break;
            case 11:
                {
                predictivesearch = QString("predictivesearch11");
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

QString CntSqlSearch::CreateQwertyQuery(const QString& pattern) const
    {
    QStringList qwertyString; 
    qwertyString = qwertyTokens(pattern);
    if(qwertyString.count() == 1 )
        {
        if(qwertyString.at(0).length() == 1)
            {
            return SELECT_CONTACT_ID + selectQweryTable(qwertyString.at(0)) + ORDER_BY_FIRSTNAME_LASTNAME;
            }
        else if (qwertyString.at(0).length() > 1)
            {
            return ExactMatchSearchQwerty(qwertyString.at(0)) + ORDER_BY_FIRSTNAME_LASTNAME;
            }
        else
            {
            //Empty string
            return QString(""); 
            }
        }
    else
        {
        return SearchTokensFromOneTable(pattern, qwertyString, CntSqlSearch::QwertyEmail);
        }
    }
// pattern length is between KMinimumSearchPatternLength...KLimitLength
QString CntSqlSearch::CreateQuery(const QString& pattern) const
	{
	QStringList tokens = GetTokens(pattern);
	if (TestPattern(pattern, CntSqlSearch::ZeroIsFirstNumber))
        {
        return CompareTwoColumnsWithModifiedPattern(pattern, tokens);  // Case 7
        }
	else if(tokens.count() < KTwoTokens)
        {
	    if (TestPattern(pattern, CntSqlSearch::ZerosEndOfFirstToken))
            {
            return TwoDifferentTokensSearch(pattern, tokens);  // Case 6
            }
        else
            {
            return ExactMatchSearch(pattern) + Order(tokens); // Case 2
            }
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

QString CntSqlSearch::ExactMatchSearchQwerty(const QString& pattern) const
    {
    return QString(SELECT_CONTACT_ID + selectQweryTable(pattern) +
                   " WHERE " + ExactMatchQwerty(pattern)); 
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
// In this case the limits are different, so there are 12 combinations the two
// values can exist in four columns:
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
//
// Qwert case
// Where X means: (value > lower-limit AND value < upper-limit)
// and Y means: (value > lower-limit-2 AND value < upper-limit-2)
QString CntSqlSearch::SearchTokensFromOneTable(const QString& pattern,
											   const QStringList& tokens,
											   QueryType queryType) const
    {
    QString token;
    QString lower;
    QString upper;
    QString lower2;
    QString upper2;
    int err;
    
    if(queryType == CntSqlSearch::TwelveTable)
        {
        err = mkeyKeyMap->GetNumericLimits(tokens.at(0), lower, upper);
        if(err)
            {
            return QString("");
            }
        err = mkeyKeyMap->GetNumericLimits(tokens.at(1), lower2, upper2);
        if(err)
            {
            return QString("");
            }
        }
    else
        {
        err = mQertyKeyMap->GetNumericLimits(tokens.at(0), lower, upper);
        err = mQertyKeyMap->GetNumericLimits(tokens.at(1), lower2, upper2);
        if(err)
            {
            return QString("");
            }
        }
    QString query; 
    if (queryType == CntSqlSearch::TwelveTable)
		{
        query = SELECT_CONTACT_ID + SelectTable(pattern) + " WHERE NOT(NOT" +
	    ExactMatch(pattern) + " AND NOT" +
		CompareTwoColumns(lower, upper, lower2, upper2) + " AND NOT" +
        CompareTwoColumns(lower2, upper2, lower, upper) + ")";
		}
    else
        {
    
        query = SELECT_CONTACT_ID + selectQweryTable(tokens.at(0)) + " WHERE NOT(NOT" +
        ExactMatchQwerty(pattern) + " AND NOT" +
        CompareTwoQwertyColumns(lower, upper, lower2, upper2) + " AND NOT" +
        CompareTwoQwertyColumns(lower2, upper2, lower, upper) + ")";
        }
	query += Order(tokens, queryType);
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
	QString lower;
	QString upper;
	
	TInt err = mkeyKeyMap->GetNumericLimits(token, lower, upper);
	    
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


QString CntSqlSearch::TwoDifferentTokensSearch(const QString& pattern, const QStringList& tokens) const
        {
        QString token = tokens.at(0);
        QString sortPatern = pattern;
        sortPatern.truncate(pattern.length()-1);
#if defined(USE_DEMORGAN)
        QString query(SELECT_CONTACT_ID + SelectTable(pattern) + " WHERE NOT(NOT" +
            ExactMatch(sortPatern) +
        " AND NOT" + ExactMatch(pattern) + ")");
#else
        QString query(SELECT_CONTACT_ID + SelectTable(pattern) + " WHERE (" +
            ExactMatch(sortPatern) +  // exact match (e.g. "2")
        ") OR " + ExactMatch(pattern)); // exact match (e.g. "20")
#endif
        query += Order(tokens);
        return query;
        }
/*Modified token is search from two different tables. 
  Patern is modified look-up contacts without first zero e.g exact match 05 or 5.
    SELECT contact_id
    FROM
    (
    SELECT predictivesearch5.contact_id, predictivesearch5.first_name, predictivesearch5.last_name FROM predictivesearch5
        UNION 
        SELECT predictivesearch0.contact_id, predictivesearch0.first_name, predictivesearch0.last_name FROM predictivesearch0
        WHERE ((NOT(NOT(predictivesearch0.nbr>22517998136852479 AND predictivesearch0.nbr<27021597764222976) AND NOT(predictivesearch0.nbr2>22517998136852479 AND predictivesearch0.nbr2<27021597764222976) AND NOT(predictivesearch0.nbr3>22517998136852479 AND predictivesearch0.nbr3<27021597764222976) AND NOT(predictivesearch0.nbr4>22517998136852479 AND predictivesearch0.nbr4<27021597764222976)))) 
    ) AS PR
    ORDER BY PR.first_name, PR.last_name ASC;
    
    - AND case - 
    SELECT contact_id FROM (
    SELECT predictivesearch5.contact_id, predictivesearch5.first_name, predictivesearch5.last_name FROM predictivesearch5 WHERE NOT(NOT(NOT(NOT(nbr>382805968326492159 AND nbr<387309567953862656) AND NOT(nbr2>382805968326492159 AND nbr2<387309567953862656) AND NOT(nbr3>382805968326492159 AND nbr3<387309567953862656) AND NOT(nbr4>382805968326492159 AND nbr4<387309567953862656))) AND NOT(NOT(NOT(nbr2>360287970189639679 AND nbr2<432345564227567616 AND nbr>382805968326492159 AND nbr<387309567953862656) AND NOT(nbr3>360287970189639679 AND nbr3<432345564227567616 AND nbr>382805968326492159 AND nbr<387309567953862656) AND NOT(nbr3>360287970189639679 AND nbr3<432345564227567616 AND nbr2>382805968326492159 AND nbr2<387309567953862656) AND NOT(nbr4>360287970189639679 AND nbr4<432345564227567616 AND nbr>382805968326492159 AND nbr<387309567953862656) AND NOT(nbr4>360287970189639679 AND nbr4<432345564227567616 AND nbr2>382805968326492159 AND nbr2<387309567953862656) AND NOT(nbr4>360287970189639679 AND nbr4<432345564227567616 AND nbr3>382805968326492159 AND nbr3<387309567953862656))) AND NOT(NOT(NOT(nbr2>382805968326492159 AND nbr2<387309567953862656 AND nbr>360287970189639679 AND nbr<432345564227567616) AND NOT(nbr3>382805968326492159 AND nbr3<387309567953862656 AND nbr>360287970189639679 AND nbr<432345564227567616) AND NOT(nbr3>382805968326492159 AND nbr3<387309567953862656 AND nbr2>360287970189639679 AND nbr2<432345564227567616) AND NOT(nbr4>382805968326492159 AND nbr4<387309567953862656 AND nbr>360287970189639679 AND nbr<432345564227567616) AND NOT(nbr4>382805968326492159 AND nbr4<387309567953862656 AND nbr2>360287970189639679 AND nbr2<432345564227567616) AND NOT(nbr4>382805968326492159 AND nbr4<387309567953862656 AND nbr3>360287970189639679 AND nbr3<432345564227567616)))) 
    UNION
    SELECT predictivesearch0.contact_id, predictivesearch0.first_name, predictivesearch0.last_name FROM predictivesearch0 WHERE ((NOT(NOT(predictivesearch0.nbr>23930870578544639 AND predictivesearch0.nbr<23931970090172416) AND NOT(predictivesearch0.nbr2>23930870578544639 AND predictivesearch0.nbr2<23931970090172416) AND NOT(predictivesearch0.nbr3>23930870578544639 AND predictivesearch0.nbr3<23931970090172416) AND NOT(predictivesearch0.nbr4>23930870578544639 AND predictivesearch0.nbr4<23931970090172416)))) AND NOT(NOT(NOT(nbr2>360287970189639679 AND nbr2<432345564227567616 AND nbr>0387309567953862656382805968326492159 AND nbr<0387309567953862656382805968326492159) AND NOT(nbr3>360287970189639679 AND nbr3<432345564227567616 AND nbr>0387309567953862656382805968326492159 AND nbr<0387309567953862656382805968326492159) AND NOT(nbr3>360287970189639679 AND nbr3<432345564227567616 AND nbr2>0387309567953862656382805968326492159 AND nbr2<0387309567953862656382805968326492159) AND NOT(nbr4>360287970189639679 AND nbr4<432345564227567616 AND nbr>0387309567953862656382805968326492159 AND nbr<0387309567953862656382805968326492159) AND NOT(nbr4>360287970189639679 AND nbr4<432345564227567616 AND nbr2>0387309567953862656382805968326492159 AND nbr2<0387309567953862656382805968326492159) AND NOT(nbr4>360287970189639679 AND nbr4<432345564227567616 AND nbr3>0387309567953862656382805968326492159 AND nbr3<0387309567953862656382805968326492159))) AND NOT(NOT(NOT(nbr2>0387309567953862656382805968326492159387309567953862656382805968326492159 AND nbr2<0387309567953862656382805968326492159387309567953862656382805968326492159 AND nbr>360287970189639679 AND nbr<432345564227567616) AND NOT(nbr3>0387309567953862656382805968326492159387309567953862656382805968326492159 AND nbr3<0387309567953862656382805968326492159387309567953862656382805968326492159 AND nbr>360287970189639679 AND nbr<432345564227567616) AND NOT(nbr3>0387309567953862656382805968326492159387309567953862656382805968326492159 AND nbr3<0387309567953862656382805968326492159387309567953862656382805968326492159 AND nbr2>360287970189639679 AND nbr2<432345564227567616) AND NOT(nbr4>0387309567953862656382805968326492159387309567953862656382805968326492159 AND nbr4<0387309567953862656382805968326492159387309567953862656382805968326492159 AND nbr>360287970189639679 AND nbr<432345564227567616) AND NOT(nbr4>0387309567953862656382805968326492159387309567953862656382805968326492159 AND nbr4<0387309567953862656382805968326492159387309567953862656382805968326492159 AND nbr2>360287970189639679 AND nbr2<432345564227567616) AND NOT(nbr4>0387309567953862656382805968326492159387309567953862656382805968326492159 AND nbr4<0387309567953862656382805968326492159387309567953862656382805968326492159 AND nbr3>360287970189639679 AND nbr3<432345564227567616)))
    )AS PR ORDER BY PR.first_name, PR.last_name ASC;
    */

QString CntSqlSearch::CompareTwoColumnsWithModifiedPattern(const QString& pattern,
                                                                  const QStringList& tokens) const
    {
    QString lower;
    QString upper;
    QString lower2;
    QString upper2;
    QString lower_zero;
    QString upper_zero;
    
    QString tokenWithZeros;
    int err;
    
    //QString patternAfterZero = pattern.right(pattern.count() - 1);
    // It has been checked earlier that tables are not same
    QString firstTable;
    QString secondTable;
    QString queryString;
    // Case like 05
    if (tokens.at(0).count() == 1 && pattern.length() == 2)
        {
        firstTable = SelectTable(pattern);
        secondTable = SelectTable(tokens.at(0));
        queryString = QString("SELECT contact_id FROM (SELECT " + secondTable + ".contact_id, " + secondTable + ".first_name, " + secondTable + ".last_name FROM " + secondTable 
                                + " UNION SELECT " + firstTable + ".contact_id, " + firstTable + ".first_name, " + firstTable + ".last_name FROM " + firstTable 
                                + " WHERE " + ModifiedMatchColumns( pattern) + ") AS PR ORDER BY PR.first_name, PR.last_name ASC;");
        }
    //case like 05055 or 0506
    else if (tokens.count() >= 2)
        {
        firstTable = SelectTable(pattern);
        secondTable = SelectTable(tokens.at(0));
        
        err = mkeyKeyMap->GetNumericLimits(tokens.at(0), lower, upper);
        if(err)
            {
            return QString("");
            }
        err = mkeyKeyMap->GetNumericLimits(tokens.at(1), lower2, upper2);
        if(err)
            {
            return QString("");
            }
        int patternCount = pattern.count();
        for( int i = 0; i < patternCount ;i++ )
            {
            if (pattern.startsWith('0'))
                {
                tokenWithZeros.append('0');
                }
            else
                {
                break;
                }
            }
        tokenWithZeros.append(tokens.at(0));
        
        err = mkeyKeyMap->GetNumericLimits(tokenWithZeros, lower_zero, upper_zero);
        if(err)
            {
            return QString("");
            }

        queryString = QString("SELECT contact_id FROM (SELECT " + secondTable + ".contact_id, " + secondTable + ".first_name, " + secondTable + ".last_name FROM " + secondTable + 
                                   + " WHERE (" + CompareTwoColumns(lower, upper, lower2, upper2) + " OR" +
                                   CompareTwoColumns(lower2, upper2, lower, upper) + ")" +
                              " UNION" +
                                   " SELECT " + firstTable + ".contact_id, " + firstTable + ".first_name, " + firstTable + ".last_name FROM " + firstTable 
                                   + " WHERE " + ModifiedMatchColumns( pattern) + " OR"
                                   + CompareTwoColumns(lower_zero, upper, lower_zero, upper2) + " OR"
                                   + CompareTwoColumns(lower2, upper2, lower_zero, upper_zero) +
                              ") AS PR ORDER BY PR.first_name, PR.last_name ASC;");
            
        
            
            /* "SELECT contact_id FROM ( SELECT... UNION SELECT ...) AS PR ORDER BY PR.first_name, PR.last_name ASC;*/
            /* 5 table 
             queryString = QString("SELECT " + secondTable + ".contact_id, " + secondTable + ".first_name, " + secondTable + ".last_name FROM " + secondTable 
                                                + " WHERE NOT(NOT" + ExactMatch(tokens.at(0)) + " AND NOT" +
                                               CompareTwoColumns(lower, upper, lower2, upper2) + " AND NOT" +
                                               CompareTwoColumns(lower2, upper2, lower, upper) + " );");*/
        
            /* table 0 queryString = QString("SELECT " + firstTable + ".contact_id, " + firstTable + ".first_name, " + firstTable + ".last_name FROM " + firstTable 
                                               + " WHERE " + ModifiedMatchColumns( pattern) + " AND NOT"
                                               + CompareTwoColumns(zeroPrefix.append(lower), zeroPrefix.append(upper), lower2, upper2) + " AND NOT"
                                               + CompareTwoColumns(lower2, upper2, zeroPrefix.append(lower), zeroPrefix.append(upper)));*/
                
        }
    else
        {
        //case like 055
        firstTable = SelectTable(pattern);
        secondTable = SelectTable(tokens.at(0));
        queryString = QString("SELECT contact_id FROM (SELECT " + secondTable + ".contact_id, " + secondTable + ".first_name, " + secondTable + ".last_name FROM " + secondTable 
                                + " WHERE " + ModifiedMatchColumns( tokens.at(0)) + 
                                + " UNION SELECT " + firstTable + ".contact_id, " + firstTable + ".first_name, " + firstTable + ".last_name FROM " + firstTable 
                                + " WHERE " + ModifiedMatchColumns( pattern) + ") AS PR ORDER BY PR.first_name, PR.last_name ASC;");
        }
    return queryString;
    }

QString CntSqlSearch::ModifiedMatchColumns(const QString& pattern) const
    {
    return "(" + ExactMatch(pattern, SelectTable(pattern)) +  ")";
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

QString CntSqlSearch::CompareTwoQwertyColumns(const QString& lower,
                                        const QString& upper,
                                        const QString& lower2,
                                        const QString& upper2) const
    {
    // Using De Morgan's law to replace ORs with ANDs:
    // A || B || C || D || E || F --> !(!A && !B && !C && !D && !E && !F)
    //
    // As A (match found in columns 1 and 2) is more likely true than other
    // combinations, then !A is more likely false than other combinations, so
    // it is put first in the AND statement.    
    QString query =
        "(NOT(NOT(" +
        // 2nd & 1st column (='A')
        KQm2 + ">" + lower2 + " AND " + KQm2 + "<" + upper2 + " AND " +
        KQm1 + ">" + lower + " AND " + KQm1 + "<" + upper +
        ") AND NOT(" +
        // 3nd & 1st column (='B')
        KQm3 + ">" + lower2 + " AND " + KQm3 + "<" + upper2 + " AND " +
        KQm1 + ">" + lower + " AND " + KQm1 + "<" + upper +
        ") AND NOT(" +
        // 3rd & 2nd column (='C')
        KQm3 + ">" + lower2 + " AND " + KQm3 + "<" + upper2 + " AND " +
        KQm2 + ">" + lower + " AND " + KQm2 + "<" + upper +
        ") AND NOT(" +
        // 4th & 1st column (='D')
        KQm4 + ">" + lower2 + " AND " + KQm4 + "<" + upper2 + " AND " +
        KQm1 + ">" + lower + " AND " + KQm1 + "<" + upper +
        ") AND NOT(" +
        // 4th & 2nd column (='E')
        KQm4 + ">" + lower2 + " AND " + KQm4 + "<" + upper2 + " AND " +
        KQm2 + ">" + lower + " AND " + KQm2 + "<" + upper +
        ") AND NOT(" +
        // 4th & 3rd column (='F')
        KQm4 + ">" + lower2 + " AND " + KQm4 + "<" + upper2 + " AND " +
        KQm3 + ">" + lower + " AND " + KQm3 + "<" + upper + ")))";
        // 5th & 1rd column (='G')
        KQm5 + ">" + lower2 + " AND " + KQm5 + "<" + upper2 + " AND " +
        KQm1 + ">" + lower + " AND " + KQm1 + "<" + upper + ")))";
        // 5th & 2rd column (='H')
        KQm5 + ">" + lower2 + " AND " + KQm5 + "<" + upper2 + " AND " +
        KQm2 + ">" + lower + " AND " + KQm2 + "<" + upper + ")))";
        // 5th & 3rd column (='I')
        KQm5 + ">" + lower2 + " AND " + KQm5 + "<" + upper2 + " AND " +
        KQm3 + ">" + lower + " AND " + KQm3 + "<" + upper + ")))";
        // 5th & 4rd column (='I')
        KQm5 + ">" + lower2 + " AND " + KQm5 + "<" + upper2 + " AND " +
        KQm4 + ">" + lower + " AND " + KQm4 + "<" + upper + ")))";
        // 6th & 1rd column (='J')
        KQm6 + ">" + lower2 + " AND " + KQm6 + "<" + upper2 + " AND " +
        KQm1 + ">" + lower + " AND " + KQm1 + "<" + upper + ")))";
        // 6th & 2rd column (='K')
        KQm6 + ">" + lower2 + " AND " + KQm6 + "<" + upper2 + " AND " +
        KQm2 + ">" + lower + " AND " + KQm2 + "<" + upper + ")))";
        // 6th & 3rd column (='L')
        KQm6 + ">" + lower2 + " AND " + KQm6 + "<" + upper2 + " AND " +
        KQm3 + ">" + lower + " AND " + KQm3 + "<" + upper + ")))";
        // 6th & 43rd column (='M')
        KQm6 + ">" + lower2 + " AND " + KQm6 + "<" + upper2 + " AND " +
        KQm4 + ">" + lower + " AND " + KQm4 + "<" + upper + ")))";
        // 6th & 5rd column (='M')
        KQm6 + ">" + lower2 + " AND " + KQm6 + "<" + upper2 + " AND " +
        KQm5 + ">" + lower + " AND " + KQm5 + "<" + upper + ")))";
        // 7th & 1rd column (='N')
        KQm7 + ">" + lower2 + " AND " + KQm7 + "<" + upper2 + " AND " +
        KQm1 + ">" + lower + " AND " + KQm1 + "<" + upper + ")))";
        // 7th & 1rd column (='O')
        KQm7 + ">" + lower2 + " AND " + KQm7 + "<" + upper2 + " AND " +
        KQm2 + ">" + lower + " AND " + KQm2 + "<" + upper + ")))";
        // 7th & 3rd column (='P')
        KQm7 + ">" + lower2 + " AND " + KQm7 + "<" + upper2 + " AND " +
        KQm3 + ">" + lower + " AND " + KQm3 + "<" + upper + ")))";
        // 7th & 4rd column (='Q')
        KQm7 + ">" + lower2 + " AND " + KQm7 + "<" + upper2 + " AND " +
        KQm4 + ">" + lower + " AND " + KQm4 + "<" + upper + ")))";
        // 7th & 5rd column (='Q')
        KQm7 + ">" + lower2 + " AND " + KQm7 + "<" + upper2 + " AND " +
        KQm5 + ">" + lower + " AND " + KQm5 + "<" + upper + ")))";
        // 7th & 5rd column (='R')
        KQm7 + ">" + lower2 + " AND " + KQm7 + "<" + upper2 + " AND " +
        KQm6 + ">" + lower + " AND " + KQm6 + "<" + upper + ")))";
       
    return query;
    }
QString CntSqlSearch::ExactMatch(const QString& pattern, QString table) const
    {
    QString lower;
    QString upper;
    
    int err = mkeyKeyMap->GetNumericLimits(pattern, lower, upper);
    if(err)
        {
        return QString("");
        }
    
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
	//      NOT(N3>lower && < N3<upper) AND NOT(N4>lower && < N4<upper))
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

QString CntSqlSearch::ExactMatchQwerty(const QString& pattern, QString table) const
    {
    QString lower;
    QString upper;
    TInt err = mQertyKeyMap->GetNumericLimits(pattern, lower, upper);
    
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
    //      NOT(N3>lower && < N3<upper) AND NOT(N4>lower && < N4<upper))
    //
    // As KColumn1 is most likely to contain a match, "NOT(KColumn1)" is more
    // likely to be false than "NOT(KColumn2)" etc. So put KColumn1 first in the
    // AND statement.
    return QString("(NOT(NOT(" +
        table + KQm1 + ">" + lower + " AND " + table + KQm1 + "<" + upper + ") AND NOT(" +
        table + KQm2 + ">" + lower + " AND " + table + KQm2 + "<" + upper + ") AND NOT(" +
        table + KQm3 + ">" + lower + " AND " + table + KQm3 + "<" + upper + ") AND NOT(" +
        table + KQm4 + ">" + lower + " AND " + table + KQm4 + "<" + upper + ") AND NOT(" +
        table + KQm5 + ">" + lower + " AND " + table + KQm5 + "<" + upper + ") AND NOT(" +
        table + KQm6 + ">" + lower + " AND " + table + KQm6 + "<" + upper + ") AND NOT(" +
        table + KQm7 + ">" + lower + " AND " + table + KQm7 + "<" + upper + ")))");
#else
    // Since first column has always some value, and 2nd column is more likely to
    // have a value than 3rd column etc. Put columns in that order:
    // (column 1 comparison) OR (column 2 comparison) OR (column 3 comparison) OR
    // (column 4 comparison)
    // If 1st column results true, there is no need to compare other columns etc.
    return QString("(" +
        table + KQm1 + ">" + lower + " AND " + table + KQm1 + "<" + upper + ") OR (" +
        table + KQm2 + ">" + lower + " AND " + table + KQm2 + "<" + upper + ") OR (" +
        table + KQm3 + ">" + lower + " AND " + table + KQm3 + "<" + upper + ") OR (" +
        table + KQm4 + ">" + lower + " AND " + table + KQm4 + "<" + upper + ") OR (" +
        table + KQm5 + ">" + lower + " AND " + table + KQm5 + "<" + upper + ") OR (" +
        table + KQm6 + ">" + lower + " AND " + table + KQm6 + "<" + upper + ") OR (" +
        table + KQm7 + ">" + lower + " AND " + table + KQm7 + "<" + upper + ")");
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
    const int KFirstColumn = 0;
    const int KSecondColumn = 1;
    QString firstColumn = numbers.at(KFirstColumn);
    QString secondColumn = numbers.at(KSecondColumn);

    if( firstColumn.count() >  1 && secondColumn.count() > 1)
        {
        return "(" + ExactMatch(numbers.at(KFirstColumn), SelectTable(numbers.at(KFirstColumn)))
            + " AND " +
            ExactMatch(numbers.at(KSecondColumn), SelectTable(numbers.at(KSecondColumn))) + ")";
        }
    else if(firstColumn.count() > 1)
        {
        return ExactMatch(numbers.at(KFirstColumn), SelectTable(numbers.at(KFirstColumn)));
        }
    else
        {
        return ExactMatch(numbers.at(KSecondColumn), SelectTable(numbers.at(KSecondColumn)));
        }
    }

QString CntSqlSearch::Order(QStringList tokens, QueryType queryType) const
	{
    QString table;
	if (tokens.count() > 1 )
		{
        if(queryType == CntSqlSearch::QwertyEmail)
            {
            table = selectQweryTable(tokens.at(0));
            }
        else
            {
            table = SelectTable(tokens.at(0));
            }
        return QString(" ORDER BY " + table + ".first_name, " + table + ".last_name ASC;");
		}
	return QString(ORDER_BY_FIRSTNAME_LASTNAME);
	}

QString CntSqlSearch::ChangeStringPadings( const QString &pattern ) const
    { 
    QString newPattern = pattern;
    if (QLocale::system().language() == QLocale::Thai)
        {
        newPattern.remove(KStarChar, Qt::CaseInsensitive);
        newPattern.remove(KPlusChar, Qt::CaseInsensitive);
        newPattern.remove(KPChar, Qt::CaseInsensitive);
        newPattern.remove(KWChar, Qt::CaseInsensitive);
        newPattern.remove(KHashChar, Qt::CaseInsensitive);
        }
    else
        { 
        newPattern.replace(KStarChar, 'A');
        newPattern.replace(KPlusChar, 'A');
        newPattern.replace(KPChar, 'A');
        newPattern.replace(KWChar, 'A');
        newPattern.replace(KHashChar, 'B');
        }
    return newPattern;
    }

bool CntSqlSearch::TestPattern( const QString &pattern, SearchMethod searchMethod ) const
    {
    QStringList tokens = GetTokens(pattern);
    if (!tokens.isEmpty() && !pattern.isEmpty())
        {
        if (CntSqlSearch::ZerosEndOfFirstToken == searchMethod)
            {
            if( tokens.count() == KOneToken && !tokens.at(0).contains("0")
                && !pattern.startsWith('0') && pattern.count('0') == 1
                && pattern.endsWith('0'))
                {
                return true;
                }
            }
        if (CntSqlSearch::ZeroIsFirstNumber == searchMethod )
            {
            if(pattern.startsWith('0') && pattern.count() > 1 )
                {
                return true;
                }
            if (pattern.startsWith('0') && tokens.count() >= KTwoTokens )
                {
                return true;
                }
            }
        }
    return false;
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

bool CntSqlSearch::isQwerty(const QString &pattern)
    {
    QChar rs(30);
    int rs_index = pattern.indexOf(rs);  
    int qwerty_index = pattern.indexOf("vqwerty");
    if(rs_index >= 0 && qwerty_index >= 0 )
        {
        if(rs_index + 1 == qwerty_index)
            {
            return true;
            }
        return false;
        }
    return false;
    }
QStringList CntSqlSearch::qwertyTokens(const QString &pattern) const
    {
    QString decodePatern;
    QString keymapsString = mQertyKeyMap->GetMappedString(pattern);
    int index = pattern.indexOf(30);
    if(index > 0 )
        {
        QString decodePatern = keymapsString.left(index);
        return decodePatern.split(32, QString::SkipEmptyParts);
        }
    else
        {
        return QStringList("");
        }
    }
QString CntSqlSearch::UpperLimit( const QString &pattern ) const
    {
    return Pad( pattern, KUpperLimitPadding );
    }

QString CntSqlSearch::LowerLimit( const QString &pattern ) const
    {
    return Pad( pattern, KLowerLimitPadding );
    }
