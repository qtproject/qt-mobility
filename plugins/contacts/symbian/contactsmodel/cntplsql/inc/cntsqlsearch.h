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

#include <QObject>
#include <QString>

// If this macro is defined, searches like "102" are made on one table only
#define SEARCH_FROM_ONE_TABLE

// If defined, De Morgan's laws are used to replace ORs with ANDs in the
// SELECT statement.
#define USE_DEMORGAN

class CPcsKeyMap; 
class CQwertyKeyMap; 
class C12keyKeyMap;

class CntSqlSearch// : public QObject
{
    //Q_OBJECT

public:

enum SearchMethod { Undefinded, ZerosEndOfFirstToken, ZeroIsFirstNumber };

enum QueryType
        {
        TwelveTable = 0,
        QwertyEmail,
        QwertyContact
        };

public:
    CntSqlSearch( const CPcsKeyMap& twelveKeyKeyMap,
                  const CPcsKeyMap& qertyKeyMap );
    
    ~CntSqlSearch();

    QString CreatePredictiveSearch(const QString &pattern);
    

private:
    
    QString selectQweryTable(const QString &pattern) const;
    
    QString SelectTable(const QString &pattern) const;
    
    QStringList GetTokens(const QString& pattern) const;
    
    QString CreateQwertyQuery(
            const QString& pattern) const;
    
    QString CreateQuery(const QString& pattern) const;
    
    QString ExactMatchSearch(const QString& pattern) const;
    
    QString ExactMatchSearchQwerty(const QString& pattern) const;
    
    QString CreateSpaceStringSearch(QStringList numbers, const QString &pattern) const;

    QString IntersectionSearch(const QString& pattern,
                               const QStringList& numbers) const;
    QString SearchTokensFromOneTable(const QString& pattern,
								     const QStringList& tokens,
								     QueryType queryType = TwelveTable) const;

    QString IdenticalTokensSearch(const QString& pattern,
                                  const QStringList& numbers) const;

    QString CompareTwoColumns(const QString& lower,
						 	  const QString& upper,
							  const QString& lower2,
							  const QString& upper2) const;
    
    QString CompareTwoQwertyColumns(const QString& lower,
                                    const QString& upper,
                                    const QString& lower2,
                                    const QString& upper2) const;

    QString TwoDifferentTokensSearch(const QString& pattern,
                                     const QStringList& tokens) const;
    
    QString CompareTwoColumnsWithModifiedPattern(const QString& pattern,
                                                const QStringList& tokens) const;
    
    QString ModifiedMatchColumns(const QString& pattern) const;

    QString ExactMatch(const QString& pattern, QString table = "") const;
    
    QString ExactMatchQwerty(const QString& pattern, QString table = "") const;

    QString CreateJoinTableSearch(QString pattern, QStringList numbers) const;

    QString ExactMatchColumns(QStringList numbers) const;

    QString Order(QStringList numbers, QueryType queryType = TwelveTable ) const;

    QString UpperLimit( const QString &pattern ) const;
    
    QString LowerLimit( const QString &pattern ) const;

    QString ChangeStringPadings( const QString &pattern ) const;

    bool TestPattern( const QString &pattern, SearchMethod searchMethod =  Undefinded ) const;
    
    QString Pad( const QString &pattern, char padChar ) const;
    
    bool isQwerty(const QString &pattern);
    
    QStringList qwertyTokens(const QString &pattern) const;
    
private:
    
    const C12keyKeyMap* mkeyKeyMap;
    const CQwertyKeyMap* mQertyKeyMap; 
    
    friend class UT_CntSqlSearch;
};
//#endif
