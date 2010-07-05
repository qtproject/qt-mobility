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

#ifndef __CONTACTVIEW_TESTS_H__
#define __CONTACTVIEW_TESTS_H__

#include <cntviewbase.h>
#include "cntbasestep.h"
#include <coreappstest/testserver.h>
#include "persistencelayer.h"

enum FieldFullness
	{
	EBlank = 0,
	EMedium,
	EFull,
	};
	
enum PerformanceContacts
	{
	EFew = 10,
	EMid = 100,
	EMany = 500,
	EMax = 1000,
	EDone = -1,
	};

class CViewObserver;

class CContactViewTest : public CCntBaseStep, public MContactUiCompactObserver
	{
friend class CViewObserver;
public:
	CContactViewTest(CCntPerfServer &aParent);
	virtual TVerdict doTestStepL();
	virtual TVerdict CContactViewTest::doTestStepPostambleL();
	virtual void PreTestL();

//testing methods
private:
	void doAllEmptyTestL(const CContactViewBase &);
	void AllEmptyTestL();
	void ViewEmptyTestL();
	void doViewEmptyTestL(const CContactViewBase &aLView);
	void ContactEmptyTestL();
	void doContactEmptyTestL(const CContactViewBase &aLView);
	void AllFullTestL();
	void doAllFullTestL(const CContactViewBase &aLView);
	void EmptyFieldsetTestL();
	void doEmptyFieldsetTestL(const CContactViewBase &aLView);
	void EmptyFieldsetSortTestL();
	void doEmptyFieldsetSortTestL(const CContactViewBase &aLView);
	void MissingFieldTestL();
	void doMissingFieldTestL(const CContactViewBase &aLView);
	void PerformanceTestL();
	void doPerformanceTestL(const CContactViewBase &aLView);
	void RecoverCompressTestL();
	void doRecoverCompressTestL(const CContactViewBase &aLView);
	void ConcurrentRecoverCompressTestL();
	void doConcurrentRecoverCompressTestL(const CContactViewBase &aLView);
	void RecoverAndCompressThreadTestL();

//utility functions
private:
	void CleanupL();
	void InitializeL();
	void ResetDatabaseL(const TInt aMany=0);
	TBool CheckViewContact(const CViewContact &aVContact, const TDesC &aVal);
	TBool CheckViewContactL(const CViewContact &aVContact);
	HBufC* FieldStringLC(const CContactItemField &aField);
	void ResetSortL();
	void SetContactL();
	TBool FindAndClearFieldL(const TContactItemId aCid, const TDesC &aField);
	TPtrC GetNextFieldL();
	CContactViewBase* CreateViewL();
	TPtrC GetFieldL(const TInt aIndex);
	void ClosePersistanceL(const TBool aReOpen);
	void OpenPersistanceL();
	void SetMediumL();
	void SetFullL();
	void StartRecoverL();
	void EndRecoverL();
	void StartCompressL();
	void EndCompressL();
	static TInt ThreadRunL(TAny* aAny);

//from MContactUiCompactObserver
private:
	void Step(TInt ){}
	void HandleError(TInt aError);
	
private:
	CViewObserver* 			iObserver;
	RContactViewSortOrder* 	iSortOrder;
	TBool 					iRemote;
	TTime 					iStartP;
	TTime 					iFinishP;
	TTime 					iStartC;
	FieldFullness 			iFullness;
	PerformanceContacts		iPerformanceContacts;
	TBool					iPLView;//run local view directly on top of the persistance layer, as used by view sub sessions within the server
	CPersistenceLayer* 		iPersistence;
	RPIMTestServer* 		iPimServ;
	CContactActiveRecover* 	iRecover;
	CContactActiveCompress* iCompress;
	CContactDatabase* 		iContactsDatabase2;
	TInt					iError;
	TInt					iIteration;
	TBool 					iRecoverFlag;
	RThread					iThread;
	RSemaphore				iSemaphore;
	RSemaphore				iSemaphore2;
	TInt                    iDamageCounter;
	};

#endif

