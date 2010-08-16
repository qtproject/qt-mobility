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

#ifndef __PERFORMANCEFUNCTIONALITY_BASE__
#define __PERFORMANCEFUNCTIONALITY_BASE__
#include <testexecutestepbase.h>
#include <cntdb.h>
#include <cntitem.h>
#include "rcntlist.h"
#include "performancefunctionalityserver.h"

class CPerformanceFunctionalityBase;


class TActiveRecoverObserver : public MContactUiCompactObserver
    {
//from MContactUiCompactObserver
public:
    TActiveRecoverObserver(CPerformanceFunctionalityBase *aTestStep);
	void Step(TInt );
	void HandleError(TInt aError);
public:
   	TInt		                    iCompressOrRecoverError;//active recover/compress error
   	CPerformanceFunctionalityBase*  iTestStep;
    };

class CPerformanceFunctionalityBase : public CTestStep
	{
public:
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();
	~CPerformanceFunctionalityBase();
protected:
	virtual void PreTestL() = 0;
	CPerformanceFunctionalityBase(const TInt aContacts) 
	        : iContacts(aContacts), iParent(NULL), iActiveRecoverObserver(this)
		{
		}
	CPerformanceFunctionalityBase(const TInt aContacts, CPerformanceFunctionalityTestsSuite &aParent) 
	        : iContacts(aContacts), iParent(&aParent), iActiveRecoverObserver(this)
		{
		}
	virtual void InitializeL();
	virtual void Cleanup();
	void AddEmptyContactsL(const TInt aNumber, const TBool aTrans);
	void DeleteContactL(const TContactItemId aCid, const TBool aTrans);
	TPtrC GetNextFieldL(const TDesC &aConfigSection, const TDesC &aName, const TBool aNewSection);
	TPtrC GetFieldL(const TDesC &aConfigSection, const TDesC &aName, const TInt aIndex);
	void OpenL( const TContactItemId aCid );
	void CommitL(const TBool aTrans);
	void ReadL( const TContactItemId aCid );
	void CloseL(const TBool aClose);
	void SetFieldL(const TInt aPos, const TDesC& aText);
	TBool CompareFieldL(const TInt aPos, const TDesC& aText);
	void SetAllFieldsL(const TDesC& aText);
	void SetManyFieldsL(const TDesC& aText, const TInt aMany);
	TBool CheckAllFieldsL(const TDesC& aText);
	void ExportVcardL(const TBool aDelete, const TDesC &aVCard);
	void ImportVcardL(const TInt aMany, RFileReadStream &aRfrs);
	TInt GetSharedIntL(const TDesC &aShare);
	void ShareIntL(const TDesC & aShare, const TInt aInt, const TBool aAppend=EFalse, const TDesC &aSeperator=KNullDesC);
	void SetSharedTextL(const TDesC &aShare, const TDesC &aText, const TBool aAppend);
	TInt SharedCountL(const TDesC &aShare);
	void MissingTestPanic() const;
	const TText* PrintNotification(const TInt aNotification) const;
	void PrintNotifications(const TDesC &aType, const RArray<TInt> &aEvents, 
					 		const RArray<TInt> &aContacts);
	void WaitForServerToExitL();
   	void PrintTestComparison(       	const TBool aResult, 
        								const TText *aCompare, 
        								const TInt aLine, 
        								const TText *aFile);
	void PrintTestComparisonIteration(	const TBool aResult,
										const TInt aIteration, 
										const TText *aCompare, 
										const TInt aLine, 
										const TText *aFile);

	TBool StringComparisonTestPrint(	const TDesC &aCompareL,
										const TText *aCompareOperator,  
										const TDesC	&aCompareR, 
										const TBool aResult,
										const TInt 	aIteration,  
										const TInt 	aSubIteration, 
										const TInt 	aLine, 
										const TText *aFile);
									
	TBool IntegerComparisonTestPrint(	const TInt	aCompareL,
										const TText *aCompareOperator,  
										const TInt 	aCompareR, 
										const TBool aResult,
										const TInt 	aIteration, 
										const TInt 	aSubIteration,  
										const TInt 	aLine, 
										const TText *aFile);
//utitilty functions
protected:
	void StartCompressL();
	void EndCompressL();	
	void StartRecoverL();
	void EndRecoverL();			

private:						
    void CloseProcessL(const TDesC& aProcessName, const TInt aIteration);
    										
protected:
    // number of contacts created / used by this test
	TInt                                    iContacts; 
	CContactDatabase*                       iContactsDatabase;
	CContactItemViewDef*                    iViewAll;
	CContactItem*                           iContactItem;
	CContactItemFieldSet*                   iFields;
	RCntList*                               iIterate;
	CPerformanceFunctionalityTestsSuite*    iParent;
	CContactTemplate*                       iTemplate;
	TBool                                   iConcurrent;
	TBool                                   iUnclean;
	//complete active compress/recover, as opposed to cancelling
	TBool 	                            	iCompleteSteps;
	TActiveRecoverObserver                  iActiveRecoverObserver;

private:
	TInt                                    iNextFieldPosition;
	CContactActiveRecover*	                iRecover;
	CContactActiveCompress*                 iCompress;

	};

#endif
