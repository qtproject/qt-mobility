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

#include <e32base.h>
#include <e32cons.h>
#include "cntsqlprovider.h"

/**
Constant descriptor representing 'update' sql key word. Used to build sql statements.

@internalComponent
@released
*/	
_LIT(KSqlUpdate, "UPDATE");


/**
Constant descriptor representing 'delete' sql key word. Used to build sql statements.

@internalComponent
@released
*/	
_LIT(KSqlDelete, "DELETE");


/**
Constant descriptor representing 'insert' sql key word. Used to build sql statements.

@internalComponent
@released
*/	
_LIT(KSqlInsert, "INSERT");


/**
Constant descriptor representing 'select' sql key word. Used to build sql statements.

@internalComponent
@released
*/	
_LIT(KSqlSelect, "SELECT");


/**
Constant descriptor used to build sql statements.

@internalComponent
@released
*/	
_LIT(KSqlWhere, "WHERE");


/**
Constant descriptor used to build sql statements.

@internalComponent
@released
*/	
_LIT(KSqlSet, "SET");


/**
Constant descriptor used to build sql statements.

@internalComponent
@released
*/	
_LIT(KSqlInto, "INTO");


/**
Constant descriptor used to build sql statements.

@internalComponent
@released
*/	
_LIT(KSqlValues, "VALUES");


/**
Constant descriptor used to build sql statements.

@internalComponent
@released
*/	
_LIT(KSqlFrom, "FROM");


/**
Constant descriptor used to build sql statements.

@internalComponent
@released
*/	
_LIT(KSpace, " ");


/**
Constant descriptor used to build sql statements.

@internalComponent
@released
*/	
_LIT(KEqual, " = ");


/**
Constant descriptor used to build sql statements.

@internalComponent
@released
*/	
_LIT(KSemicolon, ";");


/**
Constant descriptor used to build sql statements.

@internalComponent
@released
*/	
_LIT(KComma, ",");


/**
Constant descriptor used to build sql statements.

@internalComponent
@released
*/	
_LIT(KOpenBracket, "(");


/**
Constant descriptor used to build sql statements.

@internalComponent
@released
*/	
_LIT(KCloseBracket, ")");

/**
TStatement type constructor,

@param aSqlStatement Specifies the sql statement type: select, insert, delete, update.
@param aTableName 	 Table against which sql statement will be built

*/
TCntSqlStatementType::TCntSqlStatementType(TCntSqlStatement aSqlStatement, const TDesC& aTableName)
	:
	iSqlStatement(aSqlStatement), 
	iTableName(aTableName)
	{
	}

/**
TSqlStatement setter. Set the TSqlStatement held by this class,

@param aSqlStatement Specifies the sql statement type: select, insert, delete, update.

*/	
void TCntSqlStatementType::SetSqlStatement(TCntSqlStatement aSqlStatement)
	{
	iSqlStatement = aSqlStatement;	
	}
	
/**
TSqlStatement getter. Returns the sql statement type held by this class,

@return 	TSqlStatement type held by this class

*/
TCntSqlStatement TCntSqlStatementType::SqlStatement() const
	{
	return iSqlStatement;	
	}
	
/**
Table name getter. Returns the table's name held by this class,

@return 	TDesC containing the table's name held by this class

*/
const TDesC& TCntSqlStatementType::TableName() const
	{
	return iTableName;	
	}
	
/**
Factory method used to construct and retrieve CCntSqlStatement based objects. 
CCntSqlStatement objects are used to build and provide sql strings (sql statements) to be 
used in RSqlStatement.

@param aStatementType TStatementType containing thwe sql statement type requested and the
					  table name against which sql query will be run.

@return Pointer  to concrete CCntSqlStatement implementation used to retrieve sql strings.

*/		
CCntSqlStatement* TSqlProvider::GetSqlStatementL(TCntSqlStatementType aStatementType)
	{
	CCntSqlStatement* cntSqlStatement = NULL;
	
	switch(aStatementType.SqlStatement())
		{
		case EInsert:
			cntSqlStatement = CCntSqlInsert::NewL( aStatementType.TableName() );
			break;
		case EUpdate:
			cntSqlStatement = CCntSqlUpdate::NewL( aStatementType.TableName() );
			break;		
		case EDelete:
			cntSqlStatement = CCntSqlDelete::NewL( aStatementType.TableName() );
			break;
		case ESelect:
			cntSqlStatement = CCntSqlSelect::NewL( aStatementType.TableName() );
		}
	
	return(cntSqlStatement);
	}

/**
SQL parameter setter. Set value for a specified sql parameter,

@param aParam SQL parameter name
@param aValue SQL parameter value. 

@leave KErrNoMemory, an out of memory occured

@panic USER 0 panic; in debug mode if number of specified parameters is not equal with 
		number of parameter values
		
*/	
void CCntSqlStatement::SetParamL(const TDesC& aParam, const TDesC& aValue)
	{
	ASSERT( iParams->Count() == iValues->Count() ); // in debug mode check 
													// if number of provided params is equal 
													// with number of provided values

	ASSERT( iProcessed == EFalse ); // in debug mode check if the statement wasn't processed yet
	
	iParams->AppendL(aParam);
	iValues->AppendL(aValue);
	}
	
/**
Utility method used to retrieve the position of a given parameter.
The array of parameters is searched sequentially because we have to provide
the index of the parameter as it was inserted (so binary search is not usefull here)
Limit the usage of this method since it can be time consuming.

@param  aParam parameter name for which index has to be retrieved
@return index of the parameter if this is found in the parameter array (eg. previous set)
		or KErrNotFound if the parameter was not found
*/
TInt CCntSqlStatement::ParameterIndex(const TDesC& aParam) const
	{
	ASSERT( iParams->Count() == iValues->Count() ); // in debug mode check 
													// if number of provided params is equal 
													// with number of provided values
	for(TInt i = 0; i < iParams->Count(); ++i)
		{
		if(aParam.Compare((*iParams)[i]) == 0)
			{
			return i;				
			}
		}
		
	return KErrNotFound;
	}

/**		
SQL condition setter, Set condition for a sql statement (to be used in where clause)

@param aCondition condition value.

@leave KErrNoMemory, an out of memory occured

*/	
void CCntSqlStatement::SetConditionL(const TDesC& aCondition)
	{
	ClearCondition();
	iCondition = aCondition.AllocL();
	ClearSqlString();
	}

/**		
Clearing SQL condition from a sql statement 
*/	
void CCntSqlStatement::ClearCondition()
	{
	if(iCondition)
		{
		delete iCondition;	
		iCondition = NULL;
		ClearSqlString();
		} 
	}

	
/**
Remove all sql parameters prviously set and their values. Also reset previously set 
sql condition.

*/	
void CCntSqlStatement::Reset()
	{
	delete iCondition;	
	iCondition = NULL;
	
	ClearSqlString();
		
	ClearParams();
	}

void CCntSqlStatement::ClearParams()
    {
    ClearSqlString();
        
    iParams->Reset();
    iValues->Reset();
    }

/**
Second phase constructor for CCntSqlStatement based classes

@param aTableName table name against which sql statement will be run

@leave KErrNoMemory, an out of memory occured

*/		
void CCntSqlStatement::ConstructL(const TDesC& aTableName)
	{
	const TInt KGranularity = 5;
	
	iTableName = aTableName.AllocL();	
	iParams = new (ELeave) CDesCArrayFlat(KGranularity);
	iValues = new (ELeave) CDesCArrayFlat(KGranularity);	
	
	// Lazy initialisation for iSqlString; iSqlString is initialised in SqlStringL() method.
	// This is done because in SqlStringL() method we know what size buffer must have
	
	iProcessed = EFalse;
	}
			
/**
CCntSqlStatement destructor.

*/	
CCntSqlStatement::~CCntSqlStatement()
	{
	delete iTableName;	
	delete iCondition;	
	delete iSqlString;	
	
	if (iParams)
		{
		iParams->Reset();	
		delete iParams;	
		}
	
	if (iValues)
		{
		iValues->Reset();
		delete iValues;		
		}
	}
	
void CCntSqlStatement::SetTableName(HBufC* aTableName)
    {
    delete iTableName;
    iTableName = aTableName;
    
	// Table name changes, so SQL statement must be re-generated
	ClearSqlString();
    }

void CCntSqlStatement::ClearSqlString()
	{
	iProcessed = EFalse;
	delete iSqlString;
	iSqlString = NULL;
	}

/**
Creates a concrete CCntSqlUpdate object (used to retrieve sql update statements)

@param aTableName table name against which update statement will be run

@return concrete CCntSqlUpdate object

@leave KErrNoMemory, an out of memory occured

*/		
CCntSqlUpdate* CCntSqlUpdate::NewL(const TDesC& aTableName)
	{
	CCntSqlUpdate* self = new (ELeave) CCntSqlUpdate();
	CleanupStack::PushL(self );
	self->ConstructL(aTableName);
	CleanupStack::Pop(self);
	return self;
	}
	
/**
Return a string representing an update sql statement

@leave KErrArgument if the no parameter was provided
*/		
TDesC& CCntSqlUpdate::SqlStringL()
	{
	if(iProcessed)
		{
		return *iSqlString;	
		}
		
	ASSERT( iParams->Count() == iValues->Count() ); // in debug mode check 
													// if number of provided params is equal 
													// with number of provided values
	
	if(iParams->Count() == 0)
		{
		User::Leave( KErrArgument );	
		}
	
	// First calculate the buffer size
	TInt bufferSize = 0;
	bufferSize = KSqlUpdate().Size() + 
				iTableName->Size() + 
				KSpace().Size() + 
				KSqlSet().Size() + 
				KSpace().Size();
	
	for(TInt loop = 0; loop < iParams->Count(); ++loop) 
		{
		bufferSize += (*iParams)[loop].Size() + 
					KEqual().Size() + 
					(*iValues)[loop].Size();
		if(loop < iParams->Count() -1)
			{
			bufferSize += KComma().Size();		
			}
		}
	if(iCondition)
		{
		bufferSize += KSpace().Size() + 
					KSqlWhere().Size() + 
					KSpace().Size() + iCondition->Size();
		}
	bufferSize += KSemicolon().Size();
	
	// Allocate buffer
	iSqlString = HBufC::NewL( bufferSize );		
	
	TPtr dataPtr = iSqlString->Des();
	
	dataPtr.Append(KSqlUpdate);
	dataPtr.Append(KSpace);
	dataPtr.Append(*iTableName);
	dataPtr.Append(KSpace);
	dataPtr.Append(KSqlSet);
	dataPtr.Append(KSpace);
	
	for(TInt loop = 0; loop < iParams->Count(); ++loop) 
		{
		dataPtr.Append((*iParams)[loop]);
		dataPtr.Append(KEqual);
		dataPtr.Append((*iValues)[loop]);
		if(loop < iParams->Count() -1)
			{
			dataPtr.Append(KComma);		
			}
		}
	
	if(iCondition)
		{
		dataPtr.Append(KSpace);
		dataPtr.Append(KSqlWhere);
		dataPtr.Append(KSpace);	
		dataPtr.Append(*iCondition);
		}
	dataPtr.Append(KSemicolon);	

	iProcessed = ETrue;
	return *iSqlString;	
	}	

/**
Creates a concrete CCntSqlDelete object (used to retrieve sql delete statements)

@param aTableName table name against which delete statement will be run

@return concrete CCntSqlDelete object

@leave KErrNoMemory, an out of memory occured

*/		
CCntSqlDelete* CCntSqlDelete::NewL(const TDesC& aTableName)
	{
	CCntSqlDelete* self = new (ELeave) CCntSqlDelete();
	CleanupStack::PushL(self);
	self->ConstructL(aTableName);
	CleanupStack::Pop(self);
	return self;	
	}
	
/**
Return a string representing a delete sql statement

@leave	KErrArgument if condition is not set
*/			
TDesC& CCntSqlDelete::SqlStringL()
	{
	if(iProcessed) 
		{
		return *iSqlString;	
		}

	if(!iCondition)
		{
		User::Leave(KErrArgument);		
		}	
	
	// First calculate the buffer size
	TInt bufferSize = 0;
	bufferSize = KSqlDelete().Size() + 
				KSpace().Size() + 
				KSqlFrom().Size() +
				KSpace().Size() +
				iTableName->Size() +
				KSpace().Size() + 
				KSqlWhere().Size() +
				KSpace().Size() + 
				iCondition->Size() + 
				KSemicolon().Size();
	
	// Allocate buffer
	iSqlString = HBufC::NewL( bufferSize );
	
	TPtr dataPtr = iSqlString->Des();
	
	dataPtr.Append(KSqlDelete);
	dataPtr.Append(KSpace);
	dataPtr.Append(KSqlFrom);
	dataPtr.Append(KSpace);
	dataPtr.Append(*iTableName);
	
	dataPtr.Append(KSpace);
	dataPtr.Append(KSqlWhere);
	dataPtr.Append(KSpace);	
	dataPtr.Append(*iCondition);
	dataPtr.Append(KSemicolon);
	
	iProcessed = ETrue;
	return *iSqlString;	
	}

/**
Creates a concrete CCntSqlInsert object (used to retrieve sql insert statements)

@param aTableName table name against which insert statement will be run

@return concrete CCntSqlInsert object

@leave KErrNoMemory, an out of memory occured

*/			
CCntSqlInsert* CCntSqlInsert::NewL(const TDesC& aTableName)
	{
	CCntSqlInsert* self = new (ELeave) CCntSqlInsert();
	CleanupStack::PushL(self);
	self->ConstructL(aTableName);
	CleanupStack::Pop(self);
	return self;	
	}
	
/**
Return a string representing an insert sql statement

@leave 	KErrArgument if the no parameter was provided
*/			
TDesC& CCntSqlInsert::SqlStringL()
	{
	if(iProcessed)
		{
		return *iSqlString;	
		}

	ASSERT( iParams->Count() == iValues->Count() ); // in debug mode check 
													// if number of provided params is equal 
													// with number of provided values
	
	if(iParams->Count() == 0)
		{
		User::Leave( KErrArgument );	
		}		
	
	// First calculate the buffer size
	TInt bufferSize = 0;
	bufferSize = KSqlInsert().Size() + 
				KSpace().Size() +
				KSqlInto().Size() +
				KSpace().Size() + 
				iTableName->Size() + 
				KSpace().Size() +		
				KOpenBracket().Size();
	for(TInt loop = 0; loop < iParams->Count(); ++loop) 
		{
		bufferSize += (*iParams)[loop].Size() + (*iValues)[loop].Size();
		if(loop < iParams->Count() - 1) 
			{
			bufferSize += KComma().Size() + KComma().Size();		
			}
		}
	bufferSize += KCloseBracket().Size() + 
				KSpace().Size() + 
				KSqlValues().Size() +
				KSpace().Size() + 	
				KCloseBracket().Size() +
				KSemicolon().Size();
	
	// Allocate buffer
	iSqlString = HBufC::NewL( bufferSize );
	
	TPtr dataPtr = iSqlString->Des();
	
	dataPtr.Append(KSqlInsert);
	dataPtr.Append(KSpace);
	dataPtr.Append(KSqlInto);
	dataPtr.Append(KSpace);
	dataPtr.Append(*iTableName);
	dataPtr.Append(KSpace);
			
	dataPtr.Append(KOpenBracket);
	for(TInt loop = 0; loop < iParams->Count(); ++loop) 
		{
		dataPtr.Append((*iParams)[loop]);
		if(loop < iParams->Count() - 1) 
			{
			dataPtr.Append(KComma);		
			}
		}
		
	dataPtr.Append(KCloseBracket);
	
	dataPtr.Append(KSpace);
	dataPtr.Append(KSqlValues);
	dataPtr.Append(KSpace);
	
	dataPtr.Append(KOpenBracket);
	for(TInt loop = 0; loop < iValues->Count(); ++loop) 
		{
		dataPtr.Append((*iValues)[loop]);
		if(loop < iValues->Count() - 1)
			{
			dataPtr.Append(KComma);		
			}
		}
	
	dataPtr.Append(KCloseBracket);
	dataPtr.Append(KSemicolon);
	
	iProcessed = ETrue;
	
	return *iSqlString;	
	}
	
/**
Creates a concrete CCntSqlSelect object (used to retrieve sql select statements)

@param aTableName table name against which select statement will be run

@return concrete CCntSqlSelect object

@leave KErrNoMemory, an out of memory occured

*/			
CCntSqlSelect* CCntSqlSelect::NewL(const TDesC& aTableName)
	{
	CCntSqlSelect* self = new (ELeave) CCntSqlSelect();
	CleanupStack::PushL(self);
	self->ConstructL(aTableName);
	CleanupStack::Pop(self);
	return self;			
	}
	
/**
Return a string representing a select sql statement
When constructing simple select statement (against one single table) table name 
has to be provided.
For making joint selects a list of tables will be provided: table1, table2 as
table name.

@leave	KErrArgument if there is no argument provided
*/			
TDesC& CCntSqlSelect::SqlStringL()
	{
	if(iProcessed) 
		{
		return *iSqlString;	
		}
	
	if(iParams->Count() == 0)
		{
		User::Leave(KErrArgument);	
		}
	
	// First calculate the buffer size
	TInt bufferSize = 0;
	bufferSize = KSqlSelect().Size() +
				KSpace().Size();
	for(TInt loop = 0; loop < iParams->Count(); ++loop) 
		{
		bufferSize += (*iParams)[loop].Size();
		if(loop < iParams->Count() - 1)
			{
			bufferSize += KComma().Size();		
			}
		}	
	bufferSize += KSpace().Size() + 
				KSqlFrom().Size() +
				KSpace().Size() +
				iTableName->Size();
	if(iCondition)
		{
		bufferSize += KSpace().Size() + 
					KSqlWhere().Size() +
					KSpace().Size() + 
					iCondition->Size();
		}
	bufferSize += KSemicolon().Size();
	
	// Allocate buffer
	iSqlString = HBufC::NewL( bufferSize );
	TPtr dataPtr = iSqlString->Des();
	
	dataPtr.Append(KSqlSelect);
	dataPtr.Append(KSpace);

	for(TInt loop = 0; loop < iParams->Count(); ++loop) 
		{
		dataPtr.Append((*iParams)[loop]);
		if(loop < iParams->Count() - 1)
			{
			dataPtr.Append(KComma);		
			}
		}	
	
	dataPtr.Append(KSpace);
	
	dataPtr.Append(KSqlFrom);
	dataPtr.Append(KSpace);
	dataPtr.Append(*iTableName);

	if(iCondition)
		{
		dataPtr.Append(KSpace);
		dataPtr.Append(KSqlWhere);
		dataPtr.Append(KSpace);	
		dataPtr.Append(*iCondition);
		}
	dataPtr.Append(KSemicolon);	
			
	iProcessed = ETrue;
	
	return *iSqlString;	
	}
