/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "testcontactspbapexport.h"

CTestContactsPBAPExport::~CTestContactsPBAPExport()
/**
 * Destructor
 */
	{
	delete iExportObj;
	}

CTestContactsPBAPExport::CTestContactsPBAPExport()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestContactsPBAPExport);
	}

TVerdict CTestContactsPBAPExport::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Start export of vCards"));

	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTestContactsPBAPExport::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
	ExportContactsL();
	return TestStepResult();
	}

TVerdict CTestContactsPBAPExport::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CActiveScheduler::Install(NULL);
	delete iScheduler;

	INFO_PRINTF1(_L("Completed export of vCards "));
	return TestStepResult();
	}

void CTestContactsPBAPExport::ExportContactsL()
	{
	TInt err = KErrNone;
	// Retrieve the file name to which contact item is to be exported
   	RFs fsSession;
	RFileWriteStream writeStream;

	// connect to file system
	User::LeaveIfError(fsSession.Connect());
	CleanupClosePushL(fsSession);

	GetInputFromIni();

   	// Makes one or more directories.
   	fsSession.MkDirAll(iExportTo);

	// Replaces a single file with another
	User::LeaveIfError(writeStream.Replace(fsSession, iExportTo, EFileWrite));

	INFO_PRINTF1(_L("Exporting Contact....."));

	// Existing database
   	TPtrC databaseFile(_L("C:contactDb.cdb"));

	CContactDatabase* dBase = NULL;
	CContactIdArray* idArray = NULL;

	// Open the existing database
	dBase = CContactDatabase::OpenL(databaseFile);
	CleanupStack::PushL(dBase);

	// Create Utility class object, to export the contact from database
	CTestStep* self = static_cast<CTestStep*>(this);
	iExportObj = new(ELeave) CContactsPBAPExportUtilityClass(self);

	SetFilterL();

	CCntFilter* exportFilter = CCntFilter::NewL();
	CleanupStack::PushL(exportFilter);

	// Get all the contacts from the database to export
	exportFilter->SetContactFilterTypeCard(ETrue);
	dBase->FilterDatabaseL(*exportFilter);
	idArray = exportFilter->iIds;
	CleanupStack::PushL(idArray);

	if(iDamageDb)
		{
		#ifdef _DEBUG
		#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
		TRAPD(err1,dBase->DamageDatabaseL(0x666));
		if(err1 == KErrNone)
			{
			TRAPD(err,iExportObj->ExportvCardL(dBase, iStandard, idArray, writeStream, iContactFilter));
			INFO_PRINTF2(_L("Err:%d"),err);
			if(err == KErrNotReady)
				{
				SetTestStepResult(EPass);
				}
			else
				{
				SetTestStepResult(EFail);
				}
			if(dBase->IsDamaged())
				{
				dBase->RecoverL();
				}
			}
		else
			{
			INFO_PRINTF2(_L("Could not damage database Err:"),err1);
			}
		#else
			SetTestStepResult(EPass);
		#endif
		#endif
		}
    else
	    {
	    if(iInvalidFileSystem)
		    {
		    #ifdef _DEBUG
		    fsSession.SetErrorCondition(KErrNotReady);
		    TRAPD(err,iExportObj->ExportvCardL(dBase, iStandard, idArray, writeStream, iContactFilter));
			if(err == KErrNotReady)
				{
				SetTestStepResult(EPass);
				}
			else
				{
				SetTestStepResult(EFail);
				}
			fsSession.SetErrorCondition(KErrNone);
			#endif
			}
	    else
			{
			if(!iSetOOM)
				{
				if(idArray->Count() > 0)
					{
					for(TInt i=0; i<idArray->Count() ; i++)
						{
						TInt dCount = dBase->CountL();
						if(i>=dCount)
							{
							break;
							}

						// temporary array used to export one contact at a time
						CContactIdArray* tempIdArray = CContactIdArray::NewL();
						CleanupStack::PushL(tempIdArray);
						tempIdArray->AddL((*idArray)[i]);
						TRAPD(err,iExportObj->ExportvCardL(dBase, iStandard, tempIdArray, writeStream, iContactFilter));

						if(err != KErrNone )
							{
							if(err != KErrNotFound)
								{
								SetTestStepError(err);
								}
							}

						CleanupStack::PopAndDestroy(tempIdArray);
						}
					}
				else
					{
					if(idArray->Count()==0)
						{
						TRAPD(err,iExportObj->ExportvCardL(dBase, iStandard, idArray, writeStream, iContactFilter));
						if(err != KErrNone)
							{
							SetTestStepError(err);
							}
						}
					}

				}
			else
				{
				TInt tryCount = 1;
				for ( ;; )
					{
					__UHEAP_SETFAIL(RHeap::EDeterministic, tryCount);
					TRAP(err, iExportObj->ExportvCardL(dBase, iStandard, idArray, writeStream, iContactFilter));

					if ( err == KErrNone )
						{
						__UHEAP_RESET;
						INFO_PRINTF1(_L("OOM testing of CContactDatabase::ExportSelectedContactsL Api is done"));
						break;
						}
					if ( err != KErrNoMemory )
						{
						INFO_PRINTF2(_L("The unexpected error code is:%d"),err);
						SetTestStepResult(EFail);
						break;
						}
					__UHEAP_SETFAIL(RHeap::ENone, 0);
					tryCount++;
					}
				}
			}
	    }

	CleanupStack::Pop(idArray);
	CleanupStack::PopAndDestroy(exportFilter);

	INFO_PRINTF1(_L("Exported Contact"));
	writeStream.CommitL();
	writeStream.Close();

	INFO_PRINTF2(_L("Total number of contacts in database %d "), dBase->CountL());

	// Cleanup
	CleanupStack::PopAndDestroy(dBase);
    CleanupStack::PopAndDestroy(&fsSession);
	}

// Gets the input from ini file
void CTestContactsPBAPExport::GetInputFromIni()
	{
	iSetOOM = EFalse;
	GetStringFromConfig(ConfigSection(), KStandard, iStandard);
	GetStringFromConfig(ConfigSection(), KExportTo, iExportTo);
	GetStringFromConfig(ConfigSection(), KFilter, iFilter);
	// For OOM testing
	GetBoolFromConfig(ConfigSection(), KSetOOM, iSetOOM);
	GetBoolFromConfig(ConfigSection(), KDamageDb, iDamageDb);
	GetBoolFromConfig(ConfigSection(), KInvalidFileSystem, iInvalidFileSystem);
	GetBoolFromConfig(ConfigSection(), KFilterBitsFutureUse, iFilterBitsFutureUse);
	GetBoolFromConfig(ConfigSection(), KSetFilterOnlyFutureUse, iSetFilterOnlyFutureUse);
	}

// Sets the filter with the properties specified in ini file
void CTestContactsPBAPExport::SetFilterL()
	{
	if(iFilterBitsFutureUse)
		{
		iContactFilter = KFilterBitsForFutureUse;
		}
	else
		{
		if(iSetFilterOnlyFutureUse)
			{
			iContactFilter = KFilterBitsOnlyForFutureUse;
			}
		else
			{
			// Set the filter with the values given in ini file
			RArray<TPtrC>	filterList;
			CleanupClosePushL(filterList);
			iExportObj->TokenizeStringL(iFilter, filterList);

			iContactFilter = 0;
			const TInt count = filterList.Count();

			for(TInt i = 0;i < count;i++)
				{
				if(filterList[i].Compare(KVersion) == 0)
					{
					iContactFilter |= ETokenIdVersion;
					}
				if(filterList[i].Compare(KNameVal) == 0)
					{
					iContactFilter |= ETokenIdN;
					}
				if(filterList[i].Compare(KFirstName) == 0)
					{
					iContactFilter |= ETokenIdFn;
					}
				if(filterList[i].Compare(KTel) == 0)
					{
					iContactFilter |= ETokenIdTel;
					}
				if(filterList[i].Compare(KPhoto) == 0)
					{
					iContactFilter |= ETokenIdPhoto;
					}
				if(filterList[i].Compare(KBday) == 0)
					{
					iContactFilter |= ETokenIdBday;
					}
				if(filterList[i].Compare(KAdr) == 0)
					{
					iContactFilter |= ETokenIdAdr;
					}
				if(filterList[i].Compare(KLabel) == 0)
					{
					iContactFilter |= ETokenIdLabel;
					}
				if(filterList[i].Compare(KEmail) == 0)
					{
					iContactFilter |= ETokenIdEmail;
					}
				if(filterList[i].Compare(KMailer) == 0)
					{
					iContactFilter |= ETokenIdMailer;
					}
				if(filterList[i].Compare(KTitle) == 0)
					{
					iContactFilter |= ETokenIdTitle;
					}
				if(filterList[i].Compare(KRole) == 0)
					{
					iContactFilter |= ETokenIdRole;
					}
				if(filterList[i].Compare(KLogo) == 0)
					{
					iContactFilter |= ETokenIdLogo;
					}
				if(filterList[i].Compare(KOrg) == 0)
					{
					iContactFilter |= ETokenIdOrg;
					}
				if(filterList[i].Compare(KNote) == 0)
					{
					iContactFilter |= ETokenIdNote;
					}
				if(filterList[i].Compare(KRev) == 0)
					{
					iContactFilter |= ETokenIdRev;
					}
				if(filterList[i].Compare(KSound) == 0)
					{
					iContactFilter |= ETokenIdSound;
					}
				if(filterList[i].Compare(KUrl) == 0)
					{
					iContactFilter |= ETokenIdUrl;
					}
				if(filterList[i].Compare(KUid) == 0)
					{
					iContactFilter |= ETokenIdUid;
					}
				if(filterList[i].Compare(KKey) == 0)
					{
					iContactFilter |= ETokenIdKey;
					}
				if(filterList[i].Compare(KTZ) == 0)
					{
					iContactFilter |= ETokenIdTz;
					}
				if(filterList[i].Compare(KGeo) == 0)
					{
					iContactFilter |= ETokenIdGeo;
					}
				if(filterList[i].Compare(KAgent) == 0)
					{
					iContactFilter |= ETokenIdAgent;
					}
				}
			CleanupStack::Pop();
			filterList.Close();
			}
		}
	}

