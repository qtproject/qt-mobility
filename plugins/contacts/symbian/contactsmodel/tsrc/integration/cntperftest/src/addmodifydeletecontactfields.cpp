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


#include "addmodifydeletecontactfields.h"

/* The following set of functions are available for creating and updating desired contact items fields
*  A contact item field has 3 important parts to it
*  1) Field Type
*  2) Storage Type
*  3) Content
*  Functions are available which construct the contact items fields with meet the above criteria
*  In General, the contact item fields are set with random string as their content
*  In specific case, the contact item field will be supplied withh desired content
*  useful in case of find views and Sub Views
*  In case of filtered views, it is required that the contact item possess fields that meet the
*  contact item view filter. Functions are available to added contact item fields in order to meet
*  the contact item view filter criteria
*/

/**
* Retrieve the relevant contact field storage type for the specified contact field
* @param aFieldType - Contact field information
* @return TStorageType - The contact field storage type
*/
TStorageType TAddModifyDeleteContactFields::GetStorageType(const TFieldType& aFieldType)
	{
	if((aFieldType == KUidContactFieldAnniversary) || (aFieldType == KUidContactFieldBirthday))
		{
		return KStorageTypeDateTime ;
		}
	return KStorageTypeText;
	}

/**
* Adds content to the specified field based on the available contact field type information
* @param aFieldType - Contact field type information
* @param aField - The contact field to be updated
*/
void TAddModifyDeleteContactFields::SetDataL(const TFieldType& aFieldType, CContactItemField& aField)
	{
	HBufC* buf = HBufC::NewLC(SharedConstants::KMaxBufferLength);
	TPtr bufPtr = buf->Des();

	if ((aFieldType == KUidContactFieldJobTitle) || (aFieldType == KUidContactFieldNote) ||

	    (aFieldType == KUidContactFieldAssistant) || (aFieldType == KUidContactFieldSpouse) ||

	    (aFieldType == KUidContactFieldChildren) || (aFieldType == KUidContactFieldRegion)  ||

	    (aFieldType == KUidContactFieldCountry) || (aFieldType == KUidContactFieldCompanyName) ||

	    (aFieldType == KUidContactFieldCompanyNamePronunciation) || (aFieldType == KUidContactFieldGivenName) ||

	    (aFieldType == KUidContactFieldFamilyName) || (aFieldType == KUidContactFieldGivenNamePronunciation)  ||

	    (aFieldType == KUidContactFieldFamilyNamePronunciation) || (aFieldType == KUidContactFieldAdditionalName) ||

	    (aFieldType == KUidContactFieldSuffixName) || (aFieldType == KUidContactFieldPrefixName)  ||

	    (aFieldType == KUidContactFieldDepartmentName) || (aFieldType == KUidContactFieldClass) || 
	    
	    (aFieldType == KUidContactFieldClass))
	    	{
			SetRandomAlphaString(bufPtr, SharedConstants::KMaxBufferLength);
			aField.TextStorage()->SetTextL(bufPtr);
	    	}

	else if ((aFieldType == KUidContactFieldPostcode ) || (aFieldType == KUidContactFieldPhoneNumber) ||

		 (aFieldType == KUidContactFieldAnniversary ) || (aFieldType == KUidContactFieldSms) ||

		 (aFieldType == KUidContactFieldFax ) || (aFieldType == KUidContactFieldBirthday) ||

		 (aFieldType == KUidContactFieldICCPhonebook ) || (aFieldType == KUidContactFieldDTMF) ||

		 (aFieldType == KUidContactFieldICCSlot) ||  (aFieldType == KUidContactsVoiceDialField))
		 	{
		   	SetRandomNumericString(bufPtr, SharedConstants::KMaxBufferLength);
			aField.TextStorage()->SetTextL(bufPtr);
		 	}

	else if ((aFieldType == KUidContactFieldAddress ) || (aFieldType == KUidContactFieldPostOffice) ||

		(aFieldType == KUidContactFieldExtendedAddress ) ||  (aFieldType == KUidContactFieldLocality) ||

		(aFieldType == KUidContactFieldEMail ) || (aFieldType == KUidContactFieldMsg) ||

		(aFieldType == KUidContactFieldUrl ) || (aFieldType == KUidContactFieldPicture) ||

		(aFieldType == KUidContactFieldRingTone) ||  (aFieldType == KUidContactFieldICCGroup) ||

		(aFieldType == KUidContactFieldIMAddress ) || (aFieldType == KUidContactFieldSecondName) ||

		(aFieldType == KUidContactFieldSIPID))
			{
			SetRandomAlphaNumericString(bufPtr, SharedConstants::KMaxBufferLength);
			aField.TextStorage()->SetTextL(bufPtr);
			}
	CleanupStack::PopAndDestroy(); // buf
	}

/**
 * Update the specified buffer with random alpha content as per specified length
 * @param aBuf - Buffer to be updated
 * @param aLength - Length information
 */
void TAddModifyDeleteContactFields::SetRandomAlphaString(TDes& aBuf,TInt aLength)
	{
	aBuf.SetLength(aLength);

	for (TInt ii = 0; ii < aLength; ++ii)
		{
		aBuf[ii] = RandomAlphaCharCode();
		}
	}

/**
 * Update the specified buffer with random numeric content as per specified length
 * @param aBuf - Buffer to be updated
 * @param aLength - Length information
 */
void TAddModifyDeleteContactFields::SetRandomNumericString(TDes& aBuf,TInt aLength)
	{
	aBuf.SetLength(aLength);

	for (TInt ii = 0; ii < aLength; ++ii)
		{
		aBuf[ii] = RandomNumericCharCode();
		}
	}

/**
 * Update the specified buffer with random alpha and numeric content as per specified length
 * @param aBuf - Buffer to be updated
 * @param aLength - Length information
 */
void TAddModifyDeleteContactFields::SetRandomAlphaNumericString(TDes& aBuf,TInt aLength)
	{
	aBuf.SetLength(aLength);
	for (TInt ii = 0; ii < (aLength-1); ii = ii+2)
		{
		aBuf[ii] = RandomAlphaCharCode();
		aBuf[ii+1] = RandomNumericCharCode();
		}
	}

/**
 * Returns any random character between A to Z
 * @return TText - Random Character
 */
TText TAddModifyDeleteContactFields::RandomAlphaCharCode()
	{
	return RandomCharCode('A','Z',' ');
	}

/**
 * Returns any random ascii character equivalent of number between 0 to 9
 * @return TText - Random Number 
 */
TText TAddModifyDeleteContactFields::RandomNumericCharCode()
	{
	return RandomCharCode('0','9',' ');
	}


TText TAddModifyDeleteContactFields::RandomCharCode(TText aLowerBound,TText aUpperBound,TText aException)
	{
	TText charCode = 0;

	do
		{
		charCode=RandomCharCode(aLowerBound,aUpperBound);
		}
		while (charCode == aException);

	return charCode;
	}

TText TAddModifyDeleteContactFields::RandomCharCode(TText aLowerBound,TText aUpperBound)
	{
	TText charCode = STATIC_CAST(TText,(Math::Rand(iRandSeed) % (aUpperBound - aLowerBound)) + aLowerBound);
	ASSERT(charCode >= aLowerBound && charCode <= aUpperBound);
	return charCode;
	}


/**
* Retrieve the contact field uid information based on the contact field information specified in string format
* @param aContactFieldString - Contact field information in String format
* @return TUid - The contact field information in uid format
*/

TUid TAddModifyDeleteContactFields::GetContactFieldType(const TPtrC& aContactFieldString)
	{

	_LIT(KAddress, 						"Address");
	_LIT(KPostOffice,					"PostOffice");
	_LIT(KExtendedAddress, 				"ExtendedAddress");
	_LIT(KLocality,	 					"Locality");
	_LIT(KRegion,	 					"Region");
	_LIT(KPostCode,	 					"PostCode");
	_LIT(KCountry,	 					"Country");
	_LIT(KCompanyName, 					"CompanyName");
	_LIT(KCompanyNamePronunciation, 	"CompanyNamePronunciation");
	_LIT(KPhoneNumber,     				"PhoneNumber");
	_LIT(KGivenName,       		        "GivenName");
	_LIT(KFamilyName,                	"FamilyName");
	_LIT(KGivenNamePronunciation,     	"GivenNamePronunciation");
	_LIT(KFamilyNamePronunciation,    	"FamilyNamePronunciation");
	_LIT(KAdditionalName,             	"AdditionalName");
	_LIT(KSuffixName,                	"SuffixName");
	_LIT(KPrefixName,                	"PrefixName");
	_LIT(KHidden,                		"Hidden");
	_LIT(KEMail,                		"EMail");
	_LIT(KMsg,                			"Msg");
	_LIT(KSms,                			"Sms");
	_LIT(KFax,                			"Fax");
	_LIT(KNote,			  				"Note");
	_LIT(KBirthday,		          		"Birthday");
	_LIT(KUrl,			  				"Url");
	_LIT(KPicture,		          		"Picture");
	_LIT(KRingTone,		          		"RingTone");
	_LIT(KDTMF,			  				"DTMF");
	_LIT(KVoiceDialField,		  		"VoiceDialField");
	_LIT(KJobTitle,		          		"JobTitle");
	_LIT(KICCSlot,		          		"ICCSlot");
	_LIT(KICCPhonebook,	          		"ICCPhonebook");
	_LIT(KICCGroup,		          		"ICCGroup");
	_LIT(KIMAddress,		  			"IMAddress");
	_LIT(KSecondName,		  			"SecondName");
	_LIT(KSIPID,		          		"SIPID");
	_LIT(KAssistant,		  			"Assistant");
	_LIT(KAnniversary,	          		"Anniversary");
	_LIT(KSpouse,		          		"Spouse");
	_LIT(KChildren,		          		"Children");
	_LIT(KClass,		          		"Class");
	_LIT(KDepartmentName,	          	"DepartmentName");

	TUid	uid;

	if ( aContactFieldString.Compare(KAddress) == 0)
		{
		uid = KUidContactFieldAddress;
		}
	else if ( aContactFieldString.Compare(KPostOffice) == 0)
		{
		uid = KUidContactFieldPostOffice;
		}
	else if ( aContactFieldString.Compare(KExtendedAddress) == 0)
		{
		uid = KUidContactFieldExtendedAddress;
		}
	else if ( aContactFieldString.Compare(KLocality) == 0)
		{
		uid = KUidContactFieldLocality;
		}
	else if ( aContactFieldString.Compare(KRegion) == 0)
		{
		uid = KUidContactFieldRegion;
		}
	else if ( aContactFieldString.Compare(KPostCode) == 0)
		{
		uid = KUidContactFieldPostcode;
		}
	else if ( aContactFieldString.Compare(KCountry) == 0)
		{
		uid = KUidContactFieldCountry;
		}
	else if ( aContactFieldString.Compare(KCompanyName) == 0)
		{
		uid = KUidContactFieldCompanyName;
		}
	else if ( aContactFieldString.Compare(KCompanyNamePronunciation) == 0)
		{
		uid = KUidContactFieldCompanyNamePronunciation;
		}
	else if ( aContactFieldString.Compare(KPhoneNumber) == 0)
		{
		uid = KUidContactFieldPhoneNumber;
		}
	else if ( aContactFieldString.Compare(KGivenName) == 0)
		{
		uid = KUidContactFieldGivenName;
		}
	else if ( aContactFieldString.Compare(KFamilyName) == 0)
		{
		uid = KUidContactFieldFamilyName;
		}
	else if ( aContactFieldString.Compare(KGivenNamePronunciation) == 0)
		{
		uid = KUidContactFieldGivenNamePronunciation;
		}
	else if ( aContactFieldString.Compare(KFamilyNamePronunciation) == 0)
		{
		uid = KUidContactFieldFamilyNamePronunciation;
		}
	else if ( aContactFieldString.Compare(KAdditionalName) == 0)
		{
		uid = KUidContactFieldAdditionalName;
		}
	else if ( aContactFieldString.Compare(KSuffixName) == 0)
		{
		uid = KUidContactFieldSuffixName;
		}
	else if ( aContactFieldString.Compare(KPrefixName) == 0)
		{
		uid = KUidContactFieldPrefixName;
		}
	else if ( aContactFieldString.Compare(KHidden) == 0)
		{
		uid = KUidContactFieldHidden;
		}
	else if ( aContactFieldString.Compare(KEMail) == 0)
		{
		uid = KUidContactFieldEMail;
		}
	else if ( aContactFieldString.Compare(KMsg) == 0)
		{
		uid = KUidContactFieldMsg;
		}
	else if ( aContactFieldString.Compare(KSms) == 0)
		{
		uid = KUidContactFieldSms;
		}
	else if ( aContactFieldString.Compare(KFax) == 0)
		{
		uid = KUidContactFieldFax;
		}
	else if ( aContactFieldString.Compare(KNote) == 0)
		{
		uid = KUidContactFieldNote;
		}
	else if ( aContactFieldString.Compare(KBirthday) == 0)
		{
		uid = KUidContactFieldBirthday;
		}
	else if ( aContactFieldString.Compare(KUrl) == 0)
		{
		uid = KUidContactFieldUrl;
		}
	else if ( aContactFieldString.Compare(KPicture) == 0)
		{
		uid = KUidContactFieldPicture;
		}
	else if ( aContactFieldString.Compare(KRingTone) == 0)
		{
		uid = KUidContactFieldRingTone;
		}
	else if ( aContactFieldString.Compare(KDTMF) == 0)
		{
		uid = KUidContactFieldDTMF;
		}
	else if ( aContactFieldString.Compare(KVoiceDialField) == 0)
		{
		uid = KUidContactsVoiceDialField;
		}
	else if ( aContactFieldString.Compare(KJobTitle) == 0)
		{
		uid = KUidContactFieldJobTitle;
		}
	else if ( aContactFieldString.Compare(KICCSlot) == 0)
		{
		uid = KUidContactFieldICCSlot;
		}
	else if ( aContactFieldString.Compare(KICCPhonebook) == 0)
		{
		uid = KUidContactFieldICCPhonebook;
		}
	else if ( aContactFieldString.Compare(KICCGroup) == 0)
		{
		uid = KUidContactFieldICCGroup;
		}
	else if ( aContactFieldString.Compare(KIMAddress) == 0)
		{
		uid = KUidContactFieldIMAddress;
		}
	else if ( aContactFieldString.Compare(KSecondName) == 0)
		{
		uid = KUidContactFieldSecondName;
		}
	else if ( aContactFieldString.Compare(KSIPID) == 0)
		{
		uid = KUidContactFieldSIPID;
		}
	else if ( aContactFieldString.Compare(KAssistant) == 0)
		{
		uid = KUidContactFieldAssistant;
		}
	else if ( aContactFieldString.Compare(KAnniversary) == 0)
		{
		uid = KUidContactFieldAnniversary;
		}
	else if ( aContactFieldString.Compare(KSpouse) == 0)
		{
		uid = KUidContactFieldSpouse;
		}
	else if ( aContactFieldString.Compare(KChildren) == 0)
		{
		uid = KUidContactFieldChildren;
		}
	else if ( aContactFieldString.Compare(KClass) == 0)
		{
		uid = KUidContactFieldClass;
		}
	else if ( aContactFieldString.Compare(KDepartmentName) == 0)
		{
		uid = KUidContactFieldDepartmentName;
		}
	else
		{
		uid = KUidContactFieldNone;
		}

	return uid;
	}

/**
 * Adds contact field to meet the specified contact based to meet the filter requirements
 * @param aContact contact to be updated
 * @param aBitWiseFilter desired view filter
 */
void TAddModifyDeleteContactFields::AddFilterBasedFieldsL(CContactItem& aContact, TInt aBitWiseFilter)
	{
	HBufC* buf = HBufC::NewLC(SharedConstants::KMaxBufferLength);
	TPtr bufPtr = buf->Des();

	if(aBitWiseFilter & CContactDatabase::ELandLine)
		{
		SetRandomNumericString(bufPtr,SharedConstants::KMaxBufferLength);
		SetWorkPhoneL(aContact, bufPtr);
		}

	if(aBitWiseFilter & CContactDatabase::ESmsable)
		{
		SetRandomNumericString(bufPtr, SharedConstants::KMaxBufferLength);
		SetWorkMobileL(aContact, bufPtr);
		}

	if(aBitWiseFilter & CContactDatabase::EMailable)
		{
		SetRandomAlphaNumericString(bufPtr, SharedConstants::KMaxBufferLength);
		if (aBitWiseFilter&CContactDatabase::EHome)
			SetHomeEmailAddressL(aContact, bufPtr);
		else
			SetWorkEmailAddressL(aContact, bufPtr);
		}

	if(aBitWiseFilter & CContactDatabase::EFaxable)
		{
		SetRandomNumericString(bufPtr, SharedConstants::KMaxBufferLength);
		SetFaxL(aContact, bufPtr);
		}

	if(aBitWiseFilter & CContactDatabase::ERingTone)
		{
		SetRandomNumericString(bufPtr, SharedConstants::KMaxBufferLength);
		SetRingToneL(aContact, bufPtr);
		}

	if(aBitWiseFilter & CContactDatabase::EVoiceDial)
		{
		SetRandomNumericString(bufPtr, SharedConstants::KMaxBufferLength);
		SetVoiceDialL(aContact, bufPtr);
		}

	if(aBitWiseFilter & CContactDatabase::EWirelessVillage)
		{
		SetRandomNumericString(bufPtr, SharedConstants::KMaxBufferLength);
		SetIMAddressL(aContact, bufPtr);
		}
	if(aBitWiseFilter & CContactDatabase::EIMAddress)
		{
		SetRandomAlphaNumericString(bufPtr, SharedConstants::KMaxBufferLength);
		SetIMAddressL(aContact, bufPtr);
		}

	CleanupStack::PopAndDestroy(buf);
	}

/**
 * Adds contact field to meet the work phone filter requirement
 * @param aContact contact to be updated
 * @param aWorkPhone content for work phone contact field
 */
void TAddModifyDeleteContactFields::SetWorkPhoneL(CContactItem& aContact, const TDesC& aWorkPhone)
	{
	CContentType* contentType = CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldPhoneNumber);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapWORK);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	SetTextFieldL(aContact, *contentType,aWorkPhone);
	CleanupStack::PopAndDestroy(contentType); 
	}

/**
 * Adds contact field to meet the work mobile filter requirement
 * @param aContact contact to be updated
 * @param aWorkMobile content for work mobile contact field
 */
void TAddModifyDeleteContactFields::SetWorkMobileL(CContactItem& aContact, const TDesC& aWorkMobile)
	{
	CContentType* contentType = CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldPhoneNumber);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapWORK);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapCELL);
	SetTextFieldL(aContact, *contentType,aWorkMobile);
	CleanupStack::PopAndDestroy(contentType); 
	}

/**
 * Adds contact field to meet the home phone filter requirement
 * @param aContact contact to be updated
 * @param aHomePhone content for home phone contact field
 */
void TAddModifyDeleteContactFields::SetHomePhoneL(CContactItem& aContact, const TDesC& aHomePhone)
	{
	CContentType* contentType = CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldPhoneNumber);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapHOME);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	SetTextFieldL(aContact, *contentType,aHomePhone);
	CleanupStack::PopAndDestroy(contentType); 
	}

/**
 * Adds contact field to meet the home mobile filter requirement
 * @param aContact contact to be updated
 * @param aHomeMobile content for home mobile contact field
 */
void TAddModifyDeleteContactFields::SetHomeMobileL(CContactItem& aContact, const TDesC& aHomeMobile)
	{
	CContentType* contentType = CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldPhoneNumber);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapHOME);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapCELL);
	SetTextFieldL(aContact, *contentType,aHomeMobile);
	CleanupStack::PopAndDestroy(contentType); 
	}
	
/**
 * Adds contact field to meet the home email filter requirement
 * @param aContact contact to be updated
 * @param aEmailAddress content for home email contact field
 */
void TAddModifyDeleteContactFields::SetHomeEmailAddressL(CContactItem& aContact, const TDesC& aEmailAddress)
	{
	CContentType* contentType = CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldEMail);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapHOME);
	SetTextFieldL(aContact, *contentType,aEmailAddress);
	CleanupStack::PopAndDestroy(contentType);
	}

/**
 * Adds contact field to meet the work email filter requirement
 * @param aContact contact to be updated
 * @param aEmailAddress content for work email contact field
 */
void TAddModifyDeleteContactFields::SetWorkEmailAddressL(CContactItem& aContact, const TDesC& aEmailAddress)
	{
	CContentType* contentType = CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldEMail);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapWORK);
	SetTextFieldL(aContact, *contentType,aEmailAddress);
	CleanupStack::PopAndDestroy(contentType);
	}

/**
 * Adds a new contact field to meet the fax filter requirement
 * @param aContact contact to be updated
 * @param aFax content for fax contact field
 */
void TAddModifyDeleteContactFields::SetFaxL(CContactItem& aContact, const TDesC& aFax)
	{
	CContentType* contentType = CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldFax);
	SetTextFieldL(aContact, *contentType, aFax);
	CleanupStack::PopAndDestroy(contentType); 
	}
	
/**
 * Adds a new contact field to meet the IMAddress filter requirement
 * @param aContact contact to be updated
 * @param aIMAddress content for fax contact field
 */
void TAddModifyDeleteContactFields::SetIMAddressL(CContactItem& aContact, const TDesC& aIMAddress)
	{
	CContentType* contentType = CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldIMAddress);
	SetTextFieldL(aContact, *contentType, aIMAddress);
	CleanupStack::PopAndDestroy(contentType); 
	}

/**
 * Adds a new contact field to meet the voice dial filter requirement
 * @param aContact contact to be updated
 * @param aVoiceDial content for voice dial contact field
 */
void TAddModifyDeleteContactFields::SetVoiceDialL(CContactItem& aContact, const TDesC& aVoiceDial)
	{
	CContentType* contentType = CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactsVoiceDialField);
	SetTextFieldL(aContact,  *contentType, aVoiceDial );
	CleanupStack::PopAndDestroy(contentType); 
	}

/**
 * Adds a new contact field to meet the ring tone filter requirement
 * @param aContact contact to be updated
 * @param aRingTone content for ring tone contact field
 */
void TAddModifyDeleteContactFields::SetRingToneL(CContactItem& aContact, const TDesC& aRingTone)
	{
	CContentType* contentType = CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldRingTone);
	SetTextFieldL(aContact, *contentType, aRingTone );
	CleanupStack::PopAndDestroy(contentType); 
	}
	
/**
 * Adds desired contact field content to the contact specified
 * @param aContact contact to be updated
 * @param aTypesToMatch Storage type for Contact Field
 * @param aText content for the desired field
 */
void TAddModifyDeleteContactFields::SetTextFieldL(CContactItem& aContact, const CContentType& aTypesToMatch, const TDesC& aText)
	{
	TStorageType storageType = KStorageTypeText;
	CContactItemField* field = 	CContactItemField::NewL(storageType, aTypesToMatch);
	CleanupStack::PushL(field);
	field->TextStorage()->SetTextL(aText);
	aContact.AddFieldL(*field);
	CleanupStack::Pop(field);	
	}

/**
 * Update the contact item fields with new content
 * @param aContact contact to be updated
 */
void TAddModifyDeleteContactFields::UpdateFieldsL(CContactItem& aContact)
	{
	CContactItemFieldSet& fieldSet = aContact.CardFields();
	for(TInt i = 0; i < fieldSet.Count();i++)
		{
		TFieldType fieldType(KUidContactFieldNone);
		CContactItemField& field = fieldSet[i];
		SetDataL(fieldType, field);
		}
	}
