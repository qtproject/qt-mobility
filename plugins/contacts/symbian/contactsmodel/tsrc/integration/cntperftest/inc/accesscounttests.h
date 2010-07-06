/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
 @publishedAll
 @released
*/

#ifndef __ACCESS_COUNT_TESTS_H__
#define __ACCESS_COUNT_TESTS_H__

#include "cntbasestep.h"

class CAccessCountTests : public CCntBaseStep
	{
public:
	CAccessCountTests(CCntPerfServer &aParent);
	virtual TVerdict doTestStepL();
	virtual void PreTestL();
private:
	void AccessCountTestL(const TContactItemId aCid);
	void ReverseAgentDeletionTestL(const TInt aNumContacts);
	void AddAgentFieldL(const TContactItemId aCid);
	void ForwardAgentDeletionTestL(const TInt aNumContacts);
	TContactItemId GetAgentFieldL(const TContactItemId aCid);
	void SetAgentFieldL(const TContactItemId aCid, const TContactItemId aVal);
	void AddGetAgentTestL(const TContactItemId aCid);
	void AgentAccessCountTestL();
	void AgentExportImportAccessTestL();
	void ExportVcardL(const CContactIdArray &aConids);
	void ImportVcardL();
	TInt GetAccessCountL(const TContactItemId aCid);
	};

#endif
