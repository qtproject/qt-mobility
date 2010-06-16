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




#if (!defined __TESTIMPORTCONTACTMERGEMODE_H__)
#define __TESTIMPORTCONTACTMERGEMODE_H__
#include <testexecutestepbase.h>
#include "testimportcontactmergemodestep.h"
#include "testimpexvcardsuitedefs.h"

class CTestImportContactMergeModeStep : public CTestStep
	{
public:
	CTestImportContactMergeModeStep();
	~CTestImportContactMergeModeStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	CActiveScheduler* iScheduler;
	RFs iFsSession;
	void ImportFieldsInMergeModeL(TBuf<50> aExistingPath,TBuf<50> aNewPath, TInt aStorageType,TUid aContFieldUid, TUid avCardMap);
	void ExternaliseFileL(CVersitParser& aParser,const TDesC& aFileName, RFs& fsSession);
	void InternalizeFileL(CVersitParser& aParser,const TDesC& aFileName, RFs& fsSession);
	void AddUidRevPropertyL(const CContactItem& aContactItem, RFs& fsSession, const TDesC& aSource, const TDesC& aDestination);
    CParserPropertyValueDateTime* CreateDateTimePropertyL(const TTime& aDateTime, TVersitDateTime::TRelativeTime aRelativeTime);
	
	static TInt iStorage[];
	static TUid iFieldUid[];
	static TUid iVcardUid[];
	};

_LIT(KTestImportContactMergeModeStep,"TestImportContactMergeModeStep");

#endif
