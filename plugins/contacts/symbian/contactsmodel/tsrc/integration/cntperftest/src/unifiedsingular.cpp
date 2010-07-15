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


/**
 @file
 @publishedAll
 @released
*/

#include <e32panic.h>
#include <testexecutelog.h>
#include <cntfldst.h>
#include "unifiedsingular.h"
#include "clientserver.h"

_LIT(KNewFieldsTest,"NewFields");
_LIT(KTest1,"Adding empty new fields test...");

CUnifiedSingular::CUnifiedSingular()
			: CCntBaseStep(200)//Number of contacts
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(SharedConstants::KUnifiedSingular);
	}

TVerdict CUnifiedSingular::doTestStepL()
	{
	
//Comment out all the tests for sqlite version because it tests against the 
//field content based on fields position which is not part of cntmodel contract.
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__ 
	__UHEAP_MARK;
	
	InitializeL();
	
	iIterate->Reset();
	
	const TDesC &KSectionName = ConfigSection();
	

	if( KSectionName == KNewFieldsTest )
		{
		INFO_PRINTF1(KTest1);
		EmptyNewFieldTestL(iIterate->NextL());
		}
	else
		{
		MissngTestPanic();
		}
	Cleanup();
	
	__UHEAP_MARKEND;
#endif //__SYMBIAN_CNTMODEL_USE_SQLITE__
	
	return TestStepResult();
	}




/*performs a single set on a field, set label, add uid, or add new field, set value*/
void CUnifiedSingular::SetSingularL(const TContactItemId &aCid, const TInt &aPos, 
const TUnitType &aType, const TInt32 &aUid, const TDesC &aLabel, 
CContentType ** const aCtype, const TBool aEmpty)
	{
	CContactItem *cedit = NULL;
	cedit = iContactsDatabase->OpenContactLX(aCid,*iViewAll);
	CleanupStack::PushL(cedit);
	CContactItemFieldSet &fields = cedit->CardFields();
	TBool rptType = EFalse;
	const TFieldType &tfield = TFieldType::Uid(aUid);
	switch(aType)
		{
		case EAddUid:
			{
			CContactItemField &field = fields[aPos];
			rptType = field.ContentType().ContainsFieldType( tfield ) && (field.ContentType().Mapping() != tfield);
			field.AddFieldTypeL( tfield );
			*aCtype = CContentType::NewL( field.ContentType() );
			if(rptType)
				{
				(*aCtype)->RemoveFieldType(tfield);
				}
			CleanupStack::PushL(*aCtype);
			}
		break;
		case ESetLabel:
			SetLabelL(fields,aPos,aLabel);
		break;
		case EAddField:
			AddNewFieldL( fields, aLabel, TFieldType::Uid(aUid),aEmpty );
		break;
		case ESetFieldValue:
			SetFieldL(fields,aPos,aLabel);
		break;
		default:
			{
			_LIT(KSetSingular,"setsingular");
			User::Panic(KSetSingular,EInvariantFalse);
			}
		break;
		}
	iContactsDatabase->CommitContactL(*cedit);
	if(aCtype)
		{
		CleanupStack::Pop(*aCtype);
		}
	CleanupStack::PopAndDestroy(cedit);
	CleanupStack::Pop();
	cedit = NULL;
	}
	
/*performs a single check on a field, check label, check uid, or check new field*/
TBool CUnifiedSingular::CheckSingularL(const TContactItemId &aCid, const TInt &aPos, 
const TUnitType &aType, const TInt32 &aUid, const CContentType &aCtype, const TDesC &aLabel, 
const TBool aEmpty)
	{
	TBool ret=EFalse;
	
	CContactItem *cedit = NULL;
	cedit = iContactsDatabase->ReadContactLC(aCid,*iViewAll);
	CContactItemFieldSet &fields = cedit->CardFields();
	switch(aType)
		{
		case EAddUid:
			{
			_LIT(KInvalidField, "Following field contains incorrect uids:");
			const CContentType &ctype = fields[aPos].ContentType();
			ret = SINGLECOMPARE( CompareCTypes(ctype,aCtype),0,0 );
			if(!ret)
				{
				const TPtrC KFieldLabel = fields[aPos].Label();
				ERR_PRINTF1( KInvalidField);
				ERR_PRINTF1( KFieldLabel);
				}
			}
		break;
		case ESetLabel:
			ret = STRINGCOMPARE( fields[aPos].Label(), ==, aLabel,0,0 );
		break;
		case EAddField:
			ret = SINGLECOMPARE(  CheckNewFieldL( fields, aPos, TFieldType::Uid(aUid),aEmpty ),0,0 );
		break;
		default:
			{
			_LIT(KCheckSingular,"checksingular");
			User::Panic(KCheckSingular,EInvariantFalse);
			}
		break;
		}
	iContactsDatabase->CloseContactL(aCid);
	CleanupStack::PopAndDestroy(cedit);
	cedit = NULL;
	
	return ret;
	}
	
/*compares atype2 to atype1, must be equal length, and all atype2 items must be contained within
atype1*/
TBool CUnifiedSingular::CompareCTypes(const CContentType &aType1,const CContentType &aType2)
	{
	TBool ret = EFalse;
	TInt length = aType1.FieldTypeCount();
	if(length != aType2.FieldTypeCount() )
		{
		SINGLECOMPARE( ret,0,0 );
		return ret;
		}
	
	for(TInt i = 0; i<length; i++)
		{
		if( !aType1.ContainsFieldType(aType2.FieldType(i)) )
			{
			SINGLECOMPARE( ret, i,0 );
			return ret;
			}
		}
	ret = ETrue;
	return ret;
	}
	
/*
add new field without value, then commit, field is not stored
*/
void CUnifiedSingular::EmptyNewFieldTestL(const TContactItemId &aCid)
	{
	CContactItem *cedit = NULL;
	cedit = iContactsDatabase->ReadContactLC(aCid,*iViewAll);
	CContactItemFieldSet &fields = cedit->CardFields();
	TInt length = fields.Count();
	iContactsDatabase->CloseContactL(aCid);
	CleanupStack::PopAndDestroy(cedit);
	cedit = NULL;
	CContentType * ctype = NULL;
	
	for(int i=0, uids = TestConstants::KAltUID;i < length; uids++, i++)
		{
		SetSingularL(aCid,i,EAddField,uids,TestConstants::KLongString,NULL,ETrue);
		SINGLECOMPARE( CheckSingularL(aCid,length+i,EAddField,uids,*ctype,KNullDesC,ETrue), i,0 );
		}
		
	}



