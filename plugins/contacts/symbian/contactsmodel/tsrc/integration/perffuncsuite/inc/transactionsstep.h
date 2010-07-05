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
#ifndef __TRANSACTION_STEP_H__
#define __TRANSACTION_STEP_H__
#include <testexecutestepbase.h>
#include "performancefunctionalitybase.h"

enum FieldFullness
	{
	EEmpty = 0,
	EMedium,
	EFull,
	};

class CPerformanceFunctionalityTestsSuite;

class CTransactionsStep : public CPerformanceFunctionalityBase
	{
public:
	CTransactionsStep(CPerformanceFunctionalityTestsSuite &aParent);
	virtual TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();

private:
	enum TCUDOperations
		{
		EOpen = 0,
		EUpdate,
		EAddDelete,
		};

private:
	void PreTestL();
	void EndTransactionL( const TBool aRevert );
	void NestedTransactionTestL();
	void EmptyCommitTransactionTestL( const TBool aRevert );
	void UpdateCommitTransactionTestL( const TBool aRevert );
	void CreateCommitTransactionTestL( const TBool aRevert );
	void DeleteCommitTransactionTestL( const TBool aRevert );
	void ManyUpdateCommitTransactionTestL( const TBool aRevert );
	void TransactionPerformanceL(	const TBool aDelete, 
									const FieldFullness aField,
									const TInt aContacts,
									TReal &aDiffseconds,
									TReal &aCumseconds);
	void EmptyTransactionPerformanceTestL(const TBool aDelete);
	void MediumTransactionPerformanceTestL(const TBool aDelete);
	void FullTransactionPerformanceTestL(const TBool aDelete);
	void ModifyContactsL(const FieldFullness aField);
	void MultipleCommitTestL(const TBool aTrans);
	void MultipleCommitL(const TBool aTrans);
	void FailCommitTransactionTestL();
	};

_LIT(KTransactionsStep,"TransactionsStep");

#endif
