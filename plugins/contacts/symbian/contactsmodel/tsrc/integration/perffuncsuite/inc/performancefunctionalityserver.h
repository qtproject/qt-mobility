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

#ifndef __PERFORMANCEFUNCTIONALITY_SERVER_H__
#define __PERFORMANCEFUNCTIONALITY_SERVER_H__
#include <testexecuteserverbase.h>


class CPerformanceFunctionalityTestsSuite : public CTestServer
	{
public:
	static CPerformanceFunctionalityTestsSuite* NewL();
	~CPerformanceFunctionalityTestsSuite();
	// Base class pure virtual override
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	RFs& Fs();

private:
	RFs *iFs;
	};

inline RFs &CPerformanceFunctionalityTestsSuite::Fs()
	{
	return *iFs;
	}

#endif
