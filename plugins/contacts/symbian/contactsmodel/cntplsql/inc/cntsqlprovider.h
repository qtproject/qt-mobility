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

#ifndef __CNTSQLPROVIDER_H__
#define __CNTSQLPROVIDER_H__

#include "cntsqlprovider.h"

#include <e32cmn.h>
#include <e32base.h>
#include <badesca.h>

/**
Enumeration defining sql statement type.

@internalComponent
@released
*/
enum TCntSqlStatement
	{
	EInsert,
	EUpdate,
	EDelete,
	ESelect	
	};
	
/**
Class used to build a sql statement class. It holds the statement type and the 
database table against which sql statement is run

@internalComponent
@released
*/
NONSHARABLE_CLASS(TCntSqlStatementType)
	{
public:
	TCntSqlStatementType(TCntSqlStatement aSqlStatement, const TDesC& aTableName);
	void SetSqlStatement(TCntSqlStatement aSqlStatement);
	TCntSqlStatement SqlStatement() const;
	const TDesC& TableName() const;
private:
	TCntSqlStatement iSqlStatement;
	const TDesC& iTableName;
	};	

/**
An abstract object representing a sql statement.

@internalComponent
@released
*/
NONSHARABLE_CLASS(CCntSqlStatement) : public CBase
	{
public:
	virtual TDesC& SqlStringL() = 0;
	void SetParamL(const TDesC& aParam, const TDesC& aValue);
	void SetConditionL(const TDesC& aCondition);
	void ClearCondition();
	TInt ParameterIndex(const TDesC& aParam) const;
	void Reset();
	void ClearParams();
	~CCntSqlStatement();

	// aTableName New table name, ownership is transferred
	void SetTableName(HBufC* aTableName);

	void ClearSqlString();

protected:
	void ConstructL(const TDesC& aTableName);
	
protected:
	HBufC* 			iTableName;
	CDesCArrayFlat*	iParams;		
	CDesCArrayFlat*	iValues;
	HBufC*			iCondition;
	HBufC*			iSqlString;
	TBool			iProcessed;
	};

/**
Concrete implementation for sql update statement.

@internalComponent
@released
*/	
NONSHARABLE_CLASS(CCntSqlUpdate) : public CCntSqlStatement
	{
public:
	static CCntSqlUpdate* NewL(const TDesC& aTableName);
	TDesC& SqlStringL();
	};

/**
Concrete implementation for sql delete statement.

@internalComponent
@released
*/		
NONSHARABLE_CLASS(CCntSqlDelete) : public CCntSqlStatement
	{
public:
	static CCntSqlDelete* NewL(const TDesC& aTableName);
	TDesC& SqlStringL();
	};


/**
Concrete implementation for sql insert statement.

@internalComponent
@released
*/	
NONSHARABLE_CLASS(CCntSqlInsert) : public CCntSqlStatement
	{
public:
	static CCntSqlInsert* NewL(const TDesC& aTableName);
	TDesC& SqlStringL();
	};
	

/**
Concrete implementation for sql select statement.

@internalComponent
@released
*/		
NONSHARABLE_CLASS(CCntSqlSelect) : public CCntSqlStatement
	{
public:
	static CCntSqlSelect* NewL(const TDesC& aTableName);
	TDesC& SqlStringL();
	};
	
/**
Factory class. Used to build CCntSqlStatement objects based on statement type provided

@internalComponent
@released
*/
NONSHARABLE_CLASS(TSqlProvider)
	{
public:
	static CCntSqlStatement* GetSqlStatementL(TCntSqlStatementType aStatementType);		
	};	

#endif //__CNTSQLPROVIDER_H__

