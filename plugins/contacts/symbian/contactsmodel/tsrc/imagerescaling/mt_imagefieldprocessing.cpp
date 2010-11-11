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

#include "t_upscalerhelper.h"

#define SETUP \
    RFs fs; \
    CleanupClosePushL(fs);\
    User::LeaveIfError(fs.Connect()); \
    RCntModel cntClient; \
    CleanupClosePushL(cntClient); \
    cntClient.ConnectL(); \
    cntClient.OpenDatabase(); \

#define TEAR_DOWN \
    CleanupStack::PopAndDestroy(2); \

LOCAL_C RTest test(_L("T_ImagerProcessing"));

LOCAL_C void TestCreateContactL()
    {
    test.Next(_L("TestCreateContactL"));

    SETUP;

    CContactItem* contact = CContactItem::NewLC(KUidContactCard);

    CContactItemField* newField = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldCodImage);
    newField->SetMapping(KUidContactFieldVCardMapUnknown);
    newField->TextStorage()->SetTextL(KSrcImage());
    contact->AddFieldL(*newField); // Takes ownership
    CleanupStack::Pop(newField);

    TContactItemId id = cntClient.CreateContactL(*contact);
    CleanupStack::PopAndDestroy(contact);

    // View definition to read image field
    CContactItemViewDef* imageViewDef = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields, CContactItemViewDef::EMaskHiddenFields);
    imageViewDef->AddL(KUidContactFieldCodImage);

    contact = cntClient.ReadContactL(imageViewDef ,id);
    CleanupStack::PopAndDestroy(imageViewDef);  //  imageViewDef

    TInt index = contact->CardFields().Find(KUidContactFieldCodImage, KUidContactFieldVCardMapUnknown);

    // Test image field found
    test(index != KErrNotFound);

    CContactItemField& field = contact->CardFields()[index];
    TPtrC imagePtr = field.TextStorage()->Text();

    // Image should exist
    test(BaflUtils::FileExists(fs, imagePtr));

    // Test for GUID
    TPtrC guid = contact->Guid();
    test(imagePtr.Find(guid));

    cntClient.CloseContact(id);
    delete contact;

    TEAR_DOWN;
    }

LOCAL_C void TestDeleteContactL()
    {
    test.Next(_L("TestDeleteContactL"));

    SETUP;

    CContactItem* contact = CContactItem::NewLC(KUidContactCard);

    CContactItemField* newField = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldCodImage);
    newField->SetMapping(KUidContactFieldVCardMapUnknown);
    newField->TextStorage()->SetTextL(KSrcImage());
    contact->AddFieldL(*newField); // Takes ownership
    CleanupStack::Pop(newField);

    TContactItemId id = cntClient.CreateContactL(*contact);
    CleanupStack::PopAndDestroy(contact);

    // View definition to read image field
    CContactItemViewDef* imageViewDef = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields, CContactItemViewDef::EMaskHiddenFields);
    imageViewDef->AddL(KUidContactFieldCodImage);

    contact = cntClient.ReadContactL(imageViewDef ,id);
    CleanupStack::PopAndDestroy(imageViewDef);  //  imageViewDef

    TInt index = contact->CardFields().Find(KUidContactFieldCodImage, KUidContactFieldVCardMapUnknown);

    // Test image field found
    test(index != KErrNotFound);

    CContactItemField& field = contact->CardFields()[index];
    TPtrC imagePtr = field.TextStorage()->Text();

    test(BaflUtils::FileExists(fs, imagePtr));

    cntClient.DeleteContactL(id, EDeferEvent, ETrue);

    test(!BaflUtils::FileExists(fs, imagePtr));

    cntClient.CloseContact(id);
    delete contact;

    TEAR_DOWN;
    }

LOCAL_C void TestUpdateContactL()
    {
    test.Next(_L("TestUpdateContactL"));

    SETUP;

    CContactItem* contact = CContactItem::NewLC(KUidContactCard);

    TContactItemId id = cntClient.CreateContactL(*contact);
    CleanupStack::PopAndDestroy(contact);

    // View definition to read image field
    CContactItemViewDef* imageViewDef = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields, CContactItemViewDef::EMaskHiddenFields);
    imageViewDef->AddL(KUidContactFieldMatchAll);

    contact = cntClient.OpenContactLX(imageViewDef ,id);
    CleanupStack::PushL(contact);

    CContactItemField* newField = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldCodImage);
    newField->SetMapping(KUidContactFieldVCardMapUnknown);
    newField->TextStorage()->SetTextL(KSrcImage());
    contact->AddFieldL(*newField); // Takes ownership
    CleanupStack::Pop(newField);

    cntClient.CommitContactL(*contact, EFalse);
    CleanupStack::PopAndDestroy(2);  //  contact, imageViewDef

    // View definition to read image field
    imageViewDef = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields, CContactItemViewDef::EMaskHiddenFields);
    imageViewDef->AddL(KUidContactFieldCodImage);

    contact = cntClient.ReadContactL(imageViewDef ,id);

    TInt index = contact->CardFields().Find(KUidContactFieldCodImage, KUidContactFieldVCardMapUnknown);

    // Test image field found
    test(index != KErrNotFound);

    CContactItemField& field = contact->CardFields()[index];
    TPtrC imagePtr = field.TextStorage()->Text();

    // Image should exist
    test(BaflUtils::FileExists(fs, imagePtr));

    // Test for GUID
    TPtrC guid = contact->Guid();
    test(imagePtr.Find(guid));

    cntClient.CloseContact(id);
    CleanupStack::PopAndDestroy(2);  //  contact, imageViewDef

    TEAR_DOWN;
    }

LOCAL_C void TestCreateContactWithoutImagesFoldeL()
    {
    test.Next(_L("TestCreateContactWithoutImagesFoldeL"));

    SETUP;

    // Delete the images folder and all contents
    TInt drive;

#ifdef __WINS__
    TInt err = DriveInfo::GetDefaultDrive(DriveInfo::EDefaultPhoneMemory, drive);
#else
    TInt err = DriveInfo::GetDefaultDrive(DriveInfo::EDefaultMassStorage, drive);
#endif

    // Do not leave with this error. The phone does not have to have this support
    if (err == KErrNone)
        {
        // Get the root path in this drive to create
        // to create the images directory
        TPath dir;
        User::LeaveIfError(PathInfo::GetRootPath(dir, drive));
        dir.Append(KImagesFolder);

        CFileMan* fileMan = CFileMan::NewL(fs);
        err = fileMan->RmDir(dir); // err not used
        delete fileMan;
        }
    else
        {
        test.Printf(_L("Could not remove the images folder\n"));
        return;
        }

    // Create an image and store an image without the images dir available
    CContactItem *contact = CContactItem::NewLC(KUidContactCard);

    CContactItemField *newField = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldCodImage);
    newField->SetMapping(KUidContactFieldVCardMapUnknown);
    newField->TextStorage()->SetTextL(KSrcImage());
    contact->AddFieldL(*newField); // Takes ownership
    CleanupStack::Pop(newField);

    TContactItemId id = cntClient.CreateContactL(*contact);
    CleanupStack::PopAndDestroy(contact);

    // View definition to read image field
    CContactItemViewDef *imageViewDef = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields, CContactItemViewDef::EMaskHiddenFields);
    imageViewDef->AddL(KUidContactFieldCodImage);

    contact = cntClient.ReadContactL(imageViewDef ,id);
    CleanupStack::PopAndDestroy(imageViewDef);  //  imageViewDef

    TInt index = contact->CardFields().Find(KUidContactFieldCodImage, KUidContactFieldVCardMapUnknown);

    // Test image field found
    test(index != KErrNotFound);

    CContactItemField& field = contact->CardFields()[index];
    TPtrC imagePtr = field.TextStorage()->Text();

    // Image path should not change
    test(imagePtr.Compare(KSrcImage()) == 0);

    cntClient.CloseContact(id);
    delete contact;

    TEAR_DOWN;
    }

LOCAL_C void GenerateTestImageL()
    {
    RFs fs;
    CleanupClosePushL(fs);
    User::LeaveIfError(fs.Connect());

    CTestImageCreator* scaler = CTestImageCreator::NewLC(fs);

    scaler->DecodeL();

    CleanupStack::PopAndDestroy(2);
    }

LOCAL_C void DoTestsL()
	{
    CleanupClosePushL(test);
	test.Start(_L("Test image field processing"));
	User::LeaveIfError(RFbsSession::Connect());

	// Generate a test image
	GenerateTestImageL();

	TestCreateContactL();
	TestUpdateContactL();
	TestDeleteContactL();

	// Test this as the last since it removes the images folder
	TestCreateContactWithoutImagesFoldeL();

    test.End();
    // Pause the test in eshell
    test.Getch();
    RFbsSession::Disconnect();
    CleanupStack::PopAndDestroy(); // test.Close
	}



GLDEF_C TInt E32Main()
	{
    // Init
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
  	if (!cleanupStack)
      return KErrNoMemory;

    CActiveScheduler* activeScheduler = new CActiveScheduler;
    if (!activeScheduler) {
        delete cleanupStack;
        return KErrNoMemory;
    }
    CActiveScheduler::Install(activeScheduler);

    // Run the tests
    TRAPD(err, DoTestsL());

    // Cleanup
    delete activeScheduler;
 	delete cleanupStack;
	activeScheduler = NULL;
	cleanupStack = NULL;

	return err;
    }
