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


#ifndef __T_PACKAGER_H__
#define __T_PACKAGER_H__

#include <badesca.h>
#include "t_cnttest.h"

namespace nsCntTest
	{	
	_LIT(KPl, "Pl");
	_LIT(KCnt,"Cnt");
	}

class CContactItem;
class CCntPackager;

// CPackagerTests - the class implementing the 
// Contact Packaging Tests. 
class CPackagerTests : public CCntTest
	{
public:
	static CPackagerTests* NewLC (CCntTestImplementor& aCntTestImpl);
	~CPackagerTests();

	void ConstructL();

	//Test procedures
	void AllTestsL();
	void ContactItemTestL();
	void EmptyContactItemTestL();
	void ContentTypeTestL();
	void CntTextDefTestL();
	void CntItemViewDefTestL();
	void CntViewDefTestL();	
	void DescArrayTestL();
	void MessageBufferTestL();
	void CContactIdArrayTestL();
	void CCntFilterTestL();
	TInt CompareContactItemTestL(CContactItem* item1, CContactItem* item2);
	TInt CompareContactItemFieldTestL(CContactItemField* itemField1, CContactItemField* itemField2);
	TInt CompareContentTypeTestL(const CContentType& contentType1, const CContentType& contentType2);
	TInt CompareCntTextDefL(CContactTextDef* textDef1, CContactTextDef* textDef2);
	TInt CompareCntItemViewDefTestL(CContactItemViewDef* viewDef1, CContactItemViewDef* viewDef2);
	TInt CompareCntViewDefTestL(CContactViewDef* def1, CContactViewDef* def2);	
	TInt CompareDescArrayTestL(CDesCArray* testArray1, CDesCArray* testArray2);
	TInt CompareCContactIdArrayTestL(CContactIdArray* idArray1, CContactIdArray* idArray2);
	TInt CompareCCntFilterTestL(CCntFilter* filter1, CCntFilter* filter2);
	

	virtual void HandleDatabaseEventL(TContactDbObserverEvent /*aEvent*/)
		{};
	virtual void DeleteNotifyL(TContactItemId /*aContactId*/)
		{};

private:
	inline CPackagerTests (CCntTestImplementor& aCntTestImpl);
	CContactItemViewDef* iMatchAll;
	RFs iFs;
	};

CPackagerTests::CPackagerTests (CCntTestImplementor& aCntTestImpl):CCntTest(aCntTestImpl) {};

	

#endif //__T_PACKAGER_H__
