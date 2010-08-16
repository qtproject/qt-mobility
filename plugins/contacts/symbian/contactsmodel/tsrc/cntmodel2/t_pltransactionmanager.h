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



#ifndef __TPLTRANSACTIONMANAGER__
#define __TPLTRANSACTIONMANAGER__

#include "persistencelayer.h"
#include "nbcnttestlib.h"

namespace nsPlTransMgr 
{
	_LIT(KFilename, "C:TransMgr.cdb");
	const TInt KOnce 	   = 1;
	_LIT(KNumber,  "123456789");
	_LIT(KName,    "NAME");
	_LIT(KEmail,   "name@email.com");
	_LIT(KHomeAdd, "Home address");
	
}


class CPlTransMgrTest : public CBase
	{
public:	
	static CPlTransMgrTest* NewLC();
	~CPlTransMgrTest();
	void RunTestsL();

	virtual void HandleDatabaseEventL(TContactDbObserverEvent /*aEvent*/)
		{};
	virtual void DeleteNotifyL(TContactItemId /*aContactId*/)
		{};

private:
	void ConstructL();
	void CreateAndCommitTestL();
	void RollbackAndRecoverTestL();
	void OutOfMemoryAddTestL();
	void OutOfMemoryDeleteTestL();
	void OutOfMemoryUpdateTestL();	
	TBool IsContactInDbL(TInt aCntID);
	TInt AddContactL();
	void DeleteContactL(TInt aCntID); 
	void UpdateContactL(TInt aCntID);
	TInt AddAndCommitContactL();
	void UpdateAndCommitTestL();
	TBool CheckChangedFieldL(TInt aCntID);
	void DeleteAndCommitTestL();

private: // member variables
	CContactItemViewDef* 	iMatchAll;
	CPersistenceLayer* 		iPersistLayer;
	CCntItemBuilder*		iCntItemBldr;
	RFs iFs;
	TInt iTestNo;
	};
	
	
#endif //__TPLTRANSACTIONMANAGER__
