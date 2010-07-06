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
* This class are the concrete implementation of the right hand side of the
* the bridge pattern.
*
*/


#ifndef __T_PL_PERFORMANCE_H__
#define __T_PL_PERFORMANCE_H__

#include "nbcnttestlib/nbcnttestlib.h"
#include "t_cnttest.h"
#include "t_utils2.h"




namespace nsPlPerformance
{
	_LIT(KCreatePlPerform, "C:Contacts.cdb");
#if defined(__WINS__)
	_LIT(KPersistCSV, "C:\\PlPersist.csv");	
	_LIT(KContactCSV, "C:\\PlContact.csv");
	_LIT(KClSvrCSV,	  "C:\\PlClntSvr.csv");
#else
	_LIT(KPersistCSV, "C:\\PlPersist.csv");	
	_LIT(KContactCSV, "C:\\PlContact.csv");	
	_LIT(KClSvrCSV,	  "C:\\PlClntSvr.csv");
	
#endif
	_LIT8(KColumnHeader, "TestName,Num Contacts ,Time\r\n");
	_LIT(KNumber, "123456789");
	_LIT(KName, "NAME");
	_LIT(KEmail, "name@email.com");
	_LIT(KHomeAdd, "Home address");
	
	const TInt KFirstEntry   = 0;
	const TInt KSize1Sample  = 4;
	const TInt KSize2Sample  = 16;
	const TInt KSize3Sample  = 64;
/*	const TInt KSize4Sample  = 256;
	const TInt KSize5Sample  = 1024;
	const TInt KSize6Sample  = 4096;
*/
	const TInt KSize4Sample  = 64;
	const TInt KSize5Sample  = 64;
	const TInt KSize6Sample  = 64;

	const TInt KBatchSize	 = 64;
	
	TInt64 KSeed 		 	 = 10;
}


class CIdBuffer;
class CCsvWriter;

// CPlPerformanceAPI - the class implementing the 
// Contact Performance Tests. 
class CPlPerformanceAPI : public CCntTest 
	{
public:
	static CPlPerformanceAPI* NewLC (CCntTestImplementor& aCntTestImpl, const TDesC& aFilename);
	~CPlPerformanceAPI();

	void CreateContactsBatchL(TInt aSize);
	void UpdateContactsBatchL(const TInt aStartPos, const TInt aEndPos);
	void DeleteContactsBatchL(const TInt aStartPos, const TInt aEndPos);
	void ReadContactsTestL  (const TInt aStartPos, const TInt aEndPos);	
	void DeleteDefaultDatabaseTestL();
	void ListDatabaseTestL();
	
private:	
	void CreateContactsTestL(TInt aSize);
	void UpdateContactsTestL(const TInt aStartPos, const TInt aEndPos);
	void DeleteContactsTestL(const TInt aStartPos, const TInt aEndPos);
	
private: // methods
	void ConstructL      (const TDesC& aFilename);
	void CreateTemplateL ();
	TInt AddToIdBuffer   (TContactItemId aID);
	void CreateTestDataL ();
	void ShuffleContactIdsL();
private: // contsructors
	inline CPlPerformanceAPI(CCntTestImplementor& aCntTestImpl);

private: // member variables
	CContactItemViewDef* 	iMatchAll;
	CCntItemBuilder* 		iCntItemBldr;
	CIdBuffer* 				iIdBuffer;
	CCsvWriter*				iCsvWriter;
	bool					iShuffle;
	};



class CCsvWriter : CBase
	{
public:
	static CCsvWriter* NewLC(const TDesC& aFilename);
	~CCsvWriter();
	void WriteNextLineL(const TDesC8& aCsvLine);

private:
	void ConstructL();
	CCsvWriter() {};
	RFs iFs;
	RFile iSrcFile;
	};
	

class CIdBuffer
	{
public:
	CIdBuffer();
	void AddL(TInt aID);
	void RandomShuffleL();
	TInt Get(TInt& aIndex) const;
	
	TInt Count() const;
	inline ~CIdBuffer(){delete iContactIds;};
private:
	CArrayFix<TInt>* iContactIds; // Note Upper limit of 640 on RArray. 
	};


// Inline methods
inline CPlPerformanceAPI::CPlPerformanceAPI (CCntTestImplementor& aCntTestImpl):CCntTest(aCntTestImpl),iShuffle(true) {};



#endif //__T_PL_PERFORMANCE_H__
