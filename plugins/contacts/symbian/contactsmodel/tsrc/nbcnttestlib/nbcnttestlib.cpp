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

#include <cntitem.h>
#include <cntfldst.h>
#include "nbcnttestlib.h"



TContact::TContact(CContactItem* aCnt)
:FirstName(KUidContactFieldGivenName, TUid::Null(), aCnt),
Email(KUidContactFieldVCardMapEMAILINTERNET, aCnt),
Address(KUidContactFieldAddress, aCnt),
iCnt(aCnt)
	{};




void TContact::TProp::operator= (const TDesC& aStr)
	{
	CContactItemField& field = Find();
	field.TextStorage()->SetTextL(aStr);
	}


TContact::TProp::operator const TPtrC()
	{
	const CContactItemField& field = Find();
	return field.TextStorage()->Text();
	}

CContactItemField& TContact::TProp::Find()
	{
	for (TInt fieldNum = iCnt->CardFields().Count() - 1; fieldNum>=0; --fieldNum)  //For all the fields of contact
		{
		CContactItemField& currField = iCnt->CardFields()[fieldNum]; //current contact field
		if (currField.ContentType().ContainsFieldType(iUid1))
			{
			if (iUid2 == TUid::Null()) 
				{ //No need to check for the second UID
				return currField;
				}
			else if (currField.ContentType().ContainsFieldType(iUid2))
				{
				return currField;
				}
			}
		}

	User::Leave(KErrBadName);	//We didn't find the field 

	CContactItemField* currField = NULL; //Unreachable code
	return *currField;
	}


//////////////////////////////////////////////


EXPORT_C TCnt::TGetSet& TCnt::operator [](TUid aUid)
	{
	//TInt32* id = reinterpret_cast<TInt32*>(&aUid);
	TInt32 mapping;

	switch(aUid.iUid)
		{
		case KUidContactFieldPhoneNumberValue:
		case KUidContactFieldFaxValue:
			mapping = KIntContactFieldVCardMapTEL;
		break;

		case KUidContactFieldEMailValue:		mapping = KIntContactFieldVCardMapEMAILINTERNET;
		break;
		case KUidContactFieldPostOfficeValue:	mapping = KIntContactFieldVCardMapPOSTOFFICE; 
		break;
		case KUidContactFieldExtendedAddressValue: mapping = KIntContactFieldVCardMapEXTENDEDADR; 
		break;
		case KUidContactFieldAddressValue: 		mapping = KIntContactFieldVCardMapADR; 
		break;
		case KUidContactFieldLocalityValue:		mapping = KIntContactFieldVCardMapLOCALITY; 
		break;
		case KUidContactFieldRegionValue: 		mapping = KIntContactFieldVCardMapREGION; 
		break;
		case KUidContactFieldPostCodeValue: 	mapping = KIntContactFieldVCardMapPOSTCODE; 
		break;
		case KUidContactFieldCountryValue: 		mapping = KIntContactFieldVCardMapCOUNTRY; 
		break;
		case KUidContactFieldUrlValue:			mapping = KIntContactFieldVCardMapURL; 
		break;
		case KUidContactFieldSecondNameValue: 	mapping = KIntContactFieldVCardMapSECONDNAME; 
		break;
		case KUidContactFieldCompanyNameValue: 	mapping = KIntContactFieldVCardMapORG; 
		break;
		case KUidContactFieldJobTitleValue: 	mapping = KIntContactFieldVCardMapTITLE; 
		break;
		case KUidContactFieldPictureValue: 		mapping = KIntContactFieldVCardMapLOGO; 
		break;
		case KUidContactFieldBirthdayValue: 	mapping = KIntContactFieldVCardMapBDAY; 
		break;
		case KUidContactFieldNoteValue: 		mapping = KIntContactFieldVCardMapNOTE; 
		break;
		case KUidContactFieldCompanyNamePronunciationValue: mapping = KIntContactFieldVCardMapORGPronunciation; 
		break;
		case KUidContactFieldSIPIDValue: 		mapping = KIntContactFieldVCardMapSIPID; 
		break;
		default:
			mapping = KIntContactFieldVCardMapUnusedN;
		};

	iTaker.iContType = CContentType::NewL(aUid, TUid::Uid(mapping));
	CleanupStack::PushL(iTaker.iContType);
	return iTaker;
	}

EXPORT_C TCnt::TGetSet& TCnt::TGetSet::operator[](TInt aIndex)
	{
	iIndex = aIndex;
	return *this;
	}

EXPORT_C TCnt::TGetSet& TCnt::TGetSet::operator&(TUid aUid)
	{
	iContType->AddFieldTypeL(aUid);
	return *this;
	}

_LIT(KFieldNotFound, "Field Not Found");

EXPORT_C void TCnt::TGetSet::operator= (const TDesC& aStr)
	{
	CContactItemField* aField = Find();
	__ASSERT_ALWAYS(aField,  User::Panic(KFieldNotFound, KErrBadName));
	aField->TextStorage()->SetTextL(aStr);
	}

EXPORT_C TBool TCnt::TGetSet::operator==(const TDesC& aStr)
	{
	CContactItemField* aField = Find();
	__ASSERT_ALWAYS(aField,  User::Panic(KFieldNotFound, KErrBadName));
	return aField->TextStorage()->Text() == aStr;
	}

EXPORT_C void TCnt::TGetSet::operator+= (const TDesC& aStr)
	{
	//Create the field if it is not found
	CContactItemField* field = CContactItemField::NewLC(KStorageTypeText, *iContType);
 	iCnt->AddFieldL(*field); //Takes ownership

	field->TextStorage()->SetTextL(aStr);

	CleanupStack::Pop(field); 
	CleanupStack::PopAndDestroy(iContType);
	}

EXPORT_C TInt TCnt::TGetSet::Count()
	{
	TInt counter(0);
	const TInt numOfFields = iCnt->CardFields().Count();
	for (TInt fieldNum = 0; fieldNum < numOfFields; ++fieldNum)  //For all the fields of contact
		{
		CContactItemField& currField = iCnt->CardFields()[fieldNum]; //current contact field
		if (currField.ContentType() == *iContType)
			{
			++counter;
			}
		}
	return counter;
	}


EXPORT_C CContactItemField& TCnt::TGetSet::Field()
	{
	return *Find();
	}


EXPORT_C TCnt::TGetSet::operator const TPtrC()
	{
	const CContactItemField* aField = Find();
	__ASSERT_ALWAYS(aField,  User::Panic(KFieldNotFound, KErrBadName));
	return aField->TextStorage()->Text();
	}

extern const TText* VCardMaping(const TFieldType& type);



CContactItemField* TCnt::TGetSet::Find()
	{
	TInt fieldsToSkip = iIndex;
	const TInt numOfFields = iCnt->CardFields().Count();

	for (TInt fieldNum = 0; fieldNum < numOfFields; ++fieldNum)  //For all the fields of contact
		{
		CContactItemField& currField = iCnt->CardFields()[fieldNum]; //current contact field
		if (currField.ContentType() == *iContType)
			{
			if (!fieldsToSkip)
				{
				CleanupStack::PopAndDestroy(iContType);
				return &currField;
				}
			else
				{
				--fieldsToSkip;
				}
			}
		}

	return NULL;
	}




// Contact Builder.

EXPORT_C CCntItemBuilder* CCntItemBuilder::NewLC(const CContactTemplate& aTemplate)
	{
	CCntItemBuilder* self = new (ELeave) CCntItemBuilder(aTemplate);
	CleanupStack::PushL(self);
	return self;
	}

	
void CCntItemBuilder::ConstructL()
	{
	iCntModifier = CCntItemModifier::NewLC();
	CleanupStack::Pop(iCntModifier);
	}

EXPORT_C CCntItemBuilder::~CCntItemBuilder() 
	{
	delete iCntModifier;
	}

EXPORT_C CContactItem* CCntItemBuilder::GetCntItemLC()
	{
	CContactItem* contactItem = CContactCard::NewLC(&iTemplate);
	FillWithRandomDataL(*contactItem);
	return contactItem; //return ownership
	}



void CCntItemBuilder::FillWithRandomDataL(CContactItem& aContactItem)
	{
	HBufC* buf=HBufC::NewLC(256);
	TPtr bufPtr=buf->Des();
	SetRandomAlphaString(bufPtr,32);
// 1
	iCntModifier->SetTextFieldL(KUidContactFieldGivenName, bufPtr, aContactItem);
	SetRandomAlphaString(bufPtr,32);
// 2
	iCntModifier->SetTextFieldL(KUidContactFieldFamilyName, bufPtr, aContactItem);

	SetRandomAlphaString(bufPtr,32);
// 3
	iCntModifier->SetTextFieldL(KUidContactFieldCompanyName, bufPtr, aContactItem);

	SetRandomAlphaString(bufPtr,32);
// 4
	iCntModifier->SetTextFieldL(KUidContactFieldJobTitle, bufPtr, aContactItem);
	
	SetRandomNumericString(bufPtr,24);
// 5
	iCntModifier->SetTextFieldL(KUidContactFieldPhoneNumber,
				  KUidContactFieldVCardMapWORK,
   				  KUidContactFieldVCardMapVOICE, 
   				  bufPtr, aContactItem);
	
	SetRandomNumericString(bufPtr,24);
// 6		
	iCntModifier->SetTextFieldL(KUidContactFieldPhoneNumber,
				KUidContactFieldVCardMapWORK, 
				KUidContactFieldVCardMapVOICE,
				KUidContactFieldVCardMapCELL,
		 		bufPtr, aContactItem);


	SetRandomNumericString(bufPtr,24);
// 7
	iCntModifier->SetTextFieldL(KUidContactFieldPhoneNumber, 
				 KUidContactFieldVCardMapHOME, 
				 KUidContactFieldVCardMapVOICE, 
				 bufPtr, aContactItem);

	SetRandomNumericString(bufPtr,24);    
// 8	
	
	iCntModifier->SetTextFieldL(KUidContactFieldPhoneNumber,
				  KUidContactFieldVCardMapHOME,
 				  KUidContactFieldVCardMapVOICE,
				  KUidContactFieldVCardMapCELL,
				  bufPtr, aContactItem);
				
	SetRandomNumericString(bufPtr,24);

// 9	
	iCntModifier->SetTextFieldL(KUidContactFieldFax, 
				 bufPtr, aContactItem);
	
	SetRandomAlphaNumericString(bufPtr,48);

// 10	
	iCntModifier->SetTextFieldL(KUidContactFieldEMail,
				  KUidContactFieldVCardMapWORK,
				  bufPtr, aContactItem);

	SetRandomAlphaNumericString(bufPtr,48);

// 11	
	iCntModifier->SetTextFieldL(KUidContactFieldUrl, 
				  bufPtr, aContactItem);

	SetRandomAlphaNumericString(bufPtr,32);

// 12	
	iCntModifier->SetTextFieldL(KUidContactFieldAddress, 
				  bufPtr, aContactItem);

	SetRandomAlphaString(bufPtr,32);
// 13
	iCntModifier->SetTextFieldL(KUidContactFieldLocality, 
				  bufPtr, aContactItem);

	SetRandomAlphaString(bufPtr,32);
// 14
	iCntModifier->SetTextFieldL(KUidContactFieldRegion, 
				  bufPtr, aContactItem);

	SetRandomAlphaNumericString(bufPtr,16);
// 15
	iCntModifier->SetTextFieldL(KUidContactFieldPostcode, 
				  bufPtr, aContactItem);
				
	SetRandomAlphaString(bufPtr,32);
// 16
	iCntModifier->SetTextFieldL(KUidContactFieldCountry, 
				  bufPtr, aContactItem);
	SetRandomAlphaNumericString(bufPtr,256);
// 17
	iCntModifier->SetTextFieldL(KUidContactFieldNote, 
				  bufPtr, aContactItem);

	CleanupStack::PopAndDestroy(buf); // buf
	}


void CCntItemBuilder::SetRandomWordL(TDes& aBuf, TInt32 aType )
	{
	TInt len = 16;
	SetRandomAlphaString(aBuf, len);

	// the following are to make these look a bite more realistic
	if(aType==KUidContactFieldEMailValue)	
		{
		aBuf.Append('@');
		HBufC* buf  = HBufC::NewLC (KEmailAddrLen);
		SetRandomAlphaString (aBuf, KEmailAddrLen);
		aBuf.Append (buf->Des());
		CleanupStack::PopAndDestroy(buf);
		aBuf.Append (_L(".co.uk"));
		}
	if(aType==KUidContactFieldCompanyNameValue || aType==KUidContactFieldCompanyNamePronunciationValue)
		{
		aBuf.Append(' ');
		aBuf.Append(_L(" Ltd."));
		}
	}


void CCntItemBuilder::SetRandomAlphaString(TDes& aBuf,TInt aLength)
	{
	aBuf.SetLength(aLength);

	for (TInt i=0; i<aLength; ++i)
		{
		aBuf[i]=RandomAlphaCharCode();
		}
	}


TText CCntItemBuilder::RandomAlphaCharCode()
	{
	return RandomCharCode('A','z',' ');
	}

TText CCntItemBuilder::RandomNumericCharCode()
	{
	return RandomCharCode('0','9',' ');
	}

TText CCntItemBuilder::RandomAlphaNumbericCharCode()
	{
	return RandomCharCode('0','z',' ');
	}

TText CCntItemBuilder::RandomCharCode(TText aLowerBound,TText aUpperBound,TText aException)
	{
	TText charCode = 0;
	do
		{
		charCode = RandomCharCode(aLowerBound,aUpperBound);
		}
	while (charCode == aException);

	return charCode;
	}
	
TText CCntItemBuilder::RandomCharCode(TText aLowerBound,TText aUpperBound)
	{
//	TTime  now;
//	now.UniversalTime();
	TInt64 randSeed = 45879;
//	TInt64 randSeed = now.Int64();
	TText  charCode = STATIC_CAST(TText,(Math::Rand(randSeed)%(aUpperBound-aLowerBound))+aLowerBound);

	ASSERT(charCode >= aLowerBound && charCode <= aUpperBound);
	return charCode;
	}
	

void CCntItemBuilder::SetRandomNumericString(TDes& aBuf, TInt aLength)
	{
	aBuf.SetLength(aLength);

	for (TInt ii = 0; ii < aLength; ++ii)
		{
		aBuf[ii]=RandomNumericCharCode();
		}
	}


void CCntItemBuilder::SetRandomAlphaNumericString(TDes& aBuf,TInt aLength)
	{
	aBuf.SetLength(aLength);
	for (TInt ii = 0; ii < aLength; ++ii)
		{
		aBuf[ii]=RandomAlphaNumbericCharCode();
		}
	}







// ---------- CCntItemModifier ----------------


EXPORT_C CCntItemModifier* CCntItemModifier::NewLC()
	{
	CCntItemModifier* self = new (ELeave) CCntItemModifier();
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C CCntItemModifier::~CCntItemModifier()
	{
	}


EXPORT_C void CCntItemModifier::SetTextFieldL(TFieldType aFieldType, const TDesC& aText, CContactItem& aContactItem)
	{
	CContactItemFieldSet& fields = aContactItem.CardFields();
	TInt fieldIndex = aContactItem.CardFields().Find(aFieldType);

	if (fieldIndex > KErrNotFound)
		{
		CContactItemField& field = fields[ fieldIndex ];
		ASSERT(field.StorageType() == KStorageTypeText);
		
		STATIC_CAST(CContactTextField*, field.Storage())->SetText(aText.AllocL());
		}
	else
		{
		InsertTextFieldL(aFieldType, aText, aContactItem);
		}
	}



EXPORT_C void CCntItemModifier::SetTextFieldL(TFieldType aFieldType, TFieldType a2ndFieldType,
							const TDesC& aText, 
							CContactItem& aContactItem) const
	{
	CContentType* contentTypeToMatch = CContentType::NewL(); 
	CleanupStack::PushL (contentTypeToMatch);
	contentTypeToMatch->AddFieldTypeL (aFieldType);
	contentTypeToMatch->AddFieldTypeL (a2ndFieldType);

	CContactItemField& field   =  FindFieldL(*contentTypeToMatch, aContactItem);
	ASSERT(field.StorageType() == KStorageTypeText);
	STATIC_CAST(CContactTextField*, field.Storage())->SetText(aText.AllocL());
	CleanupStack::PopAndDestroy(); // contentTypeToMatch.

	}


EXPORT_C void CCntItemModifier::SetTextFieldL(TFieldType aFieldType, TFieldType a2ndFieldType,
							TFieldType a3rdFieldType,  
							const TDesC& aText, CContactItem& aContactItem) const
	{
	CContentType* contentTypeToMatch = CContentType::NewL(); 
	CleanupStack::PushL (contentTypeToMatch);
	contentTypeToMatch->AddFieldTypeL (aFieldType);
	contentTypeToMatch->AddFieldTypeL (a2ndFieldType);
	contentTypeToMatch->AddFieldTypeL (a3rdFieldType);

	CContactItemField& field   =  FindFieldL(*contentTypeToMatch, aContactItem);
	ASSERT(field.StorageType() == KStorageTypeText);
	STATIC_CAST(CContactTextField*, field.Storage())->SetText(aText.AllocL());
	CleanupStack::PopAndDestroy(); // contentTypeToMatch.
	}


EXPORT_C void CCntItemModifier::SetTextFieldL(TFieldType aFieldType, TFieldType a2ndFieldType,
							TFieldType a3rdFieldType, TFieldType a4thFieldType, 
							const TDesC& aText, CContactItem& aContactItem) const
	{
	CContentType* contentTypeToMatch = CContentType::NewL(); 
	CleanupStack::PushL (contentTypeToMatch);
	contentTypeToMatch->AddFieldTypeL (aFieldType);
	contentTypeToMatch->AddFieldTypeL (a2ndFieldType);
	contentTypeToMatch->AddFieldTypeL (a3rdFieldType);
	contentTypeToMatch->AddFieldTypeL (a4thFieldType);

	CContactItemField& field   =  FindFieldL(*contentTypeToMatch, aContactItem);
	ASSERT(field.StorageType() == KStorageTypeText);
	STATIC_CAST(CContactTextField*, field.Storage())->SetText(aText.AllocL());
	CleanupStack::PopAndDestroy(); // contentTypeToMatch.
	}


EXPORT_C TBool CCntItemModifier::FindTextFieldL(TFieldType aFieldType, TFieldType a2ndFieldType,
							TFieldType a3rdFieldType, TFieldType a4thFieldType, 
							const TDesC& aText, CContactItem& aContactItem) const
							
	{
	CContentType* contentTypeToMatch = CContentType::NewL(); 
	CleanupStack::PushL (contentTypeToMatch);
	contentTypeToMatch->AddFieldTypeL (aFieldType);
	contentTypeToMatch->AddFieldTypeL (a2ndFieldType);
	contentTypeToMatch->AddFieldTypeL (a3rdFieldType);
	contentTypeToMatch->AddFieldTypeL (a4thFieldType);

	CContactItemField& field   =  FindFieldL(*contentTypeToMatch, aContactItem);
	CContactTextField* textField = static_cast<CContactTextField*> (field.Storage());
	CleanupStack::PopAndDestroy(); // contentTypeToMatch.
	if (textField->Text() == aText)
		{
		return TRUE;
		}
	return FALSE;
	}


EXPORT_C TBool CCntItemModifier::FindTextFieldL(TFieldType aFieldType, 
							const TDesC& aText, 
							CContactItem& aContactItem)const
	{
	CContactItemFieldSet& fields = aContactItem.CardFields();
	TInt fieldIndex = aContactItem.CardFields().Find(aFieldType);
	CContactItemField& field = fields[ fieldIndex ];
	STATIC_CAST(CContactTextField*, field.Storage())->SetText(aText.AllocL());
	CContactTextField* textField = static_cast<CContactTextField*> (field.Storage());
	if (textField->Text() == aText)
		{
		return TRUE;
		}
	return FALSE;
	}


EXPORT_C TBool CCntItemModifier::FindTextFieldL(TFieldType aFieldType, TFieldType a2ndFieldType,
							const TDesC& aText, 
							CContactItem& aContactItem) const
	{
	CContentType* contentTypeToMatch = CContentType::NewL(); 
	CleanupStack::PushL (contentTypeToMatch);
	contentTypeToMatch->AddFieldTypeL (aFieldType);
	contentTypeToMatch->AddFieldTypeL (a2ndFieldType);

	CContactItemField& field   =  FindFieldL(*contentTypeToMatch, aContactItem);
	CContactTextField* textField = static_cast<CContactTextField*> (field.Storage());
	CleanupStack::PopAndDestroy(); // contentTypeToMatch.
	if (textField->Text() == aText)
		{
		return TRUE;
		}
	return FALSE;
	}



TInt CCntItemModifier::FindField(TFieldType aFieldType, CContactItem& aContactItem) const
	{
	return aContactItem.CardFields().Find( aFieldType );
	}


CContactItemField& CCntItemModifier::FindFieldL(TFieldType aFieldType, CContactItem& aContactItem) const
	{
	CContactItemFieldSet& fields = aContactItem.CardFields();
	TInt index=fields.Find(aFieldType);
	User::LeaveIfError(index);
	return fields[index];
	}


CContactItemField& CCntItemModifier::FindFieldL(const CContentType& aTypesToMatch, CContactItem& aContactItem) const
	{
	CContactItemFieldSet& fields = aContactItem.CardFields();
	const TInt numTypesToMatch (aTypesToMatch.FieldTypeCount());
	const TFieldType mainFieldType (aTypesToMatch.FieldType(0));
	TInt index=-1;
	FOREVER
		{
		index = fields.FindNext(mainFieldType,index+1);
		User::LeaveIfError(index);

		CContactItemField& thisPhoneField=fields[index];
		const CContentType& thisPhoneFieldTypes=thisPhoneField.ContentType();
		const TInt numFieldTypes(thisPhoneFieldTypes.FieldTypeCount());

		if (numFieldTypes != numTypesToMatch)
			{
			continue;
			}

		TInt numTypesLeftToMatch = numTypesToMatch;
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


void CCntItemModifier::InsertTextFieldL(TFieldType aID, const TDesC& aStringOfStuff, CContactItem& aContactItem)
	{
	CContactItemField* itemFieldPtr   = CContactItemField::NewLC( KStorageTypeText, aID );
	CContactTextField* textStoragePtr = itemFieldPtr->TextStorage(); // we don't have ownership here, hence no stack.

	textStoragePtr->SetTextL( aStringOfStuff );
	textStoragePtr = NULL; 

	aContactItem.AddFieldL(*itemFieldPtr); // hand ownership over here
	CleanupStack::Pop(itemFieldPtr); // and pop,
	}






#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
	{
	return KErrNone;
	}
#endif
