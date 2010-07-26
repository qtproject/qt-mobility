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
* Tests the performance of ExportSelectedContactsL(with PBAP support) api
*
*/



 
#ifndef __TESTCONTACTSPBAPEXPORTPERFORMANCE_H__
#define __TESTCONTACTSPBAPEXPORTPERFORMANCE_H__

// System includes
#include <cntfilt.h>
#include <e32test.h>
#include <s32file.h>
#include <e32std.h>
#include <f32file.h>
#include <cntdb.h>
#include <vcard.h>
#include <cntitem.h>
#include <cntfldst.h>
#include <testexecutestepbase.h>

// User includes
#include "cntvcardutils.h"
#include "contactspbapexportutilityclass.h"
#include "testcontactspbapexport.h"
#include "testimpexvcardsuiteserver.h"

#define NOW(time) TTime time; time.HomeTime();

// Constants
const TInt64  KContactFilter = 0x3FFFFF;
const TReal KTolerance = 0.004;
const TInt KBufLength = 50;

class CTestContactsPBAPExportPerformance : public CTestStep
	{
public:
	CTestContactsPBAPExportPerformance();
	~CTestContactsPBAPExportPerformance();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	void TestPerformanceL(void);
	void ExportWithOldApiL(CContactDatabase* aDBase, CContactIdArray* aIdArray, RFileWriteStream& aWriteStream);
	void ExportWithPBAPVersionL(CContactDatabase* aDBase, const TDesC& aVersion, CContactIdArray* aIdArray, RFileWriteStream& aWriteStream);
	
private:
	CActiveScheduler* iScheduler;
	CContactsPBAPExportUtilityClass* iExportObj;
	
	};
	
_LIT(KTestContactsPBAPExportPerformance,	"TestContactsPBAPExportPerformance");
_LIT(KV21,									"2.1");
_LIT(KV30,									"3.0");
_LIT(KBaseTime,								"BaseTime");
_LIT(KExportvCardPerformanceOld,			"ExportvCardPerformanceOld");
_LIT(KOldApi,								"OldApi");

#endif
