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
* UpdateForSync Test module
*
*/

#include <e32test.h>
#include <cntdef.hrh>
#include <cntdef.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <e32svr.h>
#include <f32file.h>
#include "t_utils2.h"
#include "t_utils.h"

_LIT(KTestName, "T_UPDATEFORSYNC");
_LIT(KLogFileName, "UpdateForSyncProfile.log");

LOCAL_D RTest test(KTestName);


class CProfiler : public CBase
	{
public:
	static CProfiler* NewLC(RTest& aTest);
	~CProfiler();

	void StartProfile();
	void EndProfile();
private:
	RFs iFs;
	CLog* iLog;
	};

CProfiler* CProfiler::NewLC(RTest& aTest)
	{
	CProfiler* self=new(ELeave) CProfiler();
	CleanupStack::PushL(self);
	User::LeaveIfError(self->iFs.Connect());
	self->iLog=CLog::NewL(aTest,KLogFileName);
	return self;
	}

CProfiler::~CProfiler()
	{
	delete iLog;
	iFs.Close();
	}
	
void CProfiler::StartProfile()
	{
	CCntTest::ProfileReset(0,1);
	CCntTest::ProfileStart(0);
	}

void CProfiler::EndProfile()
	{
	CCntTest::ProfileEnd(0);
	TCntProfile profile[1];
	CCntTest::ProfileResult(profile,0,1);
	_LIT(KProfileFormat,"Total %d.%03d sec (%d hits)\n");
	RDebug::Print(KProfileFormat,profile[0].iTime/1000000, profile[0].iTime%1000000, profile[0].iCount);
	iLog->LogLine(KProfileFormat,profile[0].iTime/1000000, profile[0].iTime%1000000, profile[0].iCount);
	}




LOCAL_C void DoTestEmailKeepDefaultPropertyL()
	{
	_LIT(KFieldValue,"email@dummy.com");
	_LIT(KNewFieldValue,"newemail@dummy.com");


	CContactItemFieldSet* fieldSet=CContactItemFieldSet::NewLC();

	CContactItemField* field=CContactItemField::NewLC(KStorageTypeText);
	field->TextStorage()->SetTextL(KFieldValue());
	field->SetMapping(KUidContactFieldVCardMapEMAILINTERNET);
	field->AddFieldTypeL(KUidContactFieldEMail);
	field->AddFieldTypeL(KUidContactFieldVCardMapPREF);
	fieldSet->AddL(*field); //take ownership
	CleanupStack::Pop(field);
	
	field=CContactItemField::NewLC(KStorageTypeText);
	field->TextStorage()->SetTextL(KNewFieldValue());
	field->SetMapping(KUidContactFieldVCardMapEMAILINTERNET);
	field->AddFieldTypeL(KUidContactFieldEMail);

	
	fieldSet->UpdateFieldSyncL(*field,1);

	//Check that default property was not dropped
	__ASSERT_ALWAYS((*fieldSet)[0].ContentType().ContainsFieldType(KUidContactFieldVCardMapPREF),User::Leave(KErrGeneral));
	//Check that field was updated properly
	__ASSERT_ALWAYS((*fieldSet)[0].TextStorage()->Text()==KNewFieldValue(),User::Leave(KErrGeneral));

	
	CleanupStack::PopAndDestroy(2,fieldSet); //field
	}


LOCAL_C void DoTestPhoneKeepDefaultPropertyL()
	{
	_LIT(KFieldValue,"+44123456789");
	_LIT(KNewFieldValue,"+44987654321");

	CContactItemFieldSet* fieldSet=CContactItemFieldSet::NewLC();

	CContactItemField* field=CContactItemField::NewLC(KStorageTypeText);
	field->TextStorage()->SetTextL(KFieldValue());
	field->SetMapping(KUidContactFieldVCardMapTEL);
	field->AddFieldTypeL(KUidContactFieldPhoneNumber);
	field->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	field->AddFieldTypeL(KUidContactFieldVCardMapPREF);
	fieldSet->AddL(*field); //take ownership
	CleanupStack::Pop(field);
	
	field=CContactItemField::NewLC(KStorageTypeText);
	field->TextStorage()->SetTextL(KNewFieldValue());
	field->SetMapping(KUidContactFieldVCardMapTEL);
	field->AddFieldTypeL(KUidContactFieldPhoneNumber);
	field->AddFieldTypeL(KUidContactFieldVCardMapVOICE);

	fieldSet->UpdateFieldSyncL(*field,1);

	//Check that default property was not dropped
	__ASSERT_ALWAYS((*fieldSet)[0].ContentType().ContainsFieldType(KUidContactFieldVCardMapPREF),User::Leave(KErrGeneral));
	//Check that field was updated properly
	__ASSERT_ALWAYS((*fieldSet)[0].TextStorage()->Text()==KNewFieldValue(),User::Leave(KErrGeneral));
	
	CleanupStack::PopAndDestroy(2,fieldSet); //field
	}

LOCAL_C void DoTestPhoneKeepSpeedDialPropertyL()
	{
	_LIT(KFieldValue,"+44123456789");
	_LIT(KNewFieldValue,"+44987654321");

	CContactItemFieldSet* fieldSet=CContactItemFieldSet::NewLC();

	CContactItemField* field=CContactItemField::NewLC(KStorageTypeText);
	field->TextStorage()->SetTextL(KFieldValue());
	field->SetMapping(KUidContactFieldVCardMapTEL);
	field->AddFieldTypeL(KUidContactFieldPhoneNumber);
	field->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	field->AddFieldTypeL(KUidContactFieldVCardMapPREF);
	field->AddFieldTypeL(KUidSpeedDialFive);
	fieldSet->AddL(*field); //take ownership
	CleanupStack::Pop(field);
	
	field=CContactItemField::NewLC(KStorageTypeText);
	field->TextStorage()->SetTextL(KNewFieldValue());
	field->SetMapping(KUidContactFieldVCardMapTEL);
	field->AddFieldTypeL(KUidContactFieldPhoneNumber);
	field->AddFieldTypeL(KUidContactFieldVCardMapVOICE);

	fieldSet->UpdateFieldSyncL(*field,1);

	//Check that default property was not dropped
	__ASSERT_ALWAYS((*fieldSet)[0].ContentType().ContainsFieldType(KUidContactFieldVCardMapPREF),User::Leave(KErrGeneral));
	//Check that speed dial property was not dropped
	__ASSERT_ALWAYS((*fieldSet)[0].ContentType().ContainsFieldType(KUidSpeedDialFive),User::Leave(KErrGeneral));
	//Check that field was updated properly
	__ASSERT_ALWAYS((*fieldSet)[0].TextStorage()->Text()==KNewFieldValue(),User::Leave(KErrGeneral));
	
	CleanupStack::PopAndDestroy(2,fieldSet); //field
	}

void DoTestPhoneKeepVoiceDialPropertyL()
	{
	_LIT(KFieldValue,"+44123456789");
	_LIT(KNewFieldValue,"+44987654321");

	CContactItemFieldSet* fieldSet=CContactItemFieldSet::NewLC();

	CContactItemField* field=CContactItemField::NewLC(KStorageTypeText);
	field->TextStorage()->SetTextL(KFieldValue());
	field->SetMapping(KUidContactFieldVCardMapTEL);
	field->AddFieldTypeL(KUidContactFieldPhoneNumber);
	field->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	//field->AddFieldTypeL(KUidContactFieldVCardMapPREF);
	field->AddFieldTypeL(KUidContactsVoiceDialField);
	fieldSet->AddL(*field); //take ownership
	CleanupStack::Pop(field);
	
	field=CContactItemField::NewLC(KStorageTypeText);
	field->TextStorage()->SetTextL(KNewFieldValue());
	field->SetMapping(KUidContactFieldVCardMapTEL);
	field->AddFieldTypeL(KUidContactFieldPhoneNumber);
	field->AddFieldTypeL(KUidContactFieldVCardMapVOICE);

	fieldSet->UpdateFieldSyncL(*field,1);

	//Check that default property was not dropped
	//__ASSERT_ALWAYS((*fieldSet)[0].ContentType().ContainsFieldType(KUidContactFieldVCardMapPREF),User::Leave(KErrGeneral));
	//Check that speed dial property was not dropped
	__ASSERT_ALWAYS((*fieldSet)[0].ContentType().ContainsFieldType(KUidContactsVoiceDialField),User::Leave(KErrGeneral));
	//Check that field was updated properly
	__ASSERT_ALWAYS((*fieldSet)[0].TextStorage()->Text()==KNewFieldValue(),User::Leave(KErrGeneral));
	
	CleanupStack::PopAndDestroy(2,fieldSet); //field

	}


void DoProfilingL()
	{
	_LIT(KFieldValue,"+44123456789");
	_LIT(KNewFieldValue,"+44987654321");

	CProfiler* profiler=CProfiler::NewLC(test);
	
	CContactItemFieldSet* fieldSet=CContactItemFieldSet::NewLC();

	CContactItemField* field=CContactItemField::NewLC(KStorageTypeText);
	field->TextStorage()->SetTextL(KFieldValue());
	field->SetMapping(KUidContactFieldVCardMapTEL);
	field->AddFieldTypeL(KUidContactFieldPhoneNumber);
	field->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	field->AddFieldTypeL(KUidContactFieldVCardMapPREF);
	fieldSet->AddL(*field); //take ownership
	CleanupStack::Pop(field);

	field=CContactItemField::NewLC(KStorageTypeText);
	field->TextStorage()->SetTextL(KFieldValue());
	field->SetMapping(KUidContactFieldVCardMapTEL);
	field->AddFieldTypeL(KUidContactFieldPhoneNumber);
	field->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	field->AddFieldTypeL(KUidContactFieldVCardMapCELL);
	field->AddFieldTypeL(KUidContactFieldVCardMapPREF);
	fieldSet->AddL(*field); //take ownership
	CleanupStack::Pop(field);

	field=CContactItemField::NewLC(KStorageTypeText);
	field->TextStorage()->SetTextL(KFieldValue());
	field->SetMapping(KUidContactFieldVCardMapTEL);
	field->AddFieldTypeL(KUidContactFieldPhoneNumber);
	field->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	field->AddFieldTypeL(KUidContactFieldVCardMapCELL);
	field->AddFieldTypeL(KUidContactFieldVCardMapWORK);
	field->AddFieldTypeL(KUidContactFieldVCardMapPREF);
	fieldSet->AddL(*field); //take ownership
	CleanupStack::Pop(field);

	field=CContactItemField::NewLC(KStorageTypeText);
	field->TextStorage()->SetTextL(KFieldValue());
	field->SetMapping(KUidContactFieldVCardMapTEL);
	field->AddFieldTypeL(KUidContactFieldPhoneNumber);
	field->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	field->AddFieldTypeL(KUidContactFieldVCardMapWORK);
	field->AddFieldTypeL(KUidContactFieldVCardMapPREF);
	fieldSet->AddL(*field); //take ownership
	CleanupStack::Pop(field);

	
	field=CContactItemField::NewLC(KStorageTypeText);
	field->TextStorage()->SetTextL(KNewFieldValue());
	field->SetMapping(KUidContactFieldVCardMapTEL);
	field->AddFieldTypeL(KUidContactFieldPhoneNumber);
	//field->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	field->AddFieldTypeL(KUidContactFieldVCardMapWORK);

	

	profiler->StartProfile();
	for (TInt i=0;i<1000;i++)
		{
		fieldSet->UpdateFieldSyncL(*field,1);
		}
	profiler->EndProfile();
	CleanupStack::PopAndDestroy(3,profiler); //fieldset, field
	}

/**

@SYMTestCaseID     PIM-T-UPDATEFORSYNC-0001

*/

LOCAL_C void DoTestsL()
    {
	test.Start(_L("@SYMTESTCaseID:PIM-T-UPDATEFORSYNC-0001 T_UPDATEFORSYNC"));

	
	DoProfilingL();
	
	DoTestEmailKeepDefaultPropertyL();
	DoTestPhoneKeepDefaultPropertyL();
	DoTestPhoneKeepSpeedDialPropertyL();
	DoTestPhoneKeepVoiceDialPropertyL();


	test.End();
    }

GLDEF_C TInt E32Main()
	{
    // Init
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if (!cleanupStack)
        {
        return KErrNoMemory;
        }
    
	// Run the tests
	__UHEAP_MARK;
    TRAPD(err, DoTestsL());
	test.Close(); //avoid memory leak
	__UHEAP_MARKEND;
    // Cleanup

	if (err)
		{
		User::Panic(_L("T_UpdateForSync Failed !"),err);
		}

    delete cleanupStack;
	return err;
    }
