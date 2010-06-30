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




#if (!defined __TESTUPDATEFIELDSSTEP_H__)
#define __TESTUPDATEFIELDSSTEP_H__
#include <testexecutestepbase.h>

_LIT(KTestUpdateFieldsStep,"TestUpdateFieldsStep");

class CTestUpdateFieldsStep : public CTestStep
	{
public:
	CTestUpdateFieldsStep();
	~CTestUpdateFieldsStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	CActiveScheduler* iScheduler;
	RFs iFsSession;
	void UpdateFieldsL(TPtrC aValue, TInt aStorageType,TUid aContFieldUid, TUid aFieldvCardUid);
	void UpdateFieldsNewTemplateL(TPtrC aValue, TInt aStorageType,TUid aContFieldUid, TUid aFieldvCardUid);
	static TInt iStorage[];
	static TUid iFieldUid[];
	static TUid iVcardUid[];
	};

#endif
