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


#ifndef CCNTPERMANENTDATA_H_
#define CCNTPERMANENTDATA_H_

#include <cntviewbase.h>


class CCntDbManagerController;
class CCntDbManager;
class CContactDefaultViewDefinition;


/**
This class manages permanent data created during Server start-up.
*/
class CCntPermanentData : public CActive, public MContactViewObserver
	{
private:
	enum TState
		{
		EStateInitial = 0,
		EStateOpeningDb,
		EStateOpeningView,
		EStateFinished
		};
	
public:
	~CCntPermanentData();
	
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	void CompleteSelf(TInt aReason);
	
	static CCntPermanentData* NewL(CCntDbManagerController& aController);
	
	void StartDataCreationL();
	TBool IsOnlyClientOfDbManager(CCntDbManager* manager) const;
	void ReleaseDbManagerL(CCntDbManager* manager);
	
	void HandleContactViewEvent(const CContactViewBase &aView, const TContactViewEvent &aEvent);

private:
	CCntPermanentData(CCntDbManagerController& aController);
	void ConstructL();
	void Close();
	
	TBool OpenNextViewL();
	CContactViewBase* OpenViewFromDefinitionL(const CContactDefaultViewDefinition& aViewDef);
	
private:
	TState iState;
	CCntDbManagerController& iController;
	CCntDbManager * iManager;
	RPointerArray<CContactDefaultViewDefinition> iViewDefs;
	RPointerArray<CContactViewBase> iViews; // This does not own the CContactViewBase, RPointerArray is
	                                        // only used since RArray<CContactViewBase&> does not compile.
	};


#endif // CCNTPERMANENTDATA_H_
