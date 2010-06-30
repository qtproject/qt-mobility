/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CONCURRENT_H__
#define __CONCURRENT_H__

//  Include Files
#include <e32base.h>
#include <cntdb.h>
#include <e32test.h>

//  Function Prototypes
GLDEF_C TInt E32Main();

// The various test steps.
enum TTestSteps { 
				ETestCreate, 
				ETestOpen, 
				ETestReplace, 
				ETestsEnd
				};


/**
* The class is used to encapsulate the main thread.
* This thread opens the session to the DB in thread shared mode, 
* and manipultes it by adding and deleting contacts.
* It also controls the child thread.
*/
class CMainThread : public CBase
	{
	public:
	
	static CMainThread* NewL();
	void ConstructL();
	~CMainThread();
	
	void TestCreateDatabaseL();
	void TestOpenDatabaseL();
	void TestReplaceDatabaseL();
	void CloseDatabaseSessionL();
	
	void TestAddContactL();
	void TestDeleteContactL();

	void LaunchChildThreadL();
	void ResumeChildThreadL();
	
	protected:
	
	RTest* iTest;
	RThread iChildThread;
	TContactItemId iContactId;
	};

/**
* The child thread class which works with the shared session of the database.
* This encapsulated the functionality for accessing the shared db session.
*/
class CChildThread : public CBase
	{
	public:
	
	static CChildThread * NewL();
	void ConstructL();
	~CChildThread();
	
	void AccessDBTestL();
	
	protected:
	
	RTest* iTest;
	};

#endif  // __CONCURRENT_H__


