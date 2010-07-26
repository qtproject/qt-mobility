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


#include <cntfilt.h>
#include "cntstd.h"


EXPORT_C CCntFilter* CCntFilter::NewL()
/** Allocates and constructs a default contact database filter.

@return Pointer to the new filter. */
	{
	return new(ELeave) CCntFilter;
	};

/** Allocates and constructs a default contact database filter. 

@return Pointer to the new filter. This is left on 
the cleanup stack. 
*/
EXPORT_C CCntFilter* CCntFilter::NewLC()
	{
	CCntFilter* newFilter=CCntFilter::NewL();
	CleanupStack::PushL(newFilter);
	return newFilter;
	};


EXPORT_C CCntFilter* CCntFilter::NewL(const CCntFilter* aFilter)
/** Allocates and constructs a contact database filter based on another one. 

The state value, type flags, the date/time value and the ID array are copied.

@param aFilter Pointer to the filter to copy.
@return Pointer to the new filter. */
	{
	CCntFilter* newFilter=CCntFilter::NewLC(aFilter);
	CleanupStack::Pop(); // newFilter
	return newFilter;
	};


EXPORT_C CCntFilter* CCntFilter::NewLC(const CCntFilter* aFilter)
/** Allocates and constructs a contact database filter based on another one. 

The state value, type flags, the date/time value and the ID array are copied.

@param aFilter Pointer to the filter to copy.
@return Pointer to the new filter. This is left on the cleanup stack. */
	{
	CCntFilter* newFilter=new(ELeave) CCntFilter(aFilter);
	CleanupStack::PushL(newFilter);
	return newFilter;
	};

/** Allocates and constructs a contact database filter based on a RReadStream. 

The state value, type flags, the date/time value and the ID array are copied.

@param aStream RReadStream containing object to internalize.
@return Pointer to the new filter. This is left on the cleanup stack. 
@internalTechnology 
*/	
CCntFilter* CCntFilter::NewLC(RReadStream& aStream)
	{
	CCntFilter* newFilter=CCntFilter::NewLC();
	newFilter->InternalizeL(aStream);	
	return newFilter;
	}		

EXPORT_C void CCntFilter::Reset()
/** Resets the filter. The state value is set to EIncludeAllContacts and the contact 
ID array is reset. */
	{
	iInclude=EIncludeAllContacts; 
	if (iIds)
		iIds->Reset();
	};

EXPORT_C void CCntFilter::SetIncludeModifiedContacts(TBool aIncludeModified)
/** Sets the filter to include only contacts modified since the filter's date/time.

@param aIncludeModified ETrue to include only contacts modified since the 
filter's date/time, EFalse to include contact items in all states. */
	{
	if (aIncludeModified)
		iInclude = EIncludeModifiedContacts;
	else
		iInclude = EIncludeAllContacts;
	}; 

EXPORT_C void CCntFilter::SetIncludeNewContacts(TBool aIncludeNew)
/** Sets the filter to include only contacts created since the filter's date/time.

@param aIncludeNew ETrue to include only contacts created since the filter's 
date/time, EFalse to include contact items in all states. */
	{
	if (aIncludeNew)
		iInclude = EIncludeNewContacts;
	else
		iInclude = EIncludeAllContacts;
	}; 

EXPORT_C void CCntFilter::SetIncludeDeletedContacts(TBool aIncludeDeleted)
/** Sets the filter to include only contacts deleted since the filter's date/time.

@param aIncludeDeleted ETrue to include only contacts deleted since the filter's 
date/time, EFalse to include contact items in all states. */
	{
	if (aIncludeDeleted)
		iInclude = EIncludeDeletedContacts;
	else
		iInclude = EIncludeAllContacts;
	}; 

EXPORT_C TBool CCntFilter::TestContactFilterType(TUid aTypeUid)
/** Tests whether a contact item type, identified by its UID is included in or 
excluded from the filter.

@param aTypeUid UID which identifies a contact item type. One of: KUidContactCard, 
KUidContactGroup, KUidContactCardTemplate or KUidContactOwnCard.
@return ETrue if the specified contact type is included in the filter, EFalse 
not. */
	{	
	if (ContactFilterTypeALL())
		return ETrue;	// all types are set to be search
	else if (aTypeUid == KUidContactCard && ContactFilterTypeCard())
		return ETrue;
	else if (aTypeUid == KUidContactGroup && ContactFilterTypeGroup())
		return ETrue;
	else if (aTypeUid == KUidContactCardTemplate && ContactFilterTypeTemplate())
		return ETrue;
	else if (aTypeUid == KUidContactOwnCard && ContactFilterTypeOwnCard())
		return ETrue;
	return EFalse;
	}

EXPORT_C void CCntFilter::SetContactFilterTypeALL(TBool aFilterAllContacts)
/** Sets whether all contact item types (contact card groups, contact cards, own 
cards and contact card templates) should be included in the filter.

@param aFilterAllContacts ETrue if all contact item types should be included 
in the filter, EFalse if not. */
	{
	if (aFilterAllContacts)
		iContactType |= EIncludeAllTypes;
	else
		iContactType &= ~EIncludeAllTypes ;
	};


EXPORT_C void CCntFilter::SetContactFilterTypeCard(TBool aFileterCards)
/** Sets whether contact cards should be included in or excluded from the filter.

@param aFileterCards ETrue to include contact cards in the filter, EFalse to exclude them. */		
	{
	if (aFileterCards)
		iContactType |= EContactCards;
	else
		iContactType &= ~EContactCards;
	};

EXPORT_C void CCntFilter::SetContactFilterTypeGroup(TBool aFilterGroups)
/** Sets whether contact card groups should be included in or excluded from the 
filter.

@param aFilterGroups ETrue to include contact card groups in the filter, EFalse 
to exclude them. */
	{
	if (aFilterGroups)
		iContactType |= EContactGroups;
	else
		iContactType &= ~EContactGroups;

	};

EXPORT_C void CCntFilter::SetContactFilterTypeTemplate(TBool aFilterTemplates)
/** Sets whether contact card templates should be included in or excluded from 
the filter.

@param aFilterTemplates ETrue to include contact card templates in the filter, 
EFalse to exclude them. */
	{
	if (aFilterTemplates)
		iContactType |= EContactTemplates;
	else
		iContactType &= ~EContactTemplates;
	};

EXPORT_C void CCntFilter::SetContactFilterTypeOwnCard(TBool aFilterOwnCard)
/** Sets whether own cards should be included in or excluded from the filter.

@param aFilterOwnCard ETrue to include own cards in the filter, EFalse to 
exclude them. */
	{
	if (aFilterOwnCard)
		iContactType |= EContactOwnCard;
	else
		iContactType &= ~EContactOwnCard;

	};	


EXPORT_C CCntFilter::~CCntFilter()
/** Deletes all resources owned by the filter prior to its destruction. */
	{
	delete iIds;
	};

//	TVersion Version() const;
//	virtual TBool IsValid(const Whatever& aWhatever) const; void SetSinceDate(const TTime&);

CCntFilter::CCntFilter()
	{
	};

CCntFilter::CCntFilter(const CCntFilter* aFilter)
:iIds(aFilter->iIds), iSinceDateTime(aFilter->iSinceDateTime),iInclude(aFilter->iInclude),
	iContactType(aFilter->iContactType)
	{
	};
	
void CCntFilter::InternalizeL(RReadStream& aStream)
/** Internalises a CCntFilter object from a read stream. 
@param aStream Stream from which the object should be internalised. */
	{
	// CContactIdArray* iIds;
	if (iIds)
		{
		delete iIds;		
		iIds = NULL;
		}
		
	//TBool CheckIfExists=aStream.ReadInt32L();
	if (aStream.ReadInt32L())
		{
		iIds = CContactIdArray::NewL();		
		iIds->InternalizeL(aStream);
		}			
			
	// TTime iSinceDateTime;
	TInt64 tempInt64;
	aStream >> tempInt64;
	iSinceDateTime = TTime(tempInt64);	
	
	// TInclude	iInclude;
	iInclude = static_cast<TInclude>(aStream.ReadInt32L());

	// TInt32 iContactType;
	iContactType = aStream.ReadInt32L();
	
	// TAny* iReserved1;
	// TAny* iReserved2;	
	}

void CCntFilter::ExternalizeL(RWriteStream& aStream) const 
/** Externalises a CCntFilter object to a write stream.
@param aStream Stream to which the object should be externalised. */
	{
	// CContactIdArray* iIds;
	if(iIds)
		{
		aStream.WriteInt32L(1);
		iIds->ExternalizeL(aStream);
		}
	else
		{
		aStream.WriteInt32L(0); // Empty array
		}
			
	// TTime iSinceDateTime;	
	aStream << iSinceDateTime.Int64();
	
	// TInclude	iInclude;
	aStream.WriteInt32L(iInclude);
		
	// TInt32 iContactType;
	aStream.WriteInt32L(iContactType);	
	
	// TAny* iReserved1;
	// TAny* iReserved2;
		
	}			

/*
protected:
	TTime            iSinceDate;
	TBool            iIncludeChangedContacts; 
	TBool            iIncludeDeletedContacts; 
	TBool            iIncludeInsertedContacts; 
	TAny*	         iReserved1;
	TAny*			 iReserved2;
*/


