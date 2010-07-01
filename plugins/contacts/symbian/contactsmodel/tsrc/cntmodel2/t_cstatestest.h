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



#ifndef __T_STATE_TEST_H__
#define __T_STATE_TEST_H__

#include "nbcnttestlib.h"
#include <cntitem.h>
#include <cntdbobs.h>



class COpenCommitTest : CBase
{
public:
	static COpenCommitTest* NewLC();
	~COpenCommitTest();
	void RunOpenCommitTestL();
	void RunGroupTestL();
private:
	COpenCommitTest();
	void ConstructL();
	const CContactTemplate&  GetSysTemplateL();

private:
	CContactDatabase* iCntDb;
	CCntItemBuilder*  iCntItemBldr;
	CContactTemplate* iGoldenTemplate;
	RTest* iTest;	
};










#endif // __T_STATE_TEST_H__
