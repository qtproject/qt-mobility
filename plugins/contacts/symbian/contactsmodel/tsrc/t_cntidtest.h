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


#ifndef CCNTTEST_H
#define CCNTTEST_H

#include <e32base.h>
#include <e32test.h>
#include <cntdef.h>

class CConsoleBase;
class CContactDatabase;
class CContactItemField;

class CCntTest : public CActive
    {
public: // Construction and destructor
    static CCntTest* NewL();
    ~CCntTest();

    void RunTest();

protected: // From CActive
    virtual void RunL();
    virtual void DoCancel();

private: // Implementation
    CCntTest();
    void ConstructL();
    void PrintTemplateInfoL();
    void CreateContactsToDbL(TInt aCount);
    TContactItemId DoCreateContactL(TInt aIdentifier);
    CContactItemField* CreateFieldLC(
            const TUid aMappingUId,
            TFieldType aFieldType, 
            const TDesC &aText);
    void ReadContactFieldIdsL();
    void CompleteL();
    void IssueRequest();


private:
    enum TCntTestPhase
        {
        ECntTestIdle,
        ECntTestTemplate,
        ECntTestCreating,
        ECntTestPrint,
        ECntTestDone
        };
        
private: // Data
    CContactDatabase* iContactDb;
    TCntTestPhase iPhase;
    RArray<TContactItemId> iContactIds;
    };

#endif // CCNTTEST_H
