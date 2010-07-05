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
*
*/


#include <cntfldst.h>
#include "t_packagertesthelper.h"

//_LIT(KSecureDBName, "c:CONTACTS.CDB");

CPackagerCntFactory::CPackagerCntFactory()
/** Constructor. */ 
 	{
 	}

CPackagerCntFactory::~CPackagerCntFactory()
/** The destructor frees all resources owned by the factory, 
prior to its destruction. */
	{
	delete iContactItem;
	delete iContentType;
	delete iContactTextDef;
	delete iContactItemViewDef;
	delete iContactViewDef;
	delete iContactIdArray;
	delete iContactFilter;
	delete iViewContact;
	delete iDesCArray;	
	}
	
CPackagerCntFactory* CPackagerCntFactory::NewL()
/** Allocates and constructs a new default creator. 

@return Pointer to the new CPackagerCntFactory object. */
	{ // static
	return (new(ELeave) CPackagerCntFactory);
	} 	
		
CContactItem& CPackagerCntFactory::CreateDefaultCContactItemL() const
/** Allocates and constructs a new default CContactItem if it
	hasn't been created already. 

@return a Reference to the member CContactItem object. */
	{	
	if(!iContactItem)
		{
		iContactItem = doCreateDefaultCContactItemL();
		}	
	return *iContactItem;
	}
CContentType& CPackagerCntFactory::CreateDefaultCContentTypeL() const
/** Allocates and constructs a new default CContentType if it
	hasn't been created already. 

@return a Reference to the member CContentType object. */
	{
	if(!iContentType)
		{
		iContentType = doCreateDefaultCContentTypeL();
		}	
	return *iContentType;	
	}
CContactTextDef& CPackagerCntFactory::CreateDefaultCContactTextDefL() const
/** Alloc,
ates and constructs a new default CContactTextDef if it
	hasn't been created already. 

@return a Reference to the member CContactTextDef object. */
	{
	if(!iContactTextDef)
		{
		iContactTextDef = doCreateDefaultCContactTextDefL();
		}		
	return *iContactTextDef;	
	}
CContactItemViewDef& CPackagerCntFactory::CreateDefaultCContactItemViewDefL() const
/** Allocates and constructs a new default CContactItemViewDef if it
	hasn't been created already. 

@return a Reference to the member CContactItemViewDef object. */
	{
	if(!iContactItemViewDef)
		{
		iContactItemViewDef = doCreateDefaultCContactItemViewDefL();
		}		
	return *iContactItemViewDef;	
	}
CContactViewDef& CPackagerCntFactory::CreateDefaultCContactViewDefL() const
/** Allocates and constructs a new default CContactViewDef if it
	hasn't been created already. 

@return a Reference to the member CContactViewDef object. */
	{
	if(!iContactViewDef)
		{
		iContactViewDef = CContactViewDef::NewL(doCreateDefaultCContactItemViewDefL());
		}		
	return *iContactViewDef;	
	}
CContactIdArray& CPackagerCntFactory::CreateDefaultCContactIdArrayL() const
/** Allocates and constructs a new default CContactIdArray if it
	hasn't been created already. 

@return a Reference to the member CContactIdArray object. */
	{
	if(!iContactIdArray)
		{
		iContactIdArray = doCreateDefaultCContactIdArrayL();
		}		 
	return *iContactIdArray;	
	}
CCntFilter& CPackagerCntFactory::CreateDefaultCCntFilterL() const
/** Allocates and constructs a new default CCntFilter if it
	hasn't been created already. 

@return a Reference to the member CCntFilter object. */
	{
	if(!iContactFilter)
		{
		iContactFilter = doCreateDefaultCCntFilterL();
		}		
	return *iContactFilter;	
	}
CViewContact& CPackagerCntFactory::CreateDefaultCViewContactL() const
/** Allocates and constructs a new default CViewContact if it
	hasn't been created already. 

@return a Reference to the member CViewContact object. */
	{
	if(!iViewContact)
		{
		iViewContact = doCreateDefaultCViewContactL();
		}		
	return *iViewContact;	
	}
CDesCArray& CPackagerCntFactory::CreateDefaultCDesCArrayL() const
/** Allocates and constructs a new default CDesCArray if it
	hasn't been created already. 

@return a Reference to the member CDesCArray object. */
	{
	if(!iDesCArray)
		{
		iDesCArray = doCreateDefaultCDesCArrayL();
		}		
	return *iDesCArray;
	}

CContactItem* CPackagerCntFactory::doCreateDefaultCContactItemL() const
/** Implementation method for constructing a new default CContactItem object. 

@return a Pointer to the CContactItem object. */
	{
   	CPlTestImpl* plCntTestImpl = CPlTestImpl::NewLC();
   	plCntTestImpl->CreateDatabaseL(nsCntTestImpl::KSecureDBName, ETrue);

	const CContactTemplate& goldenTemplate = plCntTestImpl->GetSysTemplate();//const_cast<CContactTemplate&>(thePersistenceLayer->ContactProperties().SystemTemplateL());;
	CContactItem* theItem = CContactCard::NewLC(&goldenTemplate);
	
	TCnt theCnt(theItem);
	theCnt[KUidContactFieldGivenName] = KContactItemGivenName;
	theCnt[KUidContactFieldFamilyName] = KContactItemFamilyName;

	theCnt[KUidContactFieldPhoneNumber] & KHome & KVoice & KCell = KContactItemFirstPhoneNumber;
	theCnt[KUidContactFieldPhoneNumber] & KHome & KVoice & KCell += KContactItemSecondPhoneNumber;

	theCnt[KUidContactFieldEMail] & KHome = KContactItemFirstEmail;
	theCnt[KUidContactFieldAddress] & KHome = KContactItemFirstAddress;
	theCnt[KUidContactFieldLocality] & KHome = KContactItemFirstLocality;
	theCnt[KUidContactFieldIMAddress] & KHome & KWv = KContactItemIMAddress;
	theCnt[KUidContactFieldSecondName] = KContactItemSecondName;

	theCnt[KUidContactFieldCompanyName] = KContactItemCompanyName;
	theCnt[KUidContactFieldJobTitle]	= KContactItemJobTitle;
	theCnt[KUidContactFieldPhoneNumber] & KWork & KVoice = KContactItemThirdPhoneNumber;

	theCnt[KUidContactFieldEMail] & KWork = KContactItemSecondEmail;
	theCnt[KUidContactFieldUrl] & KWork = KContactItemURL;
	theCnt[KUidContactFieldAddress] & KWork = KContactItemSecondAddress;
	theCnt[KUidContactFieldLocality] & KWork = KContactItemSecondLocality;

	theCnt[KUidContactFieldCustom1] = _L("Filterable 1");
	theCnt[KUidContactFieldCustom2] = _L("Filterable 2");

	theCnt[KUidContactFieldGivenNamePronunciation] = KContactItemGivenNamePronunciation;
	theCnt[KUidContactFieldFamilyNamePronunciation] = KContactItemFamilyNamePronunciation;
	
	theCnt[KUidContactFieldSIPID] & KPoc  = KContactItemFirstSIPID;
	theCnt[KUidContactFieldSIPID] & KSwis = KContactItemSecondSIPID;
	theCnt[KUidContactFieldSIPID] & KVoip = KContactItemThirdSIPID;
	
	CleanupStack::Pop(theItem);
	CleanupStack::PopAndDestroy(plCntTestImpl);
	return theItem;
	}
	
CContentType* CPackagerCntFactory::doCreateDefaultCContentTypeL() const
/** Implementation method for constructing a new default CContentType object. 

@return a Pointer to the CContentType object. */
	{
	CContactItem* theItem = doCreateDefaultCContactItemL();
	CleanupStack::PushL(theItem);
	CContentType* theContent = CContentType::NewL(theItem->CardFields()[0].ContentType());
	CleanupStack::PopAndDestroy(theItem);
	return theContent;
	}	
	
	
CContactTextDef* CPackagerCntFactory::doCreateDefaultCContactTextDefL() const
/** Implementation method for constructing a new default CContactTextDef object. 

@return a Pointer to the CContactTextDef object. */
	{
	CContactTextDef* theTextDef = CContactTextDef::NewLC();
	theTextDef->AppendL(TContactTextDefItem(KUidContactFieldFamilyName));
	theTextDef->AppendL(TContactTextDefItem(KUidContactFieldGivenName));
	theTextDef->AppendL(TContactTextDefItem(KUidContactFieldCompanyName));
	theTextDef->AppendL(TContactTextDefItem(KUidContactFieldAddress));
	theTextDef->AppendL(TContactTextDefItem(KUidContactFieldEMail));
	theTextDef->AppendL(TContactTextDefItem(KUidContactFieldAdditionalName));
	CleanupStack::Pop(theTextDef);
	return theTextDef;
	}	
	
	
CContactItemViewDef* CPackagerCntFactory::doCreateDefaultCContactItemViewDefL() const
/** Implementation method for constructing a new default CContactItemViewDef object. 

@return a Pointer to the CContactItemViewDef object. */
	{
	CContactItemViewDef* theViewDef=CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,CContactItemViewDef::EMaskHiddenFields);
	theViewDef->AddL(KUidContactFieldGivenName);
	theViewDef->AddL(KUidContactFieldFamilyName);
	theViewDef->AddL(KUidContactFieldPhoneNumber);
	theViewDef->AddL(KUidContactFieldCompanyName);
	theViewDef->AddL(KUidContactFieldAddress);
	CleanupStack::Pop(theViewDef);
	return theViewDef;	
	}				
	
CContactIdArray* CPackagerCntFactory::doCreateDefaultCContactIdArrayL() const
/** Implementation method for constructing a new default CContactIdArray object. 

@return a Pointer to the CContactIdArray object. */
	{
	CContactIdArray* theArray = CContactIdArray::NewL();
	CleanupStack::PushL(theArray);
	theArray->AddL(KId1);
	theArray->AddL(KId2);
	theArray->AddL(KId3);
	theArray->AddL(KId4);
	theArray->AddL(KId5);
	CleanupStack::Pop(theArray);
	return theArray;
	}	
	
CCntFilter* CPackagerCntFactory::doCreateDefaultCCntFilterL() const
/** Implementation method for constructing a new default CCntFilter object. 

@return a Pointer to the CCntFilter object. */
	{
	CCntFilter* theFilter = CCntFilter::NewLC();
	theFilter->SetContactFilterTypeCard(ETrue);
	TTime time;
	time.UniversalTime();
	time-=TTimeIntervalMinutes(KInterval); // changes in the last KInterval mins
	theFilter->SetFilterDateTime(time);
	theFilter->SetIncludeNewContacts(ETrue);
	theFilter->SetContactFilterTypeALL(ETrue);
	theFilter->iIds = doCreateDefaultCContactIdArrayL();
	CleanupStack::Pop(theFilter);
	return theFilter;
	}	
	
CViewContact* CPackagerCntFactory::doCreateDefaultCViewContactL() const
/** Implementation method for constructing a new default CPackagerCntFactory object. 

@return a Pointer to the CPackagerCntFactory object. */
	{
	CViewContact* theContact = CViewContact::NewLC(KId5);
	theContact->AddFieldL(KContactItemGivenName);
	theContact->AddFieldL(KContactItemFamilyName);
	theContact->AddFieldL(KNullDesC);
	CleanupStack::Pop(theContact);
	return theContact;	
	}
	
CDesCArray* CPackagerCntFactory::doCreateDefaultCDesCArrayL() const
/** Implementation method for constructing a new default CDesCArray object. 

@return a Pointer to the CDesCArray object. */
	{
	CDesCArray* theTestArray = new(ELeave) CDesCArrayFlat(5);
	CleanupStack::PushL(theTestArray);
	theTestArray->AppendL(KContactItemCompanyName);
	theTestArray->AppendL(KContactItemFamilyName);
	theTestArray->AppendL(KContactItemGivenName);
	theTestArray->AppendL(KContactItemFirstPhoneNumber);
	theTestArray->AppendL(KContactItemSecondPhoneNumber);
	CleanupStack::Pop(theTestArray);
	return theTestArray;
	}					

CPackagerCntComparator::CPackagerCntComparator()
/** Constructor. */ 
 	{
 	}
 	
CPackagerCntComparator::~CPackagerCntComparator()
/** The destructor frees all resources owned by the comparator, 
prior to its destruction. */
	{
	}	 	
 	
CPackagerCntComparator* CPackagerCntComparator::NewL()
/** Allocates and constructs a new default comparator. 

@return Pointer to the new CPackagerCntComparator object. */
	{ // static
	 return (new(ELeave) CPackagerCntComparator);
	} 	

TBool CPackagerCntComparator::Compare(const CContactItem& anItem1, const CContactItem& anItem2) const
/** Compares two CContactItem items. 

@param anItem1 The first CContactItem to be compared.
@param anItem2 The second CContactItem to be compared.
@return ETrue if the two items are equal, EFalse otherwise. */
	{
	return (DoCompareCContactItemType(anItem1, anItem2) 							&
		DoCompareCContactItemFieldSet(anItem1.CardFields(), anItem2.CardFields())	&
		DoCompareCContactItemAttribute(anItem1, anItem2)							&
		DoCompareTContactItemId(anItem1.Id(), anItem2.Id())							&
		DoCompareTContactItemId(anItem1.TemplateRefId(), anItem2.TemplateRefId())	&
		DoCompareTTime(anItem1.LastModified(), anItem2.LastModified())				&
		DoCompareTTime(anItem1.iCreationDate, anItem2.iCreationDate)				&
		DoCompareTInt(anItem1.AccessCount(), anItem2.AccessCount())					&
		DoCompareTPtrC(const_cast<CContactItem&>(anItem1).Guid(), const_cast<CContactItem&>(anItem2).Guid()));

	}
		
TBool CPackagerCntComparator::Compare(const CContentType& anItem1, const CContentType& anItem2) const
/** Compares two CContentType items. 

@param anItem1 The first CContentType to be compared.
@param anItem2 The second CContentType to be compared.
@return ETrue if the two items are equal, EFalse otherwise. */
	{	
	return (anItem1==anItem2);				
	}
	
TBool CPackagerCntComparator::Compare(const CContactTextDef& anItem1, const CContactTextDef& anItem2) const
/** Compares two CContactTextDef items. 

@param anItem1 The first CContactTextDef to be compared.
@param anItem2 The second CContactTextDef to be compared.
@return ETrue if the two items are equal, EFalse otherwise. */
	{
	return(DoCompareCContactTextDefArray(anItem1, anItem2)					&
		DoCompareTUid(anItem1.FallbackField(), anItem2.FallbackField())	&
		DoCompareTInt(const_cast<CContactTextDef&>(anItem1).ExactMatchOnly(), const_cast<CContactTextDef&>(anItem2).ExactMatchOnly()));
	}
	
TBool CPackagerCntComparator::Compare(const CContactItemViewDef& anItem1, const CContactItemViewDef& anItem2) const
/** Compares two CContactItemViewDef items. 

@param anItem1 The first CContactItemViewDef to be compared.
@param anItem2 The second CContactItemViewDef to be compared.
@return ETrue if the two items are equal, EFalse otherwise. */
	{
	return (DoCompareTInt(anItem1.Mode(),anItem2.Mode()) 	&
		DoCompareTInt(anItem1.Use(),anItem2.Use())			&
		DoCompareCContactItemViewDefArray(anItem1, anItem2));
	
	}
	
TBool CPackagerCntComparator::Compare(const CContactViewDef& anItem1, const CContactViewDef& anItem2) const
/** Compares two CContactViewDef items. 

@param anItem1 The first CContactViewDef to be compared.
@param anItem2 The second CContactViewDef to be compared.
@return ETrue if the two items are equal, EFalse otherwise. */
	{
	return Compare(anItem1.ItemDef(), anItem2.ItemDef());
	}
	
TBool CPackagerCntComparator::Compare(const CContactIdArray& anItem1, const CContactIdArray& anItem2) const
/** Compares two CContactIdArray items. 

@param anItem1 The first CContactIdArray to be compared.
@param anItem2 The second CContactIdArray to be compared.
@return ETrue if the two items are equal, EFalse otherwise. */
	{// Need to check for NULL  arrays first.	
	return DoCompareCContactIdArray(anItem1, anItem2);
	}
	
TBool CPackagerCntComparator::Compare(const CCntFilter& anItem1, const CCntFilter& anItem2) const
/** Compares two CCntFilter items. 

@param anItem1 The first CCntFilter to be compared.
@param anItem2 The second CCntFilter to be compared.
@return ETrue if the two items are equal, EFalse otherwise. */
	{
	return (Compare(*anItem1.iIds, *anItem2.iIds)				&
		DoCompareTTime(const_cast<CCntFilter&>(anItem1).GetFilterDateTime(), const_cast<CCntFilter&>(anItem2).GetFilterDateTime()) &
		DoCompareCCntFilterInclude(anItem1, anItem2)			&
		DoCompareCCntFilterContactTypeL(anItem1, anItem2));
	
	}
	
TBool CPackagerCntComparator::Compare(const CViewContact& anItem1, const CViewContact& anItem2) const
/** Compares two CViewContact items. 

@param anItem1 The first CViewContact to be compared.
@param anItem2 The second CViewContact to be compared.
@return ETrue if the two items are equal, EFalse otherwise. */
	{
	return(DoCompareTUid(anItem1.ContactTypeUid(), anItem2.ContactTypeUid())&&
		DoCompareTContactItemId(anItem1.Id(), anItem2.Id())					&&
		DoCompareTInt(anItem1.ContactType(), anItem2.ContactType())			&&
		DoCompareCViewContactField(anItem1, anItem2)						&&
		DoCompareTInt(anItem1.ContactHint(), anItem2.ContactHint()));
	}
	
TBool CPackagerCntComparator::Compare(const CDesCArray& anItem1, const CDesCArray& anItem2) const
/** Compares two CDesCArray items. 

@param anItem1 The first CDesCArray to be compared.
@param anItem2 The second CDesCArray to be compared.
@return ETrue if the two items are equal, EFalse otherwise. */
	{
	return DoCompareCDesCArray(anItem1, anItem2);
	}
	
	
TBool CPackagerCntComparator::DoCompareCContactItemFieldSet(const CContactItemFieldSet& anItemFieldSet1, const CContactItemFieldSet& anItemFieldSet2) const
/** Compares two CContactItemFieldSet items. 

@param anItemFieldSet1 The first CContactItemFieldSet to be compared.
@param anItemFieldSet2 The second CContactItemFieldSet to be compared.
@return ETrue if the two items are equal, EFalse otherwise. */
	{// Need to check for NULL  arrays first.	
	if((!&anItemFieldSet1[0]) && (!&anItemFieldSet2[0]))
		{
		return ETrue;
		}

	if((!&anItemFieldSet1[0]) || (!&anItemFieldSet2[0]))
		{
		return EFalse;
		}
		
	// Check if arrays are same length to begin with.
	TInt maxCount = anItemFieldSet1.Count();
	if(!DoCompareTInt(maxCount, anItemFieldSet2.Count()))
		{
		return EFalse;
		}	
		
	for(TInt i=0; i<maxCount; ++i)
		{		
		if(!(DoCompareCContactItemField(anItemFieldSet1[i],anItemFieldSet2[i])))
			{
			return EFalse;	
			}
		}
		
	return ETrue;
	}
	
TBool CPackagerCntComparator::DoCompareCContactItemField(const CContactItemField& anItemField1, const CContactItemField& anItemField2) const
/** Compares two CContactItemField items. 

@param anItemField1 The first CContactItemField to be compared.
@param anItemField2 The second CContactItemField to be compared.
@return ETrue if the two items are equal, EFalse otherwise. */
	{
	
	// todo VERIFY COPNTENT TYPE LEAVE
	return(Compare(anItemField1.ContentType(), anItemField2.ContentType())	&
		DoCompareCContactItemFieldStorageTypeL(anItemField1, anItemField2)	&
		DoCompareTPtrC(anItemField1.Label(), anItemField2.Label())			&
		DoCompareTInt(anItemField1.Id(), anItemField2.Id())				&
		DoCompareCContactItemFieldAttribute(anItemField1, anItemField2)		&
		DoCompareCContactItemExtendedFieldAttribute(anItemField1, anItemField2));
		// TemplateFieldId;	Not Exported!!!!!
		// DoCompareTIntL(anItemField1.TemplateFieldId(), anItemField2.TemplateFieldId());
	}
	
TBool CPackagerCntComparator::DoCompareCContactItemFieldStorageTypeL(const CContactItemField& anItemField1, const CContactItemField& anItemField2) const
/** Compares two storage type items contained within their respective CContactItemField parent.

@param anItemField1 The first CContactItemField containing the storage type to be compared.
@param anItemField2 The second CContactItemField containing the storage type to be compared.
@return ETrue if the two items are equal, EFalse otherwise. */
	{
	// Check if storage type is same first, otherwise don't need to 
	// compare storage content.
	if(!DoCompareTUint(anItemField1.StorageType(), anItemField2.StorageType()))
		{
		return EFalse;
		}	
	
	TBool result = ETrue;
    switch (anItemField1.StorageType())
        {
    case KStorageTypeText:
		result = DoCompareTPtrC(anItemField1.TextStorage()->Text(), anItemField2.TextStorage()->Text());
        break;
        
    case KStorageTypeStore:
		result = (anItemField1.StoreStorage()->Thing()==anItemField2.StoreStorage()->Thing());
        break;
        
    case KStorageTypeDateTime:
		result = DoCompareTTime(anItemField1.DateTimeStorage()->Time(), anItemField2.DateTimeStorage()->Time());
        break;
        
    case KStorageTypeContactItemId:
		result = DoCompareTContactItemId(anItemField1.AgentStorage()->Value(), anItemField2.AgentStorage()->Value());
        break;
        
    default:
    	User::Leave(KErrNotSupported);
    	break;
        }
        		
	return result;	
	}
	
TBool CPackagerCntComparator::DoCompareTUid(const TUid& aTUid1, const TUid& aTUid2) const
/** Compares two TUid items. 

@param aTUid1 The first TUid to be compared.
@param aTUid2 The second TUid to be compared.
@return ETrue if the two items are equal, EFalse otherwise. */
	{
	return (aTUid1==aTUid2);
	}
	
TBool CPackagerCntComparator::DoCompareTContactItemId(const TContactItemId& aCntId1, const TContactItemId& aCntId2) const
/** Compares two TContactItemId items. 

@param aCntId1 The first TContactItemId to be compared.
@param aCntId2 The second TContactItemId to be compared.
@return ETrue if the two items are equal, EFalse otherwise. */
	{
	return (aCntId1==aCntId2);
	}
	
TBool CPackagerCntComparator::DoCompareTTime(const TTime& aTime1, const TTime& aTime2) const
/** Compares two TTime items. 

@param aTime1 The first TTime to be compared.
@param aTime2 The second TTime to be compared.
@return ETrue if the two items are equal, EFalse otherwise. */
	{
	return (aTime1==aTime2);
	}	
	
TBool CPackagerCntComparator::DoCompareTInt(const TInt& anInt1, const TInt& anInt2) const
/** Compares two integers. 

@param anInt1 The first integer to be compared.
@param anInt2 The second integer to be compared.
@return ETrue if the two integers are equal, EFalse otherwise. */
	{
	return (anInt1==anInt2);
	}
	
TBool CPackagerCntComparator::DoCompareTUint(const TUint& anInt1, const TUint& anInt2) const
/** Compares two unsigned integers. 

@param anInt1 The first unsigned integer to be compared.
@param anInt2 The second unsigned integer to be compared.
@return ETrue if the two unsigned integers are equal, EFalse otherwise. */
	{
	return (anInt1==anInt2);
	}

TBool CPackagerCntComparator::DoCompareTStorageType(const TStorageType& aStorageType1, const TStorageType& aStorageType2) const
/** Compares two TStorageType items. 

@param aStorageType1 The first TStorageType to be compared.
@param aStorageType2 The second TStorageType to be compared.
@return ETrue if the two items are equal, EFalse otherwise. */
	{
	return (aStorageType1==aStorageType2);
	}
	
TBool CPackagerCntComparator::DoCompareHBufC(const HBufC& aBuf1, const HBufC& aBuf2) const
/** Compares two HBufC items. 

@param aBuf1 The first HBufC to be compared.
@param aBuf2 The second HBufC to be compared.
@return ETrue if the two items are equal, EFalse otherwise. */
	{
	return (aBuf1==aBuf2);
	}		
	
TBool CPackagerCntComparator::DoCompareTPtrC(const TPtrC& aPtr1, const TPtrC& aPtr2) const
/** Compares two TPtrC items. 

@param aPtr1 The first TPtrC to be compared.
@param aPtr2 The second TPtrC to be compared.
@return ETrue if the two items are equal, EFalse otherwise. */
	{
	return (aPtr1==aPtr2);
	}
		
TBool CPackagerCntComparator::DoCompareCContactItemViewDefArray(const CContactItemViewDef& anItemViewDefArray1, const CContactItemViewDef& anItemViewDefArray2) const
/** Compares two TUid arrays contained within their respective CContactItemViewDef parent.

@param anItemViewDefArray1 The first array to be compared.
@param anItemViewDefArray2 The second array to be compared.
@return ETrue if the two arrays are equal, EFalse otherwise. */
	{// Need to check for NULL  arrays first.
	if((!&anItemViewDefArray1) && (!&anItemViewDefArray2))
		{
		return ETrue;
		}

	if((!&anItemViewDefArray1) || (!&anItemViewDefArray2))
		{
		return EFalse;
		}
			
	// Check if arrays are same length to begin with.
	TInt maxCount = anItemViewDefArray1.Count();
	if(!DoCompareTInt(maxCount, anItemViewDefArray2.Count()))
		{
		return EFalse;
		}
		
	for(TInt i=0; i<maxCount; ++i)
		{
		if(!DoCompareTUid(anItemViewDefArray1[i], anItemViewDefArray2[i]))
			{
			return EFalse;
			}
		}
		
	return ETrue;
	}
	
TBool CPackagerCntComparator::DoCompareCContactTextDefArray(const CContactTextDef& aTextDefArray1, const CContactTextDef& aTextDefArray2) const
/** Compares two CContactTextDef arrays. 

@param aTextDefArray1 The first CContactTextDef to be compared.
@param aTextDefArray2 The second CContactTextDef to be compared.
@return ETrue if the two items are equal, EFalse otherwise. */
	{// Need to check for NULL  arrays first.
	if((!&aTextDefArray1) && (!&aTextDefArray2))
		{
		return ETrue;
		}

	if((!&aTextDefArray1) || (!&aTextDefArray2))
		{
		return EFalse;
		}
	
	// Check if arrays are same length to begin with.
	TInt maxCount = aTextDefArray1.Count();
	if(!DoCompareTInt(maxCount, aTextDefArray2.Count()))
		{
		return EFalse;
		}

	for(TInt i=0; i<maxCount; ++i)
		{
		if(!DoCompareTUid(aTextDefArray1[i].iFieldType, aTextDefArray2[i].iFieldType))
			{
			return EFalse;
			}
			
		if(aTextDefArray1[i].iSeperator!=aTextDefArray2[i].iSeperator)
			{
			return EFalse;
			}		
		}

	return ETrue;
	}
	
TBool CPackagerCntComparator::DoCompareCCntFilterInclude(const CCntFilter& aFilter1, const CCntFilter& aFilter2) const
/** Compares two CCntFilter Include items. 

@param aFilter1 The first CCntFilter with its Include item to be compared.
@param aFilter2 The second CCntFilter  with its Include item to be compared.
@return ETrue if the two items are equal, EFalse otherwise. */
	{
	return ((const_cast<CCntFilter&>(aFilter1).IncludeModifiedContacts()==const_cast<CCntFilter&>(aFilter2).IncludeModifiedContacts())	&
	(const_cast<CCntFilter&>(aFilter1).IncludeNewContacts()				==const_cast<CCntFilter&>(aFilter2).IncludeNewContacts())		&
	(const_cast<CCntFilter&>(aFilter1).IncludeDeletedContacts()			==const_cast<CCntFilter&>(aFilter2).IncludeDeletedContacts()));
	}
	
TBool CPackagerCntComparator::DoCompareCCntFilterContactTypeL(const CCntFilter& aFilter1, const CCntFilter& aFilter2) const
/** Compares two CCntFilter ContactType items. 

@param aFilter1 The first CCntFilter with its ContactType item to be compared.
@param aFilter2 The second CCntFilter  with its ContactType item to be compared.
@return ETrue if the two items are equal, EFalse otherwise. */
	{
	return ((aFilter1.ContactFilterTypeALL()==aFilter2.ContactFilterTypeALL())		&
	(aFilter1.ContactFilterTypeCard()		==aFilter2.ContactFilterTypeCard())		&
	(aFilter1.ContactFilterTypeGroup()		==aFilter2.ContactFilterTypeGroup())	&			
	(aFilter1.ContactFilterTypeOwnCard()	==aFilter2.ContactFilterTypeOwnCard())	&
	(aFilter1.ContactFilterTypeTemplate()	==aFilter2.ContactFilterTypeTemplate()));
	}
	
TBool CPackagerCntComparator::DoCompareCContactIdArray(const CContactIdArray& anArray1, const CContactIdArray& anArray2) const
/** Compares two CContactIdArray items. 

@param anArray1 The first CContactIdArray to be compared.
@param anArray2 The second CContactIdArray to be compared.
@return ETrue if the two items are equal, EFalse otherwise. */
	{// Need to check for NULL  arrays first.
	if((!&anArray1) && (!&anArray2))
		{
		return ETrue;
		}

	if((!&anArray1) || (!&anArray2))
		{
		return EFalse;
		}
			
	// Check if arrays are same length to begin with.
	TInt maxCount = anArray1.Count();
	if(!DoCompareTInt(maxCount, anArray2.Count()))
		{
		return EFalse;
		}

	for(TInt i=0; i<maxCount; ++i)
		{
		if(!DoCompareTContactItemId(anArray1[i], anArray2[i]))
			{
			return EFalse;
			}
		}

	return ETrue;	
	}	
	
TBool CPackagerCntComparator::DoCompareCDesCArray(const CDesCArray& anArray1, const CDesCArray& anArray2) const
/** Compares two CDesCArray items. 

@param anArray1 The first CDesCArray to be compared.
@param anArray2 The second CDesCArray to be compared.
@return ETrue if the two items are equal, EFalse otherwise. */
	{// Need to check for NULL  arrays first.
	if((!&anArray1) && (!&anArray2))
		{
		return ETrue;
		}

	if((!&anArray1) || (!&anArray1))
		{
		return EFalse;
		}
	
	// Check if arrays are same length to begin with.
	TInt maxCount = anArray1.Count();
	if(!DoCompareTInt(maxCount, anArray2.Count()))
		{
		return EFalse;
		}

	for(TInt i=0; i<maxCount; ++i)
		{
		if(anArray1[i]!=anArray2[i])
			{
			return EFalse;
			}
		}

	return ETrue;	
	}	
	
TBool CPackagerCntComparator::DoCompareCContactItemType(const CContactItem& anItem1, const CContactItem& anItem2) const
/** Compares two CContactItem items' types. 

@param anItem1 The first CContactItem type to be compared.
@param anItem2 The second CContactItem type to be compared.
@return ETrue if the two items are equal, EFalse otherwise. */
	{
	TUid cntItem1Uid = anItem1.Type();
	if(!DoCompareTUid(cntItem1Uid, anItem2.Type()))
		{
		return EFalse;
		}

	TBool result = ETrue;
	switch(cntItem1Uid.iUid)
		{ // GroupsJoined() and ItemsContained() 
		  // do not hand over ownership, see cntitem.cpp.

		case KUidContactCardValue:
			result = Compare(*(static_cast<CContactCard&>((const_cast<CContactItem&>(anItem1))).GroupsJoined()), 
						*(static_cast<CContactCard&>((const_cast<CContactItem&>(anItem2))).GroupsJoined()));
			break;
			
		case KUidContactGroupValue:
			result = (Compare(*(static_cast<CContactGroup&>((const_cast<CContactItem&>(anItem1))).GroupsJoined()), 
						*(static_cast<CContactGroup&>((const_cast<CContactItem&>(anItem2))).GroupsJoined())) &											
					Compare(*(static_cast<CContactGroup&>((const_cast<CContactItem&>(anItem1))).ItemsContained()), 
						*(static_cast<CContactGroup&>((const_cast<CContactItem&>(anItem1))).ItemsContained())));
			break;
						
		case KUidContactOwnCardValue:
			result = Compare(*(static_cast<CContactOwnCard&>((const_cast<CContactItem&>(anItem1))).GroupsJoined()), 
						*(static_cast<CContactOwnCard&>((const_cast<CContactItem&>(anItem2))).GroupsJoined()));			
			break;
			
		case KUidContactICCEntryValue: 
			result = Compare(*(static_cast<CContactICCEntry&>((const_cast<CContactItem&>(anItem1))).GroupsJoined()), 
						*(static_cast<CContactICCEntry&>((const_cast<CContactItem&>(anItem2))).GroupsJoined()));			
			break;	
			
		default:
			break;		
		}

	return result;
	}

TBool CPackagerCntComparator::DoCompareCViewContactField(const CViewContact& anItem1, const CViewContact& anItem2) const
/** Compares two CViewContact items' field arrays. 

@param anItem1 The first CViewContact to be compared.
@param anItem2 The second CViewContact to be compared.
@return ETrue if the two items are equal, EFalse otherwise. */
	{// Need to check for NULL  arrays first.
	if((!&anItem1) && (!&anItem2))
		{
		return ETrue;
		}

	if((!&anItem1) || (!&anItem2))
		{
		return EFalse;
		}
		
	// Check if arrays are same length to begin with.
	TInt maxCount = anItem1.FieldCount();
	if(!DoCompareTInt(maxCount, anItem2.FieldCount()))
		{
		return EFalse;
		}

	for(TInt i=0; i<maxCount; ++i)
		{
		if(!DoCompareTPtrC(anItem1.Field(i), anItem2.Field(i)))
			{
			return EFalse;
			}
		}

	return ETrue;
	}
	
TBool CPackagerCntComparator::DoCompareCContactItemFieldAttribute(const CContactItemField& anItemField1, const CContactItemField& anItemField2) const
/** Compares two CContactItemField items' field attributes. 

@param anItemField1 The first CContactItemField to be compared.
@param anItemField2 The second CContactItemField to be compared.
@return ETrue if the two items are equal, EFalse otherwise. */
	{	
	return ((anItemField1.IsHidden()		==anItemField2.IsHidden())			&
		(anItemField1.IsReadOnly()			==anItemField2.IsReadOnly())		&
		(anItemField1.IsDisabled()			==anItemField2.IsDisabled())		&
		(anItemField1.IsTemplate()			==anItemField2.IsTemplate())		&
		(anItemField1.DoSynchronize()		==anItemField2.DoSynchronize())		&
		(anItemField1.IsDeleted()			==anItemField2.IsDeleted())			&
		(anItemField1.OverRidesLabel()		==anItemField2.OverRidesLabel())	&
		(anItemField1.HasExtraStream()		==anItemField2.HasExtraStream())	&
		(anItemField1.UsesTemplateTypes()	==anItemField2.UsesTemplateTypes())	&
		(anItemField1.LabelUnspecified()	==anItemField2.LabelUnspecified())	&
		(anItemField1.UserAddedField()		==anItemField2.UserAddedField())	&
		(anItemField1.IsReadOnly()			==anItemField2.IsReadOnly()));
	}
	
TBool CPackagerCntComparator::DoCompareCContactItemExtendedFieldAttribute(const CContactItemField& anItemField1, const CContactItemField& anItemField2) const
/** Compares two CContactItemField items' extended field attributes. 

@param anItemField1 The first CContactItemField to be compared.
@param anItemField2 The second CContactItemField to be compared.
@return ETrue if the two items are equal, EFalse otherwise. */
	{
	return ((anItemField1.IsPrivate()		==anItemField2.IsPrivate())		&
		(anItemField1.IsSpeedDial()			==anItemField2.IsSpeedDial())	&
		(anItemField1.IsCustomFilterable()	==anItemField2.IsCustomFilterable()));
	}	
	
TBool CPackagerCntComparator::DoCompareCContactItemAttribute(const CContactItem& anItem1, const CContactItem& anItem2) const
/** Compares two CContactItem items' attributes. 

@param anItem1 The first CContactItem to be compared.
@param anItem2 The second CContactItem to be compared.
@return ETrue if the two items are equal, EFalse otherwise. */
	{// GuidIsCompressed() private and not exported!
	return DoCompareTUint(anItem1.iAttributes, anItem2.iAttributes);
	}

