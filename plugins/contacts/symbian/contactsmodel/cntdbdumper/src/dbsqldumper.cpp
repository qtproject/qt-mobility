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

#include "dbsqldumper.h"
#include "dbhtmltags.h"
#include "dbsqlconstants.h"
#include <e32test.h>
#include <s32mem.h>

_LIT(KErrorMessage, "DBSqlDumper Failed");	
const TInt KMaxPathLength = 100;
const TInt KBufferLength = 80;


/**
Class constructor
*/
CDbStructure::CDbStructure()
	{
	Init();	
	}
	
/**
Class destructor
*/
CDbStructure::~CDbStructure()	
	{
	iColumnsIndex.Reset();
	iNumColumns.Reset();	
	}

/**
Initialize table structure (number of tables, number of column in each table)

*/
void CDbStructure::Init()
	{	
	iTablesIndex = KInitialValue;
	iNumTables = KNumOfTables;
	iColumnsIndex.Reset();
	iNumColumns.Reset();
	//columns in pref table
	iColumnsIndex.Append(KInitialValue);
	iNumColumns.Append(KNumColInPrefTable);
	//columns in contact table
	iColumnsIndex.Append(KInitialValue);
	iNumColumns.Append(KNumColInContactTable);
	//columns in group table
	iColumnsIndex.Append(KInitialValue);
	iNumColumns.Append(KNumColInGroupTable);
	//columns in comm-address table
	iColumnsIndex.Append(KInitialValue);
	iNumColumns.Append(KNumColInCommTable);
	
	//columns in predictive search tables
	const TInt KAmountOfPredictiveSearchTables = 10;
	for (TInt i = 0; i < KAmountOfPredictiveSearchTables; ++i)
		{
		iColumnsIndex.Append(KInitialValue);
		iNumColumns.Append(KNumColInPredSearchTable);
		}
	}
		
/**
Moves the cursor forward to the next table

@return ETrue if the cursor can be moved to the next index or EFalse if the cursor
		is already at the end
*/		
TBool CDbStructure::NextTable()
	{
	++iTablesIndex;
	return iTablesIndex < iNumTables;
	}
	
/**
Return current table name in the cursor

*/	
const TDesC& CDbStructure::TableName()
	{
	return TableName(iTablesIndex);
	}
	
/**
Returns name of the table located at specified position in the table array

*/
const TDesC& CDbStructure::TableName(TInt aTableIndex)
	{
	switch(aTableIndex)
		{
		case EContactPrefTableName:
			{
			return KSqlContactPrefTableName;	
			}
		case EContactTableName:
			{
			return KSqlContactTableName;
			}
		case EContactGroupTableName:
			{
			return KSqlContactGroupTableName;	
			}
		case EContactCommAddressTableName:
			{
			return KSqlContactCommAddrTableName;		
			}

		case KContactPredSearchTable0Name:
			return KSqlContactPredSearchTable0;
		case KContactPredSearchTable1Name:
			return KSqlContactPredSearchTable1;
		case KContactPredSearchTable2Name:
			return KSqlContactPredSearchTable2;
		case KContactPredSearchTable3Name:
			return KSqlContactPredSearchTable3;
		case KContactPredSearchTable4Name:
			return KSqlContactPredSearchTable4;
		case KContactPredSearchTable5Name:
			return KSqlContactPredSearchTable5;
		case KContactPredSearchTable6Name:
			return KSqlContactPredSearchTable6;
		case KContactPredSearchTable7Name:
			return KSqlContactPredSearchTable7;
		case KContactPredSearchTable8Name:
			return KSqlContactPredSearchTable8;
		case KContactPredSearchTable9Name:
			return KSqlContactPredSearchTable9;
		case KContactPredSearchTable10Name:
			return KSqlContactPredSearchTable10;
		case KContactPredSearchTable11Name:
			return KSqlContactPredSearchTable11;

		default:
			{
			return KNullText;
			} 
		}
	}
	
/**
Moves the cursor forward to the next column in the specified table

@return ETrue if the cursor can be moved to the next index or EFalse if the cursor
		is already at the end
*/
TBool CDbStructure::NextColumn(TInt aTableIndex)
	{
	++iColumnsIndex[aTableIndex];
	return iColumnsIndex[aTableIndex] < iNumColumns[aTableIndex];
	}

/**
Return current column name for the specified table

@param	aTableIndex table index for which column name has to be returned
*/	
const TDesC& CDbStructure::Column(TInt aTableIndex)
	{
	return Column(aTableIndex, iColumnsIndex[aTableIndex]);	
	}
	
/**	
Return name of the column located at specified pozition in the specified table

*/
const TDesC& CDbStructure::Column(TInt aTableIndex, TInt aColumnIndex)
	{
	switch(aTableIndex)
		{
		case EContactPrefTableName: //columns in preference table
			switch(aColumnIndex)
				{
				case 0:
					{
					return KPrefPrefId;	
					}
				case 1:
					{
					return KPrefSchemaVersion;	
					}
				case 2:
					{
					return KPrefDatabaseUid;	
					}
				case 3:
					{
					return KPrefPrefCreationDate;	
					}
				case 4:
					{
					return KPrefMachineId;	
					}
				case 5:
					{
					return KPrefPreferredOrder;	
					}
				case 6:
					{
					return KPrefferredTemplateId;
					}
				}
			break;	
		case EContactTableName: //columns in contact table
			switch(aColumnIndex)
				{
				case 0:
					{
					return KContactId;
					}
				case 1:
					{
					return KContactTemplateId;
					}
				case 2:
					{
					return KContactTypeFlags;
					}
				case 3:
					{
					return KContactAccessCount;
					}
				case 4:
					{
					return KContactLastModified;
					}
				case 5:
					{
					return KContactGuidString;
					}
				case 6:
					{
					return KContactFirstName;
					}
				case 7:
					{
					return KContactLastName;
					}
				case 8:
					{
					return KContactCompanyName;
					}
				case 9:
					{
					return KContactFirstNamePrn;
					}
				case 10:
					{
					return KContactLastNamePrn;
					}
				case 11:
					{
					return KContactCompanyNamePrn;
					}
				case 12:
					{
					return KContactTextFieldHeader;
					}
				case 13:
					{
					return KContactBinaryFieldHeader;
					}
				case 14:
					{
					return KContactTextFields;
					}
				case 15:
					{
					return KContactBinaryFields;
					}
				}
			break;	
		case EContactGroupTableName: //columns in group table
			switch(aColumnIndex)
				{
				case 0:
					{
					return KGroupContactGroupId;
					}
				case 1:
					{
					return KGroupContactGroupId;
					}
				case 2:
					{
					return KGroupContactGroupMemberId;
					}
				}
			break;				
		case EContactCommAddressTableName: //columns in comm-address table
			switch(aColumnIndex)
				{
				case 0: 
					{
					return KCommAddrId;
					}
				case 1:
					{
					return KCommAddrType;
					}
				case 2:
					{
					return KCommAddrValue;
					}
				case 3:
					{
					return KCommAddrExtraValue;
					}
				case 4:
					{
					return KCommAddrContactId;
					}
				}
			break;
		case KContactPredSearchTable0Name: //columns in predictive search tables
		case KContactPredSearchTable1Name:
		case KContactPredSearchTable2Name:
		case KContactPredSearchTable3Name:
		case KContactPredSearchTable4Name:
		case KContactPredSearchTable5Name:
		case KContactPredSearchTable6Name:
		case KContactPredSearchTable7Name:
		case KContactPredSearchTable8Name:
		case KContactPredSearchTable9Name:
		case KContactPredSearchTable10Name:
		case KContactPredSearchTable11Name:
			switch(aColumnIndex)
				{
				case 0:
					return KPredSearchContactId;
				case 1:
					return KPredSearchNameAsNumber;
				case 2:
					return KPredSearchNameAsNumber2;
				case 3:
					return KPredSearchNameAsNumber3;
				case 4:
					return KPredSearchNameAsNumber4;
				case 5:
					return KPredSearchFirstName;
				case 6:
					return KPredSearchLastName;
				}
			break;
		}
		return KNullText;
	}
	
/**
Check if current column is the last column

*/	
TBool CDbStructure::IsLastColumn(TInt aTableIndex)
	{
	return iColumnsIndex[aTableIndex] == (iNumColumns[aTableIndex]-1);
	}
	
/**
Return number of columns specified table has

*/
TInt CDbStructure::ColumnNo(TInt aTableIndex)
	{
	return 	iNumColumns[aTableIndex];
	}

/**
Class destructor

*/	
CDbSqlDumper::~CDbSqlDumper()
	{
	delete iBuffer;
	iTableNames.Reset();
	iFile.Close();
	iDatabase.Close();
	iFsSession.Close();
	delete iDbStructure;
	}

/**
Second phase constructor for CDbSqlDumper class

@param  aDbName name of the database to be dumped.
@param	aOutputDir directory where html file will be generated		
*/	
void CDbSqlDumper::ConstructL(const TDesC& aDbName, const TDesC& aOutputDir)
	{
	// Output File Name	
	_LIT(KOutputFile,"DbDump.html");
	TBuf<KMaxPathLength> path(aOutputDir);
	path.Append(KOutputFile);
	
	User::LeaveIfError(iFsSession.Connect() );
	iDatabase.OpenL(aDbName);

	User::LeaveIfError(iFile.Replace(iFsSession, path, EFileWrite) );

	iBuffer = HBufC8::NewL(KBufferLength);
	iDbStructure = new (ELeave) CDbStructure();
	while(iDbStructure->NextTable())
		{
		iTableNames.AppendL(&(iDbStructure->TableName()));		
		}	
	}

/**
Return a concrete CDbSqlDumper object

@param  aDbName name of the database to be dumped.
@param	aOutputDir directory where html file will be generated		
*/	
CDbSqlDumper* CDbSqlDumper::NewLC(const TDesC& aDbName, const TDesC& aOutputDir)
	{
	CDbSqlDumper* self = new (ELeave) CDbSqlDumper();
	CleanupStack::PushL(self);
	self->ConstructL(aDbName, aOutputDir);
	return self;
	}
	
/**
Utility method used to build sql string. The result sql string will be
used to select from specified table

@param	aTableIndex	table index in the table array
*/	
void CDbSqlDumper::PrepareSQLStatementL(TInt aTableIndex)
	{
	//calculate the space needed to build the saq string
	iDbStructure->Init();
	TInt size = KSQLSelect().Size();
	while(iDbStructure->NextColumn(aTableIndex))
		{
		size += iDbStructure->Column(aTableIndex).Size() + 1;
		}
	size += KFrom().Size() + iDbStructure->TableName(aTableIndex).Size();	

	HBufC* sqlString = HBufC::NewLC( size );
	
	TPtr buffer = sqlString->Des();
	
	iDbStructure->Init();
	buffer.Append(KSQLSelect);
	while(iDbStructure->NextColumn(aTableIndex))
		{
		buffer.Append(iDbStructure->Column(aTableIndex));
		if(!iDbStructure->IsLastColumn(aTableIndex))	
			{
			buffer.Append(KComma);	
			}
		else	
			{
			buffer.Append(KSpace);	
			}		
		}
	buffer.Append(KFrom);	
	buffer.Append(iDbStructure->TableName(aTableIndex) );
	User::LeaveIfError(iSqlStatement.Prepare(iDatabase, buffer));
	
	CleanupStack::PopAndDestroy(sqlString);
	}

/**
Outputs one table values. Fetch all selected rows and output them in the
html file

@param	aTableIndex	table index in the table array
@leave  if there is an error at sql level, it will be forwarded
*/	
void CDbSqlDumper::OutputTableToFileL(TInt aTableIndex) 
	{
	TInt err;
	iFile.Write(KTable);
	OutputTableNameToFile(aTableIndex);
	OutputTableColHeadingsToFile(aTableIndex);
	PrepareSQLStatementL(aTableIndex);
	
	// output the table's rows
	while ((err =iSqlStatement.Next()) == KSqlAtRow )
		{
		iFile.Write(KRow);
		OutputTableColDataToFileL(aTableIndex);
		iFile.Write(KRowEnd);	
		}

	// check if there were any error
	if(err != KSqlAtEnd) 
		{
		iSqlStatement.Close();
		User::Leave(err);	
		}
	
	iSqlStatement.Close();
	iFile.Write(KTableEnd);
	}

/**
Outputs the table name in the html file

@param	aTableIndex	table index in the table array

*/	
void CDbSqlDumper::OutputTableNameToFile(TInt aTableIndex)
	{
	iFile.Write(KBold);	
	iBuffer->Des().Copy( *(iTableNames[aTableIndex]) ); // write iTableNames[aTableIndex] to the file.
	iFile.Write(*iBuffer);
	iFile.Write(KBoldEnd);
	}

/**
Outputs the header of the tables created in the html file. 
Header will contain column names

@param	aTableIndex	table index in the table array

*/
void CDbSqlDumper::OutputTableColHeadingsToFile(TInt aTableIndex)
	{
	iFile.Write(KHdRow);

	iDbStructure->Init();
	while(iDbStructure->NextColumn(aTableIndex))
		{
		iFile.Write(KCell);
		iBuffer->Des().Copy(iDbStructure->Column(aTableIndex));
		iFile.Write(*iBuffer);
		iFile.Write(KCellEnd);
		}

    iFile.Write(KRowEnd);
	}

/**
Outputs one row of a tables data.

@param	aTableIndex	table index in the table array

*/
void CDbSqlDumper::OutputTableColDataToFileL(TInt aTableIndex)
	{
	TInt counter = 0;
	const TInt KNumberOfCols( iDbStructure->ColumnNo(aTableIndex) );  

    // cols are from 1, to maxCols. not from 0 to KNumberOfCols-1.
	for (counter = 0; counter < KNumberOfCols; ++counter)   // for each column value in this row 
		{
		iFile.Write(KCell);

		if (iSqlStatement.IsNull(counter) )
			{
			iBuffer->Des().Format(KNullCol); // write out 'NULL' to the file
			}
		else
			{
			switch ( iSqlStatement.ColumnType(counter) )
				{
				case ESqlInt:
					{
					TInt32 val = iSqlStatement.ColumnInt(counter); 
					iBuffer->Des().Format(KInt32String, val);
					}
				break;
				case ESqlInt64:
					{
					TInt64 val = iSqlStatement.ColumnInt64(counter);
					iBuffer->Des().Format(KUInt64String, I64HIGH(val), I64LOW(val) );
					}
				break;
				case ESqlReal:
					{
					TReal32 val = iSqlStatement.ColumnReal(counter);
					iBuffer->Des().Format(KRealString, val);
					}
				break;
				case ESqlText:
					{
					iBuffer->Des().Copy(iSqlStatement.ColumnTextL(counter));
					}
				break;
				case ESqlBinary:
					if ((iDbStructure->Column(aTableIndex, counter) == KContactTextFieldHeader) ||
						(iDbStructure->Column(aTableIndex, counter) == KContactBinaryFieldHeader))
						{
						FieldHeaderColL(counter);
						}
					else 
						{
						LongBinaryL(counter);
						}
				break;
				default:
					iBuffer->Des().Format(KUnknownMessage);
				break;
				} // switch
			} // if

		iFile.Write(*iBuffer);
		iFile.Write(KCellEnd);
		
		} // for
	}

/**
Outputs a binary column/value representing header fields

@param	aFieldIndex	column index in the curently executed sql statement

*/
void CDbSqlDumper::FieldHeaderColL(TInt aFieldIndex)
	{
	TAutoClose<RSqlColumnReadStream> headerBlob;
	TAutoClose<RStoreReadStream> stream;

	headerBlob.iObj.ColumnBinary(iSqlStatement, aFieldIndex);
	CEmbeddedStore* headerStore = CEmbeddedStore::FromLC(headerBlob.iObj);
	stream.iObj.OpenLC(*headerStore, headerStore->Root());

	DumpStreamL(stream.iObj);

	CleanupStack::PopAndDestroy(&stream.iObj); //OpenLC
	CleanupStack::PopAndDestroy(headerStore);
	}

/**
Outputs a binary column/value representing binaryfields

@param	aFieldIndex	column index in the curently executed sql statement

*/
void CDbSqlDumper::LongBinaryL(TInt aFieldIndex)
	{
	TAutoClose<RSqlColumnReadStream> rdStream;
	rdStream.iObj.ColumnBinary(iSqlStatement,aFieldIndex);
	
	DumpStreamL(rdStream.iObj);
	}

/**
Utility method used to read from a read stream and write to file

@param	stream RReadStream from which information will be read

*/
void CDbSqlDumper::DumpStreamL(RReadStream& stream)
	{
	const TInt KStreamSize = stream.Source()->SizeL();

	iFile.Write(KDumpFont);

	_LIT8(KSizeMessage, "size is %d <BR>");
	_LIT8(KEightSpaces, "        ");
	_LIT8(KByteFormat, "%02x&nbsp;");
	_LIT8(KFormatString, "%S&nbsp;<BR>\r\n");
	
	iBuffer->Des().Format(KSizeMessage, KStreamSize);
	iFile.Write(*iBuffer);

	TBuf8<8> str(KEightSpaces); //Reserve 8 symbols

	TInt counter(0);
	while (counter < KStreamSize)
		{
		for(TInt i=0; i < 8; ++i)
			{
			if (counter >= KStreamSize)
				{
				break;	
				}
			//format the bynary value to a printable char	
			TInt8 byte( stream.ReadInt8L() );
			iBuffer->Des().Format(KByteFormat, byte);
			iFile.Write(*iBuffer);
			
			str[i] = (byte>32 ? byte : '.');
			++counter;
			}
		iBuffer->Des().Format(KFormatString, &str);
		iFile.Write(*iBuffer);
		}

	iFile.Write(KDumpFontEnd);
	}

/**
Output database content to a file

*/
void CDbSqlDumper::OutputDBContentsL()    
	{
	// Output Title
	_LIT8(KHdr,"<H2>Contacts Model DB Dump:</H2>");
	iFile.Write(KHdr);
		
	TInt counter = 0;
	const TInt KNumTables( iTableNames.Count() ); 
	
	// for each table - output all tables 
	for (counter = 0; counter < KNumTables; ++counter)
		{
		OutputTableToFileL(counter);
		}			
	}

void RunDumpL()
	{
// Location of Output Directory. (File will be called DbDump.html). Final "\" required on path.
_LIT(KOutputDir, "?:\\");
TBuf<sizeof(KOutputDir)> outputDir(KOutputDir);
outputDir[0] = (TUint8) RFs::GetSystemDriveChar();

// Location of Contacts DB
_LIT(KDBName, "?:\\private\\10003A73\\SQLite__contacts.cdb");
TBuf<sizeof(KDBName)> fileDBName(KDBName);
fileDBName[0] = (TUint8) RFs::GetSystemDriveChar();

	// dump sql database
	CDbSqlDumper* sqlDumper = CDbSqlDumper::NewLC(fileDBName, outputDir);   
    sqlDumper->OutputDBContentsL();   
	CleanupStack::PopAndDestroy(sqlDumper);   
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CActiveScheduler* scheduler = new CActiveScheduler;
	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);
		CTrapCleanup* cleanup = CTrapCleanup::New();
		if (cleanup)
			{
			TRAPD(err,RunDumpL() );	
			__ASSERT_ALWAYS(err == KErrNone, User::Panic(KErrorMessage,err) ); 
			delete cleanup;
			}
		delete scheduler;
		}
	__UHEAP_MARKEND;
	return KErrNone;
    }
