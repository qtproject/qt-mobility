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


#ifndef __T_PERSISTENCELAYER_H__
#define __T_PERSISTENCELAYER_H__

#include "t_cnttest.h"




namespace nsCntTest
	{	
	_LIT(KPl, "Pl");
	_LIT(KCnt,"Cnt");
	}


// CPersistenceTests - the class implementing the 
// Contact Performance Tests. 
class CPersistenceTests : public CCntTest 
	{
public:
	static CPersistenceTests* NewLC (CCntTestImplementor& aCntTestImpl);
	~CPersistenceTests();

	void ConstructL();

	//Test procedures
	void FileCreationL();
	void AllTestsL(); 		
	void BasicCRUDL();  	
	void GroupsL();		
	void ViewDefsInclL();	
	void ViewDefsMaskL();
	void DuplicatedFieldUidsL();
	void AccessCountL();
	void TemplateAccessCountL();
	void OomTestL();
	void IccL();
	void SystemTemplateUpdates();


private:
	inline CPersistenceTests (CCntTestImplementor& aCntTestImpl);
	CContactItemViewDef* iMatchAll;
	};

CPersistenceTests::CPersistenceTests (CCntTestImplementor& aCntTestImpl):CCntTest(aCntTestImpl) {};

	

#endif //__T_PERSISTENCELAYER_H__
