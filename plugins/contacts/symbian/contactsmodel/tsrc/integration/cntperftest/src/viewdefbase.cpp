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
#include "viewdefbase.h"


//Number of contacts
CViewDefBase::CViewDefBase(const TInt aContacts):CCntBaseStep(aContacts)
	{
	}

/**More items to be initilized within the uheap macro pair*/	
void CViewDefBase::InitializeL()
	{
	
	CCntBaseStep::InitializeL();
	
	iViewII = CContactItemViewDef::NewL(CContactItemViewDef::EIncludeFields,
	CContactItemViewDef::EIncludeHiddenFields);
	
	iViewIM = CContactItemViewDef::NewL(CContactItemViewDef::EIncludeFields,
	CContactItemViewDef::EMaskHiddenFields);
	
	iViewMI = CContactItemViewDef::NewL(CContactItemViewDef::EMaskFields,
	CContactItemViewDef::EIncludeHiddenFields);
	
	iViewMM = CContactItemViewDef::NewL(CContactItemViewDef::EMaskFields,
	CContactItemViewDef::EMaskHiddenFields);
	
	iDefView=CContactItemViewDef::NewL(CContactItemViewDef::EIncludeFields,
	CContactItemViewDef::EMaskHiddenFields);
	iDefView->AddL(KUidContactFieldMatchAll);
	
	}

/**More items to be cleaned up within the uheap macro pair*/		
void CViewDefBase::Cleanup()
	{
	CCntBaseStep::Cleanup();
	
	delete iViewII;
	iViewII = NULL;
	delete iViewIM;
	iViewIM = NULL;
	delete iViewMI;
	iViewMI = NULL;
	delete iViewMM;
	iViewMM = NULL;
	delete iDefView;
	iDefView = NULL;
	}

/**returns the number of nonhidden, hidden and empty fields,
that match aView for the current contact item*/	
void CViewDefBase::FieldsInView(const CContactItemViewDef &aView, TInt &aRetNormal, 
								TInt &aRetHidden, const TBool aHasEmpty)
	{
	TInt length = 0;
	length = iFields->Count();
	
	for(TInt i = 0; i < length; )
		{
		if( !( (*iFields)[i].Storage()->IsFull() ) )
			{
			if(aHasEmpty)
				{
				++i;
				continue;
				}
			else
				{
				_LIT(KUnfullView,"FieldsInView field is not full");
				User::Panic(KUnfullView, EInvariantFalse);
				}
			}
		if( ( i = FindInFieldSet(aView, i) ) != KErrNotFound )
			{
			if( (*iFields)[i].IsHidden() )
				{
				++aRetHidden;
				}
			else
				{
				++aRetNormal;
				}
			++i;
			}
		else
			{
			break;
			}
		}
	
	}

/**returns the position of the next field that matches aView for the current contact item,
returns KErrNotFound when it reaches end of fieldset or if it doesnt find a match*/		
TInt CViewDefBase::FindInFieldSet(const CContactItemViewDef &aView, const TInt &aFrom)
	{
	const TInt length = iFields->Count();
	const TInt viewLength = aView.Count();
	CContentType * ctype = NULL;
	TUid tuidback = KUidContactFieldNone;
	TBool allUids = EFalse;
	
	for(TInt i = aFrom; i < length; i++ )
		{
		ctype = CONST_CAST( CContentType *, &((*iFields)[i].ContentType()) );
		tuidback = ctype->Mapping();
		ctype->SetMapping(KUidContactFieldNone);//contain now should ignore mapping
		allUids = EFalse;
		
		for(TInt k = 0; k < viewLength; ++k)
			{	
			if( ctype->ContainsFieldType( aView[k] ) )
				{
				allUids = ETrue;
				break;
				}
			}
		ctype->SetMapping(tuidback);
		if(allUids)
			{
			return i;
			}
		}
	return KErrNotFound;
	}

/**adds a pair of existing uids, in every possible combination, to aView, 
aView is reset before the pair are added to aview.if aMany is greater than one, more than a pair
of uids are added but not in every possible combination*/		
void CViewDefBase::AddMultipleUidsL( CContactItemViewDef &aView, TInt &aPos, const TInt &aMany )
	{
	
	static TInt lastPlace = 0;
	const static TInt count = iExistingUidsArray->Count();	
	aView.Reset();
	aView.AddL( TUid::Uid( (*iExistingUidsArray)[aPos] ) );
	
	for(TInt i = 0; (lastPlace < count) && (i < aMany); ++lastPlace, ++i)
		{
		if(lastPlace == aPos || ((*iExistingUidsArray)[lastPlace] == 0) )
			{
			--i;
			continue;
			}
		aView.AddL( TUid::Uid( (*iExistingUidsArray)[lastPlace] ) );
		}
	if( lastPlace == count )
		{
		lastPlace = 0;
		++aPos;//this should never equal count
		_LIT(KNext,"next %d");
		INFO_PRINTF2(KNext, aPos);
		if( (aPos < count) && ((*iExistingUidsArray)[aPos] == 0) )
			{
			++aPos;
			}
		}
		
	}


