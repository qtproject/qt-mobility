/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
@SYMPREQ                  PREQ1187
@SYMComponent             app-engines_cntmodel
@SYMAuthor                Simon Mellor, JamesCl
@SYMTestStatus            Implemented
@SYMTestType              CT

@SYMTestCaseDesc          Tests the performance of an initial sync to an empty contacts database
						  by importing a corporate-profile database of 1000 contacts from a vcard
						  file. The test mimics the way the SyncML code performs and initial sync.

@SYMTestActions           Measures the time taken to import a vcard file of the 1000 contacts, 
						  using the following calls on the contacts database:
						  -- CContactDatabase::DatabaseBeginL()
						  -- CContactDatabase::DatabaseCommitL()
						  -- CContactDatabase::ImportContactsL()
						  -- CContactDatabase::CompactL()

@SYMTestExpectedResults   Test case will run without leaving and will output timing information.

*/

#include "t_perfinitialsync.h"
#include "../t_utils.h"
#include <cntvcard.h>
#include <cntitem.h>
#include <f32file.h>
#include <e32svr.h>
#include <coreappstest/testserver.h>

_LIT(KDbPathName,"c:\\private\\10003A73\\Contacts.cdb");
_LIT(KDatabaseName, "c:Contacts.cdb");
_LIT(KOutputFormat,"Initial Sync of %d contacts took: %d s %03d \n");

// After how many imports a CompactL() be called 
const TInt KCompactResolution = 32; // matches the SyncML code

CAddBench::CAddBench()
	{
	} 

CAddBench::~CAddBench()
	{
	delete iDb;
	iFs.Close();
	}  

CAddBench* CAddBench::NewLC(RTest& aTest)
	{
	CAddBench* self = new (ELeave) CAddBench();
	CleanupStack::PushL(self);
	self->ConstructL(aTest);
	return(self);
	}  

void CAddBench::ConstructL(RTest& aTest)
	{
	iTest = &aTest;
	User::LeaveIfError(iFs.Connect());

	_LIT(KInitialSyncTestTitle, "\nBeginning Initial Sync testing...\n");
	iTest->Printf(KInitialSyncTestTitle);
	
	RPIMTestServer serv;
	CleanupClosePushL(serv);
    User::LeaveIfError(serv.Connect());
    TRAPD (KErr, serv.DeleteFileL(KDbPathName));
    
	if (!( ((KErr == KErrNotFound) || (KErr == KErrNone)) || (KErr == KErrPathNotFound)))
		{
		User::Leave(KErr);
		}
	CleanupStack::PopAndDestroy(&serv);	
		
	CreateEmptyDBL();
	} 
	
/**
	This routine mimics calls made by CSmlContactsDba::ImportContactL v2 (called 
	from CSmlContactsDba::AddItemL).  
	The routine simluates single VCards being added one at a time.
*/
void CAddBench::ImportFileL(const TDesC& aFilename, TInt aNumEntries)
	{
	iNumContacts = aNumEntries;
	RFileReadStream aReadStream;
	CleanupClosePushL(aReadStream);
	
	User::LeaveIfError(aReadStream.Open(iFs,aFilename,EFileRead));

	iDb->DatabaseBeginL(EFalse);

	for (TInt counter = 0; counter < aNumEntries; ++counter)
		{		
		TBool importOK = ETrue;		
				
		// import one contact from the file
		CArrayPtr<CContactItem>* entryArray = iDb->ImportContactsL(TUid::Uid(KUidVCardConvDefaultImpl),
																	aReadStream,
																	importOK,
															    	(CContactVCardConverter::EImportSingleContact | 
															    		CContactVCardConverter::EConnectWhitespace | 
															    		CContactVCardConverter::EIgnoreUid));

		CleanupStack::PushL(TCleanupItem(CleanupPtrArray, entryArray));

		if (!importOK)
			{			
 				User::Leave(KErrNotFound);
			}
			
									
		if ((counter > 0) && ((counter % KCompactResolution) == 0) )
			{	
			iDb->DatabaseCommitL(EFalse);
			iDb->CompactL();
			iDb->DatabaseBeginL(EFalse);
			}
		CContactItem* contact = entryArray->At(0);
		iTime = contact->LastModified();					
		CleanupStack::PopAndDestroy(entryArray);			
		}

	iDb->DatabaseCommitL(EFalse);

	CleanupStack::PopAndDestroy(&aReadStream);	
	aReadStream.Close();
	}
	
/**
	Static convenience cleanup method copied from CSmlContactsDba  
*/
void CAddBench::CleanupPtrArray(TAny* aCArrayPtr)
	{ 
	CArrayPtr<CContactItem>* array = reinterpret_cast<CArrayPtr<CContactItem>*>(aCArrayPtr);
	array->ResetAndDestroy();
	delete array;
	}

/** 
	Replace the current db with an empty one - ie start from scratch
*/
void CAddBench::CreateEmptyDBL()
	{
	delete iDb;
	iDb = NULL;
	iDb = CContactDatabase::ReplaceL(KDatabaseName);
	}

void CAddBench::DoSyncL(TInt aEntryCount, const TDesC& aVCardLoc )
	{
	TFileName path;
	path.Append(aVCardLoc);	
	
	TCntPerfTimer timer;
	timer.StartTimer();	
	ImportFileL(aVCardLoc, aEntryCount);
	timer.StopTimer();	

	TInt result = timer.Result();	
	TBuf<64> formattable;
	formattable.Format(KOutputFormat, iNumContacts, result / 1000000, (result / 1000) % 1000);
	iTest->Printf(formattable);
	}
