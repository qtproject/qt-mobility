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




#ifndef __TESTGETSETVIEWSETTINGSTEP_H__
#define __TESTGETSETVIEWSETTINGSTEP_H__

#include "teststartupserver.h"

class CTestGetSetViewSettingStep : public CTestStep
    {
public: 
    CTestGetSetViewSettingStep();
    ~CTestGetSetViewSettingStep();
	TVerdict doTestStepPreambleL();
    TVerdict doTestStepPostambleL();
	TVerdict doTestStepL();
	
private:      
    TPtrC iIniApiAction;
    TInt  iIniNumberOfViews;
    
    TBool TestStoredDefaultViewsL(RPointerArray<CContactDefaultViewDefinition>& aStoredViewSettings);
    void AppendToDefaultViewListL();
    void SetNewDefaultViewsL();
    void DeleteAViewDefinitionL();
    void OOMTestGetDefaultViewsL();
    void OOMTestSetDefaultViewsL();
    };
    
#endif
