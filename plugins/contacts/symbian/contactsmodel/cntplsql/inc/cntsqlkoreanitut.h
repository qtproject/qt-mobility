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
class KoreanInput;
class QStringList;

class CntSqlKoreanItuT : public CntSqlSearchBase 
{

public:
    enum SqlQueryType { NA,  
                        // Basic cases:
                        // 1: "0", "5"
                        // Just one digit. Select all contact ids from the table. No need to compare
                        // values.
                        AllFromOneTable,
                        // 2: 233, 12 or 
                        //This is basic korean search words. All words should to start with consonant
                        //otherwise are words are not following language rules.¨
                        KoreanBasicSearch,
                        // 3: "333", "369" or 303.
                        // This is search for contacts which start with vowel (in legal name) or contact which is stored as phone number
                        ExactMatchFromOneTable,
                        };    

public:
    CntSqlKoreanItuT( const C12keyKeyMap* twelveKeyKeyMap);
    
    ~CntSqlKoreanItuT();
    
public: //from CCntSqlSearchBase

    QString createInputSpecificSearch(const QString &pattern);
    
    QueryType getQueryType();
    
private:
    
    QString basicKoreanSearch(const QString &pattern);
    
    QString compareColumnsInOrder( QStringList &tokens) const;
    
    QString compareColumnsFromMidleInOrder( QStringList &tokens) const;
    
    QString getSearchColumns(const QString& token, int position) const;
    
    QStringList getSearchPattern(const QString &pattern);
    
    SqlQueryType getSQLQueryType(const QString &pattern);
    
   
private:    
    KoreanInput* mKoreaninput;
    QStringList* mTokens;

friend class UT_CntSqlKoreanItuT;    
};
//#endif

#endif /* CNTSQLKOREANITUT_H_ */
