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




#if (!defined __TESTADDFIELDSSTEP_H__)
#define __TESTADDFIELDSSTEP_H__
#include <testexecutestepbase.h>

class CTestAddFieldsStep : public CTestStep
	{
public:
	CTestAddFieldsStep();
	~CTestAddFieldsStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	CActiveScheduler* iScheduler;
	RFs iFsSession;
	void AddFieldsL(TPtrC aValue, TInt aStorageType,TUid aContFieldUid, TUid aFieldvCardUid);
	void AddCoupleFieldsL(TPtrC aValue, TInt aStorageType,TUid aContFieldUid, TUid aFieldvCardUid);
	void AddAllFieldsL(void);
	void AddFieldsNewTemplateL(TPtrC aValue, TInt aStorageType,TUid aContFieldUid, TUid aFieldvCardUid);
	static TInt iStorage[];
	static TUid iFieldUid[];
	static TUid iVcardUid[];
	};

_LIT(KTestAddFieldsStep,"TestAddFieldsStep");

#endif
