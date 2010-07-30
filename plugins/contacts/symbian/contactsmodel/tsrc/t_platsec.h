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


class CPlatSecTest : public CBase, public MIdleFindObserver
	{
public:
	static CPlatSecTest* NewLC(RTest& aTest);
	~CPlatSecTest();
	void DoTestL();
private: // From MIdleFindObserver.
	CPlatSecTest(RTest& aTest);
	void ConstructL();
	void IdleFindCallback();
private:
	RFs iFs;
	RTest& iTest;
	TFileName iLogFileName;
	TFileName iDbFileName;
	CLog* iLog;
	CContactDatabase* iDb;
	RPIMTestServer iTestServer;
	};
