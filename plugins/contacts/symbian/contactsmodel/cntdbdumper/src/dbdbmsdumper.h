/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CDbDbmsDumper_H__
#define __CDbDbmsDumper_H__

#include <e32std.h>
#include <e32def.h>
#include <f32file.h> 
#include <d32dbms.h> 
#include <badesca.h>

/**
CDbDbmsDumper class dumps the contents and columns of all tables in 
a given database in HTML format to a given location 
*/
class CDbDbmsDumper : public CBase
	{
public:  
	static CDbDbmsDumper* NewLC(const TDesC& aDbName, const TDesC& aOutputDir);
	~CDbDbmsDumper();
	TInt NumberOfTables() const;
	void OutputDBContentsL(); 
		                      
private:
    void ConstructL(const TDesC& aDbName, const TDesC& aOutputDir);  
	void OutputTableNameToFile(TInt aTableIndex);
	CDesCArray* OutputTableColHeadingsToFileLC(TInt aTableIndex);
	void OutputTableColDataToFileL(const CDesCArray& colNames);
	void OutputTableToFileL(TInt aTableIndex);

	void FieldHeaderColL(TInt aFieldIndex);
	void LongBinaryL(TInt aFieldIndex);
	void DumpStreamL(RReadStream& stream);


private: 
	RFs	iFsSession;
	RDbNamedDatabase iDatabase;
	RDbs iDbsSession;
	RDbTable iDbTable;
	CDbTableNames* iTableNames; 
	RFile iFile;
	HBufC8*	iBuffer;
};

#endif // __CDbDbmsDumper_H__
