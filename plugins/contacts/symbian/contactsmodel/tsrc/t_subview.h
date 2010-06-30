/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __T_SUBVIEW_H__
#define __T_SUBVIEW_H__

#include <e32test.h>

#include <cntdb.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <cntview.h>
#include "cntviewprivate.h"
#include "dbcreator.h"

_LIT(KDbName, "C:t_subview.cdb");
_LIT(KTestName, "t_subview");
const TInt KNumContacts(500);
_LIT(KLowChar, "f");
_LIT(KHighChar, "r");
_LIT(KLowBoundary, ">=f");
_LIT(KHighBoundary, "<s");

RTest test(KTestName);


class CSubViewTest : public CActive, public MContactViewObserver
	{
public:
	static CSubViewTest* NewLC();
	void HandleContactViewEvent(const CContactViewBase& aView, const TContactViewEvent& aEvent);
	void DoTestL();
	
private:
	void ConstructL();
	CSubViewTest();
	~CSubViewTest();
	void Cleanup();
	void RunL();
	void DoCancel();
	void CreateNamedLocalViewL();	
	void CreateSubViewL();	
	void ResortNamedLocalViewL();
	void PrintSubViewL() const;
	void PreResortTestL() const; 
	void PostResortTestL() const;
	void CheckSortOrderL() const;
	void SubAndLocalViewCompareTestL() const;

private:
	enum TSubViewTestState {ETestStarted,
							ENamedLocalViewCreated, 
							ESubViewCreated, 
							EResorting, 
							ENamedLocalViewResorted, 
							ESubViewUpdated,
							ESortingComplete
							};
private:
	CContactDatabase* iContactsDb;
	CContactNamedLocalView* iNamedLocalView;
	CContactSubView* iSubView;
	TInt iViewCount;
	TSubViewTestState iState;
	TInt iLnameIdx;
	TInt iFnameIdx;
	TInt iCnameIdx;
	TInt iNumIdx;
	TBuf<24> iNameOfFirstContact;
	TBuf<24> iNameOfLastContact;
	};



#endif //__T_SUBVIEW_H__
