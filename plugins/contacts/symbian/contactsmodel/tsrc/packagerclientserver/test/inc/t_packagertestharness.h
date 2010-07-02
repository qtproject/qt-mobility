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



#ifndef __PACKAGER_TEST_HARNESS_H__
#define __PACKAGER_TEST_HARNESS_H__

#include <e32std.h>
#include <e32base.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntdb.h>
#include "persistencelayer.h"
#include "t_cnttestimplementor.h"
#include "t_packagertesthelper.h"
#include "t_packagertestclient.h"

enum TTestSteps
	{
	EAsynchTests = 0,
	ESynchTests
	};

class CPackagerTestHarness : CActive
{
public: 	
	static CPackagerTestHarness* NewL();
	static CPackagerTestHarness* NewLC();
	~CPackagerTestHarness();
	
	CPackagerCntFactory& FactoryL() const;
	CPackagerCntComparator& ComparatorL() const;	
	
	void SendCContactItemL();
	void SendCContentTypeL();
	void SendCContactTextDefL();
	void SendCContactItemViewDefL();
	void SendCContactViewDefL();
	void SendCContactIdArrayL();
	void SendCCntFilterL();
	void SendCViewContactL();
	void SendCDesCArrayL();
	
	void GetCContactItemL() const;
	void GetCContentTypeL() const;
	void GetCContactTextDefL() const;
	void GetCContactItemViewDefL() const;
	void GetCContactViewDefL() const;
	void GetCContactIdArrayL() const;
	void GetCCntFilterL() const;
	void GetCViewContactL() const;
	void GetCDesCArrayL() const;	
	
	void DoCancel();
	void RunL();
	void StartActiveObject();

private: 
	CPackagerTestHarness();
	void TestError() const;
	void ConstructL();
	void NextAsynchStepL();
	void NextSynchStepL();
	
private:
	RPackagerTestClient iClient;
	mutable CPackagerCntFactory* iFactory;
	mutable CPackagerCntComparator* iComparator;	
	
	CContactItem* iContactItem;
	CContentType* iContentType;
	CContactTextDef* iContactTextDef;
	CContactItemViewDef* iContactItemViewDef;
	CContactViewDef* iContactViewDef;
	CContactIdArray* iContactIdArray;
	CCntFilter* iContactFilter;
	CViewContact* iViewContact;
	CDesCArray* iDesCArray;	
	
	TInt iTestStep;
	TInt iNextStep;
}; 

#endif //__PACKAGER_SERVER_TEST_H__
