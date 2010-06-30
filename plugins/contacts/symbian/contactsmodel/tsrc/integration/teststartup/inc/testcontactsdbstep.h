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




#ifndef __TESTCONTACTSDBSTEP_H__
#define __TESTCONTACTSDBSTEP_H__

#include "teststartupserver.h"

class CTestContactsDbStep : public CTestStep
    {
public:     
    CTestContactsDbStep();
    ~CTestContactsDbStep();
	TVerdict doTestStepPreambleL();
    TVerdict doTestStepPostambleL();
	TVerdict doTestStepL();
	
private:
    CActiveScheduler*     iSched;
    CContactDatabase*     iDb;
    TPtrC                 iCntsDbAction; 
    TInt                  iNumberOfContacts;
    CContactItemFieldSet* iFields;
    
    void CreateDefaultCntsDb();
    void OpenDefaultCntsDb();
    void DeleteDefaultCntsDb();
    TBool CheckExistingViewDefsL();
    void AddContactsL(TInt aNumber);
    void ReplaceDefaultCntsDb();
    void SetManyFieldsL(CContactItemFieldSet &aFields, const TDesC& aText, const TInt aMany);
    void SetFieldL(CContactItemFieldSet &aFields, const TInt aPos, const TDesC& aText);
    };
    
#endif
