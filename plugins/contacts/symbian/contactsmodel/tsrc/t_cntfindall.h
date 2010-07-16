/*
* Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
@SYMTestCaseID INC049017
@SYMTestType CT
@SYMTestPriority High
@SYMDEF INC049017
@SYMTestCaseDependencies cnttutil
@SYMTestCaseDesc Tests that Identity fields are searched when FindAsyncL and FindLC are used with KUidContactFieldMatchAll.
@SYMTestActions Create a contact database and add four contacts.
Search the database by using identity fields: GivenName, FamilyName and CompanyName.
Search for contacts by providing a name and using KUidContactFieldMatchAll.
Perform the search synchronously and asynchronously.
@SYMTestExpectedResults Contacts are found by given name, family name or company name when using KUidContactFieldMatchAll.
*/

//
//CFindTest
//

class CFindTest : public CBase, public MIdleFindObserver
	{
public:
	CFindTest();
	~CFindTest();
	void ConstructL();
	void DoTestsL();	
private: // From MIdleFindObserver.
	void IdleFindCallback();
private:
	void AddContactL(const TDesC& aFirstName,const TDesC& aLastName,const TDesC& aCompanyName,const TDesC& aWorkPhone,const TDesC& aHomePhone,const TDesC& aWorkMobile,const TDesC& aHomeMobile,const TDesC& aEmail);
	void AddContactCompactL(const TDesC& aFirstName,const TDesC& aLastName,const TDesC& aCompanyName,const TDesC& aWorkPhone,const TDesC& aHomePhone,const TDesC& aWorkMobile,const TDesC& aHomeMobile,const TDesC& aEmail);
	void FindPhoneNumberL(const TDesC& aTextToFind, const TInt aNumExpectedFinds,TBool aAsync=EFalse);
	void FindEmailAddressL(const TDesC& aTextToFind, const TInt aNumExpectedFinds,TBool aAsync=EFalse);
	void FindNameInFirstNameFieldL(const TDesC& aTextToFind, const TInt aNumExpectedFinds,TBool aAsync=EFalse);
	void FindNameInLastNameFieldL(const TDesC& aTextToFind, const TInt aNumExpectedFinds,TBool aAsync=EFalse);
	void FindInCompanyFieldL(const TDesC& aTextToFind, const TInt aNumExpectedFinds,TBool aAsync=EFalse);
	void FindInAllFieldsL(const TDesC& aTextToFind, const TInt aNumExpectedFinds,TBool aAsync=EFalse);
	void FindNotesL(const TDesC& aTextToFind, const TInt aNumExpectedFinds,TBool aAsync=EFalse);
	void DoFindL(const TDesC& aTextToFind,const CContactItemFieldDef& aFieldDef, const TInt aNumExpectedFinds);
	void DoFindAsyncL(const TDesC& aTextToFind,const CContactItemFieldDef& aFieldDef, const TInt aNumExpectedFinds);
private:
	RTest iTest;
	TFileName iLogFileName;
	TFileName iDbFileName;
	CLog* iLog;
	CContactDatabase* iDb;
	CIdleFinder* iIdleFinder;
	CContactTemplate*	iTemplate;
	};

