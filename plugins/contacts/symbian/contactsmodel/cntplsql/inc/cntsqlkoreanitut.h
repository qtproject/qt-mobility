/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Retrieves the character map for each of the numeric keys.
*/

#ifndef CNTSQLKOREANITUT_H_
#define CNTSQLKOREANITUT_H_

#include <QString>
#include "cntsqlsearchbase.h"

class C12keyKeyMap;

class CntSqlKoreanItuT : public CntSqlSearchBase 
{

public:
    enum SqlQueryType { NA,  
                        // Basic cases:
                        // 1: "0", "5"
                        // Just one digit. Select all contact ids from the table. No need to compare
                        // values.
                        AllFromOneTable ,
                        // 2: "123", "01", "10", "00"
                        // No zeros which have non-zeros in their both sides
                        // One or zero tokens, when pattern is split using '0'.
                        ExactMatchFromOneTable
                        };    

public:
    CntSqlKoreanItuT( C12keyKeyMap* twelveKeyKeyMap);
    
    ~CntSqlKoreanItuT();
    
public: //from CCntSqlSearchBase

    QString createInputSpecificSearch(const QString &pattern);
    
    QueryType getQueryType();
    
private:
    
    SqlQueryType getSQLQueryType(const QString &pattern);
    

friend class UT_CntSqlKoreanItuT;    
};
//#endif

#endif /* CNTSQLKOREANITUT_H_ */
