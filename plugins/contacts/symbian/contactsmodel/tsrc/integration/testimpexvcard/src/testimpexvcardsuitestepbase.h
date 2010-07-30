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




#if (!defined __TESTIMPEXVCARD_STEP_BASE__)
#define __TESTIMPEXVCARD_STEP_BASE__
#include <testexecutestepbase.h>
#include "testimpexvcardsuitedefs.h"

/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
class CTestImpExvCardSuiteStepBase : public CTestStep
	{
public:
	virtual ~CTestImpExvCardSuiteStepBase();
	CTestImpExvCardSuiteStepBase();
	void OpenVCFAndImportItemL(TBuf<80> aPath, RFs aFsSession, TInt aIndex);
	void ImportCardL(RReadStream& aReadStream);
	void OpenBDAYVCFAndImportItemL(TBuf<80> aPath, TInt aIndex);
	void ImportBDAYCardL(RReadStream& aReadStream);
	void ExportItemL(TBuf<80> aPath, TBool aAddBDay);
	void ImportItemL(TBuf<80> aPath, TBool aImportBDay);
	void OpenDBL();
	void CloseDB();
	void SetNameL(CContactItem& aItem,TUid aType,const TDesC& aName, TBool aAddField);
	TTime FormatDateTime(const TDesC& aParamString);
	TPtrC GetExpectedUTCFromIniL(TInt numberOfCases, TBuf<80> aConfig, TBool aExpectedBDay);

public:
	TTime iTimeFromImport;
	TTime iBDayFromImport;
	TTime iRecordedTime;
	TTime iBDayLocal;
	CContactDatabase* iDb;
	
protected:
	};

#endif
