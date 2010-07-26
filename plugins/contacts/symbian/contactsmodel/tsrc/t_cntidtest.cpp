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

#include <e32cons.h>
#include <cntdb.h>
#include <cntfield.h>
#include <cntitem.h>
#include <cntfldst.h>

#include "t_cntidtest.h"

// Local definitions
_LIT(KCntSysTemplateInfo,"Sys template has %d fields\n");
_LIT(KCntCreatingContacts,"Creating %d contacts\n");
_LIT(KCntCreatingContact,"Creating contact %d\n");
_LIT(KCntPrintContactIds,"Printing contact ids\n");
_LIT(KCntPrintContactId, "Contact ID: %d\n" );
_LIT(KCntPrintContactField, "Field number: %d ID: %d\n" );
_LIT(KDbFileName,"c:t_cntIdTest.cdb");
_LIT(KREADIDFAILED,"failed to verify the field ID\n");

GLREF_C RTest test;

const TInt KCntTestContactCount = 1;
const TInt KCntTestBufSize = 64;

//--------------------------------------------------------------------------
// CCntTest::CCntTest
//--------------------------------------------------------------------------
//
CCntTest::CCntTest()
:   CActive(EPriorityStandard)
    {
    CActiveScheduler::Add(this);
    }
    
//--------------------------------------------------------------------------
// CCntTest::~CCntTest
//--------------------------------------------------------------------------
//
CCntTest::~CCntTest()
    {
    Cancel();
    iContactIds.Close();
    delete iContactDb;
    }

//--------------------------------------------------------------------------
// CCntTest::NewL
//--------------------------------------------------------------------------
//
CCntTest* CCntTest::NewL()
    {
    CCntTest* self = new (ELeave) CCntTest();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

//--------------------------------------------------------------------------
// CCntTest::ConstructL
//--------------------------------------------------------------------------
//
void CCntTest::ConstructL()
    {
    iContactDb = CContactDatabase::ReplaceL(KDbFileName);
    }

//--------------------------------------------------------------------------
// CCntTest::RunTest
//--------------------------------------------------------------------------
//
void CCntTest::RunTest()
    {
    iPhase = ECntTestTemplate;
    IssueRequest();
    }

//--------------------------------------------------------------------------
// CCntTest::RunL
//--------------------------------------------------------------------------
//
void CCntTest::RunL()
    {
    switch( iPhase )
        {        
        case ECntTestTemplate:
            PrintTemplateInfoL();
            break;
            
        case ECntTestCreating:
            CreateContactsToDbL(KCntTestContactCount);
            break;
            
        case ECntTestPrint:
            ReadContactFieldIdsL();
            break;
            
        case ECntTestDone:
            CompleteL();
            break;
            
        default:
            break;
        }
    }
     
//--------------------------------------------------------------------------
// CCntTest::DoCancel
//--------------------------------------------------------------------------
//
void CCntTest::DoCancel()
    {       
    }         
       
//--------------------------------------------------------------------------
// CCntTest::CreateTemplateL
//--------------------------------------------------------------------------
//
void CCntTest::PrintTemplateInfoL()
    {
    CContactItem* sysTemplate = 
        iContactDb->OpenContactL(iContactDb->TemplateId());
    CleanupStack::PushL(sysTemplate);
        
    CContactItemFieldSet& fieldSet = sysTemplate->CardFields();

    test.Printf( KCntSysTemplateInfo, fieldSet.Count() );

    CleanupStack::PopAndDestroy(); // sysTemplate
    
    IssueRequest();
    iPhase = ECntTestCreating;
    }

//--------------------------------------------------------------------------
// CCntTest::CreateContactsToDbL
//--------------------------------------------------------------------------
//
void CCntTest::CreateContactsToDbL(TInt aCount)
    {
    test.Printf(KCntCreatingContacts, aCount);
    
    for (TInt i(0); i < aCount; ++i )
        {
        test.Printf(KCntCreatingContact, i );
        User::LeaveIfError( iContactIds.Append(DoCreateContactL(i)) );
        }    
    IssueRequest();
    iPhase = ECntTestPrint;
    }

//--------------------------------------------------------------------------
// CCntTest::DoCreateContactL
//--------------------------------------------------------------------------
//
TContactItemId CCntTest::DoCreateContactL( TInt aIdentifier )
    {
    TBuf<KCntTestBufSize> name(_L("Contact "));
    TBuf<KCntTestBufSize> number1(_L("1231231320") );
    TBuf<KCntTestBufSize> number2(_L("1231231321") );
    
    name.AppendNum(aIdentifier);
    number1.AppendNum(aIdentifier);
    number2.AppendNum(aIdentifier);
    
    CContactCard* card = CContactCard::NewLC();

    card->AddFieldL(*CreateFieldLC(KUidContactFieldVCardMapUnusedN,
                                   KUidContactFieldGivenName, 
                                   _L("ContactModel")));
    CleanupStack::Pop();
    card->AddFieldL(*CreateFieldLC(KUidContactFieldVCardMapUnusedN,
                                   KUidContactFieldFamilyName, 
                                   name ));
    CleanupStack::Pop();
    card->AddFieldL(*CreateFieldLC(KUidContactFieldVCardMapTEL,
                                   KUidContactFieldPhoneNumber, 
                                   number1 ));
    CleanupStack::Pop();
    card->AddFieldL(*CreateFieldLC(KUidContactFieldVCardMapTELFAX,
                                   KUidContactFieldPhoneNumber, 
                                   number2 ));
    CleanupStack::Pop();
    
    TContactItemId id = iContactDb->AddNewContactL(*card);
    CleanupStack::PopAndDestroy(card);    
    return id;    
    }

//--------------------------------------------------------------------------
// CCmKillerCntModel::CreateFieldLC
//--------------------------------------------------------------------------
//
CContactItemField* CCntTest::CreateFieldLC(
    const TUid aMappingUId,
    TFieldType aFieldType, 
    const TDesC &aText)
    {
    CContactItemField* field = 
        CContactItemField::NewLC(KStorageTypeText, aFieldType);        
    field->SetMapping(aMappingUId);
    // field->SetTemplateField(ETrue);
    field->TextStorage()->SetTextL(aText);
    return field;    
    }

//--------------------------------------------------------------------------
// CCntTest::PrintContactFieldIdsL
//--------------------------------------------------------------------------
//
void CCntTest::ReadContactFieldIdsL()
    {
    test.Printf(KCntPrintContactIds);
    
    TInt contactCount(iContactIds.Count());    
    for (TInt i(0); i < contactCount; ++i )
        {
        test.Printf(KCntPrintContactId, iContactIds[i] );
        
        CContactItem* item = iContactDb->ReadContactL(iContactIds[i]);
        CleanupStack::PushL(item);
        
        CContactItemFieldSet& fieldSet = item->CardFields();        
        TInt fieldCount( fieldSet.Count() );        
        for (TInt j(0); j < fieldCount; ++j )
            {
            TInt fieldId( fieldSet[j].Id() );
            test.Printf(KCntPrintContactField, j, fieldId );
            if(fieldId != j)
            	{
            	test.Printf(KREADIDFAILED);
                test(EFalse); // fail the test
            	}
            }
        iContactDb->CloseContactL( item->Id() );
        CleanupStack::PopAndDestroy(); // item
        }
    
    IssueRequest();
    iPhase = ECntTestDone;
    }

//--------------------------------------------------------------------------
// CCntTest::CompleteL
//--------------------------------------------------------------------------
//
void CCntTest::CompleteL()
    {
    CActiveScheduler::Stop();
    }
    
//--------------------------------------------------------------------------
// CCntTest::IssueRequest
//--------------------------------------------------------------------------
//
void CCntTest::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone );
    SetActive();
    }

