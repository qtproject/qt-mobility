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


/**
 @file
 @internalComponent
 @released
*/


#include "cviewiterator.h"


/**
Constructs CViewIterator object and initialises all its members.

@param aManager MLplViewIteratorManager interface implemented by the Persistence
Layer.
@param aTextDef CContactTextDef object describing the set of fields which will
be added to each CViewContact object during iteration.
@param aViewPreferences Iteration preferences.
*/
CViewIterator::CViewIterator(MLplViewIteratorManager& aManager,const CContactTextDef& aTextDef,TContactViewPreferences aViewPreferences)
	:
	iManager(aManager),
	iViewPreferences(aViewPreferences),
	iTextDef(aTextDef)
	{
	iTableMask = TableMask(aTextDef);	
	}


/**
CViewIterator destructor.
*/
CViewIterator::~CViewIterator()
	{
	}


/**
Move the view iterator to the first view contact.
*/
void CViewIterator::GoFirstL()
	{
	iCurrentItemId = -1;
	}


/**
Move the view iterator to the next view contact and return the view contact
at the new position.

@return View contact at the next position in the view.  The return value will be
NULL if no view contact can be obtained.
*/
CViewContact* CViewIterator::NextItemL()
	{
	// Parcel up parameters in 'T' class.
	TItemAtLParams params;
	params.iGreaterEqual = ETrue;
	params.iIndex = ++iCurrentItemId;
	params.iMask = iTableMask;
	params.iViewPrefs = iViewPreferences;
	
	// Could be calling proxy if used on client side.
	CViewContact *viewContact = iManager.ItemAtL(params,iTextDef, KIgnoreSessionId);

	if (viewContact)
		{
		iCurrentItemId = viewContact->Id();
		}

	return viewContact;
	}


/** 
Get the view contact with the given Contact ID.

@return View contact with the given Contact ID.
*/
CViewContact* CViewIterator::ItemAtL(TInt aCntItemId)
	{
	iCurrentItemId = aCntItemId;
	
	// Parcel up parameters in 'T' class.
	TItemAtLParams params;
	params.iGreaterEqual = EFalse;
	params.iIndex = iCurrentItemId;
	params.iMask = iTableMask;
	params.iViewPrefs = iViewPreferences;

	return iManager.ItemAtL(params, iTextDef, KIgnoreSessionId);
	}


/**
Get a table mask using the given text definition.

@param aTextDef Text definition from which to create the table mask.

@return MLplViewIteratorManager::EIdentityTableOnly if the text definition only
requires access to the Identity table, MLplViewIteratorManager::EAllTables
otherwise.
*/
MLplViewIteratorManager::TTableMask CViewIterator::TableMask(const CContactTextDef& aTextDef)
	{
	TInt columns=0;
	TInt sortDefFlags=0;
	// Find out whether we can do a fast sort using the Identity table by
	// looking at the text definition.
	CViewIterator::ConstructBitwiseFlagsFromTextDef(sortDefFlags,columns,&aTextDef);
	if( UsesIdentityTableOnly(sortDefFlags) )
		{
		return MLplViewIteratorManager::EIdentityTableOnly;
		}
	else
		{
		return MLplViewIteratorManager::EAllTables;
		}
	}


/**
Update a set of find flags and an Identity table column count from the field
type UIDs in the given field definition.

@param aFindFlags Updated with those flags which map to the field type UIDs in
aFieldDef.
@param aIdentityColumnsCount Updated with the number of field type UIDs in
aFieldDef which map to columns in the Identity table.
@param aFieldDef Field definition from which to create set of find flags and
Identity table column count.
*/
void CViewIterator::ConstructBitwiseFindFlags(TInt& aFindFlags,TInt& aIdentityColumnsCount,const CContactItemFieldDef* aFieldDef)
	{
	if(aFieldDef!=NULL && aFieldDef->Count()>0)
		{
		for(TInt ii=0;ii<aFieldDef->Count();ii++)
			{
			SetFindFlagsAndColumnsCount(aFieldDef->At(ii).iUid,aFindFlags,aIdentityColumnsCount);
			}
		}
	else
		{
		aFindFlags|=EFindInAllFields|EFindInAnyIdentityField;
		}
	}


/**
Update a set of find flags and an Identity table column count from the field
type UIDs in the given text definition.  This can be used to tell the find
method what tables need to be searched for a given text definition.  If the text
definition is NULL we search in all tables.

@param aFindFlags Updated with those flags which map to the field type UIDs in
aTextDef.
@param aIdentityColumnsCount Updated with the number of field type UIDs in
aTextDef which map to columns in the Identity table.
@param aTextDef Text definition from which to create set of find flags and
Identity table column count.
*/
void CViewIterator::ConstructBitwiseFlagsFromTextDef(TInt& aFindFlags,TInt& aIdentityColumnsCount,const CContactTextDef* aTextDef)
	{
	if(aTextDef!=NULL && aTextDef->Count()>0)
		{
		for(TInt ii=0;ii<aTextDef->Count();ii++)
			{
			SetFindFlagsAndColumnsCount(aTextDef->At(ii).iFieldType.iUid,aFindFlags,aIdentityColumnsCount);
			}
		TFieldType fallback = aTextDef->FallbackField();
		if (fallback!=KUidContactFieldNone)
			{
			SetFindFlagsAndColumnsCount(fallback.iUid,aFindFlags,aIdentityColumnsCount);
			}
		}
	else
		{
		aFindFlags|=EFindInAllFields|EFindInAnyIdentityField;
		}
	}


/**
For the given field type UID, increase aIdentityColumnsCount if the field maps
to one of the columns in the Identity table and add the relevant find flag.

@param aUid Field type UID.
@param aFindFlags Updated with find flag which maps to the given field type UID.
@param aIdentityColumnsCount Incremented if the field type UID maps to a column
in the Identity table.
*/
void CViewIterator::SetFindFlagsAndColumnsCount(TInt32 aUid,TInt& aFindFlags,TInt& aIdentityColumnsCount)
	{
	switch(aUid)
		{
		case KUidContactFieldGivenNameValue:
			{
			aIdentityColumnsCount++;
			aFindFlags|=EFindFirstName;
			break;
			}
		case KUidContactFieldFamilyNameValue:
			{
			aIdentityColumnsCount++;
			aFindFlags|=EFindLastName;
			break;
			}
		case KUidContactFieldCompanyNameValue:
			{
			aIdentityColumnsCount++;
			aFindFlags|=EFindCompanyName;
			break;
			}
		case KUidContactFieldGivenNamePronunciationValue:
			{
			aIdentityColumnsCount++;
			aFindFlags|=EFindFirstNamePronunciation;
			break;
			}
		case KUidContactFieldFamilyNamePronunciationValue:
			{
			aIdentityColumnsCount++;
			aFindFlags|=EFindLastNamePronunciation;
			break;
			}
		case KUidContactFieldCompanyNamePronunciationValue:
			{
			aIdentityColumnsCount++;
			aFindFlags|=EFindCompanyNamePronunciation;
			break;
			}
		case KUidContactFieldEMailValue:
			{
			aFindFlags|=EFindInEmailTableOnly;
			break;
			}
		case KUidContactFieldMatchAllValue:
			{	
			aFindFlags|=EFindInAllFields|EFindInAnyIdentityField;
			break;
			}
		default:
			aFindFlags|=EFindInAllFields;
		}
	}


/**
Determine if the Identity table requires to be searched for the given find
flags.

@param aFindFlags Set of find flags describing which fields will be searched.

@return ETrue If one of the flags in aFindFlags maps to a column in the Identity
table, EFalse otherwise.
*/
TBool CViewIterator::SearchIdentityTableRequired(TInt aFindFlags)
	{
	return aFindFlags & 
		( EFindInAnyIdentityField | EFindFirstName | EFindLastName | EFindCompanyName |
			EFindFirstNamePronunciation | EFindLastNamePronunciation | EFindCompanyNamePronunciation);
	}


/**
Determine if only the Identity table requires to be searched for the given find
flags.

@param aFindFlags Set of find flags describing which fields will be searched.

@return ETrue If one of the flags in aFindFlags maps to a column in the Identity
table and no columns in any other table, EFalse otherwise.
*/
TBool CViewIterator::UsesIdentityTableOnly(TInt aFindFlags)
	{
	return (aFindFlags &  (EFindFirstName | EFindLastName | EFindCompanyName |
			EFindFirstNamePronunciation |EFindLastNamePronunciation |EFindCompanyNamePronunciation) ) &&
		! (aFindFlags & (EFindInAllFields | EFindInEmailTableOnly) );
	}
