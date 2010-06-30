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


#include <cntview.h>	//mcontactviewobserver
#include "t_utils2.h"	//crandomcontactgenerator


class CFilterFindContactsTestConductor : public CBase
	{
	public:
		static CFilterFindContactsTestConductor* NewL();
		~CFilterFindContactsTestConductor();
		void RunTestsL();
		
	private:
		CFilterFindContactsTestConductor();
		void ConstructL();
		void AddSomeContactsL(TInt aCount);
	
	private:
		CContactDatabase* iDb;
		CRandomContactGenerator* iRandomGenerator;
	};


class CFilterFindContactsTester : public CActive, public MContactViewObserver
	{
	public:
		static CFilterFindContactsTester* NewL(CContactDatabase& iDb);
		~CFilterFindContactsTester();
	private:
		CFilterFindContactsTester(CContactDatabase& iDb);
		void ConstructL();
		void NextTest();
		void DisplayL();
		void AddEntryL();
		
	private: // From MContactViewObserver.
		virtual void HandleContactViewEvent(const CContactViewBase& aView,const TContactViewEvent& aEvent);

	private:// From CActive
		virtual void RunL();
		TInt RunError(TInt aError);
		virtual void DoCancel();

		
		enum TTestNumber
			{
			EInit,
			ECreateLocalView,
			ECheckAddContactEntry,
			ECreateFindView,
			EFindAddContacts,
			EModifyUnsortableContact,
			EFindRemoveContacts,
			ECheckCorrectSeparators,
			ETestsComplete
			};
		
		
		TInt iCurrentTest;
		TBool iAddedFindContacts;
		TContactItemId iFindAdd1;
		TContactItemId iFindAdd2;
		CContactDatabase& iDb;
		TContactItemId iContactId;
		RContactViewSortOrder iSortOrder;
		CContactLocalView* iLocalView;
		CContactFindView* iFindView;
		CPtrCArray* iFindDesArray;

	};

