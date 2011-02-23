/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
 @internalComponent
 @released
*/
#include "cntsqlsearchbase.h"
#include "c12keykeymap.h"
#include <QString>
#include <QStringList>

const int KTwoTokens = 2;

// Predictive search table's columns
const QString KColumn1 = "nbr";
const QString KColumn2 = "nbr2";
const QString KColumn3 = "nbr3";
const QString KColumn4 = "nbr4";

#define ORDER_BY_FIRSTNAME_LASTNAME " ORDER BY first_name, last_name ASC;"
#define SELECT_CONTACT_ID           "SELECT contact_id FROM "

/**
CCntPplViewSession first phase constructor.
*/
CntSqlSearchBase::CntSqlSearchBase(const C12keyKeyMap* twelveKeyKeyMap)
: mTwelveKeyMap(twelveKeyKeyMap), 
  CntSqlSearchInterface()
    {
    }

    
/**
CCntPplViewSession destructor.
*/
CntSqlSearchBase::~CntSqlSearchBase()
    {
    
    }

QString CntSqlSearchBase::exactMatch(const QString& pattern, QString table) const
    {
    QString lower;
    QString upper;
    
    int err = mTwelveKeyMap->GetNumericLimits(pattern, lower, upper);
    if(err)
        {
        return QString("");
        }
    
    if (table.length() > 0)
        {
        table += ".";
        }

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

    }
/*   
 * 
 * 
 */
QString CntSqlSearchBase::exactMatchSearch(const QString& pattern) const
    {
    return QString(SELECT_CONTACT_ID + selectTable(pattern) +
                   " WHERE " + exactMatch(pattern)) + ORDER_BY_FIRSTNAME_LASTNAME; 
    }
/*   
 * 
 * 
 */
QString CntSqlSearchBase::returnAllcontacts(const QString &pattern) const  
     {
     return SELECT_CONTACT_ID + selectTable(pattern) + ORDER_BY_FIRSTNAME_LASTNAME;
     }



/*   
 * 
 * 
 */
QString CntSqlSearchBase::selectTable(const QString &pattern) const
    {
    QString predictivesearch;
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

