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
#include "cntsqlkoreanitut.h"
#include "cntsqlsearchinterface.h"
#include "cntsqlsearchbase.h"
#include <QString>

const int KMinimumSearchPatternLength = 1;
    
/**
CCntPplViewSession first phase constructor.
*/
CntSqlKoreanItuT::CntSqlKoreanItuT(C12keyKeyMap* twelveKeyKeyMap)
              :CntSqlSearchBase(twelveKeyKeyMap)
    {
    }
  
/**
CCntPplViewSession destructor.
*/
CntSqlKoreanItuT::~CntSqlKoreanItuT()
    {
    
    }
   
QString CntSqlKoreanItuT::createInputSpecificSearch(const QString &pattern)
    {
    int type = getSQLQueryType(pattern);
    QString sqlString;
    switch (type) {
        case AllFromOneTable:
            sqlString = returnAllcontacts(pattern);
            break;
        case ExactMatchFromOneTable:
            sqlString = exactMatchSearch(pattern);
            break;
        default: 
            ;
        }
    return sqlString;
    }

CntSqlSearchInterface::QueryType CntSqlKoreanItuT::getQueryType()
    {
    return CntSqlSearchInterface::KoreaITUT;
    }

CntSqlKoreanItuT::SqlQueryType CntSqlKoreanItuT::getSQLQueryType(const QString &pattern)
    {
    if (pattern.length() == KMinimumSearchPatternLength)
        {
        return AllFromOneTable;
        }
    else if (pattern.length() > KMinimumSearchPatternLength) //TODO: Change this after tokens are detected!!!
        {
        return ExactMatchFromOneTable;
        }
    else
        {
        return NA;
        }
    }


