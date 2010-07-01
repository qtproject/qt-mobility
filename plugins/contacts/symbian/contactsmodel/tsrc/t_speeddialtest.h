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

		
#ifndef __T_SPEEDDIALTEST_H__
#define __T_SPEEDDIALTEST_H__

#include "t_utils2.h"

///////////////////////////////////////////////////////////////////////////////
// Class CLockServerTester Definition
///////////////////////////////////////////////////////////////////////////////

class CLockServerTester : public CBase
	{
public:
	static CLockServerTester* NewL();
	~CLockServerTester();	
private:
	CLockServerTester();
	void ConstructL();
public:
    void FindServerAndKillItL();
	void SetupProcessL();
	void DeleteProcess();
	void OpenAndKillProcess();
private:	
	struct
		{
		TFindProcess* iFind;
		RProcess* iExecution;
		} iProcess;
		
	TFullName iProcessFullName;
	
	};

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

TInt DeleteIniFile();
void AddContactsL(CRandomContactGenerator& aGenerator);
CContactDatabase* ReplaceDatabaseAndCreateContactsLC();
void CreateContactsL(CContactDatabase& aDb);
CContactDatabase* SetupLC();
void AssignSpeedDialL( TInt aContact, TInt aSpeedDialID, CContactDatabase& database );
void UpdateSpeedDialL( TInt aContact, TInt aSpeedDialID, CContactDatabase& database );
void RemoveSpeedDialWhenOpenL( TInt aContact, TInt aSpeedDialID, CContactDatabase& database );
void DoTestL();
TBool ContactIsASpeedDial(const CContactItem& aItem);

void TestDatabaseWithSpeedDialsL();
void TestOneL();
void TestTwoL();
void TestThreeL();
void TestFourL();

#endif
