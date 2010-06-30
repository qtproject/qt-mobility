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

#ifndef __CDbSqlDumper_H__
#define __CDbSqlDumper_H__

#include <e32std.h>
#include <e32def.h>
#include <f32file.h>
#include <badesca.h>
#include <sqldb.h> 
#include <e32cmn.h>
#include <e32base.h>

/**
Utility class used to provide table names and column names existing in the sql database.
It is mapped to a specific sql schema.

*/
class CDbStructure : public CBase
	{
public:
	CDbStructure();
	~CDbStructure();
	void Init();
	TBool NextTable();
	const TDesC& TableName();
	const TDesC& TableName(TInt aTablePosition);
	TBool NextColumn(TInt aTableIndex);
	const TDesC& Column(TInt aTableIndex);
	const TDesC& Column(TInt aTableIndex, TInt aColumnIndex);
	TBool IsLastColumn(TInt aTableIndex);
	TInt ColumnNo(const TInt aTableIndex);
	
private:
	TInt iTablesIndex;
	TInt iNumTables;	
	RArray<TInt> iColumnsIndex;
	RArray<TInt> iNumColumns;
	};

/** 
CDbSqlDumper class dumps the contents and columns of all tables in 
a given sql database in HTML format to a given location 
*/
class CDbSqlDumper : public CBase
	{
public:  
   	static CDbSqlDumper* NewLC(const TDesC& aDbName, const TDesC& aOutputDir);
	~CDbSqlDumper();
	void OutputDBContentsL(); 
		                      
private: 	
    void ConstructL(const TDesC& aDbName, const TDesC& aOutputDir);  
	void OutputTableNameToFile(TInt aTableIndex);
	void OutputTableColHeadingsToFile(TInt aTableIndex);
	void OutputTableColDataToFileL(TInt aTableIndex);
	void OutputTableToFileL(TInt aTableIndex);

	void FieldHeaderColL(TInt aFieldIndex);
	void LongBinaryL(TInt aFieldIndex);
	void DumpStreamL(RReadStream& stream);

	void PrepareSQLStatementL(TInt aTableIndex);
private: 
	RFs	iFsSession;
	RSqlDatabase iDatabase;
	RSqlStatement iSqlStatement;
	RPointerArray<TDesC> iTableNames; 
	RFile iFile;
	HBufC8*	iBuffer;
	CDbStructure* iDbStructure;
	};
	
#endif // __CDbSqlDumper_H__
