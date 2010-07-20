/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Packaging Test module
*
*/



#include <e32test.h>
#include <coreappstest/testserver.h>

_LIT(KTestName, "T_Packager"); // Used by Testhelper

#include <cntfldst.h>
#include "testhelpers.h"
//#include "t_cnttest.h"

#include "nbcnttestlib/nbcnttestlib.h" 

#include "t_packager.h"
#include "t_cnttestimpldefs.h"
#include "ccntpackager.h"
#include <cntfilt.h>



//#define RUN_CNT
#define RUN_PL

////////////////////////////////////////////////////////////////////
// This test used to spit out a lot of html files which display
// the contents of contact items. There are a lot of files
// generated in the process which, as this is normally run on the
// overnight build, are not read and just take up disk space.
// Therefore, the call to the contact dumping method has been
// configured out by #ifdefs and needs to be manually re-activated.
//
// So, uncomment the following line to turn on contact dumping.
//#define __T_PACKAGER_DUMP_CONTACT__
////////////////////////////////////////////////////////////////////


#define KUidContactFieldCustom1Value		0x101FD209
#define KUidContactFieldCustom2Value		0x101FD20A
const TUid KUidContactFieldCustom1={KUidContactFieldCustom1Value};
const TUid KUidContactFieldCustom2={KUidContactFieldCustom2Value};

_LIT(KCreatePlPerform, "C:PlPerform.cdb");


void CPackagerTests::AllTestsL()
	{
	ContactItemTestL();
//	iCntTestImpl.CloseDatabase();
	EmptyContactItemTestL();
//	ContentTypeTestL();	
	CntItemViewDefTestL();
	CntViewDefTestL();	
	DescArrayTestL();
//	CContactIdArrayTestL();
	CCntFilterTestL();
	CntTextDefTestL();
//	MessageBufferTestL();

	iCntTestImpl.CloseDatabase();
	}

void CPackagerTests::ContactItemTestL()
	{
	CCntPackager* thePackage = CCntPackager::NewL();
	CleanupStack::PushL(thePackage);
	
	test.Next(_L("ContactItemTestL"));


	iCntTestImpl.CreateDatabaseL(nsCntTestImpl::KSecureDBName, ETrue);

	const CContactTemplate& goldenTemplate = iCntTestImpl.GetSysTemplate();
	//iTempl = iCntTestImpl.GetSysTemplate();

#ifdef __T_PACKAGER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.DumpContactL(goldenTemplate, _L("Template")));
#endif

	CContactItem* card = CContactCard::NewLC(&goldenTemplate);
	TCnt theCnt(card);

	theCnt[KUidContactFieldGivenName] = _L("David");
	theCnt[KUidContactFieldFamilyName] = _L("Coulthard");

	theCnt[KUidContactFieldPhoneNumber] & KHome & KVoice & KCell = _L("mo1 39849342");
	theCnt[KUidContactFieldPhoneNumber] & KHome & KVoice & KCell += _L("mo2 34530903495803948503984545");

	theCnt[KUidContactFieldEMail] & KHome = _L("home@email");
	theCnt[KUidContactFieldAddress] & KHome = _L("Home addresssssss");
	theCnt[KUidContactFieldLocality] & KHome = _L("Glazgo");
	theCnt[KUidContactFieldIMAddress] & KHome & KWv = _L("wireless@village");
	theCnt[KUidContactFieldSecondName] = _L("Second name");

	theCnt[KUidContactFieldCompanyName] = _L("McLaren");
	theCnt[KUidContactFieldJobTitle]	= _L("Driver");
	theCnt[KUidContactFieldPhoneNumber] & KWork & KVoice = _L("work Tel 2342");

	theCnt[KUidContactFieldEMail] & KWork = _L("work@email");
	theCnt[KUidContactFieldUrl] & KWork = _L("sdfsd");
	theCnt[KUidContactFieldAddress] & KWork = _L("afas df asd");
	theCnt[KUidContactFieldLocality] & KWork = _L("sdfsd");

	theCnt[KUidContactFieldCustom1] = _L("Filterable 1");
	theCnt[KUidContactFieldCustom2] = _L("Filterable 2");

	theCnt[KUidContactFieldGivenNamePronunciation] = _L("first name re");
	theCnt[KUidContactFieldFamilyNamePronunciation] = _L("Last name re");
	
	theCnt[KUidContactFieldSIPID] & KPoc  = _L("SIP POC");
	theCnt[KUidContactFieldSIPID] & KSwis = _L("SIP SWIS");
	theCnt[KUidContactFieldSIPID] & KVoip = _L("SIP VOIP");

#ifdef __T_PACKAGER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.DumpContactL(*card, _L("Before Creation")));
#endif

	iCntTestImpl.CreateL(*card);	
	
	// For now, call this to allocate memory for buffer in packager.
	TPtrC8 theBuffer = thePackage->GetTransmittingBuffer();
	
	TInt err  = KErrNoMemory;
	while(err==KErrNoMemory)
	{
		
		TRAPD(error, (theBuffer.Set(thePackage->PackL(*card))));
		err = error;
		if(error==KErrNoMemory)
			{
			thePackage->GetTransmittingBuffer();
			
			}
	}
	
	CContactItem* cntItem = thePackage->UnpackCntItemLC();
//	CleanupStack::PushL(cntItem);
	err = CompareContactItemTestL(cntItem, card);
	User::LeaveIfError(err);
	CleanupStack::PopAndDestroy(3, thePackage);
//	POPD(cntItem);
//	POPD(card);
//	POPD(thePackage);

	
	}

void CPackagerTests::EmptyContactItemTestL()
	{
	CCntPackager* thePackage = CCntPackager::NewL();
	CleanupStack::PushL(thePackage);
	
	test.Next(_L("EmptyContactItemTestL"));


	CPersistenceLayer* iPersistLayer = CPersistenceLayer::NewLC(iFs);
	CContactItemViewDef* cntItemVdef = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields, CContactItemViewDef::EMaskHiddenFields);
	cntItemVdef->AddL(KUidContactFieldMatchAll);
	
	MLplContactsFile& cntFile = iPersistLayer->ContactsFileL();

	TRAPD(err, cntFile.CreateL(KCreatePlPerform, true ? MLplContactsFile::EPlOverwrite : MLplContactsFile::EPlLeaveIfExist));
	User::LeaveIfError(err);
	TRAP(err, while(cntFile.OpenStepL(KCreatePlPerform)){});
	User::LeaveIfError(err);
	
	CContactItem* card = NULL;
	card = iPersistLayer->PersistenceBroker().ReadLC(KGoldenTemplateId, *cntItemVdef, EPlAllInfo, 0);

	// For now, call this to allocate memory for buffer in packager.
	TPtrC8 theBuffer = thePackage->GetTransmittingBuffer();
	
	err  = KErrNoMemory;
	while(err==KErrNoMemory)
		{
		
		TRAPD(error, (theBuffer.Set(thePackage->PackL(*card))));
		err = error;
		if(error==KErrNoMemory)
			{
			thePackage->GetTransmittingBuffer();
			
			}
		}

	CContactItem* cntItem = thePackage->UnpackCntItemLC();
	err = CompareContactItemTestL(cntItem, card);
	User::LeaveIfError(err);
	CleanupStack::PopAndDestroy(5, thePackage);
	
	}

void CPackagerTests::ContentTypeTestL()
	{
	CCntPackager* thePackage = CCntPackager::NewL();
	CleanupStack::PushL(thePackage);
	
	test.Next(_L("ContentTypeTestL"));


	iCntTestImpl.CreateDatabaseL(nsCntTestImpl::KSecureDBName, ETrue);

	const CContactTemplate& goldenTemplate = iCntTestImpl.GetSysTemplate();
	//iTempl = iCntTestImpl.GetSysTemplate();

#ifdef __T_PACKAGER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.DumpContactL(goldenTemplate, _L("Template")));
#endif


	CContactItem* card = CContactCard::NewLC(&goldenTemplate);
	TCnt theCnt(card);

	theCnt[KUidContactFieldGivenName] = _L("David");
	theCnt[KUidContactFieldFamilyName] = _L("Coulthard");

	theCnt[KUidContactFieldPhoneNumber] & KHome & KVoice & KCell = _L("mo1 39849342");
	theCnt[KUidContactFieldPhoneNumber] & KHome & KVoice & KCell += _L("mo2 34530903495803948503984545");

	theCnt[KUidContactFieldEMail] & KHome = _L("home@email");
	theCnt[KUidContactFieldAddress] & KHome = _L("Home addresssssss");
	theCnt[KUidContactFieldLocality] & KHome = _L("Glazgo");
	theCnt[KUidContactFieldIMAddress] & KHome & KWv = _L("wireless@village");
	theCnt[KUidContactFieldSecondName] = _L("Second name");

	theCnt[KUidContactFieldCompanyName] = _L("McLaren");
	theCnt[KUidContactFieldJobTitle]	= _L("Driver");
	theCnt[KUidContactFieldPhoneNumber] & KWork & KVoice = _L("work Tel 2342");

	theCnt[KUidContactFieldEMail] & KWork = _L("work@email");
	theCnt[KUidContactFieldUrl] & KWork = _L("sdfsd");
	theCnt[KUidContactFieldAddress] & KWork = _L("afas df asd");
	theCnt[KUidContactFieldLocality] & KWork = _L("sdfsd");

	theCnt[KUidContactFieldCustom1] = _L("Filterable 1");
	theCnt[KUidContactFieldCustom2] = _L("Filterable 2");

	theCnt[KUidContactFieldGivenNamePronunciation] = _L("first name re");
	theCnt[KUidContactFieldFamilyNamePronunciation] = _L("Last name re");
	
	theCnt[KUidContactFieldSIPID] & KPoc  = _L("SIP POC");
	theCnt[KUidContactFieldSIPID] & KSwis = _L("SIP SWIS");
	theCnt[KUidContactFieldSIPID] & KVoip = _L("SIP VOIP");

#ifdef __T_PACKAGER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.DumpContactL(*card, _L("Before Creation")));
#endif
	
//	iCntTestImpl.CreateL(*card);
	
	// Get the Content Type from the contact item for testing.
	// Contact Item Field Set	
	CContactItemFieldSet& item1Fieldset = card->CardFields();
	//CContactItemField* item1Field = &(item1Fieldset)[0];
	TInt item1Fieldset_Count = item1Fieldset.Count();
	CContentType* cntType1 = CContentType::NewL(item1Fieldset[0].ContentType());
	CleanupStack::PushL(cntType1);
		
	if(item1Fieldset_Count<=0)
		{
		return;
		}		
	
	// For now, call this to allocate memory for buffer in packager.
	TPtrC8 theBuffer = thePackage->GetTransmittingBuffer();
	
	TInt err  = KErrNoMemory;
	while(err==KErrNoMemory)
	{
		
		TRAPD(error, (theBuffer.Set(thePackage->PackL(*cntType1))));
		err = error;
		if(error==KErrNoMemory)
			{
			thePackage->GetTransmittingBuffer();
			
			}
	}
	
	CContentType* cntType2 = thePackage->UnpackCntContentTypeLC();
	err = CompareContentTypeTestL(*cntType1, *cntType2);
	User::LeaveIfError(err);
	CleanupStack::PopAndDestroy(4, thePackage);

	}	
	

void CPackagerTests::CntTextDefTestL()
	{
	CCntPackager* thePackage = CCntPackager::NewL();
	CleanupStack::PushL(thePackage);	
	test.Next(_L("CntTextDefTestL"));


	// To Do: Generate Test CContactTextDef object.
	CContactTextDef* textDef1 = CContactTextDef::NewLC();
	
	//TContactViewPreferences preferences =  static_cast<TContactViewPreferences> (EContactsOnly | EUnSortedAtEnd | ESingleWhiteSpaceIsEmptyField);
	
	textDef1->AppendL(TContactTextDefItem(KUidContactFieldFamilyName));
	textDef1->AppendL(TContactTextDefItem(KUidContactFieldGivenName));
	textDef1->AppendL(TContactTextDefItem(KUidContactFieldCompanyName));
	textDef1->AppendL(TContactTextDefItem(KUidContactFieldAddress));
	textDef1->AppendL(TContactTextDefItem(KUidContactFieldEMail));
	textDef1->AppendL(TContactTextDefItem(KUidContactFieldAdditionalName));

	// For now, call this to allocate memory for buffer in packager.
	TPtrC8 theBuffer = thePackage->GetTransmittingBuffer();
	
	TInt err = KErrNoMemory;
	while(err==KErrNoMemory)
		{
		TRAPD(error, (theBuffer.Set(thePackage->PackL(*textDef1))));
		err = error;
		if(error==KErrNoMemory)
			{
			thePackage->GetTransmittingBuffer();
			
			}
		}

	CContactTextDef* textDef2 = thePackage->UnpackCntTextDefLC();
	err = CompareCntTextDefL(textDef1, textDef2);
	User::LeaveIfError(err);
	CleanupStack::PopAndDestroy(3, thePackage);
	
	}

TInt CPackagerTests::CompareContactItemTestL(CContactItem* item1, CContactItem* item2)
	{
	
	// Contact Item type
	if(item1->Type()!=item2->Type())
		{
		return KErrNotFound;
		}
		
	// Contact Item Field Set	
	CContactItemFieldSet& item1Fieldset = item1->CardFields();
	CContactItemFieldSet& item2Fieldset = item2->CardFields();
	TInt item1Fieldset_Count = item1Fieldset.Count();
	TInt item2Fieldset_Count = item2Fieldset.Count();
	if(item1Fieldset_Count!=item2Fieldset_Count)
		{
		return KErrNotFound;
		}	
		
	else // Same count!
		{
		for(TInt i=0; i<item1Fieldset_Count; i++)
			{
			if(CompareContactItemFieldTestL(&(item1Fieldset)[i], &(item2Fieldset)[i])!=KErrNone)
				{
				return KErrNotFound;	
				}
			
			}
		}

	// Contact item's iAttributes
	if(item1->iAttributes!=item2->iAttributes)
		{
		return KErrNotFound;
		}
	
	// Contact item's ID
	if(item1->Id()!=item2->Id())
		{
		return KErrNotFound;
		}		
		
	// Contact item's TemplateRefId
	if(item1->TemplateRefId()!=item2->TemplateRefId())
		{
		return KErrNotFound;
		}	
	
	// Contact item's LastModified
	if(item1->LastModified()!=item2->LastModified())
		{
		return KErrNotFound;
		}					
			
	// Contact item's iCreationDate
	if(item1->iCreationDate!=item2->iCreationDate)
		{
		return KErrNotFound;
		}
		
	// Contact item's iAccessCount
	if(item1->AccessCount()!=item2->AccessCount())
		{
		return KErrNotFound;
		}		
		
	// Contact item's iGuid
	if((item1->iGuid==NULL)||(item2->iGuid==NULL))
		{
		if(!((item1->iGuid==NULL)&&(item2->iGuid==NULL)))
			{
			return KErrNotFound;
			}		
		}
	else	
		{
		if(item1->iGuid->Length()!=item2->iGuid->Length())
			{
			return KErrNotFound;
			}	
			
		else
			{
			if(item1->iGuid->Compare(item2->iGuid->Des())!=0)
				{
				return KErrNotFound;
				}	
			}		
		}
		
	// Contact item's group if applicable.
	// We already check type above.
	if((item1->Type()==KUidContactGroup)||
	   (item1->Type()==KUidContactICCEntry)||
	   (item1->Type()==KUidContactCard)||
	   (item1->Type()==KUidContactOwnCard))
		{
		CContactCard* card1 = (CContactCard*) item1;
		CContactCard* card2 = (CContactCard*) item2;
		if((card1->GroupsJoined()==NULL)||(card2->GroupsJoined()==NULL))
			{
			if(!((card1->GroupsJoined()==NULL)&&(card2->GroupsJoined()==NULL)))
				{
				return KErrNotFound;
				}
				
			}
		else
			{
			CContactIdArray* array1 = card1->GroupsJoinedLC();
			CContactIdArray* array2 = card2->GroupsJoinedLC();
			if(array1->Count()!=array2->Count())
				{
				return KErrNotFound;
				}	
				
			// Checked count above already.
			for(TInt i=0;i<array1->Count();i++)			
				{
				if((*array1)[i]!=(*array2)[i])
					{
					return KErrNotFound;
					}				
				}
			CleanupStack::PopAndDestroy(2);	
			}
		}
			
		// Contact item's items if applicable.
	if(item1->Type()==KUidContactGroup)
		{
		CContactGroup* group1 = (CContactGroup*) item1;
		CContactGroup* group2 = (CContactGroup*) item2;
		if((group1->GroupsJoined()==NULL)||(group2->GroupsJoined()==NULL))
			{
			if(!((group1->GroupsJoined()==NULL)&&(group2->GroupsJoined()==NULL)))
				{
				return KErrNotFound;
				}					
			}	
			
		else
			{
			CContactIdArray* array1 = group1->GroupsJoinedLC();
			CContactIdArray* array2 = group2->GroupsJoinedLC();
			if(array1->Count()!=array2->Count())
				{
				return KErrNotFound;
				}	
				
			// Checked count above already.
			for(TInt i=0;i<array1->Count();i++)			
				{
				if((*array1)[i]!=(*array2)[i])
					{
					return KErrNotFound;
					}				
				}
			CleanupStack::PopAndDestroy(2);	
			}						
		}
						
	
	return KErrNone;
	
	}
	
TInt CPackagerTests::CompareContactItemFieldTestL(CContactItemField* itemField1, CContactItemField* itemField2)
	{
	// CContentType *iContentType;
	/*if(!(itemField1->ContentType()==itemField2->ContentType()))
		{
		return KErrNotFound;
		}*/
	if(CompareContentTypeTestL(itemField1->ContentType(), itemField2->ContentType())!=KErrNone)
		{
		return KErrNotFound;
		}
		
	// TStorageType iStorageType;
	if(itemField1->StorageType()!=itemField2->StorageType())
		{
		return KErrNotFound;
		}
				
	// HBufC* iLabel;	
	if((itemField1->iLabel==NULL)||(itemField2->iLabel==NULL))
		{
		if(!((itemField1->iLabel==NULL)&&(itemField2->iLabel==NULL)))
			{
			return KErrNotFound;
			}		
		}	
	else
		{
		if(itemField1->iLabel->Length()!=itemField2->iLabel->Length())
			{
			return KErrNotFound;
			}
		else
			{
			if(itemField1->iLabel->Compare(itemField2->iLabel->Des())!=0)
				{
				return KErrNotFound;
				}
			}				
		}
		
	// TInt iId;
	if(itemField1->iId!=itemField2->iId)
		{
		return KErrNotFound;
		}
		
	// TUint32 iAttributes;
	if(itemField1->iAttributes!=itemField2->iAttributes)
		{
		return KErrNotFound;
		}	
	
	// TUint32 iExtendedAttributes;
	if(itemField1->iExtendedAttributes!=itemField2->iExtendedAttributes)
		{
		return KErrNotFound;
		}	
	
	// CContactFieldStorage* iStorage;
    switch (itemField1->StorageType())
        {
    case KStorageTypeText:
		if(itemField1->TextStorage()->Text()!=itemField2->TextStorage()->Text())
			{
			return KErrNotFound;
			}			
        break;
        
    case KStorageTypeStore:
		if(itemField1->StoreStorage()->Thing()!=itemField2->StoreStorage()->Thing())			
			{
			return KErrNotFound;
			}			
        break;
        
    case KStorageTypeDateTime:
		if(itemField1->DateTimeStorage()->Time()!=itemField2->DateTimeStorage()->Time())
			{
			return KErrNotFound;
			}
        break;
        
    case KStorageTypeContactItemId:
		if(itemField1->AgentStorage()->Value()!=itemField2->AgentStorage()->Value())
			{
			return KErrNotFound;
			}        
        break;
        
    default:
    	User::Leave(KErrNotSupported);
    	break;
        }
        	
	// TInt iTemplateFieldId;	
	if(itemField1->iTemplateFieldId!=itemField2->iTemplateFieldId)
		{
		return KErrNotFound;
		}		
	
	return KErrNone;
	}
	
TInt CPackagerTests::CompareContentTypeTestL(const CContentType& contentType1, const CContentType& contentType2)
	{
	// CContentType *iContentType;
	if(!(contentType1==contentType2))
		{
		return KErrNotFound;
		}
	
	
	return KErrNone;
	}
	
TInt CPackagerTests::CompareCntTextDefL(CContactTextDef* textDef1, CContactTextDef* textDef2)
	{
	// CContactTextDef*
	TInt textDef1_Count = textDef1->Count();
	TInt textDef2_Count = textDef2->Count();	
	if(textDef1_Count!=textDef2_Count)
		{
		return KErrNotFound;
		}	
		
	else // Same count!
		{
		for(TInt i=0; i<textDef1_Count; i++)
			{
			if((*textDef1)[i].iFieldType.iUid!=(*textDef2)[i].iFieldType.iUid)
				{
				return KErrNotFound;
				}
				
			if((*textDef1)[i].iSeperator!=(*textDef2)[i].iSeperator)
				{
				return KErrNotFound;
				}				
			}
		}
		
	if(textDef1->FallbackField()!=textDef2->FallbackField())
		{
		return KErrNotFound;
		}
		
	if(textDef1->ExactMatchOnly()!=textDef2->ExactMatchOnly())
		{
		return KErrNotFound;
		}		

	return KErrNone;
	}


void CPackagerTests::DescArrayTestL()
	{
	test.Next(_L("DescArrayTestL"));

	CCntPackager* thePackage = CCntPackager::NewL();
	CleanupStack::PushL(thePackage);
	
	// Create a Descriptor Array.
	CDesCArray* testArray1 = new(ELeave) CDesCArrayFlat(5);
	CleanupStack::PushL(testArray1);
	testArray1->AppendL(_L("Symbian"));
	testArray1->AppendL(_L("Software"));
	testArray1->AppendL(_L("Ltd."));
	testArray1->AppendL(_L("United Kingdom"));
	testArray1->AppendL(_L("SE1"));

	// For now, call this to allocate memory for buffer in packager.
	TPtrC8 theBuffer = thePackage->GetTransmittingBuffer();
	TInt err  = KErrNoMemory;
	while(err==KErrNoMemory)
		{
					
		TRAPD(error, (theBuffer.Set(thePackage->PackL(*testArray1))));
		err = error;
		if(error==KErrNoMemory)
			{
			thePackage->GetTransmittingBuffer();
			
			}
		}
		
	CDesCArray* testArray2 = thePackage->UnpackDesCArrayLC();
//	CleanupStack::PushL(testArray2);
	err = CompareDescArrayTestL(testArray2, testArray1);
	User::LeaveIfError(err);
		
	CleanupStack::PopAndDestroy(3, thePackage);
	
	}	
	
TInt CPackagerTests::CompareDescArrayTestL(CDesCArray* testArray1, CDesCArray* testArray2)
	{				
	// CDesCArray	
	if((testArray1==NULL)||(testArray2==NULL))
		{
		if(!((testArray1==NULL)&&(testArray2==NULL)))
			{
			return KErrNotFound;
			}		
		}	
		
	else
		{
		if(testArray1->Count()!=testArray2->Count())
			{
			return KErrNotFound;
			}
		else
			{
			for(TInt i=0; i<testArray1->Count(); i++)
				{
				if((*testArray1)[i]!=(*testArray2)[i])
					{
					return KErrNotFound;
					}
				}
			}				
		}
		
	
	return KErrNone;
	}	
	

void CPackagerTests::CntItemViewDefTestL()
	{
	test.Next(_L("CntItemViewDefL"));

	CCntPackager* thePackage = CCntPackager::NewL();
	CleanupStack::PushL(thePackage);
	
	// Create a populated CContactItemViewDef object.
	CContactItemViewDef* viewDef1=CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EMaskHiddenFields);
	viewDef1->AddL(KUidContactFieldGivenName);
	viewDef1->AddL(KUidContactFieldFamilyName);
	viewDef1->AddL(KUidContactFieldPhoneNumber);
	viewDef1->AddL(KUidContactFieldCompanyName);
	viewDef1->AddL(KUidContactFieldAddress);
	test(viewDef1->Count()==5);
	test((*viewDef1)[0]==KUidContactFieldGivenName);
	test((*viewDef1)[1]==KUidContactFieldFamilyName);
	test((*viewDef1)[2]==KUidContactFieldPhoneNumber);
	test((*viewDef1)[3]==KUidContactFieldCompanyName);
	test((*viewDef1)[4]==KUidContactFieldAddress);

	// For now, call this to allocate memory for buffer in packager.
	TPtrC8 theBuffer = thePackage->GetTransmittingBuffer();
	TInt err  = KErrNoMemory;
	while(err==KErrNoMemory)
		{
					
		TRAPD(error, (theBuffer.Set(thePackage->PackL(*viewDef1))));
		err = error;
		if(error==KErrNoMemory)
			{
			thePackage->GetTransmittingBuffer();
			
			}
		}
	
	CContactItemViewDef* viewDef2 = thePackage->UnpackCntItemViewDefLC();
	err = CompareCntItemViewDefTestL(viewDef1, viewDef2);
	User::LeaveIfError(err);
		
	CleanupStack::PopAndDestroy(3, thePackage);
	
	}	
	

void CPackagerTests::CntViewDefTestL()
	{
	test.Next(_L("CntViewDefL"));

	CCntPackager* thePackage = CCntPackager::NewL();
	CleanupStack::PushL(thePackage);
	
	// Create a populated CContactItemViewDef object.
	CContactItemViewDef* viewDef1=CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EMaskHiddenFields);
	// CleanupStack::PushL(viewDef1);
	viewDef1->AddL(KUidContactFieldGivenName);
	viewDef1->AddL(KUidContactFieldFamilyName);
	viewDef1->AddL(KUidContactFieldPhoneNumber);
	viewDef1->AddL(KUidContactFieldCompanyName);
	viewDef1->AddL(KUidContactFieldAddress);
	test(viewDef1->Count()==5);
	test((*viewDef1)[0]==KUidContactFieldGivenName);
	test((*viewDef1)[1]==KUidContactFieldFamilyName);
	test((*viewDef1)[2]==KUidContactFieldPhoneNumber);
	test((*viewDef1)[3]==KUidContactFieldCompanyName);
	test((*viewDef1)[4]==KUidContactFieldAddress);
	
	// Create CContactViewDef
	CContactViewDef* def1=CContactViewDef::NewL(viewDef1);
	CleanupStack::Pop(viewDef1); // viewDef1
	CleanupStack::PushL(def1);
	
	// For now, call this to allocate memory for buffer in packager.
	TPtrC8 theBuffer = thePackage->GetTransmittingBuffer();
	TInt err  = KErrNoMemory;
	while(err==KErrNoMemory)
		{
					
		TRAPD(error, (theBuffer.Set(thePackage->PackL(*def1))));
		err = error;
		if(error==KErrNoMemory)
			{
			thePackage->GetTransmittingBuffer();
			
			}
		}
	
	CContactViewDef* def2 = thePackage->UnpackCntViewDefLC();
	
	err = CompareCntViewDefTestL(def1, def2);
	User::LeaveIfError(err);
	
	CleanupStack::PopAndDestroy(3, thePackage);	

	}	
	
void CPackagerTests::MessageBufferTestL()
	{
	test.Next(_L("MessageBufferTestL"));

	CCntPackager* thePackage = CCntPackager::NewL();
	CleanupStack::PushL(thePackage);
	
	// Create a Descriptor Array.
	CDesCArray* testArray1 = new(ELeave) CDesCArrayFlat(5);
	CleanupStack::PushL(testArray1);
	testArray1->AppendL(_L("Symbian"));
	testArray1->AppendL(_L("Software"));
	testArray1->AppendL(_L("Ltd."));
	testArray1->AppendL(_L("United Kingdom"));
	testArray1->AppendL(_L("SE1"));

	// For now, call this to allocate memory for buffer in packager.
	TPtr8 theBuffer(NULL, NULL, NULL);
	TInt err  = KErrNoMemory;
	while(err==KErrNoMemory)
		{
					
		TRAPD(error, (theBuffer.Set(thePackage->PackL(*testArray1))));
		err = error;
		if(error==KErrNoMemory)
			{
			thePackage->GetTransmittingBuffer();
			
			}
		}
		
	RMessage2 testMessage;		
//	TDesC8& desBuf(theBuffer);
	TInt length = theBuffer.Length();
	TInt size = theBuffer.Size();
	
//	HBufC8* data = theBuffer.AllocLC();// HBufC8::NewLC(2000);
	
//	TPtr8 theBufferClone(data->Des());
//	theBufferClone.Copy(theBuffer.Ptr());
	length = theBuffer.Length();
	size = theBuffer.Size();	
	testMessage.WriteL(0, theBuffer);
	testMessage.Complete(KErrNone);
	
	// Reset buffer and set buffer from the Message
	thePackage->Clear();
	thePackage->SetBufferFromMessageL(testMessage);

		
	CDesCArray* testArray2 = thePackage->UnpackDesCArrayLC();
//	CleanupStack::PushL(testArray2);
	err = CompareDescArrayTestL(testArray2, testArray1);
	User::LeaveIfError(err);
		
	CleanupStack::PopAndDestroy(3, thePackage);
	}
	

void CPackagerTests::CContactIdArrayTestL()
	{
	test.Next(_L("CContactIdArrayTestL"));

	CCntPackager* thePackage = CCntPackager::NewL();
	CleanupStack::PushL(thePackage);
	
	// Create a CContactIdArrayTestL Array from a contact item.
	iCntTestImpl.CreateDatabaseL(nsCntTestImpl::KSecureDBName, ETrue);

	const CContactTemplate& goldenTemplate = iCntTestImpl.GetSysTemplate();

#ifdef __T_PACKAGER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.DumpContactL(goldenTemplate, _L("Template")));
#endif
	
	CContactItem* card = CContactCard::NewLC(&goldenTemplate);
	TCnt theCnt(card);

	theCnt[KUidContactFieldGivenName] = _L("David");
	theCnt[KUidContactFieldFamilyName] = _L("Coulthard");

	theCnt[KUidContactFieldPhoneNumber] & KHome & KVoice & KCell = _L("mo1 39849342");
	theCnt[KUidContactFieldPhoneNumber] & KHome & KVoice & KCell += _L("mo2 34530903495803948503984545");

	theCnt[KUidContactFieldEMail] & KHome = _L("home@email");
	theCnt[KUidContactFieldAddress] & KHome = _L("Home addresssssss");
	theCnt[KUidContactFieldLocality] & KHome = _L("Glazgo");
	theCnt[KUidContactFieldIMAddress] & KHome & KWv = _L("wireless@village");
	theCnt[KUidContactFieldSecondName] = _L("Second name");

	theCnt[KUidContactFieldCompanyName] = _L("McLaren");
	theCnt[KUidContactFieldJobTitle]	= _L("Driver");
	theCnt[KUidContactFieldPhoneNumber] & KWork & KVoice = _L("work Tel 2342");

	theCnt[KUidContactFieldEMail] & KWork = _L("work@email");
	theCnt[KUidContactFieldUrl] & KWork = _L("sdfsd");
	theCnt[KUidContactFieldAddress] & KWork = _L("afas df asd");
	theCnt[KUidContactFieldLocality] & KWork = _L("sdfsd");

	theCnt[KUidContactFieldCustom1] = _L("Filterable 1");
	theCnt[KUidContactFieldCustom2] = _L("Filterable 2");

	theCnt[KUidContactFieldGivenNamePronunciation] = _L("first name re");
	theCnt[KUidContactFieldFamilyNamePronunciation] = _L("Last name re");
	
	theCnt[KUidContactFieldSIPID] & KPoc  = _L("SIP POC");
	theCnt[KUidContactFieldSIPID] & KSwis = _L("SIP SWIS");
	theCnt[KUidContactFieldSIPID] & KVoip = _L("SIP VOIP");

#ifdef __T_PACKAGER_DUMP_CONTACT__
	TESTTRAP(iCntTestImpl.DumpContactL(*card, _L("Before Creation")));
#endif

	iCntTestImpl.CreateL(*card);
	
	// Create second contact item
	CPersistenceLayer* iPersistLayer = CPersistenceLayer::NewLC(iFs);
	CContactItemViewDef* cntItemVdef = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields, CContactItemViewDef::EMaskHiddenFields);
	cntItemVdef->AddL(KUidContactFieldMatchAll);
	
	MLplContactsFile& cntFile = iPersistLayer->ContactsFileL();

	TRAPD(err, cntFile.CreateL(KCreatePlPerform, true ? MLplContactsFile::EPlOverwrite : MLplContactsFile::EPlLeaveIfExist));
	User::LeaveIfError(err);
	TRAP(err, while(cntFile.OpenStepL(KCreatePlPerform)){});
	User::LeaveIfError(err);
	
	CContactItem* card2 = NULL;
	card2 = iPersistLayer->PersistenceBroker().ReadLC(KGoldenTemplateId, *cntItemVdef, EPlAllInfo, 0);		
		
	
	// Create CContactIdArray
	CContactIdArray* theIdArray1 = CContactIdArray::NewLC();
	theIdArray1->AddL(card->Id());
	theIdArray1->AddL(card->TemplateRefId());
	theIdArray1->AddL(card2->Id());
	theIdArray1->AddL(card2->TemplateRefId());	

	// For now, call this to allocate memory for buffer in packager.
	TPtrC8 theBuffer(NULL, NULL);
	TInt err2  = KErrNoMemory;
	while(err2==KErrNoMemory)
		{
		TRAPD(error, (theBuffer.Set(thePackage->PackL(*theIdArray1))));
		err2= error;
		if(error==KErrNoMemory)
			{
			thePackage->GetTransmittingBuffer();
			
			}
		}
		
	CContactIdArray* theIdArray2 = thePackage->UnpackCntIdArrayLC();
	err2 = CompareCContactIdArrayTestL(theIdArray1, theIdArray2);
	User::LeaveIfError(err2);

	CleanupStack::PopAndDestroy(7, thePackage);
	}
	
TInt CPackagerTests::CompareCContactIdArrayTestL(CContactIdArray* idArray1, CContactIdArray* idArray2)
	{				
	// CContactIdArray
	if((idArray1==NULL)||(idArray2==NULL))
		{
		if(!((idArray1==NULL)&&(idArray2==NULL)))
			{
			return KErrNotFound;
			}		
		}	
		
	else
		{
		if(idArray1->Count()!=idArray2->Count())
			{
			return KErrNotFound;
			}
		else
			{
			for(TInt i=0; i<idArray1->Count(); i++)
				{
				if((*idArray1)[i]!=(*idArray2)[i])
					{
					return KErrNotFound;
					}
				}
			}				
		}

	return KErrNone;
	}		
	
TInt CPackagerTests::CompareCntItemViewDefTestL(CContactItemViewDef* viewDef1, CContactItemViewDef* viewDef2)
	{				
	// CContactItemViewDef	

	if(viewDef1->Mode()!=viewDef2->Mode())
		{
		return KErrNotFound;
		}		
	
	if(viewDef1->Use()!=viewDef2->Use())
		{
		return KErrNotFound;
		}		

	if(viewDef1->Count()!=viewDef2->Count())
		{
		return KErrNotFound;
		}
	else
		{
		for(TInt i=0; i<viewDef1->Count(); i++)
			{
			if((*viewDef1)[i]!=(*viewDef2)[i])
				{
				return KErrNotFound;
				}
			}
		}						
	
	return KErrNone;
	}
	
TInt CPackagerTests::CompareCntViewDefTestL(CContactViewDef* def1, CContactViewDef* def2)
	{				
	// CContactViewDef	

	return CompareCntItemViewDefTestL(&(def1->ItemDef()), &(def2->ItemDef()));
	}	
	

void CPackagerTests::CCntFilterTestL()
	{
	test.Next(_L("CContactIdArrayTestL"));

	CCntPackager* thePackage = CCntPackager::NewL();
	CleanupStack::PushL(thePackage);
	
	// Create a CCntFilter item.
	CCntFilter* filter1 = CCntFilter::NewL();
	CleanupStack::PushL(filter1);

	filter1->SetContactFilterTypeCard(ETrue);
	TTime time;
	time.UniversalTime();
	time-=TTimeIntervalMinutes(20); // changes in the last 20 mins
	filter1->SetFilterDateTime(time);
	filter1->SetIncludeNewContacts(ETrue);
	filter1->SetContactFilterTypeALL(ETrue);

	// For now, call this to allocate memory for buffer in packager.
	TPtrC8 theBuffer(NULL, NULL);
	TInt err  = KErrNoMemory;
	while(err==KErrNoMemory)
		{
		TRAPD(error, (theBuffer.Set(thePackage->PackL(*filter1))));
		err= error;
		if(error==KErrNoMemory)
			{
			thePackage->GetTransmittingBuffer();
			
			}
		}
		
	CCntFilter* filter2 = thePackage->UnpackCntFilterLC();
	err = CompareCCntFilterTestL(filter1, filter2);
	User::LeaveIfError(err);

	CleanupStack::PopAndDestroy(3, thePackage);
	}
	
TInt CPackagerTests::CompareCCntFilterTestL(CCntFilter* filter1, CCntFilter* filter2)
	{				
	// CCntFilter
	if(CompareCContactIdArrayTestL(filter1->iIds, filter2->iIds)!=KErrNone)
		{
		return KErrNotFound;
		}
	
	if(filter1->GetFilterDateTime()!=filter2->GetFilterDateTime())
		{
		return KErrNotFound;
		}
		
	if(filter1->iInclude!=filter2->iInclude)
		{
		return KErrNotFound;
		}
		
	if(filter1->iContactType!=filter2->iContactType)
		{
		return KErrNotFound;
		}		
		
	return KErrNone;
	}		
	
//Persistence test construction and destruction
CPackagerTests* CPackagerTests::NewLC (CCntTestImplementor& aCntTestImpl)
	{
	CPackagerTests* self = new (ELeave) CPackagerTests(aCntTestImpl);
	CleanupStack::PushL(self);

	self->ConstructL();

	return self;
	}

void CPackagerTests::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	iMatchAll = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EIncludeHiddenFields);
	iMatchAll->AddL(KUidContactFieldMatchAll);
	CleanupStack::Pop(iMatchAll);
	}

CPackagerTests::~CPackagerTests()
	{
	delete iMatchAll;
	iFs.Close();	
	}


/**

@SYMTestCaseID     PIM-T-PACKAGER-0001

*/


LOCAL_C void DoTestsL()
	{
    CleanupClosePushL(test);
	CPackagerTests* cntTester = NULL;
	test.Start(_L("@SYMTESTCaseID:PIM-T-PACKAGER-0001 Persistency tests"));

	
	RPIMTestServer serv;
	User::LeaveIfError(serv.Connect());

#ifdef RUN_CNT

	test.Start(_L("Contact model tests"));

    __UHEAP_MARK;
	CCntTestImpl* oldCntTestImpl = CCntTestImpl::NewLC();

	cntTester = CPackagerTests::NewLC(*oldCntTestImpl);
	cntTester->AllTestsL();
	POPD(cntTester);

	CleanupStack::PopAndDestroy(oldCntTestImpl);
	oldCntTestImpl = NULL;
	test.End();
    __UHEAP_MARKEND;
#endif	

#ifdef RUN_PL

	test.Start(_L("Persistence Layer Tests"));

    __UHEAP_MARK;
	CPlTestImpl* plCntTestImpl = CPlTestImpl::NewLC();
  	cntTester = CPackagerTests::NewLC(*plCntTestImpl);
	cntTester->AllTestsL();

	POPD(cntTester);
	CleanupStack::PopAndDestroy(plCntTestImpl);
	plCntTestImpl = NULL;
	test.End();
    __UHEAP_MARKEND;

#endif 
	

	serv.Close();	
	test.End();
    CleanupStack::PopAndDestroy(1);  // test.Close

	}




GLDEF_C TInt E32Main()
	{
    // Init
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if (!cleanupStack)
        {
        return KErrNoMemory;
        }

    CActiveScheduler* activeScheduler = new CActiveScheduler;
    if (!activeScheduler)
        {
        return KErrNoMemory;
        }
    CActiveScheduler::Install(activeScheduler);

    // Run the tests
    __UHEAP_MARK;
    TRAPD(err, DoTestsL());
    __UHEAP_MARKEND;


	test(__NB_failures == 0);
		
    // Cleanup
    delete activeScheduler;
    delete cleanupStack;
	activeScheduler = NULL;
	cleanupStack = NULL;
	
	return err;
    }


