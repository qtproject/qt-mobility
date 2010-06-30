/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32test.h>
#include <s32mem.h>
#include <badesca.h> //for cdescarray
#include <cntstd.h>
#include "dbdbmsdumper.h"
#include "dbhtmltags.h"
#include <coreappstest\testserver.h>

_LIT(KErrorMessage, "DBDbmsDumper Failed");
const TInt KMaxPathLength = 100;
const TInt KBufferLength = 80;

/**
Class destructor
*/
CDbDbmsDumper::~CDbDbmsDumper()
	{
	delete iBuffer;
	delete iTableNames;
	iFile.Close();
	iDatabase.Close();
	iDbsSession.Close();
	iFsSession.Close();
	}

/**
Second phase constructor for CDbDbmsDumper

@param  aDbName name of the database to be dumped.
@param	aOutputDir directory where html file will be generated		
*/	
void CDbDbmsDumper::ConstructL(const TDesC& aDbName, const TDesC& aOutputDir)
	{
	// Output File Name	
	_LIT(KOutputFile,"DbDump.html");
	TBuf<KMaxPathLength> path(aOutputDir);
	path.Append(KOutputFile);
	
	User::LeaveIfError(iFsSession.Connect());
	User::LeaveIfError(iDbsSession.Connect());

	// Copy the Contacts database from the contacts server private directory to 
	// the DBMS private directory
	RPIMTestServer serv;
	User::LeaveIfError(serv.Connect());
	
	_LIT(KDBContactsPrivFile, "?:\\private\\10003a73\\Contacts.cdb");
	_LIT(KDBDBMsPrivFile, "?:\\private\\100012a5\\DBS_100065FF_Contacts.cdb");
	
	TBuf<sizeof(KDBContactsPrivFile)> contactsDBPrivFilePath(KDBContactsPrivFile);
	TBuf<sizeof(KDBDBMsPrivFile)> dBMsPrivFilePath(KDBDBMsPrivFile);
	
	contactsDBPrivFilePath[0] = (TUint8) RFs::GetSystemDriveChar();
	dBMsPrivFilePath[0] = (TUint8) RFs::GetSystemDriveChar();
	
	serv.CopyFileL(contactsDBPrivFilePath, dBMsPrivFilePath);
	serv.Close();

	_LIT(KCntDbSecureFormat,"secure[100065FF]");

	// Contact databases are in DBMS private directory
	User::LeaveIfError(iDatabase.Open(iDbsSession, aDbName, KCntDbSecureFormat));
	User::LeaveIfError(iFile.Replace(iFsSession, path, EFileWrite) );

	iBuffer = HBufC8::NewL(KBufferLength);
	iTableNames = iDatabase.TableNamesL();
	}

/**
Return a concrete CDbDbmsDumper object

@param  aDbName name of the database to be dumped.
@param	aOutputDir directory where html file will be generated		
*/	
CDbDbmsDumper* CDbDbmsDumper::NewLC(const TDesC& aDbName, const TDesC& aOutputDir)
	{
	CDbDbmsDumper* self = new (ELeave) CDbDbmsDumper();
	CleanupStack::PushL(self);
	self->ConstructL(aDbName, aOutputDir);
	return self;
	}

/**
Returns number of tables that exist in the curent dbms database

*/
TInt CDbDbmsDumper::NumberOfTables() const
	{
	return iTableNames->Count();
	}

/**
Output one table to the html file. Traverse a table and output each row
in the html file

@param 	aTableIndex table index in the table array

*/
void CDbDbmsDumper::OutputTableToFileL(TInt aTableIndex) 
	{
	iFile.Write(KTable);
	OutputTableNameToFile(aTableIndex);
	CDesCArray* colNames = OutputTableColHeadingsToFileLC(aTableIndex);
	User::LeaveIfError(iDbTable.Open(iDatabase, (*iTableNames)[aTableIndex], RDbRowSet::EReadOnly) );
	User::LeaveIfError(iDbTable.SetNoIndex() );    // set rowset order as underlying row order       
	iDbTable.BeginningL();            // set cursor to start of table
	iDbTable.NextL();
	
	// output the table's rows
	while (!iDbTable.AtEnd() )
		{
		iFile.Write(KRow);
		OutputTableColDataToFileL(*colNames);
		iDbTable.NextL();
		iFile.Write(KRowEnd);	
		}

	iDbTable.Close();
	iFile.Write(KTableEnd);
	CleanupStack::PopAndDestroy(colNames);
	}

/**
Output table name to the html file

@param 	aTableIndex table index in the table array

*/
void CDbDbmsDumper::OutputTableNameToFile(TInt aTableIndex)
	{
	iFile.Write(KBold);	
	iBuffer->Des().Copy( (*iTableNames)[aTableIndex]); // write (*iTableNames)[aTableIndex] to the file.
	iFile.Write(*iBuffer);
	iFile.Write(KBoldEnd);
	}

/**
Outputs table header for the tables created in the html file
The header contains the column names

@param 	aTableIndex table index in the table array

*/
CDesCArray* CDbDbmsDumper::OutputTableColHeadingsToFileLC(TInt aTableIndex)
	{
	CDesCArraySeg* colNames = new (ELeave) CDesCArraySeg(8);
	CleanupStack::PushL(colNames);
	CDbColSet* colSet = iDatabase.ColSetL( (*iTableNames)[aTableIndex] );
	CleanupStack::PushL(colSet);
	const TInt KMaxCols( (colSet->Count() ) );  
	TInt counter = 0;
    iFile.Write(KHdRow);

	// cols are from 1, to maxCols. not from 0 to maxCols-1.
	for (counter = 1; counter <= KMaxCols; ++counter)
		{
		iFile.Write(KCell);
		iBuffer->Des().Copy( (*colSet)[counter].iName );
		colNames->AppendL((*colSet)[counter].iName);
		iFile.Write(*iBuffer);
		iFile.Write(KCellEnd);
		}

    iFile.Write(KRowEnd);
	CleanupStack::PopAndDestroy(colSet);
	return colNames;
	}

/**
Output a table row to the html file

@param colNames descriptor array containing rows to be written in the resulting html file

*/
void CDbDbmsDumper::OutputTableColDataToFileL(const CDesCArray& colNames)
	{
	TInt counter = 0;
	const TInt KNumberOfCols( (iDbTable.ColCount() ));  
	iDbTable.GetL(); // get the row to be output

    // cols are from 1, to maxCols. not from 0 to KNumberOfCols-1.
	for (counter = 1; counter <= KNumberOfCols; ++counter)   // for each column value in this row 
		{
		iFile.Write(KCell);

		if (iDbTable.IsColNull(counter) )
			{
			iBuffer->Des().Format(KNullCol); // write out 'NULL' to the file
			}
		else
			{
			switch ( iDbTable.ColType(counter) )
				{
				case EDbColInt8:
				case EDbColInt16:
				case EDbColInt32:
					{
					TInt32 val = iDbTable.ColInt32(counter); 
					iBuffer->Des().Format(KInt32String, val);
					}
				break;
				case EDbColBit:
				case EDbColUint8:
				case EDbColUint16:
				case EDbColUint32:
					{
					TUint32 val = iDbTable.ColUint32(counter); 
					iBuffer->Des().Format(KUInt32String, val);
					}
				break;
				case EDbColInt64:
					{
					TInt64 val = iDbTable.ColInt64(counter);
					iBuffer->Des().Format(KUInt64String, I64HIGH(val), I64LOW(val) );
					}
				break;
				case EDbColReal32:
					{
					TReal32 val = iDbTable.ColReal32(counter);
					iBuffer->Des().Format(KRealString, val);
					}
				break;
				case EDbColReal64:
					{
					TReal64 val = iDbTable.ColReal64(counter);
					iBuffer->Des().Format(KRealString, val);
					}
				break;
				case EDbColDateTime:
					{
					TTime val = iDbTable.ColTime(counter);
					TBuf<80> tmpBuffer;
					val.FormatL(tmpBuffer, KFormatDate);
					iBuffer->Des().Copy(tmpBuffer);
					}
				break;
				case EDbColBinary:
				case EDbColText8:
					{
					TPtrC8 val = iDbTable.ColDes8(counter);
					iBuffer->Des().Copy(val);
					}
				break;
				case EDbColText16:
					{
					TPtrC16 val = iDbTable.ColDes16(counter);
					iBuffer->Des().Copy(val);
					}
				break;
				case EDbColLongText8:
					iBuffer->Des().Format(KLongText8);   
				break;
				case EDbColLongText16:
					LongBinaryL(counter);
				break;
				case EDbColLongBinary:
					if (colNames[counter-1] == KFieldHeaderColName)
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
Output the header blob to resulting html file

@param aFieldIndex column index containing the header values in dbms database

*/
void CDbDbmsDumper::FieldHeaderColL(TInt aFieldIndex)
	{
	TAutoClose<RDbColReadStream> headerBlob;
	TAutoClose<RStoreReadStream> stream;

	headerBlob.iObj.OpenL(iDbTable, aFieldIndex);
	CEmbeddedStore* headerStore=CEmbeddedStore::FromLC(headerBlob.iObj);
	stream.iObj.OpenLC(*headerStore, headerStore->Root());

	DumpStreamL(stream.iObj);

	CleanupStack::PopAndDestroy(&stream.iObj); //OpenLC
	CleanupStack::PopAndDestroy(headerStore);
	}

/**
Output the values stored in binary blob fields in dbms database

@param aFieldIndex column index containing the binary values in dbms database

*/
void CDbDbmsDumper::LongBinaryL(TInt aFieldIndex)
	{
	TAutoClose<RDbColReadStream> rdStream;
	rdStream.iObj.OpenL(iDbTable,aFieldIndex);
	
	DumpStreamL(rdStream.iObj);
	}

/**
Utility method. Used to read from read stream and output to html file

*/
void CDbDbmsDumper::DumpStreamL(RReadStream& stream)
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
			//format byte in a printable character	
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
Dump the content of a dbms database in a html file

*/
void CDbDbmsDumper::OutputDBContentsL()    
	{
	// Output Title
	_LIT8(KHdr,"<H2>Contacts Model DB Dump:</H2>");
	iFile.Write(KHdr);
	
	// Preferences Table Name that will appear first in the output file
	_LIT(KPrefTableName,"PREFERENCES");
	
	TInt counter = 0;
	const TInt KNumTables( (iTableNames->Count() )); 
	TBool outputPrefTable( (EFalse) );
	
	// if a Table named Preferences Exists in the DB output it first
	while (!outputPrefTable && counter < KNumTables)
		{
	    if ((*iTableNames)[counter++].Compare(KPrefTableName) == 0)
			{
			OutputTableToFileL(--counter);
			outputPrefTable = ETrue;
	   		}
	    }
	
	// for each table - output all tables except the Preferences Table	
	for (counter = 0; counter < KNumTables; ++counter)
		{
		if ((*iTableNames)[counter].Compare(KPrefTableName) != 0)
			{
			OutputTableToFileL(counter);
			}
		}			
	}

void RunDumpL()
	{
	// Location of Output Directory. (File will be called DbDump.html). Final "\" required on path.   
	_LIT(KOutputDir, "?:\\");	
	TBuf<sizeof(KOutputDir)> outputDir(KOutputDir);
	outputDir[0] = (TUint8) RFs::GetSystemDriveChar();
	
	// Location of Contacts DB
	_LIT(KDBName, "?:Contacts.cdb");	
	TBuf<sizeof(KDBName)> fileDBName(KDBName);
	fileDBName[0] = (TUint8) RFs::GetSystemDriveChar();
	
	// dump dbms database 
	CDbDbmsDumper* dbmsDumper = CDbDbmsDumper::NewLC(fileDBName, outputDir);
    dbmsDumper->OutputDBContentsL();   
	CleanupStack::PopAndDestroy(dbmsDumper);   
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
