/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "t_perfdbcreator.h"


// Group constants
const TInt   KGroups=10;
const TInt   KGroupSize=25;
const TText * const KGroupNames[KGroups]=
	{
	_S("Family"),
	_S("Friends"),
	_S("Work"),
	_S("Group 4"),
	_S("Group 5"),
	_S("Group 6"),
	_S("Group 7"),
	_S("Group 8"),
	_S("Group 9"),
	_S("Group 10")
	};


//************************************************************************************************************
//   CPerfDbCreator
//************************************************************************************************************
CPerfDbCreator* CPerfDbCreator::NewL()
   {
	CPerfDbCreator* self = new(ELeave)CPerfDbCreator();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
   } 


//------------------------------------------------------------------------------------------------------------
CPerfDbCreator::CPerfDbCreator()
:  iSeed(KSeed)
   {
   }


//------------------------------------------------------------------------------------------------------------
void CPerfDbCreator::ConstructL()
   {
   }


//------------------------------------------------------------------------------------------------------------
CPerfDbCreator::~CPerfDbCreator()
   {
	if (iContactsDb != NULL)
	   delete iContactsDb;
	
	if (iTemplate != NULL)
	   delete iTemplate;
   }
  

//------------------------------------------------------------------------------------------------------------
// The contacts to be duplicated are not really random
void CPerfDbCreator::GenContactsL(TContactLevel aLevel, TInt aSingles, TInt aDuplicates)
   {
   TInt            i, dupDiv, duplicated=0;
   TReal           dupDivReal;
   TContactItemId  contactItemId;
   CContactItem*   contactItem;

   
   if (aDuplicates != 0)
      {
      Math::Round(dupDivReal, (TReal)aSingles/(TReal)aDuplicates, 0);
      dupDiv= (TInt) dupDivReal;
      }
   else
      dupDiv=0xffff;
   
   
   for (i=0; i < aSingles; i++)
      {
      contactItemId = AddContactL(aLevel);
      // duplicate every dupDiv contact, but do not exceed aDuplicates
      if ((duplicated<aDuplicates) && ((i % dupDiv) == 0)) 
         {
         duplicated++;
         contactItem = iContactsDb->ReadContactL(contactItemId);
         iContactsDb->AddNewContactL(*contactItem);
         delete contactItem;
         }
       
      // Compact after every KCompactEvery added contact
      if ((i % KCompactEvery) == 0)
         iContactsDb->CompactL();
      }
   }

   
//------------------------------------------------------------------------------------------------------------
void CPerfDbCreator::GenDbL(TUserProfile aProfile, TInt aCount, TInt aDupRatio,  TBool aAddSeedPrefix, const TDesC &aFileName, TBool aWithGroups)
   {
   TReal  levelContacts[KLevels], levelDuplicates[KLevels];
   TReal  totalContacts, totalDuplicates;
   TInt   i;
   
   iAddSeedPrefix=aAddSeedPrefix;
   iSeed=KSeed;
   
   iContactsDb = CContactDatabase::ReplaceL(aFileName);
	
   // Template will be used later, to add new contacts
   iTemplate=static_cast<CContactTemplate*>(iContactsDb->ReadContactL(iContactsDb->TemplateId()));
   
   
   // Calculate number of contacts and duplicates for each level
   totalDuplicates= (aCount*aDupRatio)/100/2;
   totalContacts=   aCount-totalDuplicates;

   for (i=0; i<KLevels; i++)
      {
      levelContacts[i]=(totalContacts*KRatios[aProfile][i])/100;
      levelDuplicates[i]=(totalDuplicates*KRatios[aProfile][i])/100;
      Math::Round(levelContacts[i], levelContacts[i], 0);
      Math::Round(levelDuplicates[i], levelDuplicates[i], 0);
      }
      
   // Generate contacts and duplicates for each level
	for (i=0; i<KLevels; i++)
		{
		GenContactsL(static_cast<TContactLevel>(i), (TInt) levelContacts[i], (TInt) levelDuplicates[i]);
		}
      
	// Create groups
	if (aWithGroups)
		{
		for (i = 0; i < KGroups; i++)
			{
			AddGroupL(TPtrC(KGroupNames[i]));
			}
		}

   // And clanup
	delete iContactsDb;
	iContactsDb = NULL;
	delete iTemplate;
	iTemplate = NULL;
   }


//------------------------------------------------------------------------------------------------------------
void CPerfDbCreator::GenAllTheSameDbL(TContactLevel aLevel, TInt aCount, TBool aAddSeedPrefix, const TDesC &aFileName)
   {
   TInt            i;
   TContactItemId  contactItemId;
   CContactItem*   contactItem;   
   
   
   iAddSeedPrefix=aAddSeedPrefix;
   iSeed=KSeed;

   iContactsDb = CContactDatabase::ReplaceL(aFileName);
	
   // Template will be used later, to add new contacts
   iTemplate=static_cast<CContactTemplate*>(iContactsDb->ReadContactL(iContactsDb->TemplateId()));


   // Generate database
   contactItemId = AddContactL(aLevel); // contact 0
   for (i=1; i < aCount; i++)          // contacts 1,2,3...
      {
      contactItem = iContactsDb->ReadContactL(contactItemId);
      iContactsDb->AddNewContactL(*contactItem);
      delete contactItem;

      // Compact after every KCompactEvery added contact
      if ((i % KCompactEvery) == 0)
        iContactsDb->CompactL();
      }
   
   // And clanup
	delete iContactsDb;
	iContactsDb = NULL;
	delete iTemplate;
	iTemplate = NULL;   
   }
   

//------------------------------------------------------------------------------------------------------------   
TContactItemId CPerfDbCreator::AddContactL(TContactLevel aLevel)
	{
	CPerfTestContact* contact=CPerfTestContact::NewLC(*iTemplate);
	HBufC* buf=HBufC::NewLC(256);
	TPtr bufPtr=buf->Des();
	

   // Always in the contact
 	SetRandomAlphaString(bufPtr,32, iAddSeedPrefix);
  	contact->SetFirstNameL(bufPtr);
  	SetRandomAlphaString(bufPtr,32, iAddSeedPrefix);
  	contact->SetLastNameL(bufPtr);
  	SetRandomNumericString(bufPtr,24);
  	contact->SetHomePhoneL(bufPtr);
	
   if (aLevel == ELevel2)
	   {
      SetRandomAlphaNumericString(bufPtr,32);
	   contact->SetHomeStreetL(bufPtr);
	   SetRandomAlphaString(bufPtr,32);
	   contact->SetHomeTownL(bufPtr);
	   SetRandomAlphaString(bufPtr,32);
	   contact->SetHomeCountyL(bufPtr);
	   SetRandomAlphaNumericString(bufPtr,16);
	   contact->SetHomePostCodeL(bufPtr);
	   SetRandomAlphaString(bufPtr,32);
	   contact->SetHomeCountryL(bufPtr);
	   }
	   
   if (aLevel >= ELevel2)
      {
      SetRandomAlphaNumericString(bufPtr,48);
	   contact->SetHomeEmailAddressL(bufPtr);	   
      }
	   
   if (aLevel == ELevel3)
	   {	   
      SetRandomAlphaString(bufPtr,32, iAddSeedPrefix);
	   contact->SetCompanyNameL(bufPtr);
      SetRandomAlphaNumericString(bufPtr,32);
	   contact->SetWorkStreetL(bufPtr);
	   SetRandomAlphaString(bufPtr,32);
	   contact->SetWorkTownL(bufPtr);
	   SetRandomAlphaString(bufPtr,32);
	   contact->SetWorkCountyL(bufPtr);
	   SetRandomAlphaNumericString(bufPtr,16);
	   contact->SetWorkPostCodeL(bufPtr);
	   SetRandomAlphaString(bufPtr,32);
	   contact->SetWorkCountryL(bufPtr);	   
	   }
	
	if (aLevel >= ELevel3)
	   {
	   SetRandomNumericString(bufPtr,24);
	   contact->SetWorkMobileL(bufPtr);
	   
	   SetRandomNumericString(bufPtr,24);
	   contact->SetWorkFaxL(bufPtr);	   

	   SetRandomNumericString(bufPtr,24);
	   contact->SetHomeMobileL(bufPtr);	   
	   }
	 
   if (aLevel >= ELevel4)
	   {
      SetRandomNumericString(bufPtr,24);
	   contact->SetWorkPhoneL(bufPtr);
	   	   
   	SetRandomAlphaNumericString(bufPtr,48);
	   contact->SetWorkEmailAddressL(bufPtr);
	
      // Picture is not used for sorting, so do not
      // grenerate any random data.	   
	   HBufC8* buf8=HBufC8::NewLC(2048);
	   TPtr8   bufPtr8=buf8->Des();
	   contact->SetPictureL(bufPtr8);
	   CleanupStack::PopAndDestroy(1);
	   }

	TContactItemId id = iContactsDb->AddNewContactL(contact->ContactItem());
	CleanupStack::PopAndDestroy(2); // buf, contact.
	return id;
	}

//------------------------------------------------------------------------------------------------------------   
void CPerfDbCreator::AddGroupL(const TDesC & aGroupLabel)
	{
	CContactGroup * group = static_cast<CContactGroup *>(iContactsDb->CreateContactGroupLC(aGroupLabel));
 	
 	// Add a number of existing items to the group
 	TInt itemCount = iContactsDb->CountL();
 	TInt contactId;
 	for (TInt i = 0 ; i < KGroupSize; i++)
 		{
 		contactId = Math::Rand(iSeed) % itemCount;
 		iContactsDb->AddContactToGroupL(contactId, group->Id());
 		}
 		
 	CleanupStack::PopAndDestroy(group);
	}
	
//------------------------------------------------------------------------------------------------------------   
void CPerfDbCreator::SetRandomAlphaString(TDes& aBuf,TInt aLength, TBool aWithSeed)
	{
	aBuf.SetLength(aLength);

	for (TInt ii=0;ii<aLength;++ii)
		{
		aBuf[ii]=RandomAlphaCharCode();
		}
		
   if (aWithSeed)
      aBuf.Replace(0,8, iTextSeed);
	}


//------------------------------------------------------------------------------------------------------------   
void CPerfDbCreator::SetRandomNumericString(TDes& aBuf,TInt aLength, TBool aWithSeed)
	{
	aBuf.SetLength(aLength);
	

	for (TInt ii=0;ii<aLength;++ii)
		{
		aBuf[ii]=RandomNumericCharCode();
		}
		
   if (aWithSeed)
      aBuf.Replace(0,8, iTextSeed);		
	}


//------------------------------------------------------------------------------------------------------------   
void CPerfDbCreator::SetRandomAlphaNumericString(TDes& aBuf,TInt aLength, TBool aWithSeed)
	{
	aBuf.SetLength(aLength);

	for (TInt ii=0;ii<aLength;++ii)
		{
		aBuf[ii]=RandomAlphaNumbericCharCode();
		}
   
   if (aWithSeed)
      aBuf.Replace(0,8, iTextSeed);		
	}


//------------------------------------------------------------------------------------------------------------   
TText CPerfDbCreator::RandomAlphaCharCode()
	{
	return RandomCharCode('A','z',' ');
	}


//------------------------------------------------------------------------------------------------------------   
TText CPerfDbCreator::RandomNumericCharCode()
	{
	return RandomCharCode('0','9',' ');
	}


//------------------------------------------------------------------------------------------------------------   
TText CPerfDbCreator::RandomAlphaNumbericCharCode()
	{
	return RandomCharCode('0','z',' ');
	}


//------------------------------------------------------------------------------------------------------------   
TText CPerfDbCreator::RandomCharCode(TText aLowerBound,TText aUpperBound,TText aException)
	{
	TText charCode=0;
	
	do
		{
		charCode=RandomCharCode(aLowerBound,aUpperBound);
		}
		while (charCode==aException);

	return charCode;
	}


//------------------------------------------------------------------------------------------------------------   
TText CPerfDbCreator::RandomCharCode(TText aLowerBound,TText aUpperBound)
	{
	TText charCode=STATIC_CAST(TText,(Math::Rand(iSeed)%(aUpperBound-aLowerBound))+aLowerBound);
	ASSERT(charCode>=aLowerBound && charCode<=aUpperBound);
    iTextSeed.NumFixedWidthUC((TUint) iSeed, EHex,8);   
	return charCode;
	}




//************************************************************************************************************
//   CPerfTestContact
//************************************************************************************************************
CPerfTestContact* CPerfTestContact::NewL(CContactItem& aSeedContact)
	{
	CPerfTestContact* self=CPerfTestContact::NewLC(aSeedContact);
	CleanupStack::Pop();
	return self;
	}


//------------------------------------------------------------------------------------------------------------	
CPerfTestContact* CPerfTestContact::NewLC(CContactItem& aSeedContact)
	{
	CPerfTestContact* self=new(ELeave) CPerfTestContact();
	CleanupStack::PushL(self);
	self->ConstructL(aSeedContact);
	return self;
	}


//------------------------------------------------------------------------------------------------------------	
CPerfTestContact::CPerfTestContact()
	{
	}


//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::ConstructL(CContactItem& aSeedContact)
	{
	iContactItem=CContactCard::NewL(&aSeedContact);
	}


//------------------------------------------------------------------------------------------------------------	
CPerfTestContact::~CPerfTestContact()
	{
	delete iContactItem;
	}


//------------------------------------------------------------------------------------------------------------	
CContactItem& CPerfTestContact::ContactItem()
	{
	return *iContactItem;
	}


//------------------------------------------------------------------------------------------------------------	
const TPtrC CPerfTestContact::FirstNameL()
	{
	return TextFieldL(KUidContactFieldGivenName);
	}


//------------------------------------------------------------------------------------------------------------	
const TPtrC CPerfTestContact::LastNameL()
	{
	return TextFieldL(KUidContactFieldFamilyName);
	}


//------------------------------------------------------------------------------------------------------------	
const TPtrC CPerfTestContact::CompanyNameL()
	{
	return TextFieldL(KUidContactFieldCompanyName);
	}


//------------------------------------------------------------------------------------------------------------	
const TPtrC CPerfTestContact::JobTitleL()
	{
	return TextFieldL(KUidContactFieldJobTitle);
	}


//------------------------------------------------------------------------------------------------------------	
const TPtrC CPerfTestContact::WorkPhoneL()
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldPhoneNumber);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapWORK);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	const TPtrC text=TextFieldL(*contentType);
	CleanupStack::PopAndDestroy(); // contentType.
	return text;
	}


//------------------------------------------------------------------------------------------------------------	
const TPtrC CPerfTestContact::WorkMobileL()
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldPhoneNumber);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapWORK);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapCELL);
	const TPtrC text=TextFieldL(*contentType);
	CleanupStack::PopAndDestroy(); // contentType.
	return text;
	}


//------------------------------------------------------------------------------------------------------------	
const TPtrC CPerfTestContact::HomePhoneL()
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldPhoneNumber);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapHOME);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	const TPtrC text=TextFieldL(*contentType);
	CleanupStack::PopAndDestroy(); // contentType.
	return text;
	}


//------------------------------------------------------------------------------------------------------------	
const TPtrC CPerfTestContact::HomeMobileL()
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldPhoneNumber);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapHOME);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapCELL);
	const TPtrC text=TextFieldL(*contentType);
	CleanupStack::PopAndDestroy(); // contentType.
	return text;
	}


//------------------------------------------------------------------------------------------------------------	
const TPtrC CPerfTestContact::FaxL()
	{
	return TextFieldL(KUidContactFieldFax);
	}


//------------------------------------------------------------------------------------------------------------	
const TPtrC CPerfTestContact::EmailAddressL()
	{
	return TextFieldL(KUidContactFieldEMail);
	}


//------------------------------------------------------------------------------------------------------------	
const TPtrC CPerfTestContact::InternetAddressL()
	{
	return TextFieldL(KUidContactFieldUrl);
	}


//------------------------------------------------------------------------------------------------------------	
const TPtrC CPerfTestContact::StreetL()
	{
	return TextFieldL(KUidContactFieldAddress);
	}


//------------------------------------------------------------------------------------------------------------	
const TPtrC CPerfTestContact::TownL()
	{
	return TextFieldL(KUidContactFieldLocality);
	}


//------------------------------------------------------------------------------------------------------------	
const TPtrC CPerfTestContact::CountyL()
	{
	return TextFieldL(KUidContactFieldRegion);
	}


//------------------------------------------------------------------------------------------------------------	
const TPtrC CPerfTestContact::PostCodeL()
	{
	return TextFieldL(KUidContactFieldPostcode);
	}


//------------------------------------------------------------------------------------------------------------	
const TPtrC CPerfTestContact::CountryL()
	{
	return TextFieldL(KUidContactFieldCountry);
	}


//------------------------------------------------------------------------------------------------------------	
const TPtrC CPerfTestContact::NotesL()
	{
	return TextFieldL(KUidContactFieldNote);
	}


//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::SetFirstNameL(const TDesC& aFirstName, TBool aIsPronunciation)
	{
	const TUid id = aIsPronunciation ? KUidContactFieldGivenNamePronunciation : KUidContactFieldGivenName;
	SetTextFieldL(id,aFirstName, KUidContactFieldVCardMapUnusedN);
	}


//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::SetLastNameL(const TDesC& aLastName, TBool aIsPronunciation)
	{
	const TUid id = aIsPronunciation ? KUidContactFieldFamilyNamePronunciation : KUidContactFieldFamilyName;
	SetTextFieldL(id,aLastName, KUidContactFieldVCardMapUnusedN);
	}


//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::SetCompanyNameL(const TDesC& aCompanyName, TBool aIsPronunciation)
	{	
	if ( aIsPronunciation )
		SetTextFieldL(KUidContactFieldCompanyNamePronunciation,aCompanyName, KUidContactFieldVCardMapORGPronunciation);
	else
		SetTextFieldL(KUidContactFieldCompanyName,aCompanyName, KUidContactFieldVCardMapORG);
	}


//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::SetJobTitleL(const TDesC& aJobTitle)
	{
	SetTextFieldL(KUidContactFieldJobTitle,aJobTitle);
	}


//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::SetWorkPhoneL(const TDesC& aWorkPhone)
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldPhoneNumber);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapWORK);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	SetTextFieldL(*contentType,aWorkPhone);
	CleanupStack::PopAndDestroy(); // contentType.
	}


//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::SetWorkMobileL(const TDesC& aWorkMobile)
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldPhoneNumber);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapWORK);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapCELL);
	SetTextFieldL(*contentType,aWorkMobile);
	CleanupStack::PopAndDestroy(); // contentType.
	}


//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::SetHomePhoneL(const TDesC& aHomePhone)
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldPhoneNumber);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapHOME);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	SetTextFieldL(*contentType,aHomePhone);
	CleanupStack::PopAndDestroy(); // contentType.
	}


//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::SetHomeMobileL(const TDesC& aHomeMobile)
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldPhoneNumber);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapHOME);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapCELL);
	SetTextFieldL(*contentType,aHomeMobile);
	CleanupStack::PopAndDestroy(); // contentType.
	}


//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::SetWorkFaxL(const TDesC& aFax)
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldFax);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapWORK);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapFAX);
	SetTextFieldL(KUidContactFieldFax,aFax);
	CleanupStack::PopAndDestroy(); // contentType.
	}


//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::SetHomeEmailAddressL(const TDesC& aEmailAddress)
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldEMail);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapHOME);
	SetTextFieldL(*contentType,aEmailAddress);
	CleanupStack::PopAndDestroy(contentType);
	}


//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::SetWorkEmailAddressL(const TDesC& aEmailAddress)
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldEMail);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapWORK);
	SetTextFieldL(*contentType,aEmailAddress);
	CleanupStack::PopAndDestroy(contentType); 
	}


//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::SetInternetAddressL(const TDesC& aInternetAddress)
	{
	SetTextFieldL(KUidContactFieldUrl,aInternetAddress);
	}


//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::SetHomeStreetL(const TDesC& aStreet)
	{
	SetTextFieldL(KUidContactFieldAddress,aStreet);
	}
	
	
//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::SetWorkStreetL(const TDesC& aStreet)
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
   contentType->AddFieldTypeL(KUidContactFieldAddress);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapADR);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapWORK);
   SetTextFieldL(KUidContactFieldAddress,aStreet);
	CleanupStack::PopAndDestroy(); // contentType.
	}	


//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::SetHomeTownL(const TDesC& aTown)
	{
	SetTextFieldL(KUidContactFieldLocality,aTown);
	}


//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::SetWorkTownL(const TDesC& aTown)
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldLocality);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapLOCALITY);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapWORK);
	SetTextFieldL(KUidContactFieldLocality,aTown);
	CleanupStack::PopAndDestroy(); // contentType.	
	}


//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::SetHomeCountyL(const TDesC& aCounty)
	{
	SetTextFieldL(KUidContactFieldRegion,aCounty);
	}


//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::SetWorkCountyL(const TDesC& aCounty)
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldRegion);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapREGION);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapWORK);
	SetTextFieldL(KUidContactFieldRegion,aCounty);
	CleanupStack::PopAndDestroy(); // contentType.	
	}


//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::SetHomePostCodeL(const TDesC& aPostCode)
	{
	SetTextFieldL(KUidContactFieldPostcode,aPostCode);
	}
	
	
//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::SetWorkPostCodeL(const TDesC& aPostCode)
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldPostcode);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapPOSTCODE);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapWORK);
	SetTextFieldL(KUidContactFieldPostcode,aPostCode);
	CleanupStack::PopAndDestroy(); // contentType.	
	}	


//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::SetHomeCountryL(const TDesC& aCountry)
	{
	SetTextFieldL(KUidContactFieldCountry,aCountry);
	}
	
	
//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::SetWorkCountryL(const TDesC& aCountry)
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldCountry);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapCOUNTRY);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapWORK);
	SetTextFieldL(KUidContactFieldCountry,aCountry);
	CleanupStack::PopAndDestroy(); // contentType.		
	}	


//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::SetNotesL(const TDesC& aNotes)
	{
	SetTextFieldL(KUidContactFieldNote,aNotes);
	}
	

//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::SetPictureL(const TDesC8& aPicture)
	{
	SetBinaryFieldL(KUidContactFieldPicture,aPicture);
	}
	

//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::SetIMAddressL(const TDesC& aIMAddress, TUid aAddOnContentType)
	{
	SetTextFieldL(KUidContactFieldIMAddress, aIMAddress, aAddOnContentType);
	}


//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::InsertTextFieldL(TFieldType aID, const TDesC& aStringOfStuff )
	{
	CContactItemField* itemFieldPtr = NULL;
	CContactTextField* textStoragePtr = NULL;

	itemFieldPtr = CContactItemField::NewLC( KStorageTypeText, aID );
	textStoragePtr = itemFieldPtr->TextStorage(); // we don't have ownership here, hence no stack.

	textStoragePtr->SetTextL( aStringOfStuff );
	textStoragePtr = NULL; // we've finished with this ptr now.

	iContactItem->AddFieldL( *itemFieldPtr ); // hand ownership over here
	CleanupStack::Pop( itemFieldPtr ); // and pop,
	}
	
	
//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::InsertBinaryFieldL(TUid aID, const TDesC8& aBufOfStuff )
	{
	CContactItemField*  itemFieldPtr = NULL;
	CContactStoreField* binStoragePtr = NULL;

	itemFieldPtr = CContactItemField::NewLC( KStorageTypeStore, aID );
	binStoragePtr = itemFieldPtr->StoreStorage(); // we don't have ownership here, hence no stack.

	binStoragePtr->SetThingL(aBufOfStuff);
	binStoragePtr = NULL; // we've finished with this ptr now.

	iContactItem->AddFieldL( *itemFieldPtr ); // hand ownership over here
	CleanupStack::Pop( itemFieldPtr ); // and pop,
	}	


//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::SetVoiceDialL(const TDesC& aVoiceDial)
	{
	SetTextFieldL( KUidContactsVoiceDialField, aVoiceDial );
	}


//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::SetRingToneL(const TDesC& aRingTone)
	{
	SetTextFieldL( KUidContactFieldRingTone, aRingTone );
	}


//------------------------------------------------------------------------------------------------------------	
const TPtrC CPerfTestContact::TextFieldL(TFieldType aFieldType) const
	{
	CContactItemField& field=FindFieldL(aFieldType);
	ASSERT(field.StorageType()==KStorageTypeText);
	return STATIC_CAST(CContactTextField*,field.Storage())->Text();
	}


//------------------------------------------------------------------------------------------------------------	
const TPtrC CPerfTestContact::TextFieldL(const CContentType& aTypesToMatch) const
	{
	CContactItemField& field=FindFieldL(aTypesToMatch);
	ASSERT(field.StorageType()==KStorageTypeText);
	return STATIC_CAST(CContactTextField*,field.Storage())->Text();
	}


//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::SetTextFieldL(TFieldType aFieldType,const TDesC& aText)
	{
	CContactItemFieldSet& fields = iContactItem->CardFields();
	TInt fieldIndex = 0;

	fieldIndex = FindField( aFieldType );

	if ( fieldIndex > KErrNotFound )
		{
		CContactItemField& field = fields[ fieldIndex ];
		ASSERT(field.StorageType()==KStorageTypeText);
		STATIC_CAST(CContactTextField*,field.Storage())->SetText(aText.AllocL());
		}
	else
		{
		InsertTextFieldL(aFieldType, aText);
		}
	}


//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::SetBinaryFieldL(TFieldType aFieldType,const TDesC8& aBuf)
	{
	CContactItemFieldSet& fields = iContactItem->CardFields();
	TInt fieldIndex = 0;

	fieldIndex = FindField( aFieldType );

	if ( fieldIndex > KErrNotFound )
		{
		CContactItemField& field = fields[ fieldIndex ];
		ASSERT(field.StorageType()==KStorageTypeStore);
		STATIC_CAST(CContactStoreField*,field.Storage())->SetThingL(aBuf);
		}
	else
		{
		InsertBinaryFieldL(aFieldType, aBuf);
		}
	}


//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::SetTextFieldL(const CContentType& aTypesToMatch,const TDesC& aText) const
	{
	CContactItemField& field=FindFieldL(aTypesToMatch);
	ASSERT(field.StorageType()==KStorageTypeText);
	STATIC_CAST(CContactTextField*,field.Storage())->SetText(aText.AllocL());
	}


//------------------------------------------------------------------------------------------------------------	
void CPerfTestContact::SetTextFieldL(TFieldType aFieldType, const TDesC& aText, TUid aAddOnContentType) 
	{
	CContactItemFieldSet& fields = iContactItem->CardFields();
	CContactTextField* textField = NULL;
	TInt fieldIndex = 0;

	fieldIndex = FindField( aFieldType );

	if (fieldIndex == KErrNotFound )
		{
		InsertTextFieldL( aFieldType, aText );
		fieldIndex = FindField( aFieldType );
		CContactItemField& field = fields[ fieldIndex ];
		field.AddFieldTypeL( aAddOnContentType );
		}
	else if (fieldIndex >= KErrNone )
		{
		
		CContactItemField& field = fields[ fieldIndex ];
		ASSERT(field.StorageType()==KStorageTypeText);
		textField = static_cast<CContactTextField*>(field.Storage());
		textField->SetText( aText.AllocL() );
		field.AddFieldTypeL( aAddOnContentType );
		}
	}


//------------------------------------------------------------------------------------------------------------	
CContactItemField& CPerfTestContact::FindFieldL(TFieldType aFieldType) const
	{
	CContactItemFieldSet& fields=iContactItem->CardFields();
	TInt index=fields.Find(aFieldType);
	User::LeaveIfError(index);
	return fields[index];
	}


//------------------------------------------------------------------------------------------------------------	
TInt CPerfTestContact::FindField(TFieldType aFieldType) const
	{
	return iContactItem->CardFields().Find( aFieldType );
	}


//------------------------------------------------------------------------------------------------------------	
CContactItemField& CPerfTestContact::FindFieldL(const CContentType& aTypesToMatch) const
	{
	CContactItemFieldSet& fields=iContactItem->CardFields();
	const TInt numTypesToMatch(aTypesToMatch.FieldTypeCount());
	const TFieldType mainFieldType(aTypesToMatch.FieldType(0));
	TInt index=-1;
	FOREVER
		{
		index=fields.FindNext(mainFieldType,index+1);
		User::LeaveIfError(index);

		CContactItemField& thisPhoneField=fields[index];
		const CContentType& thisPhoneFieldTypes=thisPhoneField.ContentType();
		const TInt numFieldTypes(thisPhoneFieldTypes.FieldTypeCount());

		if (numFieldTypes!=numTypesToMatch)
			{
			continue;
			}

		TInt numTypesLeftToMatch=numTypesToMatch;
		for (TInt ii=0;ii<numFieldTypes;++ii)
			{
			TFieldType thisFieldType(thisPhoneFieldTypes.FieldType(ii));
			for (TInt jj=0;jj<numTypesToMatch;++jj)
				{
				TFieldType thisFieldTypeToMatch(aTypesToMatch.FieldType(jj));
				if (thisFieldType==thisFieldTypeToMatch)
					{
					if (--numTypesLeftToMatch<=0)
						{
						return thisPhoneField;
						}

					break;
					}
				}
			}
		}
	}





//************************************************************************************************************
//   TPerfTimer
//************************************************************************************************************
TPerfTimer::TPerfTimer()
   {
   iStartTime.HomeTime();
   iPrevTime=iStartTime;
   }
   
   
//------------------------------------------------------------------------------------------------------------	
void TPerfTimer::GetTime(TDes &aTimeDes)
   {
   _LIT(KTimeFormat,"|%6.02f |%6.02f  | ");   
   TTime tmpTime;
   TReal total, delta;

   tmpTime.HomeTime();
   
#ifdef EKA2
   total = (TReal)tmpTime.MicroSecondsFrom(iStartTime).Int64()/1000000;
   delta = (TReal)tmpTime.MicroSecondsFrom(iPrevTime).Int64()/1000000;
#else
   total = tmpTime.MicroSecondsFrom(iStartTime).Int64().GetTReal()/1000000;
   delta = tmpTime.MicroSecondsFrom(iPrevTime).Int64().GetTReal()/1000000;
#endif   

   iPrevTime=tmpTime;   
   aTimeDes.Format(KTimeFormat, total, delta);
   }
   
   
//------------------------------------------------------------------------------------------------------------
void TPerfTimer::GetTime(TInt /*aTotal*/, TInt /*aDelta*/)
   {
   }


//------------------------------------------------------------------------------------------------------------
void TPerfTimer::Reset()
   {
   iStartTime.HomeTime();
   iPrevTime=iStartTime;   
   }
