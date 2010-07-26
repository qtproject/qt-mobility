/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CCntModelTesterAsynch_H__
#define __CCntModelTesterAsynch_H__

#include <e32base.h>
#include <cntdef.h>
#include "t_utils2.h"

class CContactDatabase;
class CContactItemField;
class CCntModelTesterSetToneToContacts;
class CCntModelTesterAsynch;
	
// CLASS DECLARATION

//
// CTestConductor.
//

class CTestConductor : public CBase
	{
public:
	static CTestConductor* NewL();
	~CTestConductor();
	void SetTestError(TInt aTestError);

private:
	CTestConductor();
	void ConstructL();
	void RunTestsL();
private:
	RFs iFs;
	CLog* iLog;
	CContactDatabase* iDb;
	CRandomContactGenerator* iRandomGenerator;
	CCntModelTesterAsynch* iAsync;
	TInt iTestError;
	};

class CCntModelTesterAsynch : public CActive
    {
    friend class CTester;
    
    public:  // Interface		
        
        static CCntModelTesterAsynch* NewL(CLog& iLog);

		/**
         * Destructor.
         */
        ~CCntModelTesterAsynch();
        
        void IssueRequest();
        void SetSpeedDialL();
        void RemoveAndCreateFieldL();
        
    private: 
        CCntModelTesterAsynch(CLog& iLog);
        void ConstructL();
        void CreateContactsL();
        TContactItemId CreateContactL( CContactDatabase* aDb,
                                   const TDesC& aName, 
                                   const TDesC& aNumber);
        CContactItemField* CreateFieldLC(const TUid aMappingUId,
                                         TFieldType aFieldType, 
                                         const TDesC &aText);
    
    private:  // from CActive
	    void DoCancel();
        void RunL();
	    void Error(TInt aError);

    private:  // Data
		/// Ref. compression strategy
        CContactDatabase* iDb;
        CLog& iLog;
        CContactIdArray* iArray;
        CCntModelTesterSetToneToContacts* iOperation;
        CContactItem* iGroup;
    };
    
// Active object to Test Async open from two threads at once
class CTester : public CActive
	{
public:
	static CTester* NewL();
	~CTester();
	
private:
	enum TTest
		{
		EAsyncOpenNamed,
		EAsyncOpenedNamed=EAsyncOpenNamed+10,
		ESyncOpenNamed,
		ESyncOpenedNamed=ESyncOpenNamed+10,
		ENumTests
		};	


	CTester();
	void ConstructL();
	void NextTest();
	void RunL();      // From CActive.
	void DoCancel();  // From CActive.
	void Error(TInt aError);
private:
    CContactOpenOperation* iContactOpenOperation;
    TInt iCurrentTestNumber;
	CContactDatabase* iDb;
	};


// This class (CDbOpenLauncher) creates a thread to do async and sync opens 
class CDbOpenLauncher : public CBase	
	{
public:
	static void Create(const TDesC& aName, const TDesC& aViewName);

private:
	static void DoMainL(const TDesC& aName);
	static TInt LaunchThread(TAny* aAny);	
		
	class CViewer : public CBase
		{
	public: 
		static CViewer* NewL(const TDesC& aViewName);
		~CViewer();
	private: 
		void ConstructL(const TDesC& aViewName);
		
	private:
		CContactDatabase* iDatabase;
		CTester* tester;
		
	};
			
};   

#endif // __CCntModelTesterAsynch_H__

// End of File
