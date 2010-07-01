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




/**
 @file
 @internalComponent
 @released
*/


#ifndef __CVIEWCONTACTMANAGER_H__
#define __CVIEWCONTACTMANAGER_H__

#include "persistencelayer.h"
#include <cntviewbase.h>

class MContactViewSortObserver
    {
    public:
    enum TIccViewNotify
        {
        TIccViewNotify_IccOnlyLocked,
        TIccViewNotify_IccUnlocked          
        };
        
    public:
    	virtual TBool IdleSorterNotifyL(TInt aSortErr) =0;
    	virtual void  IccViewNotifyL(TInt aIccViewReady) =0;
    };


/**
The CViewContactManager is an internal class responsible to sort contact view items, 
according to sort plug-in, and provide CViewContact items for CContactLocalView.
And it's the only view class interact with persistence layer(CPplContactViewItemManager)
to retrieve contact view item from database.
@internalComponent
*/
NONSHARABLE_CLASS(CViewContactManager) : public CBase, public MContactViewSortObserver
	{
public:
    static CViewContactManager* NewL(CContactLocalView& aLocalView, MLplPersistenceLayerFactory& aFactory, const CContactTextDef& aTextDef, TContactViewPreferences aViewPerference, CViewContactSortPlugin* aSortPlugin);
	~CViewContactManager();
	
	TContactItemId AtL(TInt aIndex) const;
	TInt Count() const;
	TInt FindL(TContactItemId aContactId) const;
	void SortL();
	void SortL(const CContactTextDef& aTextDef);
	void StopSortL();
    TInt InsertL(const TContactItemId aContactId, TContactViewPreferences& aViewPreference);
	TInt RemoveL(const TContactItemId aContactId);
	HBufC* AllFieldsLC(TInt aIndex, const TDesC& aSeparator) const;
	
	const CViewContact& ContactAtL(TInt aIndex) const;

	TBool IsICCSynchronised() const;
	
    //Implementation for MContactSortObserver
	TBool IdleSorterNotifyL(TInt aSortErr);
    void  IccViewNotifyL(TInt aIccViewReady);
	
private:
	/* To represent the state when retrieving data from database */
	enum TReadState 
	    {
	    EReadFullChunk,    // The read items reached the number of limitation per invocation
	    EReadFullForMerge, // The read items reached the number of limitation per merge
	    EReadCompleted     // Read is completed, no more view contact item in database.
	    };
	    
private: 
	CViewContactManager(CContactLocalView& aLocalView, MLplViewIteratorManager& aLplViewMgr, TContactViewPreferences aViewPerference, CViewContactSortPlugin* aSortPlugin);
	void ConstructL(MLplPersistenceLayerFactory& aFactory, const CContactTextDef& aTextDef);
	
    TInt InsertViewContactL(const CViewContact* aNewContact, TBool aSortByIdOnly, TInt aStart);
	void HeapSortL(RPointerArray<CViewContact>& aViewContacts);
	RPointerArray<CViewContact>* MergeL(RPointerArray<CViewContact>& aLeftContacts, RPointerArray<CViewContact>& aRightContacts);
	
	TReadState ReadInViewContactsL(RPointerArray<CViewContact>& aViewContacts);
	CViewContact& ViewContactAtL(TInt aIndex) const;
	
    void ResetSort();
    
	static TBool ContactCorrectType(TUid aType, TContactViewPreferences aTypeToInclude);
	static void  ResetAndDestroyRPointerArrayPointer(TAny *aArray);
	
	//These functions are copied from CContactViewBase classes
	TBool  IsContactSortable(const CViewContact& aContact, TContactViewPreferences& aViewPreferences) const;
	TInt   CompareContactsAndIdsL(const CViewContact& aFirst, const CViewContact& aSecond) const;
	TInt   TextCompareFieldsL(const CViewContact& aFirst, const CViewContact& aSecond) const;
	HBufC* FieldsWithSeparatorLC(const CViewContact& aViewContact, const TDesC& aSeparator) const;
	static TInt CompareFieldsWithCollationLevel(const CViewContact& aFirst, const CViewContact& aSecond, TInt aCollationLevel, const TCollationMethod* aCollateMethod);
    	
private:
	CContactLocalView& 		        iLocalView;
	
	CIdleContactSorter*				iIdleSorter;
	
	TInt                            iViewSessionId;
	
	RPointerArray<CViewContact>*	iViewContacts;
	RPointerArray<CViewContact>*    iUnsortedViewContacts;
	RPointerArray<CViewContact>*    iViewContactsBuffer;

	CViewContactSortPlugin*			iSortPluginImpl;  //this is not owned by the class
	
    TContactViewPreferences         iViewPreferences;
    TCollationMethod				iCollationMethod;	
	
	MLplViewIteratorManager& 		iLplViewItemMgr;
	};

#endif // __CVIEWCONTACTMANAGER_H__
